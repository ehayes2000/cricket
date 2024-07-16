#include <errno.h>
#include <string.h>
#include <stdio.h>
#include "time_functions.h"


void handle_stop(InstnTable*, pid_t, size_t);
void profile_functions(char* program){ 
  // map offsets to source info
  InstnTable* src_map = map_functions(program); 
  pid_t child = fork();
  // child
  if (child == 0){
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    execl(program, NULL);
    fpritnf(stderr, "exec failed with %s\n", strerror(errno));
    exit(errno);
  } 
  // parent    
  int status, err;
  err = waitpid(child, &status, 0);
  if (err != -1){
    fprintf(stderr, "waitpid failed with %s\n", strerror(errno));
    exit(errno);
  }
  if (WIFEXITED(status)){
    fprintf(stderr, "child exited\n");
    exit(1);
  }
  // get base address of child
  size_t base_address = get_process_base_address(child);
  // set breakpoints at all functions
  for (InstnNode* i = table_iterate(src_map); i != NULL; i = i->next){
    i->word = enable_breakpoint(child, base_address + i->instn);
  }
  ptrace(PTRACE_SETOPTIONS, child, NULL, PTRACE_O_EXITKILL | PTRACE_O_TRACESYSGOOD);    
  while (1){
    waitpid(child, &status, 0);
    if (WIFEXITED(status)){
      printf("child exited\n");
      return;
    } else if (WIFSTOPPED(status)){
      handle_stop(src_map, child, base_address);
    }
    if (ptrace(PTRACE_CONT, child, NULL, NULL) == -1){
      fprintf(stderr, "failed to restart child %s\n", strerror(errno));
      exit(errno);
    }
  }
}


struct timeval dirty_time;
bool call_done;

void handle_stop(InstnTable* src_map, pid_t child, size_t base_address){

}