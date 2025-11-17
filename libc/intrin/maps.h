#ifndef COSMOPOLITAN_MAPS_H_
#define COSMOPOLITAN_MAPS_H_
#include "libc/assert.h"
#include "libc/dce.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/tree.h"
#include "libc/runtime/runtime.h"
#include "libc/thread/thread.h"
COSMOPOLITAN_C_START_

/* please be warned this will make mmap O(n) */
#define MMAP_DEBUG 0

/* size of dynamic memory that is used internally by your memory manager */
#define MAPS_SLAB_SIZE 65536

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

/* if true, mmap(0) will auto-assign sparse addresses which helps mremap */
#define MMAP_IS_SPARSE (IsXnu() || IsFreebsd() || IsOpenbsd() || IsWindows())

#define MAP_TREE_CONTAINER(e) TREE_CONTAINER(struct Map, tree, e)

#if !MMAP_DEBUG
#define MAPS_ASSERT(x) ((void)0)
#else
#define MAPS_ASSERT(x) \
  ((void)((x) || (__maps_assert_fail(#x, __FILE__, __LINE__), 0)))
#endif

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

struct MapSlab {
  struct MapSlab *next;
  atomic_size_t used;
};

struct MapPageTable {
  atomic_uint_fast64_t p[8];
};

struct MapPageDirectory {
  union {
    _Atomic(struct MapPageTable *) pt;
    _Atomic(struct MapPageDirectory *) pd;
  } p[512];
};

struct Maps {
  union {
    uint128_t rand;
    uint64_t randlo;
  };
  struct Tree *maps;
  char *balloc;
  bool32 once;
  _Atomic(char *) bolloc;
  _Atomic(struct Map *) freed;
  _Atomic(struct MapSlab *) slabs;
  _Atomic(struct MapPageDirectory *) alive;
  size_t count;
  size_t pages;
  struct Map stack;
  struct Map guard;
  union {
    struct MapSlab slab1[1];
    _Alignas(void *) char slab1memory[MAPS_SLAB_SIZE];
  };
};

struct AddrSize {
  char *addr;
  size_t size;
};

extern struct Maps __maps;

void __maps_init(void);
void __maps_wipe(void);
void __maps_lock(void);
void __maps_check(void);
void __maps_unlock(void);
void *__maps_randaddr(void);
void __maps_add(struct Map *);
void __maps_free(struct Map *);
void __maps_insert(struct Map *);
int __maps_untrack(char *, size_t);
struct Map *__maps_alloc(void);
struct Map *__maps_floor(const char *);
bool __maps_track(char *, size_t, int, int);
void __maps_stack(char *, int, int, size_t, int);
int __maps_compare(const struct Tree *, const struct Tree *);
void __maps_assert_fail(const char *, const char *, int) wontreturn;
struct AddrSize __get_main_stack(void);
void __maps_unmark(void *, size_t);
void __maps_mark(void *, size_t);
uint64_t __maps_hash(uint64_t);
void *__maps_randaddr(void);
void *__maps_balloc(size_t);
uint64_t __maps_rand(void);

#if MMAP_DEBUG
#define __maps_held()                                 \
  ({                                                  \
    int result;                                       \
    if (pthread_mutex_held_np(&__maps.lock, &result)) \
      __builtin_trap();                               \
    result;                                           \
  })
#else
#define __maps_check() (void)0
#endif

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
