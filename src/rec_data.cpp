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
    std::vector<Message *> vm;
    char buf[512];
    int nbytes;

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
        vm = parse_message(buf);
        for (std::vector<Message*>::iterator it = vm.begin(); it!=vm.end(); ++it) {
            if ((*it)->command.size() > 0)
            {
                // std::cout << "FD REC DATA : " << i << "\n";
                serv.do_command(*it, i);
            }
        }
    }
}
