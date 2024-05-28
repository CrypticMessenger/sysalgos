import json
import socket
import threading
import time


class Server:
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.bind((self.host, self.port))
        self.users = {}
        self.user_active = []
        self.chat_rooms = {} 
        # chat_room["chatroom_name"] = {"users": ["user1", "user2", "user3"], "messages": [["message1","sender1","timestamp1"], ["message2","sender2","timestamp2"], ["message3","sender3","timestamp3"]]}
    
    def listen(self):
        self.socket.listen(5)
        print(f"Server listening on {self.host}:{self.port}")
        while True:
            client_socket, address = self.socket.accept()
            print(f"New connection from {address[0]}:{address[1]}")
            client_handler = threading.Thread(target=self.handle_client, args=(client_socket,))
            client_handler.start()
            
    def login(self, username, password):
        if(username not in self.users):
            return "Kindly register first"
        else:
            if(self.users[username] == password):
                self.user_active.append(username)
           
            return self.users[username] == password
    def logout(self, username):
        self.user_active.remove(username)
        #remvoe user from all the chatrooms
        for chat_room in self.chat_rooms:
            if(username in self.chat_rooms[chat_room]["users"]):
                self.chat_rooms[chat_room]["users"].remove(username)
    def register(self, username, password):
        if(username in self.users):
            return "username already exists!"
        else:
            self.users[username] = password
            return True
    
    def handle_client(self, client_socket):
        client_socket.send("200".encode())
        while(True):
            request = client_socket.recv(1024).decode()
            if not request:
                break
            print(request)
            if(request.startswith("/login")):
                _, username, password = request.split('|')
                response = self.login(username, password)
                json_str = json.dumps(response)
                client_socket.send(json_str.encode())
            elif(request.startswith("/register")):
                _, username, password = request.split('|')
                response = self.register(username, password)
                json_str = json.dumps(response)
                client_socket.send(json_str.encode())
            elif(request.startswith("/createChatroom")):
                _, chat_room_name = request.split('|')
                if(chat_room_name in self.chat_rooms):
                    json_str = json.dumps("400")
                    client_socket.send(json_str.encode())
                else:
                    self.chat_rooms[chat_room_name] = {"users": [], "messages": []}
                    json_str = json.dumps("200")
                    client_socket.send(json_str.encode())
            elif(request.startswith("/joinChatroom")):
                _, chat_room_name,username = request.split('|')
                if(chat_room_name not in self.chat_rooms):
                    json_str = json.dumps("400")
                    client_socket.send(json_str.encode())
                else:
                    self.chat_rooms[chat_room_name]["users"].append(username)
                    json_str = json.dumps("200")
                    client_socket.send(json_str.encode())
            elif(request.startswith("/getAllChatrooms")):
                chatrooms = list(self.chat_rooms.keys())
                json_str = json.dumps(chatrooms)
                client_socket.send(json_str.encode())
            elif(request.startswith("/getActiveUsers")):
                json_str = json.dumps(self.user_active)
                client_socket.send(json_str.encode())
            elif(request.startswith("/getChatroomMembers")):
                _, chat_room_name = request.split('|')
                if(chat_room_name not in self.chat_rooms):
                    json_str = json.dumps("400")
                    client_socket.send(json_str.encode())
                else:
                    json_str = json.dumps(self.chat_rooms[chat_room_name]["users"])
                    client_socket.send(json_str.encode())
            # featchMessages
            elif(request.startswith("/fetchMessages")):
                _, chat_room_name = request.split('|')
                if(chat_room_name not in self.chat_rooms):
                    json_str = json.dumps("400")
                    client_socket.send(json_str.encode())
                else:
                    json_str = json.dumps(self.chat_rooms[chat_room_name]["messages"])
                    client_socket.send(json_str.encode())
            #exit chatroom
            elif(request.startswith("/exitChatroom")):

                _, chat_room_name,username = request.split('|')
                if(chat_room_name not in self.chat_rooms):
                    json_str = json.dumps("400")
                    client_socket.send(json_str.encode())
                else:
                    self.chat_rooms[chat_room_name]["users"].remove(username)
                    json_str = json.dumps("200")
                    client_socket.send(json_str.encode())
            #sendMessage
            elif(request.startswith("/sendMessage")):
                _, chat_room_name,message,username,current_date,current_time = request.split('|')
                if(chat_room_name not in self.chat_rooms):
                    json_str = json.dumps("400")
                    client_socket.send(json_str.encode())
                else:
                    self.chat_rooms[chat_room_name]["messages"].append([message,username,str(current_date)+" "+str(current_time)])
                    json_str = json.dumps("200")
                    client_socket.send(json_str.encode())
            #logout
            elif(request.startswith("/logout")):
                _, username = request.split('|')
                self.logout(username)
                json_str = json.dumps("200")
                client_socket.send(json_str.encode())

            

if __name__ == "__main__":
    server = Server("localhost", 8080)
    server_thread = threading.Thread(target=server.listen)
    server_thread.start()
    server_thread.join()
