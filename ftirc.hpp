/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ftirc.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbajrami <tbajrami@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/30 12:55:56 by tbajrami          #+#    #+#             */
/*   Updated: 2021/05/05 16:15:44 by tbajrami         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <vector>

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

struct Client;

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

int     addclient(std::vector<Client> *clients, int listener);
void    rec_data(Server &serv, std::vector<Client> *clients, int i, Fds *fdmax);
void    parse_message(Server &serv, char buf[]);


/* UTILS */

const int	ft_atoi(const char *str);
void	    ft_bzero(void *s, size_t n);