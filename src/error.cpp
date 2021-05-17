/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbajrami <tbajrami@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/06 18:12:30 by tbajrami          #+#    #+#             */
/*   Updated: 2021/05/06 18:14:07 by tbajrami         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/replies.hpp"

const char *msg_error(int code)
{
    switch(code) {
        case  ERR_ALREADYREGISTERED :
            return (":You may not reregister");
        case ERR_NICKNAMEINUSE :
            return(":Nickname already in use");
    }
}
