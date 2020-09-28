/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/assert.h"
#include "libc/fmt/bing.h"
#include "libc/log/check.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

/**
 * Decodes human-readable CP437 glyphs into binary, e.g.
 *
 *   char binged[5];
 *   char golden[5] = "\0\1\2\3\4";
 *   unbingbuf(binged, sizeof(binged), u" ☺☻♥♦", -1);
 *   CHECK_EQ(0, memcmp(binged, golden, 5));
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
