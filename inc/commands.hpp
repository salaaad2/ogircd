#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <iostream>

using std::string;

enum e_commands {
    PASS,
    NICK,
    USER,
    JOIN,
    INVITE,
    PART,
    NAMES,
    LIST,
    MODE,
    PRIVMSG,
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

string getStrFromEnum(e_commands e)
{
    switch (e) {
    case PASS:
        return ("PASS");
    case NICK:
        return ("NICK");
    case USER:
        return ("USER");
    case JOIN:
        return ("JOIN");
    case INVITE:
        return ("INVITE");
    case PART:
        return ("PART");
    case NAMES:
        return ("NAMES");
    case LIST:
        return ("LIST");
    case MODE:
        return ("MODE");
    case PRIVMSG:
        return ("PRIVMSG");
    case QUIT:
        return ("QUIT");
    case VERSION:
        return ("VERSION");
    case TIME:
        return ("TIME");
    case INFO:
        return ("INFO");
    case WHO:
        return ("WHO");
    case WHOWAS:
        return ("WHOWAS");
    case PING:
        return ("PING");
    case WHOIS:
        return ("WHOIS");
    case TOPIC:
        return ("TOPIC");
    case KICK:
        return ("KICK");
    case SHUTDOWN:
        return ("SHUTDOWN");
    }
}

#endif // COMMANDS_H_
