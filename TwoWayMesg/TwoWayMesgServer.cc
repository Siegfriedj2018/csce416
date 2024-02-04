/* Copyright 2023 Justin Siegfried */

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>


int main(int argc, char *argv[]) {
  const size_t kMAXMESGLEN = 1024;
  int server_sock_fd, server_port, server_client_sock_fd;
  int bytes_read, success;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_addr_len;
  char buffer[kMAXMESGLEN];
  std::string server_name, client_name;

  /* check the commandline for the correct number of arguements */
  if (argc < 3) {
    std::cerr << "Usage: ./TwoWayMesgServer <port> <server name>" << std::endl;
    std::cout << "\tEx: ./TwoWayMesgServer 5000 Test_server" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  /* Gets the port number to listen one from user entry */
  // server_port = std::stoi(*(argv+1));
  // server_name = *(argv+2);
  server_port = 50000;
  server_name = "Server";

  /* Create the server socket */
  server_sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (server_sock_fd == -1) {
    std::cerr << "Failed to create socket: " << strerror(errno) << std::endl;
    exit(EXIT_FAILURE);
  }

  /* set server address space to zero */
  memset(&server_addr, 0, sizeof(server_addr));

  /* Bind the socket to the given port */
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(server_port);
  success = bind(server_sock_fd,
                     reinterpret_cast<struct sockaddr*>(&server_addr),
                     sizeof(server_addr));

  if (success == -1) {
    std::cerr << "Failed to bind server socket: " <<
                  strerror(errno) <<
                  std::endl;
    exit(EXIT_FAILURE);
  }

  /* Listen for incoming connections from client */
  success = listen(server_sock_fd, 5);
  if (success == -1) {
    std::cerr << "Failed to listen for connections: " <<
                  strerror(errno) <<
                  std::endl;
    exit(EXIT_FAILURE);
  }

  /* Wait for incoming connections request */
  std::clog << "Waiting for connection..." << std::endl;
  client_addr_len = sizeof(client_addr);
  server_client_sock_fd = accept(server_sock_fd,
                                 (struct sockaddr *) &client_addr,
                                 &client_addr_len);

  if (server_client_sock_fd == -1) {
    std::cerr << "Failed to accept client connection: " <<
                  strerror(errno) <<
                  std::endl;
    exit(EXIT_FAILURE);
  }

  /* Print the client's ip address and port */
  inet_ntop(AF_INET, &client_addr.sin_addr, buffer, sizeof(buffer));
  std::cout << "Connected to client at ('" << buffer << "', '" <<
                ntohs(client_addr.sin_port) << "')" << std::endl;

  /* No more clients close the server socket */
  close(server_sock_fd);

  /* notifing user of easy way to exit */
  std::cout << "To exit, type 'exit' or 'quit' followed by enter" << std::endl;

  /* read client name from client */
  read(server_client_sock_fd, buffer, kMAXMESGLEN);
  /* send server name to client */
  strncpy(buffer, server_name.c_str(), sizeof(buffer));
  write(server_client_sock_fd, buffer, kMAXMESGLEN);

  /* place client name in var */
  client_name = buffer;

  /* client stuff */
  while (true) {
    /* Clearing the read and write buffers so it is easier to debug */
    memset(buffer, 0, kMAXMESGLEN);
    // memset(wbuffer, 0, kMAXMESGLEN);

    // /* Read in input from user and place in read buffer */
    // bytes_read = read(server_client_sock_fd, read_buffer, kMAXMESGLEN);

    // /* Error handling */
    // if (bytes_read == 0) {
    //   std::cerr << "Client disconnected." << std::endl;
    //   break;
    // }

    // if (bytes_read == -1) {
    //   std::cerr << "Failed to read from client: " <<
    //                 strerror(errno) << std::endl;
    //   exit(EXIT_FAILURE);
    // }

    // std::cout << client_name << ": ";
    // printf("%s", read_buffer);
    // std::cout << std::endl;

    // std::string user_input = "";
    // std::getline(std::cin, user_input);
    // if (user_input == "exit" || user_input == "quit") {
    //   break;
    // }

    // // Copy the user input into the buffer and send it to the client
    // strncpy(write_buffer, user_input.c_str(), sizeof(write_buffer));
    // write(server_client_sock_fd, write_buffer, kMAXMESGLEN);
  }


  close(server_client_sock_fd);
  std::cout << "Connection to client closed" << std::endl;
  return 0;
}
