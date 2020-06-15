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
#include "libc/escape/escape.h"
#include "libc/macros.h"
#include "libc/str/str.h"

// TODO(jart): implement me

/**
 * Decodes backslash escape sequences in-place.
 *
 * We support the C standard definitions, and the gotchas that entails:
 *
 *     \newline                              ← noop for multi-line str
 *     \'  \"  \?  \\                        ← we try hard to avoid \?
 *     \a  \b  \f  \n  \r  \t  \v  \e        ← \e is A++ GNU extension
 *     \ octal-digit                         ← base-8 literal encoding
 *     \ octal-digit octal-digit             ← octal: super dependable
 *     \ octal-digit octal-digit octal-digit ← longstanding DEC legacy
 *     \x[0-9A-Fa-f]+                        ← base16 literal encoding
 *     \u[0-9A-Fa-f]{4}                      ← UNICODEs
 *     \U[0-9A-Fa-f]{8} (astral planes)      ← UNICODEs: Astral Planes
 *
 * @param n is # bytes in p
 * @param p is byte array of string literal content
 * @return new byte size of s, or -i to indicate first error at s[i]
 * @note we do not check for NUL terminator on input
 * @note array is zero-filled if shortened
 * @see cescapec(), strlen()
 */
int cunescape(size_t n, char p[n]) {
  unsigned char t[32];
  unsigned i, j, m, tc, tn, mask;
  for (m = n, i = 0; i < n; i += j) {
    tc = sizeof(t);
    tn = MIN(m - i, tc);
    memset(t, 0, tc);
    memcpy(t, p + i, tn);
    for (mask = j = 0; j < tc; ++j) {
      if (p[j] == '\\') mask |= 1u << j;
    }
    if (j < ARRAYLEN(t)) {
      memcpy(p + i, t, tn);
    }
  }
  return m;
}
