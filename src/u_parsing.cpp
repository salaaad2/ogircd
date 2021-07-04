#include "../inc/ftirc.hpp"
#include "../inc/Server.hpp"

size_t count_commands(string buf)
{
	string tmp(buf);
	size_t a;
	size_t c;

	c = 0;
	for (a = tmp.find("\r\n"); a != string::npos;
	     a = tmp.find("\r\n", a + 2)) {
		c++;
	}
	return (c);
}

int get_command(string buf, Message *nm, size_t i)
{
	while (buf[i] && buf[i] != ' ' && buf[i] != '\r') {
		nm->command += buf[i];
		i++;
	}
	nm->len += nm->command.length();
	return (i);
}

int get_params(string buf, Message *nm, size_t i)
{
	while (buf[i] && buf[i] != '\r' && buf[i] == ' ')
		i++;
	while (buf[i] && buf[i] != '\r') {
		if (buf[i] == ' ' || buf[i] == ',')
			i++;
		else if (buf[i] != ':') {
			string param;
			while (buf[i] && buf[i] != '\r' && buf[i] != ' ' &&
			       buf[i] != ',') {
				param += buf[i];
				i++;
				nm->len++;
			}
			nm->params.push_back(param);
		} else if (buf[i] == ':') {
			i++;
			string param;
			while (buf[i] && buf[i] != '\r' && buf[i] != ',') {
				param += buf[i];
				nm->len++;
				i++;
			}
			nm->params.push_back(param);
		}
	}
	if (buf[i] == '\r')
		return (i + 2);
	return i;
}

std::vector<Message *> parse_message(string buf)
{
	std::vector<Message *> vm;
	Message *nm = new Message;
	size_t i = 0;
	size_t nc = 0;
	size_t n = 0;

	n = count_commands(buf);
	while (nc < n) {
		if ((i = get_command(buf, nm, i)) == 0) {
			break;
		} else
			i = get_params(buf, nm, i);
		vm.push_back(nm);
		nc++;
		if (nc != n)
			nm = new Message;
	}
	return vm;
}
