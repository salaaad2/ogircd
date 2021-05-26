#include "../inc/Server.hpp"

void Server::servercmd(Message *msg, std::string prefix, int fd) // <servername> <hopcount> <token> <info>
{
    network *net;
    std::cout << "msg->params.size() " << msg->params.size() << "\n";
    if (msg->params.size() == 3)
    {
        if (_m_fdserver.count(fd) == 1)
            send_reply("", prefix, ERR_ALREADYREGISTERED);
        else
        {
            std::cout << "PAIRING IN PROGRESS\n";
            std::string req;
            net = new network;
            net->servername = msg->params[0];
            net->hopcount = ft_atoi(msg->params[1].c_str());
            net->token = ft_atoi(msg->params[2].c_str());
            _m_fdserver[fd] = net;
            req = "PASS :abc\r\n";
            send(fd, req.c_str(), req.size(), 0);
            req = "SERVER 42@irc 1 1\r\n";
            send(fd, req.c_str(), req.size(), 0);
        }
    }
    else if (msg->params.size() == 5)
    {

        if (_m_fdserver.count(fd) == 1)
            send_reply("", prefix, ERR_ALREADYREGISTERED);
        else
        {
            std::cout << "PAIRING IN PROGRESS\n";
            std::string req;
            net = new network;
            net->servername = msg->params[0];
            net->hopcount = ft_atoi(msg->params[2].c_str());
            net->token = ft_atoi(msg->params[4].c_str());
            _m_fdserver[fd] = net;
            req = "PASS :abc\r\n";
            send(fd, req.c_str(), req.size(), 0);
            req = "SERVER 42@irc 1 1\r\n";
            send(fd, req.c_str(), req.size(), 0);
        }
    }
    else
    {
        std::cout << "PAIRING DONE\n";
    }
}

void Server::connectcmd(Message *msg, std::string & prefix) //TODO : check priv
{
    std::vector<std::string> vec;
    int net_socket;
    if (msg->params.size() < 3)
        send_reply("", prefix, ERR_NEEDMOREPARAMS);
    else
    {
        vec.push_back(msg->params[0] + ":" + msg->params[2] + ":" + _password);
        vec.push_back(ft_utoa(_port));
        vec.push_back(_password);
        _pm = new Params(vec.size() + 1, vec);
        if ((net_socket = connect_serv()) != -1)
        {
            Message msg;
            msg.params.push_back(_pm->getHost());
            msg.params.push_back(ft_utoa(1));
            msg.params.push_back(ft_utoa(net_socket));
            servercmd(&msg, prefix, net_socket);
        }
        delete _pm;
    }
}
