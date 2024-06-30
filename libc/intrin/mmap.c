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

#define PGUP(x) (((x) + granularity - 1) & -granularity)

#if MMDEBUG
#define ASSERT(x) (void)0
#else
#define ASSERT(x)                                                         \
  do {                                                                    \
    if (!(x)) {                                                           \
      char bt[160];                                                       \
      struct StackFrame *bp = __builtin_frame_address(0);                 \
      kprintf("%!s:%d: assertion failed: %!s\n", __FILE__, __LINE__, #x); \
      kprintf("bt %!s\n", (DescribeBacktrace)(bt, bp));                   \
      __print_maps();                                                     \
      _Exit(99);                                                          \
    }                                                                     \
  } while (0)
#endif

static atomic_ulong rollo;

static bool overlaps_existing_map(const char *addr, size_t size) {
  int granularity = __granularity();
  for (struct Map *map = __maps.maps; map; map = map->next)
    if (MAX(addr, map->addr) <
        MIN(addr + PGUP(size), map->addr + PGUP(map->size)))
      return true;
  return false;
}

void __maps_check(void) {
#if MMDEBUG
  size_t maps = 0;
  size_t pages = 0;
  int granularity = getauxval(AT_PAGESZ);
  unsigned id = __maps.mono++;
  for (struct Map *map = __maps.maps; map; map = map->next) {
    ASSERT(map->addr != MAP_FAILED);
    ASSERT(map->visited != id);
    ASSERT(map->size);
    map->visited = id;
    pages += PGUP(map->size) / granularity;
    maps += 1;
  }
  ASSERT(maps = __maps.count);
  ASSERT(pages == __maps.pages);
  for (struct Dll *e = dll_first(__maps.used); e;
       e = dll_next(__maps.used, e)) {
    ASSERT(MAP_CONTAINER(e)->visited == id);
    --maps;
  }
  ASSERT(maps == 0);
  for (struct Map *m1 = __maps.maps; m1; m1 = m1->next)
    for (struct Map *m2 = m1->next; m2; m2 = m2->next)
      ASSERT(MAX(m1->addr, m2->addr) >=
             MIN(m1->addr + PGUP(m1->size), m2->addr + PGUP(m2->size)));
#endif
}

void __maps_free(struct Map *map) {
  map->next = 0;
  map->size = 0;
  map->addr = MAP_FAILED;
  ASSERT(dll_is_alone(&map->elem));
  dll_make_last(&__maps.free, &map->elem);
}

void __maps_insert(struct Map *map) {
  struct Map *last = __maps.maps;
  int granularity = getauxval(AT_PAGESZ);
  __maps.pages += PGUP(map->size) / granularity;
  if (last && !IsWindows() &&                  //
      map->addr == last->addr + last->size &&  //
      (map->flags & MAP_ANONYMOUS) &&          //
      map->flags == last->flags &&             //
      map->prot == last->prot) {
    last->size += map->size;
    dll_remove(&__maps.used, &last->elem);
    dll_make_first(&__maps.used, &last->elem);
    __maps_free(map);
  } else if (last && !IsWindows() &&                 //
             map->addr + map->size == last->addr &&  //
             (map->flags & MAP_ANONYMOUS) &&         //
             map->flags == last->flags &&            //
             map->prot == last->prot) {
    last->addr -= map->size;
    last->size += map->size;
    dll_remove(&__maps.used, &last->elem);
    dll_make_first(&__maps.used, &last->elem);
    __maps_free(map);
  } else {
    dll_make_first(&__maps.used, &map->elem);
    map->next = __maps.maps;
    __maps.maps = map;
    ++__maps.count;
  }
  __maps_check();
}

struct Map *__maps_alloc(void) {
  struct Dll *e;
  struct Map *map;
  if ((e = dll_first(__maps.free))) {
    dll_remove(&__maps.free, e);
    map = MAP_CONTAINER(e);
    map->next = 0;
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
  dll_init(&map->elem);
  for (int i = 1; i < granularity / sizeof(struct Map); ++i) {
    dll_init(&map[i].elem);
    __maps_free(map + i);
  }
  map->next = 0;
  return map;
}

int __munmap(char *addr, size_t size, bool untrack_only) {

  // validate arguments
  int pagesz = getauxval(AT_PAGESZ);
  int granularity = __granularity();
  if (((uintptr_t)addr & (granularity - 1)) ||  //
      !size || (uintptr_t)addr + size < size)
    return einval();

  // untrack mappings
  int rc = 0;
  struct Dll *delete = 0;
  __maps_lock();
  struct Map *map = __maps.maps;
  struct Map **prev = &__maps.maps;
  while (map) {
    char *map_addr = map->addr;
    size_t map_size = map->size;
    struct Map *next = map->next;
    if (MAX(addr, map_addr) <
        MIN(addr + PGUP(size), map_addr + PGUP(map_size))) {
      if (addr <= map_addr && addr + PGUP(size) >= map_addr + PGUP(map_size)) {
        // remove mapping completely
        dll_remove(&__maps.used, &map->elem);
        dll_make_first(&delete, &map->elem);
        *prev = next;
        __maps.pages -= (map_size + pagesz - 1) / pagesz;
        __maps.count -= 1;
        map = next;
        continue;
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
          __maps_check();
          leftmap->addr = map_addr;
          leftmap->size = left;
          dll_make_first(&delete, &leftmap->elem);
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
          __maps_check();
          rightmap->addr = addr;
          rightmap->size = right;
          dll_make_first(&delete, &rightmap->elem);
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
            leftmap->next = map;
            leftmap->addr = map_addr;
            leftmap->size = left;
            leftmap->off = map->off;
            leftmap->prot = map->prot;
            leftmap->flags = map->flags;
            map->addr += left + middle;
            map->size = right;
            if (!(map->flags & MAP_ANONYMOUS))
              map->off += left + middle;
            dll_make_first(&__maps.used, &leftmap->elem);
            *prev = leftmap;
            __maps.pages -= (middle + pagesz - 1) / pagesz;
            __maps.count += 1;
            __maps_check();
            middlemap->addr = addr;
            middlemap->size = size;
            dll_make_first(&delete, &middlemap->elem);
          } else {
            rc = -1;
          }
        } else {
          rc = -1;
        }
      }
    }
    prev = &map->next;
    map = next;
  }
  __maps_unlock();

  // delete mappings
  for (struct Dll *e = dll_first(delete); e; e = dll_next(delete, e)) {
    map = MAP_CONTAINER(e);
    if (!untrack_only) {
      if (!IsWindows()) {
        if (sys_munmap(map->addr, map->size))
          rc = -1;
      } else {
        if (!UnmapViewOfFile(map->addr))
          rc = -1;
        if (!CloseHandle(map->h))
          rc = -1;
      }
    }
  }

  // free mappings
  if (!dll_is_empty(delete)) {
    __maps_lock();
    struct Dll *e;
    while ((e = dll_first(delete))) {
      dll_remove(&delete, e);
      __maps_free(MAP_CONTAINER(e));
    }
    __maps_check();
    __maps_unlock();
  }

  return rc;
}

static void *__mmap_chunk(void *addr, size_t size, int prot, int flags, int fd,
                          int64_t off, int granularity) {

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
      if (overlaps_existing_map(addr, size))
        return (void *)eexist();
    } else {
      noreplace = true;
    }
  } else if (flags & MAP_FIXED) {
    should_untrack = true;
  }

  // allocate Map object
  struct Map *map;
  __maps_lock();
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
  map->h = res.maphandle;
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
                         int64_t off, int granularity) {

  // validate file map args
  if (fd != -1) {
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
    return __mmap_chunk(addr, size, prot, flags, fd, off, granularity);

  // if the concept of granularity wasn't exciting enough
  if (!addr && !(flags & (MAP_FIXED | MAP_FIXED_NOREPLACE)))
    addr = (char *)(WINBASE + atomic_fetch_add(&rollo, PGUP(size)) % WINMAXX);

  // windows forbids unmapping a subset of a map once it's made
  if (size <= granularity || size > 100 * 1024 * 1024)
    return __mmap_chunk(addr, size, prot, flags, fd, off, granularity);

  // so we create a separate map for each granule in the mapping
  if (!(flags & MAP_FIXED)) {
    while (overlaps_existing_map(addr, size)) {
      if (flags & MAP_FIXED_NOREPLACE)
        return (void *)eexist();
      addr += granularity;
    }
  }
  char *res = addr;
  while (size) {
    char *got;
    size_t amt = MIN(size, granularity);
    got = __mmap_chunk(addr, amt, prot, flags, fd, off, granularity);
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

void *__mmap(char *addr, size_t size, int prot, int flags, int fd,
             int64_t off) {
  char *res;
  int granularity = __granularity();

  // validate arguments
  if (((uintptr_t)addr & (granularity - 1)) ||  //
      !size || (uintptr_t)addr + size < size)
    return (void *)einval();
  if (size > 0x100000000000)
    return (void *)enomem();

  // normalize arguments
  if (flags & MAP_ANONYMOUS) {
    fd = -1;
    off = 0;
    size = PGUP(size);
  }

  // create memory mappping
  if (!__isfdkind(fd, kFdZip)) {
    res = __mmap_impl(addr, size, prot, flags, fd, off, granularity);
  } else {
    res = _weaken(__zipos_mmap)(
        addr, size, prot, flags,
        (struct ZiposHandle *)(uintptr_t)g_fds.p[fd].handle, off);
  }

  return res;
}

void *mmap(void *addr, size_t size, int prot, int flags, int fd, int64_t off) {
  void *res;
  BLOCK_SIGNALS;
  BLOCK_CANCELATION;
  res = __mmap(addr, size, prot, flags, fd, off);
  ALLOW_CANCELATION;
  ALLOW_SIGNALS;
  STRACE("mmap(%p, %'zu, %s, %s, %d, %'ld) → %p% m", addr, size,
         DescribeProtFlags(prot), DescribeMapFlags(flags), fd, off, res);
  return res;
}

int munmap(void *addr, size_t size) {
  int rc;
  BLOCK_SIGNALS;
  BLOCK_CANCELATION;
  rc = __munmap(addr, size, false);
  ALLOW_CANCELATION;
  ALLOW_SIGNALS;
  STRACE("munmap(%p, %'zu) → %d% m", addr, size, rc);
  return rc;
}

__weak_reference(mmap, mmap64);
