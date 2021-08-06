/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/bits/bits.h"
#include "libc/nexgen32e/bsr.h"
#include "libc/stdio/append.internal.h"

/**
 * Appends character or word-encoded string to buffer.
 *
 * Up to eight characters can be appended. For example:
 *
 *     appendw(&s, 'h'|'i'<<8);
 *
 * Is equivalent to:
 *
 *     appends(&s, "hi");
 *
 * The special case:
 *
 *     appendw(&s, 0);
 *
 * Will append a single NUL character.
 *
 * The resulting buffer is guaranteed to be NUL-terminated, i.e.
 * `!b[appendz(b).i]` will be the case.
 *
 * @return bytes appended or -1 if `ENOMEM`
 */
ssize_t appendw(char **b, uint64_t w) {
  char t[8];
  unsigned n = 1;
  WRITE64LE(t, w);
  if (w) n += bsrl(w) >> 3;
  return appendd(b, t, n);
}
