#include "../inc/Server.hpp"
#include "../inc/ftirc.hpp"


// void Server::canIjoin(std::string prefix, )

void Server::joincmd(Message *msg, std::string prefix)
{
    if (msg->params.size() < 1)
    {
        send_reply("", prefix, ERR_NEEDMOREPARAMS);
        return ;
    }

    std::vector<std::string> channels = parse_m_chans(msg->params[0]);
    std::vector<std::string> keys;
    
    if (msg->params.size() >= 2)
        keys = parse_keys(msg->params[1], channels);
    else
        keys = std::vector<std::string>(channels.size(), "");
    for (size_t i = 0 ; i < channels.size() ; i++)
        join2(channels[i], keys[i], prefix);
    if (channels.empty() == false)
        _m_pclients[prefix]->current_chan = channels.back();
    else
        send_reply("", prefix, ERR_BADCHANMASK);
}


void Server::join2(std::string chan, std::string key, std::string prefix)
{
    Message s;

    for (size_t i = 0; i < _m_pclients[prefix]->chans.size(); i++) {
        if (chan == _m_pclients[prefix]->chans[i])
        {
            send_reply("", prefix, ERR_USERONCHANNEL);
            return;
        }
    }
    if (chan[0] != '#' && chan[0] != '&')
    {
        send_reply("", prefix, ERR_NOSUCHCHANNEL);
        return ;
    }
    if (_m_chans.find(chan) == _m_chans.end())
        new_channel(chan, prefix);
    else
    {
        if (_m_flags[chan].find('k') != std::string::npos && key != _m_chankey[chan])
        {
            send_reply(chan, prefix, ERR_BADCHANNELKEY);
            return ;
        }
        else if (isbanned(prefix, chan))
        {
            send_reply(chan, prefix, ERR_BANNEDFROMCHAN);
            return ;
        }
        else if (_m_flags[chan].find('l') != std::string::npos && (_m_chans.size() >= _m_limits[chan]))
        {
            send_reply(chan, prefix, ERR_CHANNELISFULL);
            return ;
        }
        else if (_m_flags[chan].find('i') != std::string::npos && !isinvited(_m_pclients[prefix]->nickname, chan))
        {
            send_reply(chan, prefix, ERR_INVITEONLYCHAN);
            return ;
        }
        else
        {
            _m_chans[chan].push_back(_m_pclients[prefix]);
            _m_pclients[prefix]->chans.push_back(chan);
            if (_m_uflags[chan].find(_m_pclients[prefix]) == _m_uflags[chan].end())
                _m_uflags[chan][_m_pclients[prefix]] = "";
        }
    }

    send_reply(chan, prefix, RPL_TOPIC);
    send_reply(chan, prefix, RPL_NAMREPLY);
    send_reply(chan, prefix, RPL_ENDOFNAMES);
    s.params.push_back(_m_pclients[prefix]->nickname);
    s.params.push_back (" joined channel ");
    s.params.push_back(chan);
    s.params.push_back("\r\n");
    send_reply_broad(prefix, _m_chans[chan], -1, &s);
}

	/* CHANNELS INFOS */
	/* CHANNEL MODE : [opsitnbv] */
	/* USER MODE : [iwso] */

void Server::new_channel(std::string chan, std::string & prefix)
{
    _m_chans[chan].push_back(_m_pclients[prefix]);
   _m_pclients[prefix]->chans.push_back(chan);
    _m_topics[chan] = "Welcome to the channel you chose";
    _m_flags[chan] = "";
    _m_uflags[chan][_m_pclients[prefix]] = "Oo";
}

std::vector<std::string> Server::parse_m_chans(std::string chan)
{
    std::vector<std::string> channels;
    std::string c_chan;
    size_t i = 0;

    while (chan[i])
    {
        if (chan[i] != ',')
            c_chan.push_back(chan[i]);
        else
        {
            channels.push_back(c_chan);
            c_chan.clear();
        }
        i++;
    }
    channels.push_back(c_chan);
    return channels;
}

std::vector<std::string> Server::parse_keys(std::string key, std::vector<std::string> channels)
{
    std::vector<std::string> keys;
    std::string c_keys;
    size_t i = 0;

    while (key[i])
    {
        if (key[i] != ',')
            c_keys.push_back(key[i]);
        else
        {
            keys.push_back(c_keys);
            c_keys.clear();
        }
        i++;
    }
    keys.push_back(c_keys);
    while (keys.size() < channels.size())
        keys.push_back("");
    return keys;
}

bool Server::isbanned(std::string prefix, std::string chan)
{
    for (std::vector<std::string>::iterator it = _m_banmask[chan].begin() ; it != _m_banmask[chan].end() ; it++)
    {
        if (strmatch(prefix, *it))
        {
            for (std::vector<std::string>::iterator it2 = _m_exceptmask[chan].begin() ; it2 != _m_exceptmask[chan].end() ; it2++)
            {
                if (strmatch(prefix, *it2))
                    return false;
            }
            return true;
        }
    }
    return false;
}

bool Server::isinvited(std::string nickname, std::string chan)
{
    for (std::vector<std::string>::iterator it = _m_invite[chan].begin() ; it != _m_invite[chan].end() ; it++)
    {
        if (*it == nickname)
            return true;
    }
    return false;
}
