#include <stdbool.h>
#include <string.h>
#include "instn_table.h"

InstnTable* table_new(){
  InstnTable* t = (InstnTable*)calloc(1, sizeof(InstnTable));
  t->_head = (InstnNode*)calloc(1, sizeof(InstnNode));
  t->_tail = t->_head;
  t->_head->info.kind = SENTINAL;
  return t;
}

InstnNode* table_get_fn(const InstnTable* t, const char* fn_name){ 
  for (InstnNode *i = table_iterate(t); i != NULL; i = i->next){
    if (strcmp(i->info.source.function->name, fn_name) == 0){
      return i;
    }
  }
  return NULL;
}

SourceInfo table_get_info(const InstnTable *t, size_t instn){
  for (InstnNode* i = t->_head; i != NULL; i = i->next){
    if (i->instn == instn)
      return i->info;
  }
  SourceInfo blank = { .kind=SENTINAL };
  return blank;
}

void table_insert_info(InstnTable* t, SourceInfo src, size_t instn){
  InstnNode* ntail = calloc(1, sizeof(InstnNode));
  ntail->instn = instn;
  ntail->info = src;
  t->_tail->next = ntail;
  t->_tail = ntail;
}

bool table_delete_instn(InstnTable* t, size_t instn){
  InstnNode* p = t->_head;
  for (InstnNode* i = t->_head->next; i != NULL; i = i->next, p = p->next){
    if (i->instn == instn){
      p->next = i->next;
      if (i == t->_tail)
        t->_tail = p;
      switch (i->info.kind){
        case FUNCTION: 
          free(i->info.source.function);
          break;
        default: 
          break;
      }
      free(i);
      return true;
    }
  }
  return false;
}

const InstnNode* table_iterate(InstnTable* t) { 
  return t->_head->next;
}