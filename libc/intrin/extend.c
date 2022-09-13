/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/asancodes.h"
#include "libc/macros.internal.h"
#include "libc/runtime/directmap.internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"

#define G FRAMESIZE

static void _mapframe(void *p) {
  int prot, flags;
  struct DirectMap dm;
  prot = PROT_READ | PROT_WRITE;
  flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED;
  if ((dm = sys_mmap(p, G, prot, flags, -1, 0)).addr != p) {
    notpossible;
  }
  __mmi_lock();
  if (TrackMemoryInterval(&_mmi, (uintptr_t)p >> 16, (uintptr_t)p >> 16,
                          dm.maphandle, prot, flags, false, false, 0, G)) {
    notpossible;
  }
  __mmi_unlock();
}

/**
 * Extends static allocation.
 *
 * This simple fixed allocator has unusual invariants
 *
 *     !(p & 0xffff) && !(((p >> 3) + 0x7fff8000) & 0xffff)
 *
 * which must be the case when selecting a starting address. We also
 * make the assumption that allocations can only grow monotonically.
 * Furthermore allocations shall never be removed or relocated.
 *
 * @param p points to start of memory region
 * @param n specifies how many bytes are needed
 * @param e points to end of memory that's allocated
 * @param h is highest address to which `e` may grow
 * @return new value for `e`
 */
noasan void *_extend(void *p, size_t n, void *e, intptr_t h) {
  char *q;
#ifndef NDEBUG
  if ((uintptr_t)SHADOW(p) & (G - 1)) notpossible;
  if ((uintptr_t)p + (G << kAsanScale) > h) notpossible;
#endif
  for (q = e; q < ((char *)p + n); q += 8) {
    if (!((uintptr_t)q & (G - 1))) {
      if (q + G > (char *)h) notpossible;
      _mapframe(q);
      if (IsAsan()) {
        if (!((uintptr_t)SHADOW(q) & (G - 1))) {
          _mapframe(SHADOW(q));
          __asan_poison(q, G << kAsanScale, kAsanProtected);
        }
      }
    }
    if (IsAsan()) {
      *SHADOW(q) = 0;
    }
  }
  return q;
}
