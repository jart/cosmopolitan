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
#include "libc/fmt/internal.h"
#include "libc/stdckdint.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/sysparam.h"
#include "libc/sysv/errfuns.h"

struct state {
  FILE *f;
  size_t count;
  size_t pending;
  size_t flushed;
  char buf[512];
};

static int vfprintf_buffer(const char *s, struct state *t, size_t n) {
  size_t i, copy;
  t->count += n;
  for (i = 0; i < n; i += copy) {
    copy = MIN(n - i, sizeof(t->buf) - t->pending);
    memcpy(t->buf + t->pending, s + i, copy);
    if ((t->pending += copy) == sizeof(t->buf)) {
      t->flushed += fwrite_unlocked(t->buf, 1, sizeof(t->buf), t->f);
      t->pending = 0;
    }
  }
  return 0;
}

/**
 * Formats and writes text to stream.
 * @see printf() for further documentation
 * @cancelationpoint
 */
int vfprintf_unlocked(FILE *f, const char *fmt, va_list va) {
  int res;
  struct state st;
  st.f = f;
  st.count = 0;
  st.pending = 0;
  st.flushed = 0;
  if (__fmt(vfprintf_buffer, &st, fmt, va, &st.count) == -1)
    return -1;
  if (st.pending)
    st.flushed += fwrite_unlocked(st.buf, 1, st.pending, f);
  if (st.flushed != st.count) {
    if (!st.flushed)
      return -1;
    if (f->fd == -1)
      errno = ENOSPC;
  }
  if (ckd_add(&res, st.count, 0))
    return eoverflow();
  return res;
}
