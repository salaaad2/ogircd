#ifndef __CLIENT_H_
#define __CLIENT_H_

#include <string>
#include <vector>
#include <arpa/inet.h>

using std::string;

struct Client {
	std::vector<string> chans;
	string current_chan;
	sockaddr *clientaddr;
	unsigned int addrlen;
	int clfd;
	string password;
	string nickname;
	string username;
	string realname;
	string host;
	string prefix;
	bool is_register;
	bool is_logged;

	Client(void)
		: chans(), current_chan(), clientaddr(new sockaddr),
		  addrlen(sizeof(clientaddr)), clfd(), password(), nickname(),
		  username(), realname(), host(), prefix(), is_register(false),
		  is_logged(false)
	{
	}
	~Client(void)
	{
		delete clientaddr;
	}
	Client &operator=(const Client &cl)
	{
		chans = cl.chans;
		current_chan = cl.current_chan;
		clientaddr = cl.clientaddr;
		addrlen = cl.addrlen;
		password = cl.password;
		nickname = cl.nickname;
		username = cl.username;
		realname = cl.realname;
		host = cl.host;
		prefix = cl.prefix;
		is_register = cl.is_register;
		is_logged = cl.is_logged;
		clfd = cl.clfd;
		return (*this);
	}

    public:
	bool operator==(const Client &rhs) const
	{
		return (clfd == rhs.clfd);
	}
	bool operator>(const Client &rhs) const
	{
		return (clfd > rhs.clfd);
	}
	bool operator<(const Client &rhs) const
	{
		return (clfd < rhs.clfd);
	}
	bool operator>=(const Client &rhs) const
	{
		return (clfd >= rhs.clfd);
	}
	bool operator<=(const Client &rhs) const
	{
		return (clfd <= rhs.clfd);
	}
	bool operator!=(const Client &rhs) const
	{
		return (clfd != rhs.clfd);
	}
};

#endif // __CLIENT_H_
