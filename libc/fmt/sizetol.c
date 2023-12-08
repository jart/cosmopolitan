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
#include "libc/fmt/conv.h"
#include "libc/stdckdint.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

static int GetExponent(int c) {
  switch (c) {
    case '\0':
    case ' ':
    case '\t':
      return 0;
    case 'k':
    case 'K':
      return 1;
    case 'm':
    case 'M':
      return 2;
    case 'g':
    case 'G':
      return 3;
    case 't':
    case 'T':
      return 4;
    case 'p':
    case 'P':
      return 5;
    case 'e':
    case 'E':
      return 6;
    default:
      return -1;
  }
}

/**
 * Converts size string to long.
 *
 * The following unit suffixes may be used
 *
 * - `k` or `K` for kilo (multiply by 𝑏¹)
 * - `m` or `M` for mega (multiply by 𝑏²)
 * - `g` or `G` for giga (multiply by 𝑏³)
 * - `t` or `T` for tera (multiply by 𝑏⁴)
 * - `p` or `P` for peta (multiply by 𝑏⁵)
 * - `e` or `E` for exa  (multiply by 𝑏⁶)
 *
 * If a permitted alpha character is supplied, then any additional
 * characters after it (e.g. kbit, Mibit, TiB) are ignored. Spaces
 * before the integer are ignored, and overflows will be detected.
 *
 * Negative numbers are permissible, as well as a leading `+` sign. To
 * tell the difference between an error return and `-1` you must clear
 * `errno` before calling and test whether it changed.
 *
 * @param s is non-null nul-terminated input string
 * @param b is multiplier which should be 1000 or 1024
 * @return size greater than or equal 0 or -1 on error
 * @error EINVAL if error is due to bad syntax
 * @error EOVERFLOW if error is due to overflow
 */
long sizetol(const char *s, long b) {
  long x;
  int c, e, d;
  do {
    c = *s++;
  } while (c == ' ' || c == '\t');
  d = c == '-' ? -1 : 1;
  if (c == '-' || c == '+') c = *s++;
  if (!isdigit(c)) {
    return einval();
  }
  x = 0;
  do {
    if (ckd_mul(&x, x, 10) || ckd_add(&x, x, (c - '0') * d)) {
      return eoverflow();
    }
  } while (isdigit((c = *s++)));
  if ((e = GetExponent(c)) == -1) {
    return einval();
  }
  while (e--) {
    if (ckd_mul(&x, x, b)) {
      return eoverflow();
    }
  }
  return x;
}
