#include "../inc/ftirc.hpp"
#include "../inc/Server.hpp"

void Server::passcmd(Message *msg, int fd)
{
    if (_m_fdclients[fd].is_register == true)
        send_reply("", fd, ERR_ALREADYREGISTERED);
    else if (!msg->params[0][0])
        send_reply("", fd, ERR_NEEDMOREPARAMS);
    else if (strncmp(msg->params[0].c_str(), _password, strlen(_password)))
        send_reply("", fd, ERR_PASSWDMISMATCH);
    else
        _m_fdclients[fd].password =  msg->params[0];
}

void Server::nickcmd(Message *msg, int fd)
{
    // check against other usernicks
    // return :
    // ERR_NICKCOLLISION ERR_NICKNAMEINUSE ERR_ERRONEUSNICKNAME ERR_NICKNAMEINUSE
    if (msg->params[0].size() > 9)
        msg->params[0].resize(9);
    if (_m_nickdb.count(msg->params[0]) == 1)
    {
        if (_m_nickdb[msg->params[0]].top().is_logged == true)
        {
            send_reply(msg->params[0], fd, ERR_NICKNAMEINUSE);
            return;
        }
    }
    _m_fdclients[fd].nickname = msg->params[0];
    _m_nickdb[msg->params[0]].push(_m_fdclients[fd]);
    _m_nickdb[msg->params[0]].top().is_logged = true;
}

void Server::usercmd(Message *msg, int fd)
{
    if (_m_fdclients[fd].is_register == true)
        send_reply("", fd, ERR_ALREADYREGISTERED);
    else if (msg->params.size() == 8)
    {
        send_reply("", fd, ERR_NEEDMOREPARAMS);
    }
    else
    {
        _m_fdclients[fd].username =  msg->params[0].c_str();
        _m_fdclients[fd].realname =  msg->params[msg->params.size() - 1];
        do_registration(fd);
    }
}

void Server::do_registration(int fd)
{
    if (_m_fdclients[fd].nickname[0] && !_m_fdclients[fd].password.compare(_password))
    {
        create_client_prefix(fd);
        _m_fdclients[fd].is_register = true;
        _m_fdclients[fd].is_logged = true;
        send_reply("", fd, RPL_WELCOME);
    }
}

void Server::create_client_prefix(int fd)
{
    _m_fdclients[fd].prefix += _m_fdclients[fd].nickname;
    _m_fdclients[fd].prefix +=  "!";
    _m_fdclients[fd].prefix += _m_fdclients[fd].username;
    _m_fdclients[fd].prefix +=  "@";
    _m_fdclients[fd].prefix +=  _m_fdclients[fd].host;
    if (_m_pclients.count(_m_fdclients[fd].prefix) == 0)
        _m_pclients[_m_fdclients[fd].prefix] = _m_fdclients[fd];
    else
        _m_fdclients[fd] = _m_pclients[_m_fdclients[fd].prefix];
    _m_pclients[_m_fdclients[fd].prefix].clfd = fd;
}
