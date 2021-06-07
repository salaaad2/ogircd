#include "../inc/Server.hpp"

bool Server::isNickonchan(std::string & nick, std::string & chan)
{
    for (std::vector<Client *>::iterator it = _m_chans[chan].begin() ; it != _m_chans[chan].end() ; it++)
    {
        if ((*it)->nickname.compare(nick) == 0)
            return true;
    }
    return false;
}

std::vector<Client *>::iterator Server::clposition(std::string & nick, std::string & chan)
{
    for (std::vector<Client *>::iterator it = _m_chans[chan].begin() ; it != _m_chans[chan].end() ; it++)
    {
        if ((*it)->nickname.compare(nick) == 0)
            return it;
    }
    return _m_chans[chan].end();
}

std::vector<std::string>::iterator Server::chposition(Client *cl, std::string & chan)
{
    for (std::vector<std::string>::iterator it = cl->chans.begin() ; it != cl->chans.end() ; it++)
    {
        if ((*it) == chan)
            return it;
    }
    return cl->chans.end();
}

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

void Server::partcmd(Message *msg, Client *cl)
{
    if (msg->params.size() == 0)
    {
        send_reply("", cl, ERR_NEEDMOREPARAMS);
        return ;
    }
    std::vector<std::string> channels = parse_m_chans(msg->params[0]);

    for (std::vector<std::string>::iterator it = channels.begin() ; it != channels.end() ; it++)
    {
        if (_m_chans.find(*it) == _m_chans.end())
            send_reply("", cl, ERR_NOSUCHCHANNEL);
        else if (!isNickonchan(cl->nickname, *it))
            send_reply("", cl, ERR_USERNOTINCHANNEL);
        else
        {
            std::string to_send = ":" + cl->prefix + " PART " + (*it) + "\r\n";
            send_to_channel(to_send, (*it), NULL);
            _m_chans[*it].erase(clposition(cl->nickname, *it));
            cl->chans.erase(chposition(cl, *it));
            if (_m_chans[*it].size() == 0)
                _m_chans.erase(*it);
        }
    }
}
