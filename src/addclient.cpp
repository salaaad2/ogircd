/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   addclient.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbajrami <tbajrami@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/30 12:52:51 by tbajrami          #+#    #+#             */
/*   Updated: 2021/05/05 15:41:33 by tbajrami         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "ftirc.hpp"
#include "Client.hpp"

int addclient(std::vector<Client> *clients, int listener)
{
    Client nc;

    nc.is_server = false;
    int addrlen = sizeof(nc.clientaddr);
    (void)addrlen;
    if((nc.clfd = accept(listener, (struct sockaddr *)&nc.clientaddr, &nc.addrlen)) == -1)
    {
		std::cout << "Server-accept() error\n";
        return (-1);
    }
    else
        std::cout << "Server-accept() is OK...\n";
    std::cout << "New connection from " << inet_ntoa(nc.clientaddr.sin_addr);
    std::cout << " on socket " << nc.clfd << std::endl;
    clients->push_back(nc);
    return (nc.clfd);
}
