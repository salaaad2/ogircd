#include "../inc/Params.hpp"

Params::Params() {}
Params::~Params() {}

Params::Params(int ac, std::vector<std::string> av)
{
    if (ac != 3)
    {
        std::cerr << "wrong parameters\n";
        return ;
    }
    else
    {
        _port = ft_atoi(av[0].c_str());
        _password = av[1];
    }
}
