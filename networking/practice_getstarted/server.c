// Import required header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "constants.h"

// Main function
int main()
{
    // Declare variables
    int server_socket, client_socket, n, valread;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char buffer[MAXLINE] = {0};
    char reversedBuffer[MAXLINE] = {0};

    // Create a TCP socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Error opening socket");
        close(server_socket);
        exit(1);
    }
    printf("[+]TCP server socket created.\n");

    // Clear the server_addr struct
    memset(&server_addr, '\0', sizeof(server_addr));

    // Set the server address to IPv4
    server_addr.sin_family = AF_INET; // IPV4 Protocol

    // Set the server port number
    server_addr.sin_port = htons(PORT);

    // Allow connections to any interface
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the server address
    n = bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (n < 0)
    {
        perror("[-]Bind error");
        exit(1);
    }
    printf("[+]Binded to the port number: %d\n", PORT);

    // Set the server to listen for connections
    if (listen(server_socket, MAX_CLIENTS) < 0)
    {
        perror("listening failed");
        exit(1);
    }
    printf("LIstening...\n");

    // Accept connections from clients
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        addr_size = sizeof(client_addr);

        // Accept a connection
        if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size)) < 0)
        {
            perror("failed!");
            exit(1);
        }
        printf("A Client connected.\n");

        // Receive a message from the client
        valread = recv(client_socket, buffer, MAXLINE, 0);

        // Reverse the message
        int message_len = strlen(buffer);
        for (int j = 0; j < message_len; ++j)
        {
            reversedBuffer[j] = buffer[message_len - j - 1];
        }

        // Send the reversed message to the client
        send(client_socket, reversedBuffer, message_len, 0);
        printf("Sent reversed message to client %d: %s\n", i, reversedBuffer);

        // Reset the buffers
        memset(buffer, 0, MAXLINE);
        memset(reversedBuffer, 0, MAXLINE);

        // Close the connection
        printf("Closing the connection.\n");
        close(client_socket);
    }

    // Close the server socket
    close(server_socket);

    return 0;
}