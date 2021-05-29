#include "../inc/ftirc.hpp"
#include "../inc/Server.hpp"

void Server::passcmd(Message *msg, int fd)
{
    std::string s = ft_utoa(fd);
    if (_m_pclients[s]->is_register == true)
        send_reply("", _m_fdprefix[fd], ERR_ALREADYREGISTERED);
    else if (!msg->params[0][0])
        send_reply("", _m_fdprefix[fd], ERR_NEEDMOREPARAMS);
    else if (msg->params[0] != _password)
        send_reply("", _m_fdprefix[fd], ERR_PASSWDMISMATCH);
    else
        _m_pclients[s]->password =  msg->params[0];
}


void Server::nickcmd(Message *msg, int fd)
{
    // check against other usernicks
    // return :
    // ERR_NICKCOLLISION ERR_NICKNAMEINUSE ERR_ERRONEUSNICKNAME ERR_NICKNAMEINUSE
    std::string s = ft_utoa(fd);
    if (msg->params[0].size() > 9)
        msg->params[0].resize(9);
    if (_m_nickdb.count(msg->params[0]) == 1)
    {
        if (_m_nickdb[msg->params[0]].top()->is_logged == true)
         {
            send_reply(msg->params[0], _m_fdprefix[fd], ERR_NICKNAMEINUSE);
            return;
        }
    }
    _m_pclients[s]->nickname = msg->params[0];
}

void Server::usercmd(Message *msg, int fd)
{
    std::string s = ft_utoa(fd);
    if (_m_pclients[s]->is_register == true)
        send_reply("", _m_fdprefix[fd], ERR_ALREADYREGISTERED);
    else if (msg->params.size() == 8)
    {
        send_reply("", _m_fdprefix[fd], ERR_NEEDMOREPARAMS);
    }
    else
    {
        _m_pclients[s]->username =  msg->params[0].c_str();
        _m_pclients[s]->realname =  msg->params[msg->params.size() - 1];
        do_registration(fd);
    }
}

void Server::do_registration(int fd)
{
    std::string s = ft_utoa(fd);
    if (_m_pclients[s]->nickname[0] &&
       !_m_pclients[s]->password.compare(_password))
    {
        create_client_prefix(fd);
        _m_pclients[_m_fdprefix[fd]]->is_register = true;
        _m_pclients[_m_fdprefix[fd]]->is_logged = true;
        send_reply("", _m_fdprefix[fd], RPL_WELCOME);
        _m_pclients.erase(s);
    }
}

void Server::create_client_prefix(int fd)
{
    std::string prefix;
    std::string s = ft_utoa(fd);

    prefix += _m_pclients[s]->nickname;
    prefix +=  "!";
    prefix += _m_pclients[s]->username;
    prefix +=  "@";
    prefix +=  _m_pclients[s]->host;
    if (_m_pclients.count(prefix) == 0)
        _m_pclients[prefix] = _m_pclients[s];
    else
    {
        _m_pclients[prefix]->is_logged = true;
       _m_pclients[prefix]->clfd = _m_pclients[s]->clfd;
    }
    _m_pclients[prefix]->prefix = prefix;
    _m_fdprefix[fd] = prefix;
    _m_nickdb[_m_pclients[prefix]->nickname].push(_m_pclients[prefix]);
}
