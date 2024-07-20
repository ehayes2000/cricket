#ifndef H_SRCMAP
#define MAX_FUNCTIONS 1000

#include <elfutils/libdw.h>
#include <dwarf.h>
#include "instn_table.h"

/*
  map map breakpoint offsets to source code
*/

InstnTable* map_functions(const char* elf_file);
void offset_validator();

#endif