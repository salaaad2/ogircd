#include "../inc/Server.hpp"

void Server::modecmd(Message *msg, std::string prefix)
{
    if (msg->params.empty())
        send_reply("", prefix, ERR_NEEDMOREPARAMS);
    else if (msg->params[0] == "#" || msg->params[0] == "&")
        chanMode(msg->params, prefix);
    // else
    //     userMode(msg->params, prefix);
}

void Server::chanMode(std::vector<std::string> params, std::string prefix)
{
    if (_m_chans.find(params[1]) == _m_chans.end())
        send_reply(params[1], prefix, ERR_NOSUCHCHANNEL);
    else
        setChanMode(params, prefix);
}

void Server::setChanMode(std::vector<std::string> params, std::string prefix)
{
    std::vector<size_t> par;
    std::vector<std::string> cmds;
    std::string c_par;
    bool        ar = true;

    std::string modes = "aimnqpsrt";
    std::string pmodes = "OovklbeI";

    for (size_t j = 3 ; j < params.size() ; j++)
        c_par += params[j];

    for (size_t j = 0 ; j < c_par.size() ; j++)
    {
        if (c_par[j] == ' ')
            ;
        else if (c_par[j] == '+')
            ar = true;
        else if (c_par[j] == '-')
            ar = false;
        else if (ar && (modes.find(c_par[j]) != std::string::npos))
            cmds.push_back(std::string("+") + c_par[j]);
        else if (ar && (pmodes.find(c_par[j]) != std::string::npos))
        {
            cmds.push_back(std::string("+") + c_par[j] + " ");
            size_t k = j;
            size_t begin;
            size_t end;

            while (c_par[k] != ' ') k++;
            while (c_par[k] == ' ') k++;
            begin = k;
            while (c_par[k] && c_par[k] != ' ')
            {
                cmds.back() += c_par[k];
                k++;
            }
            end = k;
            c_par.erase(begin, end - begin);
        }
        else if (!ar && (modes.find(c_par[j]) != std::string::npos))
            cmds.push_back(std::string("-") + c_par[j]);
        else if (!ar && (pmodes.find(c_par[j]) != std::string::npos))
        {
            cmds.push_back(std::string("-") + c_par[j] + " ");
            size_t k = j;
            size_t begin;
            size_t end;

            while (c_par[k] != ' ') k++;
            while (c_par[k] == ' ') k++;
            begin = k;
            while (c_par[k] && c_par[k] != ' ')
            {
                cmds.back() += c_par[k];
                k++;
            }
            end = k;
            c_par.erase(begin, end - begin);
        }
        else if (modes.find(c_par[j]) == std::string::npos || pmodes.find(c_par[j]) == std::string::npos)
            cmds.push_back(std::string("e") + c_par[j]);
    }
    treat_modes(params, cmds, prefix);
}

void Server::treat_modes(std::vector<std::string> params, std::vector<std::string> cmds, std::string prefix)
{
    std::string to_add = "+";
    std::string to_remove = "-";

    (void)prefix;
    for (size_t i = 0 ; i < cmds.size() ; i++)
    {
        if (cmds[i][0] == '+' && cmds[i].size() == 2)
        {
            if (_m_flags[params[1]].find(cmds[i][1]) == std::string::npos)
                _m_flags[params[1]].push_back(cmds[i][1]);
        }
        if (cmds[i][0] == '-' && cmds[i].size() == 2)
        {
            if (_m_flags[params[1]].find(cmds[i][1]) != std::string::npos)
                _m_flags[params[1]].erase(_m_flags[params[1]].find(cmds[i][1]));
        }
        else if (cmds[i][0] == '+' || cmds[i][0] == '-')
        {
            treat_args(params[1], cmds[i], prefix);
        }
    }

}

void Server::treat_args(std::string chan, std::string cmd, std::string prefix)
{
    std::string arg;
    size_t i = 0;

    while (cmd[i] != ' ') i++;
    while (cmd[i] == ' ') i++;

    (void)prefix;
    if (cmd[i])
        arg = cmd.substr(i);
    std::cout << arg.size() << "\n";
    if (cmd[1] == 'b' && arg.size())
    {
        size_t u1;
        size_t u2;
        std::string ban;

        u1 = arg.find('!');
        u2 = arg.find('@');
        if (u1 != std::string::npos && u2 != std::string::npos && u1 < u2)
            ban = arg;
        else if (u1 != std::string::npos && u2 == std::string::npos)
            ban = arg + "@*";
        else if (u2 != std::string::npos && u2 < u1)
            ban = arg.substr(0, arg.length() - u2) + "!*" + arg.substr(u2);
        else if (u2 == u1 && u2 == std::string::npos)
            ban = arg + "!*@*";
        for (std::vector<std::string>::iterator it = _m_banmask[chan].begin() ; it != _m_banmask[chan].end() ; it++)
        {
            std::cout << *it << "\n";
            if (*it == ban && cmd[0] == '-')
            {
                _m_banmask[chan].erase(it);
                return ;
            }
            if (*it == ban && cmd[0] == '+')
                return ;
        }
        _m_whoban[ban] = _m_pclients[prefix]->nickname;
        _m_banmask[chan].push_back(ban);
        _m_banid[ban] = reinterpret_cast<uint64_t>(&_m_banmask[chan].back());
        std::cout << "ban : " << _m_banmask[chan].back() << "\n";
        std::cout << "who : " << _m_whoban[_m_banmask[chan].back()] << "\n";
        std::cout << "id : " << _m_banid[_m_banmask[chan].back()] << "\n";
    }
    else if (cmd[1] == 'b')
    {
        std::cout << "bob1\n";
        send_reply(chan, prefix, 367);
        send_reply("", prefix, 368);
    }
}
