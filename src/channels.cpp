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
    else if (params[3][0] == '+')
        setChanMode(params, prefix);
}

void Server::setChanMode(std::vector<std::string> params, std::string prefix)
{
    std::string modes = "opsitnbvmlk";

    for (size_t i = 1 ; params[3][i] ; i++)
    {
        if (modes.find(params[3][i]) == std::string::npos)
            send_reply(std::string(params[3], i, 1), prefix, ERR_UNKNOWNMODE);
    }
    for (size_t i = 1 ; params[3][i] ; i++)
    {
        if (params[3][i] == 'o')
            _m_flags[params[1]][0] = 'o';
        if (params[3][i] == 'p')
            _m_flags[params[1]][1] = 'p';
        if (params[3][i] == 's')
            _m_flags[params[1]][2] = 's';
        if (params[3][i] == 'i')
            _m_flags[params[1]][3] = 'i';
        if (params[3][i] == 't')
            _m_flags[params[1]][4] = 't';
        if (params[3][i] == 'n')
            _m_flags[params[1]][5] = 'n';
        if (params[3][i] == 'b')
            _m_flags[params[1]][6] = 'b';
        if (params[3][i] == 'v')
            _m_flags[params[1]][7] = 'v';
    }
    if (params[3].size() == 2)
    {
        if (params[3][1] == 'l' && params.size() > 4)
            _m_limits[params[1]] = ft_atoi(params[4].c_str());
    }
}
