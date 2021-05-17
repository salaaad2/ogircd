#ifndef __CLIENT_H_
#define __CLIENT_H_
#include <arpa/inet.h>
struct Client
{
    sockaddr_in  clientaddr;
    unsigned int addrlen;
    int          clfd;
    char         password[16];
    char         nickname[9];
    char         username[32];
    char         realname[32];
    char         host[16];
    char         prefix[57];
    bool         is_server;
    bool         is_register;
};

#endif // __CLIENT_H_
