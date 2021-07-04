#include "../inc/ftirc.hpp"
#include "../inc/Server.hpp"

void Server::passcmd(Message *msg, int fd)
{
	string s = u_utoa(fd);

	if (_m_pclients[s]->is_register == true) {
		send_reply("", _m_pclients[_m_fdprefix[fd]],
			   ERR_ALREADYREGISTERED);
	} else if (!msg->params[0][0]) {
		send_reply("", _m_pclients[_m_fdprefix[fd]],
			   ERR_NEEDMOREPARAMS);
	} else {
		_m_pclients[s]->password = msg->params[0];
	}
}

void Server::nickcmd(Message *msg, int fd)
{
	string s = u_utoa(fd);

	if (msg->params[0].empty() == 1) {
		send_reply(msg->params[0], _m_pclients[_m_fdprefix[fd]],
			   ERR_NONICKNAMEGIVEN);
		return;
	} else if (msg->params[0].size() > 9) {
		msg->params[0].resize(9);
		send_reply(msg->params[0], _m_pclients[_m_fdprefix[fd]],
			   ERR_ERRONEUSNICKNAME);
		return;
	} else if (_m_nickdb.count(msg->params[0]) == 1 &&
		   _m_nickdb[msg->params[0]].top()->is_logged == true) {
		send_reply(msg->params[0], _m_pclients[_m_fdprefix[fd]],
			   ERR_NICKNAMEINUSE);
		return;
	} else {
		if (_m_fdprefix[fd] != s) {
			s = ":" + _m_fdprefix[fd] + " NICK " + ":" +
			    msg->params[0] + "\r\n";
			send(fd, s.c_str(), strlen(s.c_str()), 0);
			for (std::vector<string>::iterator it =
				     _m_pclients[_m_fdprefix[fd]]->chans.begin();
			     it != _m_pclients[_m_fdprefix[fd]]->chans.end();
			     it++)
				send_to_channel(s, *it, NULL);
			_m_nickdb[_m_pclients[_m_fdprefix[fd]]->nickname]
				.top()
				->is_logged = false;
			_m_pclients[_m_fdprefix[fd]]->nickname = msg->params[0];
			create_client_prefix(fd);
		} else
			_m_pclients[s]->nickname = msg->params[0];
	}
}

void Server::usercmd(Message *msg, int fd)
{
	string s = u_utoa(fd);

	if (_m_pclients[s]->is_register == true)
		send_reply("", _m_pclients[_m_fdprefix[fd]],
			   ERR_ALREADYREGISTERED);
	else if (msg->params.size() == 8) {
		send_reply("", _m_pclients[_m_fdprefix[fd]],
			   ERR_NEEDMOREPARAMS);
	} else {
		_m_pclients[s]->username = msg->params[0].c_str();
		_m_pclients[s]->realname = msg->params[msg->params.size() - 1];
		do_registration(fd);
	}
}

void Server::do_registration(int fd)
{
	string s = u_utoa(fd);

	if (_m_pclients[s]->nickname[0] &&
	    !_m_pclients[s]->password.compare(_password)) {
		create_client_prefix(fd);
		_m_pclients[_m_fdprefix[fd]]->is_register = true;
		_m_pclients[_m_fdprefix[fd]]->is_logged = true;
		send_reply("", _m_pclients[_m_fdprefix[fd]], RPL_WELCOME);
		send_reply("", _m_pclients[_m_fdprefix[fd]], RPL_YOURHOST);
	}
}

void Server::create_client_prefix(int fd)
{
	string prefix;
	string s;

	s = _m_fdprefix[fd];
	prefix += _m_pclients[s]->nickname;
	prefix += "!";
	prefix += _m_pclients[s]->username;
	prefix += "@";
	prefix += _m_pclients[s]->host;
	if (_m_pclients.count(prefix) == 0)
		_m_pclients[prefix] = _m_pclients[s];
	else {
		_m_pclients[prefix]->is_logged = true;
		_m_pclients[prefix]->clfd = _m_pclients[s]->clfd;
	}
	_m_pclients[prefix]->prefix = prefix;
	_m_pclients.erase(_m_fdprefix[fd]);
	_m_fdprefix[fd] = prefix;
	_m_nickdb[_m_pclients[prefix]->nickname].push(_m_pclients[prefix]);
}
