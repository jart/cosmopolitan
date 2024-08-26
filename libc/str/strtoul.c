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
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/internal.h"
#include "libc/limits.h"
#include "libc/stdckdint.h"
#include "libc/str/str.h"
#include "libc/str/tab.h"

/**
 * Decodes unsigned integer from ASCII string.
 *
 * @param s is a non-null nul-terminated string
 * @param endptr if non-null will always receive a pointer to the char
 *     following the last one this function processed, which is usually
 *     the NUL byte, or in the case of invalid strings, would point to
 *     the first invalid character
 * @param base can be anywhere between [2,36] or 0 to auto-detect based
 *     on the the prefixes 0 (octal), 0x (hexadecimal), 0b (binary), or
 *     decimal (base 10) by default
 * @return decoded integer mod 2⁶⁴ negated if leading `-`
 * @raise ERANGE on overflow
 */
unsigned long strtoul(const char *s, char **endptr, int base) {
  char t = 0;
  int d, c = *s;
  unsigned long x = 0;
  CONSUME_SPACES(char, s, c);
  GET_SIGN(s, c, d);
  GET_RADIX(s, c, base);
  if ((c = kBase36[c & 255]) && --c < base) {
    t |= 1;
    do {
      if (ckd_mul(&x, x, base) || ckd_add(&x, x, c)) {
        if (endptr) {
          *endptr = (char *)(s + 1);
        }
        errno = ERANGE;
        return ULONG_MAX;
      }
    } while ((c = kBase36[*++s & 255]) && --c < base);
  }
  if (t && endptr) {
    *endptr = (char *)s;
  }
  return d > 0 ? x : -x;
}

__weak_reference(strtoul, strtoumax);
__weak_reference(strtoul, strtoull);
__weak_reference(strtoul, strtoull_l);
