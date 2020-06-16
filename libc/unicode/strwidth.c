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
#include "libc/bits/safemacros.h"
#include "libc/conv/conv.h"
#include "libc/limits.h"
#include "libc/str/internal.h"
#include "libc/str/str.h"
#include "libc/str/tpdecode.h"
#include "libc/unicode/unicode.h"

#define kOneTrueTabWidth 8

/**
 * Returns monospace display width in UTF-8 string.
 */
int(strwidth)(const char *s) {
  return strnwidth(s, SIZE_MAX);
}

int(strnwidth)(const char *s, size_t n) {
  /* TODO(jart): Fix this function. */
  size_t l;
  wint_t wc;
  const unsigned char *p, *pe;
  l = 0;
  if (n) {
    p = (const unsigned char *)s;
    pe = (const unsigned char *)(n == SIZE_MAX ? INTPTR_MAX : (intptr_t)s + n);
    for (;;) {
      while (p < pe && iscont(*p)) p++;
      if (p == pe || !*p) break;
      if (*p == L'\t') {
        if (l & (kOneTrueTabWidth - 1)) {
          l += kOneTrueTabWidth - (l & (kOneTrueTabWidth - 1));
        } else {
          l += kOneTrueTabWidth;
        }
        ++p;
      } else if (*p == L'\e') {
        while (++p < pe && *p) {
          if (*p == '[' || *p == ';' || isdigit(*p)) {
            continue;
          } else {
            ++p;
            break;
          }
        }
      } else {
        p += abs(tpdecode((const char *)p, &wc));
        l += max(0, wcwidth(wc));
      }
    }
  }
  return l;
}
