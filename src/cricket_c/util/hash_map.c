#include <stddef.h>
#include <stdbool.h>
#include <util/hash_map.h>
#include <string.h>

unsigned long hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;
    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash;
}

bool intcmp(void* a, void* b){
  return *(int*)a == *(int*)b;
}

void _handle_insert_collision(HashMap* map, _TableEntry* e, bool (*cmp)(void* a, void*b),
    void* key, void* value){
  _TableEntry* insert;
  while (1) { 
    if (cmp(e->key, key)){  // key already exists -> overwrite 
      insert = e;
      break;
    }
    else if (e->chain == NULL) { // end of chain add another one
      insert = calloc(1, sizeof(_TableEntry));
      e->chain = insert;
      break;
    }
    e = e->chain;
  }
  insert->key = key; 
  insert->value = value;
}

#ifndef MAKE_IDX_CMP
#define MAKE_IDX_CMP(map, key)          \
  size_t index;                         \
  bool (*cmp)(void*, void*);            \
  if (map->keykind == STRING) {         \
    cmp = strcmp;                       \
    index = hash(key) % map->cap;       \
  } else {                              \
    cmp = intcmp;                       \
    index = *(size_t*)key % map->cap;   \
  }                                     
#endif

void map_insert(HashMap* map, void *key, void* value){ 
  MAKE_IDX_CMP(map, key)
  if (map->buckets[index].value != NULL){ // no collision :)
    map->buckets[index].value = value;
    map->buckets[index].key = key;
    map->len ++;
    return true;
  } else { // :(
    _handle_insert_collision(map, &map->buckets[index], cmp, key, value);
  } 
}

void* _handle_get_collision(HashMap* map, _TableEntry* e, bool (*cmp)(void* a, void*b),
    void* key){
  while (e != NULL) {
    if (cmp(e->key, key))
      return e->value;
    e = e->chain;
  }
  return NULL;
} 

void* map_get(HashMap* map, void* key) { 
  MAKE_IDX_CMP(map, key)
  if (map->buckets[index].value == NULL){
    return NULL;
  }
  if (cmp(map->buckets[index].key, key)){
    return map->buckets[index].value;
  }
  return _handle_get_collision(map, &map->buckets[index], cmp, key);
}

bool map_delete(HashMap* map, void* key){
  MAKE_IDX_CMP(map, key)
  if (map->buckets[index].value == NULL){
    return false;
  }
  _TableEntry *e = &map->buckets[index];
  while (e != NULL) { 
    if (cmp(e->key, key)){
      e->key = NULL; // :) we leak a bit of memory
      e->value = NULL;
      map->len --;
      return true;
    }
    e = e->chain;
  }
  return false;
}

HashMap* new_map(size_t cap, KeyKind t){ 
  _TableEntry *buckets = calloc(cap, sizeof(_TableEntry));
  HashMap* map = calloc(1, sizeof(HashMap));
  map->buckets = buckets;
  map->keykind = t;
  map->len = 0;
  return map;
}