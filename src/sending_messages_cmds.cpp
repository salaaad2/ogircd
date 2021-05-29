#include "../inc/Server.hpp"

void Server::privmsgcmd(Message *msg, std::string & prefix)
{
    // ERR_NORECIPIENT --NO                ERR_NOTEXTTOSEND -- Yes
    // ERR_CANNOTSENDTOCHAN --No            ERR_NOTOPLEVEL -- No
    // ERR_WILDTOPLEVEL --No                ERR_TOOMANYTARGETS --No
    // ERR_NOSUCHNICK --Yes
    //  RPL_AWAY --No
    size_t i = 0;
    Message text;
    Client *cl_tmp;
    std::string curr_chan_tmp;
    std::list<Client*> nicknames;
    std::list<std::string> chans;

    while (i < msg->params.size() && msg->params[i] != ":")
    {
        if (msg->params[i] != " " && msg->params[i] != ",")
        {
            if (_m_nickdb.count(msg->params[i]) == 1)
            {
                cl_tmp = _m_nickdb[msg->params[i]].top();
                if (cl_tmp->is_logged == true)
                    nicknames.push_back(cl_tmp);
                else
                    send_reply(msg->params[i], prefix, ERR_NOSUCHNICK);
            }
            else if (msg->params[i] == "#" ||
                     msg->params[i] == "&")
            {
                i += (i < msg->params.size()) ? 1 : 0;
                if (_m_chans.count(msg->params[i - 1] + msg->params[i]) == 1)
                {
                    chans.push_back(msg->params[i - 1] + msg->params[i]);
                }
            }
            else
                send_reply(msg->params[i], prefix, ERR_NOSUCHNICK);
        }
        i++;
    }
    i++;
    while (i < msg->params.size())
    {
        text.params.push_back(msg->params[i]);
        i++;
    }
    if (text.params.size() == 0)
        send_reply("", prefix, ERR_NOTEXTTOSEND);
    nicknames.sort();
    nicknames.unique();
    chans.sort();
    chans.unique();
    i = 0;
    curr_chan_tmp = _m_pclients[prefix]->current_chan;

    std::string s_text;
    for (std::vector<std::string>::iterator it = text.params.begin() ; it != text.params.end() ; it++)
        s_text += *it;
    for (std::list<std::string>::iterator it = chans.begin(); it != chans.end(); it++)
    {
        _m_pclients[prefix]->current_chan = *it;
        send_to_channel(prefix + " PRIVMSG " + *it + " " + s_text + "\r\n", *it);
    }
    _m_pclients[prefix]->current_chan = curr_chan_tmp;
    std::vector<Client*> vec(nicknames.begin(), nicknames.end());
    std::string to_send; 
    for (std::vector<Client *>::iterator it = vec.begin() ; it != vec.end() ; it++)
    {
        to_send = prefix + " PRIVMSG " + (*it)->nickname + " " + s_text + "\r\n";
        send((*it)->clfd, to_send.c_str(), to_send.length(), 0);
    }
}

void Server::noticecmd(Message *msg, std::string & prefix)
{
    size_t i = 0;
    Message text;
    Client *cl_tmp;
    std::string curr_chan_tmp;
    std:: list<Client*> nicknames;
    std::list<std::string> chans;

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
    curr_chan_tmp = _m_pclients[prefix]->current_chan;
    for (std::list<std::string>::iterator it = chans.begin(); it != chans.end(); it++)
    {
        _m_pclients[prefix]->current_chan = *it;
        chan_msg(&text, prefix);
    }
    _m_pclients[prefix]->current_chan = curr_chan_tmp;
    std::vector<Client*> vec(nicknames.begin(), nicknames.end());
    send_reply_broad(prefix, vec, -1, &text);
}

