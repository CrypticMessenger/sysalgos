#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "constants.h"
#include "serverUtils.h"
// mutex locking to handle race condition
pthread_mutex_t mutex;

void *handle_client(void *arg)
{
    int connectionFd = *(int *)arg; // file descriptor for connection between client and server
    int read_val;
    char buffer[MAXLINE];
    memset(buffer, 0, MAXLINE);

    while (1)
    {
        // receiving the client request using recv() system call
        read_val = recv(connectionFd, buffer, MAXLINE, 0);

        if (buffer[0] == '0')
        {
            // aquiring the mutex locks
            pthread_mutex_lock(&mutex);
            printf("buffer:%s\n", buffer);
            char input[50];
            char query[50];
            int i, top = -1;
            double stack[50];

            // parsing the buffer array to fetch the components of query
            for (i = 2; i < MAXLINE; ++i)
            {
                if (buffer[i] == '\0')
                {
                    break;
                }
            }
            memcpy(input, &buffer[2], i - 2);
            input[i - 2] = '\0';
            printf("query: %s\n", input);
            memcpy(query, &buffer[2], i - 2);
            query[i - 2] = '\0';

            char ch;
            double op1 = 0.0, op2 = 0.0, div = 10.0, num = 0.0, dec = 0.0, res = 0.0;

            for (i = 0; input[i] != '\0'; i++)
            {
                num = 0.0;
                dec = 0.0;
                div = 10.0;
                ch = input[i];
                if (isdigit(ch))
                {
                    while (isdigit(input[i]))
                    {
                        num = num * 10 + (double)(input[i] - '0');
                        i++;
                    }
                    if (input[i] == '.')
                    {
                        i++;
                        while (isdigit(input[i]))
                        {
                            dec = dec + ((double)(input[i] - '0') / div);
                            div = div * 10.0;
                            i++;
                        }
                        i--;
                    }
                    num = (num + dec);
                    stack[++top] = (num);
                }
                else if (ch == '+' || ch == '-' || ch == '*' || ch == '/')
                {
                    op2 = stack[top--];
                    op1 = stack[top--];
                    if (ch == '+')
                    {
                        stack[++top] = op1 + op2;
                    }
                    else if (ch == '-')
                    {
                        stack[++top] = op1 - op2;
                    }
                    else if (ch == '*')
                    {
                        stack[++top] = op1 * op2;
                    }
                    else if (ch == '/')
                    {
                        stack[++top] = op1 / op2;
                    }
                }
            }
            res = stack[top--];
            memset(input, 0, 50);
            sprintf(input, "%f", res);
            if (top >= 0)
            {
                memset(buffer, 0, MAXLINE);
                buffer[0] = '1';
                buffer[1] = ' ';
                memcpy(&buffer[2], "Invalid postfix expression!!", 28);
                send(connectionFd, buffer, MAXLINE, 0);
            }
            else
            {
                memset(buffer, 0, MAXLINE);
                buffer[0] = '1';
                buffer[1] = ' ';
                memcpy(&buffer[2], input, strlen(input));
                send(connectionFd, buffer, MAXLINE, 0);
                // writing to the file
                FILE *file = fopen("server_records.txt", "a");
                fprintf(file, "client_id- %d, query:- %s, answer:-%f\n", connectionFd, query, res);
                fclose(file);
            }

            // releasing the mutex locks
            pthread_mutex_unlock(&mutex);
        }
        else if (buffer[0] == '1')
        {
            printf("disconnected.\n");
            memset(buffer, 0, MAXLINE);
            memcpy(buffer, "Bye!", 4);
            send(connectionFd, buffer, MAXLINE, 0);
            break;
        }
        else
        {
            printf("Something went wrong.\n");
            break;
        }
    }
    // Close the socket
    close(connectionFd);
    // deleating the threads
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    // initialising variables
    int clientSocket[MAXCLIENT];
    pthread_mutex_init(&mutex, NULL);
    pthread_t clientThreads[MAXCLIENT];
    int serverSocket;
    socklen_t addrSize;
    struct sockaddr_in serverAddr, clientAddr[MAXCLIENT];

    serverSocket = create_server_socket();

    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    bind_server_socket(serverSocket, serverAddr, PORT);

    listen_for(serverSocket);

    for (int i = 0; i < MAXCLIENT; ++i)
    {
        addrSize = sizeof(clientAddr[i]);
        // server is waiting for client connection to get completed using accept() system call
        if ((clientSocket[i] = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrSize)) < 0)
        {
            perror("Accept failed.\n");
            exit(1);
        }
        printf("Client connected.\n");

        if (pthread_create(&clientThreads[i], NULL, handle_client, &clientSocket[i]) != 0)
        {
            perror("error in creating thread...\n");
            exit(1);
        }
    }

    // join threads
    for (int i = 0; i < MAXCLIENT; ++i)
    {
        pthread_join(clientThreads[i], NULL);
    }

    // close sockets
    for (int i = 0; i < MAXCLIENT; ++i)
    {
        close(clientSocket[i]);
    }

    // destroy the mutex
    pthread_mutex_destroy(&mutex);
    // closing the server
    close(serverSocket);

    return 0;
}