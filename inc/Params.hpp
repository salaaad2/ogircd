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

#include "utils.hpp"
#include <iostream>
#include <vector>
class Params
{
private:

	std::string _host;
	int _port_network;
	std::string _password_network;
	int _port;
	std::string _password;
	bool    _isnew;

public:

	Params(int ac, std::vector<std::string> av)

	{
		if (ac != 3 && ac != 4)
		{
			std::cout << "wrong parameters\n";
			return ;
		}

		if (ac == 4)
		{
			size_t fst = av[1].find(':');
			size_t scd = av[1].find(':', fst + 1);
			size_t no = av[1].find(':', scd + 1);

			if (fst == std::string::npos || scd == std::string::npos || no != std::string::npos)
				return ;

			_host =  av[1].substr(0, fst);
			_port_network =  ft_atoi(av[1].substr(fst + 1, scd - fst - 1).c_str());
			_password_network = av[1].substr(scd + 1, av[1].length() - scd);
			_port = ft_atoi(av[2].c_str());
			_password = av[3];
			_isnew = false;
		}
		else
		{
			_port = ft_atoi(av[1].c_str());
			_password = av[2];
			_isnew = true;
		}
	}

		std::string getHost() const {return _host;}
		void setHost(std::string new_host) { _host = new_host;}
		std::string getPwdNetwork() const {return _password_network;}
		void setPwdNetwork(std::string new_pwd) { _password_network = ft_atoi(new_pwd.c_str());}
		std::string getPwd() const {return _password;}
		void setPwd(std::string new_pwd) { _password = ft_atoi(new_pwd.c_str());}

		int		getPortNetwork() const {return _port_network;}
		void setPortNetwork(std::string new_port) { _port_network = ft_atoi(new_port.c_str());}
		int		getPort() const {return _port;}
		void setPort(std::string new_port) { _port = ft_atoi(new_port.c_str());}
		bool	isnew() const {return _isnew;}
		void setIsNew(bool is_new) { _isnew = is_new;}
};
