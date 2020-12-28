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
#include "libc/assert.h"
#include "libc/stdio/stdio.h"

#define DIST(X, Y)   ((X) - (Y))
#define SQR(X)       ((X) * (X))
#define SUM(X, Y, Z) ((X) + (Y) + (Z))

static const uint8_t kXtermCube[] = {0, 0137, 0207, 0257, 0327, 0377};

static int uncube(int x) {
  return x < 48 ? 0 : x < 115 ? 1 : (x - 35) / 40;
}

static int rgb2xterm256(int r, int g, int b) {
  unsigned av, ir, ig, ib, il, qr, qg, qb, ql;
  av = (r + g + b) / 3;
  ql = (il = av > 238 ? 23 : (av - 3) / 10) * 10 + 8;
  qr = kXtermCube[(ir = uncube(r))];
  qg = kXtermCube[(ig = uncube(g))];
  qb = kXtermCube[(ib = uncube(b))];
  if (SUM(SQR(DIST(qr, r)), SQR(DIST(qg, g)), SQR(DIST(qb, b))) <=
      SUM(SQR(DIST(ql, r)), SQR(DIST(ql, g)), SQR(DIST(ql, b)))) {
    return ir * 36 + ig * 6 + ib + 020;
  } else {
    return il + 0350;
  }
}

bool IsUglyColorMixture(int rune, int y, int tone, bool toneisfg) {
  assert(tone == 16 || (231 <= tone && tone <= 255));
  return false;
}

int main(int argc, char *argv[]) {
  unsigned y, z, x, k, w;
  static const char16_t s[2][3] = {{u'▓', u'▒', u'░'}, {u'░', u'▒', u'▓'}};

  printf("\npure\n");
  for (y = 0; y < 6; ++y) {
    for (z = 0; z < 6; ++z) {
      for (x = 0; x < 6; ++x) {
        printf("\033[48;5;%hhum  ", 16 + x + y * 6 + z * 6 * 6);
      }
      printf("\033[0m ");
    }
    printf("\n");
  }

#define MIX(NAME, COLOR)                                                       \
  do {                                                                         \
    printf("\n%s %d ▓░/▒▒/░▓\n", NAME, COLOR);                                 \
    for (k = 0; k < 3; ++k) {                                                  \
      for (y = 0; y < 6; ++y) {                                                \
        for (z = 0; z < 6; ++z) {                                              \
          for (x = 0; x < 6; ++x) {                                            \
            printf("\033[48;5;%hhu;38;5;%hhum%lc", COLOR,                      \
                   16 + x + y * 6 + z * 6 * 6, s[0][k]);                       \
            printf("\033[48;5;%hhu;38;5;%hhum%lc", 16 + x + y * 6 + z * 6 * 6, \
                   COLOR, s[1][k]);                                            \
          }                                                                    \
          printf("\033[0m ");                                                  \
        }                                                                      \
        printf("\n");                                                          \
      }                                                                        \
    }                                                                          \
  } while (0)

  MIX("tint", 231);
  MIX("tint", 255);
  MIX("tint", 254);
  MIX("tone", 240);
  MIX("shade", 232);

  for (w = 233; w < 254; ++w) {
    MIX("gray", w);
  }

  return 0;
}
