/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbajrami <tbajrami@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/30 12:46:11 by tbajrami          #+#    #+#             */
/*   Updated: 2021/05/06 16:32:43 by tbajrami         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ftirc.hpp"

struct Client
{
    sockaddr_in  clientaddr;
    unsigned int addrlen;
    int          clfd;
    char         nickname[9];
    bool         is_server;
};
