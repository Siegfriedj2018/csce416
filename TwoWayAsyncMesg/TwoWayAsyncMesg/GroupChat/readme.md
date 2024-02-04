# These are instructions for running my GroupChatServer and GroupChatClient written in c++

# if the executables do not work, run the following commands
  
  # if you have make install:
  make clean && make GroupChatClient && make GroupChatClient

  # if you don't have make:
  g++/gcc GroupChatServer.cc -o GroupChatServer
  g++/gcc GroupChatClient.cc -o GroupChatClient

  # Then run server by:
  ./GroupChatServer 50000
  # And then the client:
  ./GroupChatClient localhost 50000 client_name

# To exit:
  type 'exit' or 'quit' in Client and control+C/control+D for server
