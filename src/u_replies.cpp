#include "../inc/replies.hpp"
#include "../inc/Server.hpp"

string Server::msg_rpl(string s, int code, Client *cl)
{
	string response;
	std::map<string, e_commands>::iterator it;

	/* REPLIES */
	switch (code) {

	case RPL_WELCOME:
		return string("Welcome to the Internet Relay Network " +
				   cl->prefix);
	case RPL_YOURHOST:
		return (":Your host is " + string(_ip) +
			" running ogircd");
	case RPL_TOPIC:
		return (s + " :" + _m_topics[s]);
	case RPL_NAMREPLY: {
		response += "= " + s + " :";
		for (size_t i = 0; i < _m_chans[s].size(); i++) {
			if (_m_uflags[s][_m_chans[s][i]].find('o') !=
			    string::npos)
				response += "@";
			else if (_m_uflags[s][_m_chans[s][i]].find('v') !=
				 string::npos)
				response += "+";
			response += _m_chans[s][i]->nickname;
			response += " ";
		}
		return response;
	}
	case RPL_INFO:
		return (msg_rpl(s, RPL_VERSION, cl) + " " +
			msg_rpl(s, RPL_STATSUPTIME, cl));
	case RPL_ENDOFINFO:
		return (":End of /INFO list");
	case RPL_ENDOFNAMES:
		return (s + " :End of /NAMES list");
	case RPL_SUMMONING:
		return ("You have been summoned");
	case RPL_VERSION:
		return (":Version is 0.1.3"); // UPDATE ON RELEASE
	case RPL_STATSCOMMANDS:
		for (it=c_map.begin(); it!=c_map.end(); ++it) {
			if (it != c_map.begin()) {
				response += ":";
				response += _ip;
			}
			response += " ";
			response += (*it).first;
			response += " 0 0 0";
			response += "\n";
		}
		return (":commands : " + response);
	case RPL_STATSLLINE:
		return (":Connections: " + u_listusers());
	case RPL_STATSUPTIME:
		return (":Up " + u_uptime(_launch_time));
	case RPL_TIME:
		return (u_current_time());
	case RPL_ENDOFSTATS:
		return (":End of /STATS report");
	case RPL_WHOREPLY:
		return (s);
	case RPL_ENDOFWHO:
		return (s + ":End of WHO list");
	case RPL_WHOISUSER:
		return (s);
	case RPL_WHOISOPERATOR:
		return (s + ": is an IRC operator");
	case RPL_WHOISIDLE:
		return (s);
	case RPL_WHOISCHANNELS:
		return (s);
	case RPL_ENDOFWHOIS:
		return (s + ":End of /WHOIS list");
	case RPL_WHOWASUSER:
		return (s);
	case RPL_ENDOFWHOWAS:
		return (s + ":End of /WHOWAS list");
	case RPL_WHOISSERVER:
		return (s);
	case RPL_LISTSTART:
		return (string() + "Channel :Users  Name");
	case RPL_LIST:
		return (s + " :" + _m_topics[s]);
	case RPL_LISTEND:
		return (string() + ":End of /LIST");
	case RPL_CHANNELMODEIS:
		return (s);
	case RPL_BANLIST:
		return (s);
	case RPL_ENDOFBANLIST:
		return (s + " :End of channel ban list");
	case RPL_EXCEPTLIST:
		return (s);
	case RPL_ENDOFEXCEPTLIST:
		return (s + " :End of channel exception list");
	case RPL_NOTOPIC:
		return (s + " :No topic is set");

		/* ERRORS */

	case ERR_ALREADYREGISTERED:
		return string(s + ":You may not reregister");
	case ERR_NICKNAMEINUSE:
		return string(s + ":Nickname already in use");
	case ERR_USERONCHANNEL:
		return string(s + ":User already on channel");
	case ERR_NOTREGISTERED:
		return string(
			s + ":You need to register before trying this command");
	case ERR_PASSWDMISMATCH:
		return string(s + ":Password doesn't match the server's");
	case ERR_NONICKNAMEGIVEN:
		return string(s + ":No nickname given");
	case ERR_ERRONEUSNICKNAME:
		return string(s + ":Erroneus nickname");
	case ERR_NEEDMOREPARAMS:
		return string(s + ":Insufficient parameter count");
	case ERR_USERSDONTMATCH:
		return string(s + ":Usernames don't match");
	case ERR_NOSUCHNICK:
		return string(s + ":No such nick/channel");
	case ERR_WASNOSUCHNICK:
		return string(s + ":There was no such nick");
	case ERR_NOTEXTTOSEND:
		return string(s + ":There is no text to send");
	case ERR_NOTOCHANNEL:
		return string(s + ":User not on channel");
	case ERR_BADCHANMASK:
		return string(":Invalid JOIN parameter" + s);
	case ERR_UNKNOWNMODE:
		return string(s + ":is unknown mode char to me");
	case ERR_NOSUCHCHANNEL:
		return string(s + ":No such channel");
	case ERR_CHANOPRIVSNEEDED:
		return string(":You are not channel operator");
	case ERR_BADCHANNELKEY:
		return string(
			s + ":Cannot join channel (+k) -- Wrong channel key");
	case ERR_BANNEDFROMCHAN:
		return string(
			s + ":Cannot join channel (+b) -- You are banned");
	case ERR_CHANNELISFULL:
		return string(
			s +
			":Cannot join channel (+l) -- Channel is full, try later");
	case ERR_INVITEONLYCHAN:
		return string(
			s + ":Cannot join channel (+i) -- Invited users only");
	case ERR_NORECIPIENT:
		return string(s + ":No recipient given (PRIVMSG)");
	case ERR_CANNOTSENDTOCHAN:
		return string(s + ":Cannot send to channel");
	default:
		return s;
	}
}
