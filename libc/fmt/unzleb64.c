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
#include "libc/macros.internal.h"

/**
 * Decodes array to signed integer w/ zig-zag encoding.
 *
 * @param p is array
 * @param n is byte length of array
 * @param o optionally receives decoded integer
 * @return bytes decoded or -1 on error
 * @see zleb64()
 */
int unzleb64(const char *p, size_t n, int64_t *o) {
  int c;
  size_t i;
  uint64_t u, t;
  i = 0;
  u = 0;
  do {
    if (i == n) return -1;
    c = p[i] & 255;
    t = c & 127;
    if (i < 10) {
      t <<= i * 7;
      u |= t;
    }
    ++i;
  } while (c & 128);
  if (o) *o = (u >> 1) ^ -(u & 1);
  return i;
}
