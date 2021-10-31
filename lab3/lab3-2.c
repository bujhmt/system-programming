#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <unistd.h> 
#include <time.h> 
#include <sys/mman.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <errno.h>

#define true 1
#define INPUT_BUFFER_SIZE 128
#define SHM_NAME "/lab3"

void throwError(char* message) {
    fprintf(stderr, "%s: %s\n", message, strerror(errno));
    exit(1);
}

struct datum_s {
	pid_t pid;
	struct tm cur_time;
	char buff[100];
};

typedef struct datum_s datum_t;

int main(void) {
  char buff[INPUT_BUFFER_SIZE];
  time_t rawtime;

  int shmDescriptor = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  
  if (shmDescriptor == -1) {
      throwError("Error while trying to shm_open");
  }
  
  if (ftruncate(shmDescriptor, sizeof(datum_t)) == -1) {
      throwError("Error while trying to ftruncate shmDescriptor");
  }
  
  datum_t* datum = mmap(NULL, sizeof(datum_t), PROT_READ | PROT_WRITE, MAP_SHARED, shmDescriptor, 0);

  if (datum == MAP_FAILED) {
      throwError("Error while trying to mmap() fd");
  }

  while(true) {
    printf("Input smth:\n");
    fgets(buff, INPUT_BUFFER_SIZE, stdin);
    msync(datum, sizeof(datum_t), MS_SYNC);
    printf("DATUM (PID: %d):\nTIME: %sPrevious value:%s\n\n", datum->pid, asctime(&(datum->cur_time)), datum->buff);
  
    datum->pid = getpid();
    time(&rawtime);
    datum->cur_time = (* localtime(&rawtime));
    strcpy(datum->buff, buff);
  }
  
  munmap(datum, sizeof(datum_t));
  shm_unlink(SHM_NAME);
  return 0;
}