/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tcp_main.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbajrami <tbajrami@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/29 14:32:03 by tbajrami          #+#    #+#             */
/*   Updated: 2021/05/06 18:21:25 by tbajrami         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "ftirc.hpp"
#include "Params.hpp"
#include "Server.hpp"

int main_loop(Server &serv, std::map<int, Client> *clients, Fds *fds)
{
	int newfd;
	
	for (int i = 0 ; i <= fds->fdmax ; i++)
	{
		if(FD_ISSET(i, &fds->read))
		{
			if (i == serv.listener)
			{
				if ((newfd = addclient(clients, serv.listener)) != -1)
				{
					if(newfd > fds->fdmax)
						fds->fdmax = newfd;
					return newfd;
				}
			}
			else
				rec_data(serv, clients, i, fds);
		}
	}
	return 0;
}

int main(int ac, char *av[])
{
	std::map<int, Client> *clients = new std::map<int, Client>;
	int newfd;
	Params *pm = new Params(ac, av);
	Server serv(pm);
	Fds *fds = new Fds;

	FD_ZERO(&fds->master);
	FD_ZERO(&fds->read);
	FD_SET(serv.listener, &fds->master);
	fds->fdmax = serv.listener;
	for(;;)
	{
		fds->read = fds->master;
		if(select(fds->fdmax + 1, &fds->read, NULL, NULL, NULL) == -1)
		{
			std::cout << "Server-select() error";
			exit(1);
		}
		if ((newfd = main_loop(serv, clients, fds)) > 0)
			FD_SET(newfd, &fds->master);
	}
	return 0;
}