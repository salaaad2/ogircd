/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbajrami <tbajrami@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/29 13:30:58 by tbajrami          #+#    #+#             */
/*   Updated: 2021/05/30 16:15:37 by tbajrami         ###   ########lyon.fr   */
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
		struct sockaddr_in										_addr;
		char													_ip[INET_ADDRSTRLEN];
		std::string												_prefix;
		std::string												_password;
		std::string                                             _peer_password;
		int                                                     _port;
		Fds														*_fds;
		std::map<int, network*>                                  _m_fdserver;
		Params                                                  *_pm;
		time_t													_launch_time;

		//client
		std::map<int, std::string>                              _m_fdprefix; // _m_pclients[_m_fdprefix[fd]] = find client with fd
		std::map<std::string, Client*>							_m_pclients; // _m_pclients[prefix] = find client with prefix
		std::map<std::string, std::stack<Client*> >				_m_nickdb; // _m_nickdb[name] = last client with nickname "name"

		

		/*channels maps */

		std::map<std::string, std::vector<Client*> > 			_m_chans; // _m_chans[#channel] = vector containing all clients on #channel
		std::map<std::string, std::string>						_m_topics; // _m_topics[#channel] = topic of channel
		std::map<std::string, std::string>						_m_flags; // _m_flags[#channel] = all channels flags

		/* invite */

		std::map<std::string, std::vector<std::string> >		_m_invite; // list of nicknames invited on channel (if 'i' mode set)

		/* bans */

		std::map<std::string, std::vector<std::string> >		_m_banmask; // all banned patterns
		std::map<std::string, std::string>						_m_whoban; // _m_whoban[pattern] = name of those who banned the pattern
		std::map<std::string, uint64_t>							_m_banid; // _m_banid[pattern] = unique id of ban

		/* exceptions */

		std::map<std::string, std::vector<std::string> >		_m_exceptmask; // all ban exceptions patterns
		std::map<std::string, std::string>						_m_whoexcept; // _m_whoexcept[pattern] = name of those who ban except the pattern
		std::map<std::string, uint64_t>							_m_exceptid; // _m_exceptid[pattern] = unique id of except

		std::map<std::string, std::string>						_m_chankey; // _m_chankey[#channel] = password of channel if set
		std::map<std::string, std::map<Client*, std::string> >	_m_uflags; // _m_uflags[#channel][client] = flags set for user in given channel
		std::map<std::string, size_t>							_m_limits; // _m_limits[#channel] = limit of users in channel (if 'l' mode set)

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
		std::vector<std::string> parse_m_chans(std::string chan);
		std::vector<std::string> parse_keys(std::string keys, std::vector<std::string> channels);
		bool isbanned(std::string prefix, std::string chan);
		bool isinvited(std::string nickname, std::string chan);
		void new_channel(std::string chan, std::string & prefix);
		void partcmd(Message *msg, std::string prefix);
		void namescmd(Message *msg, std::string prefix);
		void listcmd(Message *msg, std::string prefix);
		void modecmd(Message *msg, std::string prefix);
		void chanMode(std::vector<std::string> params, std::string prefix);
		void setChanMode(std::vector<std::string> params, std::string prefix);
		void treat_modes(std::vector<std::string> params, std::vector<std::string> cmds, std::string prefix);
		void treat_args(std::string chan, std::string cmd, std::string prefix);

		void invitecmd(Message *msg, std::string prefix);

		/*messages*/
		void privmsgcmd(Message *msg, std::string & prefix);
		void noticecmd(Message *msg, std::string & prefix);
		void chan_msg(Message * msg, std::string prefix);

};
