#include "../inc/Server.hpp"

void Server::servercmd(Message *msg, int fd)
{
    (void)msg;
    std::string s = ft_utoa(fd);
    std::string host = _m_pclients[s]->host;
    _m_fdserver[fd] = host;
    std::cout << "New connection from server " << host << " on fd " << fd << "\n";
    send(fd, "PASS :CC\r\n", strlen("PASS: CC\r\n"), 0);
    send(fd, "SERVER", strlen("SERVER"), 0);
}

void Server::connectcmd(Message *msg, std::string prefix) //TODO : check priv
{
    std::vector<std::string> vec;
    vec.push_back(msg->params[0] + ":" + msg->params[2] + ":" + _password);
    vec.push_back(ft_utoa(_port));
    vec.push_back(_password);
    Params *pm = new Params(vec.size() + 1, vec);
    (void)prefix;
    _pm = pm;
    connect_serv();
}
