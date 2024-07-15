#ifndef H_SRCMAP

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

FileNode* map_functions(const char* elf_file);
#endif