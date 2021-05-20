#ifndef __CLIENT_H_
#define __CLIENT_H_
#include <string>
#include <vector>
#include <arpa/inet.h>

struct Client
{
    std::vector<std::string> chans;
    std::string              current_chan;
    sockaddr_in         clientaddr;
    unsigned int        addrlen;
    int                 clfd;
    std::string         password;
    std::string         nickname;
    std::string         username;
    std::string         realname;
    std::string         host;
    std::string         prefix;
    bool                is_server;
    bool                is_register;
    bool                is_logged;

    Client(void) : chans(), current_chan(), clientaddr(), addrlen(), clfd(), password(), nickname(), username(),
                   realname(), host(), prefix(), is_server(false), is_register(false), is_logged(false) {}
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
        is_server = cl.is_server;
        is_register = cl.is_register;
        is_logged = cl.is_logged;
    }

    public:
        bool operator==(const Client &rhs) const {
            return (clfd == rhs.clfd);
        }
        bool operator>(const Client &rhs) const {
            return (clfd > rhs.clfd);
        }
        bool operator<(const Client &rhs) const {
            return (clfd < rhs.clfd);
        }
        bool operator>=(const Client &rhs) const {
            return (clfd >= rhs.clfd);
        }
        bool operator<=(const Client &rhs) const {
            return (clfd <= rhs.clfd);
        }
        bool operator!=(const Client &rhs) const {
            return (clfd != rhs.clfd);
        }
};

#endif // __CLIENT_H_
