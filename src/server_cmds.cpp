#include "../inc/Server.hpp"

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


void Server::timecmd(Message *msg, Client *cl) //TODO :parameter server
{
    (void)msg;
    send_reply("", cl, RPL_TIME);
}

void Server::infocmd(Message *msg, Client *cl) //TODO :parameter server
{
    (void)msg;
    std::cout << "CLFD : " << cl->clfd << "\n";
    send_reply("", cl, RPL_INFO);
    send_reply("", cl, RPL_ENDOFINFO);
}


void Server::whocmd(Message *msg, Client *cl) //TODO :o flag
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
                    req = cl->nickname + " " + (*it).first + " " + (*cl_it)->username + " " + (*cl_it)->host + " " + (*cl_it)->nickname + "H@ :" + (*cl_it)->realname;
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
                    req = cl->nickname + " " + (*it).second->username + " " + (*it).second->host + " " + (*it).second->nickname + " " + (*it).second->realname + "H@ :" + (*it).second->realname;
                    send_reply(req, cl, RPL_WHOREPLY);
                }
            }
        }
    }
    send_reply(std::string(cl->nickname) + " " +msg->params[0], cl, RPL_ENDOFWHO);
}
