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
#include "dsp/core/core.h"
#include "dsp/tty/quant.h"
#include "libc/assert.h"
#include "libc/limits.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/math.h"
#include "libc/str/str.h"

#define CUTOFF_VALUE 200

#define DIST(X, Y)   ((X) - (Y))
#define SQR(X)       ((X) * (X))
#define SUM(X, Y, Z) ((X) + (Y) + (Z))

static const uint8_t kXtermCube[] = {0, 0137, 0207, 0257, 0327, 0377};
struct TtyRgb g_ansi2rgb_[256];
static uint8_t g_quant[256];
static uint8_t g_rindex[256];
static uint8_t g_gindex[256];
static uint8_t g_bindex[256];
double g_xterm256_gamma;

struct TtyRgb tty2rgb_(struct TtyRgb rgbxt) {
  return g_ansi2rgb_[rgbxt.xt];
}

__m128 tty2rgbf_(struct TtyRgb rgbxt) {
  rgbxt = g_ansi2rgb_[rgbxt.xt];
  return (__m128){(int)rgbxt.r, (int)rgbxt.g, (int)rgbxt.b} / 255;
}

static int rgb2xterm256_(int r, int g, int b) {
  int cerr, gerr, ir, ig, ib, gray, grai, cr, cg, cb, gv;
  gray = round(r * .299 + g * .587 + b * .114);
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

static optimizesize textstartup void xterm2rgbsetup_(void) {
  uint8_t c, y;
  uint32_t i, j;
  memcpy(g_ansi2rgb_, &kCgaPalette, sizeof(kCgaPalette));
  for (i = 0; i < 256; ++i) {
    j = uncube(i);
    g_quant[i] = kXtermCube[j];
    g_rindex[i] = j * 36;
    g_gindex[i] = j * 6;
    g_bindex[i] = j + 16;
  }
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

INITIALIZER(301, _init_ansi2rgb, xterm2rgbsetup_());
