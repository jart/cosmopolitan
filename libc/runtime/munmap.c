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
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/nt/memory.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/mappings.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"

/**
 * Releases memory pages.
 *
 * @param addr is a pointer within any memory mapped region the process
 *     has permission to control, such as address ranges returned by
 *     mmap(), the program image itself, etc.
 * @param size is the amount of memory to unmap, which should be a
 *     multiple of PAGESIZE, and may be a subset of that which was
 *     mapped previously
 * @return 0 on success, or -1 w/ errno
 */
int __munmap(void *addr, size_t size) {
  int rc;
  intptr_t p;
  size_t i, j;
  struct AddrSize a;
  struct MemoryCoord c, m;

  p = (intptr_t)addr;
  assert(!(0 < p && p < 0x200000));
  assert(-0x800000000000L <= p && p <= 0x7fffffffffffL);

  if (!size) return 0;
  if (!addr || addr == MAP_FAILED) return 0;
  if (addr == NULL && size <= 0x200000) return 0;

  addr = (void *)ROUNDDOWN((intptr_t)addr, FRAMESIZE);
  size = ROUNDUP(size, FRAMESIZE);

  rc = 0;
  c = ADDRSIZE_TO_COORD(addr, size);
  j = findmapping(c.y);
  for (i = j; i; --i) {
    m = _mm.p[i - 1];
    assert(m.x <= m.y);
    assert(c.y >= m.x);
    if (c.x > m.y) {
      break;
    } else if (c.x > m.x && c.y < m.y) {
      /* remove middle portion */
      assert(!"map hole punching not implemented");
    } else if (c.x > m.x && c.y >= m.y) {
      /* remove righthand portion */
      assert(!"map hole punching not implemented");
      /* _mm.p[i - 1].y = c.x - 1; */
      /* i++; */
      /* break; */
    } else if (c.x <= m.x && c.y < m.y) {
      /* remove lefthand portion */
      assert(!"map hole punching not implemented");
      /* _mm.p[i - 1].x = c.y + 1; */
      /* j--; */
    } else if ((m.x >= c.x && m.x <= c.y) && (m.y >= c.x && m.y <= c.y)) {
      a = COORD_TO_ADDRSIZE(m);
      if (!IsWindows()) {
        rc |= munmap$sysv(a.addr, a.size);
      } else {
        if (!UnmapViewOfFile(a.addr)) rc = -1;
        if (!CloseHandle(_mm.h[i - 1])) rc = -1;
      }
    } else {
      assert(!"wut");
    }
  }

  if (i < j) {
    if (j < _mm.i) {
      memmove(&_mm.p[i], &_mm.p[j],
              (intptr_t)&_mm.p[_mm.i] - (intptr_t)&_mm.p[j]);
      memmove(&_mm.h[i], &_mm.h[j],
              (intptr_t)&_mm.h[_mm.i] - (intptr_t)&_mm.h[j]);
    }
    _mm.i -= j - i;
  }

  return rc;
}
