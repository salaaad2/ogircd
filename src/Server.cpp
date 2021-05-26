/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbajrami <tbajrami@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/13 14:12:23 by tbajrami          #+#    #+#             */
/*   Updated: 2021/05/20 15:52:36 by tbajrami         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"

#include <netdb.h>

void Server::setFds(Fds *fds) {_fds = fds;}


Server::Server(Params *pm)
{
    time(&_launch_time);
    _pm = pm;
    if (pm->isnew())
        new_serv();
    else
        connect_serv();
}

//=====================CREATION AND CONNECTION OF THE SERVER============================

void Server::new_serv()
{
    int yes = 1;
    getIP();

    if((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        std::cerr << SOCKET_ERROR << std::endl;
        exit(1);
    }
    if(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        std::cout << SETSOCK_ERROR << std::endl;;
        exit(1);
    }
    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = INADDR_ANY;
    _addr.sin_port = htons(_pm->getPort());
    _password = _pm->getPwd();
    ft_bzero(&(_addr.sin_zero), 8);
    if(bind(listener, (struct sockaddr *)&_addr, sizeof(_addr)) == -1)
    {
        perror(BIND_ERROR);
        exit(1);
    }
    if(listen(listener, 10) == -1)
    {
        perror(LISTEN_ERROR);
        exit(1);
    }
    if (!_pm->getHost().empty() && _pm->getPortNetwork() && !_pm->getPwdNetwork().empty())
    {
        std::cout << "conn_serv()" << std::endl;
        connect_serv();
    }
    else
    {
        std::cout << "delete params" << std::endl;
        delete _pm;
    }
}

void Server::connect_serv()
{
    int yes = 1;
    if((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        std::cout << SOCKET_ERROR << std::endl;
        exit(1);
    }
    if(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        std::cout << SETSOCK_ERROR << std::endl;
        exit(1);
    }
    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = INADDR_ANY;
    _addr.sin_port = htons(_pm->getPort());
    _password = _pm->getPwd();
    ft_bzero(&(_addr.sin_zero), 8);
    if(bind(listener, (struct sockaddr *)&_addr, sizeof(_addr)) == -1)
    {
        perror(BIND_ERROR);
        exit(1);
    }
    if(listen(listener, 10) == -1)
    {
        perror(LISTEN_ERROR);
        exit(1);
    }
    do_connect();
}

// TODO : while res; res = res->ai_next
// inet_ntop (res->ai_family, ptr, addrstr, 100);
void Server::do_connect()
{
    struct addrinfo hints, *res, *result;
    int errcode, status, net_socket;
    char addrstr[100];
    void *ptr;

    memset(&hints, 0, sizeof (hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags |= AI_CANONNAME;

    errcode = getaddrinfo(_pm->getHost().c_str(), NULL, &hints, &result);
    if (errcode != 0)
    {
        std::cerr << "Error: getaddrinfo on \'" << _pm->getHost() << "\' failed\n\n";
        return ;
    }
    res = result;
    inet_ntop(res->ai_family, res->ai_addr->sa_data, addrstr, 100);
    if (res->ai_family == AF_INET) {
        ptr = &((struct sockaddr_in *) res->ai_addr)->sin_addr;
        net_socket = socket(AF_INET, SOCK_STREAM, 0);
    } else if (res->ai_family == AF_INET6) {
        ptr = &((struct sockaddr_in6 *) res->ai_addr)->sin6_addr;
        net_socket = socket(AF_INET6, SOCK_STREAM, 0);
    } else {
        std::cerr << "Error: wrong hostname\n\n";
        return ;
    }
    status = connect(net_socket, (struct sockaddr *)ptr, sizeof(struct sockaddr_in));
    if (status != 0)
    {
        std::cerr << "Error: connection to the remote socket failed\n\n";
        return;
    }
    send(net_socket, "SERVER\r\n", 6, 0);
    delete _pm;
}

//========================================================================================



int Server::addclient(int listener)
{
    Client *nc = new Client();
    std::string s;
    if((nc->clfd = accept(listener, nc->clientaddr, &nc->addrlen)) == -1)
    {
        std::cout << "Server-accept() error\n";
        return (-1);
    }
    else
        std::cout << "Server-accept() is OK...\n";
    std::cout << "New connection from " << inet_ntoa(((struct sockaddr_in*)nc->clientaddr)->sin_addr);
    nc->host = inet_ntoa(((struct sockaddr_in*)nc->clientaddr)->sin_addr);
    std::cout << " on socket " << nc->clfd << std::endl;
    s = ft_utoa(nc->clfd);
    _m_pclients[s] = nc;
    _m_fdprefix[nc->clfd] = s;
    return (nc->clfd);
}

void Server::getIP()
{
    const char* google_dns_server = "8.8.8.8";
    int dns_port = 53;
    struct sockaddr_in serv;
    int sock = socket ( AF_INET, SOCK_DGRAM, 0);

    memset( &serv, 0, sizeof(serv) );
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr( google_dns_server );
    serv.sin_port = htons( dns_port );
    int err = connect( sock , (const struct sockaddr*) &serv , sizeof(serv) );
    struct sockaddr_in name;
    socklen_t namelen = sizeof(name);
    err = getsockname(sock, (struct sockaddr*) &name, &namelen);
    const char* p = inet_ntoa(name.sin_addr);
    if(p != NULL)
    {
        std::cout << "Local ip is : " << p << "\n";
    }
    strcpy(_ip, p);
}




//==============================TREAT COMMANDS======================================

void Server::send_reply(std::string s, std::string prefix, int code)
{
    std::string ccmd;
    if (code)
        ccmd = ft_format_cmd(ft_utoa(code));
    std::string to_send;
    _prefix = BOLDWHITE;
    _prefix += "[" + ft_current_time();
    _prefix.erase(_prefix.size() - 1, 1);
    _prefix += + "]:";

    to_send += (_prefix +  " " + ccmd + " " + msg_rpl(s, code, prefix) + RESET + "\r\n");
    send(_m_pclients[prefix]->clfd, to_send.c_str(), strlen(to_send.c_str()), 0);
}

void Server::send_reply_broad(std::string prefix, std::vector<Client*> cl, int code, Message *msg)
{
    std::string s;
    for (size_t i = 0; i < cl.size(); i++)
    {
        if (cl[i]->clfd != _m_pclients[prefix]->clfd)
        {
            if (code != -1)
                send_reply("", cl[i]->prefix, code);
            else
            {
                for(size_t i = 0; i < msg->params.size(); i++)
                    s += msg->params[i];
                send_reply(s, cl[i]->prefix, 0);
            }
        }
    }
}

void Server::chan_msg(Message * msg, std::string prefix) {
    std::string nick = _m_pclients[prefix]->nickname;
    std::string s;

    s += ("<" + _m_nickdb[nick].top()->nickname + ">@["+ _m_pclients[prefix]->current_chan + "] : " += msg->command + " ");
    msg->params.insert(msg->params.begin(), s);
    send_reply_broad(prefix, _m_chans[_m_pclients[prefix]->current_chan], -1, msg);
}

void Server::do_command(Message *msg, int fd)
{
    if (msg->command == "PASS") {
        passcmd(msg, fd);
    }
    else if (msg->command == "SERVER")
    {
        if (_m_pclients[_m_fdprefix[fd]]->password != _password)
            send_reply("", _m_fdprefix[fd], ERR_PASSWDMISMATCH);
        else if (_m_pclients[_m_fdprefix[fd]]->is_register == true)
            send_reply("", _m_fdprefix[fd], ERR_ALREADYREGISTERED);
        else
            servercmd(msg, fd);
    }
    else if (msg->command == "NICK")
    {
        if (_m_pclients[_m_fdprefix[fd]]->password != _password)
            send_reply("", _m_fdprefix[fd], ERR_PASSWDMISMATCH);
        else
            nickcmd(msg, fd);
    }
    else if (msg->command == "USER" ) {
        if (_m_pclients[_m_fdprefix[fd]]->password != _password)
            send_reply("", _m_fdprefix[fd], ERR_PASSWDMISMATCH);
        else if (_m_pclients[_m_fdprefix[fd]]->nickname[0] == 0)
            send_reply("", _m_fdprefix[fd], ERR_NONICKNAMEGIVEN);
        else
            usercmd(msg, fd);
    }
    else if (_m_pclients.count(_m_fdprefix[fd]) && _m_pclients[_m_fdprefix[fd]]->is_register == true)
    {
        if (msg->command == "JOIN")
            joincmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);
        else if (msg->command == "PRIVMSG")
            privmsgcmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);
        else if (msg->command == "NOTICE")
            noticecmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);
        else if (msg->command == "QUIT")
            quitcmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);
        else if (msg->command == "VERSION")
            versioncmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);
        else if (msg->command == "STATS")
            statscmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);
        else if (msg->command == "TIME")
            timecmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);
        else if (msg->command == "INFO")
            infocmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);
        else if (msg->command == "WHO")
            whocmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);
        else if (msg->command == "CONNECT")
            connectcmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);
        else if (_m_pclients[_m_fdprefix[fd]]->current_chan.empty() == false)
            chan_msg(msg, _m_pclients[_m_fdprefix[fd]]->prefix); // TODO: cadegage
        else
            send_reply(msg->command, _m_fdprefix[fd], ERR_NOTOCHANNEL);
    }
    else
        send_reply("", _m_fdprefix[fd], ERR_NOTREGISTERED);
    delete msg;
}




//===============================================================================
