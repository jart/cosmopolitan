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
#include "dsp/tty/quant.h"
#include "libc/bits/bits.h"
#include "libc/bits/xchg.h"
#include "libc/fmt/fmt.h"
#include "libc/math.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"

/* #define ROUND(x) x */
/* #define RT       int */
/* #define R1       1 */
/* #define R2       2 */
/* #define R3       3 */

#define RT        float
#define MUL(x, y) ((x) * (y))
#define RND(x)    roundf(x)
#define R1        0.25f
#define R2        0.50f
#define R3        0.75f

#define rgb_t struct TtyRgb

forceinline RT lerp(RT x, RT y, RT d) {
  return x * (1.0 - d) + y * d;
}
forceinline int lerp255(RT x, RT y, RT d) {
  return lerp(x / 255.0, y / 255.0, d) * 255.0;
}
forceinline rgb_t rgblerp(rgb_t x, rgb_t y, RT d) {
  return (rgb_t){lerp255(x.r, y.r, d), lerp255(x.g, y.g, d),
                 lerp255(x.b, y.b, d)};
}

forceinline rgb_t getquant(unsigned xt) {
  return g_ansi2rgb_[xt];
}
forceinline unsigned dist(int x, int y) {
  return x - y;
}
forceinline unsigned sqr(int x) {
  return x * x;
}
forceinline unsigned rgbdist(rgb_t x, rgb_t y) {
  return sqrt(sqr(dist(x.r, y.r)) + sqr(dist(x.g, y.g)) + sqr(dist(x.b, y.b)));
}

bool b;
rgb_t rgb, cc, c1, c2;
unsigned i, j, k, m, n, x, y;
char buf[128];

/* 0125 025 067-29   # '░' bg=0352 fg=0306 → ░░░ */
/* 0125 025 067-29   # '▓' bg=0306 fg=0352 → ▓▓▓ */
/* 0125 055 067-29   # '░' bg=0352 fg=0314 → ░░░ */

int main(int argc, char *argv[]) {
  /* memcpy(g_ansi2rgb_, &kTangoPalette, sizeof(kTangoPalette)); */
  /* i = 21; */
  /* j = 22; */
  /* c1 = getquant(i); */
  /* c2 = getquant(j); */
  /* cc = rgblerp(c1, c2, R1); */
  /* printf("rgblerp((%3d,%3d,%3d), (%3d,%3d,%3d),4) → (%3d,%3d,%3d)\n", c1.r,
   */
  /*        c1.g, c1.b, c2.r, c2.g, c2.b, cc.r, cc.g, cc.b); */
  /* exit(0); */

  for (m = 16; m < 256; m += 6) {
    for (n = 16; n < 256; n += 6) {
      printf("------------------------------------------------------------\n");
      i = m;
      j = n;
      b = false;
      while (i < m + 6) {
        printf("\n");

        cc = getquant(i);
        sprintf(buf, "\e[48;5;%dm   ", i);
        printf("0x%02x%02x%02x, %d,%d,0\t/* 0x%02x%02x%02x           "
               " + ' ' bg=%3d        → %s         \e[0m */\n",
               cc.b, cc.g, cc.r, i, 0, getquant(i).r, getquant(i).g,
               getquant(i).b, i, buf);

#if 0
        sprintf(buf, "\e[38;5;%dm███", i);
        printf("0x%08x 0x%02x%02x%02x\t"
               " '█'        fg=%3d → %s\e[0m\n",
               cc.b, cc.g, cc.r, strlen(buf), i, buf);
#endif

        for (x = i; x < i + 1; ++x) {
          for (y = j; y < j + 1; ++y) {
            for (k = 0; k < 2; ++k) {
              if (x > y /*  && */
                  /* rgbdist(getquant(x), getquant(y)) < 49744125 / 16 */ /*  &&
                  ((32
                  <= x && x <= 232) && (32 <= y && y <= 232))
                  && */
                  /* (cc.r > 0137 && cc.g > 0137 && cc.b > 0137) */) {
                sprintf(buf, "\e[48;5;%d;38;5;%dm░░░", x, y);
                cc = rgblerp(getquant(x), getquant(y), R1);
                printf("0x%02x%02x%02x, %d,%d,1\t/* 0x%02x%02x%02x + "
                       "0x%02x%02x%02x"
                       " + '░' bg=%3d fg=%3d → "
                       "\e[48;5;%dm   \e[48;5;%dm   "
                       "%s\e[48;2;%d;%d;%dm   \e[0m */\n",
                       cc.b, cc.g, cc.r, x, y, getquant(x).r, getquant(x).g,
                       getquant(x).b, getquant(y).r, getquant(y).g,
                       getquant(y).b, x, y, x, y, buf, cc.r, cc.g, cc.b);

                sprintf(buf, "\e[48;5;%d;38;5;%dm▒▒▒", x, y);
                cc = rgblerp(getquant(x), getquant(y), R2);
                printf("0x%02x%02x%02x, %d,%d,2\t/* 0x%02x%02x%02x + "
                       "0x%02x%02x%02x"
                       " + '▒' bg=%3d fg=%3d → "
                       "\e[48;5;%dm   \e[48;5;%dm   "
                       "%s\e[48;2;%d;%d;%dm   \e[0m */\n",
                       cc.b, cc.g, cc.r, x, y, getquant(x).r, getquant(x).g,
                       getquant(x).b, getquant(y).r, getquant(y).g,
                       getquant(y).b, x, y, x, y, buf, cc.r, cc.g, cc.b);

                sprintf(buf, "\e[48;5;%d;38;5;%dm▓▓▓", x, y);
                cc = rgblerp(getquant(x), getquant(y), R3);
                printf("0x%02x%02x%02x, %d,%d,3\t/* 0x%02x%02x%02x + "
                       "0x%02x%02x%02x"
                       " + '▓' bg=%3d fg=%3d → "
                       "\e[48;5;%dm   \e[48;5;%dm   "
                       "%s\e[48;2;%d;%d;%dm   \e[0m */\n",
                       cc.b, cc.g, cc.r, x, y, getquant(x).r, getquant(x).g,
                       getquant(x).b, getquant(y).r, getquant(y).g,
                       getquant(y).b, x, y, x, y, buf, cc.r, cc.g, cc.b);
              }

#if 0
              sprintf(buf, "\e[48;5;%d;38;5;%dm▓▓▓", x, y);
              cc = rgblerp((c1 = getquant(x)), (c2 = getquant(y)), R3);
              printf("0%03o%03o%03o\t# '▓' bg=%3d fg=%3d → "
                     "%s\e[48;2;%d;%d;%dm   \e[0m\n",
                     cc.b, cc.g, cc.r, strlen(buf), x, y, buf,
                     lerp255(c1.r, c2.r, R3), lerp255(c1.g, c2.g, R3),
                     lerp255(c1.b, c2.b, R3));
#endif

              xchg(&x, &y);
            }
          }
        }

        ++i;
        ++j;
      }
    }
  }

  /* for (i = 0; i < 255; ++i) { */
  /*   for (j = 0; j < 255; ++j) { */
  /*     for (k = 0; k < 255; ++k) { */
  /*       printf("0%03o%03o%03o\n", i, j, k); */
  /*     } */
  /*   } */
  /* } */

  return 0;
}
