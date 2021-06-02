#include "../inc/ftirc.hpp"
#include "../inc/Message.hpp"
#include "../inc/Server.hpp"

void rec_data(Server &serv, int i, Fds *fds)
{
    std::vector<Message *> vm;
    std::string tmp;
    char buf[512];
    int nbytes;

    ft_bzero(buf, 512);
    if ((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0)
    {
        close(i);
        FD_CLR(i, &fds->master);
    }
    tmp = buf;
    serv._m_fdreq[i] += tmp;
    if (tmp.find("\r") != std::string::npos)
    {
        vm = parse_message(serv._m_fdreq[i]);
        for (std::vector<Message*>::iterator it = vm.begin(); it!=vm.end(); ++it) {
            if ((*it)->command.size() > 0)
            {
                serv.do_command(*it, i);
            }
        }
        serv._m_fdreq[i].clear();
    }
}
