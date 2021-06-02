#include "../inc/Server.hpp"
#include "../inc/ftirc.hpp"


// void Server::canIjoin(std::string prefix, )

void Server::joincmd(Message *msg, Client *cl)
{
    if (msg->params.size() < 1)
    {
        send_reply("", cl, ERR_NEEDMOREPARAMS);
        return ;
    }

    std::vector<std::string> channels = parse_m_chans(msg->params[0]);
    std::vector<std::string> keys;
    
    if (msg->params.size() >= 2)
        keys = parse_keys(msg->params[1], channels);
    else
        keys = std::vector<std::string>(channels.size(), "");
    for (size_t i = 0 ; i < channels.size() ; i++)
        join2(channels[i], keys[i], cl);
    if (channels.empty() == false)
        cl->current_chan = channels.back();
    else
        send_reply("", cl, ERR_BADCHANMASK);
}


void Server::join2(std::string & chan, std::string & key, Client *cl)
{
    Message s;

    for (size_t i = 0; i < cl->chans.size(); i++) {
        if (chan == cl->chans[i])
        {
            send_reply("", cl, ERR_USERONCHANNEL);
            return;
        }
    }
    if (chan[0] != '#' && chan[0] != '&')
    {
        send_reply("", cl, ERR_NOSUCHCHANNEL);
        return ;
    }
    if (_m_chans.find(chan) == _m_chans.end())
        new_channel(chan, cl);
    else
    {
        if (_m_flags[chan].find('k') != std::string::npos && key != _m_chankey[chan])
        {
            send_reply(chan, cl, ERR_BADCHANNELKEY);
            return ;
        }
        else if (isbanned(cl, chan))
        {
            send_reply(chan, cl, ERR_BANNEDFROMCHAN);
            return ;
        }
        else if (_m_flags[chan].find('l') != std::string::npos && (_m_chans.size() >= _m_limits[chan]))
        {
            send_reply(chan, cl, ERR_CHANNELISFULL);
            return ;
        }
        else if (_m_flags[chan].find('i') != std::string::npos && !isinvited(cl->nickname, chan))
        {
            send_reply(chan, cl, ERR_INVITEONLYCHAN);
            return ;
        }
        else
        {
            _m_chans[chan].push_back(cl);
            cl->chans.push_back(chan);
            if (_m_uflags[chan].find(cl) == _m_uflags[chan].end())
                _m_uflags[chan][cl] = "";
        }
    }
    send_to_channel(":" + cl->prefix + " JOIN :" + chan + "\r\n", chan, NULL);
    send_reply(chan, cl, RPL_TOPIC);
    send_reply(chan, cl, RPL_NAMREPLY);
    send_reply(chan, cl, RPL_ENDOFNAMES);
    s.params.push_back(cl->nickname);
    s.params.push_back (" joined channel ");
    s.params.push_back(chan);
    s.params.push_back("\r\n");
    // send_reply_broad(cl, _m_chans[chan], -1, &s);
}

	/* CHANNELS INFOS */
	/* CHANNEL MODE : [opsitnbv] */
	/* USER MODE : [iwso] */

void Server::new_channel(std::string chan, Client *cl)
{
    _m_chans[chan].push_back(cl);
    cl->chans.push_back(chan);
    _m_topics[chan] = "Welcome to the channel you chose";
    _m_flags[chan] = "";
    _m_uflags[chan][cl] = "o";
}

std::vector<std::string> Server::parse_m_chans(std::string & chan)
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

std::vector<std::string> Server::parse_keys(std::string & key, std::vector<std::string> & channels)
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

bool Server::isbanned(Client *cl, std::string & chan)
{
    for (std::vector<std::string>::iterator it = _m_banmask[chan].begin() ;
         it != _m_banmask[chan].end() ;
         it++)
    {
        if (strmatch(cl->prefix, *it))
        {
            for (std::vector<std::string>::iterator it2 = _m_exceptmask[chan].begin() ;
                 it2 != _m_exceptmask[chan].end() ;
                 it2++)
            {
                if (strmatch(cl->prefix, *it2))
                    return false;
            }
            return true;
        }
    }
    return false;
}

bool Server::isinvited(std::string & nickname, std::string & chan)
{
    for (std::vector<std::string>::iterator it = _m_invite[chan].begin() ;
         it != _m_invite[chan].end() ;
         it++)
    {
        if (*it == nickname)
            return true;
    }
    return false;
}
