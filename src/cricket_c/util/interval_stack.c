#include "interval_stack.h"

IntervalStack* intervalStackCreate(){
  IntervalStack* s = malloc(sizeof(IntervalStack));
  *s = (IntervalStack){
    .buf = malloc(sizeof(Interval) * BUF_SIZE),
    .cap = BUF_SIZE,
    .size = 0,
  };
  return s;
}

void intervalStackPush(IntervalStack* s, Interval i){
  if (s->size >= s->cap){
    s->cap *= 2;
    Interval* new_buf = malloc(s->cap * sizeof(Interval));
    memcpy(new_buf, s->buf, s->size * sizeof(Interval));
    free(s->buf);
    s->buf = new_buf;
  }
  s->buf[s->size++] = i;
}

Interval intervalStackPop(IntervalStack* s){  
  return s->buf[--s->size];
}

Interval intervalStackTop(IntervalStack *s){
  return s->buf[s->size - 1];
}

bool intervalStackEmpty(IntervalStack *s){
  return s->size == 0;
}

void intervaltackFree(IntervalStack *s){
  free(s->buf);
  free(s);
}