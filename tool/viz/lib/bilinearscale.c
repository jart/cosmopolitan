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
#include "dsp/core/twixt8.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/bsr.h"
#include "libc/runtime/gc.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"
#include "libc/tinymath/emod.h"
#include "libc/x/x.h"
#include "tool/viz/lib/bilinearscale.h"

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
  long c, y, x, b;
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
      DCHECK_LT(bsrl(cn) + bsrl(syn) + bsrl(sxn), 32);
      DCHECK_LT(bsrl(cn) + bsrl(dyn) + bsrl(dxn), 32);
      BilinearScaler(dcw, dyw, dxw, dst, scw, syw, sxw, src, c0, cn, dyn, dxn,
                     syn, sxn, ry, rx, oy, ox,
                     gc(xmemalign(64, ROUNDUP(sizeof(int) * (dyn + 1), 64))),
                     gc(xmemalign(64, ROUNDUP(dyn + 1, 64))),
                     gc(xmemalign(64, ROUNDUP(sizeof(int) * (dxn + 1), 64))),
                     gc(xmemalign(64, ROUNDUP(dxn + 1, 64))),
                     gc(xmemalign(64, ROUNDUP(dxn, 64))),
                     gc(xmemalign(64, ROUNDUP(sxn, 64) * 2)));
    } else {
      memset(dst[c0], 0, &dst[cn][0][0] - &dst[c0][0][0]);
    }
  }
  return dst;
}
