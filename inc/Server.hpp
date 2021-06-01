/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbajrami <tbajrami@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/29 13:30:58 by tbajrami          #+#    #+#             */
/*   Updated: 2021/06/01 15:47:46 by tbajrami         ###   ########lyon.fr   */
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



class Server
{
	private:

		//server
		std::string                                             _servername;
		struct sockaddr_in			                _addr;
		char						        _ip[INET_ADDRSTRLEN];
		std::string						_password;
		int                                                     _port;
		Fds							*_fds;
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

		Server(Params *pm);
		int     addclient(int listener);

		void setFds(Fds *fds);
		Fds *getFds() const;


	private:

		void new_serv();
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

		/*channels*/

		void joincmd(Message *msg, Client *cl);
		void join2(std::string chan, std::string key, Client *cl);
		std::vector<std::string> parse_m_chans(std::string chan);
		std::vector<std::string> parse_keys(std::string keys, std::vector<std::string> channels);
		bool isbanned(Client *cl, std::string chan);
		void new_channel(std::string chan, Client *cl);
		void partcmd(Message *msg, Client *cl);
		void namescmd(Message *msg, Client *cl);
		bool isNickonchan(std::string nick, std::string chan);
		void listcmd(Message *msg, Client *cl);
		void modecmd(Message *msg, Client *cl);
		void setChanMode(std::vector<std::string> params, Client *cl);
		void chanMode(std::vector<std::string> params, Client *cl);
		void treat_modes(std::vector<std::string> params, std::vector<std::string> cmds, Client *cl);
		void treat_args(std::string chan, std::string cmd, Client *cl);
		bool isinvited(std::string nickname, std::string chan);

		void invitecmd(Message *msg, Client *cl);
		void topiccmd(Message *msg, Client *cl);
		void kickcmd(Message *msg, Client *cl);

		/*messages*/
		void privmsgcmd(Message *msg, Client *cl);
		void noticecmd(Message *msg, Client *cl);
		void chan_msg(Message * msg, Client *cl);

		std::vector<Client *>::iterator clposition(std::string nick, std::string chan);
		std::vector<std::string>::iterator chposition(Client *cl, std::string chan);

};
