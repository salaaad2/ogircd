/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ftirc.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbajrami <tbajrami@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/30 12:55:56 by tbajrami          #+#    #+#             */
/*   Updated: 2021/05/13 11:57:23 by tbajrami         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include "replies.hpp"

struct Client
{
    sockaddr_in  clientaddr;
    unsigned int addrlen;
    int          clfd;
    char         nickname[9];
    bool         is_server;
    bool         is_register;
};

struct Message
{
    char    prefix[64];
    char    command[64];
    char    params[32][32];
};

struct Fds
{
    fd_set master;
    fd_set read;
    int    fdmax;
};

class Server;
class Params;

int     addclient(std::map<int, Client> *clients, int listener);
void    rec_data(Server &serv, std::map<int, Client> *clients, int i, Fds *fdmax);
Message *parse_message(Server &serv, char buf[]);
char    *msg_error(int code);


/* UTILS */

int	        ft_atoi(const char *str);
void	    ft_bzero(void *s, size_t n);