#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 512
#define REQUIRED_ARG_COUNT 3
#define WRITE_FILE_CREATE_RULES 0644
#define ASCII_UPPECASE_MIN 65
#define ASCII_UPPECASE_MAX 90
#define ASCII_DIFF 32

void throwError(char* message) {
    fprintf(stderr, "%s: %s\n", message, strerror(errno));
    exit(1);
}

char* initializeBuffer(size_t size) {
    char* buffer = malloc(size);
    memset(buffer, 0, size);

    return buffer;
}

void toLower(char* string, size_t length) {
    for (size_t i = 0; i < length; i++) {
        char letter = string[i];

        if (letter >= ASCII_UPPECASE_MIN && letter <= ASCII_UPPECASE_MAX) {
            string[i] = letter + ASCII_DIFF;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != REQUIRED_ARG_COUNT) {
        throwError("Invalid count of input arguments");
    }

    char * readFileName = argv[1];
    char * writeFileName = argv[2];

    // read file
    int readFileDescriptor = open(readFileName, O_RDONLY);
    if (readFileDescriptor == -1) {
        throwError("Read file can't be opened!");
    }

    // write to file
    int writeFileDescriptor = open(writeFileName, O_WRONLY | O_CREAT | O_TRUNC, WRITE_FILE_CREATE_RULES);
    if (writeFileDescriptor == -1) {
        throwError("Write file can't be opened!");
    }

    char *buffer = initializeBuffer(BUFFER_SIZE);

    int readBytesCount = 0;
    int allBytesCount = 0;
    while((readBytesCount = read(readFileDescriptor, buffer, BUFFER_SIZE)) > 0) {
        toLower(buffer, readBytesCount);
        write(writeFileDescriptor, buffer, readBytesCount);
        allBytesCount += readBytesCount;
    }

    // read from file error
    if (readBytesCount == -1) {
        throwError("Read file error");
    }

    printf("Rewritten bytes: %i\n", allBytesCount);

    // end stage
    free(buffer);
    close(readFileDescriptor);
    close(writeFileDescriptor);

    return 0;
}