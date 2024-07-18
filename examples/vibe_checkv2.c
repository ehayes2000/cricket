#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <signal.h>

int main(){
  pid_t child = fork();
  if (child == 0){
    printf("child!\n");
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    raise(SIGTOP);
    printf("spinning\n");
    while (1){
      ;
    }
    printf(":(\n");
  } else {
    printf("parent\n");
    wait()
  }
}