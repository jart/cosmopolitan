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
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/fmt/internal.h"
#include "libc/stdckdint.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/sysparam.h"
#include "libc/sysv/errfuns.h"

struct state {
  int fd;
  size_t count;
  size_t wrote;
  size_t pending;
  char buf[512];
};

static int vdprintf_buffer(const char *s, struct state *t, size_t n) {
  ssize_t rc;
  size_t i, j, copy;
  t->count += n;
  for (i = 0; i < n; i += copy) {
    copy = MIN(n - i, sizeof(t->buf) - t->pending);
    memcpy(t->buf + t->pending, s + i, copy);
    if ((t->pending += copy) == sizeof(t->buf)) {
      for (j = 0; j < sizeof(t->buf); j += rc) {
        if ((rc = write(t->fd, t->buf + j, sizeof(t->buf) - j)) == -1)
          return -1;
        t->wrote += rc;
      }
      t->pending = 0;
    }
  }
  return 0;
}

/**
 * Formats string directly to file descriptor.
 *
 * @cancelationpoint
 * @asyncsignalsafe
 * @vforksafe
 */
int vdprintf(int fd, const char *fmt, va_list va) {
  int res;
  size_t i;
  ssize_t rc;
  struct state st;
  st.fd = fd;
  st.count = 0;
  st.pending = 0;
  st.wrote = 0;
  if (__fmt(vdprintf_buffer, &st, fmt, va, &st.count) == -1) {
  OnError:
    if (st.wrote)
      if (errno != ECANCELED)
        return st.wrote;
    return -1;
  }
  for (i = 0; i < st.pending; i += rc) {
    if ((rc = write(fd, st.buf + i, st.pending - i)) == -1)
      goto OnError;
    st.wrote += rc;
  }
  if (ckd_add(&res, st.wrote, 0))
    return eoverflow();
  return res;
}
