/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_message.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbajrami <tbajrami@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/05 13:41:20 by tbajrami          #+#    #+#             */
/*   Updated: 2021/05/19 13:18:43 by tbajrami         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ftirc.hpp"
#include "../inc/Server.hpp"

int
get_command(char buf[], Message *nm)
{
    int i = 0;

    while (buf[i] && buf[i] != ' ' && buf[i] != '\r') {
        nm->command += buf[i];
        i++;
    }
    return (i);
}

void
get_params(char buf[], Message *nm, int i) {
    std:: string tmp;
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
                tmp.clear();
            }
            sep_char += buf[i];
            nm->params.push_back(sep_char);
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
    nm->params.push_back(tmp);
}

Message *parse_message(char buf[])
{
    Message *nm = new Message;
    int i = 0;

    i = get_command(buf, nm);
    get_params(buf, nm, i);

    std::cout << "CMD " << nm->command <<  "\n";
    i = 0;
    while (i < nm->params.size()) {
        std::cout << "[" << nm->params[i] << "] ";
        i++;
    }
    return nm;
}
