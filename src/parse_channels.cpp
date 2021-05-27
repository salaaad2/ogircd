#include "../inc/Server.hpp"
#include "../inc/ftirc.hpp"

// void Server::canIjoin(std::string prefix, )

void Server::joincmd(Message *msg, std::string prefix)
{
    std::vector<std::string> channels = parse_m_chans(msg->params);
    std::vector<std::string> keys = parse_keys(msg->params, channels);

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
    if (_m_chans.find(chan) == _m_chans.end())
        new_channel(chan, prefix);
    else
    {
        std::cout << "flags : " << _m_flags[chan] << " pass : " << _m_chankey[chan] << "\n";
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

void Server::new_channel(std::string chan, std::string prefix)
{
    _m_chans[chan].push_back(_m_pclients[prefix]);
   _m_pclients[prefix]->chans.push_back(chan);
    _m_topics[chan] = "Welcome to the channel you chose";
    _m_flags[chan] = "";
    _m_uflags[chan][_m_pclients[prefix]] = "o";
}

std::vector<std::string> Server::parse_m_chans(std::vector<std::string> params)
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

bool Server::isbanned(std::string prefix, std::string chan)
{
    for (std::vector<std::string>::iterator it = _m_banmask[chan].begin() ; it != _m_banmask[chan].end() ; it++)
    {
        std::cout << "prefix : " << prefix << " mask : " << *it << "\n";
        if (strmatch(prefix, *it))
            return true;
    }
    return false;
}
