#ifndef H_HASHMAP
#include <stddef.h>
#include <stdbool.h>

struct _TableEntry;
typedef struct _TableEntry _TableEntry;

struct _TableEntry { 
  void* key;
  void* value;
  _TableEntry *chain;
};


typedef enum KeyKind { 
  STRING,
  INT
} KeyKind;

struct HashMap;
typedef struct HashMap HashMap;

struct HashMap { 
  _TableEntry* buckets;
  KeyKind keykind;
  size_t len; 
  const size_t cap;
};

HashMap* new_map(size_t cap, KeyKind k);
void map_insert(HashMap* map, void *key, void* value);
void* map_get(HashMap* map, void* key);
bool map_delete(HashMap* map, void* key);

#endif