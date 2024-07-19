#include <unistd.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <inttypes.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

int main(int argc, char* argv[]){
  pid_t child = fork();
  if (child == 0){
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    raise(SIGSTOP);
    while(1){
      write(STDOUT_FILENO, "hi\n", 4);
      sleep(3);
    }
  } else {
    int status;
    waitpid(child, &status, NULL);
    size_t addr = 24; //???
    uint32_t breakpt = 0x42000000;
    ptrace(PTRACE_POKETEXT, child, addr, breakpt);
    waitpid(child, &status, NULL);
    if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP){
      printf("we caught a breakpoint!\n");
    }
  }
}