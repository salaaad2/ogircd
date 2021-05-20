/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tcp_main.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbajrami <tbajrami@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/29 14:32:03 by tbajrami          #+#    #+#             */
/*   Updated: 2021/05/14 11:57:04 by tbajrami         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ftirc.hpp"
#include "../inc/Params.hpp"
#include "../inc/Server.hpp"

int main_loop(Server &serv, Fds *fds)
{
	int newfd;

	for (int i = 0 ; i <= fds->fdmax ; i++)
	{
		if(FD_ISSET(i, &fds->read))
		{
			if (i == serv.listener)
			{
				if ((newfd = serv.addclient(serv.listener)) != -1)
				{
					if(newfd > fds->fdmax)
						fds->fdmax = newfd;
					return newfd;
				}
			}
			else
				rec_data(serv, i, fds);
		}
	}
	return 0;
}

int main(int ac, char *av[])
{
	if (ac < 3)
	{
		std::cout << "Insufficient parameters. \nUsage : ./ircserv [PORT] [PASS]" << std::endl;
		return (1);
	}
	int newfd;
	Params *pm = new Params(ac, av);
	Server serv(pm);
	Fds *fds = new Fds;
	serv.setFds(fds);

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
		if ((newfd = main_loop(serv, fds)) > 0)
			FD_SET(newfd, &fds->master);
	}
	return 0;
}
