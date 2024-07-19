#include "break_table.h"

int mark_time(Interval* i, struct timeval t, size_t pc) {
  if (i->lowpc == pc){
    i->low_time = t;
    return 0;
  }
  else if (i->highpc == pc){
    i->high_time= t;
    return 0;
  } 
  return -1;
}

instruction get_instruction(Interval* i, size_t pc){  
  if (i->highpc == pc){
    return i->high_instn;
  }
  else if (i->lowpc == pc){
    return i->low_instn;
  }
  return 0; // ???
}