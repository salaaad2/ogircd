#ifndef __CLIENT_H_
#define __CLIENT_H_
#include <string>
#include <vector>
#include <arpa/inet.h>

struct Client
{
    std::vector<std::string> chans;
    sockaddr_in  clientaddr;
    unsigned int addrlen;
    int          clfd;
    std::string         password;
    std::string         nickname;
    std::string         username;
    std::string         realname;
    std::string         host;
    std::string         prefix;
    bool         is_server;
    bool         is_register;
};

#endif // __CLIENT_H_
