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

/* TODO(jart): DELETE */

#define GUARANTEE_TERMINATOR 1
#define INITIAL_CAPACITY     (32 - GUARANTEE_TERMINATOR)

/**
 * Grows array.
 * @deprecated favor realloc
 */
bool __grow(void *pp, size_t *capacity, size_t itemsize, size_t extra) {
  void **p, *p1, *p2;
  size_t n1, n2;
  size_t t1, t2;
  extra += GUARANTEE_TERMINATOR;
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
