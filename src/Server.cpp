/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbajrami <tbajrami@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/13 14:12:23 by tbajrami          #+#    #+#             */
/*   Updated: 2021/05/17 18:42:02 by tbajrami         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"

Server::Server(Params *pm)
{
    _network = new std::vector<sockaddr_in>;
    if (pm->isnew())
        new_serv(pm);
    else
        connect_serv(pm);
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
    char buffer[100];
    const char* p = inet_ntoa(name.sin_addr);
    if(p != NULL)
    {
        printf("Local ip is : %s \n" , p);
    }
    strcpy(_ip, p);
    _prefix[0] = ':';
    strcat(_prefix, _ip);
}

void Server::setFds(Fds *fds) {_fds = fds;}

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
    char message[1024];

    struct sockaddr_in server_address;

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(pm->getPortNetwork());
    server_address.sin_addr.s_addr = INADDR_ANY;
    this->_network->push_back(server_address);
    
    int connection_status = connect(net_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    if (connection_status == -1)
        printf("Error making connection to the remote socket\n\n");
    send(net_socket, "SERVER", 6, 0);
}



/******************/
/* TREAT COMMANDS */
/******************/

void Server::send_reply(int fd, int code)
{

    std::string ccmd = ft_format_cmd(ft_utoa(code));
    std::string to_send;

    to_send += (std::string(_prefix) + " " + ccmd + " " + msg_rpl(code, fd) + "\r\n");
    send(fd, to_send.c_str(), strlen(to_send.c_str()), 0);
}

void Server::do_command(Message *msg, int fd)
{
    std::string tmp(msg->command);

    std::cout << "{" << _fd_clients[fd].nickname << "} says : " << msg->command << std::endl;
    if (tmp == "PASS") {
        passcmd(msg, fd);
    }
    else if (tmp == "NICK") {
        nickcmd(msg, fd);
    }
    else if (tmp == "USER") {
        usercmd(msg, fd);
    }
    else
        send_reply(fd, RPL_NONE);
    // case "PASS":
        //     passcmd(msg, client);
        //     break;
        // case "PASS":
        //     passcmd(msg, client);
        //     break;
        // case "PASS":
        //     passcmd(msg, client);
        //     break;
}



void Server::passcmd(Message *msg, int fd)
{
    if (_fd_clients[fd].is_register == true)
        send_reply(fd, ERR_ALREADYREGISTERED);
    else if (!msg->params[0][0])
        send_reply(fd, ERR_NEEDMOREPARAMS);
    else
        strcpy(_fd_clients[fd].password, msg->params[0]);
    // else if (!strcmp(msg->params[0], _password))
    // {
    //     client.is_register = true;
    //     std::cout << "\nWelcome\n";
    //     send(client.clfd, "welcome", 8, 0);
    // }
    // else
    // {
    //     std::cout << "\nBad password : deconnexion\n";
    //     send(client.clfd, "\nBad password\n", 13, 0);
    //     close(client.clfd);
    //     FD_CLR(client.clfd, &_fds->master);
    //}
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
    if (_fd_clients[fd].is_register == true)
        send_reply(fd, ERR_ALREADYREGISTERED);
    if (!msg->params[0][0] || !msg->params[1][0] || msg->params[2][0] || msg->params[3][0])
        send_reply(fd, ERR_NEEDMOREPARAMS);
    strcpy(_fd_clients[fd].username, msg->params[0]);
    strcpy(_fd_clients[fd].realname, msg->params[3]);
    do_registration(fd);
}

void Server::do_registration(int fd)
{
    if (_fd_clients[fd].nickname[0] && !strcmp(_fd_clients[fd].password, _password))
    {
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

std::map<int, Client> Server::getFDClients(void) const {
    return (_fd_clients);
}
std::map<std::string, Client> Server::getNickClients(void) const {
    return (_nick_clients);
}

void Server::setFDClients(int i, Client cl) {
    _fd_clients[i] = cl;
}

void Server::setNickClients(std::string s, Client cl) {
    _nick_clients[s] = cl;
}

int Server::addclient(Server &serv,  int listener)
{
    Client nc;

    nc.is_server = false;
    nc.is_register = false;
    memset(nc.nickname, '\0', 9);
    int addrlen = sizeof(nc.clientaddr);
    if((nc.clfd = accept(listener, (struct sockaddr *)&nc.clientaddr, &nc.addrlen)) == -1)
    {
		std::cout << "Server-accept() error\n";
        return (-1);
    }
    else
        std::cout << "Server-accept() is OK...\n";
    std::cout << "New connection from " << inet_ntoa(nc.clientaddr.sin_addr);
    strcpy(nc.host, inet_ntoa(nc.clientaddr.sin_addr));
    std::cout << " on socket " << nc.clfd << std::endl;
    serv.setFDClients(nc.clfd, nc);
    return (nc.clfd);
}
