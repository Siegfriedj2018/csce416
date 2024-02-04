# Implementation of the two-way async message client in python

# Import socket related methods
from socket import *

# Import argv related methods
from sys import *

# Import select method
from select import *


# Client needs server's contact information
if len(argv) != 3:
    print("usage:", argv[0], "<server name> <server port>")
    exit()

# Get server's whereabouts
serverName = argv[1]
serverPort = int(argv[2])

# Create a socket
sock = socket(AF_INET, SOCK_STREAM)

# Connect to the server
sock.connect((serverName, serverPort))
print(f"Connected to server at ('{serverName}', '{serverPort}')");

# Make a file stream out of socket
sockFile = sock.makefile(mode='r')

# Make a list of inputs to watch for
inputSet = [stdin, sockFile]

# Keep sending and receiving messages from the server
while True:

    # Wait for a message from keyboard or socket
    readableSet, x, x = select(inputSet,[],[])

    # Check if there is a message from the keyboard
    if stdin in readableSet:
        # Read a line form the keyboard
        line = stdin.readline()

        # If EOF ==> client wants to close connection
        if not line:
            print('*** Client closing connection')
            break

        # Send the line to client
        sock.send(line.encode())

    # Check if there is a message from the socket
    if sockFile in readableSet:
        # Read a message from the client
        line = sockFile.readline()

        # If EOF ==> client closed the connection
        if not line:
            print('*** Server closed connection')
            break
            
        # Display the line
        print('Server:', line, end='')

# Close the connection
sockFile.close()
sock.close()
