import socket
import os

# Set up the server
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind(("localhost", 12345))
s.listen(1)
print("Server is listening...")

# Accept a client connection
conn, addr = s.accept()
print("Connected by", addr)

# Receive the file name
filename = conn.recv(1024).decode()
if filename != "":
    # Get the modified file name
    base_name = os.path.splitext(filename)[0] + "_saved"
    extension = os.path.splitext(filename)[1]
    modified_filename = base_name + extension

    # Send acknowledgement to client
    conn.sendall(b"OK")

    # Open the file and write its contents to disk
    with open(modified_filename, "wb") as f:
        while True:
            file_contents = conn.recv(1024)
            if not file_contents:
                break
            f.write(file_contents)

    print(f"File '{filename}' received and saved as '{modified_filename}' to disk.")

# Close the connection
conn.close()
s.close()
