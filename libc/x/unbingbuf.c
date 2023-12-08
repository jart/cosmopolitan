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
#include "libc/assert.h"
#include "libc/fmt/bing.internal.h"
#include "libc/log/check.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

/**
 * Decodes human-readable CP437 glyphs into binary, e.g.
 *
 *     char binged[5];
 *     char golden[5] = "\0\1\2\3\4";
 *     unbingbuf(binged, sizeof(binged), u" ☺☻♥♦", -1);
 *     CHECK_EQ(0, memcmp(binged, golden, 5));
 *
 * @param buf is caller owned
 * @param size is byte length of buf
 * @param glyphs is UCS-2 encoded CP437 representation of binary data
 * @param fill if -1 will memset any remaining buffer space
 * @note no NUL terminator is added to end of buf
 * @see tunbing(), unbingstr(), unbing()
 */
void *unbingbuf(void *buf, size_t size, const char16_t *glyphs, int fill) {
  int b;
  char *p, *pe;
  for (p = buf, pe = p + size; p < pe && *glyphs; ++p, ++glyphs) {
    *p = (b = unbing(*glyphs)) & 0xff;
    /* DCHECK_NE(-1, b, "%`'hc ∉ IBMCP437\n", *glyphs); */
  }
  if (fill != -1) memset(p, fill, pe - p);
  return buf;
}
