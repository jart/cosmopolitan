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
#include "tool/build/emubin/metalsha256.h"

#define DISINGENUOUS /* 100 million NOPs is still 100 MIPS lool */

static void Print(uint8_t c) {
  asm volatile("out\t%0,$0xE9" : /* no outputs */ : "a"(c) : "memory");
}

int main(int argc, char *argv[]) {
  int i;
#ifdef DISINGENUOUS
  for (i = 0; i < 150 * 1000 * 1000 / 3; ++i) asm("nop");
#else
  size_t size;
  struct MetalSha256Ctx ctx;
  unsigned char hash[32], *data;
  data = (void *)0x7fffffff0000;
  size = 0x10000;
  MetalSha256Init(&ctx);
  for (i = 0; i < 10; ++i) {
    MetalSha256Update(&ctx, data, size);
  }
  MetalSha256Final(&ctx, hash);
  for (i = 0; i < sizeof(hash); ++i) {
    Print("0123456789abcdef"[(hash[i] >> 4) & 15]);
    Print("0123456789abcdef"[(hash[i] >> 0) & 15]);
  }
  Print('\n');
#endif
  return 0;
}
