/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rec_data.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbajrami <tbajrami@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/03 11:02:36 by tbajrami          #+#    #+#             */
/*   Updated: 2021/05/13 13:56:17 by tbajrami         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ftirc.hpp"
#include "../inc/Server.hpp"

void rec_data(Server &serv, std::map<int, Client> *clients, int i, Fds *fds)
{
    int nbytes;
    char buf[512];
    struct sockaddr_in n;

    ft_bzero(buf, 512);
    if ((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0)
    {
        if(nbytes == 0)
            std::cout << "socket " << i << " hung up\n";
        else
            std::cout << "recv() error\n";
        close(i);
        FD_CLR(i, &fds->master);
    }
    else
    {
        std::cout << buf;
        serv.do_command(parse_message(serv, buf), clients->at(i));
    //     for(int j = 0; j <= fds->fdmax; j++)
    //     {
    //         if(FD_ISSET(j, &fds->master))
    //         {
    //             if(j != serv.listener && j != i)
    //             {
    //                 if(send(j, buf, nbytes, 0) == -1)
    //                     perror("send() error");
    //             }
    //         }
    //     }
    }
}