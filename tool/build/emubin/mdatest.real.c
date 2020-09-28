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
#include "tool/build/emubin/poke.h"
#include "tool/build/emubin/real.h"

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
