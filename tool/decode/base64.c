/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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

/**
 * @fileoverview base64 stream coder
 *
 * Does `openssl base64 [-d]` as a 20kb αcτµαlly pδrταblε εxεcµταblε.
 */

#define CHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"

const signed char kBase64[256] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0x00
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0x10
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, 62, -1, 63,  // 0x20
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,  // 0x30
    -1, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14,  // 0x40
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, 63,  // 0x50
    -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,  // 0x60
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,  // 0x70
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0x80
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0x90
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0xa0
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0xb0
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0xc0
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0xd0
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0xe0
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 0xf0
};

void Encode(void) {
  int a, b, c, w;
  while ((a = getchar()) != -1) {
    b = getchar();
    c = getchar();
    w = a << 020;
    if (b != -1) w |= b << 010;
    if (c != -1) w |= c;
    putchar(CHARS[(w >> 18) & 077]);
    putchar(CHARS[(w >> 12) & 077]);
    putchar(b != -1 ? CHARS[(w >> 6) & 077] : '=');
    putchar(c != -1 ? CHARS[w & 077] : '=');
  }
  putchar('\n');
}

int Get(void) {
  int c;
  while ((c = getchar()) != -1) {
    if ((c = kBase64[c]) != -1) break;
  }
  return c;
}

void Decode(void) {
  int a, b, c, d, w;
  while ((a = Get()) != -1 && (b = Get()) != -1) {
    c = Get();
    d = Get();
    w = a << 18 | b << 12;
    if (c != -1) w |= c << 6;
    if (d != -1) w |= d;
    putchar((w & 0xFF0000) >> 020);
    if (c != -1) putchar((w & 0x00FF00) >> 010);
    if (d != -1) putchar((w & 0x0000FF) >> 000);
  }
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    Encode();
  } else if (argc == 2 && !strcmp(argv[1], "-d")) {
    Decode();
  } else {
    fputs("usage: ", stderr);
    fputs(argv[0], stderr);
    fputs(" [-d]\n", stderr);
    return 1;
  }
  return ferror(stdin) || ferror(stdout) ? 1 : 0;
}
