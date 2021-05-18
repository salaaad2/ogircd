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
            return(strcat(response, _fd_clients[fd].prefix));
        }
        case RPL_TOPIC :
            return("Welcome to the channel you chose");
        case RPL_NAMREPLY :
            return("Namerply");
        case RPL_ENDOFNAMES :
            return("Endofnames");
        case RPL_SUMMONING :
            return ("You have been summoned");

        /* ERRORS */

        case ERR_ALREADYREGISTERED :
            return (":You may not reregister");
        case ERR_NICKNAMEINUSE :
            return(":Nickname already in use");
        case ERR_USERONCHANNEL :
            return(":User already on channel");

        default :
            return ("");
    }
}
