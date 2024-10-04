#ifndef H_PROFILER
#define H_PROFILER

#include "src_map/src_map.h"
#include "time_stack/time_stack.h"

typedef struct { 
  Instant start;
  Instant end;
  SourceInfo src;
  bool is_exit;
} Call;

void time_functions(const char *program, void(*call_handler)(Call c));

#endif