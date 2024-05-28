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
#include "clientUtils.h"
int clientSocket;

int main(int argc, char *argv[])
{

    int portNo = PORT, n;
    struct sockaddr_in serverAddress;
    struct hostent *server = gethostbyname("localhost");

    char buffer[WINDOW_SIZE];

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0)
    {
        perror("[-] ERROR creating socket");
        close(clientSocket);
        exit(1);
    }

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serverAddress.sin_addr.s_addr, server->h_length);
    serverAddress.sin_port = htons(portNo);

    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        perror("[-] ERROR connecting");
        close(clientSocket);
        exit(0);
    }
    printf("[+] Connection established...\n");
    // Prompt the user for their name
    printf("What is your name?: ");

    // Clear the buffer
    memset(buffer, 0, WINDOW_SIZE);

    // Build the message to be sent to the server
    buffer[0] = 'H';
    buffer[1] = 'I';
    buffer[2] = ' ';
    buffer[3] = ' ';
    fgets(&buffer[4], NAME_LEN, stdin); // read user's name from stdin

    sendMessage(clientSocket, buffer);

    // Clear the buffer
    memset(buffer, 0, WINDOW_SIZE);

    // Receive the server's response
    receiveMessage(clientSocket, buffer);

    // Print the server's response
    printf("%s\n", buffer);

    int numAC = 0, numSL = 0;
    while (true)
    {
        printf("1: to book tickets\n");
        printf("2: to check tickets\n");
        printf("3: to exit\n");
        printf("4: to get total tickets type wise\n");
        int choice;
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            memset(buffer, 0, WINDOW_SIZE);
            buffer[0] = 'R';
            buffer[1] = 'E';
            buffer[2] = 'Q';
            buffer[3] = ' ';
            int AC, SLR;
            printf("Enter the number of AC tickets you want to buy: ");
            if (scanf("%d", &AC) != 1)
            {
                perror("[-] Invalid input\n");
                close(clientSocket);
                exit(1);
            }
            printf("Enter the number of SL tickets you want to buy: ");
            if (scanf("%d", &SLR) != 1)
            {
                perror("[-] Invalid input\n");
                close(clientSocket);
                exit(1);
            }
            char ac[20];
            char sl[20];
            sprintf(ac, "%d", AC);
            sprintf(sl, "%d", SLR);
            memcpy(&buffer[4], ac, strlen(ac));
            memcpy(&buffer[4 + strlen(ac)], " ", 1);
            memcpy(&buffer[4 + strlen(ac) + 1], sl, strlen(sl));
            memcpy(&buffer[4 + strlen(ac) + 1 + strlen(sl)], " ", 1);

            sendMessage(clientSocket, buffer);

            memset(buffer, 0, WINDOW_SIZE);

            receiveMessage(clientSocket, buffer);
            printf("%s\n", &buffer[4]);
            // response code 200 means success
            if (strncmp(buffer, "200 ", 4) == 0)
            {
                numAC += AC;
                numSL += SLR;
            }
            break;
        case 2:
            printf("Total tickets bought: ");
            printf("AC: %d, SL: %d\n", numAC, numSL);
            break;
        case 3:
            memset(buffer, 0, WINDOW_SIZE);
            buffer[0] = 'B';
            buffer[1] = 'Y';
            buffer[2] = 'E';
            buffer[3] = ' ';
            sendMessage(clientSocket, buffer);
            memset(buffer, 0, WINDOW_SIZE);
            receiveMessage(clientSocket, buffer);
            printf("%s\n", buffer);
            close(clientSocket);
            exit(0);
            break;
        case 4:
            printf("total AC: %d, total SLR: %d\n", AC_NUM, SL_NUM);
            break;
        default:
            printf("Invalid choice\n");
            break;
        }
    }
    return 0;
}
