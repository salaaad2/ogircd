#include "../inc/ftirc.hpp"
#include "../inc/Server.hpp"

void Server::passcmd(Message *msg, int fd)
{
    std::cout << "PASSSSS\n";
    if (_m_pclients["temp_prefix@"]->is_register == true)
    {
        std::cout << "PASSS 1\n";
        send_reply("", _m_fdprefix[fd], ERR_ALREADYREGISTERED);
    }
    else if (!msg->params[0][0])
    {
        std::cout << "PASSS 2\n";
        send_reply("", _m_fdprefix[fd], ERR_NEEDMOREPARAMS);
    }
    else if (strncmp(msg->params[0].c_str(), _password, strlen(_password)))
    {
        std::cout << "PASSS 3\n";
        send_reply("", _m_fdprefix[fd], ERR_PASSWDMISMATCH);
    }
    else
    {
        std::cout << "PASSS 4\n";
        _m_pclients["temp_prefix@"]->password =  msg->params[0];
    }
}

void Server::nickcmd(Message *msg, int fd)
{
    // check against other usernicks
    // return :
    // ERR_NICKCOLLISION ERR_NICKNAMEINUSE ERR_ERRONEUSNICKNAME ERR_NICKNAMEINUSE
    std::cout << "NICCKKKKK\n";
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
    _m_pclients["temp_prefix@"]->nickname = msg->params[0];
}

void Server::usercmd(Message *msg, int fd)
{
    if (_m_pclients["temp_prefix@"]->is_register == true)
        send_reply("", _m_fdprefix[fd], ERR_ALREADYREGISTERED);
    else if (msg->params.size() == 8)
    {
        send_reply("", _m_fdprefix[fd], ERR_NEEDMOREPARAMS);
    }
    else
    {
        _m_pclients["temp_prefix@"]->username =  msg->params[0].c_str();
        _m_pclients["temp_prefix@"]->realname =  msg->params[msg->params.size() - 1];
        do_registration(fd);
    }
}

void Server::do_registration(int fd)
{
    if (_m_pclients["temp_prefix@"]->nickname[0] &&
       !_m_pclients["temp_prefix@"]->password.compare(_password))
    {
        create_client_prefix(fd);
        _m_pclients[_m_fdprefix[fd]]->is_register = true;
        _m_pclients[_m_fdprefix[fd]]->is_logged = true;
        std::cout << "fd : " << _m_pclients[_m_fdprefix[fd]]->clfd << "is reg : " << _m_pclients[_m_fdprefix[fd]]->is_register << std::endl;


        send_reply("", _m_fdprefix[fd], RPL_WELCOME);
        _m_pclients.erase("temp_prefix@");
    }
}

void Server::create_client_prefix(int fd)
{
    std::string s;

    s += _m_pclients["temp_prefix@"]->nickname;
    s +=  "!";
    s += _m_pclients["temp_prefix@"]->username;
    s +=  "@";
    s +=  _m_pclients["temp_prefix@"]->host;
    if (_m_pclients.count(s) == 0)
        _m_pclients[s] = _m_pclients["temp_prefix@"];
    else
    {
        _m_pclients[s]->is_logged = true;
       _m_pclients[s]->clfd = _m_pclients["temp_prefix@"]->clfd;
    }
    _m_pclients[s]->prefix = s;
    _m_fdprefix[fd] = s;
    _m_nickdb[_m_pclients[s]->nickname].push(_m_pclients[s]);
}
