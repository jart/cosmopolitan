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
#include "libc/macros.h"
#include "libc/nt/memory.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/mappings.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/errfuns.h"

#define VIP(X) (void *)(intptr_t)(X)

struct DirectMap {
  void *addr;
  int64_t maphandle;
};

static textwindows struct DirectMap directmap$nt(void *addr, size_t size,
                                                 unsigned prot, unsigned flags,
                                                 int fd, int64_t off) {
  struct DirectMap res;
  if ((res.maphandle = CreateFileMappingNuma(
           fd != -1 ? g_fds.p[fd].handle : kNtInvalidHandleValue,
           &kNtIsInheritable, prot2nt(prot, flags), size >> 32, size, NULL,
           kNtNumaNoPreferredNode))) {
    if (!(res.addr = MapViewOfFileExNuma(res.maphandle, fprot2nt(prot, flags),
                                         off >> 32, off, size, addr,
                                         kNtNumaNoPreferredNode))) {
      CloseHandle(res.maphandle);
      res.maphandle = kNtInvalidHandleValue;
      res.addr = VIP(winerr());
    }
  } else {
    res.maphandle = kNtInvalidHandleValue;
    res.addr = VIP(winerr());
  }
  return res;
}

static struct DirectMap directmap(void *addr, size_t size, unsigned prot,
                                  unsigned flags, int fd, int64_t off) {
  if (!IsWindows()) {
    return (struct DirectMap){mmap$sysv(addr, size, prot, flags, fd, off),
                              kNtInvalidHandleValue};
  } else {
    return directmap$nt(addr, size, prot, flags, fd, off);
  }
}

/**
 * Beseeches system for page-table entries.
 *
 * @param addr optionally requests a particular virtual base address,
 *     which needs to be 64kb aligned if passed (for NT compatibility)
 * @param size should be >0 and multiple of PAGESIZE
 * @param prot can have PROT_READ, PROT_WRITE, PROT_EXEC, PROT_NONE, etc.
 * @param flags can have MAP_ANONYMOUS, MAP_SHARED, MAP_PRIVATE, etc.
 * @param fd is an open()'d file descriptor whose contents shall be
 *     mapped, and is ignored if MAP_ANONYMOUS is specified
 * @param offset specifies absolute byte index of fd's file for mapping,
 *     and should be zero if MAP_ANONYMOUS is specified
 * @return virtual base address of new mapping, or MAP_FAILED w/ errno
 */
void *mmap(void *addr, size_t size, int prot, int flags, int fd, int64_t off) {
  size_t i;
  intptr_t p;
  struct DirectMap dm;
  struct MemoryCoord c;
  p = (intptr_t)addr;

  assert(!(0 < p && p < 0x200000));
  assert(-0x800000000000L <= p && p <= 0x7fffffffffffL);
  assert((flags & MAP_PRIVATE) ^ (flags & MAP_SHARED));
  assert((flags & MAP_ANONYMOUS) ^ (fd != -1));
  assert(off % PAGESIZE == 0);
  assert(size > 0);

  if (!(IsWindows() && fd != -1)) {
    size = ROUNDUP(size, FRAMESIZE);
  }

  if (flags & MAP_FIXED) {
    assert(addr != NULL);
    assert((intptr_t)addr % FRAMESIZE == 0);
  } else {
    if (!addr) {
      if (_mm.i) {
        addr = COORD_TO_ADDR(_mm.p[_mm.i - 1].y + 1);
        for (i = _mm.i; i; --i) {
          if (_mm.p[i - 1].y + 1 + size / FRAMESIZE <=
                  ADDR_TO_COORD(kMappingsStart + kMappingsSize) &&
              _mm.p[i - 1].y + 1 >= ADDR_TO_COORD(kMappingsStart)) {
            addr = COORD_TO_ADDR(_mm.p[i - 1].y + 1);
            break;
          }
        }
      } else {
        addr = VIP(kMappingsStart);
      }
    }
    addr = (void *)ROUNDDOWN((intptr_t)addr, FRAMESIZE);
  }

  if (_mm.i == MMAP_MAX) {
    return VIP(enomem());
  }

  if (flags & MAP_FIXED) {
    munmap(addr, size);
  } else {
    c = ADDRSIZE_TO_COORD(addr, size);
    if ((i = findmapping(c.y)) && ISOVERLAPPING(c, _mm.p[i - 1])) {
      return VIP(einval());
    }
  }

  dm = directmap(addr, size, prot, flags | MAP_FIXED, fd, off);
  if (dm.addr == MAP_FAILED) return MAP_FAILED;

  i = findmapping(ADDR_TO_COORD(dm.addr));
  if (i < _mm.i) {
    memmove(&_mm.p[i + 1], &_mm.p[i],
            (intptr_t)&_mm.p[_mm.i] - (intptr_t)&_mm.p[i]);
    memmove(&_mm.h[i + 1], &_mm.h[i],
            (intptr_t)&_mm.h[_mm.i] - (intptr_t)&_mm.h[i]);
  }

  _mm.p[i] = ADDRSIZE_TO_COORD(dm.addr, size);
  _mm.h[i] = dm.maphandle;
  _mm.i++;

  assert((intptr_t)dm.addr % __BIGGEST_ALIGNMENT__ == 0);
  return dm.addr;
}
