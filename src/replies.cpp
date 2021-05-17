#include "../inc/replies.hpp"
#include "../inc/Server.hpp"

const char *Server::msg_rpl(int code, int fd)
{
    char response[512];

    switch(code){

        /* REPLIES */

        case RPL_WELCOME :
        {
            strcpy(response, "Welcome to the Internet Relay Network\n");
            strcat(response, _fd_clients[fd].prefix);
            return response;
        }

        /* ERRORS */

        case ERR_ALREADYREGISTERED :
            return (":You may not reregister");
        case ERR_NICKNAMEINUSE :
            return(":Nickname already in u`se");

        default :
            return ("");
    }
}