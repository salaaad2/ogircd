#include "../inc/Params.hpp"

Params::Params() {}
Params::~Params() {}

Params::Params(int ac, std::vector<std::string> av)
{
    if (ac != 3)
    {
        std::cout << "wrong parameters\n";
        return ;
    }
    else
    {
        _port = ft_atoi(av[0].c_str());
        std::cout << av[0] << "<-- 3 params\n";
        _password = av[1];
    }
}
