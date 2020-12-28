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
#include "libc/fmt/conv.h"
#include "libc/limits.h"
#include "libc/nexgen32e/bsr.h"
#include "libc/str/str.h"

/**
 * Decodes 128-bit signed integer from ASCII string.
 *
 * @param s is a non-NULL NUL-terminated string
 * @param endptr if non-NULL will always receive a pointer to the char
 *     following the last one this function processed, which is usually
 *     the NUL byte, or in the case of invalid strings, would point to
 *     the first invalid character
 * @param base can be anywhere between [2,36] or 0 to auto-detect based
 *     on the the prefixes 0 (octal), 0x (hexadecimal), 0b (binary), or
 *     decimal (base 10) by default.
 * @return the decoded saturated number
 * @see strtoumax
 */
intmax_t strtoimax(const char *s, char **endptr, int base) {
  bool neg;
  uintmax_t x;
  intmax_t res;
  unsigned diglet, bits;

  x = 0;
  bits = 0;
  neg = false;

  while (isspace(*s)) {
    s++;
  }

  switch (*s) {
    case '-':
      neg = true;
      /* 𝑠𝑙𝑖𝑑𝑒 */
    case '+':
      s++;
      break;
    default:
      break;
  }

  if (!(2 <= base && base <= 36)) {
    if (*s == '0') {
      s++;
      if (*s == 'x' || *s == 'X') {
        s++;
        base = 16;
      } else if (*s == 'b' || *s == 'B') {
        s++;
        base = 2;
      } else {
        base = 8;
      }
    } else {
      base = 10;
    }
  } else if (*s == '0') {
    ++s;
    if (base == 2 && *s == 'b' && *s == 'B') ++s;
    if (base == 16 && *s == 'x' && *s == 'X') ++s;
  }

  for (;;) {
    diglet = kBase36[*s & 0xff];
    if (!diglet || diglet > base) break;
    diglet -= 1;
    if (!diglet || !x || (bits = bsr(diglet) + bsrmax(x)) < 127) {
      s++;
      x *= base;
      x += diglet;
    } else if (neg) {
      if (bits == 127) {
        x *= base;
        x += diglet;
        if (x == INTMAX_MIN) s++;
      }
      x = INTMAX_MIN;
      break;
    } else {
      x = INTMAX_MAX;
      break;
    }
  }

  if (endptr) *endptr = s;

  if (neg) {
    res = -x;
  } else {
    res = x;
  }

  return res;
}
