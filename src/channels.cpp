#include "../inc/Server.hpp"

void Server::namescmd(Message *msg, Client *cl)
{
    if (msg->params.empty())
    {
        for (std::map<std::string, std::vector<Client*> >::iterator it = _m_chans.begin() ; it != _m_chans.end() ; it++)
            send_reply(it->first, cl, RPL_NAMREPLY);
        send_reply("", cl, RPL_ENDOFNAMES);
    }
    else for (size_t i = 0 ; i < msg->params.size() ; i++)
    {
        if (msg->params[i] == "#" || msg->params[i] == "&")
        {
            i++;
            if (_m_chans.find(msg->params[i - 1] + msg->params[i]) != _m_chans.end())
            {
                send_reply(msg->params[i - 1] + msg->params[i], cl, RPL_NAMREPLY);
                send_reply("", cl, RPL_ENDOFNAMES);
            }
        }
    }
}

void Server::listcmd(Message *msg, Client *cl)
{
    send_reply("", cl, RPL_LISTSTART);
    if (msg->params.empty())
    {
        for (std::map<std::string, std::vector<Client*> >::iterator it = _m_chans.begin() ; it != _m_chans.end() ; it++)
            send_reply(it->first, cl, RPL_LIST);
    }
    else for (size_t i = 0 ; i < msg->params.size() ; i++)
    {
        if (msg->params[i] == "#" || msg->params[i] == "&")
        {
            i++;
            if (_m_chans.find(msg->params[i]) != _m_chans.end())
                send_reply(msg->params[i], cl, RPL_LIST);
        }
    }
    send_reply("", cl, RPL_LISTEND);
}
