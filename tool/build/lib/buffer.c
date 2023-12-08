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
#include "tool/build/lib/buffer.h"
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/macros.internal.h"
#include "libc/mem/arraylist2.internal.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

/* TODO(jart): replace with new append*() library */

void AppendData(struct Buffer *b, const char *data, size_t len) {
  char *p;
  unsigned n;
  if (b->i + len + 1 > b->n) {
    n = MAX(b->i + len + 1, MAX(16, b->n + (b->n >> 1)));
    if (!(p = realloc(b->p, n))) return;
    b->p = p;
    b->n = n;
  }
  memcpy(b->p + b->i, data, len);
  b->p[b->i += len] = 0;
}

void AppendChar(struct Buffer *b, char c) {
  AppendData(b, &c, 1);
}

void AppendStr(struct Buffer *b, const char *s) {
  AppendData(b, s, strlen(s));
}

void AppendWide(struct Buffer *b, wint_t wc) {
  unsigned i;
  uint64_t wb;
  char buf[8];
  i = 0;
  wb = tpenc(wc);
  do {
    buf[i++] = wb & 0xFF;
    wb >>= 8;
  } while (wb);
  AppendData(b, buf, i);
}

int AppendFmt(struct Buffer *b, const char *fmt, ...) {
  int n;
  va_list va, vb;
  va_start(va, fmt);
  va_copy(vb, va);
  n = vsnprintf(b->p + b->i, b->n - b->i, fmt, va);
  if (b->i + n + 1 > b->n) {
    do {
      if (b->n) {
        b->n += b->n >> 1;
      } else {
        b->n = 16;
      }
    } while (b->i + n + 1 > b->n);
    b->p = realloc(b->p, b->n);
    vsnprintf(b->p + b->i, b->n - b->i, fmt, vb);
  }
  va_end(vb);
  va_end(va);
  b->i += n;
  return n;
}

/**
 * Writes buffer until completion, or error occurs.
 */
ssize_t WriteBuffer(struct Buffer *b, int fd) {
  bool t;
  char *p;
  ssize_t rc;
  size_t wrote, n;
  p = b->p;
  n = b->i;
  t = false;
  do {
    if ((rc = write(fd, p, n)) != -1) {
      wrote = rc;
      p += wrote;
      n -= wrote;
    } else if (errno == EINTR) {
      t = true;
    } else {
      return -1;
    }
  } while (n);
  if (!t) {
    return 0;
  } else {
    errno = EINTR;
    return -1;
  }
}
