/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/atomic.h"
#include "libc/macros.h"
#include "libc/str/str.h"

// really want to avoid locking here so close() needn't block signals
void __releasefd(int fd) {
  int f1, f2;
  if (!(0 <= fd && fd < g_fds.n))
    return;
  g_fds.p[fd].kind = kFdEmpty;
  bzero(g_fds.p + fd, sizeof(*g_fds.p));
  f1 = atomic_load_explicit(&g_fds.f, memory_order_relaxed);
  do {
    f2 = MIN(fd, f1);
  } while (!atomic_compare_exchange_weak_explicit(
      &g_fds.f, &f1, f2, memory_order_release, memory_order_relaxed));
}
