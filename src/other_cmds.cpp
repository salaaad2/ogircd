#include "../inc/Server.hpp"

void Server::invitecmd(Message *msg, std::string prefix)
{
    std::string nickname;

    if (_m_nickdb.count(msg->params[0]) == 1)
        nickname = msg->params[0];
    else
    {
        send_reply(msg->params[0], prefix, ERR_NOSUCHNICK);
        return ;
    }
    if (msg->params.size() < 2)
    {
        send_reply(msg->params[0], prefix, ERR_NEEDMOREPARAMS);
        return ;
    }
    else if (_m_uflags[msg->params[1]][_m_pclients[prefix]].find('o') == std::string::npos)
    {
        send_reply("", prefix, ERR_CHANOPRIVSNEEDED);
        return ;
    }
    if ((msg->params[1][0] == '#' || msg->params[1][0] == '&') && _m_chans.find(msg->params[1]) != _m_chans.end())
    {
        for (std::vector<Client *>::iterator it = _m_chans[msg->params[1]].begin() ; it != _m_chans[msg->params[1]].end() ; it++)
        {
            if ((*it)->nickname == nickname)
            {
                send_reply("", prefix, ERR_USERONCHANNEL);
                return ;
            }
        }
        _m_invite[msg->params[1]].push_back(nickname);
    }
}
