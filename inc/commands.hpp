#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <iostream>
#include <map>

using std::string;

enum e_commands {
	JOIN,
	INVITE,
	PART,
	NAMES,
	LIST,
	MODE,
	PRIVMSG,
	NOTICE,
	QUIT,
	VERSION,
	TIME,
	INFO,
	WHO,
	WHOWAS,
	PING,
	WHOIS,
	TOPIC,
	KICK,
	SHUTDOWN
};

void initcmdmap(std::map<std::string, e_commands> *c_map)
{
	(*c_map)["JOIN"] = JOIN;
	(*c_map)["INVITE"] = INVITE;
	(*c_map)["PART"] = PART;
	(*c_map)["NAMES"] = NAMES;
	(*c_map)["LIST"] = LIST;
	(*c_map)["MODE"] = MODE;
	(*c_map)["PRIVMSG"] = PRIVMSG;
	(*c_map)["NOTICE"] = NOTICE;
	(*c_map)["QUIT"] = QUIT;
	(*c_map)["VERSION"] = VERSION;
	(*c_map)["TIME"] = TIME;
	(*c_map)["INFO"] = INFO;
	(*c_map)["WHO"] = WHO;
	(*c_map)["WHOWAS"] = WHOWAS;
	(*c_map)["PING"] = PING;
	(*c_map)["WHOIS"] = WHOIS;
	(*c_map)["TOPIC"] = TOPIC;
	(*c_map)["KICK"] = KICK;
	(*c_map)["SHUTDOWN"] = SHUTDOWN;
}

#endif // COMMANDS_H_
