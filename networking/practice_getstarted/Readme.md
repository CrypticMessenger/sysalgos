- to run the program follow the following steps in order:
- 1. open the terminal
- 2. navigate to the directory where the program is located
- 3. change the constants as you like in constants.h file
- 4. run the following command to start the server:
   ```bash
   gcc server.c -o server
   ./server
   ```
- 5. change the MESSAGE_FROM_CLIENT constant in constants.h file to the message you want to send to the server
- 6. run the following command to start the client:
   ```bash
   gcc client.c -o client
   ./client
   ```
- since MAX_CLIENTS is set to 5, you can run the client program 5 times to see the server's response to each client.
- to stop the server, press ctrl+c in the terminal where the server is running or give client request MAX_CLIENT number of times.
- comments have been added to the code to make it easier to understand.