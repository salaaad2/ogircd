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
