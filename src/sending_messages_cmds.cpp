#include "../inc/Server.hpp"

void Server::privmsgcmd(Message *msg, Client *cl)
{
    // ERR_NORECIPIENT --NO                ERR_NOTEXTTOSEND -- Yes
    // ERR_CANNOTSENDTOCHAN --No            ERR_NOTOPLEVEL -- No
    // ERR_WILDTOPLEVEL --No                ERR_TOOMANYTARGETS --No
    // ERR_NOSUCHNICK --Yes
    //  RPL_AWAY --No
    std::list<std::string> chans;
    std:: list<Client*> nicknames;
    std::string curr_chan_tmp;
    Message text;
    Client *cl_tmp;
    size_t i = 0;

    while (i < msg->params.size() && msg->params[i] != ":")
    {
        if (_m_nickdb.count(msg->params[i]) == 1)
        {
            cl_tmp = _m_nickdb[msg->params[i]].top();
            if (cl_tmp->is_logged == true)
                nicknames.push_back(cl_tmp);
            else
                send_reply(msg->params[i], cl, ERR_NOSUCHNICK);
        }
        else if (_m_chans.count(msg->params[i]) == 1)
            chans.push_back(msg->params[i]);
        else
            send_reply(msg->params[i], cl, ERR_NOSUCHNICK);
        i++;
    }
    i++;
    while (i < msg->params.size())
    {
        text.params.push_back(msg->params[i]);
        i++;
    }
    if (text.params.size() == 0)
        send_reply("", cl, ERR_NOTEXTTOSEND);
    nicknames.sort();
    nicknames.unique();
    chans.sort();
    chans.unique();
    i = 0;
    curr_chan_tmp = cl->current_chan;
    for (std::list<std::string>::iterator it = chans.begin(); it != chans.end(); it++)
    {
        cl->current_chan = *it;
        chan_msg(&text, cl);
    }
    cl->current_chan = curr_chan_tmp;
    std::vector<Client*> vec(nicknames.begin(), nicknames.end());
    send_reply_broad(cl, vec, -1, &text);
}

void Server::noticecmd(Message *msg, Client *cl)
{
    std::list<std::string> chans;
    std::vector<Client*> vec;
    std::list<Client*> nicknames;
    std::string curr_chan_tmp;
    Message text;
    size_t i = 0;
    Client *cl_tmp;

    while (i < msg->params.size() && msg->params[i] != ":")
    {
        if (_m_nickdb.count(msg->params[i]) == 1)
        {
            cl_tmp = _m_nickdb[msg->params[i]].top();
            if (cl_tmp->is_logged == true)
                nicknames.push_back(cl_tmp);
        }
        else if (_m_chans.count(msg->params[i]) == 1)
            chans.push_back(msg->params[i]);
        i++;
    }
    while (i < msg->params.size())
    {
        text.params.push_back(msg->params[i]);
        i++;
    }
    if (text.params.empty() == true)
        return;
    nicknames.sort();
    nicknames.unique();
    chans.sort();
    chans.unique();
    i = 0;
    curr_chan_tmp = cl->current_chan;
    for (std::list<std::string>::iterator it = chans.begin(); it != chans.end(); it++)
    {
        cl->current_chan = *it;
        chan_msg(&text, cl);
    }
    cl->current_chan = curr_chan_tmp;
    vec.assign(nicknames.begin(), nicknames.end());
    send_reply_broad(cl, vec, -1, &text);
}
