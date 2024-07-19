#ifndef H_BREAKTABLE
#define H_BREAKTABLE

#include <sys/time.h>
#include "tracee.h"

/*
  Time execution between two points
*/

typedef struct Interval {
  size_t lowpc;
  size_t highpc;
  instruction low_instn;
  instruction high_instn;
  struct timeval low_time;
  struct timeval high_time;
} Interval;

int mark_time(Interval*, struct timeval, size_t pc);
instruction get_instruction(Interval*, size_t pc);

#endif
