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
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/limits.h"
#include "libc/macros.h"
#include "libc/runtime/ring.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"

/**
 * Allocates ring buffer.
 *
 * Reads/writes wrap around on overflow.
 *
 *     ┌────────────┐
 *     │  𝑓₀..𝑓ₙ₋₁  │
 *     └┬┬──────────┘
 *      │└────────────┐
 *     ┌┴────────────┬┴────────────┐
 *     │   𝑣₀..𝑣ₙ₋₁  │  𝑣ₙ..𝑣ₙ*₂₋₁ │
 *     └─────────────┴─────────────┘
 *
 * @param r is metadata object owned by caller, initialized to zero
 * @param n is byte length
 * @return r->p, or NULL w/ errno
 * @see ringfree(), balloc()
 */
void *ringalloc(struct RingBuffer *r, size_t n) {
  void *a2;
  int fd, rc;
  size_t grain;
  assert(!r->p);
  assert(n > 0);
  assert(n <= (INT_MAX - FRAMESIZE + 1) / 2);
  if ((fd = openanon("ring", 0)) != -1) {
    grain = ROUNDUP(n, FRAMESIZE);
    rc = ftruncate(fd, grain * 2);
    assert(rc != -1);
    r->_size = grain * 2;
    r->_addr = mmap(NULL, grain, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (r->_addr != MAP_FAILED) {
      a2 = mmap(r->_addr + grain, grain, PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_FIXED, fd, grain - n);
      assert(a2 != MAP_FAILED);
      r->p = r->_addr + grain - n;
      if (IsWindows()) {
        memset(r->p, 0, n); /* @see ftruncate() */
      }
    }
  }
  rc = close(fd);
  assert(rc != -1);
  return r->p;
}
