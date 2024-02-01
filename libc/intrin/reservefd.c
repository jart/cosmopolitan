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
#include "libc/calls/internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/cmpxchg.h"
#include "libc/intrin/extend.internal.h"
#include "libc/macros.internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"

/**
 * Grows file descriptor array memory if needed.
 *
 * @see libc/runtime/memtrack64.txt
 * @see libc/runtime/memtrack32.txt
 * @asyncsignalsafe
 */
int __ensurefds_unlocked(int fd) {
  size_t n;
  if (fd < g_fds.n) return fd;
  n = fd + 1;
  g_fds.e = _extend(g_fds.p, n * sizeof(*g_fds.p), g_fds.e, MAP_PRIVATE,
                    kMemtrackFdsStart + kMemtrackFdsSize);
  g_fds.n = n;
  return fd;
}

/**
 * Grows file descriptor array memory if needed.
 * @asyncsignalsafe
 */
int __ensurefds(int fd) {
  __fds_lock();
  fd = __ensurefds_unlocked(fd);
  __fds_unlock();
  return fd;
}

/**
 * Finds open file descriptor slot.
 * @asyncsignalsafe
 */
int __reservefd_unlocked(int start) {
  int fd, f1, f2;
  for (;;) {
    f1 = atomic_load_explicit(&g_fds.f, memory_order_acquire);
    for (fd = MAX(start, f1); fd < g_fds.n; ++fd) {
      if (!g_fds.p[fd].kind) {
        break;
      }
    }
    fd = __ensurefds_unlocked(fd);
    bzero(g_fds.p + fd, sizeof(*g_fds.p));
    if (_cmpxchg(&g_fds.p[fd].kind, kFdEmpty, kFdReserved)) {
      // g_fds.f isn't guarded by our mutex
      do {
        f2 = MAX(fd + 1, f1);
      } while (!atomic_compare_exchange_weak_explicit(
          &g_fds.f, &f1, f2, memory_order_release, memory_order_relaxed));
      return fd;
    }
  }
}

/**
 * Finds open file descriptor slot.
 * @asyncsignalsafe
 */
int __reservefd(int start) {
  int fd;
  __fds_lock();
  fd = __reservefd_unlocked(start);
  __fds_unlock();
  return fd;
}
