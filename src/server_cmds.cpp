#include "../inc/Server.hpp"

void Server::quitcmd(Message *msg, Client *cl) // TODO: NOT WORKING SERVER_SELECT ERROR
{
    (void)msg;
    cl->is_logged = false;
    close(cl->clfd);
    FD_CLR(cl->clfd, &_fds->master);
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
                    if (op == false || _m_uflags[(*it).first][(*cl_it)].find("o") != std::string::npos)
                    {
                      req = msg->params[0] + " " + (*cl_it)->username + " " +
                            (*cl_it)->host + " " + _servername + " " + (*cl_it)->nickname + " H";
                      if (_m_uflags[(*it).first][(*cl_it)].find("o") !=
                          std::string::npos)
                        req += "@ :0 " + (*cl_it)->realname;
                      else if (_m_uflags[(*it).first][(*cl_it)].find("v") !=
                               std::string::npos)
                        req += "+ :0 " + (*cl_it)->realname;
                      else
                        req += " :0 " + (*cl_it)->realname;
                      send_reply(req, cl, RPL_WHOREPLY);
                    }
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
                    if ((*it).second->is_logged == true)
                        {
                            req = "* " + msg->params[0] + " " + (*it).second->username + " " + (*it).second->host + " " + (*it).second->nickname + " H :0 " + (*it).second->realname;
                            send_reply(req, cl, RPL_WHOREPLY);
                        }
                }
            }
        }
    }
    send_reply(msg->params[0] + " ", cl, RPL_ENDOFWHO);
}

void Server::whoiscmd(Message *msg, Client *cl) //TODO : @ flag for operator, RPL_WHOISOPERATOR
{
    typedef std::map<std::string, Client *>::iterator p_it;
    std::string req;
    bool find;
    if (msg->params.size() == 0)
    {
        send_reply("", cl, ERR_NONICKNAMEGIVEN);
        return;
    }
    for (size_t i = 0; i < msg->params.size(); i++)
    {
        find = false;
        for (p_it it = _m_pclients.begin(); it != _m_pclients.end(); it++)
        {
            if ((*it).second->nickname == msg->params[i] && (*it).second->is_logged == true)
            {
                req = (*it).second->nickname + " " + (*it).second->username + " " + (*it).second->host + " * :" + (*it).second->realname;
                send_reply(req, cl, RPL_WHOISUSER);
                find = true;
            }
            else if (_m_chans.count(msg->params[i]) == 1)
            {
                for (std::vector<Client *>::iterator cl_it = _m_chans[msg->params[i]].begin(); cl_it != _m_chans[msg->params[i]].end(); cl_it++)
                {
                  req = (*cl_it)->nickname + " :";
                  if (_m_uflags[msg->params[i]][(*cl_it)].find("o") !=
                      std::string::npos)
                    req += "@ :0 " + msg->params[i];
                  else if (_m_uflags[msg->params[i]][(*cl_it)].find("v") !=
                           std::string::npos)
                    req += "+ :0 " + msg->params[i];
                  else
                    req += " :0 " + msg->params[i];
                  send_reply(req, cl, RPL_WHOISCHANNELS);
                }
                find = true;
                break;
            }
        }
        if (msg->params[i] != "," && msg->params[i] != " " && find == false) {
            send_reply(msg->params[i] + " ", cl, ERR_NOSUCHNICK);
        }
        if (find)
            send_reply(std::string(cl->nickname) + " " + msg->params[i], cl,
                       RPL_ENDOFWHOIS);
    }
}

void Server::whowascmd(Message *msg, Client *cl)
{
    int count = -1;
    std::string req;
    std::stack<Client*> n_db;
    if (!msg->params.size())
    {
        send_reply("", cl, ERR_NONICKNAMEGIVEN);
        return;
    }
    if (msg->params.size() == 2)
        req = atoi(msg->params[1].c_str());
    if (_m_nickdb.count(msg->params[0]) == 0 || _m_nickdb[msg->params[0]].top()->is_logged == true)
    {
        send_reply(msg->params[0], cl, ERR_WASNOSUCHNICK);
        send_reply(msg->params[0] + " ", cl, RPL_ENDOFWHOWAS);
        return;
    }
    n_db = _m_nickdb[msg->params[0]];
    for (; count != 0 &&!n_db.empty(); count--)
    {
        req = n_db.top()->nickname + " " + n_db.top()->username + " " + n_db.top()->host + " * :" + n_db.top()->realname;
        send_reply(req, cl, RPL_WHOWASUSER);
        send_reply(msg->params[0] + " ", cl, RPL_ENDOFWHOWAS);
        n_db.pop();
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
