/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/describebacktrace.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/directmap.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/maps.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/tree.h"
#include "libc/intrin/weaken.h"
#include "libc/nt/memory.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/sysparam.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/mremap.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/errfuns.h"

#define MMDEBUG   IsModeDbg()
#define MAX_SIZE  0x0ff800000000ul
#define MAX_TRIES 50

#define MAP_FIXED_NOREPLACE_linux 0x100000

#define PGUP(x) (((x) + pagesz - 1) & -pagesz)

#define MASQUE    0x00fffffffffffff8
#define PTR(x)    ((uintptr_t)(x) & MASQUE)
#define TAG(x)    ROL((uintptr_t)(x) & ~MASQUE, 8)
#define ABA(p, t) ((uintptr_t)(p) | (ROR((uintptr_t)(t), 8) & ~MASQUE))
#define ROL(x, n) (((x) << (n)) | ((x) >> (64 - (n))))
#define ROR(x, n) (((x) >> (n)) | ((x) << (64 - (n))))

#if !MMDEBUG
#define ASSERT(x) (void)0
#else
#define ASSERT(x)                                                         \
  do {                                                                    \
    if (!(x)) {                                                           \
      char bt[160];                                                       \
      struct StackFrame *bp = __builtin_frame_address(0);                 \
      kprintf("%!s:%d: assertion failed: %!s\n", __FILE__, __LINE__, #x); \
      kprintf("bt %!s\n", _DescribeBacktrace(bt, bp));                    \
      __print_maps(0);                                                    \
      __builtin_trap();                                                   \
    }                                                                     \
  } while (0)
#endif

int __maps_compare(const struct Tree *ra, const struct Tree *rb) {
  const struct Map *a = (const struct Map *)MAP_TREE_CONTAINER(ra);
  const struct Map *b = (const struct Map *)MAP_TREE_CONTAINER(rb);
  return (a->addr > b->addr) - (a->addr < b->addr);
}

privileged optimizespeed struct Map *__maps_floor(const char *addr) {
  struct Tree *node;
  if ((node = tree_floor(__maps.maps, addr, __maps_search)))
    return MAP_TREE_CONTAINER(node);
  return 0;
}

static bool __maps_overlaps(const char *addr, size_t size, int pagesz) {
  struct Map *map, *floor = __maps_floor(addr);
  for (map = floor; map && map->addr <= addr + size; map = __maps_next(map))
    if (MAX(addr, map->addr) <
        MIN(addr + PGUP(size), map->addr + PGUP(map->size)))
      return true;
  return false;
}

void __maps_check(void) {
#if MMDEBUG
  size_t maps = 0;
  size_t pages = 0;
  int pagesz = __pagesize;
  static unsigned mono;
  unsigned id = ++mono;
  for (struct Map *map = __maps_first(); map; map = __maps_next(map)) {
    ASSERT(map->addr != MAP_FAILED);
    ASSERT(map->visited != id);
    ASSERT(map->size);
    map->visited = id;
    pages += (map->size + pagesz - 1) / pagesz;
    maps += 1;
    struct Map *next;
    if ((next = __maps_next(map))) {
      ASSERT(map->addr < next->addr);
      ASSERT(MAX(map->addr, next->addr) >=
             MIN(map->addr + PGUP(map->size), next->addr + PGUP(next->size)));
    }
  }
  ASSERT(maps = __maps.count);
  ASSERT(pages == __maps.pages);
#endif
}

static int __muntrack(char *addr, size_t size, int pagesz,
                      struct Map **deleted) {
  int rc = 0;
  struct Map *map;
  struct Map *next;
  struct Map *floor;
StartOver:
  floor = __maps_floor(addr);
  for (map = floor; map && map->addr <= addr + size; map = next) {
    next = __maps_next(map);
    char *map_addr = map->addr;
    size_t map_size = map->size;
    if (!(MAX(addr, map_addr) <
          MIN(addr + PGUP(size), map_addr + PGUP(map_size))))
      continue;
    if (addr <= map_addr && addr + PGUP(size) >= map_addr + PGUP(map_size)) {
      // remove mapping completely
      tree_remove(&__maps.maps, &map->tree);
      map->freed = *deleted;
      *deleted = map;
      __maps.pages -= (map_size + pagesz - 1) / pagesz;
      __maps.count -= 1;
      __maps_check();
    } else if (IsWindows()) {
      STRACE("you can't carve up memory maps on windows ;_;");
      rc = einval();
    } else if (addr <= map_addr) {
      // shave off lefthand side of mapping
      ASSERT(addr + PGUP(size) < map_addr + PGUP(map_size));
      size_t left = addr + PGUP(size) - map_addr;
      size_t right = map_size - left;
      ASSERT(right > 0);
      ASSERT(left > 0);
      struct Map *leftmap;
      if ((leftmap = __maps_alloc())) {
        if (leftmap == MAPS_RETRY)
          goto StartOver;
        map->addr += left;
        map->size = right;
        if (!(map->flags & MAP_ANONYMOUS))
          map->off += left;
        __maps.pages -= (left + pagesz - 1) / pagesz;
        leftmap->addr = map_addr;
        leftmap->size = left;
        leftmap->freed = *deleted;
        *deleted = leftmap;
        __maps_check();
      } else {
        rc = -1;
      }
    } else if (addr + PGUP(size) >= map_addr + PGUP(map_size)) {
      // shave off righthand side of mapping
      size_t left = addr - map_addr;
      size_t right = map_addr + map_size - addr;
      struct Map *rightmap;
      if ((rightmap = __maps_alloc())) {
        if (rightmap == MAPS_RETRY)
          goto StartOver;
        map->size = left;
        __maps.pages -= (right + pagesz - 1) / pagesz;
        rightmap->addr = addr;
        rightmap->size = right;
        rightmap->freed = *deleted;
        *deleted = rightmap;
        __maps_check();
      } else {
        rc = -1;
      }
    } else {
      // punch hole in mapping
      size_t left = addr - map_addr;
      size_t middle = PGUP(size);
      size_t right = map_size - middle - left;
      struct Map *leftmap;
      if ((leftmap = __maps_alloc())) {
        if (leftmap == MAPS_RETRY)
          goto StartOver;
        struct Map *middlemap;
        if ((middlemap = __maps_alloc())) {
          if (middlemap == MAPS_RETRY) {
            __maps_free(leftmap);
            goto StartOver;
          }
          leftmap->addr = map_addr;
          leftmap->size = left;
          leftmap->off = map->off;
          leftmap->prot = map->prot;
          leftmap->flags = map->flags;
          map->addr += left + middle;
          map->size = right;
          if (!(map->flags & MAP_ANONYMOUS))
            map->off += left + middle;
          tree_insert(&__maps.maps, &leftmap->tree, __maps_compare);
          __maps.pages -= (middle + pagesz - 1) / pagesz;
          __maps.count += 1;
          middlemap->addr = addr;
          middlemap->size = size;
          middlemap->freed = *deleted;
          *deleted = middlemap;
          __maps_check();
        } else {
          __maps_free(leftmap);
          rc = -1;
        }
      } else {
        rc = -1;
      }
    }
  }
  return rc;
}

void __maps_free(struct Map *map) {
  uintptr_t tip;
  ASSERT(!TAG(map));
  map->size = 0;
  map->addr = MAP_FAILED;
  for (tip = atomic_load_explicit(&__maps.freed, memory_order_relaxed);;) {
    map->freed = (struct Map *)PTR(tip);
    if (atomic_compare_exchange_weak_explicit(
            &__maps.freed, &tip, ABA(map, TAG(tip) + 1), memory_order_release,
            memory_order_relaxed))
      break;
    pthread_pause_np();
  }
}

static void __maps_free_all(struct Map *list) {
  struct Map *next;
  for (struct Map *map = list; map; map = next) {
    next = map->freed;
    __maps_free(map);
  }
}

void __maps_insert(struct Map *map) {
  map->flags &= MAP_TYPE | MAP_ANONYMOUS | MAP_NOFORK;

  // coalesce adjacent mappings
  if (!IsWindows() && (map->flags & MAP_ANONYMOUS)) {
    int prot = map->prot & ~(MAP_FIXED | MAP_FIXED_NOREPLACE);
    int flags = map->flags;
    bool coalesced = false;
    struct Map *floor, *other, *last = 0;
    for (other = floor = __maps_floor(map->addr);
         other && other->addr <= map->addr + map->size;
         last = other, other = __maps_next(other)) {
      if (prot == other->prot && flags == other->flags) {
        if (!coalesced) {
          if (map->addr == other->addr + other->size) {
            __maps.pages += (map->size + __pagesize - 1) / __pagesize;
            other->size += map->size;
            __maps_free(map);
            __maps_check();
            coalesced = true;
          } else if (map->addr + map->size == other->addr) {
            __maps.pages += (map->size + __pagesize - 1) / __pagesize;
            other->addr -= map->size;
            other->size += map->size;
            __maps_free(map);
            __maps_check();
            coalesced = true;
          }
        }
        if (last && other->addr == last->addr + last->size) {
          other->addr -= last->size;
          other->size += last->size;
          tree_remove(&__maps.maps, &last->tree);
          __maps.count -= 1;
          __maps_free(last);
          __maps_check();
        }
      }
    }
    if (coalesced)
      return;
  }

  // otherwise insert new mapping
  __maps.pages += (map->size + __pagesize - 1) / __pagesize;
  __maps_add(map);
  __maps_check();
}

static void __maps_track_insert(struct Map *map, char *addr, size_t size,
                                uintptr_t map_handle) {
  map->addr = addr;
  map->size = size;
  map->prot = PROT_READ | PROT_WRITE;
  map->flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_NOFORK;
  map->hand = map_handle;
  __maps_lock();
  __maps_insert(map);
  __maps_unlock();
}

bool __maps_track(char *addr, size_t size) {
  struct Map *map;
  do {
    if (!(map = __maps_alloc()))
      return false;
  } while (map == MAPS_RETRY);
  __maps_track_insert(map, addr, size, -1);
  return true;
}

struct Map *__maps_alloc(void) {
  struct Map *map;
  uintptr_t tip = atomic_load_explicit(&__maps.freed, memory_order_relaxed);
  while ((map = (struct Map *)PTR(tip))) {
    if (atomic_compare_exchange_weak_explicit(
            &__maps.freed, &tip, ABA(map->freed, TAG(tip) + 1),
            memory_order_acquire, memory_order_relaxed))
      return map;
    pthread_pause_np();
  }
  int gransz = __gransize;
  struct DirectMap sys = sys_mmap(0, gransz, PROT_READ | PROT_WRITE,
                                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (sys.addr == MAP_FAILED)
    return 0;
  map = sys.addr;
  __maps_track_insert(map, sys.addr, gransz, sys.maphandle);
  for (int i = 1; i < gransz / sizeof(struct Map); ++i)
    __maps_free(map + i);
  return MAPS_RETRY;
}

static int __munmap(char *addr, size_t size) {

  // validate arguments
  int pagesz = __pagesize;
  int gransz = __gransize;
  if (((uintptr_t)addr & (gransz - 1)) ||  //
      !size || (uintptr_t)addr + size < size)
    return einval();

  // lock the memory manager
  // abort on reentry due to signal handler
  if (__maps_lock()) {
    __maps_unlock();
    return edeadlk();
  }
  __maps_check();

  // normalize size
  // abort if size doesn't include all pages in granule
  size_t pgup_size = (size + pagesz - 1) & -pagesz;
  size_t grup_size = (size + gransz - 1) & -gransz;
  if (grup_size > pgup_size)
    if (__maps_overlaps(addr + pgup_size, grup_size - pgup_size, pagesz)) {
      __maps_unlock();
      return einval();
    }

  // untrack mappings
  int rc;
  struct Map *deleted = 0;
  rc = __muntrack(addr, pgup_size, pagesz, &deleted);
  __maps_unlock();

  // delete mappings
  for (struct Map *map = deleted; map; map = map->freed) {
    if (!IsWindows()) {
      if (sys_munmap(map->addr, map->size))
        rc = -1;
    } else if (map->hand != -1) {
      ASSERT(!((uintptr_t)map->addr & (gransz - 1)));
      if (!UnmapViewOfFile(map->addr))
        rc = -1;
      if (!CloseHandle(map->hand))
        rc = -1;
    }
  }

  // freed mappings
  __maps_free_all(deleted);

  return rc;
}

void *__maps_randaddr(void) {
  uintptr_t addr;
  addr = _rand64();
  addr &= 0x3fffffffffff;
  addr |= 0x004000000000;
  addr &= -__gransize;
  return (void *)addr;
}

void *__maps_pickaddr(size_t size) {
  char *addr;
  for (int try = 0; try < MAX_TRIES; ++try) {
    addr = atomic_exchange_explicit(&__maps.pick, 0, memory_order_acq_rel);
    if (!addr)
      addr = __maps_randaddr();
    __maps_lock();
    bool overlaps = __maps_overlaps(addr, size, __pagesize);
    __maps_unlock();
    if (!overlaps) {
      atomic_store_explicit(&__maps.pick,
                            addr + ((size + __gransize - 1) & __gransize),
                            memory_order_release);
      return addr;
    }
  }
  return 0;
}

static void *__mmap_chunk(void *addr, size_t size, int prot, int flags, int fd,
                          int64_t off, int pagesz, int gransz) {

  // allocate Map object
  struct Map *map;
  do {
    if (!(map = __maps_alloc()))
      return MAP_FAILED;
  } while (map == MAPS_RETRY);

  // polyfill nuances of fixed mappings
  int sysflags = flags;
  bool noreplace = false;
  bool should_untrack = false;
  if (flags & MAP_FIXED_NOREPLACE) {
    if (flags & MAP_FIXED) {
      __maps_free(map);
      return (void *)einval();
    }
    sysflags &= ~MAP_FIXED_NOREPLACE;
    if (IsLinux()) {
      noreplace = true;
      sysflags |= MAP_FIXED_NOREPLACE_linux;
    } else if (IsFreebsd() || IsNetbsd()) {
      sysflags |= MAP_FIXED;
      if (__maps_overlaps(addr, size, pagesz)) {
        __maps_free(map);
        return (void *)eexist();
      }
    } else {
      noreplace = true;
    }
  } else if (flags & MAP_FIXED) {
    should_untrack = true;
  }

  // remove mapping we blew away
  if (IsWindows() && should_untrack)
    __munmap(addr, size);

  // obtain mapping from operating system
  int olderr = errno;
  int tries = MAX_TRIES;
  struct DirectMap res;
TryAgain:
  res = sys_mmap(addr, size, prot, sysflags, fd, off);
  if (res.addr == MAP_FAILED) {
    if (IsWindows() && errno == EADDRNOTAVAIL) {
      if (noreplace) {
        errno = EEXIST;
      } else if (should_untrack) {
        errno = ENOMEM;
      } else if (--tries && (addr = __maps_pickaddr(size))) {
        errno = olderr;
        goto TryAgain;
      } else {
        errno = ENOMEM;
      }
    }
    __maps_free(map);
    return MAP_FAILED;
  }

  // polyfill map fixed noreplace
  // we assume non-linux gives us addr if it's freed
  // that's what linux (e.g. rhel7) did before noreplace
  if (noreplace && res.addr != addr) {
    if (!IsWindows()) {
      sys_munmap(res.addr, size);
    } else {
      UnmapViewOfFile(res.addr);
      CloseHandle(res.maphandle);
    }
    __maps_free(map);
    return (void *)eexist();
  }

  // untrack mapping we blew away
  if (!IsWindows() && should_untrack) {
    struct Map *deleted = 0;
    __muntrack(res.addr, size, pagesz, &deleted);
    __maps_free_all(deleted);
  }

  // track map object
  map->addr = res.addr;
  map->size = size;
  map->off = off;
  map->prot = prot;
  map->flags = flags;
  map->hand = res.maphandle;
  if (IsWindows()) {
    map->iscow = (flags & MAP_TYPE) != MAP_SHARED && fd != -1;
    map->readonlyfile = (flags & MAP_TYPE) == MAP_SHARED && fd != -1 &&
                        (g_fds.p[fd].flags & O_ACCMODE) == O_RDONLY;
  }
  __maps_lock();
  __maps_insert(map);
  __maps_unlock();

  return res.addr;
}

static void *__mmap_impl(char *addr, size_t size, int prot, int flags, int fd,
                         int64_t off, int pagesz, int gransz) {

  // validate file map args
  if (!(flags & MAP_ANONYMOUS)) {
    if (off & (gransz - 1))
      return (void *)einval();
    if (IsWindows()) {
      if (!__isfdkind(fd, kFdFile))
        return (void *)eacces();
      if ((g_fds.p[fd].flags & O_ACCMODE) == O_WRONLY)
        return (void *)eacces();
    }
  }

  // try to pick our own addresses on windows which are higher up in the
  // vaspace. this is important so that conflicts are less likely, after
  // forking when resurrecting mappings, because win32 has a strong pref
  // with lower memory addresses which may get assigned to who knows wut
  if (IsWindows() && !addr)
    if (!(addr = __maps_pickaddr(size)))
      return (void *)enomem();

  return __mmap_chunk(addr, size, prot, flags, fd, off, pagesz, gransz);
}

static void *__mmap(char *addr, size_t size, int prot, int flags, int fd,
                    int64_t off) {
  char *res;
  int pagesz = __pagesize;
  int gransz = __gransize;

  // validate arguments
  if ((uintptr_t)addr & (gransz - 1))
    addr = NULL;
  if (!addr && (flags & (MAP_FIXED | MAP_FIXED_NOREPLACE)))
    return (void *)eperm();
  if ((intptr_t)addr < 0)
    return (void *)enomem();
  if (!size || (uintptr_t)addr + size < size)
    return (void *)einval();
  if (size > MAX_SIZE)
    return (void *)enomem();
  if (__maps.count * pagesz + size > __virtualmax)
    return (void *)enomem();

  // create memory mappping
  if (!__isfdkind(fd, kFdZip)) {
    res = __mmap_impl(addr, size, prot, flags, fd, off, pagesz, gransz);
  } else {
    res = _weaken(__zipos_mmap)(
        addr, size, prot, flags,
        (struct ZiposHandle *)(uintptr_t)g_fds.p[fd].handle, off);
  }

  return res;
}

static void *__mremap_impl(char *old_addr, size_t old_size, size_t new_size,
                           int flags, char *new_addr, int pagesz, int gransz) {

  // normalize and validate old size
  // abort if size doesn't include all pages in granule
  size_t pgup_old_size = (old_size + pagesz - 1) & -pagesz;
  size_t grup_old_size = (old_size + gransz - 1) & -gransz;
  if (grup_old_size > pgup_old_size)
    if (__maps_overlaps(old_addr + pgup_old_size, grup_old_size - pgup_old_size,
                        pagesz))
      return (void *)einval();
  old_size = pgup_old_size;

  // validate new size
  // abort if size doesn't include all pages in granule
  if (flags & MREMAP_FIXED) {
    size_t pgup_new_size = (new_size + pagesz - 1) & -pagesz;
    size_t grup_new_size = (new_size + gransz - 1) & -gransz;
    if (grup_new_size > pgup_new_size)
      if (__maps_overlaps(new_addr + pgup_new_size,
                          grup_new_size - pgup_new_size, pagesz))
        return (void *)einval();
  }

  // allocate object for tracking new mapping
  struct Map *map;
  do {
    if (!(map = __maps_alloc()))
      return (void *)enomem();
  } while (map == MAPS_RETRY);

  // check old interval is fully contained within one mapping
  struct Map *old_map;
  if (!(old_map = __maps_floor(old_addr)) ||
      old_addr + old_size > old_map->addr + PGUP(old_map->size) ||
      old_addr < old_map->addr) {
    __maps_free(map);
    return (void *)efault();
  }

  // save old properties
  int old_off = old_map->off;
  int old_prot = old_map->prot;
  int old_flags = old_map->flags;

  // netbsd mremap fixed returns enoent rather than unmapping old pages
  if (IsNetbsd() && (flags & MREMAP_FIXED))
    if (__munmap(new_addr, new_size)) {
      __maps_free(map);
      return MAP_FAILED;
    }

  // release lock before system call if possible
  if (!flags)
    __maps_unlock();

  // the time has come
  char *res;
  if (IsNetbsd()) {
    int sysfl = (flags & MREMAP_FIXED) ? MAP_FIXED : 0;
    res = sys_mremap(old_addr, old_size, (uintptr_t)new_addr, new_size, sysfl);
  } else {
    res = sys_mremap(old_addr, old_size, new_size, flags, (uintptr_t)new_addr);
  }
  if (res == MAP_FAILED)
    __maps_free(map);

  // re-acquire lock if needed
  if (!flags)
    __maps_lock();

  if (res == MAP_FAILED)
    return MAP_FAILED;

  if (!(flags & MREMAP_MAYMOVE))
    ASSERT(res == old_addr);

  // untrack old mapping
  struct Map *deleted = 0;
  __muntrack(old_addr, old_size, pagesz, &deleted);
  __maps_free_all(deleted);

  // track map object
  map->addr = res;
  map->size = new_size;
  map->off = old_off;
  map->prot = old_prot;
  map->flags = old_flags;
  __maps_insert(map);

  return res;
}

static void *__mremap(char *old_addr, size_t old_size, size_t new_size,
                      int flags, char *new_addr) {

  int pagesz = __pagesize;
  int gransz = __gransize;

  // kernel support
  if (!IsLinux() && !IsNetbsd())
    return (void *)enosys();

  // it is not needed
  if (new_size <= old_size)
    if (!(flags & MREMAP_FIXED))
      if (flags & MREMAP_MAYMOVE)
        flags &= ~MREMAP_MAYMOVE;

  // we support these flags
  if (flags & ~(MREMAP_MAYMOVE | MREMAP_FIXED))
    return (void *)einval();
  if (IsNetbsd() && !(flags & MREMAP_MAYMOVE) &&
      ((new_size + pagesz - 1) & -pagesz) > old_size)
    return (void *)enotsup();
  if ((flags & MREMAP_FIXED) && !(flags & MREMAP_MAYMOVE))
    return (void *)einval();

  // addresses must be granularity aligned
  if ((uintptr_t)old_addr & (gransz - 1))
    return (void *)einval();
  if (flags & MREMAP_FIXED)
    if ((uintptr_t)new_addr & (gransz - 1))
      return (void *)einval();

  // sizes must not be zero
  if (!old_size)
    return (void *)einval();
  if (!new_size)
    return (void *)einval();

  // check for big size
  if (old_size > MAX_SIZE)
    return (void *)enomem();
  if (new_size > MAX_SIZE)
    return (void *)enomem();

  // check for overflow
  if ((uintptr_t)old_addr + old_size < old_size)
    return (void *)enomem();
  if (flags & MREMAP_FIXED)
    if ((uintptr_t)new_addr + new_size < new_size)
      return (void *)enomem();

  // old and new intervals must not overlap
  if (flags & MREMAP_FIXED)
    if (MAX(old_addr, new_addr) <
        MIN(old_addr + old_size, new_addr + PGUP(new_size)))
      return (void *)einval();

  // memory increase must not exceed RLIMIT_AS
  if (PGUP(new_size) > old_size)
    if (__maps.count * pagesz - old_size + PGUP(new_size) > __virtualmax)
      return (void *)enomem();

  // lock the memory manager
  // abort on reentry due to signal handler
  if (__maps_lock()) {
    __maps_unlock();
    return (void *)edeadlk();
  }
  __maps_check();

  // perform operation
  char *res = __mremap_impl(old_addr, old_size, new_size, flags, new_addr,
                            pagesz, gransz);

  // return result
  __maps_unlock();
  return res;
}

/**
 * Creates memory mapping.
 *
 * The mmap() function is used by Cosmopolitan's malloc() implementation
 * to obtain new memory from the operating system. This function is also
 * useful for establishing a mapping between a file on disk and a memory
 * address, which avoids most need to call read() and write(). It is how
 * executables are loaded into memory, for instance, in which case pages
 * are loaded lazily from disk by the operating system.
 *
 * The `addr` parameter may be zero. This lets the implementation choose
 * an available address in memory. OSes normally pick something randomly
 * assigned, for security. Most OSes try to make sure subsequent mapping
 * requests will be adjacent to one another. More paranoid OSes may have
 * different mappings be sparse, with unmapped content between them. You
 * may not use the `MAP_FIXED` parameter to create a memory map at NULL.
 *
 * The `addr` parameter may be non-zero, in which case Cosmopolitan will
 * give you a mapping at that specific address if it's available. When a
 * mapping already exists at the requested address then another one will
 * be chosen automatically. On most OSes the newly selected address will
 * be as close-by as possible, but that's not guaranteed. If `MAP_FIXED`
 * is also supplied in `flags` then this hint is taken as mandatory, and
 * existing mappings at the requested interval shall be auto-unmapped.
 *
 * The `size` parameter is implicitly rounded up to the system page size
 * reported by getpagesize() and sysconf(_SC_PAGESIZE). Your extra bytes
 * will be zero-initialized.
 *
 * The returned address will always be aligned, on the system allocation
 * granularity. This value may be obtained from getgransize() or calling
 * sysconf(_SC_GRANSIZE). Granularity is always greater than or equal to
 * the page size. On some platforms, i.e. Windows, it may be larger than
 * the page size.
 *
 * The `prot` value specifies the memory protection of the mapping. This
 * may be `PROT_NONE` to disallow all access otherwise it's a bitwise or
 * of the following constants:
 *
 * - `PROT_READ` allows read access
 * - `PROT_WRITE` allows write access
 * - `PROT_EXEC` allows execute access
 *
 * Some OSes (i.e. OpenBSD) will raise an error if both `PROT_WRITE` and
 * `PROT_EXEC` are requested. You may still modify executable memory but
 * you must use mprotect() to transition between the two states. On some
 * OSes like MacOS ARM64, you need to pass the `MAP_JIT` flag to get RWX
 * memory, which is considered zero on other OSes.
 *
 * The lower bits of the `flags` parameter specify the `MAP_TYPE`, which
 * may be:
 *
 * - `MAP_PRIVATE` for non-shared and copy-on-write mappings
 * - `MAP_SHARED` for memory that may be shared between processes
 *
 * Your `fd` argument specifies the file descriptor of the open file you
 * want to map. This parameter is ignored when `MAP_ANONYMOUS` is passed
 * via `flags`.
 *
 * Your `off` argument specifies the offset into a, file at which mapped
 * memory shall begin. It must be aligned to the allocation granularity,
 * which may be obtained from getgransize() or sysconf(_SC_GRANSIZE).
 *
 * The `MAP_FIXED_NOREPLACE` flag may be passed in `flags` which has the
 * same behavior as `MAP_FIXED` except it raises `EEXIST` when a mapping
 * already exists on the requested interval.
 *
 * The `MAP_CONCEAL` flag may be passed to prevent a memory mapping from
 * appearing in core dumps. This is currently supported on BSD OSes, and
 * is ignored on everything else.
 */
void *mmap(void *addr, size_t size, int prot, int flags, int fd, int64_t off) {
  void *res = __mmap(addr, size, prot, flags, fd, off);
  STRACE("mmap(%p, %'zu, %s, %s, %d, %'ld) → %p% m (%'zu bytes total)", addr,
         size, DescribeProtFlags(prot), DescribeMapFlags(flags), fd, off, res,
         __maps.pages * __pagesize);
  return res;
}

/**
 * Changes memory mapping.
 *
 * This system call lets you move memory without copying it. It can also
 * be used to shrink memory mappings.
 *
 * This system call is supported on Linux and NetBSD. It's used by Cosmo
 * Libc's realloc() implementation under the hood.
 *
 * The `flags` parameter may have:
 *
 * - `MREMAP_MAYMOVE` to allow relocation
 * - `MREMAP_FIXED` in which case an additional parameter is taken
 *
 */
void *mremap(void *old_addr, size_t old_size, size_t new_size, int flags, ...) {
  va_list ap;
  void *new_addr = 0;
  if (flags & MREMAP_FIXED) {
    va_start(ap, flags);
    new_addr = va_arg(ap, void *);
    va_end(ap);
  }
  void *res = __mremap(old_addr, old_size, new_size, flags, new_addr);
  STRACE("mremap(%p, %'zu, %'zu, %s, %p) → %p% m (%'zu bytes total)", old_addr,
         old_size, new_size, DescribeMremapFlags(flags), new_addr, res,
         __maps.pages * __pagesize);
  return res;
}

/**
 * Removes memory mapping.
 *
 * The `size` parameter is implicitly rounded up to the page size.
 *
 * @return 0 on success, or -1 w/ errno.
 */
int munmap(void *addr, size_t size) {
  int rc = __munmap(addr, size);
  STRACE("munmap(%p, %'zu) → %d% m (%'zu bytes total)", addr, size, rc,
         __maps.pages * __pagesize);
  return rc;
}

__weak_reference(mmap, mmap64);
