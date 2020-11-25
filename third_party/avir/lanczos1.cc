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
#include "libc/bits/xmmintrin.internal.h"
#include "libc/limits.h"
#include "libc/log/log.h"
#include "libc/runtime/runtime.h"
#include "third_party/avir/lanczos1.h"
namespace {
#include "third_party/avir/lanczos1.hpp"
}  // namespace

void lanczos1init(struct lanczos1 *resizer) {
  lanczos1free(resizer);
  resizer->p = new Lanczos1Impl;
}

void lanczos1free(struct lanczos1 *resizer) {
  Lanczos1Impl *impl;
  if (!resizer->p) return;
  impl = (Lanczos1Impl *)resizer->p;
  delete impl;
  resizer->p = nullptr;
}

/**
 * Resizes image plane w/ Lanczos interpolation, e.g.
 *
 *   struct lanczos1 scaler = {0};
 *   lanczos1init(&scaler);
 *   lanczos1(&scaler, ...);
 *   lanczos1free(&scaler);
 *
 * @param dyn is destination height
 * @param dxn is destination width
 * @param dst is destination unsigned char array
 * @param dstsize is number of bytes in dst
 * @param syn is source height
 * @param sxn is source width
 * @param ssw is number of unsigned chars per scanline in src
 * @param src is source unsigned char array
 * @param srcsize is number of bytes in src
 */
void lanczos1(struct lanczos1 *resizer, size_t dyn, size_t dxn, void *dst,
              size_t dstsize, size_t syn, size_t sxn, size_t ssw,
              const void *src, size_t srcsize) {
  Lanczos1Impl *impl;
  unsigned int roundhouse;
  LOGF("%10s%5zux×%-5zu→%5zu×%-5zu", "lanczos1", sxn, syn, dxn, dyn);
  CHECK_LE(dstsize, INT_MAX);
  CHECK_LE(srcsize, INT_MAX);
  CHECK_LE(sizeof(unsigned char) * 1 * dyn * dxn, dstsize);
  CHECK_LE(sizeof(unsigned char) * 1 * syn * sxn, srcsize);
  CHECK_LE(sizeof(unsigned char) * syn * ssw, srcsize);
  roundhouse = _MM_GET_ROUNDING_MODE();
  _MM_SET_ROUNDING_MODE(_MM_ROUND_NEAREST);
  impl = (Lanczos1Impl *)resizer->p;
  impl->lanczos.resizeImage((const unsigned char *)src, sxn, syn, ssw,
                            (unsigned char *)dst, dxn, dyn, 1);
  _MM_SET_ROUNDING_MODE(roundhouse);
}
