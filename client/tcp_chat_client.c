#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

void
p_send(char pseudo[], char password[], int net_socket)
{
    char psend[110];

    /* strcat(psend, "PASS :"); */
    /* strcat(psend, password); */
    /* strcat(psend, "\r\n"); */
    sprintf(psend, "NICK :%s\r\nPASS :%s\r\n", pseudo, password);
    send(net_socket, psend, strlen(psend), 0);
}


int main(int ac, char **av)
{
    // create socket
    int net_socket = socket(AF_INET, SOCK_STREAM, 0);
    char pseudo[100] = "salad";
    char passwd[100] = "PASS ";

    if (ac >= 4) {
        strcpy(pseudo, av[2]);
        strcpy(passwd, av[3]);
    }
    else
    {
        printf("insufficient parameters.\nUsage : ./ctest [PORT] [NICK] [PASS]");
        return (1);
    }
    // specify an address for the socket
    struct sockaddr_in server_address;

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(atoi(av[1]));
    server_address.sin_addr.s_addr = INADDR_ANY;
    /* server_address.sin_addr.s_addr = inet_addr("10.4.2.1"); */


    struct ifaddrs *ifaddr, *ifa;
    const char* ifname = "en0";

    getifaddrs(&ifaddr);
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if((ifa->ifa_addr != NULL) &&
           (strcmp(ifa->ifa_name, ifname) == 0) &&  
           (ifa->ifa_addr->sa_family == AF_INET)) {
            printf("\tAddress: <%s>\n", inet_ntoa(((struct sockaddr_in *)ifa->ifa_addr)->sin_addr));
            char *hostname = inet_ntoa(((struct sockaddr_in *)ifa->ifa_addr)->sin_addr);
            break;
        }
    }

    int connection_status = connect(net_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    if (connection_status == -1) {
        printf("Error making connection to the remote socket\n\n");
    }

    char message[512];
    bzero(message, 512);
    p_send(pseudo, passwd, net_socket);
    char response[256];
    bzero(response, 256);
    while(1)
    {
        char all[512];
        bzero(all, 512);
        bzero(response, 256);
        recv(net_socket, &response, sizeof(response), 0);
        printf("%s\n", response);
        printf("{%s}", all);
        fgets(message, 512, stdin);
        message[strlen(message) - 1] = 0;
        sprintf(all, "%s\r\n", message);
        send(net_socket, all, strlen(all), 0);
    }
    close(net_socket);

    return 0;
}

/* strcpy(all, ":"); */
/* strcat(all, pseudo); */
/* strcat(all, " "); */
/* all[strlen(all) - 1] = '\r'; */
/* all[strlen(all)] = '\n'; */
