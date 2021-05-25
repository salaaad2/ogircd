#include "../inc/Server.hpp"

void Server::namescmd(Message *msg, std::string prefix)
{
    std::cout << "bob1\n";
    if (msg->params.empty())
    {
        std::cout << "bob2\n";
        for (std::map<std::string, std::vector<Client*> >::iterator it = _m_chans.begin() ; it != _m_chans.end() ; it++)
        {
            send_reply(it->first, prefix, RPL_NAMREPLY);
            send_reply(it->first, prefix, RPL_ENDOFNAMES);
        }
    }
}

// void Server::partcmd(Message *msg, std::string prefix)
// {
    
// }
