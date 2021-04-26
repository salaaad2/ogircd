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

	strcpy(pseudo, av[1]);

    // specify an address for the socket
    struct sockaddr_in server_address;

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9002);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int connection_status = connect(net_socket, (struct sockaddr *)&server_address, sizeof(server_address));

    if (connection_status == -1)
        printf("Error making connection to the remote socket\n\n");

	char message[100] = "";
	inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);
	char ex[100];
	strcpy(ex, pseudo);
    while(1)
	{
		strcpy(pseudo, ex);
		printf("%s: ", pseudo);
		fgets(message, 100, stdin);
		strcat(pseudo, ": ");
		strcat(pseudo, message);
		send(net_socket, pseudo, strlen(pseudo), 0);
		//An extra breaking condition can be added here (to terminate the while loop)
	}

    // close socket
    close(net_socket);

    return 0;
}