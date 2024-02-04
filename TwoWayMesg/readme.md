# These are instructions for running my TwoWayMesgServer and TwoWayMesgClient written in c++

# if the executables do not work, run the following commands
  
  # if you have make install:
  make clean && make TwoWayMesgServer && make TwoWayMesgClient

  # if you don't have make:
  g++/gcc TwoWayMesgServer.cc -o TwoWayMesgServer
  g++/gcc TwoWayMesgClient.cc -o TwoWayMesgClient

  # Then run server by:
  ./TwoWayMesgServer 50000 server_name
  # And then the client:
  ./TwoWayMesgClient localhost 50000 client_name

# To exit:
  type 'exit' or 'quit' in either program
