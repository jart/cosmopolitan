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
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/log/asan.h"
#include "libc/macros.h"
#include "libc/rand/rand.h"
#include "libc/runtime/directmap.h"
#include "libc/runtime/memtrack.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/errfuns.h"

#define IP(X)        (intptr_t)(X)
#define VIP(X)       (void *)IP(X)
#define ADDR(c)      (void *)(IP(c) << 16)
#define ALIGNED(p)   (!(IP(p) & (FRAMESIZE - 1)))
#define CANONICAL(p) (-0x800000000000 <= IP(p) && IP(p) <= 0x7fffffffffff)

/**
 * Beseeches system for page-table entries.
 *
 *   char *p = mmap(NULL, 65536, PROT_READ | PROT_WRITE,
 *                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
 *   munmap(p, 65536);
 *
 * @param addr optionally requests a particular virtual base address,
 *     which needs to be 64kb aligned if passed (for NT compatibility)
 * @param size must be >0 and needn't be a multiple of FRAMESIZE
 * @param prot can have PROT_READ, PROT_WRITE, PROT_EXEC, PROT_NONE, etc.
 * @param flags can have MAP_ANONYMOUS, MAP_SHARED, MAP_PRIVATE, etc.
 * @param fd is an open()'d file descriptor whose contents shall be
 *     mapped, and is ignored if MAP_ANONYMOUS is specified
 * @param offset specifies absolute byte index of fd's file for mapping,
 *     and should be zero if MAP_ANONYMOUS is specified
 * @return virtual base address of new mapping, or MAP_FAILED w/ errno
 */
void *mmap(void *addr, size_t size, int prot, int flags, int fd, int64_t off) {
  int i, x, n, a, b;
  struct DirectMap dm;
  if (!size) return VIP(einval());
  if (!ALIGNED(off)) return VIP(einval());
  if (!ALIGNED(addr)) return VIP(einval());
  if (!CANONICAL(addr)) return VIP(einval());
  if (!(!!(flags & MAP_ANONYMOUS) ^ (fd != -1))) return VIP(einval());
  if (!(!!(flags & MAP_PRIVATE) ^ !!(flags & MAP_SHARED))) return VIP(einval());
  if (fd == -1) size = ROUNDUP(size, FRAMESIZE);
  if (flags & MAP_FIXED) {
    if (UntrackMemoryIntervals(addr, size) == -1) {
      return MAP_FAILED;
    }
  } else {
    x = kAutomapStart >> 16;
    n = ROUNDUP(size, FRAMESIZE) >> 16;
    for (i = 0; i < _mmi.i; ++i) {
      if (_mmi.p[i].y < x) continue;
      if (_mmi.p[i].x > x + n - 1) break;
      x = _mmi.p[i].y + 1;
    }
    if (x + n - 1 >= ((kAutomapStart + kAutomapSize) >> 16)) {
      return (void *)(intptr_t)enomem();
    }
    addr = (void *)(intptr_t)((int64_t)x << 16);
  }
  dm = DirectMap(addr, size, prot, flags | MAP_FIXED, fd, off);
  if (dm.addr == MAP_FAILED || dm.addr != addr) {
    return MAP_FAILED;
  }
  a = ROUNDDOWN((intptr_t)addr, FRAMESIZE) >> 16;
  b = ROUNDDOWN((intptr_t)addr + size - 1, FRAMESIZE) >> 16;
  if (TrackMemoryInterval(&_mmi, a, b, dm.maphandle, prot, flags) == -1) {
    abort();
  }
  if (weaken(__asan_map_shadow)) {
    weaken(__asan_map_shadow)(dm.addr, size);
  }
  return dm.addr;
}
