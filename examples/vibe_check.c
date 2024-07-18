#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/ptrace.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

int main() { 
  printf("hello world\n");
  // trace this process :)
  while(1){
;
  }
  ptrace(PTRACE_TRACEME, 0, NULL, NULL);
  // attach tracer to this process :(
  
  long err= ptrace(PTRACE_ATTACH, getpid(), NULL, NULL);
  if (err < 0){
    perror("ff go next");
    exit(1);
  }
}
