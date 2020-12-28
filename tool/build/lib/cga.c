/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/fmt/itoa.h"
#include "libc/macros.h"
#include "libc/str/str.h"
#include "tool/build/lib/buffer.h"
#include "tool/build/lib/cga.h"

/*                                     blk blu grn cyn red mag yel wht */
static const uint8_t kCgaToAnsi[16] = {30, 34, 32, 36, 31, 35, 33, 37,
                                       90, 94, 92, 96, 91, 95, 93, 97};

size_t FormatCga(uint8_t bgfg, char buf[hasatleast 11]) {
  char *p = buf;
  *p++ = '\e';
  *p++ = '[';
  p += uint64toarray_radix10(kCgaToAnsi[(bgfg & 0xF0) >> 4] + 10, p);
  *p++ = ';';
  p += uint64toarray_radix10(kCgaToAnsi[bgfg & 0x0F], p);
  *p++ = 'm';
  *p = '\0';
  return p - buf;
}

void DrawCga(struct Panel *p, uint8_t v[25][80][2]) {
  char buf[11];
  unsigned y, x, n, a;
  n = MIN(25, p->bottom - p->top);
  for (y = 0; y < n; ++y) {
    a = -1;
    for (x = 0; x < 80; ++x) {
      if (v[y][x][1] != a) {
        AppendData(&p->lines[y], buf, FormatCga((a = v[y][x][1]), buf));
      }
      AppendWide(&p->lines[y], kCp437[v[y][x][0]]);
    }
    AppendStr(&p->lines[y], "\e[0m");
  }
}
