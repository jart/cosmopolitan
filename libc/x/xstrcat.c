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
#include "libc/bits/safemacros.internal.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/x/x.h"

/**
 * Concatenates strings / chars to newly allocated memory, e.g.
 *
 *     xstrcat("hi", ' ', "there")
 *
 * Or without the C99 helper macro:
 *
 *     (xstrcat)("hi", ' ', "there", NULL)
 *
 * This goes twice as fast as the more powerful xasprintf(). It's not
 * quadratic like strcat(). It's much slower than high-effort stpcpy(),
 * particularly with string literals.
 *
 * @see gc()
 */
char *(xstrcat)(const char *s, ...) {
  char *p, b[2];
  va_list va;
  size_t i, n, n2, l;
  p = NULL;
  i = n = 0;
  va_start(va, s);
  do {
    if ((intptr_t)s > 0 && (intptr_t)s <= 255) {
      b[0] = (unsigned char)(intptr_t)s;
      b[1] = '\0';
      s = b;
      l = 1;
    } else {
      l = strlen(s);
    }
    if ((n2 = i + l + 16) >= n) {
      p = xrealloc(p, (n = n2 + (n2 >> 1)));
    }
    memcpy(p + i, s, l + 1);
    i += l;
  } while ((s = va_arg(va, const char *)));
  va_end(va);
  return p;
}
