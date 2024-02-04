/*
 * Implementation of a two-way message server in C
 */

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define MAXMESGLEN  1024

/*
 * Read a message from the socket and store it in the given buffer
 * Returns 0 on success, -1 on error or when the client disconnects
 */
int recvMesg(int sd, char *mesg, size_t mesglen) {
    ssize_t nread;
    nread = read(sd, mesg, mesglen - 1); // Leave space for the null terminator
    if (nread == -1) {
        perror("read");
        return -1; // Error
    } else if (nread == 0) {
        // Client disconnected
        return -1;
    } else {
        mesg[nread] = '\0'; // Null-terminate the message
        return 0; // Success
    }
}

/*
 * The server program starts from here
 */
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    int serverPort = atoi(argv[1]);

    int serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSock == -1) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in serverAddr;
    bzero((char *) &serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(serverPort);
    if (bind(serverSock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == -1) {
        perror("bind");
        exit(1);
    }

    listen(serverSock, 5);

    printf("Waiting for a client ...\n");

    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSock = accept(serverSock, (struct sockaddr *) &clientAddr, &clientAddrLen);
    if (clientSock == -1) {
        perror("accept");
        exit(1);
    }

    char clientBuffer[MAXMESGLEN];
    inet_ntop(AF_INET, &clientAddr.sin_addr, clientBuffer, sizeof(clientBuffer));
    printf("Connected to a client at ('%s', '%hu')\n", clientBuffer, ntohs(clientAddr.sin_port));

    close(serverSock); // Close the server socket

    char serverBuffer[MAXMESGLEN];

    while (1) {
        // Receive a message from the client
        int recvResult = recvMesg(clientSock, clientBuffer, sizeof(clientBuffer));

        if (recvResult == -1) {
            printf("Client closed connection\n");
            break;
        }

        printf("Client: %s", clientBuffer);

        // Prompt for a response from the server
        printf("Server: ");
        if (fgets(serverBuffer, sizeof(serverBuffer), stdin) == NULL) {
            printf("Closing connection\n");
            break;
        }

        // Send the response to the client
        write(clientSock, serverBuffer, strlen(serverBuffer));
    }

    close(clientSock);
    return 0;
}
