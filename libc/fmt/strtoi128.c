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
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/strtol.internal.h"
#include "libc/limits.h"
#include "libc/str/str.h"
#include "libc/str/tab.internal.h"

/**
 * Decodes 128-bit signed integer from ASCII string.
 *
 * @param s is a non-null nul-terminated string
 * @param endptr if non-null will always receive a pointer to the char
 *     following the last one this function processed, which is usually
 *     the NUL byte, or in the case of invalid strings, would point to
 *     the first invalid character
 * @param base can be anywhere between [2,36] or 0 to auto-detect based
 *     on the the prefixes 0 (octal), 0x (hexadecimal), 0b (binary), or
 *     decimal (base 10) by default
 * @return decoded saturated integer
 * @see strtou128()
 */
int128_t strtoi128(const char *s, char **endptr, int base) {
  char t = 0;
  int d, c = *s;
  int128_t x = 0;
  CONSUME_SPACES(s, c);
  GET_SIGN(s, c, d);
  GET_RADIX(s, c, base);
  if ((c = kBase36[c & 255]) && --c < base) {
    if (!((t |= 1) & 2)) {
      do {
        if (__builtin_mul_overflow(x, base, &x) ||
            __builtin_add_overflow(x, c * d, &x)) {
          x = d > 0 ? INT128_MAX : INT128_MIN;
          errno = ERANGE;
          t |= 2;
        }
      } while ((c = kBase36[*++s & 255]) && --c < base);
    }
  }
  if (t && endptr) *endptr = s;
  return x;
}
