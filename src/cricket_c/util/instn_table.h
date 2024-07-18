#ifndef H_INSTNTABLE
#define H_INSTNTABLE

#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include "../ptrace_util/tracee.h"

typedef enum { 
  SENTINAL,
  FUNCTION
} SourceKind;

typedef struct Function { 
  char* name;
  char* src_file;
  unsigned long line;
} Function;

typedef struct SourceInfo { 
  union { 
    Function *function;
  } source;
  SourceKind kind;
  instruction word; // data replaced
} SourceInfo;

typedef struct InstnNode InstnNode;

struct InstnNode { 
  size_t instn; // offset
  SourceInfo info;
  InstnNode *next;
}; 

typedef struct InstnTable { 
  InstnNode* _head;
  InstnNode* _tail;
} InstnTable;

InstnTable* table_new();
// get the first occurance of a function by name
InstnNode* table_get_fn(const InstnTable*, const char*); 
// get source info from a pc (size_t)
SourceInfo table_get_info(const InstnTable*, size_t);
// insert source info referenced by line number
void table_insert_info(InstnTable*, SourceInfo, size_t);
bool table_delete_instn(InstnTable*, size_t);
// return the first non-sentinal node 
const InstnNode* table_iterate(InstnTable*);

#endif