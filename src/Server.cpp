/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbajrami <tbajrami@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/13 14:12:23 by tbajrami          #+#    #+#             */
/*   Updated: 2021/05/19 16:34:10 by tbajrami         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"

void Server::setFds(Fds *fds) {_fds = fds;}



Server::Server(Params *pm)
{
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
    Client nc;

    nc.is_server = false;
    nc.is_register = false;
    if((nc.clfd = accept(listener, (struct sockaddr *)&nc.clientaddr, &nc.addrlen)) == -1)
    {
        std::cout << "Server-accept() error\n";
        return (-1);
    }
    else
        std::cout << "Server-accept() is OK...\n";
    std::cout << "New connection from " << inet_ntoa(nc.clientaddr.sin_addr);
    nc.host = inet_ntoa(nc.clientaddr.sin_addr);
    std::cout << " on socket " << nc.clfd << std::endl;
    _fd_clients[nc.clfd] = nc;
    return (nc.clfd);
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
    _prefix[0] = ':';
    strcat(_prefix, _ip);
}




//==============================TREAT COMMANDS======================================

void Server::send_reply(std::string s, int fd, int code)
{
    std::string ccmd;
    if (code)
        ccmd = ft_format_cmd(ft_utoa(code));
    std::string to_send;

    to_send += (std::string(_prefix) + " " + ccmd + " " + msg_rpl(s, code, fd) + "\r\n");
    send(fd, to_send.c_str(), strlen(to_send.c_str()), 0);
}

void Server::chan_msg(Message * msg, std::string prefix) {
    std::string s;
    size_t i = 0;

    s += ("<" + _nick_database[prefix].top().nickname + ">@[" + _prefix_clients[prefix].current_chan + "] : " += msg->command);
    while (i < msg->params.size())
    {
        if (msg->params[i] != " ")
            s.append(" ");
        s.append(msg->params[i]);
        i++;
    }
    s += "\r\n";
    send_reply_broad(prefix, _channels[_prefix_clients[prefix].current_chan], -1, msg);
}

void Server::do_command(Message *msg, int fd)
{
    std::string tmp(msg->command);
    // std::cout << "{" << _fd_clients[fd].nickname << "} says : " << msg->command << std::endl;

    if (tmp == "PASS") {
        passcmd(msg, fd);
    }
    else if (tmp == "NICK")
    {
        if (_fd_clients[fd].password != _password)
            send_reply("", fd, ERR_PASSWDMISMATCH);
        else
            nickcmd(msg, fd);
    }
    else if (tmp == "USER" ) {
        if (_fd_clients[fd].password != _password)
            send_reply("", fd, ERR_PASSWDMISMATCH);
        else if (_fd_clients[fd].nickname[0] == 0)
            send_reply("", fd, ERR_NONICKNAMEGIVEN);
        else
            usercmd(msg, fd);
    }
    else if (_fd_clients[fd].is_register == true)
    {
        if (tmp == "JOIN")
            joincmd(msg, _fd_clients[fd].prefix);
        else if (tmp == "PRIVMSG")
            privmsgcmd(msg, _fd_clients[fd].prefix);
        else if (tmp == "NOTICE")
            noticecmd(msg, _fd_clients[fd].prefix);
        else if (_fd_clients[fd].current_chan.empty() == false)
            chan_msg(msg, _fd_clients[fd].prefix);
        else
            send_reply(msg->command, fd, ERR_NOTOCHANNEL);
    }
    else
        send_reply("", fd, ERR_NOTREGISTERED);
    delete msg;
}

void Server::send_reply_broad(std::string prefix, std::vector<Client> &cl, int code, Message *msg)
{
    std::string s;
    for (size_t i = 0; i < cl.size(); i++)
    {
        if (cl[i].clfd != _prefix_clients[prefix].clfd)
        {
            if (code != -1)
                send_reply("", cl[i].clfd, code);
            else
            {
                for(size_t i = 0; i < msg->params.size(); i++)
                    s += msg->params[i];
                send_reply(s, cl[i].clfd, 0);
            }
        }
    }
}

void Server::privmsgcmd(Message *msg, std::string prefix)
{
    // ERR_NORECIPIENT --NO                ERR_NOTEXTTOSEND -- Yes
    // ERR_CANNOTSENDTOCHAN --No            ERR_NOTOPLEVEL -- No
    // ERR_WILDTOPLEVEL --No                ERR_TOOMANYTARGETS --No
    // ERR_NOSUCHNICK --Yes
    //  RPL_AWAY --No
    size_t i = 0;
    Message text;
    Client cl_tmp;
    std::string curr_chan_tmp;
    std:: list<Client> nicknames;
    std::list<std::string> chans;
    while (i < msg->params.size() && msg->params[i] != ":")
    {
        if (_nick_database.count(msg->params[i]) == 1)
        {
            cl_tmp = _nick_database[msg->params[i]].top();
            if (cl_tmp.is_logged == true)
                nicknames.push_back(cl_tmp);
            else
                send_reply(msg->params[i], _prefix_clients[prefix].clfd, ERR_NOSUCHNICK);
        }
        else if (_channels.count(msg->params[i]) == 1)
            chans.push_back(msg->params[i]);
        else
            send_reply(msg->params[i], _prefix_clients[prefix].clfd, ERR_NOSUCHNICK);
        i++;
    }
    while (i < msg->params.size())
    {
        text.params.push_back(msg->params[i]);
        i++;
    }
    if (text.params.empty() == true)
        send_reply("", _prefix_clients[prefix].clfd, ERR_NOTEXTTOSEND);
    nicknames.sort();
    nicknames.unique();
    chans.sort();
    chans.unique();
    i = 0;
    curr_chan_tmp = _prefix_clients[prefix].current_chan;
    for (std::list<std::string>::iterator it = chans.begin(); it != chans.end(); it++)
    {
        _prefix_clients[prefix].current_chan = *it;
        chan_msg(&text, prefix);
    }
    _prefix_clients[prefix].current_chan = curr_chan_tmp;
    std::vector<Client> vec(nicknames.begin(), nicknames.end());
    send_reply_broad(prefix, vec, -1, &text);
}

void Server::noticecmd(Message *msg, std::string prefix)
{
    size_t i = 0;
    Message text;
    Client cl_tmp;
    std::string curr_chan_tmp;
    std:: list<Client> nicknames;
    std::list<std::string> chans;
    while (i < msg->params.size() && msg->params[i] != ":")
    {
        if (_nick_database.count(msg->params[i]) == 1)
        {
            cl_tmp = _nick_database[msg->params[i]].top();
            if (cl_tmp.is_logged == true)
                nicknames.push_back(cl_tmp);
        }
        else if (_channels.count(msg->params[i]) == 1)
            chans.push_back(msg->params[i]);
        i++;
    }
    while (i < msg->params.size())
    {
        text.params.push_back(msg->params[i]);
        i++;
    }
    if (text.params.empty() == true)
        return;
    nicknames.sort();
    nicknames.unique();
    chans.sort();
    chans.unique();
    i = 0;
    curr_chan_tmp = _prefix_clients[prefix].current_chan;
    for (std::list<std::string>::iterator it = chans.begin(); it != chans.end(); it++)
    {
        _prefix_clients[prefix].current_chan = *it;
        chan_msg(&text, prefix);
    }
    _prefix_clients[prefix].current_chan = curr_chan_tmp;
    send_reply_broad(prefix, nicknames, -1, &text);
}

//===============================================================================
