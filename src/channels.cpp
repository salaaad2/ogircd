#include "../inc/Server.hpp"

bool Server::isNickonchan(std::string nick, std::string chan)
{
    for (std::vector<Client *>::iterator it = _m_chans[chan].begin() ; it != _m_chans[chan].end() ; it++)
    {
        if ((*it)->nickname == nick)
            return true;
    }
    return false;
}

std::vector<Client *>::iterator Server::clposition(std::string nick, std::string chan)
{
    for (std::vector<Client *>::iterator it = _m_chans[chan].begin() ; it != _m_chans[chan].end() ; it++)
    {
        if ((*it)->nickname == nick)
            return it;
    }
    return _m_chans[chan].end();
}

std::vector<std::string>::iterator Server::chposition(std::string prefix, std::string chan)
{
    for (std::vector<std::string>::iterator it = _m_pclients[prefix]->chans.begin() ; it != _m_pclients[prefix]->chans.end() ; it++)
    {
        if ((*it) == chan)
            return it;
    }
    return _m_pclients[prefix]->chans.end();
}

void Server::namescmd(Message *msg, std::string prefix)
{
    if (msg->params.empty())
    {
        for (std::map<std::string, std::vector<Client*> >::iterator it = _m_chans.begin() ; it != _m_chans.end() ; it++)
            send_reply(it->first, prefix, RPL_NAMREPLY);
        send_reply("", prefix, RPL_ENDOFNAMES);
    }
    else for (size_t i = 0 ; i < msg->params.size() ; i++)
    {
        if (msg->params[i] == "#" || msg->params[i] == "&")
        {
            i++;
            if (_m_chans.find(msg->params[i - 1] + msg->params[i]) != _m_chans.end())
            {
                send_reply(msg->params[i - 1] + msg->params[i], prefix, RPL_NAMREPLY);
                send_reply("", prefix, RPL_ENDOFNAMES);
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

void Server::partcmd(Message *msg, std::string prefix)
{
    if (msg->params.size() == 0)
    {
        send_reply("", prefix, ERR_NEEDMOREPARAMS);
        return ;
    }
    std::vector<std::string> channels = parse_m_chans(msg->params[0]);

    for (std::vector<std::string>::iterator it = channels.begin() ; it != channels.end() ; it++)
    {
        if (_m_chans.find(*it) == _m_chans.end())
            send_reply("", prefix, ERR_NOSUCHCHANNEL);
        else if (!isNickonchan(_m_pclients[prefix]->nickname, *it))
            send_reply("", prefix, ERR_USERNOTINCHANNEL);
        else
        {
            std::string to_send = ":" + prefix + " PART " + (*it) + "\r\n";
            send_to_channel(to_send, (*it));
            _m_chans[*it].erase(clposition(_m_pclients[prefix]->nickname, *it));
            _m_pclients[prefix]->chans.erase(chposition(prefix, *it));
            if (_m_chans[*it].size() == 0)
                _m_chans.erase(*it);
        }
    }
}
