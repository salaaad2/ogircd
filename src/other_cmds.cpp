#include "../inc/Server.hpp"

void Server::invitecmd(Message *msg, std::string prefix)
{
    std::string nickname;
    size_t i = 0;

    while (msg->params[i] == " ")
        i++;
    if (_m_nickdb.count(msg->params[i]) == 1)
        nickname = msg->params[i];
    else
        send_reply(msg->params[i], prefix, ERR_NOSUCHNICK);
    while (msg->params[i] == " ")
        i++;
    if ((msg->params[i] == "#" || msg->params[i] == "&") && (i < msg->params.size() + 1)
    && _m_chans.find(msg->params[i] + msg->params[i + 1]) != _m_chans.end())
    {
        std::string chan = msg->params[i] + msg->params[i + 1];
        if (_m_uflags[chan][_m_pclients[prefix]].find('o') == std::string::npos)
        {
            send_reply("", prefix, ERR_CHANOPRIVSNEEDED);
            return ;
        }
        for (std::vector<Client *>::iterator it = _m_chans[chan].begin() ; it != _m_chans[chan].end() ; it++)
        {
            if ((*it)->nickname == nickname)
            {
                send_reply("", prefix, ERR_USERONCHANNEL);
                return ;
            }
        }
        _m_invite[msg->params[i] + msg->params[i + 1]].push_back(nickname);
        
    }
}
