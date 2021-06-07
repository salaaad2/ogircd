#include "../inc/ftirc.hpp"
#include "../inc/Params.hpp"
#include "../inc/Server.hpp"

int main_loop(Server &serv, Fds *fds)
{
	int newfd = 0;

	for (int i = 0 ; i <= fds->fdmax ; i++)
	{
		if(FD_ISSET(i, &fds->read))
		{
			if (i == serv.listener)
			{
				if ((newfd = serv.addclient(serv.listener)) != -1)
				{
#ifdef DEBUG_IRC
				std::cout << "debug: New fd : " << i << std::endl;
#endif
					if(newfd > fds->fdmax)
						fds->fdmax = newfd;
					return newfd;
				}
			}
			else
				rec_data(serv, i, fds);
		}
	}
	return 0;
}

std::vector<std::string> getParams(size_t  ac, char **av)
{
	std::vector<std::string> vec;
	size_t i = 0;

	for (i = 1; i < ac; i++)
	{
		vec.push_back(std::string(av[i]));
		// std::cout << "i : " << i << std::endl;
	}
	// std::cout << "[" << 0 << "]" << vec[0] << "\n";
	// std::cout << "[" << 1 << "]" << vec[1] << "\n";
	return (vec);
}

int main(int ac, char *av[])
{
	if (ac < 3)
	{
		std::cerr << "Insufficient parameters. \nUsage : ./ircserv [PORT] [PASS]" << std::endl;
		return (1);
	}
	std::vector<std::string> vec = getParams(ac, av);
	Params * pm = new Params(ac, vec);
	Server serv(pm);
	int newfd;
	Fds *fds = serv.getFds();

	FD_ZERO(&fds->master);
	FD_ZERO(&fds->read);
	FD_SET(serv.listener, &fds->master);
	fds->fdmax = serv.listener;
	for(;;)
	{
		if (serv.getStatus() == 0) {
			std::cerr << "exit\n";
			return (0);
		}
		fds = serv.getFds();
		fds->read = fds->master;
		if (select(fds->fdmax + 1, &fds->read, NULL, NULL, NULL) == -1)
		{
			std::cerr << "Server-select() error";
			exit(1);
		}
		if ((newfd = main_loop(serv, fds)) > 0)
			FD_SET(newfd, &fds->master);
	}
	return 0;
}
