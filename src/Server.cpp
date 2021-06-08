#include "../inc/Server.hpp"
#include "../inc/commands.hpp"

#include <netdb.h>
#include <iostream>

void Server::setFds(Fds *fds)
{
	_fds = fds;
}

Fds *Server::getFds() const
{
	return (_fds);
}

int Server::getStatus() const
{
	return (_status);
}

Server::Server(std::vector<std::string> &vm)
{
	time(&_launch_time);
	_servername = "42lyon.irc.fr";
	new_serv(vm);
	_servername = _ip;
	_status = 1;
}

Server::~Server()
{
	std::map<std::string, Client *>::iterator mit;
	for (mit = _m_pclients.begin(); mit != _m_pclients.end(); ++mit) {
		delete (*mit).second;
	}
}

//=====================CREATION AND CONNECTION OF THE SERVER============================

void Server::new_serv(std::vector<std::string> &vm)
{
	int yes = 1;

	getIP();
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = INADDR_ANY;
	_addr.sin_port = htons(atoi(vm[0].c_str()));
	if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		std::cerr << SOCKET_ERROR << std::endl;
		exit(1);
	}
	if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) ==
	    -1) {
		std::cerr << SETSOCK_ERROR << std::endl;
		;
		exit(1);
	}
	_port = atoi(vm[0].c_str());
	_password = vm[1];
	ft_bzero(&(_addr.sin_zero), 8);
	if (bind(listener, (struct sockaddr *)&_addr, sizeof(_addr)) == -1) {
		perror(BIND_ERROR);
		exit(1);
	}
	if (listen(listener, 10) == -1) {
		perror(LISTEN_ERROR);
		exit(1);
	}
}

//========================================================================================

int Server::addclient(int listener)
{
	Client *nc = new Client();
	std::string s;

	if ((nc->clfd = accept(listener, nc->clientaddr, &nc->addrlen)) == -1) {
		std::cerr << "Server-accept() error\n";
		return (-1);
	} else
		nc->host = inet_ntoa(
			((struct sockaddr_in *)nc->clientaddr)->sin_addr);
	s = ft_utoa(nc->clfd);
	_m_pclients[s] = nc;
	_m_fdprefix[nc->clfd] = s;
	return (nc->clfd);
}

void Server::getIP()
{
	struct sockaddr_in serv;
	const char *google_dns_server = "8.8.8.8";
	int dns_port = 53;
	int sock = socket(AF_INET, SOCK_DGRAM, 0);

	memset(&serv, 0, sizeof(serv));
	serv.sin_family = AF_INET;
	serv.sin_addr.s_addr = inet_addr(google_dns_server);
	serv.sin_port = htons(dns_port);
	int err = connect(sock, (const struct sockaddr *)&serv, sizeof(serv));
	struct sockaddr_in name;
	socklen_t namelen = sizeof(name);
	err = getsockname(sock, (struct sockaddr *)&name, &namelen);
	const char *p = inet_ntoa(name.sin_addr);
	strcpy(_ip, p);
}

//==============================TREAT COMMANDS======================================

void Server::send_reply(std::string s, Client *cl, int code)
{
	std::string ccmd;
	std::string to_send;
	std::string prefix;
	if (code)
		ccmd = ft_format_cmd(ft_utoa(code));
	prefix = ":";
	prefix += _ip;

	to_send += (prefix + " " + ccmd + " " + cl->nickname + " " +
		    msg_rpl(s, code, cl) + "\r\n");
	send(cl->clfd, to_send.c_str(), to_send.length(), 0);
}

void Server::send_reply_broad(Client *cl, std::vector<Client *> &v_cl, int code,
			      Message *msg)
{
	std::string s;

	for (size_t i = 0; i < v_cl.size(); i++) {
		if (v_cl[i]->clfd != cl->clfd) {
			if (code != -1)
				send_reply("", v_cl[i], code);
			else {
				for (size_t i = 0; i < msg->params.size(); i++)
					s += msg->params[i];
				send_reply(s, v_cl[i], 0);
			}
		}
	}
}

void Server::do_command(Message *msg, int fd)
{
	std::string req;
	Client *cl = _m_pclients[_m_fdprefix[fd]];
	static std::map<std::string, e_commands> c_map;

	if (c_map.empty()) {
		initcmdmap(&c_map);
	}
	if (msg->command == "PASS") {
		passcmd(msg, fd);
	} else if (msg->command == "NICK") {
		if (_m_fdprefix.count(fd) != 0)
			nickcmd(msg, fd);
		else if (_m_pclients[_m_fdprefix[fd]]->password != _password)
			send_reply("", cl, ERR_PASSWDMISMATCH);
		else
			nickcmd(msg, fd);
	} else if (msg->command == "USER") {
		if (_m_pclients[_m_fdprefix[fd]]->password != _password)
			send_reply("", cl, ERR_PASSWDMISMATCH);
		else if (_m_pclients[_m_fdprefix[fd]]->nickname[0] == 0)
			send_reply("", cl, ERR_NONICKNAMEGIVEN);
		else
			usercmd(msg, fd);
	} else if (_m_pclients.count(_m_fdprefix[fd]) &&
			   _m_pclients[_m_fdprefix[fd]]->is_register == true) {
		switch(c_map[msg->command]) {
			case JOIN: joincmd(msg, cl); break;
			case INVITE: invitecmd(msg, cl); break;
			case PART: partcmd(msg, cl); break;
			case NAMES: namescmd(msg, cl); break;
			case LIST: listcmd(msg, cl); break;
			case MODE: modecmd(msg, cl); break;
			case PRIVMSG: privmsgcmd(msg, cl, 1); break;
			case NOTICE: privmsgcmd(msg, cl, 0); break;
			case QUIT: quitcmd(msg, cl); break;
			case VERSION: versioncmd(msg, cl); break;
			case TIME: timecmd(msg, cl); break;
			case INFO: infocmd(msg, cl); break;
			case WHO: whocmd(msg, cl); break;
			case WHOWAS: whowascmd(msg, cl); break;
			case PING: pingcmd(msg, cl); break;
			case WHOIS: whoiscmd(msg, cl); break;
			case TOPIC: topiccmd(msg, cl); break;
			case KICK: kickcmd(msg, cl); break;
			case SHUTDOWN: shutdcmd(msg, cl); break;
		}
	} else
		send_reply("", cl, ERR_NOTREGISTERED);
	delete msg;
}

void Server::delog(int fd)
{
	Client *cl = _m_pclients[_m_fdprefix[fd]];

	for (std::vector<std::string>::iterator it = cl->chans.begin();
	     it != cl->chans.end(); it++) {
		send_to_channel(":" + cl->prefix +
					" QUIT :Client closed connection\r\n",
				*it, NULL);
		_m_chans[*it].erase(clposition(cl->nickname, *it));
		if (_m_uflags[*it].find(cl) != _m_uflags[*it].end())
			_m_uflags[*it].erase(cl);
	}
	cl->chans.clear();
	cl->is_logged = false;
	_m_pclients.erase(cl->prefix);
	_m_nickdb.erase(cl->nickname);
	close(cl->clfd);
	FD_CLR(cl->clfd, &_fds->master);
	delete cl;
}

//===============================================================================
