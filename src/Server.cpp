/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbajrami <tbajrami@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/13 14:12:23 by tbajrami          #+#    #+#             */
/*   Updated: 2021/05/27 15:13:46 by tbajrami         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"

#include <netdb.h>
#include <errno.h>

void Server::setFds(Fds *fds) {_fds = fds;}

Fds *Server::getFds() const { return (_fds);}


Server::Server(Params *pm)
{
	time(&_launch_time);
	_pm = pm;
	_peer_password = "PeerSecret";
	_servername = "42lyon.irc.fr";

	if (pm->isnew())
		new_serv();
	else
		connect_serv();
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
		std::cout << SETSOCK_ERROR << std::endl;;
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
	if (!_pm->getHost().empty() && _pm->getPortNetwork() && !_pm->getPwdNetwork().empty())
	{
		std::cout << "conn_serv()" << std::endl;
		connect_serv();
	}
	else
		delete _pm;
}

// TODO : while res; res = res->ai_next
// inet_ntop (res->ai_family, ptr, addrstr, 100);
int Server::connect_serv()
{
	struct addrinfo hints, *res, *result;
	sockaddr_in server_address;
	int errcode, status, net_socket;

	memset(&hints, 0, sizeof (hints));
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags |= AI_CANONNAME;
	errcode = getaddrinfo(_pm->getHost().c_str(), NULL, &hints, &result);
	if (errcode != 0)
	{
		std::cerr << "Error: getaddrinfo on \'" << _pm->getHost() << "\' failed\n\n";
		return(-1) ;
	}
	res = result;
	if (res->ai_family == AF_INET ||
			res->ai_family==AF_INET6)
		net_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	else {
		std::cerr << "Error: wrong hostname\n\n";
		return(-1) ;
	}
	server_address.sin_family = res->ai_family;
	server_address.sin_addr.s_addr = inet_addr(_pm->getHost().c_str());
	server_address.sin_port = htons(_pm->getPortNetwork());
	FD_SET(net_socket, &_fds->master);
	if (net_socket != -1)
	{

		std::cout << "CONNECTION ON  " << _pm->getHost() << " ON PORT " << _pm->getPortNetwork() << "\n";
		status = connect(net_socket, (struct sockaddr *)&server_address, sizeof(server_address));
		if (status != 0)
		{
			std::cerr << "Error: connection to the remote socket failed: " << strerror(errno) << "\n";
			return(-1);
		}
	}
	else {
		std::cout << "Error: socket failed to open" << std::endl;
		return (-1);
	}
	if (net_socket > _fds->fdmax)
		_fds->fdmax = net_socket;
	freeaddrinfo(res);
	return (net_socket);
}

//========================================================================================



int Server::addclient(int listener)
{
	Client *nc = new Client();
	std::string s;

	std::cout << "ADDCLIENT\n";
	if((nc->clfd = accept(listener, nc->clientaddr, &nc->addrlen)) == -1)
	{
		std::cout << "Server-accept() error\n";
		return (-1);
	}
	else
		std::cout << "Server-accept() is OK...\n";
	std::cout << "New connection from " << inet_ntoa(((struct sockaddr_in*)nc->clientaddr)->sin_addr);
	nc->host = inet_ntoa(((struct sockaddr_in*)nc->clientaddr)->sin_addr);
	std::cout << " on socket " << nc->clfd << std::endl;
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
	if(p != NULL)
	{
		std::cout << "Local ip is : " << p << "\n";
	}
	strcpy(_ip, p);
}




//==============================TREAT COMMANDS======================================

void Server::send_reply(std::string s, std::string prefix, int code)
{
	std::string ccmd;
	std::string to_send;

	if (code)
		ccmd = ft_format_cmd(ft_utoa(code));
	_prefix = BOLDWHITE;
	_prefix += "[" + ft_current_time();
	_prefix.erase(_prefix.size() - 1, 1);
	_prefix += + "]:";

	to_send += (_prefix +  " " + ccmd + " " + msg_rpl(s, code, prefix) + RESET + "\r\n");
	send(_m_pclients[prefix]->clfd, to_send.c_str(), strlen(to_send.c_str()), 0);
}

void Server::send_reply_broad(std::string prefix, std::vector<Client*> & cl, int code, Message *msg)
{
	std::string s;

	for (size_t i = 0; i < cl.size(); i++)
	{
		if (cl[i]->clfd != _m_pclients[prefix]->clfd)
		{
			if (code != -1)
				send_reply("", cl[i]->prefix, code);
			else
			{
				for(size_t i = 0; i < msg->params.size(); i++)
					s += msg->params[i];
				send_reply(s, cl[i]->prefix, 0);
			}
		}
	}
}

void Server::chan_msg(Message * msg, std::string prefix) {
	std::string nick = _m_pclients[prefix]->nickname;
	std::string s;

	s += ("<" + _m_nickdb[nick].top()->nickname + ">@["+ _m_pclients[prefix]->current_chan + "] : " += msg->command + " ");
	msg->params.insert(msg->params.begin(), s);
	send_reply_broad(prefix, _m_chans[_m_pclients[prefix]->current_chan], -1, msg);
}

void Server::do_command(Message *msg, int fd)
{
	std::string req;

	if (msg->command == "PASS") {
		passcmd(msg, fd);
	}
	else if (msg->command == "SERVER")
	{
		if (_m_fdserver.count(fd) == 0 && _m_pclients[_m_fdprefix[fd]]->password != _peer_password)
		{
			req = msg_rpl("", ERR_PASSWDMISMATCH, "");
			send(fd, req.c_str(), strlen(req.c_str()), 0);
		}
		else if (_m_fdserver.count(fd) == 1)
		{
			req = msg_rpl("", ERR_ALREADYREGISTERED, "");
			send(fd, req.c_str(), strlen(req.c_str()), 0);
		}
		else
			servercmd(msg, "", fd);
	}
	else if (msg->command == "NICK")
	{
		if (_m_fdserver.count(fd) != 0)
			nickcmd(msg, fd);
		else if (_m_pclients[_m_fdprefix[fd]]->password != _password)
			send_reply("", _m_fdprefix[fd], ERR_PASSWDMISMATCH);
		else
			nickcmd(msg, fd);
	}
	else if (msg->command == "USER" ) {
		if (_m_pclients[_m_fdprefix[fd]]->password != _password)
			send_reply("", _m_fdprefix[fd], ERR_PASSWDMISMATCH);
		else if (_m_pclients[_m_fdprefix[fd]]->nickname[0] == 0)
			send_reply("", _m_fdprefix[fd], ERR_NONICKNAMEGIVEN);
		else
			usercmd(msg, fd);
	}
	else if (_m_pclients.count(_m_fdprefix[fd]) && _m_pclients[_m_fdprefix[fd]]->is_register == true)
	{
		if (msg->command == "JOIN")
			joincmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);

		else if (msg->command == "NAMES")
			namescmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);
		else if (msg->command == "LIST")
			listcmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);
		else if (msg->command == "MODE")
			modecmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);
		else if (msg->command == "PRIVMSG")

			privmsgcmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);
		else if (msg->command == "NOTICE")
			noticecmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);
		else if (msg->command == "QUIT")
			quitcmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);
		else if (msg->command == "VERSION")
			versioncmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);
		else if (msg->command == "STATS")
			statscmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);
		else if (msg->command == "TIME")
			timecmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);
		else if (msg->command == "INFO")
			infocmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);
		else if (msg->command == "WHO")
			whocmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);
		else if (msg->command == "CONNECT")
			connectcmd(msg, _m_pclients[_m_fdprefix[fd]]->prefix);
		else if (_m_pclients[_m_fdprefix[fd]]->current_chan.empty() == false)
			chan_msg(msg, _m_pclients[_m_fdprefix[fd]]->prefix); // TODO: cadegage
		else
			send_reply(msg->command, _m_fdprefix[fd], ERR_NOTOCHANNEL);
	}
	else if (_m_fdserver.count(fd) == 0)
		send_reply("", _m_fdprefix[fd], ERR_NOTREGISTERED);
	delete msg;
}




//===============================================================================
