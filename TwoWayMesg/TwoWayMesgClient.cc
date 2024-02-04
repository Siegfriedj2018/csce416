/* Copyright 2023 Justin Siegfried */

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>


void Clean_buffer(char &read_buf, char &wrote_buf, size_t length) {
  memset(&read_buf, 0, length);
  memset(&wrote_buf, 0, length);
}

int main(int argc, char *argv[]) {
  const size_t kMAXMESGLEN = 1024;
  const char *server_address;
  int server_port, server_client_sock_fd, success, bytes_read;
  struct sockaddr_in server_addr;
  struct hostent *host_entry;
  char read_buffer[kMAXMESGLEN], write_buffer[kMAXMESGLEN];
  std::string client_name, server_name;

  /* Check the command line for the correct number of arguments */
  if (argc < 4) {
    std::cerr << "Usage: ./TwoWayMesgClient <server address> <port> <client name>" << std::endl;
    std::cout << "Ex: ./TwoWayMesgClient localhost 5000 Test_Client" << std::endl;
    exit(EXIT_FAILURE);
  }

  /* Get server location */
  server_address = *(argv+1);
  server_port = std::stoi(*(argv+2));
  client_name = *(argv+3);

  /* Create a socket */
  server_client_sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (server_client_sock_fd == -1) {
    std::cerr << "Failed to create socket: " << strerror(errno) << std::endl;
    exit(EXIT_FAILURE);
  }

  /* Sets the server address space to zero */
  memset(&server_addr, 0, sizeof(server_addr));

  /* Fill teh server address structure */
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

  /* send the client name to server */
  strncpy(write_buffer, client_name.c_str(), sizeof(write_buffer));
  write(server_client_sock_fd, write_buffer, kMAXMESGLEN);
  /* read server name from server */
  read(server_client_sock_fd, read_buffer, kMAXMESGLEN);

  /* place server name in var */
  server_name = read_buffer;

  while (true) {
    /* Clear the read and write buffers for debugger */
    Clean_buffer(*read_buffer, *write_buffer, kMAXMESGLEN);

    /* Read user input from the keyboard */
    std::string user_input = "";
    std::getline(std::cin, user_input);
    if (user_input == "exit" || user_input == "quit") {
      break;
    }

    strncpy(write_buffer, user_input.c_str(), sizeof(write_buffer));
    write(server_client_sock_fd, write_buffer, kMAXMESGLEN);

    /* Read in input from user and place in read buffer */
    bytes_read = read(server_client_sock_fd, read_buffer, kMAXMESGLEN);

    /* Error handling */
    if (bytes_read == 0) {
      std::cerr << "server disconnected." << std::endl;
      break;
    }
    if (bytes_read == -1) {
      std::cerr << "Failed to read from server: " <<
                    strerror(errno) << std::endl;
      exit(EXIT_FAILURE);
    }

    std::cout << server_name << ": ";
    printf("%s", read_buffer);
    std::cout << std::endl;
  }

  close(server_client_sock_fd);
  std::cout << "Connection to server closed" << std::endl;
  return 0;
}
