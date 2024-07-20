#ifndef H_INSTNTABLE
#define H_INSTNTABLE

#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include "../ptrace_util/tracee.h"

typedef struct { 
  char* fn_name;
  char* src_file;
  unsigned long line;
  instruction instn; // data replaced
  bool is_exit;
} SourceInfo;

typedef struct InstnNode InstnNode;

struct InstnNode { 
  size_t offset; // offset
  SourceInfo source;
  InstnNode *next;
}; 

typedef struct InstnTable { 
  InstnNode* _head;
  InstnNode* _tail;
} InstnTable;

InstnTable* table_new();
// get the first occurance of a function by name
bool table_get_info(const InstnTable*, size_t, SourceInfo*);
// insert source info referenced by line number
void table_insert_offset(InstnTable*, SourceInfo, size_t);
bool table_delete_offset(InstnTable*, size_t);
// return the first non-sentinal node 
const InstnNode* table_iterate(InstnTable*);

#endif