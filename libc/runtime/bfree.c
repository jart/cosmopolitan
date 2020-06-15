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
#include "libc/runtime/buffer.h"
#include "libc/runtime/mappings.h"

void bfree(struct GuardedBuffer *b) {
  int mr;
  size_t mi;
  struct AddrSize az;
  struct MemoryCoord mc;
  if (b->p) {
    mc = ADDRSIZE_TO_COORD(b->p, 1);
    mi = findmapping(mc.y);
    assert(mi > 0);
    assert(ISOVERLAPPING(mc, _mm.p[mi - 1]));
    az = COORD_TO_ADDRSIZE(_mm.p[mi - 1]);
    mr = munmap(az.addr, az.size);
    assert(mr != -1);
    b->p = NULL;
  }
}
