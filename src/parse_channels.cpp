#include "../inc/Server.hpp"
#include "../inc/ftirc.hpp"

void Server::joincmd(Message *msg, std::string prefix)
{
    std::vector<std::string> channels = parse_channels(msg->params);

    for (size_t i = 0 ; i < channels.size() ; i++)
        join2(channels[i], prefix);
    if (channels.empty() == false)
    {
        _prefix_clients[prefix].current_chan = channels.back();
    }
    else
    {
        send_reply("", _prefix_clients[prefix].clfd, ERR_BADCHANMASK);
    }
}

void Server::join2(std::string chan, std::string prefix)
{
    Message s;
    int fd = _prefix_clients[prefix].clfd;

    for (size_t i = 0; i < _fd_clients[fd].chans.size(); i++) {
        if (chan == _fd_clients[fd].chans[i])
        {
            send_reply("", fd, ERR_USERONCHANNEL);
            return;
        }
    }
    if (_channels.find(chan) == _channels.end())
        new_channel(chan, prefix);
    else
    {
        _channels[chan].push_back(_prefix_clients[prefix]);
        _prefix_clients[prefix].chans.push_back(chan);
        if (_u_modes[chan].find(_prefix_clients[prefix]) == _u_modes[chan].end())
            _u_modes[chan][_prefix_clients[prefix]] = "----";
    }
    send_reply(chan, fd, RPL_TOPIC);
    send_reply(chan, fd, RPL_NAMREPLY);
    send_reply(chan, fd, RPL_ENDOFNAMES);
    s.params.push_back(_prefix_clients[prefix].nickname);
    s.params.push_back (" joined channel ");
    s.params.push_back(chan);
    s.params.push_back("\r\n");
    send_reply_broad(prefix, _channels[chan], -1, &s);
}


	/* CHANNELS INFOS */
	/* CHANNEL MODE : [opsitnbv] */
	/* USER MODE : [iwso] */

void Server::new_channel(std::string chan, std::string prefix)
{
    _channels[chan].push_back(_prefix_clients[prefix]);
   _prefix_clients[prefix].chans.push_back(chan);
    _topics[chan] = "Welcome to the channel you chose";
    _modes[chan] = "o-------";
    _u_modes[chan][_prefix_clients[prefix]] = "---o";
}

std::vector<std::string> Server::parse_channels(std::vector<std::string> params)
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
