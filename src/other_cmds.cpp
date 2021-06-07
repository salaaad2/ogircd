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
        std::string to_send = ":" + cl->prefix + " INVITE " + nickname + " " + msg->params[1] + "\r\n";
        send(_m_nickdb[nickname].top()->clfd, to_send.c_str(), to_send.length(), 0);
        to_send = ":" + _m_nickdb[nickname].top()->prefix + " 341 " + cl->nickname + " " + nickname + " " + msg->params[1] + "\r\n";
        send(cl->clfd, to_send.c_str(), to_send.length(), 0);
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
                send_reply(chan, cl, RPL_NOTOPIC);
            else
                send_reply(chan, cl, RPL_TOPIC);
        }
        else
        {
            if (_m_uflags[chan][cl].find('o') == std::string::npos)
            {
                send_reply("", cl, ERR_CHANOPRIVSNEEDED);
                return ;
            }
            _m_topics[chan] = msg->params[1];
            send_to_channel(":" + cl->prefix + " TOPIC " + chan + " :" + msg->params[1] + "\r\n", chan, NULL);
        }
    }
}

void Server::kickcmd(Message *msg, Client *cl)
{
    if (msg->params.size() < 2)
    {
        send_reply("", cl, ERR_NEEDMOREPARAMS);
        return ;
    }

    std::string chan = msg->params[0];
    std::string user = msg->params[1];

    if (_m_chans.find(chan) == _m_chans.end())
        send_reply("", cl, ERR_NOSUCHCHANNEL);
    else if (_m_uflags[chan][cl].find('o') == std::string::npos)
        send_reply("", cl, ERR_CHANOPRIVSNEEDED);
    else if (!isNickonchan(cl->nickname, chan) ||
             !isNickonchan(user, chan))
        send_reply(chan, cl, ERR_NOTOCHANNEL);
    else
    {
        std::string comment;
        if (msg->params.size() >= 3)
            comment = msg->params[2];
        else
            comment = cl->nickname;
        send_to_channel(":" + cl->prefix + " KICK " + chan + " " + user + " :" + comment + "\r\n", chan, NULL);
        _m_chans[chan].erase(clposition(user, chan));
        _m_nickdb[user].top()->chans.erase(chposition(_m_nickdb[user].top(), chan));
    }
}
