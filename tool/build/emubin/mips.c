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
#include "tool/build/emubin/metalsha256.h"

//#define DISINGENUOUS /* 100 million NOPs is still 100 MIPS lool */

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
