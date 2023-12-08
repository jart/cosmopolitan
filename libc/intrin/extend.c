/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/struct/sigset.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/asancodes.h"
#include "libc/intrin/describebacktrace.internal.h"
#include "libc/intrin/directmap.internal.h"
#include "libc/log/libfatal.internal.h"
#include "libc/macros.internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"

#define G FRAMESIZE

static void *_mapframe_impl(void *p, int f) {
  int rc, prot, flags;
  struct DirectMap dm;
  prot = PROT_READ | PROT_WRITE;
  flags = f | MAP_ANONYMOUS | MAP_FIXED;
  if ((dm = sys_mmap(p, G, prot, flags, -1, 0)).addr == p) {
    rc = __track_memory(&_mmi, (uintptr_t)p >> 16, (uintptr_t)p >> 16,
                        dm.maphandle, prot, flags, false, false, 0, G);
    if (!rc) {
      return p;
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

static void *_mapframe(void *p, int f) {
  void *res;
  // mmap isn't required to be @asyncsignalsafe but this is
  BLOCK_SIGNALS;
  __mmi_lock();
  res = _mapframe_impl(p, f);
  __mmi_unlock();
  ALLOW_SIGNALS;
  return res;
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
 * @param f should be `MAP_PRIVATE` or `MAP_SHARED`
 * @return new value for `e` or null w/ errno
 * @raise ENOMEM if we require more vespene gas
 * @asyncsignalsafe
 */
void *_extend(void *p, size_t n, void *e, int f, intptr_t h) {
  char *q;
#ifndef NDEBUG
  if ((uintptr_t)SHADOW(p) & (G - 1)) notpossible;
  if ((uintptr_t)p + (G << kAsanScale) > h) notpossible;
#endif
  // TODO(jart): Make this spin less in non-ASAN mode.
  for (q = e; q < ((char *)p + n); q += 8) {
    if (!((uintptr_t)q & (G - 1))) {
#ifndef NDEBUG
      if (q + G > (char *)h) notpossible;
#endif
      if (!_mapframe(q, f)) return 0;
      if (IsAsan()) {
        if (!((uintptr_t)SHADOW(q) & (G - 1))) {
          if (!_mapframe(SHADOW(q), f)) return 0;
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
