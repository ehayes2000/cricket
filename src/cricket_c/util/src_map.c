/*
A collection of utilities build on libdw to map intructions to / from source

profiler 
insert breakpoints around function calls

Node* map_functions(char* elfFile );
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>
#include <elfutils/libdw.h>
#include <dwarf.h>
#include "src_map.h"

bool _die_is_func(Dwarf_Die *dwarf_die){
  if (dwarf_die == NULL)
    return false;
  return dwarf_tag(dwarf_die) == DW_TAG_subprogram;
}

Dwarf_Word _get_die_line_number(Dwarf_Die *die) {
    Dwarf_Attribute attr;
    Dwarf_Word line_number = 0;

    if (dwarf_attr(die, DW_AT_decl_line, &attr) != NULL) {
        if (dwarf_formudata(&attr, &line_number) == 0) {
            return line_number;
        }
    }

    return 0;  // Return 0 if line number not found
}

void _map_dwarf_fn(InstnTable* t, const char* source_file, Dwarf_Die *dwarf_die){
  Function *fn = malloc(sizeof(Function));
  fn->name = dwarf_diename(dwarf_die);
  fn->src_file = source_file;
  fn->line = _get_die_line_number(dwarf_die);
  SourceInfo i = { .kind = FUNCTION, 
                   .source = {
                      .function = fn
                    }
                  };

  size_t pc;
  if (dwarf_lowpc(dwarf_die, &pc) == -1){ // cowabunga
    fprintf(stderr, "no lowpc %s\n", strerror(errno));
    exit(errno);
  }
  table_insert_info(t, i, pc); // lowpc
  if (dwarf_highpc(dwarf_die, &pc) == -1){ // cowabunga
    fprintf(stderr, "no high %s\n", strerror(errno));
    exit(errno);
  }
  table_insert_info(t, i, pc); // highpc
}

void _walk_tree(InstnTable *t, Dwarf_Die *dwarf_die, const char* src_file){ 
  Dwarf_Die die = *dwarf_die;
  do
    {
      Dwarf_Die child;
      if (_die_is_func(&die)){
        _map_dwarf_fn(t, src_file, &die);
      }
      if (dwarf_child (&die, &child) == 0)
	      _walk_tree(t, &child, src_file);
    }
  while (dwarf_siblingof (&die, &die) == 0);
}

const char* _get_cu_source_file(Dwarf_Die *cu_die) {
    Dwarf_Attribute attr_mem;
    Dwarf_Attribute *attr;
    attr = dwarf_attr(cu_die, DW_AT_name, &attr_mem);
    if (attr != NULL) {
        const char *filename;
        filename = dwarf_formstring(attr);
        if (filename != NULL){
          return filename;
        }
    }
    return NULL;  // Return NULL if filename not found
}

InstnTable* map_functions(const char* elf_file) { 
  /*
    TODO untested on multifile programs (likely crashes)
  */
  int fd = open(elf_file, O_RDONLY);
  if (fd == -1){
    fprintf(stderr, "failed to open file: %s\n", strerror(errno));
    exit(errno);
  }
  Dwarf *dbg = dwarf_begin(fd, DWARF_C_READ);
  if (dbg == NULL){
    fprintf(stderr, "could not parse DWARF: %s\n", strerror(errno));
    exit(errno);
  }
  Dwarf_CU *cu = NULL;
  Dwarf_Die cudie, subdie;
  uint8_t unit_type;
  InstnTable *t = table_new();
  const char* file_name;
  while(dwarf_get_units(dbg, cu, &cu, NULL,
    &unit_type, &cudie, &subdie) == 0){
      Dwarf_Die die = (unit_type == DW_UT_skeleton ? subdie : cudie);
      file_name = _get_cu_source_file(&cudie);
      _walk_tree(t, &die, file_name);
  }
  return t; 
}
