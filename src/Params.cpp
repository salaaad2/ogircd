#include "../inc/Params.hpp"

Params::Params() {}
Params::~Params() {}

Params::Params(int ac, std::vector<std::string> av)
{
    if (ac != 3 && ac != 4)
    {
        std::cout << "wrong parameters\n";
        return ;
    }

    if (ac == 4)
    {
        size_t fst = av[0].find(':');
        size_t scd = av[0].find(':', fst + 1);
        size_t no = av[0].find(':', scd + 1);

        if (fst == std::string::npos || scd == std::string::npos || no != std::string::npos)
            return ;

        _host =  av[0].substr(0, fst);
        _port_network =  ft_atoi(av[0].substr(fst + 1, scd - fst - 1).c_str());
        _password_network = av[0].substr(scd + 1, av[0].length() - scd);
        _port = ft_atoi(av[1].c_str());
        _password = av[2];
        _isnew = false;
        std::cout << "here : " << av[0] << "<-- 4 params\n";
    }
    else
    {
        _port = ft_atoi(av[0].c_str());
        std::cout << av[0] << "<-- 3 params\n";
        _password = av[1];
        _isnew = true;
    }
}
