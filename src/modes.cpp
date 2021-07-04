#include "../inc/Server.hpp"

void Server::send_to_channel(string to_send, string &chan, Client *cl)
{
	for (std::vector<Client *>::iterator it = _m_chans[chan].begin();
	     it != _m_chans[chan].end(); it++) {
		if (cl == NULL)
			send((*it)->clfd, to_send.c_str(), to_send.length(), 0);
		else if (cl != *it)
			send((*it)->clfd, to_send.c_str(), to_send.length(), 0);
	}
}

void Server::modecmd(Message *msg, Client *cl)
{
	if (msg->params.empty())
		send_reply("", cl, ERR_NEEDMOREPARAMS);
	else if (msg->params[0][0] == '#' || msg->params[0][0] == '&')
		chanMode(msg->params, cl);
	// else
	//     userMode(msg->params, cl);
}

void Server::chanMode(std::vector<string> params, Client *cl)
{
	string chan = params[0];

	if (_m_chans.find(params[0]) == _m_chans.end())
		send_reply(params[0] + params[1], cl, ERR_NOSUCHCHANNEL);
	else if (_m_uflags[chan][cl].find('o') == string::npos)
		send_reply(chan, cl, ERR_CHANOPRIVSNEEDED);
	else
		setChanMode(params, cl);
}

void Server::setChanMode(std::vector<string> params, Client *cl)
{
	std::vector<size_t> par;
	std::vector<string> cmds;
	string c_par;
	bool ar = true;

	string modes = "imqpsrt";
	string pmodes = "OovklbeI";

	for (size_t j = 1; j < params.size(); j++)
		c_par += params[j] + ' ';

	for (size_t j = 0; j < c_par.size(); j++) {
		if (c_par[j] == ' ')
			;
		else if (c_par[j] == '+')
			ar = true;
		else if (c_par[j] == '-')
			ar = false;
		else if (ar && (modes.find(c_par[j]) != string::npos))
			cmds.push_back(string("+") + c_par[j]);
		else if (ar && (pmodes.find(c_par[j]) != string::npos)) {
			cmds.push_back(string("+") + c_par[j] + " ");
			size_t k = j;
			size_t begin;
			size_t end;

			while (c_par[k] && c_par[k] != ' ')
				k++;
			while (c_par[k] && c_par[k] == ' ')
				k++;
			begin = k;
			while (c_par[k] && c_par[k] != ' ') {
				cmds.back() += c_par[k];
				k++;
			}
			end = k;
			if (begin != end)
				c_par.erase(begin, end - begin);
		} else if (!ar && (modes.find(c_par[j]) != string::npos))
			cmds.push_back(string("-") + c_par[j]);
		else if (!ar && (pmodes.find(c_par[j]) != string::npos)) {
			cmds.push_back(string("-") + c_par[j] + " ");
			size_t k = j;
			size_t begin;
			size_t end;

			while (c_par[k] && c_par[k] != ' ')
				k++;
			while (c_par[k] && c_par[k] == ' ')
				k++;
			begin = k;
			while (c_par[k] && c_par[k] != ' ') {
				cmds.back() += c_par[k];
				k++;
			}
			end = k;
			if (begin != end)
				c_par.erase(begin, end - begin);
		} else if (modes.find(c_par[j]) == string::npos ||
			   pmodes.find(c_par[j]) == string::npos)
			cmds.push_back(string(".") + c_par[j]);
	}
	treat_modes(params, cmds, cl);
}

void Server::treat_modes(std::vector<string> params,
			 std::vector<string> cmds, Client *cl)
{
	string cmd;
	bool ar;
	string chan = params[0];

	for (size_t i = 0; i < cmds.size(); i++) {
		if (cmds[i][0] == '+' && cmds[i].size() == 2) {
			if (_m_flags[chan].find(cmds[i][1]) ==
			    string::npos) {
				if (cmd.empty() || !ar) {
					ar = true;
					cmd += "+";
				}
				_m_flags[chan].push_back(cmds[i][1]);
				cmd += cmds[i][1];
			}
		} else if (cmds[i][0] == '-' && cmds[i].size() == 2) {
			if (_m_flags[chan].find(cmds[i][1]) !=
			    string::npos) {
				if (cmd.empty() || ar) {
					ar = false;
					cmd += "-";
				}
				_m_flags[chan].erase(
					_m_flags[chan].find(cmds[i][1]), 1);
				cmd += cmds[i][1];
			}
		} else if (cmds[i][0] == '+' || cmds[i][0] == '-')
			treat_args(chan, cmds[i], cl);
	}
	if (!cmd.empty())
		send_to_channel(":" + cl->prefix + " MODE " + chan + " " + cmd +
					"\r\n",
				chan, NULL);
}

void Server::treat_args(string chan, string cmd, Client *cl)
{
	string arg;
	size_t i = 0;

	while (cmd[i] != ' ')
		i++;
	while (cmd[i] == ' ')
		i++;

	if (cmd[i])
		arg = cmd.substr(i);
	if (cmd[1] == 'b' && arg.size()) {
		size_t u1;
		size_t u2;
		string ban;

        u1 = arg.find('!');
        u2 = arg.find('@');
        if (u1 != std::string::npos && u2 != std::string::npos && u1 < u2)
            ban = arg;
        else if (u1 != std::string::npos && u2 == std::string::npos)
            ban = arg + "@*";
        else if (u2 != std::string::npos && u2 < u1)
            ban = arg.substr(0, arg.length() - u2) + "!*" + arg.substr(u2);
        else if (u2 == u1 && u2 == std::string::npos)
            ban = arg + "!*@*";
        for (std::vector<std::string>::iterator it = _m_banmask[chan].begin() ; it != _m_banmask[chan].end() ; it++)
        {
            if (*it == ban && cmd[0] == '-')
            {
                _m_banmask[chan].erase(it);
                send_to_channel(":" + cl->prefix + " MODE " + chan + " -b " + ban + "\r\n", chan, NULL);
                return ;
            }
            if (*it == ban && cmd[0] == '+')
                return ;
        }
        _m_whoban[ban] = cl->nickname;
        _m_banmask[chan].push_back(ban);
        send_to_channel(":" + cl->prefix + " MODE " + chan + " +b " + ban + "\r\n", chan, NULL);
        _m_banid[ban] = reinterpret_cast<uint64_t>(&_m_banmask[chan].back());
        for (std::vector<Client *>::iterator it = _m_chans[chan].begin() ; it != _m_chans[chan].end() ; it++)
        {
            if (u_strmatch((*it)->prefix, ban) && !isexcepted(*it, chan))
            {
				std::string nickname = (*it)->nickname;
                _m_chans[chan].erase(clposition(nickname, chan));
                _m_nickdb[nickname].top()->chans.erase(chposition(_m_nickdb[nickname].top(), chan));
                return ;
            }
        }
    }
    else if (cmd[1] == 'b')
    {
        for (std::vector<std::string>::iterator it = _m_banmask[chan].begin() ; it != _m_banmask[chan].end() ; it++)
            send_reply(chan + " " + *it + " " + _m_whoban[*it] + " " + u_utoa(_m_banid[*it]), cl, RPL_BANLIST);
        send_reply("", cl, RPL_ENDOFBANLIST);
    }
    else if (cmd[1] == 'e' && arg.size())
    {
        size_t u1;
        size_t u2;
        std::string except;

		u1 = arg.find('!');
		u2 = arg.find('@');
		if (u1 != string::npos && u2 != string::npos &&
		    u1 < u2)
			except = arg;
		else if (u1 != string::npos && u2 == string::npos)
			except = arg + "@*";
		else if (u2 != string::npos && u2 < u1)
			except = arg.substr(0, arg.length() - u2) + "!*" +
				 arg.substr(u2);
		else if (u2 == u1 && u2 == string::npos)
			except = arg + "!*@*";
		for (std::vector<string>::iterator it =
			     _m_exceptmask[chan].begin();
		     it != _m_exceptmask[chan].end(); it++) {
			if (*it == except && cmd[0] == '-') {
				_m_exceptmask[chan].erase(it);
				send_to_channel(cl->prefix + " MODE " + chan +
							" -e " + except +
							"\r\n",
						chan, NULL);
				return;
			}
			if (*it == except && cmd[0] == '+')
				return;
		}
		_m_whoexcept[except] = cl->nickname;
		_m_exceptmask[chan].push_back(except);
		send_to_channel(":" + cl->prefix + " MODE " + chan + " +e " +
					except + "\r\n",
				chan, NULL);
		_m_exceptid[except] =
			reinterpret_cast<uint64_t>(&_m_exceptmask[chan].back());
	} else if (cmd[1] == 'e') {
		for (std::vector<string>::iterator it =
			     _m_exceptmask[chan].begin();
		     it != _m_exceptmask[chan].end(); it++)
			send_reply(chan + " " + *it + " " + _m_whoexcept[*it] +
					   " " + u_utoa(_m_exceptid[*it]),
				   cl, RPL_EXCEPTLIST);
		send_reply("", cl, RPL_ENDOFEXCEPTLIST);
	} else if (cmd[1] == 'v' && arg.size()) {
		for (std::vector<Client *>::iterator it =
			     _m_chans[chan].begin();
		     it != _m_chans[chan].end(); it++) {
			if ((*it)->nickname == arg) {
				if (cmd[0] == '+') {
					if (_m_uflags[chan]
						     [_m_pclients[(*it)->prefix]]
							     .find('v') ==
					    string::npos) {
						_m_uflags[chan]
							 [_m_pclients
								  [(*it)->prefix]]
								 .push_back(
									 'v');
						send_to_channel(
							":" + cl->prefix +
								" MODE " +
								chan + " +v " +
								arg + "\r\n",
							chan, NULL);
					}
					return;
				} else if (cmd[0] == '-') {
					if (_m_uflags[chan]
						     [_m_pclients[(*it)->prefix]]
							     .find('v') !=
					    string::npos) {
						_m_uflags[chan][_m_pclients
									[(*it)->prefix]]
							.erase(_m_uflags
								       [chan]
								       [_m_pclients
										[(*it)->prefix]]
									       .find('v'),
							       1);
						send_to_channel(
							":" + cl->prefix +
								" MODE " +
								chan + " -v " +
								arg + "\r\n",
							chan, NULL);
					}
					return;
				}
			}
		}
		send_reply("", cl, ERR_NOSUCHNICK);
	} else if (cmd[1] == 'o' && arg.size()) {
		for (std::vector<Client *>::iterator it =
			     _m_chans[chan].begin();
		     it != _m_chans[chan].end(); it++) {
			if ((*it)->nickname == arg) {
				if (cmd[0] == '+') {
					if (_m_uflags[chan]
						     [_m_pclients[(*it)->prefix]]
							     .find('o') ==
					    string::npos) {
						_m_uflags[chan]
							 [_m_pclients
								  [(*it)->prefix]]
								 .push_back(
									 'o');
						send_to_channel(
							":" + cl->prefix +
								" MODE " +
								chan + " +o " +
								arg + "\r\n",
							chan, NULL);
					}
					return;
				} else if (cmd[0] == '-') {
					if (_m_uflags[chan]
						     [_m_pclients[(*it)->prefix]]
							     .find('o') !=
					    string::npos) {
						_m_uflags[chan][_m_pclients
									[(*it)->prefix]]
							.erase(_m_uflags
								       [chan]
								       [_m_pclients
										[(*it)->prefix]]
									       .find('o'),
							       1);
						send_to_channel(
							":" + cl->prefix +
								" MODE " +
								chan + " -o " +
								arg + "\r\n",
							chan, NULL);
					}
					return;
				}
			}
		}
		send_reply("", cl, ERR_NOSUCHNICK);
	} else if (cmd[1] == 'k') {
		if (cmd[0] == '+' && arg.size()) {
			_m_chankey[chan] = arg;
			if (_m_flags[chan].find('k') == string::npos)
				_m_flags[chan].push_back('k');
			send_to_channel(":" + cl->prefix + " MODE " + chan +
						" +k " + arg + "\r\n",
					chan, NULL);
		} else if (cmd[0] == '-' &&
			   _m_flags[chan].find('k') != string::npos) {
			_m_flags[chan].erase(_m_flags[chan].find('k'), 1);
			_m_chankey[chan] = "";
			send_to_channel(":" + cl->prefix + " MODE " + chan +
						" -k *" + "\r\n",
					chan, NULL);
		}
	} else if (cmd[1] == 'l') {
		if (cmd[0] == '+' && arg.size()) {
			_m_flags[chan].push_back('l');
			_m_limits[chan] = atoi(arg.c_str());
			send_to_channel(":" + cl->prefix + " MODE " + chan +
						" +l " + arg + "\r\n",
					chan, NULL);
		} else if (cmd[0] == '-' &&
			   _m_flags[chan].find('l') != string::npos) {
			_m_flags[chan].erase(_m_flags[chan].find('l'), 1);
			send_to_channel(":" + cl->prefix + " MODE " + chan +
						" -l" + "\r\n",
					chan, NULL);
		}
	}
}
