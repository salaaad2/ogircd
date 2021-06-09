#include "../inc/Server.hpp"

void Server::privmsgcmd(Message *msg, Client *cl, bool reply)
{
	// ERR_NORECIPIENT --NO                ERR_NOTEXTTOSEND -- Yes
	// ERR_CANNOTSENDTOCHAN --No            ERR_NOTOPLEVEL -- No
	// ERR_WILDTOPLEVEL --No                ERR_TOOMANYTARGETS --No
	// ERR_NOSUCHNICK --Yes
	//  RPL_AWAY --No
	std::list<std::string> chans;
	std::list<Client *> nicknames;
	std::string curr_chan_tmp;
	std::string dests;
	std::string c_dest;
	std::string text;
	Client *cl_tmp;
	size_t i = 0;

	if (msg->params.size() == 0 && reply)
		return (send_reply(c_dest, cl, ERR_NORECIPIENT));
	else if (msg->params.size() == 1 && reply)
		return (send_reply(c_dest, cl, ERR_NOTEXTTOSEND));
	else {
		dests = msg->params[0];
		text = msg->params[1];
	}
	while (i < dests.size() + 1) {
		if (dests[i] && dests[i] != ',')
			c_dest += dests[i];
		else if (c_dest[0] == '#' || c_dest[0] == '&') {
			if (_m_chans.count(c_dest) == 1)
				chans.push_back(c_dest);
			else if (reply)
				send_reply(c_dest, cl, ERR_NOSUCHCHANNEL);
			c_dest.clear();
		} else {
			if (_m_nickdb.count(c_dest) == 1) {
				cl_tmp = _m_nickdb[c_dest].top();
				if (cl_tmp->is_logged == true)
					nicknames.push_back(cl_tmp);
			} else if (reply)
				send_reply(msg->params[i], cl, ERR_NOSUCHNICK);
			c_dest.clear();
		}
		i++;
	}
	nicknames.sort();
	nicknames.unique();
	chans.sort();
	chans.unique();
	curr_chan_tmp = cl->current_chan;
	for (std::list<std::string>::iterator it = chans.begin();
	     it != chans.end(); it++) {
		cl->current_chan = *it;
		if ((isNickonchan(cl->nickname, *it) &&
		     (_m_flags[*it].find("m") == std::string::npos ||
		      (_m_uflags[*it][cl].find("v") != std::string::npos ||
		       _m_uflags[*it][cl].find("o") != std::string::npos))) &&
		    !isbanned(cl, *it))
			send_to_channel(":" + cl->prefix + " " + msg->command +
						" " + *it + " " + text + "\r\n",
					*it, cl);
		else if (reply)
			send_reply(*it, cl, ERR_CANNOTSENDTOCHAN);
	}
	cl->current_chan = curr_chan_tmp;
	std::vector<Client *> vec(nicknames.begin(), nicknames.end());
	std::string to_send;
	for (std::vector<Client *>::iterator it = vec.begin(); it != vec.end();
	     it++) {
		to_send = ":" + cl->prefix + " " + msg->command + " " +
			  (*it)->nickname + " " + text + "\r\n";
		send((*it)->clfd, to_send.c_str(), to_send.length(), 0);
	}
}
