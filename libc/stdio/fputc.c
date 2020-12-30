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
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"

static noinline int __fputc(int c, FILE *f) {
  if (f->beg < f->size) {
    f->buf[f->beg++] = c;
    if (f->beg == f->size || f->bufmode == _IONBF ||
        (f->bufmode == _IOLBF && c == '\n')) {
      if (f->writer) {
        if (f->writer(f) == -1) return -1;
      } else if (f->beg == f->size) {
        f->beg = 0;
      }
    }
    return c & 0xff;
  } else {
    return __fseteof(f);
  }
}

/**
 * Writes byte to stream.
 * @return c (as unsigned char) if written or -1 w/ errno
 * @see putc() if called within loop
 */
noinstrument int fputc(int c, FILE *f) {
  if (f->beg + 1 < f->size && f->bufmode == _IOFBF) {
    f->buf[f->beg++] = c;
    return c & 0xff;
  } else {
    return __fputc(c, f);
  }
}
