#include "../inc/Server.hpp"
#include "../inc/ftirc.hpp"

void Server::joincmd(Message *msg, std::string prefix)
{
    std::cout << "CLIENT JOIN FD :" << _m_pclients[prefix]->clfd << "\n";
    std::vector<std::string> channels = parse_m_chans(msg->params);
    std::cout << "joincmd\n";
    for (size_t i = 0 ; i < channels.size() ; i++)
        join2(channels[i], prefix);
    if (channels.empty() == false)
    {
        _m_pclients[prefix]->current_chan = channels.back();
    }
    else
    {
        send_reply("", prefix, ERR_BADCHANMASK);
    }
}

void Server::join2(std::string chan, std::string prefix)
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
        _m_chans[chan].push_back(_m_pclients[prefix]);
        _m_pclients[prefix]->chans.push_back(chan);
        if (_m_uflags[chan].find(_m_pclients[prefix]) == _m_uflags[chan].end())
            _m_uflags[chan][_m_pclients[prefix]] = "";
    }
    std::cout << "joined\n";
    std::cout << "nickname : " << _m_pclients[prefix]->username << "\n";
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
            channels.push_back(params[i + 1]);
            i += 2;
            if (i < params.size() && params[i] != ",")
                break ;
        }
    }
    return channels;
}
