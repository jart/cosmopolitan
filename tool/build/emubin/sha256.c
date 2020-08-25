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
