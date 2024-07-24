#ifndef H_PROFILER
#define H_PROFILER

#include "src_map/src_map.h"
#include "time_stack/time_stack.h"

typedef struct { 
  SourceInfo info;
  Instant start;
  Instant end;
} FunctionCall;

FunctionCall* time_functions(const char *program, unsigned long *n_calls);

#endif