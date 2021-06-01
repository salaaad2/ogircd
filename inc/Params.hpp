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
	int _port;
	std::string _password;

public:

		Params();
		Params(int ac, std::vector<std::string>);
		~Params();

		std::string getHost() const
		{return _host;}
		void setHost(std::string & new_host)
		{ _host = new_host;}


		std::string getPwd() const
		{return _password;}
		void setPwd(std::string & new_pwd)
		{ _password = ft_atoi(new_pwd.c_str());}


		int		getPort() const
		{return _port;}
		void setPort(std::string & new_port)
		{ _port = ft_atoi(new_port.c_str());}
};
