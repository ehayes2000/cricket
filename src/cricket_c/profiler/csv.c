#include "profiler.h"
#include "profiler_cli.h"

#include <stdio.h>
#include <sys/time.h>

void format_csv(FunctionCall* calls, unsigned long n_calls){
  struct timeval cd, wd;
  for (unsigned long i = 0; i < n_calls; i ++){
    printf("%s,", calls[i].info.fn_name);
    timersub(&calls[i].end.cpu_time, &calls[i].start.cpu_time, &cd);
    timersub(&calls[i].end.wall_time, &calls[i].start.wall_time, &wd);
    printf("%lu %lu,%lu %lu,", 
      calls[i].start.cpu_time.tv_sec,
      calls[i].start.cpu_time.tv_usec,
      cd.tv_sec,
      cd.tv_usec
    );
    printf("%lu %lu,%lu %lu\n", 
      calls[i].start.wall_time.tv_sec,
      calls[i].start.wall_time.tv_usec,
      wd.tv_sec,
      wd.tv_usec
    );
  }
}

void profile_functions_csv(const char* program){
  unsigned long n_calls;
  FunctionCall* calls = time_functions(program, &n_calls);
  format_csv(calls, n_calls);
}
