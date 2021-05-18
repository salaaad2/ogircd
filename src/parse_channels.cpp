#include "../inc/Server.hpp"
#include "../inc/ftirc.hpp"

void Server::joincmd(Message *msg, int fd)
{
    std::vector<std::string> channels = parse_channels(msg->params[0].c_str());

    for (size_t i = 0 ; i < channels.size() ; i++)
        join2(channels[i], fd);
}

void Server::join2(std::string chan, int fd)
{
    std::string s;

    for (size_t i = 0; i < _fd_clients[fd].chans.size(); i++) {
        if (chan == _fd_clients[fd].chans[i])
        {
            send_reply(fd, ERR_USERONCHANNEL);
            return;
        }
    }
    // if (_channels.find(chan) == _channels.end())
    //     new_channel(chan, fd);
    _channels[chan].push_back(_fd_clients[fd]);
    _fd_clients[fd].chans.push_back(chan);
    //_topics[chan] = "Welcome to the channel you chose";

    send_reply(fd, RPL_TOPIC, chan);
    send_reply(fd, RPL_NAMREPLY, chan);
    send_reply(fd, RPL_ENDOFNAMES, chan);
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

std::vector<std::string> Server::parse_channels(const char params[])
{
    size_t count = 0;
    std::vector<std::string> channels;

    for (unsigned int i = 0 ; params[i] ; i++)
    {
        if (params[i] == ',')
        {
            channels.push_back(std::string(params + count, params + i));
            count = i + 1;
        }
    }
    channels.push_back(std::string(params + count));
    return channels;
}