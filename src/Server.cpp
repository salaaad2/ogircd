/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbajrami <tbajrami@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/13 14:12:23 by tbajrami          #+#    #+#             */
/*   Updated: 2021/05/27 15:13:46 by tbajrami         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"

void Server::setFds(Fds *fds) {_fds = fds;}


Server::Server(Params *pm)
{
    time(&_launch_time);
    _network = new std::vector<sockaddr_in>;
    if (pm->isnew())
        new_serv(pm);
    else
        connect_serv(pm);
}

//=====================CREATION AND CONNECTION OF THE SERVER============================

void Server::new_serv(Params *pm)
{
    int yes = 1;
    getIP();

    if((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        std::cout << SOCKET_ERROR << std::endl;
        exit(1);
    }
    if(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        std::cout << SETSOCK_ERROR << std::endl;;
        exit(1);
    }
    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = INADDR_ANY;
    _addr.sin_port = htons(pm->getPort());
    strcpy(_password, pm->getPwd());
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
}

void Server::connect_serv(Params *pm)
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
    _addr.sin_port = htons(pm->getPort());
    strcpy(_password, pm->getPwd());
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
    do_connect(pm);
}

void Server::do_connect(Params *pm)
{
    int net_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(pm->getPortNetwork());
    server_address.sin_addr.s_addr = INADDR_ANY;
    this->_network->push_back(server_address);

    int connection_status = connect(net_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    if (connection_status == -1)
        std::cout << "Error making connection to the remote socket\n\n";
    send(net_socket, "SERVER", 6, 0);
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
    std::string tmp(msg->command);
    if (tmp == "PASS") {
        passcmd(msg, fd);
    }
    else if (tmp == "SERVER")
    {
        if (_m_pclients[_m_fdprefix[fd]]->password != _password)
            send_reply("", _m_fdprefix[fd], ERR_PASSWDMISMATCH);
        else if (_m_pclients[_m_fdprefix[fd]]->is_register == true)
            send_reply("", _m_fdprefix[fd], ERR_ALREADYREGISTERED);
        else
            servercmd(msg, fd);
    }
    else if (tmp == "NICK")
    {
        if (_m_pclients[_m_fdprefix[fd]]->password != _password)
            send_reply("", _m_fdprefix[fd], ERR_PASSWDMISMATCH);
        else
            nickcmd(msg, fd);
    }
    else if (tmp == "USER" ) {
        if (_m_pclients[_m_fdprefix[fd]]->password != _password)
            send_reply("", _m_fdprefix[fd], ERR_PASSWDMISMATCH);
        else if (_m_pclients[_m_fdprefix[fd]]->nickname[0] == 0)
            send_reply("", _m_fdprefix[fd], ERR_NONICKNAMEGIVEN);
        else
            usercmd(msg, fd);
    }
    else if (_m_pclients.count(_m_fdprefix[fd]) && _m_pclients[_m_fdprefix[fd]]->is_register == true)
    {
        if (tmp == "JOIN")
            joincmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);
        else if (tmp == "NAMES")
            namescmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);
        else if (tmp == "LIST")
            listcmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);
        else if (tmp == "MODE")
            modecmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);
        else if (tmp == "PRIVMSG")
            privmsgcmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);
        else if (tmp == "NOTICE")
            noticecmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);
        else if (tmp == "QUIT")
            quitcmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);
        else if (tmp == "VERSION")
            versioncmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);
        else if (tmp == "STATS")
            statscmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);
        else if (tmp == "TIME")
            timecmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);
        else if (tmp == "INFO")
            infocmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);
        else if (tmp == "WHO")
            whocmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);
        else if (tmp == "CONNECT")
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
