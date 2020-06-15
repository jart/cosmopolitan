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
#include "libc/assert.h"
#include "libc/stdio/stdio.h"

#define DueToGnomeTerminal        1
#define DueToLiteralXterm         1
#define DueToKittyWithPragmataPro 1

#if 0
static void rgb2hsv(T r, T g, T b, T *out_h, T *out_s, T *out_v) {
  T c, h, s, v, min, max;
  h = NAN;
  s = 0;
  v = 0;
  min = MIN(MIN(r, g), b);
  max = MAX(MAX(r, g), b);
  if (max > C(0.0)) {
    v = max;
    c = max - min;
    s = c / v;
    if (c > C(0.00001)) {
      if (r >= max) {
        h = (g - b) / c;
      } else if (g >= max) {
        h = C(2.0) + (b - r) / c;
      } else {
        h = C(4.0) + (r - g) / c;
      }
      h *= C(60.0);
      if (h < C(0.0)) {
        h += C(360.0);
      }
    }
  }
  *out_h = h;
  *out_s = s;
  *out_v = v;
}
static void hsv2rgb(T h, T s, T v, T *out_r, T *out_g, T *out_b) {
  long i;
  T r, g, b, hh, p, q, t, ff;
  if (s <= C(0.0)) {
    r = v;
    g = v;
    b = v;
  } else {
    hh = h;
    if (h >= C(360.0)) hh = 0;
    hh /= C(60.0);
    i = (long)hh;
    ff = hh - i;
    p = v * (C(1.0) - s);
    q = v * (C(1.0) - (s * ff));
    t = v * (C(1.0) - (s * (C(1.0) - ff)));
    switch (i) {
      case 0:
        r = v;
        g = t;
        b = p;
        break;
      case 1:
        r = q;
        g = v;
        b = p;
        break;
      case 2:
        r = p;
        g = v;
        b = t;
        break;
      case 3:
        r = p;
        g = q;
        b = v;
        break;
      case 4:
        r = t;
        g = p;
        b = v;
        break;
      case 5:
      default:
        r = v;
        g = p;
        b = q;
        break;
    }
  }
  *out_r = r;
  *out_g = g;
  *out_b = b;
}
#endif

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
