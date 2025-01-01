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
#include "libc/dce.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/bsr.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/maps.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/auxv.h"

// this will usually return 12 since x86 pml4t uses a 47 bit address
// space in userspace, and decent arm machines uses a 48 bit address
// space. however it could go lower on embedded devices. it can also
// rise higher on expensive x86 machines with pml5t, if user uses it
static int get_address_digits(int pagesz) {
  int max_bits = 0;
  for (struct Tree *e = tree_first(__maps.maps); e; e = tree_next(e)) {
    struct Map *map = MAP_TREE_CONTAINER(e);
    char *end = map->addr + ((map->size + pagesz - 1) & -pagesz);
    int bits = bsrll((uintptr_t)end) + 1;
    if (bits > max_bits)
      max_bits = bits;
  }
  return ((max_bits + 3) & -4) / 4;
}

/**
 * Prints memory mappings known to cosmo.
 */
void __print_maps(size_t limit) {
  __maps_lock();
  char sb[16];
  char mappingbuf[8];
  struct Map *last = 0;
  int pagesz = __pagesize;
  int gransz = __gransize;
  int digs = get_address_digits(pagesz);
  for (struct Tree *e = tree_first(__maps.maps); e; e = tree_next(e)) {
    struct Map *map = MAP_TREE_CONTAINER(e);

    // show gaps between maps
    if (last) {
      char *beg = last->addr + ((last->size + gransz - 1) & -gransz);
      char *end = map->addr;
      if (end > beg) {
        size_t gap = end - beg;
        sizefmt(sb, gap, 1024);
        kprintf("%0*lx-%0*lx       %sb\n", digs, beg, digs, end, sb);
      }
    }
    last = map;

    // show mapping
    kprintf("%0*lx-%0*lx %!s", digs, map->addr, digs, map->addr + map->size,
            _DescribeMapping(mappingbuf, map->prot, map->flags));
    sizefmt(sb, map->size, 1024);
    kprintf(" %!sb", sb);
    if (IsWindows()) {
      switch (map->hand) {
        case MAPS_RESERVATION:
          kprintf(" reservation");
          break;
        case MAPS_SUBREGION:
          break;
        case MAPS_VIRTUAL:
          kprintf(" virtual");
          break;
        default:
          kprintf(" hand=%ld", map->hand);
          break;
      }
    }
    if (map->iscow)
      kprintf(" cow");
    if (map->readonlyfile)
      kprintf(" readonlyfile");
    kprintf("\n");

    // stay beneath our limit
    if (!--limit)
      break;
  }

  // print summary
  kprintf("# %'zu bytes in %'zu mappings\n", __maps.pages * pagesz,
          __maps.count);
  __maps_unlock();
}
