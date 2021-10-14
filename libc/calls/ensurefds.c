/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/assert.h"
#include "libc/bits/bits.h"
#include "libc/bits/weaken.h"
#include "libc/calls/internal.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

static void __ensurefds_destroy(void) {
  weaken(free)(g_fds.p);
}

int __ensurefds(int fd) {
  size_t n1, n2;
  struct Fd *p1, *p2;
  for (;;) {
    p1 = g_fds.p;
    n1 = g_fds.n;
    if (fd < n1) return fd;
    if (weaken(malloc)) {
      n2 = MAX(fd + 1, n1 + (n1 << 1));
      if ((p2 = weaken(malloc)(n2 * sizeof(*p1)))) {
        memcpy(p2, p1, n1 * sizeof(*p1));
        bzero(p2 + n1, (n2 - n1) * sizeof(*p1));
        if (cmpxchg(&g_fds.p, p1, p2)) {
          g_fds.n = n2;
          if (weaken(free)) {
            if (p1 == g_fds.__init_p) {
              atexit(__ensurefds_destroy);
            } else {
              weaken(free)(p1);
            }
          }
          return fd;
        } else if (weaken(free)) {
          weaken(free)(p2);
        }
      } else {
        return enomem();
      }
    } else {
      return emfile();
    }
  }
}
