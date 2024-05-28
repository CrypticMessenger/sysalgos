#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "constants.h"
void create_socket(int *sockfd)
{
    // creating the socket using socket(), with other parameters.
    // AF_INET refers to the address family.
    if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        //
        perror("error creating socket");
        exit(1);
    }
    //
    printf("socket created.\n");
}
void connect_to_server(int sockfd, struct sockaddr_in serverAddr)
{
    // Connect the socket to the server address specified in the "serverAddr" structure.
    if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        // If the connection fails, print an error message using perror() and exit the program.
        perror("Connection failed");
        exit(1);
    }
    // If the connection succeeds, print a message indicating that the connection was established.
    printf("connected to the server.\n");
}