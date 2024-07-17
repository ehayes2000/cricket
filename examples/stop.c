#include <stdio.h>
#include <signal.h>

int main() { 
  printf("hello world\n");
  raise(SIGSTOP);
  printf("goodbye world\n");

}