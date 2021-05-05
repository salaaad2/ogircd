/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbajrami <tbajrami@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/29 13:30:58 by tbajrami          #+#    #+#             */
/*   Updated: 2021/05/03 16:57:27 by tbajrami         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ftirc.hpp"
#include "Params.hpp"

class Server
{
private:

	struct sockaddr_in			_addr;
	char						*_ip;
	
public:

	int							listener;
	std::vector<sockaddr_in> 	*_network;

	Server(Params *pm)
	{
		_network = new std::vector<sockaddr_in>;
		if (pm->isnew())
			new_serv(pm);
		else
			connect_serv(pm);
	}

private:

	void new_serv(Params *pm)
	{
		int yes = 1;
		if((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		{
			std::cout << "Server-socket() error\n";
			exit(1);
		}
		if(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		{
			std::cout << "Server-setsockopt() error\n";
			exit(1);
		}
		_addr.sin_family = AF_INET;
		_addr.sin_addr.s_addr = INADDR_ANY;
		_addr.sin_port = htons(pm->getPort());
		ft_bzero(&(_addr.sin_zero), 8);
		_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		_ip = inet_ntoa(_addr.sin_addr);
		printf("ip : %s\n", _ip);
		if(bind(listener, (struct sockaddr *)&_addr, sizeof(_addr)) == -1)
		{
			perror("Server-bind() error");
			exit(1);
		}
		if(listen(listener, 10) == -1)
		{
			perror("Server-listen() error");
			exit(1);
		}
	}

	void connect_serv(Params *pm)
	{
		int yes = 1;
		if((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		{
			std::cout << "Server-socket() error\n";
			exit(1);
		}
		if(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		{
			std::cout << "Server-setsockopt() error\n";
			exit(1);
		}
		_addr.sin_family = AF_INET;
		_addr.sin_addr.s_addr = INADDR_ANY;
		_addr.sin_port = htons(pm->getPort());
		ft_bzero(&(_addr.sin_zero), 8);
		_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		_ip = inet_ntoa(_addr.sin_addr);
		printf("ip : %s\n", _ip);
		if(bind(listener, (struct sockaddr *)&_addr, sizeof(_addr)) == -1)
		{
			perror("Server-bind() error");
			exit(1);
		}
		if(listen(listener, 10) == -1)
		{
			perror("Server-listen() error");
			exit(1);
		}
		do_connect(pm);
	}

	void do_connect(Params *pm)
	{
		int net_socket = socket(AF_INET, SOCK_STREAM, 0);
		char message[1024];

		struct sockaddr_in server_address;

		server_address.sin_family = AF_INET;
		server_address.sin_port = htons(pm->getPortNetwork());
		server_address.sin_addr.s_addr = INADDR_ANY;
		server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
		this->_network->push_back(server_address);
		
		int connection_status = connect(net_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    	if (connection_status == -1)
        	printf("Error making connection to the remote socket\n\n");
		send(net_socket, "SERVER", 6, 0);
	}
};