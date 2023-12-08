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
#include "libc/log/check.h"
#include "tool/plinko/lib/print.h"
#include "tool/plinko/lib/types.h"

int PrintInt(int fd, long x, int cols, char quot, char zero, int base,
             bool issigned) {
  dword y;
  char z[32];
  int i, j, k, n;
  DCHECK_LE(base, 36);
  i = j = 0;
  y = x < 0 && issigned ? -x : x;
  do {
    if (quot && j == 3) z[i++ & 31] = quot, j = 0;
    z[i++ & 31] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"[y % base];
  } while (++j, (y /= base));
  k = i + (x < 0 && issigned);
  if (zero) {
    n = PrintZeroes(fd, +cols - k);
  } else {
    n = PrintIndent(fd, +cols - k);
  }
  if (x < 0 && issigned) n += PrintChar(fd, L'-');
  while (i) n += PrintChar(fd, z[--i & 31]);
  PrintIndent(fd, -cols - n);
  return n;
}
