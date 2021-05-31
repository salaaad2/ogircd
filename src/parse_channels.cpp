#include "../inc/Server.hpp"
#include "../inc/ftirc.hpp"


// void Server::canIjoin(std::string prefix, )

void Server::joincmd(Message *msg, Client *cl)
{
    std::vector<std::string> channels = parse_m_chans(msg->params);
    std::vector<std::string> keys = parse_keys(msg->params, channels);

    for (size_t i = 0 ; i < channels.size() ; i++)
        join2(channels[i], keys[i], cl);
    if (channels.empty() == false)
        cl->current_chan = channels.back();
    else
        send_reply("", cl, ERR_BADCHANMASK);
}


void Server::join2(std::string chan, std::string key, Client *cl)
{
    Message s;

    for (size_t i = 0; i < cl->chans.size(); i++) {
        if (chan == cl->chans[i])
        {
            send_reply("", cl, ERR_USERONCHANNEL);
            return;
        }
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
        else
        {
            _m_chans[chan].push_back(cl);
            cl->chans.push_back(chan);
            if (_m_uflags[chan].find(cl) == _m_uflags[chan].end())
                _m_uflags[chan][cl] = "";
        }
    }

    send_reply(chan, cl, RPL_TOPIC);
    send_reply(chan, cl, RPL_NAMREPLY);
    send_reply(chan, cl, RPL_ENDOFNAMES);
    s.params.push_back(cl->nickname);
    s.params.push_back (" joined channel ");
    s.params.push_back(chan);
    s.params.push_back("\r\n");
    send_reply_broad(cl, _m_chans[chan], -1, &s);
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

std::vector<std::string> Server::parse_m_chans(std::vector<std::string> & params)
{
    std::vector<std::string> channels;

    for (unsigned int i = 0 ; i < params.size() ; i++)
    {
        if (params[i] == "#" || params[i] == "&")
        {
            channels.push_back(params[i] + params[i + 1]);
            i += 2;
            if (i < params.size() && params[i] != ",")
                break ;
        }
    }
    return channels;
}

std::vector<std::string> Server::parse_keys(std::vector<std::string> params, std::vector<std::string> channels)
{
    std::vector<std::string> keys;
    size_t i = 0;

    while (i < params.size() && params[i][0] != ' ')
        i++;
    while (i < params.size())
    {
        if (params[i][0] != ' ')
        {
            keys.push_back(params[i]);
            i += 2;
            if (i < params.size() && params[i] != ",")
                break ;
        }
        i++;
    }
    while (keys.size() < channels.size())
        keys.push_back("");
    return keys;
}

bool Server::isbanned(Client *cl, std::string chan)
{
    for (std::vector<std::string>::iterator it = _m_banmask[chan].begin() ; it != _m_banmask[chan].end() ; it++)
    {
        if (strmatch(cl->prefix, *it))
        {
            for (std::vector<std::string>::iterator it2 = _m_exceptmask[chan].begin() ; it2 != _m_exceptmask[chan].end() ; it2++)
            {
                if (strmatch(cl->prefix, *it2))
                    return false;
            }
            return true;
        }
    }
    return false;
}
