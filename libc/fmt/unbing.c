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
#include "libc/fmt/bing.h"
#include "libc/log/log.h"
#include "libc/str/str.h"

#define ALTCOUNT 21

static const struct Cp437Multimappings {
  unsigned char b[ALTCOUNT];
  char16_t c[ALTCOUNT];
} kCp437iMultimappings = {
#define ALT(I, C, B) .c[I] = C, .b[I] = B
    ALT(0, u'\n', '\n'),
    ALT(1, u'\r', '\r'),
    ALT(2, u'?', '?'),    /* TRIGRAPH */
    ALT(3, u'\'', '\''),  /* CHARACTER LITERAL */
    ALT(4, u'\"', '\"'),  /* STRING LITERAL */
    ALT(5, u'\\', '\\'),  /* ESCAPE LITERAL */
    ALT(6, u'∅', '\0'),   /* EMPTY SET */
    ALT(7, u'␀', '\0'),   /* SYMBOL FOR NULL [sic] */
    ALT(7, 0x20A7, 0x9E), /* PESETA SIGN */
    ALT(8, u'Π' /*  03A0: GREEK CAPITAL LETTER PI */, 0xE3),
    ALT(9, u'∏' /*  220F: N-ARY PRODUCT */, 0xE3),
    ALT(10, u'∑' /*  2211: N-ARY SUMMATION */, 0xE4),
    ALT(11, u'µ' /*  03BC: MICRO SIGN */, 0xE6),
    ALT(12, u'Ω' /*  2126: OHM SIGN */, 0xEA),
    ALT(13, u'∂' /*  2202: PARTIAL DIFFERENTIAL */, 0xEB),
    ALT(14, u'ε' /*  03D5: PHI SMALL (CLOSED FORM) */, 0xED),
    ALT(15, u'ϕ' /*  03D5: PHI SMALL (CLOSED FORM) */, 0xED),
    ALT(16, u'∈' /*  2208: ELEMENT-OF SIGN  */, 0xED),
    ALT(17, u'∊' /*  220A: SMALL ELEMENT OF */, 0xEE),
    ALT(18, u'∈' /*  03B5: ELEMENT-OF SIGN  */, 0xEE),
    ALT(19, u'β' /*  03B2: GREEK SMALL BETA */, 0xE1),
    ALT(20, u'ſ' /*  017F: LATIN SMALL LETTER LONG S */, 0xF4),
#undef ALT
};

/**
 * Turns CP437 unicode glyph into its binary representation.
 *
 * @param c is a unicode character
 * @return byte representation, or -1 if ch wasn't ibm cp437
 * @see bing()
 */
int unbing(int c) {
  int i;
  for (i = 0; i < 256; ++i) {
    if (c == kCp437[i]) {
      return i;
    }
  }
  for (i = 0; i < ALTCOUNT; ++i) {
    if (c == kCp437iMultimappings.c[i]) {
      return kCp437iMultimappings.b[i];
    }
  }
  return -1;
}
