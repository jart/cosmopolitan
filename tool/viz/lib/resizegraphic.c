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
#include "libc/assert.h"
#include "libc/bits/xmmintrin.internal.h"
#include "libc/runtime/buffer.h"
#include "tool/viz/lib/graphic.h"

/**
 * Allocates graphic.
 *
 * @param g should be zero initialized before first call
 * @note bfree(g->b) needs to be called later
 */
struct Graphic *resizegraphic(struct Graphic *g, size_t yn, size_t xn) {
  /* assert(xn % 2 == 0); */ /* todo: ughhh this whole thing is wrong */
  yn &= ~1;
  balloc(&g->b, 64, yn * xn * sizeof(__m128) + /* wut */ PAGESIZE);
  g->yn = yn;
  g->xn = xn;
  return g;
}
