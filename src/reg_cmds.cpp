#include "../inc/ftirc.hpp"
#include "../inc/Server.hpp"

void Server::passcmd(Message *msg, int fd)
{
    if (_fd_clients[fd].is_register == true)
        send_reply("", fd, ERR_ALREADYREGISTERED);
    else if (!msg->params[0][0])
        send_reply("", fd, ERR_NEEDMOREPARAMS);
    else if (strncmp(msg->params[0].c_str(), _password, strlen(_password)))
        send_reply("", fd, ERR_PASSWDMISMATCH);
    else
        _fd_clients[fd].password =  msg->params[0];
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
        _fd_clients[fd].nickname = msg->params[0];
        if (_fd_clients[fd].nickname.size() > 9)
            _fd_clients[fd].nickname.resize(9);
    }
    else {
        send_reply("", fd, ERR_NICKNAMEINUSE);
    }
}

void Server::usercmd(Message *msg, int fd)
{
    if (_fd_clients[fd].is_register == true)
        send_reply("", fd, ERR_ALREADYREGISTERED);
    else if (msg->params.size() == 8)
    {
        send_reply("", fd, ERR_NEEDMOREPARAMS);
    }
    else
    {
        _fd_clients[fd].username =  msg->params[0].c_str();
        _fd_clients[fd].realname =  msg->params[msg->params.size() - 1];
        do_registration(fd);
    }
}

void Server::do_registration(int fd)
{
    if (_fd_clients[fd].nickname[0] && !_fd_clients[fd].password.compare(_password))
    {
        _fd_clients[fd].is_register = true;
        create_client_prefix(fd);
        send_reply("", fd, RPL_WELCOME);
    }
}

void Server::create_client_prefix(int fd)
{
    _fd_clients[fd].prefix += _fd_clients[fd].nickname;
    _fd_clients[fd].prefix +=  "!";
    _fd_clients[fd].prefix += _fd_clients[fd].username;
    _fd_clients[fd].prefix +=  "@";
    _fd_clients[fd].prefix +=  _fd_clients[fd].host;
}
