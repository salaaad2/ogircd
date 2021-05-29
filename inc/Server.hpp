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
		std::string						_prefix;
		std::string						_password;
		std::string                                             _peer_password;
		int                                                     _port;
		Fds							*_fds;
		std::map<int, network*>                                  _m_fdserver;
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
		void send_reply(std::string s, std::string prefix, int code);
		void send_reply_broad(std::string prefix, std::vector<Client*> & cl, int code, Message *msg);
		std::string msg_rpl(std::string s, int code, std::string prefix);

/* MESSAGE TREATMENT */

	public:
		/*registration*/
		void do_command(Message *msg, int fd);
		void passcmd(Message *msg, int fd);
		void nickcmd(Message *msg, int fd);
		void usercmd(Message *msg, int fd);
		void send_to_channel(std::string send, std::string chan);

		/*server*/
		void quitcmd(Message *msg, std::string & prefix);
		void versioncmd(Message *msg, std::string & prefix);
		void statscmd(Message *msg, std::string & prefix);
	//	void linkscmd(Message *msg, std::string prefix);
		void timecmd(Message *msg, std::string & prefix);
		void infocmd(Message *msg, std::string & prefix);
		void whocmd(Message *msg, std::string & prefix);

		/*server to server*/
		void servercmd(Message *msg, std::string prefix, int fd);
		void connectcmd(Message *msg, std::string & prefix);
		void broadcast_known_servers(int fd);
		void broadcast_known_users(int fd);
		void createParams(Message *msg);

		/*channels*/

		void joincmd(Message *msg, std::string prefix);
		void join2(std::string chan, std::string key, std::string prefix);
		std::vector<std::string> parse_m_chans(std::vector<std::string> & params);
		std::vector<std::string> parse_keys(std::vector<std::string> params, std::vector<std::string> channels);
		bool isbanned(std::string prefix, std::string chan);
		void new_channel(std::string chan, std::string & prefix);
		void partcmd(Message *msg, std::string prefix);
		void namescmd(Message *msg, std::string prefix);
		void listcmd(Message *msg, std::string prefix);
		void modecmd(Message *msg, std::string prefix);
			void chanMode(std::vector<std::string> params, std::string prefix);
			void setChanMode(std::vector<std::string> params, std::string prefix);
			void treat_modes(std::vector<std::string> params, std::vector<std::string> cmds, std::string prefix);
			void treat_args(std::string chan, std::string cmd, std::string prefix);

			

		/*messages*/
		void privmsgcmd(Message *msg, std::string & prefix);
		void noticecmd(Message *msg, std::string & prefix);
		void chan_msg(Message * msg, std::string prefix);
		
};
