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
#include "dsp/core/c1331.h"
#include "dsp/core/c161.h"
#include "dsp/core/core.h"
#include "dsp/scale/scale.h"
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/bsr.h"
#include "libc/rand/rand.h"
#include "libc/runtime/gc.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "third_party/gdtoa/gdtoa.h"
#include "third_party/getopt/getopt.h"
#include "third_party/stb/stb_image.h"
#include "tool/viz/lib/bilinearscale.h"
#include "tool/viz/lib/graphic.h"

#define LONG          long
#define CHAR          char
#define CLAMP(X)      MIN(255, MAX(0, X))
#define C13(A, B)     (((A) + 3 * (B) + 2) >> 2)
#define LERP(X, Y, P) ((X) + (((P) * ((Y) - (X))) >> 8))

static double r_;

static unsigned char ChessBoard(unsigned y, unsigned x, unsigned char a,
                                unsigned char b) {
  return !((y ^ x) & (1u << 2)) ? a : b;
}

static unsigned char AlphaBackground(unsigned y, unsigned x) {
  return ChessBoard(y, x, 255, 200);
}

static unsigned char OutOfBoundsBackground(unsigned y, unsigned x) {
  return ChessBoard(y, x, 40, 80);
}

static unsigned char Opacify(CHAR w, const unsigned char P[1u << w][1u << w],
                             const unsigned char A[1u << w][1u << w], LONG yn,
                             LONG xn, long y, long x) {
  if ((0 <= y && y < yn) && (0 <= x && x < xn)) {
    return LERP(AlphaBackground(y, x), P[y][x], A[y][x]);
  } else {
    return OutOfBoundsBackground(y, x);
  }
}

static void PrintImage(CHAR w, unsigned char R[1u << w][1u << w],
                       unsigned char G[1u << w][1u << w],
                       unsigned char B[1u << w][1u << w],
                       unsigned char A[1u << w][1u << w], LONG yn, LONG xn) {
  bool didhalfy;
  long y, x;
  didhalfy = false;
  for (y = 0; y < yn; y += 2) {
    if (y) printf("\e[0m\n");
    for (x = 0; x < xn; ++x) {
      printf("\e[48;2;%d;%d;%d;38;2;%d;%d;%dm▄",
             Opacify(w, R, A, yn, xn, y + 0, x),
             Opacify(w, G, A, yn, xn, y + 0, x),
             Opacify(w, B, A, yn, xn, y + 0, x),
             Opacify(w, R, A, yn, xn, y + 1, x),
             Opacify(w, G, A, yn, xn, y + 1, x),
             Opacify(w, B, A, yn, xn, y + 1, x));
    }
    if (y == 0) {
      printf("\e[0m‾0");
    } else if (yn / 2 <= y && y <= yn / 2 + 1 && !didhalfy) {
      printf("\e[0m‾%s%s", "yn/2", y % 2 ? "+1" : "");
      didhalfy = true;
    } else if (y + 1 == yn / 2 && !didhalfy) {
      printf("\e[0m⎯yn/2");
      didhalfy = true;
    } else if (y + 1 == yn) {
      printf("\e[0m⎯yn");
    } else if (y + 2 == yn) {
      printf("\e[0m_yn");
    } else if (!(y % 10)) {
      printf("\e[0m‾%,u", y);
    }
  }
  printf("\e[0m\n");
}

static void DeblinterlaceRgba(CHAR w, unsigned char R[1u << w][1u << w],
                              unsigned char G[1u << w][1u << w],
                              unsigned char B[1u << w][1u << w],
                              unsigned char A[1u << w][1u << w], LONG yn,
                              LONG xn, const unsigned char src[yn][xn][4]) {
  long y, x;
  for (y = 0; y < yn; ++y) {
    for (x = 0; x < xn; ++x) {
      R[y][x] = src[y][x][0];
      G[y][x] = src[y][x][1];
      B[y][x] = src[y][x][2];
      A[y][x] = src[y][x][3];
    }
  }
}

static void SharpenX(CHAR w, unsigned char dst[1u << w][1u << w],
                     const unsigned char src[1u << w][1u << w], char yw,
                     char xw) {
  int y, x;
  for (y = 0; y < (1u << yw); ++y) {
    for (x = 0; x < (1u << xw); ++x) {
      dst[y][x] = C161(src[y][MAX(0, x - 1)], src[y][x],
                       src[y][MIN((1u << xw) - 1, x + 1)]);
    }
  }
}

static void SharpenY(CHAR w, unsigned char dst[1u << w][1u << w],
                     const unsigned char src[1u << w][1u << w], char yw,
                     char xw) {
  int y, x;
  for (y = 0; y < (1u << yw); ++y) {
    for (x = 0; x < (1u << xw); ++x) {
      dst[y][x] = C161(src[MAX(0, y - 1)][x], src[y][x],
                       src[MIN((1u << yw) - 1, y + 1)][x]);
    }
  }
}

static void UpscaleX(CHAR w, unsigned char img[1u << w][1u << w], char yw,
                     char xw) {
  long y, x;
  for (y = (1u << yw); y--;) {
    for (x = (1u << xw); --x;) {
      img[y][x] =
          C13(img[y][MIN(((1u << xw) >> 1) - 1, (x >> 1) - 1 + (x & 1) * 2)],
              img[y][x >> 1]);
    }
  }
}

static void UpscaleY(CHAR w, unsigned char img[1u << w][1u << w], char yw,
                     char xw) {
  long y, x;
  for (y = (1u << yw); --y;) {
    for (x = (1u << xw); x--;) {
      img[y][x] =
          C13(img[MIN(((1u << yw) >> 1) - 1, (y >> 1) - 1 + (y & 1) * 2)][x],
              img[y >> 1][x]);
    }
  }
}

static void Upscale(CHAR w, unsigned char img[1u << w][1u << w],
                    unsigned char tmp[1u << w][1u << w], char yw, char xw) {
  UpscaleY(w, img, yw, xw - 1);
  SharpenY(w, tmp, img, yw, xw - 1);
  UpscaleX(w, tmp, yw, xw);
  SharpenX(w, img, tmp, yw, xw);
}

#if 0
static void *MagikarpY(CHAR w, unsigned char p[1u << w][1u << w], char yw,
                       char xw) {
  long y, x, yn, xn, ym;
  unsigned char(*t)[(1u << w) + 2][1u << w];
  t = memalign(64, ((1u << w) + 2) * (1u << w));
  memset(t, 0, ((1u << w) + 2) * (1u << w));
  yn = 1u << yw;
  xn = 1u << xw;
  ym = yn >> 1;
  for (y = 0; y < ym; ++y) {
    for (x = 0; x < xn; ++x) {
      (*t)[y + 1][x] =
          C1331(y ? p[(y << 1) - 1][x] : 0, p[y << 1][x], p[(y << 1) + 1][x],
                p[MIN(yn - 1, (y << 1) + 2)][x]);
    }
  }
  for (y = 0; y < ym; ++y) {
    for (x = 0; x < xn; ++x) {
      p[y][x] =
          C161((*t)[y + 1 - 1][x], (*t)[y + 1 + 0][x], (*t)[y + 1 + 1][x]);
    }
  }
  free(t);
  return p;
}
static void *MagikarpX(CHAR w, unsigned char p[1u << w][1u << w], char yw,
                       char xw) {
  int y, x;
  LONG yn, xn, xm;
  yn = 1u << yw;
  xn = 1u << xw;
  xm = xn >> 1;
  for (x = 0; x < xm; ++x) {
    for (y = 0; y < yn; ++y) {
      p[y][(xn - xm - 1) + (xm - x - 1)] =
          C1331(p[y][MAX(00 + 0, xn - (x << 1) - 1 + (xn & 1) - 1)],
                p[y][MIN(xn - 1, xn - (x << 1) - 1 + (xn & 1) + 0)],
                p[y][MIN(xn - 1, xn - (x << 1) - 1 + (xn & 1) + 1)],
                p[y][MIN(xn - 1, xn - (x << 1) - 1 + (xn & 1) + 2)]);
    }
  }
  for (x = 0; x < xm; ++x) {
    for (y = 0; y < yn; ++y) {
      p[y][x] = C161(p[y][MAX(xn - 1 - xm, xn - xm - 1 + x - 1)],
                     p[y][MIN(xn - 1 - 00, xn - xm - 1 + x + 0)],
                     p[y][MIN(xn - 1 - 00, xn - xm - 1 + x + 1)]);
    }
  }
  return p;
}
static void ProcessImageVerbatim(LONG yn, LONG xn,
                                 unsigned char img[yn][xn][4]) {
  CHAR w;
  void *R, *G, *B, *A;
  w = roundup2log(MAX(yn, xn));
  R = xvalloc((1u << w) * (1u << w));
  G = xvalloc((1u << w) * (1u << w));
  B = xvalloc((1u << w) * (1u << w));
  A = xvalloc((1u << w) * (1u << w));
  DeblinterlaceRgba(w, R, G, B, A, yn, xn, img);
  PrintImage(w, R, G, B, A, yn, xn);
  free(R);
  free(G);
  free(B);
  free(A);
}
static void ProcessImageDouble(LONG yn, LONG xn, unsigned char img[yn][xn][4]) {
  CHAR w;
  void *t, *R, *G, *B, *A;
  w = roundup2log(MAX(yn, xn)) + 1;
  t = xvalloc((1u << w) * (1u << w));
  R = xvalloc((1u << w) * (1u << w));
  G = xvalloc((1u << w) * (1u << w));
  B = xvalloc((1u << w) * (1u << w));
  A = xvalloc((1u << w) * (1u << w));
  DeblinterlaceRgba(w, R, G, B, A, yn, xn, img);
  Upscale(w, R, t, w, w);
  Upscale(w, G, t, w, w);
  Upscale(w, B, t, w, w);
  Upscale(w, A, t, w, w);
  free(t);
  PrintImage(w, R, G, B, A, yn * 2, xn * 2);
  free(R);
  free(G);
  free(B);
  free(A);
}
static void ProcessImageHalf(LONG yn, LONG xn, unsigned char img[yn][xn][4]) {
  CHAR w;
  void *R, *G, *B, *A;
  w = roundup2log(MAX(yn, xn));
  R = xvalloc((1u << w) * (1u << w));
  G = xvalloc((1u << w) * (1u << w));
  B = xvalloc((1u << w) * (1u << w));
  A = xvalloc((1u << w) * (1u << w));
  DeblinterlaceRgba(w, R, G, B, A, yn, xn, img);
  MagikarpY(w, R, w, w);
  MagikarpY(w, G, w, w);
  MagikarpY(w, B, w, w);
  MagikarpY(w, A, w, w);
  MagikarpX(w, R, w - 1, w);
  MagikarpX(w, G, w - 1, w);
  MagikarpX(w, B, w - 1, w);
  MagikarpX(w, A, w - 1, w);
  PrintImage(w, R, G, B, A, yn >> 1, xn >> 1);
  free(R);
  free(G);
  free(B);
  free(A);
}
static void ProcessImageHalfY(LONG yn, LONG xn, unsigned char img[yn][xn][4]) {
  CHAR w;
  void *R, *G, *B, *A;
  w = roundup2log(MAX(yn, xn));
  R = xvalloc((1u << w) * (1u << w));
  G = xvalloc((1u << w) * (1u << w));
  B = xvalloc((1u << w) * (1u << w));
  A = xvalloc((1u << w) * (1u << w));
  DeblinterlaceRgba(w, R, G, B, A, yn, xn, img);
  MagikarpY(w, R, w, w);
  MagikarpY(w, G, w, w);
  MagikarpY(w, B, w, w);
  MagikarpY(w, A, w, w);
  PrintImage(w, R, G, B, A, yn >> 1, xn);
  free(R);
  free(G);
  free(B);
  free(A);
}
static void ProcessImageHalfLanczos(LONG yn, LONG xn,
                                    unsigned char img[yn][xn][4]) {
  CHAR w;
  void *t, *R, *G, *B, *A;
  t = xvalloc((yn >> 1) * (xn >> 1) * 4);
  lanczos1b(yn >> 1, xn >> 1, t, yn, xn, &img[0][0][0]);
  w = roundup2log(MAX(yn >> 1, xn >> 1));
  R = xvalloc((1u << w) * (1u << w));
  G = xvalloc((1u << w) * (1u << w));
  B = xvalloc((1u << w) * (1u << w));
  A = xvalloc((1u << w) * (1u << w));
  DeblinterlaceRgba(w, R, G, B, A, yn >> 1, xn >> 1, img);
  free(t);
  PrintImage(w, R, G, B, A, yn >> 1, xn >> 1);
  free(R);
  free(G);
  free(B);
  free(A);
}
static void ProcessImageWash(LONG yn, LONG xn, unsigned char img[yn][xn][4]) {
  long w;
  void *R, *G, *B, *A, *t;
  w = roundup2log(MAX(yn, xn)) + 1;
  t = xvalloc((1u << w) * (1u << w));
  R = xvalloc((1u << w) * (1u << w));
  G = xvalloc((1u << w) * (1u << w));
  B = xvalloc((1u << w) * (1u << w));
  A = xvalloc((1u << w) * (1u << w));
  DeblinterlaceRgba(w, R, G, B, A, yn, xn, img);
  Upscale(w, R, t, w, w);
  Upscale(w, G, t, w, w);
  Upscale(w, B, t, w, w);
  Upscale(w, A, t, w, w);
  MagikarpY(w, R, w, w);
  MagikarpY(w, G, w, w);
  MagikarpY(w, B, w, w);
  MagikarpY(w, A, w, w);
  MagikarpX(w, R, w - 1, w);
  MagikarpX(w, G, w - 1, w);
  MagikarpX(w, B, w - 1, w);
  MagikarpX(w, A, w - 1, w);
  free(t);
  PrintImage(w, R, G, B, A, yn, xn);
  free(R);
  free(G);
  free(B);
  free(A);
}
#endif

#if 0
static void *MagikarpY(size_t ys, size_t xs, unsigned char p[ys][xs], size_t yn,
                       size_t xn) {
  int y, x, h, b;
  b = yn % 2;
  h = yn / 2;
  if (b && yn < ys) yn++;
  for (y = b; y < h + b; ++y) {
    for (x = 0; x < xn; ++x) {
      p[(yn - h - 1) + (h - y - 1)][x] =
          C1331(p[MAX(00 + 0, yn - y * 2 - 1 - 1)][x],
                p[MIN(yn - 1, yn - y * 2 - 1 + 0)][x],
                p[MIN(yn - 1, yn - y * 2 - 1 + 1)][x],
                p[MIN(yn - 1, yn - y * 2 - 1 + 2)][x]);
    }
  }
  for (y = b; y < h + b; ++y) {
    for (x = 0; x < xn; ++x) {
      p[y][x] = C161(p[MAX(yn - 1 - h, yn - h - 1 + y - 1)][x],
                     p[MIN(yn - 1 - 0, yn - h - 1 + y + 0)][x],
                     p[MIN(yn - 1 - 0, yn - h - 1 + y + 1)][x]);
    }
  }
  return p;
}
#endif

#if 0
static void *MagikarpX(size_t ys, size_t xs, unsigned char p[ys][xs], size_t yn,
                       size_t xn) {
  int y, x, w, b;
  b = xn % 2;
  w = xn / 2;
  if (b && xn < xs) xn++;
  for (x = 0; x < w; ++x) {
    for (y = b; y < yn + b; ++y) {
      p[y][(xn - w - 1) + (w - x - 1)] =
          C1331(p[y][MAX(00 + 0, xn - x * 2 - 1 - 1)],
                p[y][MIN(xn - 1, xn - x * 2 - 1 + 0)],
                p[y][MIN(xn - 1, xn - x * 2 - 1 + 1)],
                p[y][MIN(xn - 1, xn - x * 2 - 1 + 2)]);
    }
  }
  for (x = 0; x < w; ++x) {
    for (y = b; y < yn + b; ++y) {
      p[y][x] = C161(p[y][MAX(xn - 1 - w, xn - w - 1 + x - 1)],
                     p[y][MIN(xn - 1 - 0, xn - w - 1 + x + 0)],
                     p[y][MIN(xn - 1 - 0, xn - w - 1 + x + 1)]);
    }
  }
  return p;
}
#endif

#if 0
static void ProcessImageMagikarpImpl(CHAR sw,
                                     unsigned char src[5][1u << sw][1u << sw],
                                     LONG syn, LONG sxn,
                                     const unsigned char img[syn][sxn][4],
                                     LONG dyn, LONG dxn) {
  DeblinterlaceRgba2(sw, src, syn, sxn, img);
  MagikarpY(sw, src[0], sw, sw);
  MagikarpX(sw, src[0], sw - 1, sw);
  MagikarpY(sw, src[1], sw, sw);
  MagikarpX(sw, src[1], sw - 1, sw);
  MagikarpY(sw, src[2], sw, sw);
  MagikarpX(sw, src[2], sw - 1, sw);
  BilinearScale(sw, src[4], sw, src[3], dyn, dxn, syn, sxn);
  memcpy(src[3], src[4], syn * sxn);
  PrintImage2(sw, src, dyn, dxn);
}
static void ProcessImageMagikarp(LONG syn, LONG sxn,
                                 unsigned char img[syn][sxn][4]) {
  CHAR sw;
  LONG dyn, dxn;
  dyn = syn >> 1;
  dxn = sxn >> 1;
  sw = roundup2log(MAX(syn, sxn));
  ProcessImageMagikarpImpl(sw, gc(xvalloc((1u << sw) * (1u << sw) * 5)), syn,
                           sxn, img, dyn, dxn);
}
#endif

/*
********************************************************************************
*/

static unsigned char Opacify2(unsigned yw, unsigned xw,
                              const unsigned char P[yw][xw],
                              const unsigned char A[yw][xw], unsigned yn,
                              unsigned xn, unsigned y, unsigned x) {
  if ((0 <= y && y < yn) && (0 <= x && x < xn)) {
    return LERP(AlphaBackground(y, x), P[y][x], A[y][x]);
  } else {
    return OutOfBoundsBackground(y, x);
  }
}

static noinline void PrintImage2(unsigned yw, unsigned xw,
                                 unsigned char img[4][yw][xw], unsigned yn,
                                 unsigned xn) {
  bool didhalfy;
  unsigned y, x;
  didhalfy = false;
  for (y = 0; y < yn; y += 2) {
    if (y) printf("\e[0m\n");
    for (x = 0; x < xn; ++x) {
      printf("\e[48;2;%d;%d;%d;38;2;%d;%d;%dm▄",
             Opacify2(yw, xw, img[0], img[3], yn, xn, y + 0, x),
             Opacify2(yw, xw, img[1], img[3], yn, xn, y + 0, x),
             Opacify2(yw, xw, img[2], img[3], yn, xn, y + 0, x),
             Opacify2(yw, xw, img[0], img[3], yn, xn, y + 1, x),
             Opacify2(yw, xw, img[1], img[3], yn, xn, y + 1, x),
             Opacify2(yw, xw, img[2], img[3], yn, xn, y + 1, x));
    }
    if (y == 0) {
      printf("\e[0m‾0");
    } else if (yn / 2 <= y && y <= yn / 2 + 1 && !didhalfy) {
      printf("\e[0m‾%s%s", "yn/2", y % 2 ? "+1" : "");
      didhalfy = true;
    } else if (y + 1 == yn / 2 && !didhalfy) {
      printf("\e[0m⎯yn/2");
      didhalfy = true;
    } else if (y + 1 == yn) {
      printf("\e[0m⎯yn");
    } else if (y + 2 == yn) {
      printf("\e[0m_yn");
    } else if (!(y % 10)) {
      printf("\e[0m‾%,u", y);
    }
  }
  printf("\e[0m\n");
}

static noinline void *DeblinterlaceRgba2(unsigned yn, unsigned xn,
                                         unsigned char D[4][yn][xn],
                                         const unsigned char S[yn][xn][4]) {
  unsigned y, x;
  for (y = 0; y < yn; ++y) {
    for (x = 0; x < xn; ++x) {
      D[0][y][x] = S[y][x][0];
      D[1][y][x] = S[y][x][1];
      D[2][y][x] = S[y][x][2];
      D[3][y][x] = S[y][x][3];
    }
  }
  return D;
}

void ProcessImageBilinearImpl(unsigned dyn, unsigned dxn,
                              unsigned char dst[4][dyn][dxn], unsigned syn,
                              unsigned sxn, unsigned char src[4][syn][sxn],
                              unsigned char img[syn][sxn][4]) {
  DeblinterlaceRgba2(syn, sxn, src, img);
  BilinearScale(4, dyn, dxn, dst, 4, syn, sxn, src, 0, 4, dyn, dxn, syn, sxn,
                r_, r_, 0, 0);
  PrintImage2(dyn, dxn, dst, dyn, dxn);
}

void ProcessImageBilinear(unsigned yn, unsigned xn,
                          unsigned char img[yn][xn][4]) {
  unsigned dyn, dxn;
  dyn = lround(yn / r_);
  dxn = lround(xn / r_);
  ProcessImageBilinearImpl(dyn, dxn, gc(xmalloc(dyn * dxn * 4)), yn, xn,
                           gc(xmalloc(yn * xn * 4)), img);
}

static void *b2f(long n, float dst[n], const unsigned char src[n]) {
  long i;
  float f;
  for (i = 0; i < n; ++i) {
    f = src[i];
    f *= 1 / 255.;
    dst[i] = f;
  }
  return dst;
}

static void *f2b(long n, unsigned char dst[n], const float src[n]) {
  int x;
  long i;
  for (i = 0; i < n; ++i) {
    x = lroundf(src[i] * 255);
    dst[i] = MIN(255, MAX(0, x));
  }
  return dst;
}

void ProcessImageGyarados(unsigned yn, unsigned xn,
                          unsigned char img[yn][xn][4]) {
  unsigned dyn, dxn;
  dyn = lround(yn / r_);
  dxn = lround(xn / r_);
  PrintImage2(
      dyn, dxn,
      EzGyarados(4, dyn, dxn, gc(xmalloc(dyn * dxn * 4)), 4, yn, xn,
                 DeblinterlaceRgba2(yn, xn, gc(xmalloc(yn * xn * 4)), img), 0,
                 4, dyn, dxn, yn, xn, r_, r_, 0, 0),
      dyn, dxn);
}

void MagikarpDecimate(int yw, int xw, unsigned char img[4][yw][xw], int yn,
                      int xn, int n) {
  int c;
  if (n <= 1) {
    PrintImage2(yw, xw, img, yn, xn);
  } else {
    for (c = 0; c < 4; ++c) Magikarp2xY(yw, xw, img[c], yn, xn);
    for (c = 0; c < 4; ++c) Magikarp2xX(yw, xw, img[c], (yn + 1) / 2, xn);
    MagikarpDecimate(yw, xw, img, (yn + 1) / 2, (xn + 1) / 2, (n + 1) / 2);
  }
}

void ProcessImageMagikarp(unsigned yn, unsigned xn,
                          unsigned char img[yn][xn][4]) {
  MagikarpDecimate(yn, xn,
                   DeblinterlaceRgba2(yn, xn, gc(xmalloc(yn * xn * 4)), img),
                   yn, xn, lround(r_));
}

noinline void WithImageFile(const char *path,
                            void fn(unsigned yn, unsigned xn,
                                    unsigned char img[yn][xn][4])) {
  struct stat st;
  int fd, yn, xn;
  void *map, *data;
  CHECK_NE(-1, (fd = open(path, O_RDONLY)), "%s", path);
  CHECK_NE(-1, fstat(fd, &st));
  CHECK_GT(st.st_size, 0);
  CHECK_LE(st.st_size, INT_MAX);
  fadvise(fd, 0, 0, MADV_WILLNEED | MADV_SEQUENTIAL);
  CHECK_NE(MAP_FAILED,
           (map = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0)));
  CHECK_NOTNULL(
      (data = stbi_load_from_memory(map, st.st_size, &xn, &yn, NULL, 4)), "%s",
      path);
  CHECK_NE(-1, munmap(map, st.st_size));
  CHECK_NE(-1, close(fd));
  fn(yn, xn, data);
  free(data);
}

int main(int argc, char *argv[]) {
  int i, opt;
  bool bilinear;
  void (*scaler)(unsigned yn, unsigned xn, unsigned char[yn][xn][4]) =
      ProcessImageMagikarp;
  r_ = 2;
  while ((opt = getopt(argc, argv, "mlsSybr:")) != -1) {
    switch (opt) {
      case 'r':
        r_ = strtod(optarg, NULL);
        break;
      case 'm':
        scaler = ProcessImageMagikarp;
        break;
      case 's':
      case 'S':
        scaler = ProcessImageGyarados;
        break;
      case 'b':
        scaler = ProcessImageBilinear;
        break;
      default:
        break;
    }
  }
  showcrashreports();
  for (i = optind; i < argc; ++i) {
    WithImageFile(argv[i], scaler);
  }
  return 0;
}
