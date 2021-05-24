#include "../inc/Server.hpp"

void Server::servercmd(Message *msg, int fd)
{
    (void)msg;
    std::string s = ft_utoa(fd);
    std::string host = _m_pclients[s]->host;
    _m_fdserver[fd] = host;
    std::cout << "New connection from server " << host << " on fd " << fd << "\n";
    send(fd, "PASS :CC\r\n", strlen("PASS: CC\r\n"), 0);
    send(fd, "SERVER", strlen("SERVER"), 0);
}

void Server::connectcmd(Message *msg, std::string prefix) //TODO : check priv
{
   //     Command: CONNECT
   // Parameters: <target server> [<port> [<remote server>]]

   // The CONNECT command can be used to force a server to try to establish
   // a new connection to another server immediately.  CONNECT is a
   // privileged command and is to be available only to IRC Operators.  If
   // a remote server is given then the CONNECT attempt is made by that
   // server to <target server> and <port>.

   // Numeric Replies:

   //         ERR_NOSUCHSERVER                ERR_NOPRIVILEGES
   //         ERR_NEEDMOREPARAMS
    size_t i = 0;
    std::string target;
    std::string port;
    std::string remote_server;
    int connection_status;
    struct sockaddr_in server_address;
    int net_socket;
    while (i < msg->params.size())
    {
        if (i == 0)
            target = msg->params[i];
        else if (i == 2)
            port = msg->params[i];
        else if (i == 4)
            remote_server = msg->params[i];
        i++;
    }
    if (i == 0) {
        send_reply("", prefix, ERR_NEEDMOREPARAMS);
    }
    else
    {
        server_address.sin_family = AF_INET;
        if (port != "")
            server_address.sin_port = htons(atoi(port.c_str()));
        else
            server_address.sin_port = htons(6667);
        server_address.sin_addr.s_addr = inet_addr(target.c_str());
    }
    net_socket = socket(AF_INET, SOCK_STREAM, 0);
    connection_status = connect(net_socket, (struct sockaddr*)&server_address, sizeof(server_address));
    if (connection_status == -1)
        send_reply("", prefix, ERR_NOSUCHSERVER);
    else
    {
        send(net_socket, "PASS :CC\r\n", strlen("PASS :CC\r\n"), 0);
        send(net_socket, "SERVER\r\n", strlen("SERVER\r\n"), 0);
    }
}
