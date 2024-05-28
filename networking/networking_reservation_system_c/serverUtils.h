#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>

int bind_server(int serverSocket, struct sockaddr_in serverAddress)
{
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        perror("[-] Error binding socket");
        close(serverSocket);
        exit(1);
    }
    printf("[+] Socket bound successfully\n");
}

void join_threads(pthread_t *threads, int num_threads)
{
    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }
}
void close_client_sockets(int *clientSockets, int num_clients)
{
    for (int i = 0; i < num_clients; i++)
    {

        printf("[!] Socket %d closed\n", clientSockets[i]);
        close(clientSockets[i]);
    }
}
int create_server_socket()
{
    int serverSocket;
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("[-] Error creating server socket....");
        exit(1);
    }
    return serverSocket;
}