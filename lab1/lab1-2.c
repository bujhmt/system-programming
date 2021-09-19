#include <stdio.h>
#include <errno.h>
#include <sys/select.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define REQUIRED_ARG_COUNT 2
#define TIME_INTERVAL_SECONDS 5
#define BUFFER_SIZE 1024

void throwError(char* message) {
    fprintf(stderr, "%s: %s\n", message, strerror(errno));
    exit(1);
}

char* initializeBuffer(size_t size) {
    char* buffer = malloc(size);
    memset(buffer, 0, size);

    return buffer;
}

int main(int argc, char* argv[]) {
    if (argc != REQUIRED_ARG_COUNT) {
        throwError("Invalid count of input arguments");
    }

    fd_set readFileDescriptors;
    struct timeval timeValue;

    // initialize stage
    FD_ZERO(&readFileDescriptors);
    FD_SET(0, &readFileDescriptors);

    timeValue.tv_sec = TIME_INTERVAL_SECONDS;
    timeValue.tv_usec = 0;

    // main stage 
    char* key = argv[1];
    char* buffer = initializeBuffer(BUFFER_SIZE);

    int selectStatus = 0;
    while((selectStatus = select(STDIN_FILENO + 1, &readFileDescriptors, NULL, NULL, &timeValue)) != -1) {
        if (selectStatus) {
            int readBytes = read(STDIN_FILENO, buffer, BUFFER_SIZE);

            if (readBytes == -1) {
                throwError("Read error");
            }

            fprintf(stdout, "[%s]: %s", key, buffer);
        } else {
            fprintf(stderr, "[%s] No input at last 5 seconds\n", key);
        }

        FD_ZERO(&readFileDescriptors);
        FD_SET(STDIN_FILENO, &readFileDescriptors);
        timeValue.tv_sec = 5;
        timeValue.tv_usec = 0;
        memset(buffer, 0, BUFFER_SIZE);
    }

    if (selectStatus == -1) {
        throwError("Select crashed");
    }

    // end stage 
    free(buffer);
    return 0;
}