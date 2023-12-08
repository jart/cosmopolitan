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
#include "libc/sysv/errfuns.h"

struct buf {
  int n;
  char p[512];
};

struct state {
  FILE *f;
  int n;
  struct buf b;
};

static int __vfprintf_flbuf(const char *s, struct state *t, size_t n) {
  int rc;
  if (n) {
    if (n == 1 && *s != '\n' && t->f->beg < t->f->size) {
      t->f->buf[t->f->beg++] = *s;
      rc = 0;
    } else if (fwrite_unlocked(s, 1, n, t->f)) {
      rc = 0;
    } else {
      rc = -1;
    }
    if (ckd_add(&t->n, t->n, n)) {
      rc = eoverflow();
    }
  } else {
    rc = 0;
  }
  return rc;
}

static int __vfprintf_nbuf(const char *s, struct state *t, size_t n) {
  size_t i;
  for (i = 0; i < n; ++i) {
    t->b.p[t->b.n++] = s[i];
    if (t->b.n == sizeof(t->b.p)) {
      if (!fwrite_unlocked(t->b.p, 1, t->b.n, t->f)) {
        return -1;
      }
      t->b.n = 0;
    } else if (ckd_add(&t->n, t->n, 1)) {
      return eoverflow();
    }
  }
  return 0;
}

/**
 * Formats and writes text to stream.
 * @see printf() for further documentation
 */
int vfprintf_unlocked(FILE *f, const char *fmt, va_list va) {
  int rc;
  struct state st;
  int (*out)(const char *, struct state *, size_t);
  if (f->bufmode != _IONBF) {
    out = __vfprintf_flbuf;
  } else {
    out = __vfprintf_nbuf;
  }
  st.f = f;
  st.n = 0;
  st.b.n = 0;
  if ((rc = __fmt(out, &st, fmt, va)) != -1) {
    if (!st.b.n) {
      rc = st.n;
    } else if (fwrite_unlocked(st.b.p, 1, st.b.n, st.f)) {
      if (ckd_add(&rc, st.n, st.b.n)) {
        rc = eoverflow();
      }
    } else {
      rc = -1;
    }
  }
  return rc;
}
