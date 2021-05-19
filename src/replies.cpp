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
            return(s + " :" + _topics[s]);
        case RPL_NAMREPLY :
        {
            for (size_t i = 0 ; i < _channels[s].size() ; i++)
            {
                response += "+";
                response += _channels[s][i].nickname;
                response += " ";
            }
            return response;
        }
        case RPL_ENDOFNAMES :
            return(s + ":End of /NAMES list");
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
        case ERR_NOTOCHANNEL :
            return std::string (":To join a channel, type : JOIN #" + s);
        default :
            return std::string("");
    }
}
