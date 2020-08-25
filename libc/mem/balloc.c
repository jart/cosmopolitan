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
#include "libc/mem/mem.h"
#include "libc/runtime/buffer.h"

/* TODO(jart): Delete */

#define kGuard PAGESIZE
#define kGrain FRAMESIZE

/**
 * Allocates page-guarded buffer.
 *
 *     ┌─────────────────────────────────────┬s─i─g─s─e─g─v┐
 *     │   𝑣₀..𝑣ₙ₋₁                          │𝑣ₙ..𝑣ₙ₊₄₀₉₆₋₁│
 *     └─────────────────────────────────────┴s─i─g─s─e─g─v┘
 *
 * @param b is metadata object owned by caller, initialized to zero for
 *     first call; subsequent calls will resize
 * @param a is alignment requirement in bytes, e.g. 1,2,4,8,16,...
 * @param n is buffer size in bytes
 * @return b->p
 * @see ralloc()
 */
void *balloc(struct GuardedBuffer *b, unsigned a, size_t n) {
  return (b->p = memalign(a, n));
}
