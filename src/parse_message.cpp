/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_message.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbajrami <tbajrami@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/05 13:41:20 by tbajrami          #+#    #+#             */
/*   Updated: 2021/05/30 14:08:08 by tbajrami         ###   ########lyon.fr   */
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

int get_params(char buf[], Message *nm, size_t i)
{
    while (buf[i] && buf[i] != '\r' && buf[i] == ' ')
        i++;
    while (buf[i] && buf[i] != '\r')
    {
        if (buf[i] == ' ')
            i++;
        else if (buf[i] != ':')
        {
            std::string param;
            while (buf[i] && buf[i] != '\r' && buf[i] != ' ')
            {
                param += buf[i];
                i++;
                nm->len++;
            }
            nm->params.push_back(param);
        }
        else if (buf[i] == ':')
        {
            i++;
            std::string param;
            while (buf[i] && buf[i] != '\r')
            {
                param += buf[i];
                nm->len++;
                i++;
            }
            nm->params.push_back(param);
        }
    }
    if (buf[i] == '\r')
         return (i + 2);
    return i;
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
    while (nc < n) {
        if ((i = get_command(buf, nm, i)) == 0) {
            break;
        }
        else
            i = get_params(buf, nm, i);
        vm.push_back(nm);
        //std::cout << "nm command : " << nm->command << "\n";
        nc++;
        if (nc != n)
            nm = new Message;
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
    return vm;
}
