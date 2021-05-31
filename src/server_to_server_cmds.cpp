#include "../inc/Server.hpp"

void Server::broadcast_known_servers(int fd)
{
    typedef std::map<int, network*>::iterator iterator;

    std::string req;

    std::cout << "BROADCAST KNOWN SERVERS TO SOCKET " << fd << "\n";
    for (iterator it = _m_fdserver.begin(); it != _m_fdserver.end(); it++) {
        if ((*it).second->token != fd)
        {
            req = "SERVER " + (*it).second->servername + " " + ft_utoa((*it).second->hopcount) + " " + ft_utoa((*it).second->token) + "\r\n";
            send(fd, req.c_str(), strlen(req.c_str()), 0);
        }
    }
}

void Server::broadcast_known_users(int fd)
{
    typedef std::map<std::string, Client*>::iterator iterator;

    std::string req;

    std::cout << "BROADCAST KNOWN USERS TO SOCKET " << fd << "\n";
    for (iterator it = _m_pclients.begin(); it != _m_pclients.end(); it++) {
        req = "PASS :" + _password + "\r\n";
        send(fd, req.c_str(), strlen(req.c_str()), 0);
        req = "NICK :" + (*it).second->nickname + "\r\n";
        std::cout << "_m_pclients.size()" << _m_pclients.size() << "send nick : " << (*it).second->nickname << std::endl;

        send(fd, req.c_str(), strlen(req.c_str()), 0);
        req = "USER " + (*it).second->username + " " + (*it).second->host + " " +
            (*it).second->servername + ":" + (*it).second->realname + "\r\n";
        send(fd, req.c_str(), strlen(req.c_str()), 0);
    }
}

// void Server::broadcast_known_channels(int fd)
// {
//     typedef std::map<std::string, std::vector<Client*>>::iterator m_iterator;
//     typedef std::vector<Client*>::iterator v_iterator;
//     std::string req;
//     for (m_iterator m_it = _m_chans.begin(); m_it != _m_chans.end(); m_it++)
//     {
//         for (v_iterator v_it = (*m_it).second.begin(); v_it != (*m_it).second.end(); v_it++)
//         {
//             req = 0
//         }
//     }
// }

void Server::servercmd(Message *msg, std::string prefix, int fd) // <servername> <hopcount> <token> <info>
{
    std::string servername;
    std::string hopcount;
    std::string token;
    std::string req;
    Message tmp;
    network *net;

    (void)prefix;

    if (msg->params.size() == 3) {
        std::cout << "size 3 " << std::endl;
        servername = msg->params[0];
        hopcount = msg->params[1];
        token = msg->params[2];
    }
    else if (msg->params.size() == 5){
        std::cout << "size 5 " << std::endl;
        servername = msg->params[0];
        hopcount = msg->params[2];
        token = msg->params[4];
    }
    else
    {
        req = msg_rpl("", ERR_NEEDMOREPARAMS, "");
        send(fd, req.c_str(), strlen(req.c_str()), 0);
        return;
    }
    if (_m_fdserver.count(fd) == 1)
    {
        std::cout << "serv already exists " << std::endl;
        req = msg_rpl(servername, ERR_ALREADYREGISTERED, "");
        send(fd, req.c_str(), strlen(req.c_str()), 0);
        return;
    }
    else {
        std::cout << "new server" << std::endl;
        net = new network;
        net->servername = servername;
        net->hopcount = ft_atoi(hopcount.c_str()) + 1;
        net->token = fd;
        _m_fdserver[fd] = net;
        req = "PASS :" + _peer_password + "\r\n";
        send(fd, req.c_str(), req.size(), 0);
        req = "SERVER " + _servername + " 0 1\r\n";
        send(fd, req.c_str(), req.size(), 0);
        broadcast_known_servers(fd);
        // broadcast_known_users(fd);
        req = "INFO\r\n";
        send(fd, req.c_str(), req.size(), 0);
        // broadcast_known_channels(fd);
        // std::cout << "net->servername " << net->servername << "\n";
        // std::cout << "net->hopcount " << net->hopcount << "\n";
        // std::cout << "net->token " << net->token << "\n";
    }
}

void Server::createParams(Message *msg)
{
    std::vector<std::string> vec;

    vec.push_back(msg->params[0] + ":" + msg->params[2] + ":" + _password);
    vec.push_back(ft_utoa(_port));
    vec.push_back(_password);
    _pm = new Params(vec.size() + 1, vec);
}

void Server::connectcmd(Message *msg, std::string & prefix) //TODO : check priv
{
    std::vector<std::string> vec;
    int net_socket;

    if (msg->params.size() < 3)
        send_reply("", prefix, ERR_NEEDMOREPARAMS);
    else
    {
        createParams(msg);
        if ((net_socket = connect_serv()) != -1)
        {
            Message msg;
            msg.params.push_back(_pm->getHost());
            msg.params.push_back(ft_utoa(0));
            msg.params.push_back(ft_utoa(net_socket));
            servercmd(&msg, prefix, net_socket);

        }
        delete _pm;
    }
}
