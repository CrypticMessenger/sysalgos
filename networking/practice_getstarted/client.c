#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "constants.h"
// create socket
int createSocket()
{
    int socket_FD;
    // create socket
    // AF_INET: IPv4 Internet protocols
    // SOCK_STREAM: TCP
    // 0: protocol (IP)
    if ((socket_FD = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket failed");
        exit(1);
    }
    return socket_FD;
}

// connects to server given addr and socket filedescriptor
void connectToServer(int socket_FD, struct sockaddr_in server_addr)
{
    if (connect(socket_FD, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection failed");
        exit(1);
    }
}

// sends message to server given socket filedescriptor and message
void sendMessage(int socket_FD, char *message)
{
    send(socket_FD, message, strlen(message), 0);
    printf("Sent message to server: %s\n", message);
}

// receives message from server given socket filedescriptor and reversedBuffer
void receiveMessage(int socket_FD, char *reversedBuffer)
{
    recv(socket_FD, reversedBuffer, MAXLINE, 0);
    printf("Received reversed message from server: %s\n", reversedBuffer);
}

// closes socket given socket filedescriptor
void closeSocket(int socket_FD)
{
    close(socket_FD);
}

int main()
{
    // socket filedescriptor
    int socket_FD;
    // server addres
    struct sockaddr_in server_addr;
    // buffer to store message
    char buffer[MAXLINE] = {0};
    // buffer to store reversed message
    char reversedBuffer[MAXLINE] = {0};
    // message to send
    char *message = MESSAGE_FROM_CLIENT;

    // create socket
    socket_FD = createSocket();

    memset(&server_addr, '\0', sizeof(server_addr));

    // set the address family of the socket to IPv4
    server_addr.sin_family = AF_INET;

    // set the port number of the socket to PORT, using network byte order
    server_addr.sin_port = htons(PORT);

    // set the IP address of the socket to INADDR_ANY,
    // which means the socket will listen on all available network interfaces
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // connect to server
    connectToServer(socket_FD, server_addr);
    // send message
    sendMessage(socket_FD, message);
    // receive message
    receiveMessage(socket_FD, reversedBuffer);
    // close socket
    closeSocket(socket_FD);

    return 0;
}