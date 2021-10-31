#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#define WRITE_FILE_CREATE_RULES 0644
#define LOG_FILE_NAME "logs.txt"
#define SLEEP_TIME 2

int GlobalLogFileDescriptor = -1;

void signal_handler(int signo, siginfo_t * si, void * ucontext) {

  char buffer[256];
  int size = sprintf(buffer, "\nSignal number: %i\nErrno association: %i\nSignal code: %i\nSending process: %u\nSender's ruid: %u\nExit value: %i\nFaulting instruction: %p\nSignal value: %i\nBand event for SIGPOLL: %li\n", 
    si->si_signo, 
    si->si_errno,
    si->si_code,
    si->si_pid,
    si->si_uid,
    si->si_status,
    si->si_addr,
    si->si_value.sival_int,
    si->si_band
  );

  GlobalLogFileDescriptor = open(LOG_FILE_NAME, O_CREAT | O_RDWR | O_APPEND, WRITE_FILE_CREATE_RULES);
  
  
  write(GlobalLogFileDescriptor, buffer, size);
  close(GlobalLogFileDescriptor);
}

int main(void) {
  char buff[100];
  int counter = 0;
  static struct sigaction sigAction;

  int writtenBytes = sprintf(buff, "PID: %u\nStarted working...\n", getpid());
  GlobalLogFileDescriptor = open(LOG_FILE_NAME, O_CREAT | O_RDWR | O_TRUNC, WRITE_FILE_CREATE_RULES);

  write(GlobalLogFileDescriptor, buff, writtenBytes);
  
  sigAction.sa_sigaction = signal_handler;

  sigAction.sa_flags = SA_SIGINFO;
  sigaction(SIGHUP, &sigAction, NULL);

  while(1) {
    char iterationBuffer[32];
    int size = sprintf(iterationBuffer, "[%i] Waiting...\n", counter);
    write(GlobalLogFileDescriptor, iterationBuffer, size);
    sleep(SLEEP_TIME);
    counter++;
  }

  close(GlobalLogFileDescriptor);
  return 0;
}