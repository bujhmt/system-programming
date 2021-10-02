#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define NUMBER_OF_ITERATIONS 100

void throwError(char* message) {
    fprintf(stderr, "%s: %s\n", message, strerror(errno));
    exit(1);
}

void printInfo(char* key) {
    pid_t currentPID = getpid();
    gid_t currentGID = getgid();
    pid_t currentSID = getsid(currentPID);

    if (currentSID == -1) {
        throwError("Error while trying to get SID of process");
    }

    printf("[%s] PID: %i\n", key, currentPID);
    printf("[%s] GID: %i\n", key, currentGID);
    printf("[%s] SID: %i\n", key, currentSID);
    printf("\n");
}

void printProcessesInfo(char* key) {
    for (int i = 0; i < NUMBER_OF_ITERATIONS; i++) {
        printf("%i: ", i);
        printInfo(key);
    }
}

int main() {
    printInfo("Initial");
    pid_t childProcessPID = fork();

    if(childProcessPID < 0)
    {    
        throwError("Fork failed");
    }

    if (childProcessPID == 0) {
        printProcessesInfo("Child");
        printf("Child process ended...\n");

        return 0;
    } else {
        int resultInfo;
        printProcessesInfo("Parent");
        wait(&resultInfo);
        printf("\nParent process ended...\nStatus: %i\n", resultInfo);

        return 0;
    }

    return 0;
}