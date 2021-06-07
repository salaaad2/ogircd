#include "../inc/Params.hpp"

#include <cstdlib>

Params::Params() {}
Params::~Params() {}

Params::Params(int ac, std::vector<std::string> av)
{
    if (ac != 3)
    {
        std::cerr << "wrong parameters\n";
        return ;
    }
    else if (ac == 3)
    {
        _port = atoi(av[0].c_str());
        _password = av[1];
    }
}
