#include <seccomp.h>
#include <sys/ptrace.h>
// this include is needed to include ptrace_syscall_info
#include <linux/ptrace.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/time.h>
#include "util/src_map.h"
#include "ptrace_util/tracee.h"
/*
A c program that uses ptrace to print all system calls
made by a child
*/

void config_ptrace(pid_t);
void parent_process(pid_t);
void child_process(char* program[]);
struct timeval call_time;
int timeset = 0;
char* call;

int main(int argc, char* argv[]) {
  pid_t child = fork();
  if (child == 0){
    child_process(argv + 1);
  } else { 
    parent_process(child);
  }
  return 0;
}

void child_process(char* args[]) {
  ptrace(PTRACE_TRACEME, 0, NULL, NULL);
  execl("examples/say_hi", NULL);
  printf("exec failed\n");
}

void config_ptrace(pid_t pid) { 
  // config ptrace on pid, for tracing syscalls
  // exit on fail to ptrace
  // resume tracee with syscall tracing
  int status, err; 
  err = waitpid(pid, &status, 0);
  if (err == -1 ){
    fprintf(stderr, "waitpid failed with %s\n", strerror(errno));
    exit(err);
  }
  printf("config ptrace\n");
  if (WIFEXITED(status)){
    printf("child exited\n");
    exit(1);
  }
  InstnTable* fns = map_functions("examples/say_hi");
  InstnNode* head = table_iterate(fns);
  printf("child pid: %lu\n", pid);
  printf("fn: %s, file: %s, line %lu\n", head->info.source.function->name, head->info.source.function->src_file, head->info.source.function->line);
  size_t base_address = get_process_base_address(pid);
  size_t break_offset = table_iterate(fns)->instn;
  printf("%p process base address \n", base_address);
  printf("%p offset\n",break_offset);
  printf("%p offset address \n", base_address + break_offset);
  long orig = enable_breakpoint(pid, break_offset + base_address);
  
  err = ptrace(PTRACE_SETOPTIONS, pid, NULL, PTRACE_O_EXITKILL | PTRACE_O_TRACESYSGOOD);
  if (err == -1){ 
    fprintf(stderr, "ptrace failed %s\n", strerror(errno));
    exit(err);
  }
  err = ptrace(PTRACE_CONT, pid, NULL, NULL);
  if (err == -1){ 
    fprintf(stderr, "ptrace failed %s\n", strerror(errno));
    exit(err);
  }  

}

void show_stop_info(pid_t pid, int status) {
  int signal = WSTOPSIG(status);
  printf("stopped by %s\n", strsignal(signal));
  struct user_regs_struct regs;
  struct iovec iov;
  iov.iov_base = &regs;
  iov.iov_len = sizeof(regs);
  ptrace(PTRACE_GETREGSET, pid, NT_PRSTATUS, &iov);
  printf("%p pc at stop\n", regs.pc);
}

void parent_process(pid_t child) { 
  config_ptrace(child);
  int status;
  while (1) { 
    waitpid(child, &status, 0);
    // wait(&status);
    if (WIFEXITED(status)) {
      printf("child exited\n");
      return;
    } else if (WIFSTOPPED(status)) { 
      show_stop_info(child, status);
      // printf("stopped!!!\n");

      
    }
    // if (ptrace(PTRACE_CONT, child, NULL, NULL) == -1){
    //   fprintf(stderr, "PTRACE_SYSCALL failed %s\n", strerror(errno));
    // }
  }
}