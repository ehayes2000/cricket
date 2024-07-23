#ifndef H_TIMESTACK
#define H_TIMESTACK

#include <sys/time.h>
#include <stdbool.h>

#define BUF_SIZE 128
typedef struct {
  struct timeval cpu_time;
  struct timeval wall_time;
} Instant;

typedef struct {
  unsigned int cap;
  unsigned int size;
  Instant *buf
} TimeStack;

TimeStack* newTimeStack();
void pushTimeStack(TimeStack*, Instant);
Instant popTimeStack(TimeStack*);
Instant topTimeSTack(TimeStack*);
bool emptyTimeStack(TimeStack*);
void freeTimeStack(TimeStack*);
#endif