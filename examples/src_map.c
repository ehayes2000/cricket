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

typedef struct Userfn {
  char * name;
  uint64_t start; 
  uint64_t end;
} UserFn;

struct FnNode;
typedef struct FnNode FnNode;
struct FnNode { 
  UserFn fn;
  FnNode *next;
};

struct FileNode; 
typedef struct FileNode FileNode;

struct FileNode { 
  Dwarf_CU *file;
  int n_functions;
  FnNode *functions;
  FileNode *next;
};



bool die_is_func(Dwarf_Die *dwarf_die){
  if (dwarf_die == NULL)
    return false;
  return dwarf_tag(dwarf_die) == DW_TAG_subprogram;
}

FnNode* _make_node_die(Dwarf_Die *dwarf_die){
  FnNode* node = calloc(1, sizeof(FnNode));
  node->fn.name = dwarf_diename(dwarf_die);
  if (dwarf_lowpc(dwarf_die, &node->fn.start) == -1){ // cowabunga
    fprintf(stderr, "no lowpc %s\n", strerror(errno));
    exit(errno);
  }
  if (dwarf_highpc(dwarf_die, &node->fn.end) == -1){ // cowabunga
    fprintf(stderr, "no highpc %s\n", strerror(errno));
    exit(errno);
  }
  return node;
}

FnNode* _walk_tree(Dwarf_Die *dwarf_die, int *listLen, FnNode* fn){ 
  Dwarf_Die die = *dwarf_die;
  do
    {
      Dwarf_Die child;
      if (die_is_func(&die)){
        fn->next = _make_node_die(&die);
        fn = fn->next;
        *listLen += 1;
      }
      if (dwarf_child (&die, &child) == 0)
	      fn = _walk_tree(&child, listLen, fn);
    }
  while (dwarf_siblingof (&die, &die) == 0);
  return fn; // return last node
}

FileNode* map_functions(const char* elf_file) { 
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
    fprintf(stderr, "could not parse DWARF: %s", strerror(errno));
    exit(errno);
  }
  Dwarf_CU *cu = NULL;
  Dwarf_Die cudie, subdie;
  uint8_t unit_type;
  FileNode *head = NULL;
  while(dwarf_get_units(dbg, cu, &cu, NULL,
    &unit_type, &cudie, &subdie) == 0){
      FileNode *tail = calloc(1, sizeof(FileNode));
      int n_functions = 0;
      if (head == NULL) 
        head = tail;
      Dwarf_Die die = (unit_type == DW_UT_skeleton ? subdie : cudie);
      FnNode *fnhead = calloc(1, sizeof(FnNode));
      _walk_tree(&die, &n_functions, fnhead);
      tail->functions = fnhead->next;
      tail->n_functions = n_functions;
      free(fnhead);
  }
  
  FnNode* n = head->functions;
  printf(":)");
  for (int i = 0; i < head->n_functions; i ++, n=n->next) { 
    printf("%s %0lx, %0lx\n", n->fn.name, n->fn.start, n->fn.end);
  }
  return head; 
}

int main(){ 
  // const char *bin = "c_timecalls";
  // int fd = open(bin, O_RDONLY);
  // Dwarf *dbg = dwarf_begin(fd, DWARF_C_READ);
  map_functions("c_timecalls");
  return 0;
}