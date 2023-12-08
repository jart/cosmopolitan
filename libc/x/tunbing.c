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
#include "libc/fmt/bing.internal.h"
#include "libc/str/str.h"
#include "libc/x/x.h"

/**
 * Same as xunbing() w/ alignment guarantee.
 */
void *xunbinga(size_t a, const char16_t *binglyphs) {
  size_t size;
  size = strlen16(binglyphs);
  return unbingbuf(xmemalign(a, size), size, binglyphs, -1);
}

/**
 * Decodes CP437 glyphs to bounds-checked binary buffer, e.g.
 *
 *   char *mem = xunbing(u" ☺☻♥♦");
 *   EXPECT_EQ(0, memcmp("\0\1\2\3\4", mem, 5));
 *   tfree(mem);
 *
 * @see xunbing(), unbingstr(), unbing()
 */
void *xunbing(const char16_t *binglyphs) {
  return xunbinga(1, binglyphs);
}
