#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int ac, char **av)
{
	// create socket
    int net_socket = socket(AF_INET, SOCK_STREAM, 0);
	char pseudo[100];

	//strcpy(pseudo, av[1]);

    // specify an address for the socket
    struct sockaddr_in server_address;

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(atoi(av[1]));
    server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_addr.s_addr = inet_addr("10.4.5.7");


    int connection_status = connect(net_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    if (connection_status == -1)
        printf("Error making connection to the remote socket\n\n");

	//send(net_socket, "password", strlen("password"), 0);

	char message[512] = "";
	//server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
	// strcpy(message, ":user!user@host cmd blabla dodo :ffrr  b\r\n");
	// send(net_socket, message, strlen(message), 0);
	bzero(message, 512);
    while(1)
	{
		//printf("%s: ", pseudo);
		fgets(message, 512, stdin);
		// send(net_socket, pseudo, strlen(pseudo), 0);
		// send(net_socket, ": ", 2, 0);
		send(net_socket, message, strlen(message), 0);
		bzero(message, 512);
	}
    close(net_socket);

    return 0;
}