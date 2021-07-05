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

#include "Message.hpp"
#include "Client.hpp"
#include "fds.hpp"
#include "replies.hpp"
#include "utils.hpp"

#define SOCKET_ERROR "Error: creating socket"
#define LISTEN_ERROR "Error: listening server"
#define BIND_ERROR "Error: binding server"
#define SETSOCK_ERROR "Error: setting socket options"

using std::string;

class Server {
    private:
	/* server */

	string _servername;
	struct sockaddr_in _addr;
	char _ip[INET_ADDRSTRLEN];
	string _password;
	int _port;
	int _status;
	Fds *_fds;
	time_t _launch_time;

	/* client */

	std::map<int, string>
		_m_fdprefix; // _m_pclients[_m_fdprefix[fd]] = find client with fd
	std::map<string, Client *>
		_m_pclients; // _m_pclients[prefix] = find client with prefix
	std::map<string, std::stack<Client *> >
		_m_nickdb; // _m_nickdb[name] = last client with nickname "name"

	/*channels maps */

	std::map<string, std::vector<Client *> >
		_m_chans; // _m_chans[#channel] = vector containing all clients on #channel
	std::map<string, string>
		_m_topics; // _m_topics[#channel] = topic of channel
	std::map<string, string>
		_m_flags; // _m_flags[#channel] = all channels flags

	/* invite */

	std::map<string, std::vector<string> >
		_m_invite; // list of nicknames invited on channel (if 'i' mode set)

	/* bans */

	std::map<string, std::vector<string> >
		_m_banmask; // all banned patterns
	std::map<string, string>
		_m_whoban; // _m_whoban[pattern] = name of those who banned the pattern
	std::map<string, uint64_t>
		_m_banid; // _m_banid[pattern] = unique id of ban

	/* exceptions */

	std::map<string, std::vector<string> >
		_m_exceptmask; // all ban exceptions patterns
	std::map<string, string>
		_m_whoexcept; // _m_whoexcept[pattern] = name of those who ban except the pattern
	std::map<string, uint64_t>
		_m_exceptid; // _m_exceptid[pattern] = unique id of except

	/* other */

	std::map<string, string>
		_m_chankey; // _m_chankey[#channel] = password of channel if set
	std::map<string, std::map<Client *, string> >
		_m_uflags; // _m_uflags[#channel][client] = flags set for user in given channel
	std::map<string, size_t>
		_m_limits; // _m_limits[#channel] = limit of users in channel (if 'l' mode set)

    public:
	Server(std::vector<string> &vm);
	~Server();

	int listener;
	std::map<int, string> _m_fdreq;

	int addclient(int listener);
	void setFds(Fds *fds);
	Fds *getFds() const;
	int getStatus() const;
	void delog(int fd);
	void do_command(Message *msg, int fd);

    private:
	void new_serv(std::vector<string> &vm);
	void do_registration(int fd);
	void create_client_prefix(int fd);
	void getIP();
	void send_reply(string s, Client *cl, int code);
	void send_reply_broad(Client *cl, std::vector<Client *> &v_cl, int code,
			      Message *msg);
	string msg_rpl(string s, int code, Client *cl);

	/* MESSAGE TREATMENT */

	/* registration */

	void passcmd(Message *msg, int fd);
	void nickcmd(Message *msg, int fd);
	void usercmd(Message *msg, int fd);
	void send_to_channel(string send, string &chan, Client *cl);

	/* server */

	void shutdcmd(Message *msg, Client *cl);
	void quitcmd(Message *msg, Client *cl);
	void versioncmd(Message *msg, Client *cl);
	void statscmd(Message *msg, Client *cl);
	void timecmd(Message *msg, Client *cl);
	void infocmd(Message *msg, Client *cl);
	void whocmd(Message *msg, Client *cl);
	void pingcmd(Message *msg, Client *cl);
	void whoiscmd(Message *msg, Client *cl);
	void whowascmd(Message *msg, Client *cl);

	/* channels */

	void joincmd(Message *msg, Client *cl);
	void join2(string &chan, string &key, Client *cl);
	std::vector<string> parse_m_chans(string &chan);
	std::vector<string> parse_keys(string &keys,
					    std::vector<string> &channels);
	bool isbanned(Client *cl, string &chan);
	bool isexcepted(Client *cl, string chan);
	void new_channel(string chan, Client *cl);
	void partcmd(Message *msg, Client *cl);
	void namescmd(Message *msg, Client *cl);
	bool isNickonchan(string &nick, string &chan);
	void listcmd(Message *msg, Client *cl);
	void modecmd(Message *msg, Client *cl);
	void setChanMode(std::vector<string> params, Client *cl);
	void chanMode(std::vector<string> params, Client *cl);
	void treat_modes(std::vector<string> params,
			 std::vector<string> cmds, Client *cl);
	void treat_args(string chan, string cmd, Client *cl);
	bool isinvited(string &nickname, string &chan);
	void invitecmd(Message *msg, Client *cl);
	void topiccmd(Message *msg, Client *cl);
	void kickcmd(Message *msg, Client *cl);

	/* messages */

	void privmsgcmd(Message *msg, Client *cl, bool reply);
	void chan_msg(Message *msg, Client *cl);

	/* utils */

	std::vector<Client *>::iterator clposition(string &nick,
						   string &chan);
	std::vector<string>::iterator chposition(Client *cl,
						      string &chan);
};
