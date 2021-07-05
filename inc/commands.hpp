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
	STATS,
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

#endif // COMMANDS_H_
