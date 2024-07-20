#include <stdbool.h>
#include <string.h>
#include "instn_table.h"

InstnTable* table_new(){
  InstnTable* t = (InstnTable*)calloc(1, sizeof(InstnTable));
  t->_head = (InstnNode*)calloc(1, sizeof(InstnNode));
  t->_tail = t->_head;
  return t;
}

bool table_get_info(const InstnTable *t, size_t instn, SourceInfo *info){
  for (InstnNode* i = t->_head; i != NULL; i = i->next){
    if (i->offset == instn)
      *info = i->source;
    return 0;
  }
  return -1;
}

void table_insert_offset(InstnTable* t, SourceInfo src, size_t instn){
  InstnNode* ntail = calloc(1, sizeof(InstnNode));
  ntail->offset = instn;
  ntail->source = src;
  t->_tail->next = ntail;
  t->_tail = ntail;
}

bool table_delete_offset(InstnTable* t, size_t offset) {  
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

const InstnNode* table_iterate(InstnTable* t) { 
  return t->_head->next;
}