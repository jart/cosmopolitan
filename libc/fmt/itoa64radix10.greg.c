/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/alg/reverse.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"
#include "libc/limits.h"

/**
 * Converts unsigned 64-bit integer to string.
 * @param a needs at least 21 bytes
 * @return bytes written w/o nul
 */
noinline size_t uint64toarray_radix10(uint64_t i, char *a) {
  size_t j;
  j = 0;
  do {
    a[j++] = i % 10 + '0';
    i /= 10;
  } while (i > 0);
  a[j] = '\0';
  reverse(a, j);
  return j;
}

/**
 * Converts signed 64-bit integer to string.
 * @param a needs at least 21 bytes
 * @return bytes written w/o nul
 */
size_t int64toarray_radix10(int64_t i, char *a) {
  if (i >= 0) return uint64toarray_radix10(i, a);
  *a++ = '-';
  return 1 + uint64toarray_radix10(-i, a);
}
