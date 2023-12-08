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
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/thompike.h"
#include "libc/str/tpdecodecb.internal.h"

/**
 * Reads UTF-8 character from stream.
 * @return wide character or -1 on EOF or error
 */
wint_t fgetwc_unlocked(FILE *f) {
  int c, n;
  wint_t b, x, y;
  if (f->beg < f->end) {
    b = f->buf[f->beg++] & 0xff;
  } else if ((c = fgetc_unlocked(f)) != -1) {
    b = c;
  } else {
    return -1;
  }
  if (b < 0300) return b;
  n = ThomPikeLen(b);
  x = ThomPikeByte(b);
  while (--n) {
    if ((c = fgetc_unlocked(f)) == -1) return -1;
    y = c;
    if (ThomPikeCont(y)) {
      x = ThomPikeMerge(x, y);
    } else {
      ungetc_unlocked(y, f);
      return b;
    }
  }
  return x;
}

__strong_reference(fgetwc_unlocked, getwc_unlocked);
