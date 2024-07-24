#include "profiler.h"
#include "breakpoint.h"
#include "time_stack/time_stack.h"
#include "src_map/src_map.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/ptrace.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>

Instant get_child_time(pid_t pid){
  struct timeval wall_time;
  gettimeofday(&wall_time, NULL);
  struct rusage usage;
  if (getrusage(RUSAGE_SELF, &usage) == -1){  
    fprintf(stderr, "timing failed\n"); 
    exit(420);
  }
  time(NULL);
  struct timeval cpu_time;
  timeradd(&usage.ru_stime, &usage.ru_utime, &cpu_time);
  return (Instant){
    .cpu_time = cpu_time,
    .wall_time = wall_time
  };
}

// void format_interval(Instant *start, Instant *end, SourceInfo *src){
//   struct timeval cpu_diff; 
//   timersub(&end->cpu_time, &start->cpu_time, &cpu_diff);
//   struct timeval wall_diff;
//   timersub(&end->wall_time, &start->wall_time, &wall_diff);

//   struct tm *human_time = localtime(&start->wall_time.tv_sec);
//   char timestamp[50];
//   strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", human_time);

//   printf("\n[%s]\n", src->fn_name);
//   printf("- wall time\n");
//   printf("  %lus %luμ duration\n", wall_diff.tv_sec, wall_diff.tv_usec);
//   printf("  %s start\n", timestamp);
//   // printf("  %lus %luμ start\n", start->wall_time.tv_sec, start->wall_time.tv_usec);
//   printf("- cpu time\n");
//   printf("  %lus %luμ duration\n", cpu_diff.tv_sec, cpu_diff.tv_usec);
//   printf("  %lus %luμ start\n", start->cpu_time.tv_sec, start->cpu_time.tv_usec);
// }
#define FN_ARR_SIZE 1000

typedef struct { 
  unsigned int cap;
  unsigned int len;
  FunctionCall* buf;
} FnArray;

FnArray newFnArray(){
  FunctionCall* buf = malloc(sizeof(FunctionCall) * FN_ARR_SIZE);
  return (FnArray){
    .buf = buf,
    .cap = FN_ARR_SIZE,
    .len = 0,
  };
}

FnArray appendFnCall(FnArray arr, FunctionCall call){
  if (arr.len >= arr.cap){
    FunctionCall* new_buf = malloc(sizeof(FunctionCall) * 2 * arr.cap);
    memcpy(new_buf, arr.buf, sizeof(FunctionCall) * arr.len);
    arr.cap *= 2;
    free(arr.buf);
    arr.buf = new_buf;
  }
  arr.buf[arr.len++] = call;
  return arr;
}

FunctionCall* time_functions(const char* program, unsigned long *n_calls){
  FnArray calls = newFnArray();
  SrcMap* src_map = newSrcMap(program);
  TimeStack* times = newTimeStack();
  pid_t child = fork();
  if (child == 0){
    dup2(STDERR_FILENO, STDOUT_FILENO);
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    execl(program, NULL);
    perror("exec failed");
    exit(errno);
  }
  int status;
  if (waitpid(child, &status, 0) == -1){
    perror("waidpid failed");
    exit(errno);
  }
  if (!WIFSTOPPED(status)) {
    perror("unexpected wait condition\n");
    exit(4);
  }
  ptrace(PTRACE_SETOPTIONS, child, NULL, PTRACE_O_EXITKILL);
  size_t base_address = ptraceGetBaseAddress(child);
  ptraceSetBreakpoints(child ,src_map, base_address);
  ptrace(PTRACE_CONT, child, NULL, NULL);
  while (1){
    waitpid(child, &status, 0);
    if (WIFEXITED(status)){
      int exitcode = WEXITSTATUS(status);
      fprintf(stderr, "exited %d\n", exitcode);
      *n_calls = calls.len;
      return calls.buf;
    } else if (WIFSIGNALED(status)){
      int termsig = WTERMSIG(status);
      fprintf(stderr, "terminated by %s\n", strsignal(termsig));
      *n_calls = calls.len;
      return calls.buf;
    } else if (WIFSTOPPED(status)){
      int stopsig = WSTOPSIG(status);
      if (stopsig != SIGTRAP){
        fprintf(stderr, "unexpected stop signal %s\n", strsignal(stopsig));
        exit(1);
      }
    }
    size_t pc = ptraceGetPc(child);
    SourceInfo src;
    if(!srcMapGetInfo(src_map, pc - base_address, &src)){
      fprintf(stderr, "could not find source entry\n");
      exit(2);
    }
    Instant child_time = get_child_time(child);
    if (src.is_exit && !emptyTimeStack(times)){
      Instant start = popTimeStack(times);
      FunctionCall call = (FunctionCall){
        .start = start,
        .end = child_time,
        .info = src
      };
      FnArray p = calls;
      calls = appendFnCall(calls, call);
      // format_interval(&start, &child_time, &src);
    } else if (src.is_exit && emptyTimeStack(times)){
      fprintf(stderr, "empty time stack on exit :/\n");
      exit(69);
    } else {
      pushTimeStack(times, child_time);
    }
    if (!ptraceContBreakpoint(child, &src, pc)){
      fprintf(stderr, "problem resuming process\n");
      exit(3);
    }
  }
}