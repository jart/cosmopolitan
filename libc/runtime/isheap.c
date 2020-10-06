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
#include "libc/runtime/memtrack.h"
#include "libc/runtime/runtime.h"

/**
 * Returns true if address isn't stack and was malloc'd or mmap'd.
 *
 * @assume stack addresses are always greater than heap addresses
 * @assume stack memory isn't stored beneath %rsp (-mno-red-zone)
 */
bool isheap(void *p) {
  int x, i;
#if 1
  register intptr_t rsp asm("rsp");
  if ((intptr_t)p >= rsp) return false;
#endif
  if ((intptr_t)p <= (intptr_t)_end) return false;
  x = (intptr_t)p >> 16;
  i = FindMemoryInterval(&_mmi, x);
  return i < _mmi.i && x >= _mmi.p[i].x && x <= _mmi.p[i].y;
}
