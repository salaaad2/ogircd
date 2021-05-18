/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ftirc.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbajrami <tbajrami@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/30 12:55:56 by tbajrami          #+#    #+#             */
/*   Updated: 2021/05/14 14:05:10 by tbajrami         ###   ########lyon.fr   */
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
#include "fds.hpp"
#include "message.hpp"
#include "client.hpp"
#include "Server.hpp"
#include "Params.hpp"

void    rec_data(Server &serv, int i, Fds *fdmax);
