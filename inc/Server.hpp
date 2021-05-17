/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbajrami <tbajrami@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/29 13:30:58 by tbajrami          #+#    #+#             */
/*   Updated: 2021/05/17 15:51:13 by tbajrami         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "message.hpp"
#include "client.hpp"
#include "fds.hpp"
#include "Params.hpp"
#include <map>
#include "replies.hpp"
#include <vector>
#include <cstring>
#define SOCKET_ERROR "Error: creating socket"
#define LISTEN_ERROR "Error: listening server"
#define BIND_ERROR "Error: binding server"
#define SETSOCK_ERROR "Error: setting socket options"


class Server
{
	private:

		struct sockaddr_in			_addr;
		char						_ip[INET_ADDRSTRLEN];
		char						_prefix[17];
		char						_password[32];
		std::map<char[9], char[32]> _register;
		Fds							*_fds;
		std::map<int, Client> _fd_clients;
		std::map<std::string, Client> _nick_clients;

	public:

		int							listener;
		std::vector<sockaddr_in> 	*_network;

		Server(Params *pm);
int     addclient(Server &serv, int i);

		void setFds(Fds *fds);
		std::map<int, Client> getFDClients(void) const;
		void setFDClients(int, Client);
		std::map<std::string, Client> getNickClients(void) const;
		void setNickClients(std:: string, Client);


	private:

		void new_serv(Params *pm);
		void connect_serv(Params *pm);
		void do_connect(Params *pm);
		void getIP();
		void send_reply(int fd, int code, const char *msg);

/* MESSAGE TREATMENT */

	public:

		void do_command(Message *msg, int fd);
		void passcmd(Message *msg, int fd);
		void nickcmd(Message *msg, int fd);

};
