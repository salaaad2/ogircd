#include "../inc/ftirc.hpp"
#include "../inc/Server.hpp"

int postSelectLoop(Server &serv, Fds *fds)
{
	int newfd = 0;

	for (int i = 0; i <= fds->fdmax; i++) {
		if (FD_ISSET(i, &fds->read)) {
			if (i == serv.listener) {
				if ((newfd = serv.addclient(serv.listener)) !=
				    -1) {
#ifdef DEBUG_IRC
					std::cout << "debug: New fd : " << i
						  << std::endl;
#endif
					if (newfd > fds->fdmax)
						fds->fdmax = newfd;
					return newfd;
				}
			} else
				rec_data(serv, i, fds);
		}
	}
	return 0;
}

std::vector<string> parseParams(size_t ac, char **av)
{
	std::vector<string> vec;
	size_t i = 0;

	for (i = 1; i < ac; i++) {
		vec.push_back(string(av[i]));
	}
	return (vec);
}

int main(int ac, char *av[])
{
	if (ac < 3) {
		std::cerr
			<< "Insufficient parameters. \nUsage : ./ircserv [PORT] [PASS]"
			<< std::endl;
		return (1);
	}
	std::vector<string> pvec = parseParams(ac, av);
	Server serv(pvec);
	Fds *fds = new Fds;
	int newfd;
	serv.setFds(fds);

	FD_ZERO(&fds->master);
	FD_ZERO(&fds->read);
	FD_SET(serv.listener, &fds->master);
	fds->fdmax = serv.listener;
	for (;;) {
		if (serv.getStatus() == 0) {
			std::cerr << "exit\n";
			delete fds;
			return (0);
		}
		fds = serv.getFds();
		fds->read = fds->master;
		if (select(fds->fdmax + 1, &fds->read, NULL, NULL, NULL) ==
		    -1) {
			std::cerr << "Server-select() error";
			exit(1);
		}
		if ((newfd = postSelectLoop(serv, fds)) > 0)
			FD_SET(newfd, &fds->master);
	}
	return 0;
}
