#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <linux/ptrace.h>
#include <stdio.h>

int main() { 
  ptrace(PTRACE_TRACEME, 0, NULL, NULL);
  if (ptrace(PTRACE_ATTACH, getpid(), NULL, NULL) < 0){ 
    perror("ff go next");
    exit(1);
  }
}
