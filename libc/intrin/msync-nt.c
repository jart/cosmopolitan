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
#include "libc/calls/syscall-nt.internal.h"
#include "libc/intrin/maps.h"
#include "libc/nt/memory.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/sysparam.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/errfuns.h"

textwindows int sys_msync_nt(char *addr, size_t size, int flags) {

  int pagesz = __pagesize;
  size = (size + pagesz - 1) & -pagesz;

  if ((uintptr_t)addr & (pagesz - 1))
    return einval();

  int rc = 0;
  if (__maps_lock()) {
    rc = edeadlk();
  } else {
    struct Map *map, *floor;
    floor = __maps_floor(addr);
    for (map = floor; map && map->addr <= addr + size; map = __maps_next(map)) {
      char *beg = MAX(addr, map->addr);
      char *end = MIN(addr + size, map->addr + map->size);
      if (beg < end)
        if (!FlushViewOfFile(beg, end - beg))
          rc = -1;
      // TODO(jart): FlushFileBuffers too on g_fds handle if MS_SYNC?
    }
  }
  __maps_unlock();

  return rc;
}
