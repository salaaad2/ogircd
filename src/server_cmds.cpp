#include "../inc/Server.hpp"

void Server::quitcmd(Message *msg, std::string prefix) // TODO: NOT WORKING SERVER_SELECT ERROR
{
    (void)msg;
    _m_pclients[prefix]->is_logged = false;
    close(_m_pclients[prefix]->clfd);
}

void Server::versioncmd(Message *msg, std::string prefix) // TODO :parameter: server
{
    (void)msg;
    send_reply("",  prefix, RPL_VERSION);
}

void Server::statscmd(Message *msg, std::string prefix) // TODO :parameters: c - h - i - k - l - m - o - y - server
{
    if (msg->params[0] == "u")
    {
        send_reply("",  prefix, RPL_STATSUPTIME);
        send_reply(msg->params[0],  prefix, RPL_ENDOFSTATS);
    }
}

//void Server::linkscmd(Message *msg, std::string prefix) TODO


void Server::timecmd(Message *msg, std::string prefix) //TODO :parameter server
{
    (void)msg;
    send_reply("", prefix, RPL_TIME);
}

void Server::infocmd(Message *msg, std::string prefix) //TODO :parameter server
{
    (void)msg;
    send_reply("", prefix, RPL_INFO);
    send_reply("", prefix, RPL_ENDOFINFO);
}


void Server::whocmd(Message *msg, std::string prefix) //TODO :wildcard, server
{
    std::string s;
    Client *cl;
    typedef std::map<std::string, Client*>::iterator _m_iterator;
    typedef std::vector<Client*>::iterator _v_iterator;
    if (_m_chans.count(msg->params[0]) == 1)
    {
        for (_v_iterator it = _m_chans[msg->params[0]].begin(); it != _m_chans[msg->params[0]].end(); it++)
        {
            cl = *it;
            if (cl->nickname == msg->params[0] || cl->username == msg->params[0] || cl->realname == msg->params[0]
                || cl->host == msg->params[0])
                s += "\n" + cl->realname;
        }
    }
    else
    {
        for (_m_iterator it = _m_pclients.begin(); it != _m_pclients.end(); it++)
        {
            cl = it->second;
            if (cl->current_chan != _m_pclients[prefix]->current_chan && (
                    cl->nickname == msg->params[0] || cl->username == msg->params[0] || cl->realname == msg->params[0]
                    || cl->host == msg->params[0]))
                s += "\n" + cl->realname;
        }
    }
    if (!s.empty())
    {
        s.insert(0, "\n" + msg->params[0] + ":");
        send_reply(s, prefix, RPL_WHOREPLY);
        send_reply(msg->params[0], prefix,  RPL_ENDOFWHO);
    }
}
