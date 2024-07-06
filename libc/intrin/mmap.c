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
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/thread.h"

#define MMDEBUG 0               // this code is too slow for openbsd/windows
#define WINBASE 0x100080040000  // TODO: Can we support Windows Vista again?
#define WINMAXX 0x200080000000

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

static bool overlaps_existing_map(const char *addr, size_t size, int pagesz) {
  struct Map *map;
  if ((map = __maps_floor(addr)))
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
    pages += (map->size + getpagesize() - 1) / getpagesize();
    maps += 1;
    struct Map *next;
    if ((next = __maps_next(map))) {
      ASSERT(map->addr < next->addr);
      ASSERT(
          !(MAX(map->addr, next->addr) <
            MIN(map->addr + PGUP(map->size), next->addr + PGUP(next->size))));
    }
  }
  ASSERT(maps = __maps.count);
  ASSERT(pages == __maps.pages);
#endif
}

void __maps_free(struct Map *map) {
  map->size = 0;
  map->addr = MAP_FAILED;
  dll_init(&map->free);
  dll_make_first(&__maps.free, &map->free);
}

static void __maps_insert(struct Map *map) {
  __maps.pages += (map->size + getpagesize() - 1) / getpagesize();
  struct Map *floor = __maps_floor(map->addr);
  if (floor && !IsWindows() &&                 //
      map->addr + map->size == floor->addr &&  //
      (map->flags & MAP_ANONYMOUS) &&          //
      map->flags == floor->flags &&            //
      map->prot == floor->prot) {
    floor->addr -= map->size;
    floor->size += map->size;
    __maps_free(map);
    __maps_check();
  } else {
    __maps_add(map);
    __maps_check();
  }
}

struct Map *__maps_alloc(void) {
  struct Dll *e;
  struct Map *map;
  if ((e = dll_first(__maps.free))) {
    dll_remove(&__maps.free, e);
    map = MAP_FREE_CONTAINER(e);
    return map;
  }
  int granularity = __granularity();
  struct DirectMap sys = sys_mmap(0, granularity, PROT_READ | PROT_WRITE,
                                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (sys.addr == MAP_FAILED)
    return 0;
  if (IsWindows())
    CloseHandle(sys.maphandle);
  map = sys.addr;
  map->addr = MAP_FAILED;
  for (int i = 1; i < granularity / sizeof(struct Map); ++i)
    __maps_free(map + i);
  return map;
}

static int __munmap(char *addr, size_t size, bool untrack_only) {

  // validate arguments
  int pagesz = getpagesize();
  int granularity = __granularity();
  if (((uintptr_t)addr & (granularity - 1)) ||  //
      !size || (uintptr_t)addr + size < size)
    return einval();

  // normalize size
  size = (size + granularity - 1) & -granularity;

  // untrack mappings
  int rc = 0;
  struct Map *map;
  struct Map *next;
  struct Dll *deleted = 0;
  if (__maps_lock()) {
    __maps_unlock();
    return edeadlk();
  }
  for (map = __maps_floor(addr); map; map = next) {
    next = __maps_next(map);
    char *map_addr = map->addr;
    size_t map_size = map->size;
    if (!(MAX(addr, map_addr) < MIN(addr + size, map_addr + PGUP(map_size))))
      break;
    if (addr <= map_addr && addr + size >= map_addr + PGUP(map_size)) {
      // remove mapping completely
      tree_remove(&__maps.maps, &map->tree);
      dll_init(&map->free);
      dll_make_first(&deleted, &map->free);
      __maps.pages -= (map_size + pagesz - 1) / pagesz;
      __maps.count -= 1;
    } else if (IsWindows()) {
      // you can't carve up memory maps on windows. our mmap() makes
      // this not a problem (for non-enormous memory maps) by making
      // independent mappings for each 64 kb granule, under the hood
      rc = einval();
    } else if (addr <= map_addr) {
      // shave off lefthand side of mapping
      ASSERT(addr + size < map_addr + PGUP(map_size));
      size_t left = PGUP(addr + size - map_addr);
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
        dll_make_first(&deleted, &leftmap->free);
      } else {
        rc = -1;
      }
    } else if (addr + size >= map_addr + PGUP(map_size)) {
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
        dll_make_first(&deleted, &rightmap->free);
      } else {
        rc = -1;
      }
    } else {
      // punch hole in mapping
      size_t left = addr - map_addr;
      size_t middle = size;
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
          dll_make_first(&deleted, &middlemap->free);
        } else {
          rc = -1;
        }
      } else {
        rc = -1;
      }
    }
    __maps_check();
  }
  __maps_unlock();

  // delete mappings
  for (struct Dll *e = dll_first(deleted); e; e = dll_next(deleted, e)) {
    struct Map *map = MAP_FREE_CONTAINER(e);
    if (!untrack_only) {
      if (!IsWindows()) {
        if (sys_munmap(map->addr, map->size))
          rc = -1;
      } else if (map->hand != -1) {
        ASSERT(!((uintptr_t)map->addr & (granularity - 1)));
        if (!UnmapViewOfFile(map->addr))
          rc = -1;
        if (!CloseHandle(map->hand))
          rc = -1;
      }
    }
  }

  // free mappings
  if (!dll_is_empty(deleted)) {
    __maps_lock();
    struct Dll *e;
    while ((e = dll_first(deleted))) {
      dll_remove(&deleted, e);
      __maps_free(MAP_FREE_CONTAINER(e));
    }
    __maps_check();
    __maps_unlock();
  }

  return rc;
}

static void *__mmap_chunk(void *addr, size_t size, int prot, int flags, int fd,
                          int64_t off, int pagesz, int granularity) {

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
      if (overlaps_existing_map(addr, size, pagesz))
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
    if (__munmap(addr, size, false))
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
        addr += granularity;
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
  if (!IsWindows() && should_untrack)
    __munmap(res.addr, size, true);

  // track Map object
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
                         int64_t off, int pagesz, int granularity) {

  // validate file map args
  if (!(flags & MAP_ANONYMOUS)) {
    if (off & (granularity - 1))
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
    return __mmap_chunk(addr, size, prot, flags, fd, off, pagesz, granularity);

  // if the concept of pagesz wasn't exciting enough
  if (!addr && !(flags & (MAP_FIXED | MAP_FIXED_NOREPLACE))) {
    size_t slab = (size + granularity - 1) & -granularity;
    addr = (char *)(WINBASE + atomic_fetch_add(&__maps.rollo, slab) % WINMAXX);
  }

  // windows forbids unmapping a subset of a map once it's made
  if (size <= granularity || size > 100 * 1024 * 1024)
    return __mmap_chunk(addr, size, prot, flags, fd, off, pagesz, granularity);

  // so we create a separate map for each granule in the mapping
  if (!(flags & MAP_FIXED)) {
    while (overlaps_existing_map(addr, size, pagesz)) {
      if (flags & MAP_FIXED_NOREPLACE)
        return (void *)eexist();
      addr += granularity;
    }
  }
  char *res = addr;
  while (size) {
    char *got;
    size_t amt = MIN(size, granularity);
    got = __mmap_chunk(addr, amt, prot, flags, fd, off, pagesz, granularity);
    if (got != addr) {
      if (got != MAP_FAILED)
        __munmap(got, amt, false);
      if (addr > res)
        __munmap(res, addr - res, false);
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
  int granularity = __granularity();

  // validate arguments
  if (((uintptr_t)addr & (granularity - 1)) ||  //
      !size || (uintptr_t)addr + size < size)
    return (void *)einval();
  if (size > 0x100000000000)
    return (void *)enomem();
  if (__maps.count * pagesz + size > __virtualmax)
    return (void *)enomem();

  // create memory mappping
  if (!__isfdkind(fd, kFdZip)) {
    res = __mmap_impl(addr, size, prot, flags, fd, off, pagesz, granularity);
  } else {
    res = _weaken(__zipos_mmap)(
        addr, size, prot, flags,
        (struct ZiposHandle *)(uintptr_t)g_fds.p[fd].handle, off);
  }

  return res;
}

void *mmap(void *addr, size_t size, int prot, int flags, int fd, int64_t off) {
  void *res = __mmap(addr, size, prot, flags, fd, off);
  STRACE("mmap(%p, %'zu, %s, %s, %d, %'ld) → %p% m", addr, size,
         DescribeProtFlags(prot), DescribeMapFlags(flags), fd, off, res);
  return res;
}

int munmap(void *addr, size_t size) {
  int rc = __munmap(addr, size, false);
  STRACE("munmap(%p, %'zu) → %d% m", addr, size, rc);
  return rc;
}

__weak_reference(mmap, mmap64);
