/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=8 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/bits/bits.h"
#include "libc/bits/weaken.h"
#include "libc/fmt/conv.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

#define GUARANTEE_TERMINATOR 1
#define INITIAL_CAPACITY     (32 - GUARANTEE_TERMINATOR)

/**
 * Grows array, The Cosmopolitan Way.
 *
 * This function may be called once an array has run out of space. If p
 * is NULL, a new array is allocated; otherwise, the array's made 1.5x
 * bigger. It has been written that this amortizes list appends down to
 * constant-time. Extended memory is zeroed. Growth is monotonic.
 *
 * If p points to to static memory or something on the stack, it'll be
 * converted to dynamic memory automatically. This can make algorithms
 * faster when the average case is a small amount of data. It also means
 * functions using this (and free_s()) won't have a hard-requirement on
 * malloc().
 *
 * Consider trying the higher-level append() and concat() APIs (defined
 * in libc/alg/arraylist.h) as an alternative to directly using grow().
 *
 * @param pp points to pointer holding memory address
 * @param capacity tracks maximum items that can be stored in p
 *     can only be 0 if p is NULL (see reallocarray() for non-monotonic)
 * @param itemsize is the sizeof each item
 * @return true on success, or false w/ errno and *p is NOT free()'d
 * @error ENOMEM if realloc() not linked or mmap() failed
 * @note tiny programs might need to explicitly YOINK(realloc)
 * @see test/libc/runtime/grow_test.c
 */
bool __grow(void *pp, size_t *capacity, size_t itemsize, size_t extra) {
  void **p, *p1, *p2;
  size_t n1, n2;                 /* item counts */
  size_t t1, t2;                 /* byte counts */
  extra += GUARANTEE_TERMINATOR; /* p ⊃ p[𝑖]==0 */
  p = (void **)pp;
  assert(itemsize);
  assert((*p && *capacity) || (!*p && !*capacity));
  assert(!isheap(*p) || ((intptr_t)*p & 15) == 0);
  p1 = isheap(*p) ? *p : NULL;
  p2 = NULL;
  n1 = *capacity;
  n2 = (*p ? n1 + (n1 >> 1) : MAX(4, INITIAL_CAPACITY / itemsize)) + extra;
  if (!__builtin_mul_overflow(n1, itemsize, &t1) &&
      !__builtin_mul_overflow(n2, itemsize, &t2)) {
    if (weaken(realloc) && (p2 = weaken(realloc)(p1, ROUNDUP(t2, 32)))) {
      if (!p1 && *p) memcpy(p2, *p, t1);
      memset((char *)p2 + t1, 0, t2 - t1);
      *capacity = n2;
      *p = p2;
      return true;
    } else {
      enomem();
    }
  } else {
    eoverflow();
  }
  return false;
}
