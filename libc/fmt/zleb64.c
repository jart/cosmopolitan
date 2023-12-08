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
   ▒▀  ▒██▒░▄█▓▀        VARIABLE LENGTH INTEGER ENCODING        ▓█░  ▐▓▄ ░  ▓
       ░███▓▀                                                    ▀▓▓██▀▀░
        ░▀░                                                         */
#include "libc/fmt/leb128.h"

/**
 * Encodes signed integer to array w/ zig-zag encoding.
 *
 *     uleb64 INT64_MAX    l:        10𝑐         3𝑛𝑠
 *     zleb64 INT64_MAX    l:        13𝑐         4𝑛𝑠
 *     sleb64 INT64_MAX    l:        16𝑐         5𝑛𝑠
 *     uleb128 INT64_MAX   l:        18𝑐         6𝑛𝑠
 *     zleb128 INT64_MAX   l:        18𝑐         6𝑛𝑠
 *     sleb128 INT64_MAX   l:        24𝑐         8𝑛𝑠
 *
 * @param p is output array which should have 10 items
 * @param x is number
 * @return p + i
 * @see unzleb64()
 */
char *zleb64(char p[hasatleast 10], int64_t x) {
  int c;
  uint64_t u;
  u = x;
  u <<= 1;
  u ^= x >> 63;
  for (;;) {
    c = u & 127;
    if (!(u >>= 7)) {
      *p++ = c;
      return p;
    } else {
      *p++ = c | 128;
    }
  }
}
