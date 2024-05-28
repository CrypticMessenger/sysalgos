import socket

# Set up the client
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(("localhost", 12345))

# Get the file path from the user
file_path = input("Enter the file path: ")

# Get the file name from the file path
file_name = file_path.split("/")[-1]

# Open the file and read its contents
try:
    with open(file_path, "rb") as f:
        file_contents = f.read()

except FileNotFoundError:
    print("The file path you entered is invalid.")
    s.close()
    exit()

# Send the file name to the server
s.sendall(file_name.encode())

# Wait for the server to acknowledge the file name
flag_response = s.recv(1024)
if flag_response.decode() == "OK":
    print("File name received by the server.")
    # Send the file contents to the server
    s.sendall(file_contents)

else:
    print("File name not received by the server.")
    s.close()
    exit()


# Close the connection
s.close()
