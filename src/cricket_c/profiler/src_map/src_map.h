#ifndef H_SRCMAP
#define H_SRCMAP

#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>

typedef struct { 
  char* fn_name;
  char* src_file;
  unsigned long line;
  long instn; // data replaced
  bool is_exit;
  int hits;
} SourceInfo;

typedef struct InstnNode InstnNode;

struct InstnNode { 
  size_t offset; // offset [key]
  SourceInfo source; // source info [value]
  InstnNode *next;
}; 

typedef struct SrcMap { 
  InstnNode* _head;
  InstnNode* _tail;
} SrcMap;

SrcMap* newSrcMap(const char* elf);
bool srcMapGetInfo(const SrcMap*, const size_t offset, SourceInfo*);
void srcMapInsertInfo(SrcMap*, SourceInfo, size_t);
bool srcMapDeleteOffset(SrcMap*, size_t);
const InstnNode* srcMapIter(const SrcMap*);

#endif