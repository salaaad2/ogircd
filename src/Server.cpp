/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbajrami <tbajrami@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/13 14:12:23 by tbajrami          #+#    #+#             */
/*   Updated: 2021/06/01 17:32:06 by tbajrami         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"

#include <netdb.h>

void Server::setFds(Fds *fds) {_fds = fds;}

Fds *Server::getFds() const { return (_fds);}


Server::Server(Params *pm)
{
	time(&_launch_time);
	_pm = pm;
	_servername = "42lyon.irc.fr";
	new_serv();
	_servername = _ip;
}

//=====================CREATION AND CONNECTION OF THE SERVER============================

void Server::new_serv()
{
	int yes = 1;

	getIP();
	_fds = new Fds;
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = INADDR_ANY;
	_addr.sin_port = htons(_pm->getPort());
	if((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		std::cerr << SOCKET_ERROR << std::endl;
		exit(1);
	}
	if(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		std::cerr << SETSOCK_ERROR << std::endl;;
		exit(1);
	}
	_port = _pm->getPort();
	_password = _pm->getPwd();
	ft_bzero(&(_addr.sin_zero), 8);
	if(bind(listener, (struct sockaddr *)&_addr, sizeof(_addr)) == -1)
	{
		perror(BIND_ERROR);
		exit(1);
	}
	if(listen(listener, 10) == -1)
	{
		perror(LISTEN_ERROR);
		exit(1);
	}
	else
		delete _pm;
}


//========================================================================================



int Server::addclient(int listener)
{
	Client *nc = new Client();
	std::string s;

	if((nc->clfd = accept(listener, nc->clientaddr, &nc->addrlen)) == -1)
	{
		std::cerr << "Server-accept() error\n";
		return (-1);
	}
	else
	nc->host = inet_ntoa(((struct sockaddr_in*)nc->clientaddr)->sin_addr);
	s = ft_utoa(nc->clfd);
	_m_pclients[s] = nc;
	_m_fdprefix[nc->clfd] = s;
	return (nc->clfd);
}

void Server::getIP()
{
	struct sockaddr_in serv;
	const char* google_dns_server = "8.8.8.8";
	int dns_port = 53;
	int sock = socket ( AF_INET, SOCK_DGRAM, 0);

	memset( &serv, 0, sizeof(serv) );
	serv.sin_family = AF_INET;
	serv.sin_addr.s_addr = inet_addr( google_dns_server );
	serv.sin_port = htons( dns_port );
	int err = connect( sock , (const struct sockaddr*) &serv , sizeof(serv) );
	struct sockaddr_in name;
	socklen_t namelen = sizeof(name);
	err = getsockname(sock, (struct sockaddr*) &name, &namelen);
	const char* p = inet_ntoa(name.sin_addr);
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

	to_send += (prefix +  " " + ccmd + " " + cl->nickname + " " + msg_rpl(s, code, cl) + "\r\n");
	send(cl->clfd, to_send.c_str(), to_send.length(), 0);
}

void Server::send_reply_broad(Client *cl, std::vector<Client*> & v_cl, int code, Message *msg)
{
	std::string s;

	for (size_t i = 0; i < v_cl.size(); i++)
	{
		if (v_cl[i]->clfd != cl->clfd)
		{
			if (code != -1)
				send_reply("", v_cl[i], code);
			else
			{
				for(size_t i = 0; i < msg->params.size(); i++)
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

	if (msg->command == "PASS") {
		passcmd(msg, fd);
	}
	else if (msg->command == "NICK")
	{
		if (_m_fdprefix.count(fd) != 0)
			nickcmd(msg, fd);
		else if (_m_pclients[_m_fdprefix[fd]]->password != _password)
			send_reply("", cl, ERR_PASSWDMISMATCH);
		else
			nickcmd(msg, fd);
	}
	else if (msg->command == "USER" ) {
		if (_m_pclients[_m_fdprefix[fd]]->password != _password)
			send_reply("", cl, ERR_PASSWDMISMATCH);
		else if (_m_pclients[_m_fdprefix[fd]]->nickname[0] == 0)
			send_reply("", cl, ERR_NONICKNAMEGIVEN);
		else
			usercmd(msg, fd);
	}
	else if (_m_pclients.count(_m_fdprefix[fd]) &&
			 _m_pclients[_m_fdprefix[fd]]->is_register == true) {
		if (msg->command == "JOIN")
			joincmd(msg, cl);
		else if (msg->command == "PART")
			partcmd(msg, cl);
		else if (msg->command == "NAMES")
			namescmd(msg, cl);
		else if (msg->command == "LIST")
			listcmd(msg, cl);
		else if (msg->command == "MODE")
			modecmd(msg, cl);
		else if (msg->command == "PRIVMSG" || msg->command == "NOTICE")
			privmsgcmd(msg, cl, msg->command);
		else if (msg->command == "QUIT")
			quitcmd(msg, cl);
		else if (msg->command == "VERSION")
			versioncmd(msg, cl);
		else if (msg->command == "TIME")
			timecmd(msg, cl);
		else if (msg->command == "INFO")
			infocmd(msg, cl);
		else if (msg->command == "WHO")
			whocmd(msg, cl);
		else if (msg->command == "WHOWAS")
			whowascmd(msg, cl);
		else if (msg->command == "PING")
			pingcmd(msg, cl);
		else if (msg->command == "WHOIS")
			whoiscmd(msg, cl);
		else if (msg->command == "TOPIC")
			topiccmd(msg, cl);
		else if (msg->command == "KICK")
			kickcmd(msg, cl);
	}
	else
		send_reply("", cl, ERR_NOTREGISTERED);
	delete msg;
}




//===============================================================================
