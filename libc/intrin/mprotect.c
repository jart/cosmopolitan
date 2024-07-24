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
#include "libc/intrin/describeflags.h"
#include "libc/intrin/directmap.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/maps.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/tree.h"
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
  if (prot & ~(PROT_READ | PROT_WRITE | PROT_EXEC | PROT_GUARD))
    return einval();

  // make new technology consistent with unix
  if (!size)
    return 0;

  // unix checks prot before checking size
  int pagesz = __pagesize;
  if (((intptr_t)addr & (pagesz - 1)) || (uintptr_t)addr + size < size)
    return einval();

  // normalize size
  size = (size + pagesz - 1) & -pagesz;

  // change mappings
  int rc = 0;
  bool found = false;
  if (__maps_lock()) {
    __maps_unlock();
    return edeadlk();
  }
  struct Map *map, *floor;
StartOver:
  floor = __maps_floor(addr);
  for (map = floor; map && map->addr <= addr + size; map = __maps_next(map)) {
    char *map_addr = map->addr;
    size_t map_size = map->size;
    char *beg = MAX(addr, map_addr);
    char *end = MIN(addr + size, map_addr + PGUP(map_size));
    if (beg >= end)
      continue;
    found = true;
    if (addr <= map_addr && addr + size >= map_addr + PGUP(map_size)) {
      // change protection of entire mapping
      if (!__mprotect_chunk(map_addr, map_size, prot, map->iscow)) {
        map->prot = prot;
      } else {
        rc = -1;
      }
    } else if (addr <= map_addr) {
      // change lefthand side of mapping
      size_t left = addr + size - map_addr;
      size_t right = map_size - left;
      struct Map *leftmap;
      if ((leftmap = __maps_alloc())) {
        if (leftmap == MAPS_RETRY)
          goto StartOver;
        if (!__mprotect_chunk(map_addr, left, prot, false)) {
          leftmap->addr = map_addr;
          leftmap->size = left;
          leftmap->prot = prot;
          leftmap->off = map->off;
          leftmap->flags = map->flags;
          leftmap->iscow = map->iscow;
          leftmap->readonlyfile = map->readonlyfile;
          leftmap->hand = map->hand;
          map->addr += left;
          map->size = right;
          map->hand = -1;
          if (!(map->flags & MAP_ANONYMOUS))
            map->off += left;
          tree_insert(&__maps.maps, &leftmap->tree, __maps_compare);
          __maps.count += 1;
          __maps_check();
        } else {
          __maps_free(leftmap);
          rc = -1;
        }
      } else {
        rc = -1;
      }
    } else if (addr + size >= map_addr + PGUP(map_size)) {
      // change righthand side of mapping
      size_t left = addr - map_addr;
      size_t right = map_addr + map_size - addr;
      struct Map *leftmap;
      if ((leftmap = __maps_alloc())) {
        if (leftmap == MAPS_RETRY)
          goto StartOver;
        if (!__mprotect_chunk(map_addr + left, right, prot, false)) {
          leftmap->addr = map_addr;
          leftmap->size = left;
          leftmap->off = map->off;
          leftmap->prot = map->prot;
          leftmap->flags = map->flags;
          leftmap->iscow = map->iscow;
          leftmap->readonlyfile = map->readonlyfile;
          leftmap->hand = map->hand;
          map->addr += left;
          map->size = right;
          map->prot = prot;
          map->hand = -1;
          if (!(map->flags & MAP_ANONYMOUS))
            map->off += left;
          tree_insert(&__maps.maps, &leftmap->tree, __maps_compare);
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
      // change middle of mapping
      size_t left = addr - map_addr;
      size_t middle = size;
      size_t right = map_size - middle - left;
      struct Map *leftmap;
      if ((leftmap = __maps_alloc())) {
        if (leftmap == MAPS_RETRY)
          goto StartOver;
        struct Map *midlmap;
        if ((midlmap = __maps_alloc())) {
          if (midlmap == MAPS_RETRY) {
            __maps_free(leftmap);
            goto StartOver;
          }
          if (!__mprotect_chunk(map_addr + left, middle, prot, false)) {
            leftmap->addr = map_addr;
            leftmap->size = left;
            leftmap->off = map->off;
            leftmap->prot = map->prot;
            leftmap->flags = map->flags;
            leftmap->iscow = map->iscow;
            leftmap->readonlyfile = map->readonlyfile;
            leftmap->hand = map->hand;
            midlmap->addr = map_addr + left;
            midlmap->size = middle;
            midlmap->off = (map->flags & MAP_ANONYMOUS) ? 0 : map->off + left;
            midlmap->prot = prot;
            midlmap->flags = map->flags;
            midlmap->hand = -1;
            map->addr += left + middle;
            map->size = right;
            map->hand = -1;
            if (!(map->flags & MAP_ANONYMOUS))
              map->off += left + middle;
            tree_insert(&__maps.maps, &leftmap->tree, __maps_compare);
            tree_insert(&__maps.maps, &midlmap->tree, __maps_compare);
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
