#include "../inc/replies.hpp"
#include "../inc/Server.hpp"

std::string Server::msg_rpl(std::string s, int code, std::string prefix)
{
    std::string response;

    switch(code){

        /* REPLIES */

        case RPL_WELCOME :
        {
            response += BOLDCYAN;
            response += "Welcome to the Internet Relay Network ";
            response += _m_pclients[prefix]->prefix;
            response += RESET;
            return response;
        }
        case RPL_TOPIC :
            return(s + " :" + _m_topics[s]);
        case RPL_NAMREPLY :
        {
            for (size_t i = 0 ; i < _m_chans[s].size() ; i++)
            {
                response += _m_uflags[s][_m_chans[s][i]][3] == 'o' ? "@" : "+";
                response += _m_chans[s][i]->nickname;
                response += " ";
                response += RESET;
            }
            return response;
        }
        case RPL_ENDOFNAMES :
            return(BOLDWHITE + s + ":End of /NAMES list" + RESET);
        case RPL_SUMMONING :
            return (BOLDCYAN + s + "You have been summoned" + RESET);
        case RPL_VERSION :
            return (BOLDCYAN + s + "Server version is 1.0.1" + RESET);
        case RPL_STATSUPTIME :
            return (BOLDCYAN + s +  ":Server Up " + ft_uptime(_launch_time) + RESET);
        case RPL_ENDOFSTATS :
            return (BOLDCYAN + s + ":End of /STATS report" + RESET);

        /* ERRORS */

        case ERR_ALREADYREGISTERED :
            return std::string(BOLDRED + s + ":You may not reregister" + RESET);
        case ERR_NICKNAMEINUSE :
            return std::string(BOLDRED + s + ":Nickname already in use" + RESET);
        case ERR_USERONCHANNEL :
            return std::string(BOLDRED + s + ":User already on channel" + RESET);
        case ERR_NOTREGISTERED :
            return std::string(BOLDRED + s + ":You need to register before trying this command" + RESET);
        case ERR_PASSWDMISMATCH :
            return std::string(BOLDRED + s + ":Password doesn't match the server's" + RESET);
        case ERR_NONICKNAMEGIVEN :
            return std::string(BOLDRED+ s + ":User must provide a nickname" + RESET);
        case ERR_NEEDMOREPARAMS :
            return std::string(BOLDRED + s + ":Insufficient parameter count" + RESET);
        case ERR_USERSDONTMATCH :
            return std::string(BOLDRED + s + ":Usernames don't match" + RESET);
        case ERR_NOSUCHNICK :
            return std::string(BOLDRED + s + ":There is no such user on server" + RESET);
        case ERR_NOTEXTTOSEND :
            return std::string (BOLDRED + s + ":There is no text to send" + RESET);
        case ERR_NOTOCHANNEL :
            return std::string (BOLDRED":To join a channel, type : JOIN #" + s + RESET);
        case ERR_BADCHANMASK :
            return std::string (BOLDRED":Invalid JOIN parameter" + s + RESET);
        default :
            return s;
    }
}
