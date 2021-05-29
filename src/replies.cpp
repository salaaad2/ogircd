#include "../inc/replies.hpp"
#include "../inc/Server.hpp"

std::string Server::msg_rpl(std::string s, int code, std::string prefix)
{
    std::string response;

    switch(code){

        /* REPLIES */

        case RPL_WELCOME :
        {
            response += "Welcome to the Internet Relay Network ";
            response += _m_pclients[prefix]->prefix;
            return response;
        }
        case RPL_TOPIC :
            return(s + ":" + _m_topics[s] + RESET);
        case RPL_NAMREPLY :
        {
            response += "= " + s + " :";
            for (size_t i = 0 ; i < _m_chans[s].size() ; i++)
            {
                if (_m_uflags[s][_m_chans[s][i]].find('o') != std::string::npos)
                    response += "@";
                else if (_m_uflags[s][_m_chans[s][i]].find('v') != std::string::npos)
                    response += "+";
                response += _m_chans[s][i]->nickname;
                response += " ";
            }
            return response;
        }
        case RPL_INFO :
            return (msg_rpl(s, RPL_VERSION, prefix)
            + "\n" + msg_rpl(s, RPL_STATSUPTIME, prefix));
        case RPL_ENDOFINFO :
            return (":End of /INFO list");
        case RPL_ENDOFNAMES :
            return( ":End of /NAMES list");
        case RPL_SUMMONING :
            return ("You have been summoned");
        case RPL_VERSION :
            return (":Version is 1.0.1" );
        case RPL_STATSUPTIME :
            return (":Up " + ft_uptime(_launch_time));
        case RPL_TIME :
            return (ft_current_time());
        case RPL_ENDOFSTATS :
            return (":End of /STATS report");
        case RPL_WHOREPLY :
            return (RESET);
        case RPL_ENDOFWHO :
            return (s + ":End of WHO list");
        case RPL_LISTSTART :
            return (std::string() + "Channel :Users  Name");
        case RPL_LIST :
            return (s + " :" + _m_topics[s]);
        case RPL_LISTEND :
            return(std::string() + ":End of /LIST");
        case RPL_CHANNELMODEIS :
            return (s);
        case RPL_BANLIST :
            return (s);
        case RPL_ENDOFBANLIST :
            return (s + " :End of channel ban list");
        case RPL_EXCEPTLIST :
            return (s + RESET);
        case RPL_ENDOFEXCEPTLIST :
            return (s + " :End of channel exception list");

        /* ERRORS */

        case ERR_ALREADYREGISTERED :
            return std::string( s + ":You may not reregister" );
        case ERR_NICKNAMEINUSE :
            return std::string( s + ":Nickname already in use" );
        case ERR_USERONCHANNEL :
            return std::string( s + ":User already on channel" );
        case ERR_NOTREGISTERED :
            return std::string( s + ":You need to register before trying this command" );
        case ERR_PASSWDMISMATCH :
            return std::string( s + ":Password doesn't match the server's" );
        case ERR_NONICKNAMEGIVEN :
            return std::string(BOLDRED+ s + ":User must provide a nickname" );
        case ERR_NEEDMOREPARAMS :
            return std::string( s + ":Insufficient parameter count" );
        case ERR_USERSDONTMATCH :
            return std::string( s + ":Usernames don't match" );
        case ERR_NOSUCHNICK :
            return std::string( s + ":There is no such user on server" );
        case ERR_NOTEXTTOSEND :
            return std::string ( s + ":There is no text to send" );
        case ERR_NOTOCHANNEL :
            return std::string (BOLDRED":To join a channel, type : JOIN #" + s );
        case ERR_BADCHANMASK :
            return std::string (BOLDRED":Invalid JOIN parameter" + s + RESET);
        case ERR_UNKNOWNMODE :
            return std::string(BOLDRED + s + ":is unknown mode char to me" + RESET);
        case ERR_NOSUCHCHANNEL :
            return std::string(BOLDRED + s + ":No such channel");
        case ERR_CHANOPRIVSNEEDED :
            return std::string(BOLDRED":You are not channel operator");
        case ERR_BADCHANNELKEY :
            return std::string(BOLDRED + s + ":Cannot join channel (+k) -- Wrong channel key");
        case ERR_BANNEDFROMCHAN :
            return std::string(BOLDRED + s + ":Cannot join channel (+b) -- You are banned");
        case ERR_CHANNELISFULL :
            return std::string(BOLDRED + s + ":Cannot join channel (+l) -- Channel is full, try later");
        default :
            return s;
    }
}
