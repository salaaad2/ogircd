#include "../inc/Server.hpp"

void Server::invitecmd(Message *msg, Client *cl)
{
    std::string nickname;

    if (_m_nickdb.count(msg->params[0]) == 1)
        nickname = msg->params[0];
    else
    {
        send_reply(msg->params[0], cl, ERR_NOSUCHNICK);
        return ;
    }
    if (msg->params.size() < 2)
    {
        send_reply(msg->params[0], cl, ERR_NEEDMOREPARAMS);
        return ;
    }
    else if (_m_uflags[msg->params[1]][cl].find('o') == std::string::npos)
    {
        send_reply("", cl, ERR_CHANOPRIVSNEEDED);
        return ;
    }
    if ((msg->params[1][0] == '#' || msg->params[1][0] == '&') && _m_chans.find(msg->params[1]) != _m_chans.end())
    {
        for (std::vector<Client *>::iterator it = _m_chans[msg->params[1]].begin() ; it != _m_chans[msg->params[1]].end() ; it++)
        {
            if ((*it)->nickname == nickname)
            {
                send_reply("", cl, ERR_USERONCHANNEL);
                return ;
            }
        }
        _m_invite[msg->params[1]].push_back(nickname);
    }
}

void Server::topiccmd(Message *msg, Client *cl)
{
    if (msg->params.size() == 0)
    {
        send_reply("", cl, ERR_NEEDMOREPARAMS);
        return ;
    }
    else
    {
        std::string chan = msg->params[0];

        if (_m_chans.find(chan) == _m_chans.end())
            send_reply("", cl, ERR_NOSUCHCHANNEL);
        else if (!isNickonchan(cl->nickname, chan))
            send_reply(chan, cl, ERR_NOTOCHANNEL);
        else if (msg->params.size() == 1)
        {
            if (_m_topics[chan].size() == 0)
                send_reply("", cl, RPL_NOTOPIC);
            else
                send_reply("", cl, RPL_TOPIC);
        }
        else
        {
            _m_topics[chan] = msg->params[1];
            send_to_channel(":" + cl->prefix + " TOPIC " + chan + " :" + msg->params[1] + "\r\n", chan);
        }
    }
}

// void Server::kickcmd(Message *msg, Client *cl)
// {

// }