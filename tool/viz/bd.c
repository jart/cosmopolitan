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

/**
 * @fileoverview Braille Dump
 *
 * Written by Justine Tunney <jtunney@gmail.com>
 * https://twitter.com/justinetunney
 *
 * Designed by Ange Albertini (corkami)
 * https://twitter.com/angealbertini
 *
 * See also `hexdump -C`.
 */

const wchar_t K[256] = L"▁☺☻♥♦♣♠•◘○◙♂♀♪♫☼"  /* 0x00 */
                       L"►◄↕‼¶§▬↨↑↓→←∟↔▲▼"  /* 0x10 */
                       L" !\"#$%&'()*+,-./" /* 0x20 */
                       L"0123456789:;<=>?"  /* 0x30 */
                       L"@ABCDEFGHIJKLMNO"  /* 0x40 */
                       L"PQRSTUVWXYZ[\\]^_" /* 0x50 */
                       L"`abcdefghijklmno"  /* 0x60 */
                       L"pqrstuvwxyz{|}~⌂"  /* 0x70 */
                       L"█⡀⢀⣀⠠⡠⢠⣠⠄⡄⢄⣄⠤⡤⢤⣤"  /* 0x80 */
                       L"⠁⡁⢁⣁⠡⡡⢡⣡⠅⡅⢅⣅⠥⡥⢥⣥"  /* 0x90 */
                       L"⠃⡃⢃⣃⠣⡣⢣⣣⠇⡇⢇⣇⠧⡧⢧⣧"  /* 0xa0 */
                       L"⠉⡉⢉⣉⠩⡩⢩⣩⠍⡍⢍⣍⠭⡭⢭⣭"  /* 0xb0 */
                       L"⠊⡊⢊⣊⠪⡪⢪⣪⠎⡎⢎⣎⠮⡮⢮⣮"  /* 0xc0 */
                       L"⠑⡑⢑⣑⠱⡱⢱⣱⠕⡕⢕⣕⠵⡵⢵⣵"  /* 0xd0 */
                       L"⠚⡚⢚⣚⠺⡺⢺⣺⠞⡞⢞⣞⠾⡾⢾⣾"  /* 0xe0 */
                       L"⠛⡛⢛⣛⠻⡻⢻⣻⠟⡟⢟⣟⠿⡿⢿⣿"; /* 0xf0 */

int main(int argc, char *argv[]) {
  long o;
  FILE *f;
  int b, c, i, n;
  wchar_t B[17], *q;
  char R[16], A[50], *p;
  if (!(f = argc > 1 ? fopen(argv[1], "r") : stdin)) {
    fprintf(stderr, "ERROR: COULD NOT OPEN: %s\n", argv[1]);
    return 1;
  }
  for (b = o = 0;;) {
    if (!(n = fread(R, 1, 16, f))) break;
    p = A, q = B;
    for (c = i = 0; i < n; ++i) {
      if (i == 8) *p++ = ' ';
      *p++ = "0123456789abcdef"[(R[i] & 0xF0) >> 4];
      *p++ = "0123456789abcdef"[(R[i] & 0x0F) >> 0];
      *p++ = ' ';
      *q++ = K[R[i] & 255];
      c |= R[i];
    }
    if (c || !o) {
      *p = *q = b = 0;
      printf("%08x  %-49s │%ls│\n", o, A, B);
    } else if (!b) {
      b = 1;
      printf("*\n");
    }
    o += n;
  }
  if (o) printf("%08x\n", o);
  return !feof(f);
}
