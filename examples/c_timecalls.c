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
#include <sys/resource.h>
#include <sys/time.h>
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
  err = ptrace(PTRACE_ATTACH, pid, NULL, NULL);
  if (err == -1){ 
    fprintf(stderr, "ptrace failed %s\n", strerror(errno));
    exit(err);
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
    fprintf(stderr, "error collecint syscall info: %s\n", strerror(errno));
    exit(errno);
  }
  int syscall = -1;
  struct rusage usage;
  struct timeval difference;
  if (info.op == PTRACE_SYSCALL_INFO_ENTRY) { 
    timeset = 1;
    call = seccomp_syscall_resolve_num_arch(info.arch, info.entry.nr);
    printf("%s - ", call);
    if (getrusage(0, &usage) == -1) {
      fprintf(stderr, "error collecting [ENTRY] usage for %s\n. failed with error: %s", call, strerror(errno));
      printf("fail\n");
      exit(errno);
    }
    call_time = usage.ru_stime;
  } else if (info.op == PTRACE_SYSCALL_INFO_EXIT && timeset != 0){ 
    if (getrusage(0, &usage) == -1){
      fprintf(stderr, "error collecting [EXIT] usage for %s\n. failed with error: %s", call, strerror(errno));
      printf("faile\n");
      exit(errno);
    }
    printf("ok\n");
    timersub(&usage.ru_stime, &call_time, &difference);
    printf("\tstart: %lds %ldμs\n", call_time.tv_sec, call_time.tv_usec);
    printf("\tstop: %lds %ldμs\n", usage.ru_stime.tv_sec, usage.ru_stime.tv_usec);
    printf("\tduration: %lds %ldμs\n", difference.tv_sec, difference.tv_usec);
    printf("\n");
  }
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
      get_stop_syscall(child);
    }
    if (ptrace(PTRACE_SYSCALL, child, NULL, NULL) == -1){
      fprintf(stderr, "PTRACE_SYSCALL failed %s\n", strerror(errno));
    }
  }
}