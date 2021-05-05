/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Params.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbajrami <tbajrami@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/30 10:42:18 by tbajrami          #+#    #+#             */
/*   Updated: 2021/05/03 16:55:21 by tbajrami         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ftirc.hpp"

class Params
{
private:

	char    *_host;
	char    *_port_network;
	char    *_password_network;
	char    *_port;
	char    *_password;
	bool    _isnew;

public:

	Params(int ac, char **av) : _host(new char[128]), _port_network(new char[128]), _password_network(new char[128]),
								_port(new char[128]), _password(new char[128]), _isnew(true)
	{
		if (ac != 3 && ac != 4)
		{
			std::cout << "wrong parameters\n";
			return ;
		}
		std::string av1(av[1]);

		if (ac == 4)
		{
			size_t fst = av1.find(':');
			size_t scd = av1.find(':', fst + 1);
			size_t no = av1.find(':', scd + 1);

			if (fst == std::string::npos || scd == std::string::npos || no != std::string::npos)
				return ;

			strcpy(_host, (av1.substr(0, fst).c_str()));
			strcpy(_port_network, av1.substr(fst + 1, scd - fst - 1).c_str());
			strcpy(_password_network, av1.substr(scd + 1, av1.length() - scd).c_str());
			strcpy(_port, av[2]);
			strcpy(_password, av[3]);
			_isnew = false;
		}
		else
		{
			strcpy(_port, av[1]);
			strcpy(_password, av[2]);
		}
	}

	char	*getHost() const {return _host;}
	char	*getPwdNetwork() const {return _password_network;}
	char	*getPwd() const {return _password;}

	int		getPortNetwork() const {return ft_atoi(_port_network);}
	int		getPort() const {return ft_atoi(_port);}
	bool	isnew() const {return _isnew;}
};