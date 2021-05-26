#include "../inc/Server.hpp"

void Server::namescmd(Message *msg, std::string prefix)
{
    if (msg->params.empty())
    {
        for (std::map<std::string, std::vector<Client*> >::iterator it = _m_chans.begin() ; it != _m_chans.end() ; it++)
        {
            send_reply(it->first, prefix, RPL_NAMREPLY);
            send_reply(it->first, prefix, RPL_ENDOFNAMES);
        }
    }
    else for (size_t i = 0 ; i < msg->params.size() ; i++)
    {
        if (msg->params[i] == "#" || msg->params[i] == "&")
        {
            i++;
            if (_m_chans.find(msg->params[i]) != _m_chans.end())
            {
                send_reply(msg->params[i], prefix, RPL_NAMREPLY);
                send_reply(msg->params[i], prefix, RPL_ENDOFNAMES);
            }
        }
    }
}

void Server::listcmd(Message *msg, std::string prefix)
{
    send_reply("", prefix, RPL_LISTSTART);
    if (msg->params.empty())
    {
        for (std::map<std::string, std::vector<Client*> >::iterator it = _m_chans.begin() ; it != _m_chans.end() ; it++)
            send_reply(it->first, prefix, RPL_LIST);
    }
    else for (size_t i = 0 ; i < msg->params.size() ; i++)
    {
        if (msg->params[i] == "#" || msg->params[i] == "&")
        {
            i++;
            if (_m_chans.find(msg->params[i]) != _m_chans.end())
                send_reply(msg->params[i], prefix, RPL_LIST);
        }
    }
    send_reply("", prefix, RPL_LISTEND);
}
