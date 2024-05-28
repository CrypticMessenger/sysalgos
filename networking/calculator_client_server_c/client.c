#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "constants.h"
#include "clientUtils.h"

int main(int argc, char *argv[])
{
    // initializations
    char buffer[MAXLINE];
    memset(buffer, 0, sizeof(buffer));
    char msg[420];
    int sockfd, read_val;
    struct sockaddr_in serverAddr;

    // creating the socket
    create_socket(&sockfd);

    // Set up the server address structure by resetting the memory to zero,
    // specifying the AF_INET family for the IPV4 protocol, setting the port number,
    // and specifying the IP address as 127.0.0.1 (localhost).
    memset(&serverAddr, 0, sizeof(serverAddr));          // reset the memory to 0
    serverAddr.sin_family = AF_INET;                     // set the address family to IPV4
    serverAddr.sin_port = htons(PORT);                   // set the port number
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // set the IP address

    // connect to the server
    connect_to_server(sockfd, serverAddr);

    while (1)
    {

        // take input from user
        char input[420];
        memset(msg, 0, 420);
        memset(buffer, 0, MAXLINE);
        buffer[0] = '0';
        buffer[1] = ' ';
        fflush(stdin);
        printf("'bye' to get logged out \n");
        printf("Enter a string: ");
        memset(input, 0, 420);
        fgets(input, 420, stdin);
        if (strcmp(input, "bye\n") == 0)
        {
            // login out
            memset(buffer, 0, MAXLINE);
            buffer[0] = '1';
            buffer[1] = ' ';
            send(sockfd, buffer, MAXLINE, 0);

            // Receive msg from server
            memset(buffer, 0, MAXLINE);
            read_val = recv(sockfd, buffer, MAXLINE, 0);
            close(sockfd);
            exit(0);
        }
        char *newline = strchr(input, '\n');
        if (newline != NULL)
        {
            *newline = '\0';
        }

        memcpy(&buffer[2], input, strlen(input));
        send(sockfd, buffer, MAXLINE, 0);

        memset(buffer, 0, MAXLINE);
        read_val = recv(sockfd, buffer, MAXLINE, 0);
        printf("%s\n", &buffer[2]);
    }
    return 0;
}