#include "../inc/Server.hpp"

#include <netdb.h>

void Server::quitcmd(Message *msg, Client *cl) // TODO: NOT WORKING SERVER_SELECT ERROR
{
    (void)msg;
    cl->is_logged = false;
    close(cl->clfd);
}

void Server::versioncmd(Message *msg, Client *cl) // TODO :parameter: server
{
    (void)msg;
    send_reply("",  cl, RPL_VERSION);
}

void Server::statscmd(Message *msg, Client *cl) // TODO :parameters: c - h - i - k - l - m - o - y - server
{
    if (msg->params[0] == "u")
    {
        send_reply("",  cl, RPL_STATSUPTIME);
        send_reply(msg->params[0],  cl, RPL_ENDOFSTATS);
    }
}

//void Server::linkscmd(Message *msg, std::string cl) TODO


void Server::timecmd(Message *msg, Client *cl)
{
    (void)msg;
    send_reply("", cl, RPL_TIME);
}

void Server::infocmd(Message *msg, Client *cl)
{
    (void)msg;
    std::cout << "CLFD : " << cl->clfd << "\n";
    send_reply("", cl, RPL_INFO);
    send_reply("", cl, RPL_ENDOFINFO);
}


void Server::whocmd(Message *msg, Client *cl) //TODO :o flag @ flag for operator
{
    typedef std::map<std::string, std::vector<Client*> >::iterator chan_it;
    typedef std::map<std::string, Client *>::iterator p_it;
    std::string req;
    bool op = false;
    if (msg->params.size() >= 1)
    {
        if (msg->params.size() == 2 && msg->params[1] == "o")
            op = true;
        for (chan_it it = _m_chans.begin(); it != _m_chans.end(); it++)
        {
            if (strmatch((*it).first, msg->params[0]))
            {
                for (std::vector<Client *>::iterator cl_it = (*it).second.begin(); cl_it != (*it).second.end(); cl_it++)
                {
                    req = (*it).first + " " + (*cl_it)->username + " " + (*cl_it)->host + " " + (*cl_it)->nickname + " H@ :0 " + (*cl_it)->realname;
                    send_reply(req, cl, RPL_WHOREPLY);
                }
            }
        }
        if (req.empty())
        {
            for (p_it it = _m_pclients.begin(); it != _m_pclients.end(); it++)
            {
                if (strmatch((*it).second->host, msg->params[0])
                    || strmatch((*it).second->realname, msg->params[0])
                    || strmatch((*it).second->nickname, msg->params[0]))
                {
                    req = cl->nickname + " " + (*it).second->username + " " + (*it).second->host + " " + (*it).second->nickname + " H@ :0 " + (*it).second->realname;
                    send_reply(req, cl, RPL_WHOREPLY);
                }
            }
        }
    }
    send_reply(std::string(cl->nickname) + " " +msg->params[0] + " ", cl, RPL_ENDOFWHO);
}

void Server::whoiscmd(Message *msg, Client *cl) //TODO : @ flag for operator, RPL_WHOISOPERATOR
{
    typedef std::map<std::string, Client *>::iterator p_it;
    std::string req;
    if (msg->params.size() == 0)
    {
        send_reply("", cl, ERR_NONICKNAMEGIVEN);
        return;
    }
    for (size_t i = 0; i < msg->params.size(); i++)
    {
        for (p_it it = _m_pclients.begin(); it != _m_pclients.end(); it++)
        {
            if ((*it).second->nickname == msg->params[i])
            {
                req = (*it).second->nickname + " " + (*it).second->username + " " + (*it).second->host + " * :" + (*it).second->realname;
                send_reply(req, cl, RPL_WHOISUSER);
                send_reply(std::string(cl->nickname) + " " +msg->params[i] + " ", cl, RPL_ENDOFWHOIS);
            }
            else if (_m_chans.count(msg->params[i]) == 1)
            {
                req = msg->params[i];
                send_reply(req + " @ ", cl, RPL_WHOISCHANNELS);
                send_reply(std::string(cl->nickname) + " " + msg->params[i] + " ", cl, RPL_ENDOFWHOIS);
            }
            else if (msg->params[i] != "," && msg->params[i] != " ")
            {
                send_reply(msg->params[i] + " ", cl, ERR_NOSUCHNICK);
                send_reply(cl->nickname + " ", cl, RPL_ENDOFWHOIS);
            }

        }
    }
}

void Server::pingcmd(Message *msg, Client *cl)
{

    if (msg->params[0].empty())
    {
        send_reply(msg->params[0], cl, ERR_NOSUCHSERVER);
    }
    else
    {
        std::string to_send;

        to_send += (":" + cl->prefix + " PONG " + _servername + " \r\n");
        send(cl->clfd, to_send.c_str(), to_send.length(), 0);
    }
}
