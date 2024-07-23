#include "time_stack.h"
#include <stdio.h>
#include <stdlib.h>

TimeStack* newTimeStack(){
  TimeStack* s = malloc(sizeof(TimeStack));
  *s = (TimeStack){
    .buf = malloc(sizeof(struct timeval) * BUF_SIZE),
    .cap = BUF_SIZE,
    .size = 0,
  };
  return s;
}

void pushTimeStack(TimeStack* s, Instant i){
  if (s->size >= s->cap){
    s->cap *= 2;
    struct timeval* new_buf = malloc(s->cap * sizeof(Instant));
    memcpy(new_buf, s->buf, s->size * sizeof(Instant));
    free(s->buf);
    s->buf = new_buf;
  }
  s->buf[s->size++] = i;
}

Instant popTimeStack(TimeStack* s){  
  return s->buf[--s->size];
}

Instant topTimeSTack(TimeStack *s){
  return s->buf[s->size - 1];
}

bool emptyTimeStack(TimeStack *s){
  return s->size == 0;
}

void freeTimeStack(TimeStack *s){
  free(s->buf);
  free(s);
}