#include <seccomp.h>
#include <sys/ptrace.h>
// this include is needed to include ptrace_syscall_info
#include <linux/ptrace.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
/*
A c program that uses ptrace to print all system calls
made by a child
*/

void config_ptrace(pid_t);
void parent_process(pid_t);
void child_process(char* program[]);

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
  execl("/bin/ls", "ls", NULL);
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
  if (WIFEXITED(status)){
    printf("child exited\n");
    exit(1);
  }
  err = ptrace(PTRACE_SETOPTIONS, pid, NULL, PTRACE_O_EXITKILL | PTRACE_O_TRACESYSGOOD);
  if (err == -1){ 
    fprintf(stderr, "ptrace failed %s\n", strerror(errno));
    exit(err);
  }
  err = ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
  if (err == -1){ 
    fprintf(stderr, "ptrace failed %s\n", strerror(errno));
    exit(err);
  }
}

void get_stop_syscall(pid_t pid){    
  /*
    Get the syscall causing the tracee to stop
  */
  struct ptrace_syscall_info info;
  int err = ptrace(PTRACE_GET_SYSCALL_INFO, pid, sizeof(struct ptrace_syscall_info), &info);
  if (err == -1) { 
    fprintf(stderr, "PTRACE_GET_SYSCALL_INFO failed: %s\n", strerror(errno));
    exit(err);
  }
  int syscall = -1;
  char* strsyscall = "unknown";
  if (info.op == PTRACE_SYSCALL_INFO_ENTRY) { 
    syscall = info.entry.nr;
    strsyscall = seccomp_syscall_resolve_num_arch(info.arch, info.entry.nr);
    printf("PTRACE_GET_SYSCALL_INFO CALL %s\n", strsyscall);
  } else if (info.op == PTRACE_SYSCALL_INFO_EXIT){ 
    printf("PTRACE_GET_SYSCALL_INFO COMPLETE %d\n", info.exit.rval);
  }
}

void parent_process(pid_t child) { 
  config_ptrace(child);
  int status;
  struct ptrace_syscall_info info; 
  while (1) { 
    waitpid(child, &status, 0);
    // wait(&status);
    if (WIFEXITED(status)) {
      printf("child exited\n");
      return;
    } else if (WIFSTOPPED(status)) { 
      get_stop_syscall(child);
    }
    if (ptrace(PTRACE_SYSCALL, child, NULL, NULL) == -1){
      fprintf(stderr, "PTRACE_SYSCALL failed %s\n", strerror(errno));
    }
  }
}