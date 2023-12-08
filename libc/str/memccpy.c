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
 * Copies at most N bytes from SRC to DST until 𝑐 is encountered.
 *
 * This is little-known C Standard Library approach, dating back to the
 * Fourth Edition of System Five, for copying a C strings to fixed-width
 * buffers, with added generality.
 *
 * For example, strictly:
 *
 *     char buf[16];
 *     CHECK_NOTNULL(memccpy(buf, s, '\0', sizeof(buf)));
 *
 * Or unstrictly:
 *
 *     if (!memccpy(buf, s, '\0', sizeof(buf))) strcpy(buf, "?");
 *
 * Are usually more sensible than the following:
 *
 *     char cstrbuf[16];
 *     snprintf(cstrbuf, sizeof(cstrbuf), "%s", CSTR);
 *
 * @param c is search character and is masked with 255
 * @return DST + idx(c) + 1, or NULL if 𝑐 ∉ 𝑠₀․․ₙ₋₁
 * @note DST and SRC can't overlap
 * @asyncsignalsafe
 */
void *memccpy(void *dst, const void *src, int c, size_t n) {
  char *d;
  size_t i;
  const char *s;
  for (d = dst, s = src, i = 0; i < n; ++i) {
    if (((d[i] = s[i]) & 255) == (c & 255)) {
      return d + i + 1;
    }
  }
  return 0;
}
