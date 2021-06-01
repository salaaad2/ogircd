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


void Server::whocmd(Message *msg, Client *cl) //TODO :wildcard
{
    typedef std::map<std::string, Client*>::iterator _m_iterator;
    typedef std::vector<Client*>::iterator _v_iterator;

    std::string s;
    Client *cl2;

    if (_m_chans.count(msg->params[0]) == 1)
    {
        for (_v_iterator it = _m_chans[msg->params[0]].begin();
             it != _m_chans[msg->params[0]].end();
             it++)
        {
            cl2 = *it;
            if (cl2->nickname == msg->params[0] || cl2->username == msg->params[0] || cl2->realname == msg->params[0]
                || cl2->host == msg->params[0])
                s += "\n" + cl2->realname;
        }
    }
    else
    {
        for (_m_iterator it = _m_pclients.begin(); it != _m_pclients.end(); it++)
        {
            cl2 = it->second;
            if (cl2->current_chan != cl->current_chan && (
                    cl2->nickname == msg->params[0] || cl2->username == msg->params[0] || cl2->realname == msg->params[0]
                    || cl2->host == msg->params[0]))
                s += "\n" + cl2->realname;
        }
    }
    if (!s.empty())
    {
        s.insert(0, "\n" + msg->params[0] + ":");
        send_reply(s, cl, RPL_WHOREPLY);
        send_reply(msg->params[0], cl,  RPL_ENDOFWHO);
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
        std::string prefix;
        prefix = ":";
        prefix += _ip;

        to_send += (prefix + " " + cl->nickname + " PONG\r\n");
        send(cl->clfd, to_send.c_str(), to_send.length(), 0);
    }
}
