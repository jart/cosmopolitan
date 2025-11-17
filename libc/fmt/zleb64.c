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
 * This function is the reverse operation of unzleb64().
 *
 * Zig-zag encoding is the same thing as unsigned LEB encoding (which we
 * implement in uleb64() and unuleb64()) except it moves the sign bit to
 * the lowest order position.
 *
 * The maximum number of bytes that'll be serialized to `p` is 9. The
 * ninth byte is special. Normally with LEB encoding, the highest bit of
 * each byte indicates if we should consume additional bytes. That means
 * each byte has 7 bits of content, and 8*7=56 which means by the time
 * we hit the ninth byte, there's exactly 8 bits of content remaining in
 * the 64-bit integer we're decoding. So we don't use LEB on the ninth
 * byte and treat all eight bits of it as content.
 *
 * @param p is output array which should have 9 items
 * @param x is number
 * @return p + i
 * @see unzleb64()
 */
char *zleb64(char p[hasatleast 9], int64_t x) {
  uint64_t u;
  u = x;
  u <<= 1;
  u ^= x >> 63;
  return uleb64(p, u);
}
