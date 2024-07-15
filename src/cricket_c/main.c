#include <stdio.h>
#include <seccomp.h>
#include <elfutils/libdw.h>
#include "util/src_map.h"

int main(int argc, char* argv[]){
  const char* f = "examples/c_system";
  FileNode* file = map_functions(f);


  for (FnNode* i = file->functions; i != NULL; i = i->next){
    printf("%s\n  def: %0x\n  end: %0x\n", i->fn.name, i->fn.start, i->fn.end);
  }
}