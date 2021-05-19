#include "../inc/replies.hpp"
#include "../inc/Server.hpp"

std::string Server::msg_rpl(std::string s = "", int code = -1, int fd = -1)
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
            return(s + "Welcome to the channel you chose");
        case RPL_NAMREPLY :
            return(s + "Namerply");
        case RPL_ENDOFNAMES :
            return(s + "Endofnames");
        case RPL_SUMMONING :
            return (s + "You have been summoned");


        /* ERRORS */

        case ERR_ALREADYREGISTERED :
            return std::string(s + ":You may not reregister");
        case ERR_NICKNAMEINUSE :
            return std::string(s + ":Nickname already in use");
        case ERR_USERONCHANNEL :
            return std::string(s + ":User already on channel");
        case ERR_NOTREGISTERED :
            return std::string(s + ":You need to register before trying this command");
        case ERR_PASSWDMISMATCH :
            return std::string(s + ":Password doesn't match the server's");
        case ERR_NONICKNAMEGIVEN :
            return std::string(s + ":User must provide a nickname");
        case ERR_NEEDMOREPARAMS :
            return std::string(s + ":Insufficient parameter count");
        case ERR_USERSDONTMATCH :
            return std::string(s + ":Usernames don't match");
        case ERR_NOSUCHNICK :
            return std::string(s + ":There is no such user on server");
        case ERR_NOTEXTTOSEND :
            return std::string (s + ":There is no text to send");

        default :
            return std::string("");
    }
}
