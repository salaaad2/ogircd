/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbajrami <tbajrami@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/29 13:30:58 by tbajrami          #+#    #+#             */
/*   Updated: 2021/05/20 14:02:50 by tbajrami         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <vector>
#include <stack>
#include <list>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#include "message.hpp"
#include "client.hpp"
#include "fds.hpp"
#include "Params.hpp"
#include "replies.hpp"

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
		Fds							*_fds;

		std::map<int, Client>		 			_fd_clients;
		std::map<std::string, Client>           	 	_prefix_clients;
		std::map<std::string, std::stack<Client > > 		_nick_database;
		std::map<std::string, std::vector<Client> > 			_channels;
		std::map<std::string, std::string> 			_topics;
		std::map<std::string, std::string>			 _passwords;
		std::map<std::string, std::string>			 _modes;
		std::map<std::string, std::map<Client, std::string> >	 _u_modes;
	public:

		int							listener;
		std::vector<sockaddr_in> 	*_network;

		Server(Params *pm);
		int     addclient(int listener);

		void setFds(Fds *fds);


	private:

		void new_serv(Params *pm);
		void connect_serv(Params *pm);
		void do_connect(Params *pm);
		void do_registration(int fd);
		void create_client_prefix(int fd);
		void getIP();
		void send_reply(std::string s, int fd, int code);
		void send_reply_broad(std::string prefix, std::vector<Client> &cl, int code, Message *msg);
		std::string msg_rpl(std::string s, int code, int fd);

/* MESSAGE TREATMENT */

	public:
		/*registration*/
		void do_command(Message *msg, int fd);
		void passcmd(Message *msg, int fd);
		void nickcmd(Message *msg, int fd);
		void usercmd(Message *msg, int fd);

		/*channels*/
		void joincmd(Message *msg, std::string prefix);
		void join2(std::string chan, std::string prefix);
		std::vector<std::string> parse_channels(std::vector<std::string> params);
		void new_channel(std::string chan, std::string prefix);


		void privmsgcmd(Message *msg, std::string prefix);
		void noticecmd(Message *msg, std::string prefix);
		void chan_msg(Message * msg, std::string prefix);

};
