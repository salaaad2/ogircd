/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbajrami <tbajrami@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/29 13:30:58 by tbajrami          #+#    #+#             */
/*   Updated: 2021/05/27 18:21:41 by tbajrami         ###   ########lyon.fr   */
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
#include <unistd.h>
#include <ctime>

#include "message.hpp"
#include "client.hpp"
#include "fds.hpp"
#include "Params.hpp"
#include "replies.hpp"
#include "utils.hpp"

#define SOCKET_ERROR "Error: creating socket"
#define LISTEN_ERROR "Error: listening server"
#define BIND_ERROR "Error: binding server"
#define SETSOCK_ERROR "Error: setting socket options"


struct network
{
	std::string servername;
	int hopcount;
	int token;
};

class Server
{
	private:

		//server
		std::string                                             _servername;
		struct sockaddr_in			                _addr;
		char						        _ip[INET_ADDRSTRLEN];
		std::string						_password;
		std::string                                             _peer_password;
		int                                                     _port;
		Fds							*_fds;
		std::map<int, Client*>                                  _m_fdserver;
		Params                                                  *_pm;
		time_t                                              _launch_time;
		//client
		std::map<int, std::string>                              _m_fdprefix;
		std::map<std::string, Client*>           	 	_m_pclients;
		std::map<std::string, std::stack<Client*> > 		_m_nickdb;


		/*channels maps */

		std::map<std::string, std::vector<Client*> > 			_m_chans;
		std::map<std::string, std::string> 			_m_topics;
		std::map<std::string, std::string>			 _m_passwords;
		std::map<std::string, std::string>			 _m_flags;


		/* bans */
		std::map<std::string, std::vector<std::string> >		_m_banmask;
		std::map<std::string, std::string>						_m_whoban;
		std::map<std::string, uint64_t>							_m_banid;

		/* exceptions */

		std::map<std::string, std::vector<std::string> >		_m_exceptmask;
		std::map<std::string, std::string>						_m_whoexcept;
		std::map<std::string, uint64_t>							_m_exceptid;

		std::map<std::string, std::string>						_m_chankey;
		std::map<std::string, std::map<Client*, std::string> >	_m_uflags;
		std::map<std::string, size_t>							_m_limits;

	public:

		int							listener;
		std::vector<sockaddr_in> 	*_network;

		Server(Params *pm);
		int     addclient(int listener);

		void setFds(Fds *fds);
		Fds *getFds() const;


	private:

		void new_serv();
		int connect_serv();
		void do_registration(int fd);
		void create_client_prefix(int fd);
		void getIP();
		void send_reply(std::string s, Client *cl, int code);
		void send_reply_broad(Client *cl, std::vector<Client*> & v_cl, int code, Message *msg);
		std::string msg_rpl(std::string s, int code, Client *cl);

		/* MESSAGE TREATMENT */

	public:
		/*registration*/
		void do_command(Message *msg, int fd);
		void passcmd(Message *msg, int fd);
		void nickcmd(Message *msg, int fd);
		void usercmd(Message *msg, int fd);
		void send_to_channel(std::string send, std::string chan);

		/*server*/
		void quitcmd(Message *msg, Client *cl);
		void versioncmd(Message *msg, Client *cl);
		void statscmd(Message *msg, Client *cl);
		//	void linkscmd(Message *msg, Client *cl);
		void timecmd(Message *msg, Client *cl);
		void infocmd(Message *msg, Client *cl);
		void whocmd(Message *msg, Client *cl);

		/*server to server*/
		void servercmd(Message *msg, Client *cl, int fd);
		void connectcmd(Message *msg, Client *cl);
		void broadcast_known_servers(int fd);
		void broadcast_known_users(int fd);
		void createParams(Message *msg);

		/*channels*/

		void joincmd(Message *msg, Client *cl);
		void join2(std::string chan, std::string key, Client *cl);
		std::vector<std::string> parse_m_chans(std::vector<std::string> & params);
		std::vector<std::string> parse_keys(std::vector<std::string> params, std::vector<std::string> channels);
		bool isbanned(Client *cl, std::string chan);
		void new_channel(std::string chan, Client *cl);
		void partcmd(Message *msg, Client *cl);
		void namescmd(Message *msg, Client *cl);
		void listcmd(Message *msg, Client *cl);
		void modecmd(Message *msg, Client *cl);
		void chanMode(std::vector<std::string> params, Client *cl);
		void setChanMode(std::vector<std::string> params, Client *cl);
		void treat_modes(std::vector<std::string> params, std::vector<std::string> cmds, Client *cl);
		void treat_args(std::string chan, std::string cmd, Client *cl);



		/*messages*/
		void privmsgcmd(Message *msg, Client *cl);
		void noticecmd(Message *msg, Client *cl);
		void chan_msg(Message * msg, Client *cl);

};
