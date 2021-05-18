#include "../inc/replies.hpp"
#include "../inc/Server.hpp"

std::string Server::msg_rpl(int code, int fd, std::string chan)
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
            return(chan + " :" + _topics[chan]);
        case RPL_NAMREPLY :
        {
            for (size_t i = 0 ; i < _channels[chan].size() ; i++)
            {
                response += "+";
                response += _channels[chan][i].nickname;
                response += " ";
            }
            return response;
        }
        case RPL_ENDOFNAMES :
            return(chan + ":End of /NAMES list");
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
