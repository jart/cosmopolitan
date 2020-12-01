// This is an implementation of the open-addressing hash table.

#include "third_party/chibicc/chibicc.h"

#define TOMBSTONE ((void *)-1)  // Represents a deleted hash entry

static uint64_t fnv_hash(char *s, int len) {
  uint64_t hash = 0xcbf29ce484222325;
  for (int i = 0; i < len; i++) {
    hash *= 0x100000001b3;
    hash ^= (unsigned char)s[i];
  }
  return hash;
}

// Make room for new entires in a given hashmap by removing
// tombstones and possibly extending the bucket size.
static void rehash(HashMap *map) {
  // Compute the size of the new hashmap.
  int nkeys = 0;
  for (int i = 0; i < map->capacity; i++)
    if (map->buckets[i].key && map->buckets[i].key != TOMBSTONE) nkeys++;
  int cap = map->capacity;
  while ((nkeys * 100) / cap >= 50) cap = cap * 2;
  // Create a new hashmap and copy all key-values.
  HashMap map2 = {};
  map2.buckets = calloc(cap, sizeof(HashEntry));
  map2.capacity = cap;
  for (int i = 0; i < map->capacity; i++) {
    HashEntry *ent = &map->buckets[i];
    if (ent->key && ent->key != TOMBSTONE)
      hashmap_put2(&map2, ent->key, ent->keylen, ent->val);
  }
  assert(map2.used == nkeys);
  *map = map2;
}

static bool match(HashEntry *ent, char *key, int keylen) {
  return ent->key && ent->key != TOMBSTONE && ent->keylen == keylen &&
         memcmp(ent->key, key, keylen) == 0;
}

static HashEntry *get_entry(HashMap *map, char *key, int keylen) {
  if (!map->buckets) return NULL;
  uint64_t hash = fnv_hash(key, keylen);
  for (int i = 0; i < map->capacity; i++) {
    HashEntry *ent = &map->buckets[(hash + i) % map->capacity];
    if (match(ent, key, keylen)) return ent;
    if (ent->key == NULL) return NULL;
  }
  UNREACHABLE();
}

static HashEntry *get_or_insert_entry(HashMap *map, char *key, int keylen) {
  if (!map->buckets) {
    map->buckets = calloc((map->capacity = 16), sizeof(HashEntry));
  }
  if ((map->used * 100) / map->capacity >= 70) rehash(map);
  uint64_t hash = fnv_hash(key, keylen);
  for (int i = 0; i < map->capacity; i++) {
    HashEntry *ent = &map->buckets[(hash + i) % map->capacity];
    if (match(ent, key, keylen)) return ent;
    if (ent->key == TOMBSTONE) {
      ent->key = key;
      ent->keylen = keylen;
      return ent;
    }
    if (ent->key == NULL) {
      ent->key = key;
      ent->keylen = keylen;
      map->used++;
      return ent;
    }
  }
  UNREACHABLE();
}

void *hashmap_get(HashMap *map, char *key) {
  return hashmap_get2(map, key, strlen(key));
}

void *hashmap_get2(HashMap *map, char *key, int keylen) {
  HashEntry *ent = get_entry(map, key, keylen);
  return ent ? ent->val : NULL;
}

void hashmap_put(HashMap *map, char *key, void *val) {
  hashmap_put2(map, key, strlen(key), val);
}

void hashmap_put2(HashMap *map, char *key, int keylen, void *val) {
  HashEntry *ent = get_or_insert_entry(map, key, keylen);
  ent->val = val;
}

void hashmap_delete(HashMap *map, char *key) {
  hashmap_delete2(map, key, strlen(key));
}

void hashmap_delete2(HashMap *map, char *key, int keylen) {
  HashEntry *ent = get_entry(map, key, keylen);
  if (ent) ent->key = TOMBSTONE;
}

void hashmap_test(void) {
  HashMap *map = calloc(1, sizeof(HashMap));
  for (int i = 0; i < 5000; i++)
    hashmap_put(map, format("key %d", i), (void *)(size_t)i);
  for (int i = 1000; i < 2000; i++) hashmap_delete(map, format("key %d", i));
  for (int i = 1500; i < 1600; i++)
    hashmap_put(map, format("key %d", i), (void *)(size_t)i);
  for (int i = 6000; i < 7000; i++)
    hashmap_put(map, format("key %d", i), (void *)(size_t)i);
  for (int i = 0; i < 1000; i++)
    assert((size_t)hashmap_get(map, format("key %d", i)) == i);
  for (int i = 1000; i < 1500; i++)
    assert(hashmap_get(map, "no such key") == NULL);
  for (int i = 1500; i < 1600; i++)
    assert((size_t)hashmap_get(map, format("key %d", i)) == i);
  for (int i = 1600; i < 2000; i++)
    assert(hashmap_get(map, "no such key") == NULL);
  for (int i = 2000; i < 5000; i++)
    assert((size_t)hashmap_get(map, format("key %d", i)) == i);
  for (int i = 5000; i < 6000; i++)
    assert(hashmap_get(map, "no such key") == NULL);
  for (int i = 6000; i < 7000; i++)
    hashmap_put(map, format("key %d", i), (void *)(size_t)i);
  assert(hashmap_get(map, "no such key") == NULL);
  printf("OK\n");
}
