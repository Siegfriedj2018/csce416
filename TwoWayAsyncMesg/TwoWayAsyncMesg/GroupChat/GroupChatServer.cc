/* Copyright 2023 Justin Siegfried */

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <unordered_map>


int main(int argc, char *argv[]) {
  const size_t kMAXMESGLEN = 1024;
  int listener_fd, server_port, ser_cli_sock_fd, max_fd;
  int bytes_read, success;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_addr_len;
  char buffer[kMAXMESGLEN];
  std::unordered_map<int, std::string> client_name;
  int i, j, opt_val = 1;

  /* check the commandline for the correct number of arguements */
  if (argc < 2) {
    std::cerr << "Usage: ./GroupChatServer <port>" << std::endl;
    std::cout << "\tEx: ./GroupChatServer 5000" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  /* Gets the port number to listen one from user entry */
  server_port = std::stoi(*(argv+1));
  // server_name = *(argv+2);
  // server_port = 50000;
  // server_name = "Server";

  /* Create the server socket */
  listener_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (listener_fd == -1) {
    std::cerr << "Failed to create socket: " << strerror(errno) << std::endl;
    exit(EXIT_FAILURE);
  }

  // lose the pesky "address already in use" error message
  setsockopt(listener_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(int));

  /* set server address space to zero */
  memset(&server_addr, 0, sizeof(server_addr));

  /* Bind the socket to the given port */
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(server_port);
  success = bind(listener_fd,
                     reinterpret_cast<struct sockaddr*>(&server_addr),
                     sizeof(server_addr));

  if (success == -1) {
    std::cerr << "Failed to bind server socket: " <<
                  strerror(errno) <<
                  std::endl;
    exit(EXIT_FAILURE);
  }

  /* Listen for incoming connections from client */
  success = listen(listener_fd, 5);
  if (success == -1) {
    std::cerr << "Failed to listen for connections: " <<
                  strerror(errno) <<
                  std::endl;
    exit(EXIT_FAILURE);
  }

  /* Make a list of fd to listen for */
  fd_set read_set;  // main fd set
  fd_set temp_read;  // temp fd set for select()
  FD_ZERO(&read_set);
  FD_ZERO(&temp_read);
  FD_SET(listener_fd, &read_set);

  /* Set the listening sock as the max fd for now */
  max_fd = listener_fd;

  /* Wait for incoming connections request */
  std::clog << "Waiting for connection..." << std::endl;

  /* server stuff */
  while (true) {
    temp_read = read_set;
    /* Clearing the read buffer so it is easier to debug */
    memset(buffer, 0, kMAXMESGLEN);

    /* Wait for a message from a socket */
    if (select(max_fd+1, &temp_read, NULL, NULL, NULL) == -1) {
      std::cerr << "Failed to select socket: " <<
                   strerror(errno) <<
                   std::endl;
      exit(EXIT_FAILURE);
    }



    /* loop through existing connections for data to read */
    for (i = 0; i <= max_fd; ++i) {
      if (FD_ISSET(i, &temp_read)) {
        if (i == listener_fd) {
          client_addr_len = sizeof(client_addr);
          ser_cli_sock_fd = accept(listener_fd,
                                   (struct sockaddr *) &client_addr,
                                   &client_addr_len);

          if (ser_cli_sock_fd == -1) {
            std::cerr << "Failed to accept client connection: " <<
                          strerror(errno) <<
                          std::endl;
            exit(EXIT_FAILURE);
          } else {
            /* add the incoming connection to read_set to be monitored */
            FD_SET(ser_cli_sock_fd, &read_set);

            /* new max fd */
            if (ser_cli_sock_fd > max_fd) {
              max_fd = ser_cli_sock_fd;
            }
            /* read client name from buffer */
            read(ser_cli_sock_fd, buffer, kMAXMESGLEN);
            /* save client name to vector(array) */
            client_name[ser_cli_sock_fd] = buffer;

            /* Print the client's name to the log */
            std::clog << client_name[ser_cli_sock_fd] <<
                         " has joined the chat." <<
                         std::endl;
          }
        } else {
          /* Handle data from client 
           * if bytes_read is <= 0 there was an error or connection is closed
           */
          if ((bytes_read = recv(i, buffer, kMAXMESGLEN, 0)) <= 0) {
            if (bytes_read == 0) {
              std::clog << client_name[i] <<
                           " has left the chat room." <<
                           std::endl;
            }
            if (bytes_read == -1) {
              std::cerr << "Failed to read from client: " <<
                            strerror(errno) <<
                            std::endl;
            }
            close(i);
            FD_CLR(i, &read_set);
            client_name.erase(i);
          } else {
            /* Someone said something, lets broadcast it */
            for (j = 0; j <= max_fd; ++j) {
              /* Check which fd said something */
              if (FD_ISSET(j, &read_set)) {
                /* Don't broadcast on server or sending socket */
                if (j != listener_fd && j != i) {
                  /* broadcast msg from buffer, if errors it prints msg */
                  if (send(j, buffer, kMAXMESGLEN, 0) == -1) {
                    std::cerr << "Failed to broadcast: " <<
                                 strerror(errno) <<
                                 std::endl;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  close(ser_cli_sock_fd);
  close(listener_fd);
  std::cout << "**Server closed**" << std::endl;
  return 0;
}
