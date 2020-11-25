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
#include "libc/runtime/runtime.h"
#include "third_party/avir/lanczos1f.h"
namespace {
#include "third_party/avir/lanczos1f.hpp"
}  // namespace

void lanczos1finit(struct lanczos1f *resizer) {
  lanczos1ffree(resizer);
  resizer->p = new Lanczos1fImpl;
}

void lanczos1ffree(struct lanczos1f *resizer) {
  Lanczos1fImpl *impl;
  if (!resizer->p) return;
  impl = (Lanczos1fImpl *)resizer->p;
  delete impl;
  resizer->p = nullptr;
}

/**
 * Resizes image plane w/ Lanczos interpolation, e.g.
 *
 *   struct lanczos1f scaler = {0};
 *   lanczos1finit(&scaler);
 *   lanczos1f(&scaler, ...);
 *   lanczos1ffree(&scaler);
 *
 * @param dyn is destination height
 * @param dxn is destination width
 * @param dst is destination unsigned char array
 * @param syn is source height
 * @param sxn is source width
 * @param ssw is number of unsigned chars per scanline in src
 * @param src is source unsigned char array
 */
void lanczos1f(struct lanczos1f *resizer, size_t dyn, size_t dxn, void *dst,
               size_t syn, size_t sxn, size_t ssw, const void *src, double ky0,
               double kx0, double oy, double ox) {
  Lanczos1fImpl *impl;
  impl = (Lanczos1fImpl *)resizer->p;
  impl->lanczos.resizeImage((const float *)src, sxn, syn, ssw, (float *)dst,
                            dxn, dyn, 1, kx0, ky0, ox, oy);
}
