#ifndef __MESSAGE_H_
#define __MESSAGE_H_

#include <vector>
#include <string>

struct Message
{
    std::string command;
    std::vector<std::string> params;

    Message() : command(), params() {}
};

const char    *msg_rpl(int code);
Message *parse_message(char buf[]);

#endif // __MESSAGE_H_
