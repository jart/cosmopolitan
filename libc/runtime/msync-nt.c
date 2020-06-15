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
#include "libc/calls/internal.h"
#include "libc/nt/files.h"
#include "libc/nt/memory.h"
#include "libc/runtime/mappings.h"

textwindows int msync$nt(void *addr, size_t size, int flags) {
  size_t i, j;
  struct MemoryCoord c;
  if (!FlushViewOfFile(addr, size)) return winerr();
  j = findmapping(ADDR_TO_COORD(addr));
  c = ADDRSIZE_TO_COORD(addr, size);
  for (i = j; i; --i) {
    if (!ISOVERLAPPING(_mm.p[i - 1], c)) break;
    FlushFileBuffers(_mm.h[i - 1]);
  }
  return 0;
}
