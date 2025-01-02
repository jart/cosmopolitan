#ifndef COSMOPOLITAN_MAPS_H_
#define COSMOPOLITAN_MAPS_H_
#include "libc/intrin/atomic.h"
#include "libc/intrin/tree.h"
#include "libc/runtime/runtime.h"
COSMOPOLITAN_C_START_

/* size of dynamic memory that is used internally by your memory manager */
#define MAPS_SIZE 65536

/* when map->hand is MAPS_RESERVATION it means mmap() is transactionally
   reserving address space it is in the process of requesting from win32 */
#define MAPS_RESERVATION -2

/* when map->hand is MAPS_SUBREGION it means that an allocation has been
   broken into multiple fragments by mprotect(). the first fragment must
   be set to MAPS_VIRTUAL or your CreateFileMapping() handle. your frags
   must be perfectly contiguous in memory and should have the same flags */
#define MAPS_SUBREGION -3

/* indicates an allocation was created by VirtualAlloc() and so munmap()
   must call VirtualFree() when destroying it. use it on the hand field. */
#define MAPS_VIRTUAL -4

/* if this is used on MAP_PRIVATE memory, then it's assumed to be memory
   that win32 allocated, e.g. a CreateThread() stack. if this is used on
   MAP_FILE memory, then it's assumed to be part of the executable image */
#define MAP_NOFORK 0x10000000

#define MAP_TREE_CONTAINER(e) TREE_CONTAINER(struct Map, tree, e)

struct Map {
  char *addr;        /* granule aligned */
  size_t size;       /* must be nonzero */
  int64_t off;       /* ignore for anon */
  int flags;         /* memory map flag */
  short prot;        /* memory protects */
  bool iscow;        /* windows nt only */
  bool readonlyfile; /* windows nt only */
  unsigned visited;  /* checks and fork */
  intptr_t hand;     /* windows nt only */
  union {
    struct Tree tree;
    struct Map *freed;
  };
};

struct MapLock {
  void *edges;
  _Atomic(uint64_t) word;
};

struct MapSlab {
  struct MapSlab *next;
  struct Map maps[(MAPS_SIZE - sizeof(struct MapSlab *)) / sizeof(struct Map)];
};

struct Maps {
  uint128_t rand;
  struct Tree *maps;
  struct MapLock lock;
  _Atomic(uintptr_t) freed;
  _Atomic(struct MapSlab *) slabs;
  size_t count;
  size_t pages;
  struct Map stack;
  struct Map guard;
#ifdef MODE_DBG
  struct Map spool[1];
#else
  struct Map spool[20];
#endif
};

struct AddrSize {
  char *addr;
  size_t size;
};

extern struct Maps __maps;

bool __maps_held(void);
void __maps_init(void);
void __maps_lock(void);
void __maps_check(void);
void __maps_unlock(void);
bool __maps_reentrant(void);
void *__maps_randaddr(void);
void __maps_add(struct Map *);
void __maps_free(struct Map *);
void __maps_insert(struct Map *);
int __maps_untrack(char *, size_t);
bool __maps_track(char *, size_t, int, int);
struct Map *__maps_alloc(void);
struct Map *__maps_floor(const char *);
void __maps_stack(char *, int, int, size_t, int, intptr_t);
int __maps_compare(const struct Tree *, const struct Tree *);
struct AddrSize __get_main_stack(void);

forceinline optimizespeed int __maps_search(const void *key,
                                            const struct Tree *node) {
  const char *addr = (const char *)key;
  const struct Map *map = (const struct Map *)MAP_TREE_CONTAINER(node);
  return (addr > map->addr) - (addr < map->addr);
}

static inline struct Map *__maps_next(struct Map *map) {
  struct Tree *node;
  if ((node = tree_next(&map->tree)))
    return MAP_TREE_CONTAINER(node);
  return 0;
}

static inline struct Map *__maps_prev(struct Map *map) {
  struct Tree *node;
  if ((node = tree_prev(&map->tree)))
    return MAP_TREE_CONTAINER(node);
  return 0;
}

static inline struct Map *__maps_first(void) {
  struct Tree *node;
  if ((node = tree_first(__maps.maps)))
    return MAP_TREE_CONTAINER(node);
  return 0;
}

static inline struct Map *__maps_last(void) {
  struct Tree *node;
  if ((node = tree_last(__maps.maps)))
    return MAP_TREE_CONTAINER(node);
  return 0;
}

static inline bool __maps_isalloc(struct Map *map) {
  return map->hand != MAPS_SUBREGION;
}

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_MAPS_H_ */
