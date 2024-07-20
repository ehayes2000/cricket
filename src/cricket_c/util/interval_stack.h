#ifndef H_INTERVALSTACK
#define H_INTERVALSTACK

#include <sys/time.h>
#include <stdbool.h>

#define BUF_SIZE 128

typedef struct timeval Interval;

typedef struct {
  unsigned int cap;
  unsigned int size;
  Interval *buf;
} IntervalStack;

IntervalStack* intervalStackCreate();
void intervalStackPush(IntervalStack*, Interval);
Interval intervalStackPop(IntervalStack*);
Interval intervalStackTop(IntervalStack*);
bool intervalStackEmpty(IntervalStack*);
void intervalStackFree(IntervalStack*);
#endif