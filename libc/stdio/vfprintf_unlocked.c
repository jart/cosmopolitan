/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=8 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/fmt/fmt.h"
#include "libc/limits.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/errfuns.h"

struct state {
  FILE *f;
  int n;
};

static int vfprintfputchar(const char *s, struct state *t, size_t n) {
  int rc;
  if (n) {
    if (n == 1 && *s != '\n' && t->f->beg < t->f->size &&
        t->f->bufmode != _IONBF) {
      t->f->buf[t->f->beg++] = *s;
      t->n += n;
      rc = 0;
    } else if (!fwrite_unlocked(s, 1, n, t->f)) {
      rc = -1;
    } else {
      t->n += n;
      rc = 0;
    }
  } else {
    rc = 0;
  }
  return 0;
}

/**
 * Formats and writes text to stream.
 * @see printf() for further documentation
 */
int(vfprintf_unlocked)(FILE *f, const char *fmt, va_list va) {
  int rc;
  struct state st[1] = {{f, 0}};
  if ((rc = __fmt(vfprintfputchar, st, fmt, va)) != -1) {
    rc = st->n;
  }
  return rc;
}
