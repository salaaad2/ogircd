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

	/* CHANNELS INFOS */
	/* CHANNEL MODE : [opsitnbvmlk] */
	/* USER MODE : [iwso] */

void Server::modecmd(Message *msg, std::string prefix)
{
    if (msg->params.empty())
        send_reply("", prefix, ERR_NEEDMOREPARAMS);
    else if (msg->params[0] == "#" || msg->params[0] == "&")
        chanMode(msg->params, prefix);
    // else
    //     userMode(msg->params, prefix);
}

void Server::chanMode(std::vector<std::string> params, std::string prefix)
{
    if (_m_chans.find(params[1]) == _m_chans.end())
        send_reply(params[1], prefix, ERR_NOSUCHCHANNEL);
    else
        setChanMode(params, prefix);
}

void Server::setChanMode(std::vector<std::string> params, std::string prefix)
{
    size_t i = 0;
    size_t j = 3;
    std::vector<size_t> par;
    std::vector<std::vector<std::string> > cmds;
    std::string to_add;
    std::string to_remove;
    bool        ar = true;

    std::string modes = "aimnqpsrt";
    std::string pmodes = "OovklbeI";

    while (j < params.size())
    {
        while (params[j][i])
        {
            if (params[j][i] == '+')
                ar = true;
            else if (params[j][i] == '-')
                ar = false;
            else if (ar && (modes.find(params[j][i]) != std::string::npos && to_add.find(params[j][i]) != std::string::npos))
                to_add += params[j][i];
            else if (ar && (pmodes.find(params[j][i]) != std::string::npos))
                to_add += params[j][i];
            else if (ar && (modes.find(params[j][i]) != std::string::npos && to_remove.find(params[j][i]) != std::string::npos))
                to_remove += params[j][i];
            else if (modes.find(params[j][i]) == std::string::npos || pmodes.find(params[j][i]) == std::string::npos)
                send_reply("", prefix, ERR_UNKNOWNMODE);
        }
    }
}
