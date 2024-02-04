/*
 * Implementation of a two-way async message client in C
 */


/* Include header files for socket related functions */
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

/* Maximum message length */
#define MAXMESGLEN  1024

/*
 * Read a message from the socket
 * Put the message in the given buffer
 * Return NULL if nothing to read
 * Otherwise, return the message
 */
char *recvMesg(int sd, char *mesg)
{
  /* Keep reading one char at a time */
  char *ptr = mesg;
  while (1) {
    /* Read one char into the buffer */
    int nread = read(sd, ptr, sizeof(char));

    /* If errors, report and exit */
    if (nread == -1) {
      perror("socket");
      exit(1);
    }

    /* If no byte read, i.e., EOF, return NULL */
    if (nread == 0)
      return(NULL);

    /* If newline char, thats the end of message */
    if (*ptr == '\n') {
      /* String should end with a null char */
      *(ptr+1) = 0;
      break;
    }

    /* Advance the pointer to place the next char */
    ptr++;
  }

  /* Return the message */
  return(mesg);
}

/*
 * The client program starts from here
 */
int main(int argc, char *argv[])
{
  /* Client needs server's contact information */
  if (argc != 4) {
    fprintf(stderr, "usage : %s <server name> <server port> <client name>\n", argv[0]);
    exit(1);
  }

  /* Get server whereabouts */
  char *serverName = argv[1];
  int serverPort = atoi(argv[2]);
  char *client_name = argv[3];

  /* Create a socket */
  int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock == -1) {
    perror("socket");
    exit(1);
  }

  /* Fill the server address structure */
  struct sockaddr_in serverAddr;
  bzero((char *) &serverAddr, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(serverPort);

  /* Get the IP address corresponding to server host */
  struct hostent *hostEntry = gethostbyname(serverName);
  if (!hostEntry) {
    perror(serverName);
    exit(1);
  }
  bcopy(hostEntry->h_addr, (char *) &serverAddr.sin_addr, hostEntry->h_length);

  /* Connect to the server */
  if (connect(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == -1) {
    perror("connect");
    exit(1);
  }
  printf("Connected to server at ('%s', '%d')\n", serverName, serverPort);

  /* Keep sending and receiving messages from the client */
  char buffer[MAXMESGLEN];
  while (1) {
    /* Make a set of inputs to watch for */
    fd_set inputSet;
    FD_ZERO(&inputSet);
    FD_SET(0, &inputSet);
    FD_SET(sock, &inputSet);

    /* Wait for a message from keyboard or socket */
    if (select(sock+1, &inputSet, NULL, NULL, NULL) == -1) {
      perror("select");
      exit(1);
    }

    /* Check if there is a message from the keyboard */
    if (FD_ISSET(0, &inputSet)) {
      /* Read a line from the keyboard */
      char *line = fgets(buffer, MAXMESGLEN, stdin);

      /* If EOF, close the connection */
      if (line == NULL) {
        printf("*** Client closing connection\n");
        break;
      }

      /* Send the line to the server */
      write(sock, line, strlen(line));
    }

    /* Check if there is a message from the client */
    if (FD_ISSET(sock, &inputSet)) {
      char *message = recvMesg(sock, buffer);

      /* if message is NULL ==> client closed connection */
      if (message == NULL) {
        printf("*** Server closed connection\n");
        close(sock);
        break;
      }

      /* Display the message */
      printf("%s: %s", client_name, message);
    }
  }

  /* Close the connection */
  close(sock);
}
