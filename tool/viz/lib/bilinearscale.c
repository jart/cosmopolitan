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
#include "tool/viz/lib/bilinearscale.h"
#include "dsp/core/twixt8.h"
#include "libc/intrin/bsr.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"
#include "libc/tinymath/emod.h"
#include "libc/x/x.h"

static void ComputeScalingSolution(long dn, long sn, double r, double o,
                                   unsigned char pct[dn + 1], int idx[dn + 1]) {
  long i;
  double x;
  o -= .5;
  o += (r - 1) / 2;
  for (i = 0; i < dn + 1; ++i) {
    x = o + i * r;
    idx[i] = MIN(sn - 1, MAX(0, lround(x)));
    pct[i] = lround((x - idx[i]) * 255) % 256;
  }
}

static void BilinearScaler(long dcw, long dyw, long dxw,
                           unsigned char dst[dcw][dyw][dxw], long scw, long syw,
                           long sxw, const unsigned char src[scw][syw][sxw],
                           long c0, long cn, long dyn, long dxn, long syn,
                           long sxn, double ry, double rx, double oy, double ox,
                           int iy[dyn + 1], unsigned char py[dyn + 1],
                           int ix[dxn + 1], unsigned char px[dxn + 1],
                           unsigned char db[dxn], unsigned char sb[2][sxn]) {
  long c, y, x;
  ComputeScalingSolution(dxn, sxn, rx, ox, px, ix);
  ComputeScalingSolution(dyn, syn, ry, oy, py, iy);
  for (c = c0; c < cn; ++c) {
    for (y = 0; y < dyn; ++y) {
      memcpy(sb[0], src[c][iy[y + 0]], sxn);
      memcpy(sb[1], src[c][iy[y + 1]], sxn);
      for (x = 0; x < dxn; ++x) {
        db[x] = twixt8(twixt8(sb[0][ix[x]], sb[0][ix[x + 1]], px[x]),
                       twixt8(sb[1][ix[x]], sb[1][ix[x + 1]], px[x]), py[y]);
      }
      memcpy(dst[c][y], db, dxn);
    }
  }
}

void *BilinearScale(long dcw, long dyw, long dxw,
                    unsigned char dst[dcw][dyw][dxw], long scw, long syw,
                    long sxw, const unsigned char src[scw][syw][sxw], long c0,
                    long cn, long dyn, long dxn, long syn, long sxn, double ry,
                    double rx, double oy, double ox) {
  if (dyn > 0 && dxn > 0) {
    if (syn > 0 && sxn > 0) {
      DCHECK_LE(c0, cn);
      DCHECK_LE(cn, scw);
      DCHECK_LE(cn, dcw);
      DCHECK_LE(syn, syw);
      DCHECK_LE(sxn, sxw);
      DCHECK_LE(dyn, dyw);
      DCHECK_LE(dxn, dxw);
      DCHECK_LT(_bsrl(cn) + _bsrl(syn) + _bsrl(sxn), 32);
      DCHECK_LT(_bsrl(cn) + _bsrl(dyn) + _bsrl(dxn), 32);
      BilinearScaler(dcw, dyw, dxw, dst, scw, syw, sxw, src, c0, cn, dyn, dxn,
                     syn, sxn, ry, rx, oy, ox,
                     gc(xmemalign(64, ROUNDUP(sizeof(int) * (dyn + 1), 64))),
                     gc(xmemalign(64, ROUNDUP(dyn + 1, 64))),
                     gc(xmemalign(64, ROUNDUP(sizeof(int) * (dxn + 1), 64))),
                     gc(xmemalign(64, ROUNDUP(dxn + 1, 64))),
                     gc(xmemalign(64, ROUNDUP(dxn, 64))),
                     gc(xmemalign(64, ROUNDUP(sxn, 64) * 2)));
    } else {
      bzero(dst[c0], &dst[cn][0][0] - &dst[c0][0][0]);
    }
  }
  return dst;
}
