/*-*-mode:c++;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8-*-│
│vi: set net ft=c++ ts=2 sts=2 sw=2 fenc=utf-8                              :vi│
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
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "third_party/avir/lanczos.h"
namespace {
#include "third_party/avir/lancir.h"
}  // namespace

/**
 * Does Lanczos interpolation.
 * @note computers w/o AVX2+FMA need to call BilinearScale()
 */
void lanczos(unsigned dyn, unsigned dxn, void *restrict dst, unsigned syn,
             unsigned sxn, const void *restrict src, unsigned sw) {
  avir::CLancIR lanczos;
  DCHECK_ALIGNED(64, dst);
  DCHECK_ALIGNED(64, src);
  LOGF("%10s%5zux×%-5zu→%5zu×%-5zu", "lanczos", sxn, syn, dxn, dyn);
  lanczos.resizeImage((const float *)src, sxn, syn, sw, (float *)dst, dxn, dyn,
                      4);
}
