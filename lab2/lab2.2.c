#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define REQUIRED_ARG_COUNT 2
#define WRITE_FILE_CREATE_RULES 0644

void writeToFile(int fileDescriptor, char* str) {
   write(fileDescriptor, str, strlen(str));
}

void throwError(int fileDescriptor, char* message) {
    writeToFile(fileDescriptor, message);
    exit(1);
}

void closeAllParentDescriptors() {
    for (int i = 0; i < 255; i++) {
        close(i);
    }
}

int openFileToWrite(char* fileName) {
    int fileDescriptor = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, WRITE_FILE_CREATE_RULES);
    if (fileDescriptor == -1) {
        fprintf(stderr, "%s: %s\n", "Write file can't be opened!\n", strerror(errno));
        exit(1);
    }

    return fileDescriptor;
}

int main(int argc, char* argv[]) {
    if (argc != REQUIRED_ARG_COUNT) {
        fprintf(stderr, "%s: %s\n", "Invalid arguments count\n", strerror(errno));
        exit(1);
    }

    char * writeFileName = argv[1];
    int writeFileDescriptor = openFileToWrite(writeFileName);

    writeToFile(writeFileDescriptor, "Parent Process started\n");
    pid_t childPid = fork();

    if (childPid < 0) {    
        throwError(writeFileDescriptor, "Fork failed");
    }

    if (childPid == 0) {
        pid_t newSid = setsid();
        if (newSid == -1) {
            throwError(writeFileDescriptor, "Error while calling setsid\n");
        }
        if (chdir("/") == -1) {
            throwError(writeFileDescriptor, "Error while calling chdir\n");
        }

        closeAllParentDescriptors();
        open("/dev/null", O_RDWR); 
        dup(0); 
        dup(0);

        int newFileDescriptor = openFileToWrite(writeFileName);
        char buffer[100];
        int bufferLength = sprintf(buffer, "PID: %d, GID: %d, SID: %d\n", getpid(), getgid(), newSid);
        write(newFileDescriptor, buffer, bufferLength);

        while(1) {  
            time_t cur = time(NULL);
            char * stime = ctime(&cur);
            bufferLength = sprintf(buffer, "PID: %d, Current time is: %s", getpid(), stime);
            write(newFileDescriptor, buffer, bufferLength);
            sleep(5);
        }

        close(newFileDescriptor);
        return 0;
    } else {
        writeToFile(writeFileDescriptor, "Parent Process exit\n");
        exit(EXIT_SUCCESS);
    }

    return 0;
}