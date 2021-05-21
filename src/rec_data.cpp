/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rec_data.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbajrami <tbajrami@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/03 11:02:36 by tbajrami          #+#    #+#             */
/*   Updated: 2021/05/20 14:21:59 by tbajrami         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ftirc.hpp"
#include "../inc/message.hpp"
#include "../inc/Server.hpp"

void rec_data(Server &serv, int i, Fds *fds)
{
    int nbytes;
    char buf[512];
    Message ** unsp;
    size_t n = 1;

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
        std::cout << "data received  : {" << buf << "}\n";
        unsp = parse_message(buf);
        while (n < unsp[0]->count) {
            serv.do_command(unsp[n], i);
        }

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
