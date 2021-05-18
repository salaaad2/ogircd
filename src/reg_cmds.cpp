#include "../inc/ftirc.hpp"
#include "../inc/Server.hpp"

void Server::passcmd(Message *msg, int fd)
{
    if (_fd_clients[fd].is_register == true)
        send_reply(fd, ERR_ALREADYREGISTERED);
    else if (!msg->params[0][0])
        send_reply(fd, ERR_NEEDMOREPARAMS);
    else
    {
        strcpy(_fd_clients[fd].password, msg->params[0]);
        send_reply(fd, RPL_NONE);
    }
}

void Server::nickcmd(Message *msg, int fd)
{
    // check against other usernicks
    // return :
    // ERR_NICKCOLLISION ERR_NICKNAMEINUSE ERR_ERRONEUSNICKNAME ERR_NICKNAMEINUSE
    if (_nick_clients.count(msg->params[0]) == 0) {
        if (_fd_clients[fd].nickname[0] != 0)
            _nick_clients.insert(_nick_clients.find(_fd_clients[fd].nickname),  std::pair<std::string, Client>(msg->params[0], _fd_clients[fd]));
        else
            _nick_clients.insert(std::pair<std::string, Client>(msg->params[0], _fd_clients[fd]));
        strcpy(_fd_clients[fd].nickname, msg->params[0]);
        send_reply(fd, RPL_NONE);
    }
    else {
        send_reply(fd, ERR_NICKNAMEINUSE);
    }
}

void Server::usercmd(Message *msg, int fd)
{
    std::cout << "1 nick : " << _fd_clients[fd].nickname << " pass : " << _fd_clients[fd].password << "\n";
    if (_fd_clients[fd].is_register == true)
        send_reply(fd, ERR_ALREADYREGISTERED);
    else if (!msg->params[0][0] || !msg->params[1][0] || !msg->params[2][0] || !msg->params[3][0])
        send_reply(fd, ERR_NEEDMOREPARAMS);
    else
    {
        strcpy(_fd_clients[fd].username, msg->params[0]);
        strcpy(_fd_clients[fd].realname, msg->params[3]);
        do_registration(fd);
    }
}

void Server::do_registration(int fd)
{
    std::cout << "2 nick : " << _fd_clients[fd].nickname << " pass : " << _fd_clients[fd].password << "\n";

    if (_fd_clients[fd].nickname[0] && !strcmp(_fd_clients[fd].password, _password))
    {
        std::cout << "registration 2\n";
        _fd_clients[fd].is_register = true;
        create_client_prefix(fd);
        send_reply(fd, RPL_WELCOME);
    }
}

void Server::create_client_prefix(int fd)
{
    strcat(_fd_clients[fd].prefix, _fd_clients[fd].nickname);
    strcat(_fd_clients[fd].prefix, "!");
    strcat(_fd_clients[fd].prefix, _fd_clients[fd].username);
    strcat(_fd_clients[fd].prefix, "@");
    strcat(_fd_clients[fd].prefix, _fd_clients[fd].host);
}
