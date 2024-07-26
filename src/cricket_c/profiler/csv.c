#include "profiler.h"
#include "profiler_cli.h"

#include <stdio.h>
#include <sys/time.h>

void format_csv(Call c){
  if (!c.is_exit)
    return;
  struct timeval cd, wd;
  printf("%s,", c.src.fn_name);
  timersub(&c.end.cpu_time, &c.start.cpu_time, &cd);
  timersub(&c.end.wall_time, &c.start.wall_time, &wd);
  printf("%lu %lu,%lu %lu,", 
    c.start.cpu_time.tv_sec,
    c.start.cpu_time.tv_usec,
    cd.tv_sec,
    cd.tv_usec
  );
  printf("%lu %lu,%lu %lu\n", 
    c.start.wall_time.tv_sec,
    c.start.wall_time.tv_usec,
    wd.tv_sec,
    wd.tv_usec
  );
}

void profile_functions_csv(const char* program){
  time_functions(program, format_csv);
}
