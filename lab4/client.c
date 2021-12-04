#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#define PORT 5555

void throwError(char* message) {
    fprintf(stderr, "%s: %s\n", message, strerror(errno));
    exit(1);
}

int main(void) {
    char requestBuffer[1024];
    char responseBuff[1024];


    struct sockaddr_in socketAddr;
    socketAddr.sin_family = PF_INET;
    socketAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    socketAddr.sin_port = htons(PORT);

    int socketFileDescpr = socket(PF_INET, SOCK_STREAM, 0);
    if (socketFileDescpr == -1) {
        throwError("[Client] Error while opening socket\n");
    }

    if (connect(socketFileDescpr, (struct sockaddr *)&socketAddr, sizeof(socketAddr)) == -1) {
        throwError("[Client] Error while calling connect()\n");
    }

    do {
        printf("> ");
        fgets(requestBuffer, 1024, stdin);
        int requestBytes = strlen(requestBuffer);
        if ((requestBytes > 0) && (requestBuffer[requestBytes - 1] == '\n')) {
            requestBuffer[--requestBytes] = '\0';
        }

        if (send(socketFileDescpr, requestBuffer, requestBytes, 0) == -1) {
            printf("[Client]: failed to perform send() to server\n");
        }

        int responseBytes = recv(socketFileDescpr, responseBuff, sizeof(responseBuff) - 1, 0);
        responseBuff[responseBytes] = '\0';

        printf("[Client]: Response from server:\n%s\n", responseBuff);
    } while(strcmp(requestBuffer, "close") != 0);

    close(socketFileDescpr);

    printf("\n [Client]: Client stopped\n");
    return 0;
}