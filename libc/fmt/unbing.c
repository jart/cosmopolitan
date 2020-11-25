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
#include "libc/alg/alg.h"
#include "libc/macros.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/str/str.h"

static const int kCp437iMultimappings[] = {
    u'\n' << 8 | '\n',   // NEWLINE
    u'\r' << 8 | '\r',   // CARRIAGE RETURN
    u'?' << 8 | '?',     // TRIGRAPH
    u'\'' << 8 | '\'',   // CHARACTER LITERAL
    u'\"' << 8 | '\"',   // STRING LITERAL
    u'\\' << 8 | '\\',   // ESCAPE LITERAL
    u'∅' << 8 | '\0',    // EMPTY SET
    u'␀' << 8 | '\0',    // SYMBOL FOR NULL [sic]
    0x20A7 << 8 | 0x9E,  // PESETA SIGN
    u'Π' << 8 | 0xE3,    // GREEK CAPITAL LETTER PI
    u'∏' << 8 | 0xE3,    // N-ARY PRODUCT
    u'∑' << 8 | 0xE4,    // N-ARY SUMMATION
    u'µ' << 8 | 0xE6,    // MICRO SIGN
    u'Ω' << 8 | 0xEA,    // OHM SIGN
    u'∂' << 8 | 0xEB,    // PARTIAL DIFFERENTIAL
    u'ϕ' << 8 | 0xED,    // PHI SMALL (CLOSED FORM)
    u'ε' << 8 | 0xEE,    // LATIN SMALL LETTER EPSILON
    u'∊' << 8 | 0xEE,    // SMALL ELEMENT OF
    u'∈' << 8 | 0xEE,    // ELEMENT-OF SIGN
    u'β' << 8 | 0xE1,    // GREEK SMALL BETA
    u'ſ' << 8 | 0xF4,    // LATIN SMALL LETTER LONG S
};

static int g_cp437i[256 + ARRAYLEN(kCp437iMultimappings)];

/**
 * Turns CP437 unicode glyph into its binary representation.
 *
 * @param c is a unicode character
 * @return byte representation, or -1 if ch wasn't ibm cp437
 * @see bing()
 */
int unbing(int c) {
  int m, l, r;
  l = 0;
  r = ARRAYLEN(g_cp437i) - 1;
  while (l <= r) {
    m = (l + r) >> 1;
    if ((g_cp437i[m] >> 8) < c) {
      l = m + 1;
    } else if ((g_cp437i[m] >> 8) > c) {
      r = m - 1;
    } else {
      return g_cp437i[m] & 0xff;
    }
  }
  return -1;
}

static textstartup void g_cp437i_init() {
  unsigned i;
  for (i = 0; i < 256; ++i) g_cp437i[i] = kCp437[i] << 8 | i;
  memcpy(g_cp437i + 256, kCp437iMultimappings, sizeof(kCp437iMultimappings));
  djbsort(g_cp437i, ARRAYLEN(g_cp437i));
}

const void *const g_cp437i_ctor[] initarray = {g_cp437i_init};
