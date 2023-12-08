/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=8 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/struct/iovec.internal.h"
#include "libc/dce.h"
#include "libc/fmt/internal.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/nt/files.h"
#include "libc/sock/sock.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

struct VdprintfState {
  int n, t, fd;
  char b[1024];
};

static int vdprintf_putc(const char *s, struct VdprintfState *t, size_t n) {
  struct iovec iov[2];
  if (n) {
    if (t->n + n < sizeof(t->b)) {
      memcpy(t->b + t->n, s, n);
      t->n += n;
    } else {
      iov[0].iov_base = t->b;
      iov[0].iov_len = t->n;
      iov[1].iov_base = (void *)s;
      iov[1].iov_len = n;
      if (__robust_writev(t->fd, iov, 2) == -1) {
        return -1;
      }
      t->t += t->n;
      t->n = 0;
    }
  }
  return 0;
}

/**
 * Formats string directly to system i/o device.
 * @asyncsignalsafe
 * @vforksafe
 */
int vdprintf(int fd, const char *fmt, va_list va) {
  struct iovec iov[1];
  struct VdprintfState t;
  t.n = 0;
  t.t = 0;
  t.fd = fd;
  if (__fmt(vdprintf_putc, &t, fmt, va) == -1) return -1;
  if (t.n) {
    iov[0].iov_base = t.b;
    iov[0].iov_len = t.n;
    if (__robust_writev(t.fd, iov, 1) == -1) {
      return -1;
    }
    t.t += t.n;
  }
  return t.t;
}
