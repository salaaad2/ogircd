#ifndef __MESSAGE_H_
#define __MESSAGE_H_

#include <vector>
#include <string>

struct Message
{
    std::string command;
    std::vector<std::string> params;
    size_t count;
    size_t len;

    Message() : command(), params() {}
};

std::vector<Message*> parse_message(char buf[]);

#endif // __MESSAGE_H_
