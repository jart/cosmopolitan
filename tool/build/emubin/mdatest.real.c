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
#include "tool/build/emubin/poke.h"
#include "tool/build/emubin/real.h"
#include "tool/build/emubin/realstart.inc"

/*
m=tiny; make -j12 MODE=$m o/$m/tool/build/{tinyemu,emulator}.com   \
o/$m/tool/build/emubin/mdatest.bin && o/$m/tool/build/emulator.com \
-rt o/$m/tool/build/emubin/mdatest.bin
*/

static void MdaTest(uint16_t p[25][80]) {
  int i, y, x;
  for (i = 0; i < 256; ++i) {
    y = i / 16;
    x = i % 16 * 3;
    POKE(p[y][x + 0], i << 8 | "0123456789abcdef"[(i & 0xf0) >> 4]);
    POKE(p[y][x + 1], i << 8 | "0123456789abcdef"[(i & 0x0f) >> 0]);
  }
}

int main() {
  SetVideoMode(7);
  SetEs(0xb0000 >> 4);
  MdaTest((void *)0);
  for (;;) asm("pause");
}
