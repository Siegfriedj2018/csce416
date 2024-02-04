# These instructions are for running TwoWayAsyncMesg application written in java

# Compile the client and server programs
% javac TwoWayAsyncMesgServer.java
% javac TwoWayAsyncMesgClient.java

# First, run the server program with a port number (say 50000)
% java TwoWayAsyncMesgServer 50000

# Then, run the client program with the server's whereabouts
% java TwoWayAsyncMesgClient localhost 50000

# Type messages at the client and see them displayed at the server

# To end the client or server, type Ctrl-D (on Windows Ctrl-Z followed by # return)
# This is referred to as EOF (end of file), meaning end of input
# Then, the other will also quit saying server/client closed the connection
