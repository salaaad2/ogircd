#include "../inc/Server.hpp"
#include "../inc/ftirc.hpp"

void Server::joincmd(Message *msg, int fd)
{
    std::vector<std::string> channels = parse_channels(msg->params);

    for (size_t i = 0 ; i < channels.size() ; i++)
        join2(channels[i], fd);
    _fd_clients[fd].current_chan = channels.back();
}

void Server::join2(std::string chan, int fd)
{
    std::string s;

    for (size_t i = 0; i < _fd_clients[fd].chans.size(); i++) {
        if (chan == _fd_clients[fd].chans[i])
        {
            send_reply("", fd, ERR_USERONCHANNEL);
            return;
        }
    }
    if (_channels.find(chan) == _channels.end())
        new_channel(chan, fd);
    else
    {
        _channels[chan].push_back(_fd_clients[fd]);
        _fd_clients[fd].chans.push_back(chan);
    }
    send_reply(chan, fd, RPL_TOPIC);
    send_reply(chan, fd, RPL_NAMREPLY);
    send_reply(chan, fd, RPL_ENDOFNAMES);
    s += _fd_clients[fd].nickname;
    s += " joined channel ";
    s += chan;
    s += "\r\n";
    send_reply_broad(_fd_clients[fd], _channels[chan], -1, s.c_str());
}

void Server::new_channel(std::string chan, int fd)
{
    _channels[chan].push_back(_fd_clients[fd]);
    _fd_clients[fd].chans.push_back(chan);
    _topics[chan] = "Welcome to the channel you chose";
}

std::vector<std::string> Server::parse_channels(std::vector<std::string> params)
{
    size_t count = 0;
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
