#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdbool.h>
#include <signal.h>
#include "constants.h"

void sendMessage(int socket, const char *message)
{
    int n = send(socket, message, strlen(message), 0);
    if (n < 0)
    {
        perror("ERROR sending message to server");
        close(socket);
        exit(1);
    }
}
void receiveMessage(int socket, char *buffer)
{
    int n = recv(socket, buffer, WINDOW_SIZE, 0);
    if (n < 0)
    {
        perror("Connection establishment failed...");
        close(socket);
        exit(1);
    }
}
