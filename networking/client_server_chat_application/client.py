import os
import socket
import json
import threading
import time
keep_running = False
class Client:
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.username = None
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    def connect(self):
        self.socket.connect((self.host, self.port))
    
    def send(self, message):
        self.socket.sendall(message.encode())
        response = self.socket.recv(1024)
        return response.decode()

    def close(self):
        self.socket.close()
    def recieve(self):
        return self.socket.recv(1024).decode()
       
    def login(self, username, password):
        self.socket.sendall(f"/login |{username}|{password}".encode())
        json_str = self.socket.recv(1024).decode()
        response = json.loads(json_str)
        if(response == True):
            print("Logged in Successfully")
            self.username = username;
        elif(response == False):
            print("Incorrect username or password")
        else:
            print(response)
        return response
    def register(self, username, password):
        self.socket.sendall(f"/register |{username}|{password}".encode())
        json_str = self.socket.recv(1024).decode()
        response = json.loads(json_str)
        if(response == True):
            print("Registered Successfully")
        else:
            print(response)       
        return (response==True)
    def displayList(self,List):
        for i in List:
            print(i+"\n")

    def getActiveUsers(self):
        self.socket.sendall("/getActiveUsers".encode())
        json_str = self.socket.recv(1024).decode()
        response = json.loads(json_str)
        self.displayList(response)
        return response
    def fetchMessages(self,chatroom):
        self.socket.sendall(f"/fetchMessages |{chatroom}".encode())
        json_str = self.socket.recv(1024).decode()
        response = json.loads(json_str)
       
        return response
    
        
    def getAllChatrooms(self):
        self.socket.sendall("/getAllChatrooms".encode())
        json_str = self.socket.recv(1024).decode()
        response = json.loads(json_str)
        if(len(response) == 0):
            print("No Chatrooms present.")
        self.displayList(response)
        return response
    def getChatroomMembers(self, chatroom):
        self.socket.sendall(f"/getChatroomMembers |{chatroom}".encode())
        json_str = self.socket.recv(1024).decode()
        response = json.loads(json_str)
        if(response != "400"):
            print("Members of the chatroom are:\n")
            self.displayList(response)
        else:
            print("Invalid Chatroom")
        return response
    def sendMessage(self, chatroom, message):
        current_time = time.strftime("%Y-%m-%d|%H:%M:%S", time.localtime())
        self.socket.sendall(f"/sendMessage |{chatroom}|{message}|{self.username}|{current_time}".encode())
        json_str = self.socket.recv(1024).decode()
        response = json.loads(json_str)
        return response
    def exitChatroom(self, chatroom):
        self.socket.sendall(f"/exitChatroom |{chatroom}|{self.username}".encode())
        json_str = self.socket.recv(1024).decode()
        response = json.loads(json_str)
        return response
    def createChatroom(self, chatroom):
        self.socket.sendall(f"/createChatroom |{chatroom}".encode())
        json_str = self.socket.recv(1024).decode()
        response = json.loads(json_str)
        if(response == "400"):
            print("Chatroom already exists.")
        return response
    def joinChatroom(self, chatroom):
        self.socket.sendall(f"/joinChatroom |{chatroom}|{self.username}".encode())
        json_str = self.socket.recv(1024).decode()
        response = json.loads(json_str)
        if(response!="200"):
            print("Chatroom not present.")
        return (response=='200')
    def logout(self):
        self.socket.sendall(f"/logout |{self.username}".encode())
        json_str = self.socket.recv(1024).decode()
        response = json.loads(json_str)
        return response
    def client_menu(self):
        print("1. Login")
        print("2. Register")
        print("3. Exit")

def displayRealtimeMessages(client,chatroom):
     # Clear the console
    while keep_running:
        messages = client.fetchMessages(chatroom)
        for i in messages:
            msg = i[0];
            author = i[1];
            time1 = i[2];
            print(f"{author} : {msg} ({time1})")
        time.sleep(0.05)  # wa

if __name__ == "__main__":
    client = Client("localhost", 8080)
    client.connect()
    if(client.socket.recv(1024).decode() == "200"):
        print("Connected")
        while True:
            client.client_menu()
            choice = input("Enter your choice: ")
            if choice == "1":
                username = input("Enter username: ")
                password = input("Enter password: ")
                res = client.login(username, password)
                if(res == True):
                    print(f"Welcome {username}\n");
                    print("Active users:\n");        
                    activeUsers = client.getActiveUsers()#display all active users
                    while True:
                        print("1. Enter Chatroom");
                        print("2. Create Chatroom");
                        print("3. display all chatrooms");
                        print("4. get Active users")
                        print("5. Logout");
                        choice = input("Enter your choice: ")
                        if choice == "1":
                            client.getAllChatrooms();#display all chatrooms
                            chatroom =  input("Enter chatroom name: ")
                            
                            if(client.joinChatroom(chatroom)):#join chatroom
                                keep_running = True
                                client.getChatroomMembers(chatroom);#display members of chatroom
                                messageThread =threading.Thread(target=displayRealtimeMessages, args=(client, chatroom), daemon=True) # start message display thread
                                messageThread.start()
                                while True:
                                    message = input("Enter message:(type /exit to exit) ")
                                    if(message == "/exit"):
                                        client.sendMessage(chatroom, "**user exited**"); #send message
                                        client.exitChatroom(chatroom);#exit chatroom
                                        keep_running = False#join message display thread
                                        break
                                    else:
                                        client.sendMessage(chatroom, message); #send message
                                    
                        elif choice == "2":
                            chatroom = input("Enter chatroom name: ")
                            client.createChatroom(chatroom);
                            client.getAllChatrooms();
                        elif choice == "3":
                            client.getAllChatrooms();
                        elif choice == "4":
                            client.getActiveUsers();
                        elif choice == "5":
                            client.logout();

                            break;
                        else:
                            print("Invalid choice")
                            continue;
                        
                        
            elif choice == "2":
                username = input("Enter username: ")
                password = input("Enter password: ")
                res = client.register(username, password)
            elif choice == "3":
                break
        

    client.close()