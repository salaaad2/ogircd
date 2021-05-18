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
            return std::string(":You may not reregister");
        case ERR_NICKNAMEINUSE :
            return std::string(":Nickname already in use");
        case ERR_USERONCHANNEL :
            return std::string(":User already on channel");
        case ERR_NOTREGISTERED :
            return std::string(":You need to register before trying this command");
        case ERR_PASSWDMISMATCH :
            return std::string(":Password doesn't match the server's");
        case ERR_NONICKNAMEGIVEN :
            return std::string(":User must provide a nickname");
        case ERR_NEEDMOREPARAMS :
            return std::string(":Insufficient parameter count");

        default :
            return std::string("");
    }
}
