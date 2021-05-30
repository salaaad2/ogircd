#include "../inc/Server.hpp"

void Server::send_to_channel(std::string to_send, std::string chan)
{
    for (std::vector<Client *>::iterator it = _m_chans[chan].begin() ; it != _m_chans[chan].end() ; it++)
            send((*it)->clfd, to_send.c_str(), to_send.length(), 0);
}

void Server::modecmd(Message *msg, std::string prefix)
{
    if (msg->params.empty())
        send_reply("", prefix, ERR_NEEDMOREPARAMS);
    else if (msg->params[0][0] == '#' || msg->params[0][0] == '&')
        chanMode(msg->params, prefix);
    std::cout << "flags after " << _m_flags[msg->params[0]] << "\r\n";
    // else
    //     userMode(msg->params, prefix);
}

void Server::chanMode(std::vector<std::string> params, std::string prefix)
{
    std::string chan = params[0];

    if (_m_chans.find(params[0]) == _m_chans.end())
        send_reply(params[0], prefix, ERR_NOSUCHCHANNEL);
    else if (_m_uflags[chan][_m_pclients[prefix]].find('o') == std::string::npos)
        send_reply(chan, prefix, ERR_CHANOPRIVSNEEDED);
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

    for (size_t j = 1 ; j < params.size() ; j++)
        c_par += params[j] + ' ';

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

            while (c_par[k] && c_par[k] != ' ') k++;
            while (c_par[k] && c_par[k] == ' ') k++;
            begin = k;
            while (c_par[k] && c_par[k] != ' ')
            {
                cmds.back() += c_par[k];
                k++;
            }
            end = k;
            if (begin != end)
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

            while (c_par[k] && c_par[k] != ' ') k++;
            while (c_par[k] && c_par[k] == ' ') k++;
            begin = k;
            while (c_par[k] && c_par[k] != ' ')
            {
                cmds.back() += c_par[k];
                k++;
            }
            end = k;
            if (begin != end)
                c_par.erase(begin, end - begin);
        }
        else if (modes.find(c_par[j]) == std::string::npos || pmodes.find(c_par[j]) == std::string::npos)
            cmds.push_back(std::string(".") + c_par[j]);
    }
    treat_modes(params, cmds, prefix);
}

void Server::treat_modes(std::vector<std::string> params, std::vector<std::string> cmds, std::string prefix)
{
    std::string cmd;
    bool ar;
    std::string chan = params[0];

    for (size_t i = 0 ; i < cmds.size() ; i++)
    {
        if (cmds[i][0] == '+' && cmds[i].size() == 2)
        {
            if (_m_flags[chan].find(cmds[i][1]) == std::string::npos)
            {
                if (cmd.empty() || !ar)
                {
                    ar = true;
                    cmd += "+";
                }
                _m_flags[chan].push_back(cmds[i][1]);
                cmd += cmds[i][1];
            }
        }
        else if (cmds[i][0] == '-' && cmds[i].size() == 2)
        {
            if (_m_flags[chan].find(cmds[i][1]) != std::string::npos)
            {
                if (cmd.empty() || ar)
                {
                    ar = false;
                    cmd += "-";
                }
                _m_flags[chan].erase(_m_flags[chan].find(cmds[i][1]), 1);
                cmd += cmds[i][1];
            }
        }
        else if (cmds[i][0] == '+' || cmds[i][0] == '-')
            treat_args(chan, cmds[i], prefix);
    }
    if (!cmd.empty())
        send_to_channel(prefix + " MODE " + chan + " " + cmd + "\r\n", chan);
}

void Server::treat_args(std::string chan, std::string cmd, std::string prefix)
{
    std::string arg;
    size_t i = 0;

    while (cmd[i] != ' ') i++;
    while (cmd[i] == ' ') i++;

    if (cmd[i])
        arg = cmd.substr(i);
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
            if (*it == ban && cmd[0] == '-')
            {
                _m_banmask[chan].erase(it);
                send_to_channel(prefix + " MODE " + chan + " -b " + ban + "\r\n", chan);
                return ;
            }
            if (*it == ban && cmd[0] == '+')
                return ;
        }
        _m_whoban[ban] = _m_pclients[prefix]->nickname;
        _m_banmask[chan].push_back(ban);
        send_to_channel(prefix + " MODE " + chan + " +b " + ban + "\r\n", chan);
        _m_banid[ban] = reinterpret_cast<uint64_t>(&_m_banmask[chan].back());
    }
    else if (cmd[1] == 'b')
    {
        for (std::vector<std::string>::iterator it = _m_banmask[chan].begin() ; it != _m_banmask[chan].end() ; it++)
            send_reply(chan + " " + *it + " " + _m_whoban[*it] + " " + ft_utoa(_m_banid[*it]), prefix, RPL_BANLIST);
        send_reply("", prefix, RPL_ENDOFBANLIST);
    }
    else if (cmd[1] == 'e' && arg.size())
    {
        size_t u1;
        size_t u2;
        std::string except;

        u1 = arg.find('!');
        u2 = arg.find('@');
        if (u1 != std::string::npos && u2 != std::string::npos && u1 < u2)
            except = arg;
        else if (u1 != std::string::npos && u2 == std::string::npos)
            except = arg + "@*";
        else if (u2 != std::string::npos && u2 < u1)
            except = arg.substr(0, arg.length() - u2) + "!*" + arg.substr(u2);
        else if (u2 == u1 && u2 == std::string::npos)
            except = arg + "!*@*";
        for (std::vector<std::string>::iterator it = _m_exceptmask[chan].begin() ; it != _m_exceptmask[chan].end() ; it++)
        {
            if (*it == except && cmd[0] == '-')
            {
                _m_exceptmask[chan].erase(it);
                send_to_channel(prefix + " MODE " + chan + " -e " + except + "\r\n", chan);
                return ;
            }
            if (*it == except && cmd[0] == '+')
                return ;
        }
        _m_whoexcept[except] = _m_pclients[prefix]->nickname;
        _m_exceptmask[chan].push_back(except);
        send_to_channel(prefix + " MODE " + chan + " +e " + except + "\r\n", chan);
        _m_exceptid[except] = reinterpret_cast<uint64_t>(&_m_exceptmask[chan].back());
    }
    else if (cmd[1] == 'e')
    {
        for (std::vector<std::string>::iterator it = _m_exceptmask[chan].begin() ; it != _m_exceptmask[chan].end() ; it++)
            send_reply(chan + " " + *it + " " + _m_whoexcept[*it] + " " + ft_utoa(_m_exceptid[*it]), prefix, RPL_EXCEPTLIST);
        send_reply("", prefix, RPL_ENDOFEXCEPTLIST);
    }
    else if (cmd[1] == 'v' && arg.size())
    {
        for (std::vector<Client *>::iterator it = _m_chans[chan].begin() ; it != _m_chans[chan].end() ; it++)
        {
            if ((*it)->nickname == arg)
            {
                if (cmd[0] == '+')
                {
                    if (_m_uflags[chan][_m_pclients[(*it)->prefix]].find('v') == std::string::npos)
                    {
                        _m_uflags[chan][_m_pclients[(*it)->prefix]].push_back('v');
                        send_to_channel(prefix + " MODE " + chan + " +v " + arg + "\r\n", chan);
                    }
                    return ;
                }
                else if (cmd[0] == '-')
                {
                    if (_m_uflags[chan][_m_pclients[(*it)->prefix]].find('v') != std::string::npos)
                    {
                        _m_uflags[chan][_m_pclients[(*it)->prefix]].erase(_m_uflags[chan][_m_pclients[(*it)->prefix]].find('v'), 1);
                        send_to_channel(prefix + " MODE " + chan + " -v " + arg + "\r\n", chan);
                    }
                    return ;
                }
            }
        }
        send_reply("", prefix, ERR_NOSUCHNICK);
    }
    else if (cmd[1] == 'o' && arg.size())
    {
        for (std::vector<Client *>::iterator it = _m_chans[chan].begin() ; it != _m_chans[chan].end() ; it++)
        {
            if ((*it)->nickname == arg)
            {
                if (cmd[0] == '+')
                {
                    std::cout << _m_uflags[chan][_m_pclients[(*it)->prefix]] << "\n";
                    if (_m_uflags[chan][_m_pclients[(*it)->prefix]].find('o') == std::string::npos)
                    {
                        _m_uflags[chan][_m_pclients[(*it)->prefix]].push_back('o');
                        send_to_channel(prefix + " MODE " + chan + " +o " + arg + "\r\n", chan);
                    }
                    return ;
                }
                else if (cmd[0] == '-')
                {
                    if (_m_uflags[chan][_m_pclients[(*it)->prefix]].find('o') != std::string::npos)
                    {
                        _m_uflags[chan][_m_pclients[(*it)->prefix]].erase(_m_uflags[chan][_m_pclients[(*it)->prefix]].find('o'), 1);
                        send_to_channel(prefix + " MODE " + chan + " -o " + arg + "\r\n", chan);
                    }
                    return ;
                }
            }
        }
        send_reply("", prefix, ERR_NOSUCHNICK);
    }
    else if (cmd[1] == 'k')
    {
        if (cmd[0] == '+' && arg.size())
        {
            _m_chankey[chan] = arg;
            if (_m_flags[chan].find('k') == std::string::npos)
                _m_flags[chan].push_back('k');
            send_to_channel(prefix + " MODE " + chan + " +k " + arg + "\r\n", chan);
        }
        else if (cmd[0] == '-' && _m_flags[chan].find('k') != std::string::npos)
        {
            _m_flags[chan].erase(_m_flags[chan].find('k'), 1);
            _m_chankey[chan] = "";
            send_to_channel(prefix + " MODE " + chan + " -k *" + "\r\n", chan);
        }
    }
    else if (cmd[1] == 'l')
    {
        if (cmd[0] == '+' && arg.size())
        {
            _m_flags[chan].push_back('l');
            _m_limits[chan] = ft_atoi(arg.c_str());
            send_to_channel(prefix + " MODE " + chan + " +l " + arg + "\r\n", chan);
        }
        else if (cmd[0] == '-' && _m_flags[chan].find('l') != std::string::npos)
        {
            _m_flags[chan].erase(_m_flags[chan].find('l'), 1);
            send_to_channel(prefix + " MODE " + chan + " -l" + "\r\n", chan);
        }
    }
}

