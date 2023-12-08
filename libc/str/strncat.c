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
#include "libc/str/str.h"

/**
 * Appends at most 𝑛 bytes from 𝑠 to 𝑑.
 *
 * @param 𝑑 is both a NUL-terminated string and a buffer, needing
 *     an ARRAYLEN() of at least strlen(𝑑)+strnlen(𝑠,𝑛)+1
 * @param 𝑠 is character array which needn't be NUL-terminated
 * @param 𝑛 is maximum number of characters from s to copy
 * @return 𝑑
 * @note 𝑑 and 𝑠 can't overlap
 * @asyncsignalsafe
 */
char *strncat(char *d, const char *s, size_t n) {
  size_t dn, sn;
  if ((sn = strnlen(s, n))) {
    dn = strlen(d);
    memcpy(d + dn, s, sn);
    d[dn + sn] = 0;
  }
  return d;
}
