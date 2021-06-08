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
		Params(std::vector<std::string> av);
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
