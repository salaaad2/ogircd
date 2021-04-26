#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>

int main()
{

    // create socket

    int net_socket = socket(AF_INET, SOCK_STREAM, 0);

    // specify an address for the socket
    struct sockaddr_in server_address;

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9002);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int connection_status = connect(net_socket, (struct sockaddr *)&server_address, sizeof(server_address));

    if (connection_status == -1)
        printf("Error making connection to the remote socket\n\n");

    // receive data from server
    char server_response[256];
    recv(net_socket, &server_response, sizeof(server_response), 0);

    // print out server's response
    printf("The server sent the data: %s\n", server_response);

    // close socket
    close(net_socket);

    return 0;
}