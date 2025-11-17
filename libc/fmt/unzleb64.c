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
╚──────────────────────────────────────────────────────────────────────────────╝
                             ▄             ▄▄▄
                       ▄▄▓▓██▀░     ▄     ▄▓▌▀▀▀███▒
                ▄    ░██░  ░▀▀    ▓▓█▓█▌    ▄▄▄██▀░         ▄▄░
            ░▄▓██░   ░██▄   ▄▄▓▌           ▄▓███░         ░▄█▀▄▓▒
             ░░▓█▓▄  ░▀██▒  ░▀█▄         ░▓██▓▄▒▄▄▀▀    ▄█▓▀▐██░
      ▄███      ▀██▄   ▀▓█▓▄▄▓▀                ▀▀▀▀ ░▄▄█▀░▒▓██▓   ░░▄▄▄▄▄▄
   ▄▄▓▀▐██    ░   ▓██▓▒   ▀▀                          ▀▀▀ ▄▄██░  ▄▓██▓▀░░▀█░
  ▀▒▓░ ▐██▓  ▓▓▒  ▀▀░                                     ▀▀▀█▒ ▓█▓▀░    ▄█▓▓░
   ▒▀  ▒██▒░▄█▓▀        VARIABLE LENGTH INTEGER DECODING        ▓█░  ▐▓▄ ░  ▓
       ░███▓▀                                                    ▀▓▓██▀▀░
        ░▀░                                                         */

#include "libc/fmt/leb128.h"

/**
 * Decodes array to signed integer w/ zig-zag encoding.
 *
 * This function is the reverse operation of zleb64().
 *
 * Zig-zag encoding is the same thing as unsigned LEB encoding (which we
 * implement in uleb64() and unuleb64()) except it moves the sign bit to
 * the lowest order position.
 *
 * @param p is array
 * @param n is byte length of array
 * @param x optionally receives decoded integer
 * @return bytes decoded, or negative number on error
 */
int unzleb64(const char *p, size_t n, int64_t *x) {
  int k;
  size_t i;
  uint64_t t;
  if (n > 9)
    n = 9;
#pragma GCC unroll 1000
  for (k = t = i = 0; i < n; ++i, k += 7) {
    if (i == 8) {
      t |= (uint64_t)(p[i] & 255) << k;
      *x = (t >> 1) ^ -(t & 1);
      return 9;
    } else {
      t |= (uint64_t)(p[i] & 127) << k;
      if (~p[i] & 128) {
        *x = (t >> 1) ^ -(t & 1);
        return i + 1;
      }
    }
  }
  return -1;
}
