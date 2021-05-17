#ifndef __CLIENT_H_
#define __CLIENT_H_
#include <arpa/inet.h>
struct Client
{
    sockaddr_in  clientaddr;
    unsigned int addrlen;
    int          clfd;
    char         nickname[9];
    char         username[32];
    char         realname[32];
    char         host[16];
    bool         is_server;
    bool         is_register;
};

#endif // __CLIENT_H_
