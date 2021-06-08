#include "../inc/Params.hpp"

#include <cstdlib>

Params::Params()
{
}
Params::~Params()
{
}

Params::Params(std::vector<std::string> av)
{
	if (av.size() != 2) {
		std::cerr << "wrong parameters\n";
		return;
	} else {
		_port = atoi(av[0].c_str());
		_password = av[1];
	}
}
