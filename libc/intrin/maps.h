#ifndef COSMOPOLITAN_LIBC_RUNTIME_MAPS_H_
#define COSMOPOLITAN_LIBC_RUNTIME_MAPS_H_
#include "libc/intrin/atomic.h"
#include "libc/intrin/dll.h"
#include "libc/thread/tls2.internal.h"
COSMOPOLITAN_C_START_

#define MAP_CONTAINER(e) DLL_CONTAINER(struct Map, elem, e)

struct Map {
  _Atomic(struct Map *) next; /* for __maps.maps */
  char *addr;                 /* granule aligned */
  size_t size;                /* must be nonzero */
  struct Dll elem;            /* for __maps.free */
  int64_t off;                /* -1 if anonymous */
  int prot;                   /* memory protects */
  int flags;                  /* memory map flag */
  bool iscow;                 /* windows nt only */
  bool readonlyfile;          /* windows nt only */
  intptr_t h;                 /* windows nt only */
};

struct Maps {
  atomic_int lock;
  _Atomic(struct Map *) maps;
  struct Dll *free;
  struct Map stack;
  struct Dll *used;
};

struct AddrSize {
  char *addr;
  size_t size;
};

extern struct Maps __maps;

int maps_check(void);
void __maps_init(void);
void __maps_lock(void);
void __maps_unlock(void);
struct Map *__maps_alloc(void);
void __maps_free(struct Map *);
void __maps_insert(struct Map *);
void *__mmap(char *, size_t, int, int, int, int64_t);
struct AddrSize __get_main_stack(void);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_RUNTIME_MAPS_H_ */
