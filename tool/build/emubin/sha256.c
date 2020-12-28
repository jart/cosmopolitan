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

#define SHA256_BLOCK_SIZE 32

#define OUTB(PORT, BYTE) asm volatile("outb\t%b1,%0" : : "N"(PORT), "a"(BYTE))
#define INW(PORT)                                       \
  ({                                                    \
    int Word;                                           \
    asm volatile("in\t%1,%0" : "=a"(Word) : "N"(PORT)); \
    Word;                                               \
  })

static void PrintHexBytes(int n, const uint8_t p[n]) {
  int i;
  for (i = 0; i < n; ++i) {
    OUTB(0xE9, "0123456789abcdef"[(p[i] >> 4) & 15]);
    OUTB(0xE9, "0123456789abcdef"[(p[i] >> 0) & 15]);
  }
  OUTB(0xE9, '\n');
}

int main(int argc, char *argv[]) {
  int rc;
  uint8_t hash[32], buf[1];
  struct MetalSha256Ctx ctx;
  MetalSha256Init(&ctx);
  while ((rc = INW(0xE9)) != -1) {
    buf[0] = rc;
    MetalSha256Update(&ctx, buf, sizeof(buf));
  }
  MetalSha256Final(&ctx, hash);
  PrintHexBytes(sizeof(hash), hash);
  return 0;
}
