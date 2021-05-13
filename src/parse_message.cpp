/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_message.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbajrami <tbajrami@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/05 13:41:20 by tbajrami          #+#    #+#             */
/*   Updated: 2021/05/13 13:40:15 by tbajrami         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "ftirc.hpp"
#include "Server.hpp"

int add_prefix(char pr[], char buf[])
{
    int i = 1;
    int j = 0;
    
    while (buf[i] != ' ')
    {
        pr[j] = buf[i];
        i++;
        j++;
    }
    while (buf[i] == ' ')
        i++;
    return i;
}

int add_cmd(char cmd[], char buf[], int i)
{
    int j = 0;

    if (buf[i] > 47 && buf[i] < 58)
    {
        cmd[0] = buf[i];
        cmd[1] = buf[i + 1];
        cmd[2] = buf[i + 2];
        i += 3;
    }
    else while ((buf[i] > 64 && buf[i] < 91) || (buf[i] > 96 && buf[i] < 123))
    {
        cmd[j] = buf[i];
        i++;
        j++;
    }
    while (buf[i] == ' ')
        i++;
    return i;
}

void add_params(char params[32][32], char buf[], int i)
{
    int j = 0;
    int k = 0;

    while (buf[i] != '\r')
    {
        if (buf[i] != ':')
        {
            while (buf[i] != ' ')
            {
                params[j][k] = buf[i];
                i++;
                k++;
            }
            j++;
            k = 0;
            while (buf[i] == ' ')
                i++;
        }
        else
        {
            i++;
            while (buf[i] != '\r')
            {
                params[j][k] = buf[i];
                i++;
                k++;
            }
        }
    }
    j++;
    params[j][0] = 0;
}

Message *parse_message(Server &serv, char buf[])
{
    Message *nm = new Message;
    int i = 1;
    int j = 0;

    ft_bzero(nm, 1152);
    i = add_prefix(nm->prefix, buf);
    i = add_cmd(nm->command, buf, i);
    add_params(nm->params, buf, i);

    std::cout << "\nprefix : " << nm->prefix << "\ncmd : " << nm->command;
    for (int i = 0 ; nm->params[i][0] != 0 ; i++)
        std::cout << "\nparam : " << nm->params[i];
    return nm;
}