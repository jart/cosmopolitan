// This is an implementation of the open-addressing hash table.

#include "libc/nexgen32e/crc32.h"
#include "third_party/chibicc/chibicc.h"

#define INIT_SIZE      16            // initial hash bucket size
#define LOW_WATERMARK  20            // keep usage below 50% after rehashing
#define HIGH_WATERMARK 40            // perform rehash when usage exceeds 70%
#define TOMBSTONE      ((void *)-1)  // represents deleted hash table entry

long chibicc_hashmap_hits;
long chibicc_hashmap_miss;

static inline uint64_t fnv_hash(char *s, int len) {
  return crc32c(0, s, len);
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
  for (int i = 0; i < map->capacity; i++) {
    if (map->buckets[i].key && map->buckets[i].key != TOMBSTONE) {
      nkeys++;
    }
  }
  int cap = MAX(8, map->capacity);
  while ((nkeys * 100) / cap >= LOW_WATERMARK) cap = cap * 2;
  assert(cap > 0);
  // Create a new hashmap and copy all key-values.
  HashMap map2 = {};
  map2.buckets = calloc(cap, sizeof(HashEntry));
  map2.capacity = cap;
  for (int i = 0; i < map->capacity; i++) {
    HashEntry *ent = &map->buckets[i];
    if (ent->key && ent->key != TOMBSTONE) {
      hashmap_put2(&map2, ent->key, ent->keylen, ent->val);
    }
  }
  assert(map2.used == nkeys);
  *map = map2;
}

static bool match(HashEntry *ent, char *key, int keylen) {
  if (ent->key && ent->key != TOMBSTONE) {
    if (ent->keylen == keylen && !memcmp(ent->key, key, keylen)) {
      ++chibicc_hashmap_hits;
      return true;
    } else {
      ++chibicc_hashmap_miss;
      return false;
    }
  } else {
    return false;
  }
}

static HashEntry *get_entry(HashMap *map, char *key, int keylen) {
  if (!map->buckets) return NULL;
  uint64_t hash = fnv_hash(key, keylen);
  for (int i = 0; i < map->capacity; i++) {
    HashEntry *ent = &map->buckets[(hash + i) & (map->capacity - 1)];
    if (match(ent, key, keylen)) return ent;
    if (ent->key == NULL) return NULL;
  }
  UNREACHABLE();
}

static HashEntry *get_or_insert_entry(HashMap *map, char *key, int keylen) {
  if (!map->buckets) {
    map->buckets = calloc(INIT_SIZE, sizeof(HashEntry));
    map->capacity = INIT_SIZE;
  } else if ((map->used * 100) / map->capacity >= HIGH_WATERMARK) {
    rehash(map);
  }
  uint64_t hash = fnv_hash(key, keylen);
  for (int i = 0; i < map->capacity; i++) {
    HashEntry *ent = &map->buckets[(hash + i) & (map->capacity - 1)];
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
