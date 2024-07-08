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
#include "ape/sections.internal.h"
#include "libc/atomic.h"
#include "libc/calls/blockcancel.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/describebacktrace.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/directmap.internal.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/maps.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/nt/memory.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/stdio/sysparam.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/mremap.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

#define MMDEBUG IsModeDbg()
#define WINBASE (1ul << 35)              // 34 gb
#define WINMAXX ((1ul << 44) - WINBASE)  // 17 tb

#define MAP_FIXED_NOREPLACE_linux 0x100000

#define PGUP(x) (((x) + pagesz - 1) & -pagesz)

#if !MMDEBUG
#define ASSERT(x) (void)0
#else
#define ASSERT(x)                                                         \
  do {                                                                    \
    if (!(x)) {                                                           \
      char bt[160];                                                       \
      struct StackFrame *bp = __builtin_frame_address(0);                 \
      kprintf("%!s:%d: assertion failed: %!s\n", __FILE__, __LINE__, #x); \
      kprintf("bt %!s\n", (DescribeBacktrace)(bt, bp));                   \
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
  int pagesz = getpagesize();
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
                      struct Dll **deleted) {
  int rc = 0;
  struct Map *map;
  struct Map *next;
  struct Map *floor;
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
      dll_init(&map->free);
      dll_make_first(deleted, &map->free);
      __maps.pages -= (map_size + pagesz - 1) / pagesz;
      __maps.count -= 1;
      __maps_check();
    } else if (IsWindows()) {
      // you can't carve up memory maps on windows. our mmap() makes
      // this not a problem (for non-enormous memory maps) by making
      // independent mappings for each 64 kb granule, under the hood
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
        map->addr += left;
        map->size = right;
        if (!(map->flags & MAP_ANONYMOUS))
          map->off += left;
        __maps.pages -= (left + pagesz - 1) / pagesz;
        leftmap->addr = map_addr;
        leftmap->size = left;
        dll_init(&leftmap->free);
        dll_make_first(deleted, &leftmap->free);
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
        map->size = left;
        __maps.pages -= (right + pagesz - 1) / pagesz;
        rightmap->addr = addr;
        rightmap->size = right;
        dll_init(&rightmap->free);
        dll_make_first(deleted, &rightmap->free);
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
        struct Map *middlemap;
        if ((middlemap = __maps_alloc())) {
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
          dll_init(&middlemap->free);
          dll_make_first(deleted, &middlemap->free);
          __maps_check();
        } else {
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
  map->size = 0;
  map->addr = MAP_FAILED;
  dll_init(&map->free);
  dll_make_first(&__maps.free, &map->free);
}

static void __maps_insert(struct Map *map) {
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
            __maps.pages += (map->size + getpagesize() - 1) / getpagesize();
            other->size += map->size;
            __maps_free(map);
            __maps_check();
            coalesced = true;
          } else if (map->addr + map->size == other->addr) {
            __maps.pages += (map->size + getpagesize() - 1) / getpagesize();
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
  __maps.pages += (map->size + getpagesize() - 1) / getpagesize();
  __maps_add(map);
  __maps_check();
}

struct Map *__maps_alloc(void) {
  struct Dll *e;
  struct Map *map;
  if ((e = dll_first(__maps.free))) {
    dll_remove(&__maps.free, e);
    map = MAP_FREE_CONTAINER(e);
    return map;
  }
  int gransz = getgransize();
  struct DirectMap sys = sys_mmap(0, gransz, PROT_READ | PROT_WRITE,
                                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (sys.addr == MAP_FAILED)
    return 0;
  map = sys.addr;
  map->addr = sys.addr;
  map->size = gransz;
  map->prot = PROT_READ | PROT_WRITE;
  map->flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_NOFORK;
  map->hand = sys.maphandle;
  __maps_insert(map++);
  map->addr = MAP_FAILED;
  for (int i = 1; i < gransz / sizeof(struct Map) - 1; ++i)
    __maps_free(map + i);
  return map;
}

static int __munmap(char *addr, size_t size) {

  // validate arguments
  int pagesz = getpagesize();
  int gransz = getgransize();
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
  struct Dll *deleted = 0;
  __muntrack(addr, pgup_size, pagesz, &deleted);
  __maps_unlock();

  // delete mappings
  int rc = 0;
  for (struct Dll *e = dll_first(deleted); e; e = dll_next(deleted, e)) {
    struct Map *map = MAP_FREE_CONTAINER(e);
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

  // free mappings
  if (!dll_is_empty(deleted)) {
    __maps_lock();
    dll_make_first(&__maps.free, deleted);
    __maps_unlock();
  }

  return rc;
}

static void *__mmap_chunk(void *addr, size_t size, int prot, int flags, int fd,
                          int64_t off, int pagesz, int gransz) {

  // polyfill nuances of fixed mappings
  int sysflags = flags;
  bool noreplace = false;
  bool should_untrack = false;
  if (flags & MAP_FIXED_NOREPLACE) {
    if (flags & MAP_FIXED)
      return (void *)einval();
    sysflags &= ~MAP_FIXED_NOREPLACE;
    if (IsLinux()) {
      noreplace = true;
      sysflags |= MAP_FIXED_NOREPLACE_linux;
    } else if (IsFreebsd() || IsNetbsd()) {
      sysflags |= MAP_FIXED;
      if (__maps_overlaps(addr, size, pagesz))
        return (void *)eexist();
    } else {
      noreplace = true;
    }
  } else if (flags & MAP_FIXED) {
    should_untrack = true;
  }

  // allocate Map object
  struct Map *map;
  if (__maps_lock()) {
    __maps_unlock();
    return (void *)edeadlk();
  }
  __maps_check();
  map = __maps_alloc();
  __maps_unlock();
  if (!map)
    return MAP_FAILED;

  // remove mapping we blew away
  if (IsWindows() && should_untrack)
    if (__munmap(addr, size))
      return MAP_FAILED;

  // obtain mapping from operating system
  int olderr = errno;
  struct DirectMap res;
TryAgain:
  res = sys_mmap(addr, size, prot, sysflags, fd, off);
  if (res.addr == MAP_FAILED) {
    if (IsWindows() && errno == EADDRNOTAVAIL) {
      if (noreplace) {
        errno = EEXIST;
      } else if (should_untrack) {
        errno = ENOMEM;
      } else {
        addr += gransz;
        errno = olderr;
        goto TryAgain;
      }
    }
    __maps_lock();
    __maps_free(map);
    __maps_unlock();
    return MAP_FAILED;
  }

  // polyfill map fixed noreplace
  // we assume non-linux gives us addr if it's free
  // that's what linux (e.g. rhel7) did before noreplace
  if (noreplace && res.addr != addr) {
    if (!IsWindows()) {
      sys_munmap(res.addr, size);
    } else {
      UnmapViewOfFile(res.addr);
      CloseHandle(res.maphandle);
    }
    __maps_lock();
    __maps_free(map);
    __maps_unlock();
    return (void *)eexist();
  }

  // untrack mapping we blew away
  if (!IsWindows() && should_untrack) {
    struct Dll *deleted = 0;
    __muntrack(res.addr, size, pagesz, &deleted);
    if (!dll_is_empty(deleted)) {
      __maps_lock();
      dll_make_first(&__maps.free, deleted);
      __maps_unlock();
    }
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

  // mmap works fine on unix
  if (!IsWindows())
    return __mmap_chunk(addr, size, prot, flags, fd, off, pagesz, gransz);

  // if the concept of pagesz wasn't exciting enough
  if (!addr && !(flags & (MAP_FIXED | MAP_FIXED_NOREPLACE))) {
    size_t rollo, rollo2, slab = (size + gransz - 1) & -gransz;
    rollo = atomic_load_explicit(&__maps.rollo, memory_order_relaxed);
    for (;;) {
      if ((rollo2 = rollo + slab) > WINMAXX) {
        rollo = 0;
        rollo2 = slab;
      }
      if (atomic_compare_exchange_weak_explicit(&__maps.rollo, &rollo, rollo2,
                                                memory_order_acq_rel,
                                                memory_order_relaxed)) {
        addr = (char *)WINBASE + rollo;
        break;
      }
    }
  }

  // windows forbids unmapping a subset of a map once it's made
  if (size <= gransz || size > 100 * 1024 * 1024)
    return __mmap_chunk(addr, size, prot, flags, fd, off, pagesz, gransz);

  // so we create a separate map for each granule in the mapping
  if (!(flags & MAP_FIXED)) {
    while (__maps_overlaps(addr, size, pagesz)) {
      if (flags & MAP_FIXED_NOREPLACE)
        return (void *)eexist();
      addr += gransz;
    }
  }
  char *res = addr;
  while (size) {
    char *got;
    size_t amt = MIN(size, gransz);
    got = __mmap_chunk(addr, amt, prot, flags, fd, off, pagesz, gransz);
    if (got != addr) {
      if (got != MAP_FAILED)
        __munmap(got, amt);
      if (addr > res)
        __munmap(res, addr - res);
      errno = EAGAIN;
      return MAP_FAILED;
    }
    size -= amt;
    addr += amt;
    off += amt;
  }
  return res;
}

static void *__mmap(char *addr, size_t size, int prot, int flags, int fd,
                    int64_t off) {
  char *res;
  int pagesz = getpagesize();
  int gransz = getgransize();

  // validate arguments
  if (((uintptr_t)addr & (gransz - 1)) ||  //
      !size || (uintptr_t)addr + size < size)
    return (void *)einval();
  if (size > WINMAXX)
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

  // check old interval is fully contained within one mapping
  struct Map *old_map;
  if (!(old_map = __maps_floor(old_addr)) ||
      old_addr + old_size > old_map->addr + PGUP(old_map->size) ||
      old_addr < old_map->addr)
    return (void *)efault();

  // save old properties
  int old_off = old_map->off;
  int old_prot = old_map->prot;
  int old_flags = old_map->flags;

  // allocate object for tracking new mapping
  struct Map *map;
  if (!(map = __maps_alloc()))
    return (void *)enomem();

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

  // re-acquire lock if needed
  if (!flags)
    __maps_lock();

  // check result
  if (res == MAP_FAILED) {
    __maps_free(map);
    return MAP_FAILED;
  }

  if (!(flags & MREMAP_MAYMOVE))
    ASSERT(res == old_addr);

  // untrack old mapping
  struct Dll *deleted = 0;
  __muntrack(old_addr, old_size, pagesz, &deleted);
  dll_make_first(&__maps.free, deleted);
  deleted = 0;

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

  int pagesz = getpagesize();
  int gransz = getgransize();

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
  if (old_size > WINMAXX)
    return (void *)enomem();
  if (new_size > WINMAXX)
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

void *mmap(void *addr, size_t size, int prot, int flags, int fd, int64_t off) {
  void *res = __mmap(addr, size, prot, flags, fd, off);
  STRACE("mmap(%p, %'zu, %s, %s, %d, %'ld) → %p% m", addr, size,
         DescribeProtFlags(prot), DescribeMapFlags(flags), fd, off, res);
  return res;
}

void *mremap(void *old_addr, size_t old_size, size_t new_size, int flags, ...) {
  va_list ap;
  void *new_addr = 0;
  if (flags & MREMAP_FIXED) {
    va_start(ap, flags);
    new_addr = va_arg(ap, void *);
    va_end(ap);
  }
  void *res = __mremap(old_addr, old_size, new_size, flags, new_addr);
  STRACE("mremap(%p, %'zu, %'zu, %s, %p) → %p% m", old_addr, old_size, new_size,
         DescribeMremapFlags(flags), new_addr, res);
  return res;
}

int munmap(void *addr, size_t size) {
  int rc = __munmap(addr, size);
  STRACE("munmap(%p, %'zu) → %d% m", addr, size, rc);
  return rc;
}

__weak_reference(mmap, mmap64);
