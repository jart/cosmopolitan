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
#include "libc/str/str.h"

/**
 * Copies at most 𝑛 bytes from 𝑠 to 𝑑 until 𝑐 is encountered.
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
 * @return 𝑑 + idx(𝑐) + 1, or NULL if 𝑐 ∉ 𝑠₀․․ₙ₋₁
 * @note 𝑑 and 𝑠 can't overlap
 * @asyncsignalsafe
 */
void *memccpy(void *d, const void *s, int c, size_t n) {
  const char *p, *pe;
  if ((pe = memchr((p = s), c, n))) {
    return mempcpy(d, s, pe - p + 1);
  } else {
    memcpy(d, s, n);
    return NULL;
  }
}
