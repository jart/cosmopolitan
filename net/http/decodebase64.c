/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "net/http/escape.h"

static const signed char kBase64[256] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0x00
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0x10
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, 62, -1, 63,  // 0x20
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -2, -1, -1,  // 0x30
    -1, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14,  // 0x40
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, 63,  // 0x50
    -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,  // 0x60
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,  // 0x70
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0x80
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0x90
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0xa0
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0xb0
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0xc0
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0xd0
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0xe0
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0xf0
};

/**
 * Decodes base64 ascii representation to binary.
 *
 * This supports the following alphabets:
 *
 * - ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/
 * - ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_
 *
 * @param data is input value
 * @param size if -1 implies strlen
 * @param out_size if non-NULL receives output length
 * @return allocated NUL-terminated buffer, or NULL w/ errno
 */
char *DecodeBase64(const char *data, size_t size, size_t *out_size) {
  size_t n;
  char *r, *q;
  int a, b, c, d, w;
  const char *p, *pe;
  if (size == -1) size = data ? strlen(data) : 0;
  if ((r = malloc(ROUNDUP(size, 4) / 4 * 3 + 1))) {
    q = r;
    p = data;
    pe = p + size;
    for (;;) {
      do {
        if (p == pe) goto Done;
        a = kBase64[*p++ & 0xff];
      } while (a == -1);
      if (a == -2) continue;
      do {
        if (p == pe) goto Done;
        b = kBase64[*p++ & 0xff];
      } while (b == -1);
      if (b == -2) continue;
      do {
        c = p < pe ? kBase64[*p++ & 0xff] : -2;
      } while (c == -1);
      do {
        d = p < pe ? kBase64[*p++ & 0xff] : -2;
      } while (d == -1);
      w = a << 18 | b << 12;
      if (c != -2) w |= c << 6;
      if (d != -2) w |= d;
      *q++ = (w & 0xFF0000) >> 020;
      if (c != -2) *q++ = (w & 0x00FF00) >> 010;
      if (d != -2) *q++ = (w & 0x0000FF) >> 000;
    }
  Done:
    n = q - r;
    *q++ = '\0';
    if ((q = realloc(r, q - r))) r = q;
  } else {
    n = 0;
  }
  if (out_size) {
    *out_size = n;
  }
  return r;
}
