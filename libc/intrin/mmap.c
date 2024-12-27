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
#include "libc/limits.h"
#include "libc/macros.h"
#include "libc/nt/memory.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/stdckdint.h"
#include "libc/stdio/sysparam.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/mremap.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/errfuns.h"

#define MMDEBUG  0
#define MAX_SIZE 0x0ff800000000ul

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

static bool __maps_overlaps(const char *addr, size_t size) {
  int pagesz = __pagesize;
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

static int __maps_funge_prot(int prot) {
  prot &= ~MAP_FIXED;
  prot &= ~MAP_FIXED_NOREPLACE;
  return prot;
}

static int __maps_funge_flags(int flags) {
  if ((flags & MAP_TYPE) == MAP_SHARED_VALIDATE) {
    flags &= ~MAP_TYPE;
    flags |= MAP_SHARED;
  }
  return flags;
}

static bool __maps_fungible(const struct Map *map) {
  // anonymous memory is fungible on unix, so we may coalesce such
  // mappings in the rbtree to have fewer objects. on windows even
  // anonymous memory has unique win32 handles we need to preserve
  return !IsWindows() && (map->flags & MAP_ANONYMOUS);
}

static bool __maps_adjacent(const struct Map *x, const struct Map *y) {
  char *a = x->addr + ((x->size + __pagesize - 1) & -__pagesize);
  char *b = y->addr;
  ASSERT(a <= b);
  return a == b;
}

static bool __maps_mergeable(const struct Map *x, const struct Map *y) {
  if (!__maps_fungible(x))
    return false;
  if (!__maps_fungible(y))
    return false;
  if (!__maps_adjacent(x, y))
    return false;
  if (__maps_funge_prot(x->prot) != __maps_funge_prot(y->prot))
    return false;
  if (__maps_funge_flags(x->flags) != __maps_funge_flags(y->flags))
    return false;
  return true;
}

void __maps_insert(struct Map *map) {
  struct Map *left, *right;
  ASSERT(map->size);
  ASSERT(!__maps_overlaps(map->addr, map->size));
  map->flags &= MAP_TYPE | MAP_ANONYMOUS | MAP_NOFORK;
  __maps.pages += (map->size + __pagesize - 1) / __pagesize;

  // find adjacent mappings
  if ((left = __maps_floor(map->addr))) {
    right = __maps_next(left);
  } else {
    right = __maps_first();
  }

  // avoid insert by making mapping on left bigger
  if (left)
    if (__maps_mergeable(left, map)) {
      left->size += __pagesize - 1;
      left->size &= -__pagesize;
      left->size += map->size;
      __maps_free(map);
      map = 0;
    }

  // avoid insert by making mapping on right bigger
  if (map && right)
    if (__maps_mergeable(map, right)) {
      map->size += __pagesize - 1;
      map->size &= -__pagesize;
      right->addr -= map->size;
      right->size += map->size;
      __maps_free(map);
      map = 0;
    }

  // check if we filled a hole
  if (!map && left && right)
    if (__maps_mergeable(left, right)) {
      left->size += __pagesize - 1;
      left->size &= -__pagesize;
      right->addr -= left->size;
      right->size += left->size;
      tree_remove(&__maps.maps, &left->tree);
      __maps.count -= 1;
      __maps_free(left);
      map = 0;
    }

  // otherwise just insert
  if (map)
    __maps_add(map);

  // sanity check
  __maps_check();
}

static void __maps_track_insert(struct Map *map, char *addr, size_t size,
                                uintptr_t map_handle, int prot, int flags) {
  map->addr = addr;
  map->size = size;
  map->prot = prot;
  map->flags = flags;
  map->hand = map_handle;
  __maps_lock();
  __maps_insert(map);
  __maps_unlock();
}

// adds interval to rbtree (no sys_mmap)
bool __maps_track(char *addr, size_t size, int prot, int flags) {
  struct Map *map;
  do {
    if (!(map = __maps_alloc()))
      return false;
  } while (map == MAPS_RETRY);
  __maps_track_insert(map, addr, size, -1, prot, flags);
  return true;
}

// removes interval from rbtree (no sys_munmap)
int __maps_untrack(char *addr, size_t size) {
  struct Map *deleted = 0;
  __maps_lock();
  int rc = __muntrack(addr, size, __pagesize, &deleted);
  __maps_unlock();
  __maps_free_all(deleted);
  return rc;
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
  void *mark;
  int size = 65536;
  __maps_lock();
  do {
    // we're creating sudden surprise memory. the user might be in the
    // middle of carefully planning a fixed memory structure. we don't
    // want the system allocator to put our surprise memory inside it.
    mark = __maps_randaddr();
  } while (__maps_overlaps(mark, size));
  struct DirectMap sys = sys_mmap(mark, size, PROT_READ | PROT_WRITE,
                                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (sys.addr == MAP_FAILED) {
    __maps_unlock();
    return 0;
  }
  map = sys.addr;
  __maps_track_insert(map, sys.addr, size, sys.maphandle,
                      PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS | MAP_NOFORK);
  __maps_unlock();
  for (int i = 1; i < size / sizeof(struct Map); ++i)
    __maps_free(map + i);
  return MAPS_RETRY;
}

static int __munmap(char *addr, size_t size) {

  // validate arguments
  if (((uintptr_t)addr & (__gransize - 1)) ||  //
      !size || (uintptr_t)addr + size < size)
    return einval();

  // lock the memory manager
  __maps_lock();
  __maps_check();

  // normalize size
  // abort if size doesn't include all pages in granule
  size_t pgup_size = (size + __pagesize - 1) & -__pagesize;
  size_t grup_size = (size + __gransize - 1) & -__gransize;
  if (grup_size > pgup_size)
    if (__maps_overlaps(addr + pgup_size, grup_size - pgup_size)) {
      __maps_unlock();
      return einval();
    }

  // untrack mappings
  int rc;
  struct Map *deleted = 0;
  rc = __muntrack(addr, pgup_size, __pagesize, &deleted);
  __maps_unlock();

  // delete mappings
  for (struct Map *map = deleted; map; map = map->freed) {
    if (!IsWindows()) {
      if (sys_munmap(map->addr, map->size))
        rc = -1;
    } else if (map->hand != -1) {
      ASSERT(!((uintptr_t)map->addr & (__gransize - 1)));
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
  __maps_lock();
  addr = (__maps.rand *= 15750249268501108917ull) >> 64;
  __maps_unlock();
  addr &= 0x3fffffffffff;
  addr |= 0x004000000000;
  addr &= -__gransize;
  return (void *)addr;
}

static void *__maps_pickaddr(size_t size) {
  char *addr = 0;
  struct Map *map, *prev;
  size += __gransize - 1;
  size &= -__gransize;
  if ((map = __maps_last())) {
    // choose address beneath higher mapping
    for (; map; map = prev) {
      char *min = (char *)(intptr_t)__gransize;
      if ((prev = __maps_prev(map)))
        min = prev->addr + ((prev->size + __gransize - 1) & -__gransize);
      if (map->addr > min &&  //
          map->addr - min >= size) {
        addr = map->addr - size;
        break;
      }
    }
    // append if existing maps are too dense
    if (!addr) {
      map = __maps_last();
      addr = map->addr + ((map->size + __gransize - 1) & -__gransize);
      intptr_t end = (intptr_t)addr;
      if (ckd_add(&end, end, size))
        return 0;
    }
  } else {
    // roll the dice if rbtree is empty
    addr = __maps_randaddr();
  }
  return addr;
}

static void *__mmap_impl(char *addr, size_t size, int prot, int flags, int fd,
                         int64_t off) {

  // validate file map args
  if (!(flags & MAP_ANONYMOUS)) {
    if (off & (__gransize - 1))
      return (void *)einval();
    if (IsWindows()) {
      if (!__isfdkind(fd, kFdFile))
        return (void *)eacces();
      if ((g_fds.p[fd].flags & O_ACCMODE) == O_WRONLY)
        return (void *)eacces();
    }
  }

  // allocate Map object
  struct Map *map;
  do {
    if (!(map = __maps_alloc()))
      return MAP_FAILED;
  } while (map == MAPS_RETRY);

  // polyfill nuances of fixed mappings
  int sysflags = flags;
  bool noreplace = false;
  bool fixedmode = false;
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
      if (__maps_overlaps(addr, size)) {
        __maps_free(map);
        return (void *)eexist();
      }
    } else {
      noreplace = true;
    }
  } else if (flags & MAP_FIXED) {
    fixedmode = true;
  }

  // loop for memory
  int olderr = errno;
  struct DirectMap res;
  for (;;) {

    // transactionally find the mark on windows
    if (IsWindows()) {
      __maps_lock();
      if (!fixedmode) {
        // give user desired address if possible
        if (addr && __maps_overlaps(addr, size)) {
          if (noreplace) {
            __maps_unlock();
            __maps_free(map);
            return (void *)eexist();
          }
          addr = 0;
        }
        // choose suitable address then claim it in our rbtree
        if (!addr && !(addr = __maps_pickaddr(size))) {
          __maps_unlock();
          __maps_free(map);
          return (void *)enomem();
        }
      } else {
        // remove existing mappings and their tracking objects
        if (__munmap(addr, size)) {
          __maps_unlock();
          __maps_free(map);
          return (void *)enomem();
        }
      }
      // claims intended interval while still holding the lock
      if (!__maps_track(addr, size, 0, 0)) {
        __maps_unlock();
        __maps_free(map);
        return (void *)enomem();
      }
      __maps_unlock();
    }

    // ask operating system for our memory
    // notice how we're not holding the lock
    res = sys_mmap(addr, size, prot, sysflags, fd, off);
    if (res.addr != MAP_FAILED)
      break;

    // handle failure
    if (IsWindows()) {
      if (errno == EADDRNOTAVAIL) {
        // we've encountered mystery memory
        if (fixedmode) {
          // TODO(jart): Use VirtualQuery() to destroy mystery memory.
          __maps_untrack(addr, size);
          errno = ENOMEM;
        } else if (noreplace) {
          // we can't try again with a different address in this case
          __maps_untrack(addr, size);
          errno = EEXIST;
        } else {
          // we shall leak the tracking object since it should at least
          // partially cover the mystery mapping. so if we loop forever
          // the system should eventually recover and find fresh spaces
          errno = olderr;
          addr = 0;
          continue;
        }
      } else {
        __maps_untrack(addr, size);
      }
    }
    __maps_free(map);
    return MAP_FAILED;
  }

  // polyfill map fixed noreplace
  if (noreplace && res.addr != addr) {
    ASSERT(!IsWindows());
    sys_munmap(res.addr, size);
    __maps_free(map);
    return (void *)eexist();
  }

  // setup map object
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

  // track map object
  __maps_lock();
  if (IsWindows() || fixedmode)
    __maps_untrack(res.addr, size);
  __maps_insert(map);
  __maps_unlock();

  return res.addr;
}

static void *__mmap(char *addr, size_t size, int prot, int flags, int fd,
                    int64_t off) {
  char *res;

  // validate arguments
  if ((uintptr_t)addr & (__gransize - 1))
    addr = NULL;
  if (!addr && (flags & (MAP_FIXED | MAP_FIXED_NOREPLACE)))
    return (void *)eperm();
  if ((intptr_t)addr < 0)
    return (void *)enomem();
  if (!size || (uintptr_t)addr + size < size)
    return (void *)einval();
  if (size > MAX_SIZE)
    return (void *)enomem();
  if (__maps.count * __pagesize + size > __virtualmax)
    return (void *)enomem();

  // create memory mappping
  if (!__isfdkind(fd, kFdZip)) {
    res = __mmap_impl(addr, size, prot, flags, fd, off);
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
    if (__maps_overlaps(old_addr + pgup_old_size,
                        grup_old_size - pgup_old_size))
      return (void *)einval();
  old_size = pgup_old_size;

  // validate new size
  // abort if size doesn't include all pages in granule
  if (flags & MREMAP_FIXED) {
    size_t pgup_new_size = (new_size + pagesz - 1) & -pagesz;
    size_t grup_new_size = (new_size + gransz - 1) & -gransz;
    if (grup_new_size > pgup_new_size)
      if (__maps_overlaps(new_addr + pgup_new_size,
                          grup_new_size - pgup_new_size))
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
