#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main()
{
	char message[100] = "";

    // create the server socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // define the server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9002);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // bind the socket to our specified IP and port
    bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    listen(server_socket, 5);

	int conn;
	while ((conn = accept(server_socket, (struct sockaddr *)NULL, NULL)))
	{
		int pid;
		if((pid = fork()) == 0)
		{
			while (recv(conn, message, 100, 0)>0)
			{
				printf("%s\n", message);
				message[0] = 0;
			}
			exit(0);
    	}
	}
	return 0;
}