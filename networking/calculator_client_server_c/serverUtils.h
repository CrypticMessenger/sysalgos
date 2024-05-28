#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "constants.h"

// Function to create a TCP server socket.
// Returns the socket file descriptor on success, or exits the program on failure.
int create_server_socket()
{
    int serverSocket;

    // Create a new socket with the AF_INET domain, SOCK_STREAM type, and protocol 0 (default).
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // Check if the socket creation was successful or not.
    if (serverSocket < 0)
    {
        // If the socket creation fails, print an error message using perror() and exit the program.
        perror("error!");
        exit(1);
    }

    // If the socket creation succeeds, print a message indicating that the socket was created.
    printf("Socket created.\n");

    // Return the socket file descriptor, which can be used to accept incoming connections from clients.
    return serverSocket;
}

// This function listens for incoming connections on a socket and limits the queue of incoming connections.
void listen_for(int serverSocket)
{
    if (listen(serverSocket, MAXCLIENT) < 0) // listen() system call
    {
        perror("[-]Listened failed.\n"); // error message if listen() fails
        exit(1);                         // exit the program
    }
    printf("Listening...\n"); // print message to indicate that the server is listening
}

// Function to bind the server address and port to a socket.
// Exits the program on failure, or prints a message indicating success.
void bind_server_socket(int serverSocket, struct sockaddr_in serverAddr, int port)
{
    // Set the server address and port number in the serverAddr struct.
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the server address and port using the bind() system call.
    if ((bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr))) < 0)
    {
        // If the bind fails, print an error message using perror() and exit the program.
        perror("error");
        exit(1);
    }

    // If the bind succeeds, print a message indicating the port number that the server is bound to.
    printf("Bound to the port number: %d\n", port);
}