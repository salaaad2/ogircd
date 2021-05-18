#include "../inc/replies.hpp"
#include "../inc/Server.hpp"

std::string Server::msg_rpl(int code, int fd)
{
    std::string response;

    switch(code){

        /* REPLIES */

        case RPL_WELCOME :
        {
            response += "Welcome to the Internet Relay Network ";
            response += _fd_clients[fd].prefix;
            return response;
        }

        /* ERRORS */

        case ERR_ALREADYREGISTERED :
            return std::string(":You may not reregister");
        case ERR_NICKNAMEINUSE :
            return std::string(":Nickname already in use");
        default :
            return std::string("");
    }
}
