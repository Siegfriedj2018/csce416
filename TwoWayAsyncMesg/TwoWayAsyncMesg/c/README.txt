# These instructions are for running TwoWayAsyncMesg application written in c

# Compile the server and client programs
% gcc TwoWayAsyncMesgServer.c -o TwoWayAsyncMesgServer
% gcc TwoWayAsyncMesgClient.c -o TwoWayAsyncMesgClient

# First, run the server with a port number (say 50000)
% ./TwoWayAsyncMesgServer 50000

# Then, run the client with the server's whereabouts
% ./TwoWayAsyncMesgClient localhost 50000

# Type messages at the client and see them displayed at the server

# To end the client or server, type Ctrl-D (on Windows Ctrl-Z followed by # return)
# This is referred to as EOF (end of file), meaning end of input
# Then, the other will also quit saying server/client closed the connection
