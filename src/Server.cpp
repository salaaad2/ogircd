/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbajrami <tbajrami@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/13 14:12:23 by tbajrami          #+#    #+#             */
/*   Updated: 2021/05/31 17:07:20 by tbajrami         ###   ########lyon.fr   */
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
	FD_SET(net_socket, &_fds->master);
	FD_SET(net_socket, &_fds->read);
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

void Server::send_reply(std::string s, Client *cl, int code)
{
	std::string ccmd;
	std::string to_send;
	std::string prefix;
	if (code)
		ccmd = ft_format_cmd(ft_utoa(code));
	// if (!cl->is_server)
	// {
	// 	prefix += "[" + ft_current_time();
	// 	prefix.erase(prefix.size() - 1, 1);
	// 	prefix += + "]:";
	// }

	prefix = ":";
	prefix += _ip;

	to_send += (prefix +  " " + ccmd + " " + cl->nickname + " " + msg_rpl(s, code, cl) + "\r\n");
	std::cout << "send to client : " << to_send;
	// send(_m_pclients[prefix]->clfd, to_send.c_str(), strlen(to_send.c_str()), 0);
	// to_send += (prefix +  " " + ccmd + " " + cl->nickname + " " + msg_rpl(s, code, cl) + "\r\n");
	// std::cout << "send to client : " << to_send;
	send(cl->clfd, to_send.c_str(), strlen(to_send.c_str()), 0);
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

void Server::chan_msg(Message * msg, Client *cl) {
	std::string nick = cl->nickname;
	std::string s;

	s += ("<" + _m_nickdb[nick].top()->nickname + ">@["+ cl->current_chan + "] : " += msg->command + " ");
	msg->params.insert(msg->params.begin(), s);
	send_reply_broad(cl, _m_chans[cl->current_chan], -1, msg);
}

void Server::do_command(Message *msg, int fd)
{
	std::cout << "FD [" << fd << "] says " << msg->command << "\n";
	std::string req;
	Client *cl = _m_pclients[_m_fdprefix[fd]];

	if (msg->command == "PASS") {
		passcmd(msg, fd);
	}
	else if (msg->command == "SERVER")
	{
		if (_m_fdserver.count(fd) == 0 && _m_pclients[_m_fdprefix[fd]]->password != _peer_password)
		{
			req = msg_rpl("", ERR_PASSWDMISMATCH, NULL);
			send(fd, req.c_str(), strlen(req.c_str()), 0);
		}
		else if (_m_fdserver.count(fd) == 1)
		{
			req = msg_rpl("", ERR_ALREADYREGISTERED, NULL);
			send(fd, req.c_str(), strlen(req.c_str()), 0);
		}
		else
			servercmd(msg, NULL, fd);
	}
	else if (cl->is_server == false)
	{
		if (msg->command == "NICK")
		{
			if (_m_fdserver.count(fd) != 0)
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
			else if (msg->command == "PRIVMSG")
				privmsgcmd(msg, cl);
			else if (msg->command == "NOTICE")
				noticecmd(msg, cl);
			else if (msg->command == "QUIT")
				quitcmd(msg, cl);
			else if (msg->command == "VERSION")
				versioncmd(msg, cl);
			else if (msg->command == "STATS")
				statscmd(msg, cl);
			else if (msg->command == "TIME")
				timecmd(msg, cl);
			else if (msg->command == "INFO")
				infocmd(msg, cl);
			else if (msg->command == "WHO")
				whocmd(msg, cl);
			else if (msg->command == "CONNECT")
				connectcmd(msg, cl);
			else
				send_reply(msg->command, cl, ERR_NOTOCHANNEL);
		}
	}
	else
		send_reply("", cl, ERR_NOTREGISTERED);
	delete msg;
}




//===============================================================================
