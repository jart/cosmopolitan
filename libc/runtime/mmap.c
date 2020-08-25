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
#include "libc/rand/rand.h"
#include "libc/runtime/memtrack.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/errfuns.h"

#define IP(X)            (intptr_t)(X)
#define VIP(X)           (void *)IP(X)
#define COORD(a)         (int)(IP(a) >> 16)
#define ADDR(c)          (void *)(IP(c) << 16)
#define ALIGNED(p)       (!(IP(p) & (FRAMESIZE - 1)))
#define CANONICAL(p)     (-0x800000000000 <= IP(p) && IP(p) <= 0x7fffffffffff)
#define LAST_COORD(a, n) (COORD(a) + (ROUNDUP(n, FRAMESIZE) >> 16) - 1)

struct DirectMap {
  void *addr;
  int64_t maphandle;
};

struct MemoryIntervals _mmi;

static textwindows struct DirectMap DirectMapNt(void *addr, size_t size,
                                                unsigned prot, unsigned flags,
                                                int fd, int64_t off) {
  struct DirectMap res; /* NT IS TORTURE */
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

static struct DirectMap DirectMap(void *addr, size_t size, unsigned prot,
                                  unsigned flags, int fd, int64_t off) {
  if (!IsWindows()) {
    return (struct DirectMap){mmap$sysv(addr, size, prot, flags, fd, off),
                              kNtInvalidHandleValue};
  } else {
    return DirectMapNt(addr, size, prot, flags, fd, off);
  }
}

static int UntrackMemoryIntervals(void *addr, size_t size) {
  return ReleaseMemoryIntervals(&_mmi, COORD(addr), LAST_COORD(addr, size),
                                ReleaseMemoryNt);
}

/**
 * Releases memory pages.
 *
 * @param addr is a pointer within any memory mapped region the process
 *     has permission to control, such as address ranges returned by
 *     mmap(), the program image itself, etc.
 * @param size is the amount of memory to unmap, which needn't be a
 *     multiple of FRAMESIZE, and may be a subset of that which was
 *     mapped previously, and may punch holes in existing mappings,
 *     but your mileage may vary on windows
 * @return 0 on success, or -1 w/ errno
 */
int munmap(void *addr, size_t size) {
  int rc;
  if (!ALIGNED(addr) || !CANONICAL(addr) || !size) return einval();
  size = ROUNDUP(size, FRAMESIZE);
  if (UntrackMemoryIntervals(addr, size) == -1) return -1;
  if (IsWindows()) return 0;
  return munmap$sysv(addr, size);
}

/**
 * Beseeches system for page-table entries.
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
  int i;
  long gap;
  struct DirectMap dm;
  if (!size) return VIP(einval());
  if (!ALIGNED(off)) return VIP(einval());
  if (!ALIGNED(addr)) return VIP(einval());
  if (!CANONICAL(addr)) return VIP(einval());
  if (!(!!(flags & MAP_ANONYMOUS) ^ (fd != -1))) return VIP(einval());
  if (!(!!(flags & MAP_PRIVATE) ^ !!(flags & MAP_SHARED))) return VIP(einval());
  if (!(IsWindows() && fd != -1)) size = ROUNDUP(size, FRAMESIZE);
  if (flags & MAP_FIXED) {
    if (UntrackMemoryIntervals(addr, size) == -1) {
      return MAP_FAILED;
    }
  } else if (_mmi.i) {
    if (0 && IsModeDbg()) {
      addr = VIP(rand64() & 0x00007ffffffff000);
    } else {
      for (i = _mmi.i - 1; i > 0; --i) {
        gap = _mmi.p[i].x - _mmi.p[i - 1].y - 1;
        assert(gap > 0);
        if (gap >= (ROUNDUP(size, FRAMESIZE) >> 16)) {
          addr = ADDR(_mmi.p[i - 1].y + 1);
          break;
        }
      }
      if (!addr) {
        addr = ADDR(_mmi.p[_mmi.i - 1].y + 1);
      }
    }
  } else {
    addr = VIP(kMappingsStart);
  }
  assert((flags & MAP_FIXED) ||
         (!isheap(addr) && !isheap((char *)addr + size - 1)));
  dm = DirectMap(addr, size, prot, flags | MAP_FIXED, fd, off);
  if (dm.addr == MAP_FAILED || dm.addr != addr) {
    return MAP_FAILED;
  }
  if (TrackMemoryInterval(&_mmi, COORD(dm.addr), LAST_COORD(dm.addr, size),
                          dm.maphandle) == -1) {
    _Exit(1);
  }
  return dm.addr;
}
