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
#include "libc/limits.h"
#include "libc/stdckdint.h"
#include "libc/str/str.h"

/**
 * Decodes decimal integer from ASCII string.
 *
 * @param s is a non-null nul-terminated string
 * @return the decoded signed saturated integer
 */
long atol(const char *s) {
  long x;
  int c, d;
  do c = *s++;
  while (c == ' ' || c == '\t');
  d = c == '-' ? -1 : 1;
  if (c == '-' || c == '+') c = *s++;
  for (x = 0; isdigit(c); c = *s++) {
    if (ckd_mul(&x, x, 10) || ckd_add(&x, x, (c - '0') * d)) {
      errno = ERANGE;
      if (d > 0) {
        return LONG_MAX;
      } else {
        return LONG_MIN;
      }
    }
  }
  return x;
}

__weak_reference(atol, atoll);
