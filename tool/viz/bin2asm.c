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
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

#define COLS 8

int main(int argc, char *argv[]) {
  int c, col = 0;
  unsigned char ch;
  char16_t glyphs[COLS + 1];
  while ((c = getchar()) != -1) {
    if (col == 0) {
      printf("\t.byte\t");
      memset(glyphs, 0, sizeof(glyphs));
    }
    ch = c & 0xff;
    glyphs[col] = kCp437[ch];
    if (col) putchar(',');
    printf("0x%02x", ch);
    if (++col == COLS) {
      col = 0;
      printf("\t#%hs\n", glyphs);
    }
  }
  return 0;
}
