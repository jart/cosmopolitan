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
#include "dsp/core/core.h"
#include "dsp/tty/quant.h"
#include "libc/assert.h"
#include "libc/limits.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/str/str.h"

#define CUTOFF_VALUE 200

#define DIST(X, Y)   ((X) - (Y))
#define SQR(X)       ((X) * (X))
#define SUM(X, Y, Z) ((X) + (Y) + (Z))

const uint8_t kXtermCube[6] = {0, 0137, 0207, 0257, 0327, 0377};
struct TtyRgb g_ansi2rgb_[256];
double g_xterm256_gamma;

struct TtyRgb tty2rgb_(struct TtyRgb rgbxt) {
  return g_ansi2rgb_[rgbxt.xt];
}

ttyrgb_m128 tty2rgbf_(struct TtyRgb rgbxt) {
  rgbxt = g_ansi2rgb_[rgbxt.xt];
  return (ttyrgb_m128){(int)rgbxt.r, (int)rgbxt.g, (int)rgbxt.b} / 255;
}

static int rgb2xterm256_(int r, int g, int b) {
  int cerr, gerr, ir, ig, ib, gray, grai, cr, cg, cb, gv;
  gray = round(871024 / 4096299. * r + 8788810 / 12288897. * g +
               887015 / 12288897. * b);
  grai = gray > 238 ? 23 : (gray - 3) / 10;
  ir = r < 48 ? 0 : r < 115 ? 1 : (r - 35) / 40;
  ig = g < 48 ? 0 : g < 115 ? 1 : (g - 35) / 40;
  ib = b < 48 ? 0 : b < 115 ? 1 : (b - 35) / 40;
  cr = kXtermCube[ir];
  cg = kXtermCube[ig];
  cb = kXtermCube[ib];
  gv = 8 + 10 * grai;
  cerr = SQR(DIST(cr, r)) + SQR(DIST(cg, g)) + SQR(DIST(cb, b));
  gerr = SQR(DIST(gv, r)) + SQR(DIST(gv, g)) + SQR(DIST(gv, b));
  if (cerr <= gerr) {
    return 16 + 36 * ir + 6 * ig + ib;
  } else {
    return 232 + grai;
  }
}

/**
 * Quantizes RGB to ANSI w/ euclidean distance in 3D color space.
 */
struct TtyRgb rgb2ansi_(int r, int g, int b) {
  uint32_t d, least;
  size_t c, best, min, max;
  r = MAX(MIN(r, 255), 0);
  g = MAX(MIN(g, 255), 0);
  b = MAX(MIN(b, 255), 0);
  min = ttyquant()->min;
  max = ttyquant()->max;
  if (min == 16 && max == 256) {
    return (struct TtyRgb){r, g, b, rgb2xterm256_(r, g, b)};
  } else {
    least = UINT32_MAX;
    best = 0;
    for (c = min; c < max; c++) {
      d = SUM(SQR(DIST(g_ansi2rgb_[c].r, r)), SQR(DIST(g_ansi2rgb_[c].g, g)),
              SQR(DIST(g_ansi2rgb_[c].b, b)));
      if (d < least) {
        least = d;
        best = c;
      }
    }
    return (struct TtyRgb){r, g, b, best};
  }
}

static int uncube(int x) {
  return x < 48 ? 0 : x < 115 ? 1 : (x - 35) / 40;
}

static textstartup void rgb2ansi_init(void) {
  uint8_t c;
  uint32_t i;
  memcpy(g_ansi2rgb_, &kCgaPalette, sizeof(kCgaPalette));
  for (i = 16; i < 232; ++i) {
    g_ansi2rgb_[i].r = kXtermCube[((i - 020) / 044) % 06];
    g_ansi2rgb_[i].g = kXtermCube[((i - 020) / 06) % 06];
    g_ansi2rgb_[i].b = kXtermCube[(i - 020) % 06];
    g_ansi2rgb_[i].xt = i;
  }
  for (i = 232, c = 8; i < 256; i++, c += 10) {
    g_ansi2rgb_[i].r = c;
    g_ansi2rgb_[i].g = c;
    g_ansi2rgb_[i].b = c;
    g_ansi2rgb_[i].xt = i;
  }
}

const void *const rgb2ansi_init_ctor[] initarray = {rgb2ansi_init};
