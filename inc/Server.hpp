/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbajrami <tbajrami@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/29 13:30:58 by tbajrami          #+#    #+#             */
/*   Updated: 2021/05/14 13:41:07 by tbajrami         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ftirc.hpp"
#include "Params.hpp"

class Server
{
private:

	struct sockaddr_in			_addr;
	char						_ip[INET_ADDRSTRLEN];
	char						_prefix[17];
	char						_password[32];
	std::map<char[9], char[32]> _register;
	Fds							*_fds;
	
public:

	int							listener;
	std::vector<sockaddr_in> 	*_network;

	Server(Params *pm);

	void setFds(Fds *fds);

private:

	void new_serv(Params *pm);
	void connect_serv(Params *pm);
	void do_connect(Params *pm);
	void getIP();
	void send_reply(int fd, char code[3], char prefix[]);

/* MESSAGE TREATMENT */

public:

	void do_command(Message *msg, Client &client);
	void passcmd(Message *msg, Client &client);
	void nickcmd(Message *msg, Client &client);

};