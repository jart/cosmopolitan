/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/bsr.h"
#include "tool/plinko/lib/char.h"
#include "tool/plinko/lib/ktpenc.h"
#include "tool/plinko/lib/plinko.h"

int PrintChar(int fd, int s) {
  unsigned c;
  int d, e, i, n;
  c = s & 0xffffffff;
  if (bp[fd] + 6 > sizeof(g_buffer[fd])) Flush(fd);
  if (c < 0200) {
    g_buffer[fd][bp[fd]++] = c;
    if (c == L'\n') Flush(fd);
  } else {
    d = c;
    e = kTpEnc[_bsrl(d) - 7];
    i = n = e & 255;
    do g_buffer[fd][bp[fd] + i--] = 0200 | (d & 077);
    while (d >>= 6, i);
    g_buffer[fd][bp[fd]] = d | e >> 8;
    bp[fd] += n + 1;
  }
  return GetMonospaceCharacterWidth(c);
}
