#include "src_map.h"


bool srcMapGetInfo(const SrcMap* t, const size_t offset, SourceInfo* info){
  for (InstnNode* i = t->_head->next; i != NULL; i = i->next){
    if (i->offset == offset){
      *info = i->source;
      return true;
    }
  }
  return false;  
}

void srcMapInsertInfo(SrcMap* t, SourceInfo src, size_t offset){
  InstnNode* ntail = calloc(1, sizeof(InstnNode));
  ntail->offset = offset;
  ntail->source = src;
  t->_tail->next = ntail;
  t->_tail = ntail;
}

bool srcMapDeleteOffset(SrcMap* t, size_t offset){
  InstnNode* p = t->_head;
  for(InstnNode* i = t->_head->next; i != NULL; i = i->next, p = p->next){
    if (i->offset == offset){
      p->next = i->next;
      free(i);
      return true;
    }
  }
  return false;
}
const InstnNode* srcMapIter(const SrcMap* t){
  return t->_head->next;
}
