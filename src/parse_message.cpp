/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_message.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbajrami <tbajrami@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/05 13:41:20 by tbajrami          #+#    #+#             */
/*   Updated: 2021/05/20 14:37:36 by tbajrami         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ftirc.hpp"
#include "../inc/Server.hpp"

size_t
count_commands(char buf[])
{
    std::string tmp(buf);
    size_t a;
    size_t c;

    c = 0;
    for (a = tmp.find("\r\n");
         a != std::string::npos;
         a = tmp.find("\r\n", a + 2))
    {
        c++;
    }
    return (c);
}

int
get_command(char buf[], Message *nm, size_t i)
{
    //  std::cout << "buf : " << buf;
    // std::cout << "got command : " << nm->command << "\n";
    while (buf[i] && buf[i] != ' ' && buf[i] != '\r') {
        nm->command += buf[i];
        i++;
    }
    nm->len += nm->command.length();
    return (i);
}

int
get_params(char buf[], Message *nm, size_t i) {
    std::string tmp;
    std::string sep_char;
    while (buf[i] && buf[i] != '\r' && (buf[i] == ' '
            || buf[i] == ':'))
    {
        i++;
    }
    while (buf[i] && buf[i] != '\r') {
        if (buf[i] == ':' || buf[i] == ' ' || buf[i] == ',' || buf[i] == '#')
        {
            if (tmp != "")
            {
                nm->params.push_back(tmp);
                nm->len += tmp.length();
                tmp.clear();
            }
            sep_char += buf[i];
            nm->params.push_back(sep_char);
            nm->len += sep_char.length();
            sep_char.clear();
            // if (buf[i] && buf[i] != ' ')
            // {
            //     tmp += buf[i];
            //     nm->params.push_back(tmp);
            //     tmp.clear();
            // }
        }
        else
            tmp += buf[i];
        i++;
    }
    if (tmp != "")
        nm->params.push_back(tmp);
    nm->len += tmp.length();
    if (buf[i] == '\r')
        return (i + 2);
    return  (i);
}

std::vector<Message*>
parse_message(char buf[])
{
    std::vector<Message*> vm;
    Message *nm = new Message;
    size_t i = 0;
    size_t nc = 0;
    size_t n = 0;

    n = count_commands(buf);
    while (nc != n) {
        if ((i = get_command(buf, nm, i)) == 0) {
            break;
        }
        else
            i = get_params(buf, nm, i);
        std::cout << "nm command : " << nm->command << "\n";
        vm.push_back(nm);
        nc++;
        nm = new Message;
    }
    return vm;
}

// std::cout << "count : " << n <<  "\n";
// std::cout << "command number : " << nc << "command : " << nm->command <<
// "first param : " << nm->params[0] << "\n";
// i = 0;
// std::cout << "CMD " << nm->command <<  "\n";
// while (i < nm->params.size()) {
//     std::cout << "[" << nm->params[i] << "] ";
//     i++;
// }
// std::cout << "\n";
