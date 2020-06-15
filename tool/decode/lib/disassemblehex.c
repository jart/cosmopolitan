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
#include "tool/decode/lib/disassemblehex.h"

static size_t countzeroes(const uint8_t *data, size_t size) {
  size_t i;
  for (i = 0; i < size; ++i) {
    if (data[i] != '\0') break;
  }
  return i;
}

void disassemblehex(uint8_t *data, size_t size, FILE *f) {
  int col;
  uint8_t ch;
  size_t i, z;
  char16_t glyphs[kDisassembleHexColumns + 1];
  col = 0;
  for (i = 0; i < size; ++i) {
    ch = data[i];
    if (!col) {
      z = countzeroes(&data[i], size - i) / kDisassembleHexColumns;
      if (z > 2) {
        fprintf(f, "\t.%s\t%zu*%d\n", "zero", z, kDisassembleHexColumns);
        i += z * kDisassembleHexColumns;
        if (i == size) break;
      }
      fprintf(f, "\t.%s\t", "byte");
      memset(glyphs, 0, sizeof(glyphs));
    }
    /* TODO(jart): Fix Emacs */
    glyphs[col] = kCp437[ch == '"' || ch == '\\' || ch == '#' ? '.' : ch];
    if (col) fputc(',', f);
    fprintf(f, "0x%02x", ch);
    if (++col == kDisassembleHexColumns) {
      col = 0;
      fprintf(f, "\t#%hs\n", glyphs);
    }
  }
  if (col) fputc('\n', f);
}
