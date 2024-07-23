#include "src_map.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>
#include <elfutils/libdw.h>
#include <dwarf.h>

SrcMap* allocSrcMap(){
  SrcMap* t = (SrcMap*)calloc(1, sizeof(SrcMap));
  t->_head = (InstnNode*)calloc(1, sizeof(InstnNode));
  t->_tail = t->_head;
  return t;
}

bool die_is_func(Dwarf_Die *dwarf_die){
  if (dwarf_die == NULL)
    return false;
  return dwarf_tag(dwarf_die) == DW_TAG_subprogram;
}

Dwarf_Word get_die_line_number(Dwarf_Die *die) {
    Dwarf_Attribute attr;
    Dwarf_Word line_number = 0;

    if (dwarf_attr(die, DW_AT_decl_line, &attr) != NULL) {
        if (dwarf_formudata(&attr, &line_number) == 0) {
            return line_number;
        }
    }
    return 0;  // Return 0 if line number not found
}

void map_dwarf_fn(SrcMap* t, const char* source_file, Dwarf_Die *dwarf_die){
  size_t pc;
  if (dwarf_lowpc(dwarf_die, &pc) == -1){ // cowabunga
    return;
  }

  SourceInfo i = {
    .fn_name = dwarf_diename(dwarf_die),
    .src_file = source_file,
    .line = get_die_line_number(dwarf_die),
    .is_exit = false,
  };
  if (strncmp(i.fn_name, "main", 4) == 0){
    return;
  }
  srcMapInsertInfo(t, i, pc); // lowpc
  if (dwarf_highpc(dwarf_die, &pc) == -1){ // cowabunga
    return;
  }
  i.is_exit = true;
  // highpc is exclusive
  srcMapInsertInfo(t, i, pc - 4); // highpc
}

void walk_tree(SrcMap *t, Dwarf_Die *dwarf_die, const char* src_file){ 
  Dwarf_Die die = *dwarf_die;
  do
    {
      Dwarf_Die child;
      if (die_is_func(&die)){
        map_dwarf_fn(t, src_file, &die);
      }
      if (dwarf_child (&die, &child) == 0)
	      walk_tree(t, &child, src_file);
    }
  while (dwarf_siblingof (&die, &die) == 0);
}

const char* get_cu_source_file(Dwarf_Die *cu_die) {
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

SrcMap* newSrcMap(const char* elf_file) { 
  int fd = open(elf_file, O_RDONLY);
  if (fd == -1){
    fprintf(stderr, "failed to open file: %s\n", strerror(errno));
    exit(errno);
  }
  Dwarf *dbg = dwarf_begin(fd, DWARF_C_READ);
  if (dbg == NULL){
    perror("could not parse DWARF");
    exit(errno);
  }
  Dwarf_CU *cu = NULL;
  Dwarf_Die cudie, subdie;
  uint8_t unit_type;
  SrcMap *t = allocSrcMap();
  const char* file_name;
  while(dwarf_get_units(dbg, cu, &cu, NULL,
    &unit_type, &cudie, &subdie) == 0){
      Dwarf_Die die = (unit_type == DW_UT_skeleton ? subdie : cudie);
      file_name = get_cu_source_file(&cudie);
      walk_tree(t, &die, file_name);
  }
  return t; 
}