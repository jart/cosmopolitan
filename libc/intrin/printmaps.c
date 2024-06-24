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
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/maps.h"
#include "libc/macros.internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/auxv.h"

/**
 * Prints memory mappings.
 */
void __print_maps(void) {
  int limit = 13;
  long maptally = 0;
  char mappingbuf[8], sb[16];
  __maps_lock();
  struct Dll *e, *e2;
  for (e = dll_first(__maps.used); e; e = e2) {
    e2 = dll_next(__maps.used, e);
    struct Map *map = MAP_CONTAINER(e);
    maptally += map->size;
    kprintf("%012lx-%012lx %!s", map->addr, map->addr + map->size,
            (DescribeMapping)(mappingbuf, map->prot, map->flags));
    sizefmt(sb, map->size, 1024);
    kprintf(" %!sb", sb);
    if (map->h && map->h != -1)
      kprintf(" h=%ld", map->h);
    if (map->iscow)
      kprintf(" cow");
    if (map->readonlyfile)
      kprintf(" readonlyfile");
    kprintf("\n");
    if (!--limit)
      break;
  }
  kprintf("# %'zu bytes in %'zu mappings\n",
          __maps.pages * getauxval(AT_PAGESZ), __maps.count);
  __maps_unlock();
}
