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
#include "libc/bits/bits.h"
#include "libc/conv/conv.h"
#include "libc/macros.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

/**
 * @fileoverview CRC Lookup Table Generator
 * @see http://reveng.sourceforge.net/crc-catalogue/17plus.htm
 */

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s POLYNOMIAL\n", argv[0]);
    exit(1);
  }
  static uint32_t tab[256];
  crc32init(tab, strtoimax(argv[1], NULL, 0));
  for (unsigned i = 0; i < ARRAYLEN(tab); ++i) {
    if (i > 0) {
      printf(",");
      if (i % 6 == 0) {
        printf("\n");
      }
    }
    printf("0x%08x", tab[i]);
  }
  printf("\n");
  return 0;
}
