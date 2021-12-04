#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#define WRITE_FILE_CREATE_RULES 0644
#define LOG_FILE_NAME "logs.txt"
#define PORT 5555
#define true 1

void parentProcess(int fd);
void childProcess(int fd);
void processClient(int client_sockfd, int fd);

void throwError(char* message) {
    fprintf(stderr, "%s: %s\n", message, strerror(errno));
    exit(1);
}

int main(void) {
    int logFileDescr = open(LOG_FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC, WRITE_FILE_CREATE_RULES);
    if (logFileDescr == -1) {
        throwError("Error while opening logs file");
    }

    char* startLog = "Server started...\n";
    write(logFileDescr, startLog, strlen(startLog));

    pid_t fork_id = fork();
    if (fork_id < 0) {
        char* errLog = "Error while trying to fork()\n";
        write(logFileDescr, errLog, strlen(errLog));
        close(logFileDescr);
    }
    else if (fork_id == 0) {
        childProcess(logFileDescr);
    }
    else {
        parentProcess(logFileDescr);
    }

    // End of program
    close(logFileDescr);
    printf("End of main\n");
    return 0;
}

void parentProcess(int fileDescpr) {
    char* log = "The child has been created, exiting now...\n";
    write(fileDescpr, log, strlen(log));
    close(fileDescpr);
    exit(EXIT_SUCCESS);
}

void childProcess(int fileDescpr) {
    struct sockaddr_in socketAddr;

    socketAddr.sin_family = PF_INET;
    socketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    socketAddr.sin_port = htons(PORT);

    int socketFileDescrp = socket(PF_INET, SOCK_STREAM, 0);

    if (socketFileDescrp == -1) {
        char* errLog = "[Server] ERROR when opening socket...\n";
        write(fileDescpr, errLog,strlen(errLog));
        close(fileDescpr);
        exit(EXIT_FAILURE);
    }

    if (bind(socketFileDescrp, (struct sockaddr *)&socketAddr, sizeof(socketAddr)) == -1) {
        char* errLog = "[Server] ERROR when binding...\n";
        write(fileDescpr, errLog,strlen(errLog));
        close(fileDescpr);
        exit(EXIT_FAILURE);
    }

    char temp[100];
    sprintf(temp, "[Server] Starting listening. PID: %d\n", getpid());
    write(fileDescpr, temp, strlen(temp));

    listen(socketFileDescrp, 5);

    struct sockaddr_in clientSocketAddr;
    int clientSocketFileDescr;
    int clientSocketAddrSize = sizeof(clientSocketAddr);

    while(true) {
        clientSocketFileDescr = accept(socketFileDescrp, (struct sockaddr *)&clientSocketAddr, &clientSocketAddrSize);

        if (clientSocketFileDescr < 0) {
            char* errLog = "[Server]: Error on acception connection from the client...\n";
            write(fileDescpr, errLog, strlen(errLog));
        }
        else {
            int forkId = fork();
            if (forkId < 0) {
                char* errLog = "[Server]: Error occured while trying to fork() for client\n";
                write(fileDescpr, errLog, strlen(errLog));
                close(clientSocketFileDescr);
            }
            else if (forkId == 0) {
                close(socketFileDescrp);
                processClient(clientSocketFileDescr, fileDescpr);
                close(fileDescpr);
                exit(EXIT_SUCCESS);
            }
            else {
                char* log = "[Server]: fork()'ed for client successfully\n";
                write(fileDescpr, log, strlen(log));
                close(clientSocketFileDescr);
            } 
        }
    }

    close(fileDescpr);
}

void processClient(int clientSocketFileDescpr, int fileDescpr) {
    char requestBuff[1024];
    char responseBuff[1024];

    time_t rawtime;

    do {
        int requestBytes = recv(clientSocketFileDescpr, requestBuff, sizeof(requestBuff) - 1, 0);
        requestBuff[requestBytes] = '\0';

        if (requestBytes <= 0) {
            char* log = "[Request]: empty request received, skipping it\n";
            write(fileDescpr, log, strlen(log));
            send(clientSocketFileDescpr, log, strlen(log), 0);
            continue;
        }

        time(&rawtime);
        struct tm currentTime = (* localtime(&rawtime));
        int responseBytes = sprintf(responseBuff, "PID: [%d] on %sYour message: %s", getpid(), asctime(&currentTime), requestBuff);

        if (send(clientSocketFileDescpr, responseBuff, responseBytes, 0) == -1) {
            char* errLog = "[Request]: failed to perform send() to client\n";
            write(fileDescpr, errLog, strlen(errLog));
        }
    } while (strcmp(requestBuff, "close") != 0);

    char* log = "[Request]: \"close\" command received, finishing communication with him\n";
    write(fileDescpr, log, strlen(log));
    close(clientSocketFileDescpr);
}