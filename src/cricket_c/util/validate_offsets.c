#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../ptrace_util/tracee.h"
#include "src_map.h"


void validate_child_process() { 
  write(STDOUT_FILENO, "hello world\n", 13);
  while(1){
    write(STDOUT_FILENO, "tick\n", 6);
    sleep(3);
  }
}

void offset_validator(){
  printf("offset validator started\n");
  const char* this_bin = "bin/cricket";
  InstnTable *offsets = map_functions(this_bin); 
  size_t child_offset;
  for (InstnNode* i = table_iterate(offsets); i != NULL; i = i->next){
    if (strcmp(i->source.fn_name, "validate_child_process") == 0){
      printf("found child offset\n");
      printf("fn: %s\n", i->source.fn_name);
      printf("offset: %p\n", (void*)i->offset);
      child_offset = i->offset;
      break;
    }
  }
  printf("\n\nspawning child\n\n");
  pid_t child = fork();  
  if (child == 0){ 
    validate_child_process();
  } else { 
    size_t base_address = get_process_base_address(child);
    printf("child base address %p\n", (void*)base_address);
    printf("child fn virtual address %p\n", (void*)(base_address + child_offset));
    printf("child spawned: %d\n", child);
    while(1){};
    printf("how did we get here?\n");
  }
}

