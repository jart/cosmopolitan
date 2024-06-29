/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/directmap.internal.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/maps.h"
#include "libc/intrin/strace.internal.h"
#include "libc/nt/memory.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/sysparam.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/errfuns.h"

#define PGUP(x) (((x) + pagesz - 1) & -pagesz)

static int __mprotect_chunk(char *addr, size_t size, int prot, bool iscow) {

  if (!IsWindows())
    return sys_mprotect(addr, size, prot);

  uint32_t op;
  if (!VirtualProtect(addr, size, __prot2nt(prot, iscow), &op))
    return -1;

  return 0;
}

int __mprotect(char *addr, size_t size, int prot) {

  // unix checks prot before checking size
  if (prot & ~(PROT_READ | PROT_WRITE | PROT_EXEC))
    return einval();

  // make new technology consistent with unix
  if (!size)
    return 0;

  // unix checks prot before checking size
  int pagesz = getauxval(AT_PAGESZ);
  if ((intptr_t)addr & (pagesz - 1))
    return einval();

  // change mappings
  int rc = 0;
  __maps_lock();
  bool found = false;
  struct Map *map = __maps.maps;
  struct Map **prev = &__maps.maps;
  while (map) {
    char *map_addr = map->addr;
    size_t map_size = map->size;
    struct Map *next = map->next;
    char *beg = MAX(addr, map_addr);
    char *end = MIN(addr + PGUP(size), map_addr + PGUP(map_size));
    if (beg < end) {
      found = true;
      if (addr <= map_addr && addr + PGUP(size) >= map_addr + PGUP(map_size)) {
        // change protection of entire mapping
        if (!__mprotect_chunk(map_addr, map_size, prot, map->iscow)) {
          map->prot = prot;
        } else {
          rc = -1;
        }
      } else if (IsWindows()) {
        // windows does allow changing protection at 4096 byte chunks
        // however we currently don't have data structures that track
        // this within the 64 kb map granules that can't be broken up
        if (__mprotect_chunk(beg, end - beg, prot, map->iscow) == -1)
          rc = -1;
      } else if (addr <= map_addr) {
        // cleave lefthand side of mapping
        size_t left = PGUP(addr + size - map_addr);
        size_t right = map_size - left;
        struct Map *leftmap;
        if ((leftmap = __maps_alloc())) {
          if (!__mprotect_chunk(map_addr, left, prot, false)) {
            leftmap->next = map;
            leftmap->addr = map_addr;
            leftmap->size = left;
            leftmap->prot = prot;
            leftmap->off = map->off;
            leftmap->flags = map->flags;
            map->addr += left;
            map->size = right;
            if (!(map->flags & MAP_ANONYMOUS))
              map->off += left;
            dll_make_first(&__maps.used, &leftmap->elem);
            *prev = leftmap;
            __maps.count += 1;
            __maps_check();
          } else {
            __maps_free(leftmap);
            rc = -1;
          }
        } else {
          rc = -1;
        }
      } else if (addr + PGUP(size) >= map_addr + PGUP(map_size)) {
        // cleave righthand side of mapping
        size_t left = addr - map_addr;
        size_t right = map_addr + map_size - addr;
        struct Map *leftmap;
        if ((leftmap = __maps_alloc())) {
          if (!__mprotect_chunk(map_addr + left, right, prot, false)) {
            leftmap->next = map;
            leftmap->addr = map_addr;
            leftmap->size = left;
            leftmap->off = map->off;
            leftmap->prot = map->prot;
            leftmap->flags = map->flags;
            map->addr += left;
            map->size = right;
            map->prot = prot;
            if (!(map->flags & MAP_ANONYMOUS))
              map->off += left;
            dll_make_first(&__maps.used, &leftmap->elem);
            *prev = leftmap;
            __maps.count += 1;
            __maps_check();
          } else {
            __maps_free(leftmap);
            rc = -1;
          }
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
          struct Map *midlmap;
          if ((midlmap = __maps_alloc())) {
            if (!__mprotect_chunk(map_addr + left, middle, prot, false)) {
              leftmap->next = midlmap;
              leftmap->addr = map_addr;
              leftmap->size = left;
              leftmap->off = map->off;
              leftmap->prot = map->prot;
              leftmap->flags = map->flags;
              midlmap->next = map;
              midlmap->addr = map_addr + left;
              midlmap->size = middle;
              midlmap->off = (map->flags & MAP_ANONYMOUS) ? 0 : map->off + left;
              midlmap->prot = prot;
              midlmap->flags = map->flags;
              map->addr += left + middle;
              map->size = right;
              if (!(map->flags & MAP_ANONYMOUS))
                map->off += left + middle;
              dll_make_first(&__maps.used, &leftmap->elem);
              dll_make_first(&__maps.used, &midlmap->elem);
              *prev = leftmap;
              __maps.count += 2;
              __maps_check();
            } else {
              __maps_free(midlmap);
              __maps_free(leftmap);
              rc = -1;
            }
          } else {
            __maps_free(leftmap);
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

  // allow user to change mappings unknown to cosmo runtime
  if (!found)
    rc = __mprotect_chunk(addr, size, prot, false);

  __maps_unlock();
  return rc;
}

/**
 * Modifies restrictions on virtual memory address range.
 *
 * @param addr needs to be 4kb aligned
 * @param prot can have PROT_{NONE,READ,WRITE,EXEC}
 * @return 0 on success, or -1 w/ errno
 * @raise ENOMEM on tracking memory oom
 * @see mmap()
 */
int mprotect(void *addr, size_t size, int prot) {
  int rc;
  rc = __mprotect(addr, size, prot);
  STRACE("mprotect(%p, %'zu, %s) → %d% m", addr, size, DescribeProtFlags(prot),
         rc);
  return rc;
}
