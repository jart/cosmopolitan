/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/assert.h"
#include "libc/bits/likely.h"
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/sysdebug.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/log/backtrace.internal.h"
#include "libc/log/libfatal.internal.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/rand/rand.h"
#include "libc/runtime/directmap.internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/errfuns.h"

#define IP(X)      (intptr_t)(X)
#define VIP(X)     (void *)IP(X)
#define SMALL(n)   ((n) <= 0xffffffffffff)
#define ALIGNED(p) (!(IP(p) & (FRAMESIZE - 1)))
#define ADDR(x)    ((int64_t)((uint64_t)(x) << 32) >> 16)
#define SHADE(x)   (((intptr_t)(x) >> 3) + 0x7fff8000)
#define FRAME(x)   ((int)((intptr_t)(x) >> 16))

forceinline wontreturn void Die(void) {
  if (weaken(__die)) weaken(__die)();
  abort();
}

noasan static bool IsMapped(char *p, size_t n) {
  return OverlapsImageSpace(p, n) || IsMemtracked(FRAME(p), FRAME(p + (n - 1)));
}

noasan static bool NeedAutomap(char *p, size_t n) {
  return !p || OverlapsArenaSpace(p, n) || OverlapsShadowSpace(p, n) ||
         IsMapped(p, n);
}

noasan static bool ChooseInterval(int x, int n, int *res) {
  int i;
  if (_mmi.i) {
    i = FindMemoryInterval(&_mmi, x);
    if (i < _mmi.i) {
      if (x + n < _mmi.p[i].x) {
        *res = x;
        return true;
      }
      while (++i < _mmi.i) {
        if (_mmi.p[i].x - _mmi.p[i - 1].y > n) {
          *res = _mmi.p[i - 1].y + 1;
          return true;
        }
      }
    }
    if (INT_MAX - _mmi.p[i - 1].y >= n) {
      *res = _mmi.p[i - 1].y + 1;
      return true;
    }
    return false;
  } else {
    *res = x;
    return true;
  }
}

noasan static bool Automap(int n, int *res) {
  *res = -1;
  if (ChooseInterval(FRAME(kAutomapStart), n, res)) {
    assert(*res >= FRAME(kAutomapStart));
    if (*res + n <= FRAME(kAutomapStart + (kAutomapStart - 1))) {
      return true;
    } else {
      SYSDEBUG("mmap(0x%p, 0x%x) ENOMEM (automap interval exhausted)",
               ADDR(*res), ADDR(n + 1));
      return false;
    }
  } else {
    SYSDEBUG("mmap(0x%p, 0x%x) ENOMEM (automap failed)", ADDR(*res),
             ADDR(n + 1));
    return false;
  }
}

static noasan void *MapMemory(void *addr, size_t size, int prot, int flags,
                              int fd, int64_t off, int f, int x, int n) {
  struct DirectMap dm;
  dm = sys_mmap(addr, size, prot, f, fd, off);
  if (UNLIKELY(dm.addr == MAP_FAILED)) {
    if (IsWindows() && (flags & MAP_FIXED)) {
      SYSDEBUG("mmap(0x%p, 0x%x) -> %s (%s)", addr, size, strerror(errno),
               "can't recover from MAP_FIXED errors on Windows");
      assert(!"MapMemory() failed");
      Die();
    }
    return MAP_FAILED;
  }
  if (UNLIKELY(dm.addr != addr)) {
    SYSDEBUG("KERNEL DIDN'T RESPECT MAP_FIXED");
    assert(!"MapMemory() failed");
    Die();
  }
  if (!IsWindows() && (flags & MAP_FIXED)) {
    if (UntrackMemoryIntervals(addr, size)) {
      SYSDEBUG("FIXED UNTRACK FAILED %s", strerror(errno));
      assert(!"MapMemory() failed");
      Die();
    }
  }
  if (TrackMemoryInterval(&_mmi, x, x + (n - 1), dm.maphandle, prot, flags)) {
    if (sys_munmap(addr, n) == -1) {
      SYSDEBUG("TRACK MUNMAP FAILED %s", strerror(errno));
      assert(!"MapMemory() failed");
      Die();
    }
    return MAP_FAILED;
  }
  if (weaken(__asan_map_shadow) && !OverlapsShadowSpace(addr, size)) {
    weaken(__asan_map_shadow)((intptr_t)addr, size);
  }
  return addr;
}

/**
 * Maps memory from system, one frame at a time.
 *
 * This is useful on Windows since it allows us to partially unmap or
 * punch holes into existing mappings.
 */
static textwindows dontinline noasan void *MapMemories(char *addr, size_t size,
                                                     int prot, int flags,
                                                     int fd, int64_t off, int f,
                                                     int x, size_t n) {
  struct DirectMap dm;
  size_t i, m = (n - 1) * FRAMESIZE;
  assert(m < size && m + FRAMESIZE >= size);
  dm = sys_mmap(addr + m, size - m, prot, f, fd, fd == -1 ? 0 : off + m);
  if (dm.addr == MAP_FAILED) {
    SYSDEBUG("MapMemories(%p+%x/%x) %s", addr, m, size, strerror(errno));
    return MAP_FAILED;
  }
  if (TrackMemoryInterval(&_mmi, x + (n - 1), x + (n - 1), dm.maphandle, prot,
                          flags) == -1) {
    SYSDEBUG("MapMemories(%p+%x/%x) unrecoverable failure #1 %s", addr, m, size,
             strerror(errno));
    assert(!"MapMemories() failed");
    Die();
  }
  for (i = 0; i < m; i += FRAMESIZE) {
    dm = sys_mmap(addr + i, FRAMESIZE, prot, f, fd, fd == -1 ? 0 : off + i);
    if (dm.addr == MAP_FAILED ||
        TrackMemoryInterval(&_mmi, x + i / FRAMESIZE, x + i / FRAMESIZE,
                            dm.maphandle, prot, flags) == -1) {
      SYSDEBUG("MapMemories(%p+%x/%x) unrecoverable failure #2 %s", addr, i,
               size, strerror(errno));
      assert(!"MapMemories() failed");
      Die();
    }
  }
  if (weaken(__asan_map_shadow) && !OverlapsShadowSpace(addr, size)) {
    weaken(__asan_map_shadow)((intptr_t)addr, size);
  }
  return addr;
}

/**
 * Beseeches system for page-table entries, e.g.
 *
 *     char *m;
 *     m = mmap(NULL, FRAMESIZE, PROT_READ | PROT_WRITE,
 *              MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
 *     munmap(m, FRAMESIZE);
 *
 * @param addr should be 0 to let your memory manager choose address;
 *     unless MAP_FIXED or MAP_FIXED_NOREPLACE are specified in flags
 *     in which case this function will do precicely as you ask, even
 *     if p=0 (in which you need -fno-delete-null-pointer-checks); it
 *     needs to be 64kb aligned because it's a wise choice that sadly
 *     needs to be made mandatory because of Windows although you can
 *     use __sys_mmap() to circumvent it on System Five in which case
 *     runtime support services, e.g. asan memory safety, could break
 * @param size must be >0 and needn't be a multiple of FRAMESIZE, but
 *     will be rounded up to FRAMESIZE automatically if MAP_ANONYMOUS
 *     is specified
 * @param prot can have PROT_READ/PROT_WRITE/PROT_EXEC/PROT_NONE/etc.
 * @param flags can have MAP_ANONYMOUS, MAP_SHARED, MAP_PRIVATE, etc.
 * @param fd is an open()'d file descriptor, whose contents shall be
 *     made available w/ automatic reading at the chosen address and
 *     must be -1 if MAP_ANONYMOUS is specified
 * @param off specifies absolute byte index of fd's file for mapping,
 *     should be zero if MAP_ANONYMOUS is specified, and sadly needs
 *     to be 64kb aligned too
 * @return virtual base address of new mapping, or MAP_FAILED w/ errno
 */
noasan void *mmap(void *addr, size_t size, int prot, int flags, int fd,
                  int64_t off) {
  struct DirectMap dm;
  int a, b, i, f, m, n, x;
  char mode[8], *p = addr;
  if (UNLIKELY(!size)) {
    SYSDEBUG("mmap(0x%p, 0x%x) EINVAL (size=0)", p, size);
    return VIP(einval());
  }
  if (UNLIKELY(!SMALL(size))) {
    SYSDEBUG("mmap(0x%p, 0x%x) EINVAL (size isn't 48-bit)", p, size);
    return VIP(einval());
  }
  if (UNLIKELY(!IsLegalPointer(p))) {
    SYSDEBUG("mmap(0x%p, 0x%x) EINVAL (p isn't 48-bit)", p, size);
    return VIP(einval());
  }
  if (UNLIKELY(!ALIGNED(p))) {
    SYSDEBUG("mmap(0x%p, 0x%x) EINVAL (p isn't 64kb aligned)", p, size);
    return VIP(einval());
  }
  if (UNLIKELY(fd < -1)) {
    SYSDEBUG("mmap(0x%p, 0x%x, fd=%d) EBADF", p, size, (long)fd);
    return VIP(ebadf());
  }
  if (UNLIKELY(!((fd != -1) ^ !!(flags & MAP_ANONYMOUS)))) {
    SYSDEBUG("mmap(0x%p, 0x%x, %s, %d, %d) EINVAL (fd anonymous mismatch)", p,
             size, DescribeMapping(prot, flags, mode), (long)fd, off);
    return VIP(einval());
  }
  if (UNLIKELY(!(!!(flags & MAP_PRIVATE) ^ !!(flags & MAP_SHARED)))) {
    SYSDEBUG("mmap(0x%p, 0x%x) EINVAL (MAP_SHARED ^ MAP_PRIVATE)", p, size);
    return VIP(einval());
  }
  if (UNLIKELY(off < 0)) {
    SYSDEBUG("mmap(0x%p, 0x%x, off=%d) EINVAL (neg off)", p, size, off);
    return VIP(einval());
  }
  if (UNLIKELY(INT64_MAX - size < off)) {
    SYSDEBUG("mmap(0x%p, 0x%x, off=%d) EINVAL (too large)", p, size, off);
    return VIP(einval());
  }
  if (UNLIKELY(!ALIGNED(off))) {
    SYSDEBUG("mmap(0x%p, 0x%x) EINVAL (p isn't 64kb aligned)", p, size);
    return VIP(einval());
  }
  if ((flags & MAP_FIXED_NOREPLACE) && IsMapped(p, size)) {
    if (OverlapsImageSpace(p, size)) {
      SYSDEBUG("mmap(0x%p, 0x%x) EFAULT (overlaps image)", p, size);
    } else {
      SYSDEBUG("mmap(0x%p, 0x%x) EFAULT (overlaps existing)", p, size);
    }
    return VIP(efault());
  }
  SYSDEBUG("mmap(0x%p, 0x%x, %s, %d, %d)", p, size,
           DescribeMapping(prot, flags, mode), (long)fd, off);
  if (fd == -1) {
    size = ROUNDUP(size, FRAMESIZE);
    if (IsWindows()) {
      prot |= PROT_WRITE; /* kludge */
    }
  }
  n = FRAME(size) + !!(size & (FRAMESIZE - 1));
  f = (flags & ~MAP_FIXED_NOREPLACE) | MAP_FIXED;
  if (flags & MAP_FIXED) {
    x = FRAME(p);
    if (IsWindows()) {
      if (UntrackMemoryIntervals(p, size)) {
        SYSDEBUG("FIXED UNTRACK FAILED %s", strerror(errno));
        assert(!"mmap() failed");
        Die();
      }
    }
  } else if (!NeedAutomap(p, size)) {
    x = FRAME(p);
  } else if (!Automap(n, &x)) {
    return VIP(enomem());
  }
  p = (char *)ADDR(x);
  if (IsOpenbsd() && (f & MAP_GROWSDOWN)) { /* openbsd:dubstack */
    dm = sys_mmap(p, size, prot, f & ~MAP_GROWSDOWN, fd, off);
    if (dm.addr == MAP_FAILED) return MAP_FAILED;
  }
  if (!IsWindows()) {
    return MapMemory(p, size, prot, flags, fd, off, f, x, n);
  } else {
    return MapMemories(p, size, prot, flags, fd, off, f, x, n);
  }
}
