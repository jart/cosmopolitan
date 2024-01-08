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
#include "dsp/tty/quant.h"
#include "libc/intrin/hilbert.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "tool/viz/lib/graphic.h"
#include "tool/viz/lib/knobs.h"

#define CLAMP(X) MIN(255, MAX(0, X))

struct Dither {
  struct {
    int m5[32 * 32][2];
    int m4[16 * 16][2];
    int m3[8 * 8][2];
    int m2[4 * 4][2];
    int m1[2 * 2][2];
    int m0[2 * 2][2];
  } memory;
  struct Chunk {
    long b;
    void *c;
  } chunks[6];
} kDither;

static struct Chunk ChunkInit(long b, int c[b * b][2]) {
  long i;
  axdx_t h;
  for (i = 0; i < b * b; ++i) {
    h = unhilbert(b, i);
    c[i][0] = h.ax;
    c[i][1] = h.dx;
  }
  return (struct Chunk){b, c};
}

static optimizesize void DitherInit(struct Dither *d) {
  d->chunks[0] = ChunkInit(1 << 0, d->memory.m0);
  d->chunks[1] = ChunkInit(1 << 1, d->memory.m1);
  d->chunks[2] = ChunkInit(1 << 2, d->memory.m2);
  d->chunks[3] = ChunkInit(1 << 3, d->memory.m3);
  d->chunks[4] = ChunkInit(1 << 4, d->memory.m4);
  d->chunks[5] = ChunkInit(1 << 5, d->memory.m5);
}

static int GetQuantError(unsigned char r, unsigned char g, unsigned char b) {
  struct TtyRgb q = tty2rgb(rgb2tty(r, g, b));
  return ((r - q.r) + (g - q.g) + (b - q.b)) / 3;
}

static int SerpentineDitherSq2(long yw, long xw, unsigned char rgb[3][yw][xw],
                               long y, long x, long b, const int ci[b * b][2],
                               int e) {
  long i;
  for (i = 0; i < b * b; ++i) {
    e = GetQuantError((rgb[0][y + ci[i][0]][x + ci[i][1]] =
                           CLAMP(rgb[0][y + ci[i][0]][x + ci[i][1]] + e)),
                      (rgb[1][y + ci[i][0]][x + ci[i][1]] =
                           CLAMP(rgb[1][y + ci[i][0]][x + ci[i][1]] + e)),
                      (rgb[2][y + ci[i][0]][x + ci[i][1]] =
                           CLAMP(rgb[2][y + ci[i][0]][x + ci[i][1]] + e))) *
        15 / 16;
  }
  return e;
}

static void SerpentineDither(long yw, long xw, unsigned char rgb[3][yw][xw],
                             long yn, long xn, long y, long x, long r,
                             const struct Dither *d) {
  void *c;
  long b, i, j, n, m;
  b = d->chunks[r].b;
  c = d->chunks[r].c;
  n = (yn - y) / b;
  m = (xn - x) / b;
  for (i = 0; i < n; ++i) {
    for (j = 0; j < m; ++j) {
      SerpentineDitherSq2(yw, xw, rgb, y + i * b, x + j * b, b, c, 0);
    }
  }
  if (r) {
    SerpentineDither(yw, xw, rgb, yn, xn, y + 0 * 0, x + m * b, r - 1, d);
    SerpentineDither(yw, xw, rgb, yn, xn, y + n * b, x + 0 * 0, r - 1, d);
    SerpentineDither(yw, xw, rgb, yn, xn, y + n * b, x + m * b, r - 1, d);
  }
}

/**
 * Makes color banding go away a little in low color modes.
 */
void dither(long yw, long xw, unsigned char rgb[3][yw][xw], long yn, long xn) {
  static bool once;
  if (!once) {
    DitherInit(&kDither);
    once = true;
  }
  SerpentineDither(yw, xw, rgb, yn, xn, 0, 0, ARRAYLEN(kDither.chunks) - 1,
                   &kDither);
}
