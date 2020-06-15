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
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

testonly void testlib_formatbinaryashex(const char *want, const void *got,
                                        size_t n, char **out_v1,
                                        char **out_v2) {
  size_t i;
  uint8_t b;
  char *gothex;
  if (n == -1ul) n = strlen(want) / 2;
  gothex = xmalloc(n * 2 + 1);
  gothex[n * 2] = '\0';
  for (i = 0; i < n; ++i) {
    b = ((uint8_t *)got)[i];
    gothex[i * 2 + 0] = "0123456789abcdef"[(b >> 4) & 0xf];
    gothex[i * 2 + 1] = "0123456789abcdef"[(b >> 0) & 0xf];
  }
  *out_v1 = strdup(want);
  *out_v2 = gothex;
}
