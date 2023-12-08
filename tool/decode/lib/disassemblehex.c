/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/str/tab.internal.h"
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
      bzero(glyphs, sizeof(glyphs));
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
