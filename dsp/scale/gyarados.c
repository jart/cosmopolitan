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
#include "dsp/core/c161.h"
#include "dsp/core/core.h"
#include "dsp/core/ituround.h"
#include "dsp/core/q.h"
#include "dsp/core/twixt8.h"
#include "libc/intrin/bsr.h"
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "tool/viz/lib/knobs.h"

/**
 * @fileoverview Gyarados resizes graphics.
 * @note H/T John Costella, Facebook, Photoshop, Carl Friedrich Gauss
 * @note Eric Brasseur has an interesting blog post on tip of iceberg
 * @see Magikarp
 */

#define M      14
#define SQR(X) ((X) * (X))

struct SamplingSolution {
  int n, s;
  void *weights;
  void *indices;
};

static double ComputeWeight(double x) {
  if (-1.5 < x && x < 1.5) {
    if (-.5 < x && x < .5) {
      return .75 - SQR(x);
    } else if (x < 0) {
      return .5 * SQR(x + 1.5);
    } else {
      return .5 * SQR(x - 1.5);
    }
  } else {
    return 0;
  }
}

static struct SamplingSolution *NewSamplingSolution(long n, long s) {
  struct SamplingSolution *ss;
  ss = xcalloc(1, sizeof(struct SamplingSolution));
  ss->n = n;
  ss->s = s;
  ss->weights = xcalloc(n * s, sizeof(short));
  ss->indices = xcalloc(n * s, sizeof(short));
  return ss;
}

static bool IsNormalized(int n, double A[n]) {
  int i;
  double x;
  for (x = i = 0; i < n; ++i) x += A[i];
  return fabs(x - 1) < 1e-4;
}

void FreeSamplingSolution(struct SamplingSolution *ss) {
  if (ss) {
    free(ss->indices);
    free(ss->weights);
    free(ss);
  }
}

struct SamplingSolution *ComputeSamplingSolution(long dn, long sn, double dar,
                                                 double off, double par) {
  double *fweights;
  double sum, hw, x, f;
  short *weights, *indices;
  struct SamplingSolution *res;
  long j, i, k, n, min, max, s, N[6];
  if (!dar) dar = sn, dar /= dn;
  if (!off) off = (dar - 1) / 2;
  f = dar < 1 ? 1 / dar : dar;
  s = 3 * f + 4;
  fweights = gc(xcalloc(s + /*xxx*/ 2, sizeof(double)));
  res = NewSamplingSolution(dn, s);
  weights = res->weights;
  indices = res->indices;
  for (i = 0; i < dn; ++i) {
    x = off + i * dar;
    hw = 1.5 * f;
    min = ceil(x - hw);
    max = floor(x + hw);
    n = max - min + 1;
    CHECK_LE(n, s);
    for (k = 0, j = min; j <= max; ++j) {
      fweights[k++] = ComputeWeight((j - x) / (f / par));
    }
    for (sum = k = 0; k < n; ++k) sum += fweights[k];
    for (j = 0; j < n; ++j) fweights[j] *= 1 / sum;
    DCHECK(IsNormalized(n, fweights));
    for (j = 0; j < n; ++j) {
      indices[i * s + j] = MIN(sn - 1, MAX(0, min + j));
    }
    for (j = 0; j < n; j += 6) {
      GetIntegerCoefficients(N, fweights + j, M, 0, 255);
      for (k = 0; k < MIN(6, n - j); ++k) {
        weights[i * s + j + k] = N[k];
      }
    }
  }
  return res;
}

static void *ZeroMatrix(long yw, long xw, int p[yw][xw], long yn, long xn) {
  long y;
  for (y = 0; y < yn; ++y) {
    memset(p[y], 0, xn);
  }
  return p;
}

static int Sharpen(int ax, int bx, int cx) {
  return (-1 * ax + 6 * bx + -1 * cx + 2) / 4;
}

static void GyaradosImpl(long dyw, long dxw, int dst[dyw][dxw], long syw,
                         long sxw, const int src[syw][sxw], long dyn, long dxn,
                         long syn, long sxn, int tmp0[restrict dyn][sxn],
                         int tmp1[restrict dyn][sxn],
                         int tmp2[restrict dyn][dxn], long yfn, long xfn,
                         const short fyi[dyn][yfn], const short fyw[dyn][yfn],
                         const short fxi[dxn][xfn], const short fxw[dxn][xfn],
                         bool sharpen) {
  long i;
  int eax, dy, dx, sx;
  for (sx = 0; sx < sxn; ++sx) {
    for (dy = 0; dy < dyn; ++dy) {
      for (eax = i = 0; i < yfn; ++i) {
        eax += fyw[dy][i] * src[fyi[dy][i]][sx];
      }
      tmp0[dy][sx] = QRS(M, eax);
    }
  }
  for (dy = 0; dy < dyn; ++dy) {
    for (sx = 0; sx < sxn; ++sx) {
      tmp1[dy][sx] = sharpen ? Sharpen(tmp0[MIN(dyn - 1, MAX(0, dy - 1))][sx],
                                       tmp0[dy][sx],
                                       tmp0[MIN(dyn - 1, MAX(0, dy + 1))][sx])
                             : tmp0[dy][sx];
    }
  }
  for (dx = 0; dx < dxn; ++dx) {
    for (dy = 0; dy < dyn; ++dy) {
      for (eax = i = 0; i < xfn; ++i) {
        eax += fxw[dx][i] * tmp1[dy][fxi[dx][i]];
      }
      tmp2[dy][dx] = QRS(M, eax);
    }
  }
  for (dx = 0; dx < dxn; ++dx) {
    for (dy = 0; dy < dyn; ++dy) {
      dst[dy][dx] = sharpen ? Sharpen(tmp2[dy][MIN(dxn - 1, MAX(0, dx - 1))],
                                      tmp2[dy][dx],
                                      tmp2[dy][MIN(dxn - 1, MAX(0, dx + 1))])
                            : tmp2[dy][dx];
    }
  }
}

/**
 * Scales image.
 *
 * @note gyarados is magikarp in its infinite form
 * @see Magikarp2xY(), Magikarp2xX()
 */
void *Gyarados(long dyw, long dxw, int dst[dyw][dxw], long syw, long sxw,
               const int src[syw][sxw], long dyn, long dxn, long syn, long sxn,
               struct SamplingSolution *cy, struct SamplingSolution *cx,
               bool sharpen) {
  if (dyn > 0 && dxn > 0) {
    if (syn > 0 && sxn > 0) {
      CHECK_LE(syn, syw);
      CHECK_LE(sxn, sxw);
      CHECK_LE(dyn, dyw);
      CHECK_LE(dxn, dxw);
      CHECK_LT(_bsrl(syn) + _bsrl(sxn), 32);
      CHECK_LT(_bsrl(dyn) + _bsrl(dxn), 32);
      CHECK_LE(dyw, 0x7fff);
      CHECK_LE(dxw, 0x7fff);
      CHECK_LE(syw, 0x7fff);
      CHECK_LE(sxw, 0x7fff);
      CHECK_LE(dyn, 0x7fff);
      CHECK_LE(dxn, 0x7fff);
      CHECK_LE(syn, 0x7fff);
      CHECK_LE(sxn, 0x7fff);
      GyaradosImpl(dyw, dxw, dst, syw, sxw, src, dyn, dxn, syn, sxn,
                   gc(xmemalign(64, sizeof(int) * dyn * sxn)),
                   gc(xmemalign(64, sizeof(int) * dyn * sxn)),
                   gc(xmemalign(64, sizeof(int) * dyn * dxn)), cy->s, cx->s,
                   cy->indices, cy->weights, cx->indices, cx->weights, sharpen);
    } else {
      ZeroMatrix(dyw, dxw, dst, dyn, dxn);
    }
  }
  return dst;
}

void *GyaradosUint8(long dyw, long dxw, unsigned char dst[dyw][dxw], long syw,
                    long sxw, const unsigned char src[syw][sxw], long dyn,
                    long dxn, long syn, long sxn, long lo, long hi,
                    struct SamplingSolution *cy, struct SamplingSolution *cx,
                    bool sharpen) {
  static bool once;
  static int Tin[256];
  static unsigned char Tout[32768];
  long i, y, x;
  int(*tmp)[MAX(dyn, syn)][MAX(dxn, sxn)];
  if (!once) {
    for (i = 0; i < ARRAYLEN(Tin); ++i) {
      Tin[i] = F2Q(15, rgb2linpc(i / 255., 2.4));
    }
    for (i = 0; i < ARRAYLEN(Tout); ++i) {
      Tout[i] = MIN(255, MAX(0, round(rgb2stdpc(Q2F(15, i), 2.4) * 255.)));
    }
    once = true;
  }
  tmp = xmemalign(64, sizeof(int) * MAX(dyn, syn) * MAX(dxn, sxn));
  for (y = 0; y < syn; ++y) {
    for (x = 0; x < sxn; ++x) {
      (*tmp)[y][x] = Tin[src[y][x]];
    }
  }
  Gyarados(MAX(dyn, syn), MAX(dxn, sxn), *tmp, MAX(dyn, syn), MAX(dxn, sxn),
           *tmp, dyn, dxn, syn, sxn, cy, cx, sharpen);
  for (y = 0; y < dyn; ++y) {
    for (x = 0; x < dxn; ++x) {
      dst[y][x] = Tout[MIN(32767, MAX(0, (*tmp)[y][x]))];
    }
  }
  free(tmp);
  return dst;
}

void *EzGyarados(long dcw, long dyw, long dxw, unsigned char dst[dcw][dyw][dxw],
                 long scw, long syw, long sxw,
                 const unsigned char src[scw][syw][sxw], long c0, long cn,
                 long dyn, long dxn, long syn, long sxn, double ry, double rx,
                 double oy, double ox) {
  long c;
  struct SamplingSolution *cy, *cx;
  cy = ComputeSamplingSolution(dyn, syn, ry, oy, 1);
  cx = ComputeSamplingSolution(dxn, sxn, rx, ox, 1);
  for (c = c0; c < cn; ++c) {
    GyaradosUint8(dyw, dxw, dst[c], syw, sxw, src[c], dyn, dxn, syn, sxn, 0,
                  255, cy, cx, true);
  }
  FreeSamplingSolution(cx);
  FreeSamplingSolution(cy);
  return dst;
}
