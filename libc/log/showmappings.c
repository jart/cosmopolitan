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
#include "libc/calls/calls.h"
#include "libc/log/log.h"
#include "libc/runtime/mappings.h"
#include "libc/stdio/stdio.h"

STATIC_YOINK("ntoa");
STATIC_YOINK("stoa");

/**
 * Displays memory mappings in a way that's readable.
 *
 * It's readable because (1) map sizes are in bytes bytes with thousand
 * separators; (2) output is valid code; (3) memory addresses aren't
 * obfuscated; (4) we only need 8 hex digits to display each 64kb
 * granular 48-bit NexGen32e pointer; (5) ranges are inclusive; and (6)
 * gaps are made apparent.
 */
void showmappings(FILE *f) {
  size_t i, bytes, maptally, gaptally;
  maptally = 0;
  gaptally = 0;
  (fprintf)(f, "%s%zu%s\n", "_mm.i == ", _mm.i, ";");
  for (i = 0; i < _mm.i; ++i) {
    if (i && _mm.p[i].x != _mm.p[i - 1].y + 1) {
      bytes = _mm.p[i].x - _mm.p[i - 1].y - 1;
      bytes *= FRAMESIZE;
      gaptally += bytes;
      (fprintf)(f, "%s%,9zu%s\n", "/* ", bytes, " */");
    }
    bytes = _mm.p[i].y - _mm.p[i].x + 1;
    bytes *= FRAMESIZE;
    maptally += bytes;
    (fprintf)(f, "%s%3u%s0x%08x,0x%08x};%s%,9zu%s\n", "_mm.p[", i, "]=={",
              _mm.p[i].x, _mm.p[i].y, " /* ", bytes, " */");
  }
  (fprintf)(f, "%s%,zu%s%,zu%s\n", "/* ", maptally, " bytes mapped w/ ",
            gaptally, " bytes gapped */");
  fprintf(f, "\n");
}
