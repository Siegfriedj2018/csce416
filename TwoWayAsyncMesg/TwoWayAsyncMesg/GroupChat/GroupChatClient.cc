/* Copyright 2023 Justin Siegfried */

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>


void Clean_buffer(char *read_buf, char *write_buf, size_t length) {
  memset(read_buf, 0, length);
  memset(write_buf, 0, length);
}

int main(int argc, char *argv[]) {
  const size_t kMAXMESGLEN = 1024;
  const char *server_address;
  int server_port, server_client_sock_fd, success, bytes_read;
  struct sockaddr_in server_addr;
  struct hostent *host_entry;
  char read_buffer[kMAXMESGLEN], write_buffer[kMAXMESGLEN];
  std::string client_name;

  /* Check the command line for the correct number of arguments */
  if (argc < 4) {
    std::cerr <<
      "Usage: ./GroupChatClient <server address> <port> <client name>" <<
      std::endl;
    std::cout << "\tEx: ./GroupChatClient localhost 5000 John" << std::endl;
    exit(EXIT_FAILURE);
  }

  /* Get server location */
  server_address = *(argv+1);
  server_port = std::stoi(*(argv+2));
  client_name = *(argv+3);
  // client_name += ": ";


  /* Create a socket */
  server_client_sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (server_client_sock_fd == -1) {
    std::cerr << "Failed to create socket: " << strerror(errno) << std::endl;
    exit(EXIT_FAILURE);
  }

  /* Sets the server address space to zero */
  memset(&server_addr, 0, sizeof(server_addr));

  /* Fill the server address structure */
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(server_port);

  /* Get the IP address of the corresponding server host */
  host_entry = gethostbyname(server_address);
  if (!host_entry) {
    std::cerr << "Server not found: " << strerror(errno) << std::endl;
    exit(EXIT_FAILURE);
  }
  memmove(reinterpret_cast<char *>(&server_addr.sin_addr),
          host_entry->h_addr_list[0],
          host_entry->h_length);

  /* Connect to the server */
  success = connect(server_client_sock_fd,
                    reinterpret_cast<const sockaddr*>(&server_addr),
                    sizeof(server_addr));
  if (success == -1) {
    std::cerr << "Failed to connect to server: " <<
                 strerror(errno) <<
                 std::endl;
    exit(EXIT_FAILURE);
  }

  std::cout << "Connected to server at ('" << server_address << "', '" <<
               server_port << "')" << std::endl;

  /* notifing user of easy way to exit */
  std::cout << "To exit, type 'exit' or 'quit' followed by enter" << std::endl;

  /* send client name to server for logging */
  strncpy(write_buffer, client_name.c_str(), sizeof(client_name));
  write(server_client_sock_fd, write_buffer, kMAXMESGLEN);


  while (true) {
    /* Clear the read and write buffers for debugger */
    Clean_buffer(read_buffer, write_buffer, kMAXMESGLEN);
    fd_set read_set;
    FD_ZERO(&read_set);
    FD_SET(0, &read_set);
    FD_SET(server_client_sock_fd, &read_set);

    /* Wait for a message from the keyboard or socket */
    if (select(server_client_sock_fd+1, &read_set, NULL, NULL, NULL) == -1) {
      std::cerr << "Failed to select socket: " <<
                   strerror(errno) <<
                   std::endl;
      exit(EXIT_FAILURE);
    }

    // /* Read user input from the keyboard */
    if (FD_ISSET(0, &read_set)) {
      std::string user_input = "";
      std::getline(std::cin, user_input);
      if (user_input == "exit" || user_input == "quit") {
        break;
      }

      strncpy(write_buffer, client_name.c_str(), kMAXMESGLEN);
      strncat(write_buffer, ": ", 3);
      // dont know why -1 works
      strncat(write_buffer, user_input.c_str(), kMAXMESGLEN-1);
      write(server_client_sock_fd, write_buffer, kMAXMESGLEN);
    }

    /* Read in input from user and place in read buffer */
    if (FD_ISSET(server_client_sock_fd, &read_set)) {
      bytes_read = read(server_client_sock_fd, read_buffer, kMAXMESGLEN);

      /* Error handling */
      if (bytes_read == 0) {
        std::cerr << "Server Disconnected." << std::endl;
        break;
      }
      if (bytes_read == -1) {
        std::cerr << "Failed to read from server: " <<
                      strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
      }

      std::cout << read_buffer;
      std::cout << std::endl;
    }
  }

  close(server_client_sock_fd);
  std::cout << "Connection to server closed" << std::endl;
  return 0;
}
