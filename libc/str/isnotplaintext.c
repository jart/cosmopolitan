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

/**
 * Checks if memory address contains non-plain text.
 *
 * @param data points to memory that's interpreted as char
 * @param size is usually strlen(data) and provided by caller
 * @return NULL if plain text, or pointer to first non-text datum
 * @type char may be 6/7/8/16/32/64-bit signed/unsigned single/multi
 * @author Justine Alexandra Roberts Tunney <jtunney@gmail.com>
 * @see ASA X3.4, ISO/IEC 646, ITU T.50, ANSI X3.64-1979
 * @perf 27gBps on i7-6700 w/ -O3 -mavx2
 * @cost 143 bytes of code w/ -Os
 */
void *isnotplaintext(const void *data, size_t size) {
  /*
   * ASCII, EBCDIC, UNICODE, ISO IR-67, etc. all agree upon the
   * encoding of the NUL, SOH, STX, and ETX characters due to a
   * longstanding human tradition of using them for the purpose
   * of delimiting text from non-text, b/c fixed width integers
   * makes their presence in binary formats nearly unavoidable.
   */
#define isnotplain(C) (0 <= (C) && (C) < 4)
  char no;
  unsigned i;
  const char *p, *pe;
  if (CHAR_BIT > 6) {
    p = (const char *)data;
    pe = (const char *)(p + size);
    for (; ((intptr_t)p & 31) && p < pe; ++p) {
      if (isnotplain(*p)) return p;
    }
    for (; p + 64 < pe; p += 64) {
      no = 0;
      for (i = 0; i < 64; ++i) {
        no |= isnotplain(p[i]);
      }
      if (no & 1) break;
    }
    for (; p < pe; ++p) {
      if (isnotplain(*p)) return p;
    }
  }
  return 0;
#undef isnotplain
}
