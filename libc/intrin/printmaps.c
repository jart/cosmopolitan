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
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/maps.h"
#include "libc/macros.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/auxv.h"

/**
 * Prints memory mappings.
 */
void __print_maps(size_t limit) {
  char mappingbuf[8], sb[16];
  __maps_lock();
  for (struct Tree *e = tree_first(__maps.maps); e; e = tree_next(e)) {
    struct Map *map = MAP_TREE_CONTAINER(e);
    kprintf("%012lx-%012lx %!s", map->addr, map->addr + map->size,
            _DescribeMapping(mappingbuf, map->prot, map->flags));
    sizefmt(sb, map->size, 1024);
    kprintf(" %!sb", sb);
    if (map->hand && map->hand != -1)
      kprintf(" hand=%ld", map->hand);
    if (map->iscow)
      kprintf(" cow");
    if (map->readonlyfile)
      kprintf(" readonlyfile");
    kprintf("\n");
    if (!--limit)
      break;
  }
  kprintf("# %'zu bytes in %'zu mappings\n", __maps.pages * __pagesize,
          __maps.count);
  __maps_unlock();
}
