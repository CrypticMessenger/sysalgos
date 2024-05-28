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
#include "constants.h"
#include "serverUtils.h"
#include <fcntl.h>
#include <sys/file.h>

int AC = AC_NUM;
int SL = SL_NUM;
int serverSocket;

// Function to handle client connections
void *handle_client(void *arg)
{
    int socket_fd = *(int *)arg;
    char buffer[WINDOW_SIZE];
    char clientName[NAME_LEN];
    int num_bytes_recieved;
    int num_bytes_sent;

    // Persistent connection
    while (true)
    {
        // set buffer to 0
        memset(buffer, 0, WINDOW_SIZE);
        num_bytes_recieved = read(socket_fd, buffer, WINDOW_SIZE);
        // printf("num_bytes_recieved: %d\n", num_bytes_recieved);
        if (num_bytes_recieved < 0)
        {
            perror("Error reading from socket\n");
            close(serverSocket);
            exit(1);
        }
        else if (num_bytes_recieved == 0)
        {
            // Client disconnected
            printf("Client disconnected on socket %d\n", socket_fd);
            break;
        }

        if (strncmp(buffer, "HI  ", 4) == 0)
        {
            // Hello from the client
            printf("Hello from the client\n");

            memcpy(clientName, &buffer[4], NAME_LEN);
            // Trim newline characters from name
            for (int i = 0; i < NAME_LEN; i++)
            {
                if (clientName[i] == '\n')
                {
                    clientName[i] = '\0';
                    break;
                }
            }

            printf("Connected connected on scoket %d.\n", socket_fd);
            memset(buffer, 0, WINDOW_SIZE); // Clear the buffer

            // // Send a hello message to the client
            char hello_msg[WINDOW_SIZE];
            memset(hello_msg, 0, WINDOW_SIZE);
            memcpy(hello_msg, "Hello ", 6);
            memcpy(&hello_msg[6], clientName, NAME_LEN);
            int num_bytes_sent = send(socket_fd, hello_msg, WINDOW_SIZE, 0);
            if (num_bytes_sent < 0)
            {
                perror("[-] Error sending hello message to socket\n");
                close(serverSocket);
                exit(1);
            }
        }

        else if (strncmp(buffer, "REQ ", 4) == 0)
        {
            // Ticket Request from the client

            int i = 4;
            int num_ac = 0, num_slr = 0;
            // Extract the number of AC and SL tickets from the buffer
            for (; i < WINDOW_SIZE && buffer[i] != ' '; i++)
                ;
            char AC_str[20];

            memcpy(AC_str, &buffer[4], i - 4);
            AC_str[i - 4] = '\0';
            num_ac = atoi(AC_str);
            int j = i + 1;
            for (; j < WINDOW_SIZE && buffer[j] != ' '; j++)
                ;
            char SLR_str[20];
            memcpy(SLR_str, &buffer[i + 1], j - i - 1);
            SLR_str[j - i - 1] = '\0';

            num_slr = atoi(SLR_str);

            // Check if the tickets are available
            if (num_ac <= AC && num_slr <= SL)
            {
                AC -= num_ac;
                SL -= num_slr;
                memset(buffer, 0, WINDOW_SIZE);

                buffer[0] = '2';
                buffer[1] = '0';
                buffer[2] = '0';
                buffer[3] = ' ';
                // Send a success message to the client
                memcpy(&buffer[4], "success", strlen("success"));
                num_bytes_recieved = write(socket_fd, buffer, WINDOW_SIZE);
                if (num_bytes_recieved < 0)
                {
                    perror("[-] Error writing to socket\n");
                    close(serverSocket);
                    exit(1);
                }

                time_t t = time(NULL);
                struct tm *tm_info = localtime(&t);

                // Format the time string
                char timestamp[20];
                strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

                // Write the record to the file with the timestamp
                // FILE *file = fopen(LOG_FILE, "a");
                // fprintf(file, "REQ_BOOK at %s -->  %s requests AC: %d, SL: %d\n", timestamp, clientName, num_ac, num_slr);
                // fclose(file);
                FILE *file = fopen(LOG_FILE, "a");
                if (file == NULL)
                {
                    perror("Error opening file");
                    exit(1);
                }

                // Acquire an exclusive lock on the file
                int fd = fileno(file);
                if (flock(fd, LOCK_EX) == -1)
                {
                    perror("Error acquiring lock on file");
                    fclose(file);
                    exit(1);
                }

                if (fprintf(file, "REQ_BOOK at %s -->  %s requests AC: %d, SL: %d\n", timestamp, clientName, num_ac, num_slr) < 0)
                {
                    perror("Error writing to file");
                    flock(fd, LOCK_UN);
                    fclose(file);
                    exit(1);
                }

                // Release the lock
                if (flock(fd, LOCK_UN) == -1)
                {
                    perror("Error releasing lock on file");
                    fclose(file);
                    exit(1);
                }

                if (fclose(file) == EOF)
                {
                    perror("Error closing file");
                    exit(1);
                }
            }
            else
            {
                bzero(buffer, WINDOW_SIZE);
                buffer[0] = '2';
                buffer[1] = '0';
                buffer[2] = '1';
                buffer[3] = ' ';
                // Send a failure message to the client
                memcpy(&buffer[4], "failure booking tickets.", strlen("failure booking tickets."));
                num_bytes_sent = write(socket_fd, buffer, WINDOW_SIZE);
                if (num_bytes_sent < 0)
                {
                    perror("Error writing to socket");
                    close(serverSocket);
                    exit(1);
                }
            }
        }
        else if (strncmp(buffer, "BYE ", 4) == 0)
        {
            memset(buffer, 0, WINDOW_SIZE); // Clear the buffer

            // Send a goodbye message to the client
            memcpy(buffer, "Good Bye", strlen("Good Bye"));
            num_bytes_sent = write(socket_fd, buffer, WINDOW_SIZE);
            if (num_bytes_sent < 0)
            {
                perror("Error writing to socket");
                close(serverSocket);
                exit(1);
            }
            printf("Client disconnected on socket%d\n", socket_fd);
            break;
        }

        // Write to the client
    }
    // Close the socket
    close(socket_fd);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    char username[NAME_LEN];
    char password[NAME_LEN];
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);
    if (strcmp(username, "admin") == 1 || strcmp(password, "admin") == 1)
    {
        exit(1);
    }
    int serverPort = PORT;
    int clientSockets[MAX_CLIENTS];
    socklen_t clientLen;
    struct sockaddr_in serverAddr, clientAddress[MAX_CLIENTS];
    pthread_t clientThreads[MAX_CLIENTS];

    // Create a new server socket
    int serverSocket = create_server_socket();

    // Set the server address structure
    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(serverPort);

    // Bind the socket to the server address
    bind_server(serverSocket, serverAddr);

    // Listen for incoming connections
    listen(serverSocket, MAX_CLIENTS);
    printf("[!] Server Listening on port %d\n", serverPort);

    clientLen = sizeof(clientAddress[0]);

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        // Accept a new client
        clientSockets[i] = accept(serverSocket, (struct sockaddr *)&clientAddress[i], &clientLen);
        if (clientSockets[i] < 0)
        {
            perror("[-] Error accepting client\n");
            close(serverSocket);
            exit(1);
        }
        // Create a new thread for the client
        if (pthread_create(&clientThreads[i], NULL, handle_client, &clientSockets[i]) != 0)
        {
            perror("[-] Error creating thread for client 1\n");
            close(serverSocket);
            exit(1);
        }
    }

    // Join all the threads , wait till all the threads end and then exit
    join_threads(clientThreads, MAX_CLIENTS);

    // Close the client sockets
    close_client_sockets(clientSockets, MAX_CLIENTS);

    printf("[!] Exiting...\n");
    // close server socket
    close(serverSocket);

    return 0;
}
