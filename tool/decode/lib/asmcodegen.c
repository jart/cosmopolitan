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
#include "libc/fmt/fmt.h"
#include "libc/mem/mem.h"
#include "libc/runtime/gc.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "tool/decode/lib/asmcodegen.h"

char b1[BUFSIZ];
char b2[BUFSIZ];

char *format(char *buf, const char *fmt, ...) {
  va_list va;
  va_start(va, fmt);
  vsnprintf(buf, BUFSIZ, fmt, va);
  va_end(va);
  return buf;
}

nodiscard char *tabpad(const char *s, unsigned width) {
  char *p;
  size_t i, l, need;
  l = strlen(s);
  need = width > l ? (roundup(width, 8) - l - 1) / 8 + 1 : 0;
  p = memcpy(malloc(l + need + 1), s, l);
  for (i = 0; i < need; ++i) p[l + i] = '\t';
  if (!need) {
    p[l] = ' ';
    ++need;
  }
  p[l + need] = '\0';
  return p;
}

void show(const char *directive, const char *value, const char *comment) {
  if (comment) {
    printf("\t%s\t%s# %s\n", directive, gc(tabpad(value, COLUMN_WIDTH)),
           comment);
  } else {
    printf("\t%s\t%s\n", directive, gc(tabpad(value, COLUMN_WIDTH)));
  }
}
