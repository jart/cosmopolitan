/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "dsp/core/half.h"
#include "dsp/core/ks8.h"
#include "dsp/core/kss8.h"
#include "dsp/scale/cdecimate2xuint8x8.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/str/str.h"
#include "libc/x/x.h"

/**
 * @fileoverview Magikarp resizes graphics in half very fast.
 * @note H/T John Costella, Facebook, and Photoshop
 * @note sharpening is good for luma but not chroma
 * @see Gyarados
 */

#define CLAMP(X) MIN(255, MAX(0, X))
#define MAGIKARP(...) \
  CLAMP(KS8(5, K[0], K[1], K[2], K[3], K[4], K[5], K[6], K[7], __VA_ARGS__))

signed char g_magikarp[8];
const signed char kMagikarp[8][8] = {
    {-1, -1, 3, 15, 15, 3, -1, -1},  // magic kernel sharp
    {-1, -3, 6, 28, 6, -3, -1, 0},   //
    {0, 0, -11, 53, -11, 0, 0, 0},   //
    {-2, -6, 2, 22, 22, 2, -6, -2},  //
    {-3, -9, 1, 27, 27, 1, -9, -3},  //
};

signed char g_magkern[8];
const signed char kMagkern[8][8] = {
    {1, 2, 3, 10, 10, 3, 2, 1},
    {0, 4, 4, 16, 4, 4, 0, 0},
    {0, 1, 2, 6, 14, 6, 2, 1},
    {0, 1, 2, 13, 13, 2, 1, 0},
};

void *Magikarp2xX(long ys, long xs, unsigned char p[ys][xs], long yn, long xn) {
  long y;
  if (yn && xn > 1) {
    for (y = 0; y < yn; ++y) {
      /* gcc/clang both struggle with left-to-right matrix ops */
      cDecimate2xUint8x8(xn, p[y], g_magikarp);
    }
  }
  return p;
}

void *Magikarp2xY(long ys, long xs, unsigned char p[ys][xs], long yn, long xn) {
  long y, x, h;
  signed char K[8];
  memcpy(K, g_magikarp, sizeof(K));
  for (h = HALF(yn), y = 0; y < h; ++y) {
    for (x = 0; x < xn; ++x) {
      p[y][x] = /* gcc/clang are good at optimizing top-to-bottom matrix ops */
          MAGIKARP(p[MAX(00 + 0, y * 2 - 3)][x], p[MAX(00 + 0, y * 2 - 2)][x],
                   p[MAX(00 + 0, y * 2 - 1)][x], p[MIN(yn - 1, y * 2 + 0)][x],
                   p[MIN(yn - 1, y * 2 + 1)][x], p[MIN(yn - 1, y * 2 + 2)][x],
                   p[MIN(yn - 1, y * 2 + 3)][x], p[MIN(yn - 1, y * 2 + 4)][x]);
    }
  }
  return p;
}

void *Magkern2xX(long ys, long xs, unsigned char p[ys][xs], long yn, long xn) {
  long y;
  if (yn && xn > 1) {
    for (y = 0; y < yn; ++y) {
      cDecimate2xUint8x8(xn, p[y], g_magkern);
    }
  }
  return p;
}

void *Magkern2xY(long ys, long xs, unsigned char p[ys][xs], long yn, long xn) {
  long y, x, h;
  signed char K[8];
  memcpy(K, g_magkern, sizeof(K));
  for (h = HALF(yn), y = 0; y < h; ++y) {
    for (x = 0; x < xn; ++x) {
      p[y][x] =
          MAGIKARP(p[MAX(00 + 0, y * 2 - 3)][x], p[MAX(00 + 0, y * 2 - 2)][x],
                   p[MAX(00 + 0, y * 2 - 1)][x], p[MIN(yn - 1, y * 2 + 0)][x],
                   p[MIN(yn - 1, y * 2 + 1)][x], p[MIN(yn - 1, y * 2 + 2)][x],
                   p[MIN(yn - 1, y * 2 + 3)][x], p[MIN(yn - 1, y * 2 + 4)][x]);
    }
  }
  return p;
}

void *MagikarpY(long dys, long dxs, unsigned char d[restrict dys][dxs],
                long sys, long sxs, const unsigned char s[sys][sxs], long yn,
                long xn, const signed char K[8]) {
  long y, x;
  for (y = 0; y < yn; ++y) {
    for (x = 0; x < xn; ++x) {
      d[y][x] = MAGIKARP(s[MAX(00 + 0, y - 3)][x], s[MAX(00 + 0, y - 2)][x],
                         s[MAX(00 + 0, y - 1)][x], s[MIN(yn - 1, y + 0)][x],
                         s[MIN(yn - 1, y + 1)][x], s[MIN(yn - 1, y + 2)][x],
                         s[MIN(yn - 1, y + 3)][x], s[MIN(yn - 1, y + 4)][x]);
    }
  }
  return d;
}

static textstartup void g_magikarp_init() {
  memcpy(g_magkern, kMagkern[0], sizeof(g_magkern));
  memcpy(g_magikarp, kMagikarp[0], sizeof(g_magikarp));
}
const void *const g_magikarp_ctor[] initarray = {g_magikarp_init};
