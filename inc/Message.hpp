#ifndef __MESSAGE_H_
#define __MESSAGE_H_

#include <vector>
#include <string>

using std::string;

struct Message {
	string command;
	std::vector<string> params;
	size_t count;
	size_t len;

	Message() : command(), params()
	{
	}
};

std::vector<Message *> parse_message(string buf);

#endif // __MESSAGE_H_
