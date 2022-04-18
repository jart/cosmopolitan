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
#include "libc/calls/strace.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/log/backtrace.internal.h"
#include "libc/log/libfatal.internal.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/processmemorycounters.h"
#include "libc/rand/rand.h"
#include "libc/runtime/directmap.internal.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/errfuns.h"

#define IP(X)      (intptr_t)(X)
#define VIP(X)     (void *)IP(X)
#define ALIGNED(p) (!(IP(p) & (FRAMESIZE - 1)))
#define ADDR(x)    ((int64_t)((uint64_t)(x) << 32) >> 16)
#define SHADE(x)   (((intptr_t)(x) >> 3) + 0x7fff8000)
#define FRAME(x)   ((int)((intptr_t)(x) >> 16))

static wontreturn void OnUnrecoverableMmapError(const char *s) {
  if (weaken(__die)) weaken(__die)();
  STRACE("%s %m", s);
  __restorewintty();
  _Exit(199);
}

noasan static bool IsMapped(char *p, size_t n) {
  return OverlapsImageSpace(p, n) || IsMemtracked(FRAME(p), FRAME(p + (n - 1)));
}

noasan static bool NeedAutomap(char *p, size_t n) {
  return !p || OverlapsArenaSpace(p, n) || OverlapsShadowSpace(p, n) ||
         IsMapped(p, n);
}

noasan static bool ChooseMemoryInterval(int x, int n, int *res) {
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
  if (ChooseMemoryInterval(FRAME(kAutomapStart), n, res)) {
    assert(*res >= FRAME(kAutomapStart));
    if (*res + n <= FRAME(kAutomapStart + (kAutomapStart - 1))) {
      return true;
    } else {
      STRACE("mmap(%.12p, %p) ENOMEM (automap interval exhausted)", ADDR(*res),
             ADDR(n + 1));
      return false;
    }
  } else {
    STRACE("mmap(%.12p, %p) ENOMEM (automap failed)", ADDR(*res), ADDR(n + 1));
    return false;
  }
}

noasan static size_t GetMemtrackSize(struct MemoryIntervals *mm) {
  size_t i, n;
  for (n = i = 0; i < mm->i; ++i) {
    n += ((size_t)(mm->p[i].y - mm->p[i].x) + 1) << 16;
  }
  return n;
}

static noasan void *MapMemory(void *addr, size_t size, int prot, int flags,
                              int fd, int64_t off, int f, int x, int n) {
  struct DirectMap dm;
  dm = sys_mmap(addr, size, prot, f, fd, off);
  if (UNLIKELY(dm.addr == MAP_FAILED)) {
    if (IsWindows() && (flags & MAP_FIXED)) {
      OnUnrecoverableMmapError(
          "can't recover from MAP_FIXED errors on Windows");
    }
    return MAP_FAILED;
  }
  if (UNLIKELY(dm.addr != addr)) {
    OnUnrecoverableMmapError("KERNEL DIDN'T RESPECT MAP_FIXED");
  }
  if (!IsWindows() && (flags & MAP_FIXED)) {
    if (UntrackMemoryIntervals(addr, size)) {
      OnUnrecoverableMmapError("FIXED UNTRACK FAILED");
    }
  }
  if (TrackMemoryInterval(&_mmi, x, x + (n - 1), dm.maphandle, prot, flags,
                          false, false, off, size)) {
    if (sys_munmap(addr, n) == -1) {
      OnUnrecoverableMmapError("TRACK MUNMAP FAILED");
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
                                                       int fd, int64_t off,
                                                       int f, int x, int n) {
  size_t i, m;
  int64_t oi, sz;
  struct DirectMap dm;
  bool iscow, readonlyfile;
  m = (size_t)(n - 1) << 16;
  assert(m < size);
  assert(m + FRAMESIZE >= size);
  oi = fd == -1 ? 0 : off + m;
  sz = size - m;
  dm = sys_mmap(addr + m, sz, prot, f, fd, oi);
  if (dm.addr == MAP_FAILED) return MAP_FAILED;
  iscow = (flags & MAP_PRIVATE) && fd != -1;
  readonlyfile = (flags & MAP_SHARED) && fd != -1 &&
                 (g_fds.p[fd].flags & O_ACCMODE) == O_RDONLY;
  if (TrackMemoryInterval(&_mmi, x + (n - 1), x + (n - 1), dm.maphandle, prot,
                          flags, readonlyfile, iscow, oi, sz) == -1) {
    OnUnrecoverableMmapError("MapMemories unrecoverable #1");
  }
  for (i = 0; i < m; i += FRAMESIZE) {
    oi = fd == -1 ? 0 : off + i;
    sz = FRAMESIZE;
    dm = sys_mmap(addr + i, sz, prot, f, fd, oi);
    if (dm.addr == MAP_FAILED ||
        TrackMemoryInterval(&_mmi, x + i / FRAMESIZE, x + i / FRAMESIZE,
                            dm.maphandle, prot, flags, readonlyfile, iscow, oi,
                            sz) == -1) {
      OnUnrecoverableMmapError("MapMemories unrecoverable #2");
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
#if defined(SYSDEBUG) && (_KERNTRACE || _NTTRACE)
  if (IsWindows()) {
    STRACE("mmap(%p, %'zu, %s, %s, %d, %'ld) → ...", addr, size,
           DescribeProtFlags(prot), DescribeMapFlags(flags), fd, off);
  }
#endif
  void *res;
  char *p = addr;
  struct DirectMap dm;
  size_t virtualused, virtualneed;
  int a, b, i, f, m, n, x;
  if (UNLIKELY(!size)) {
    STRACE("size=0");
    res = VIP(einval());
  } else if (UNLIKELY(!IsLegalSize(size))) {
    STRACE("size isn't 48-bit");
    res = VIP(einval());
  } else if (UNLIKELY(!IsLegalPointer(p))) {
    STRACE("p isn't 48-bit");
    res = VIP(einval());
  } else if (UNLIKELY(!ALIGNED(p))) {
    STRACE("p isn't 64kb aligned");
    res = VIP(einval());
  } else if (UNLIKELY(fd < -1)) {
    STRACE("mmap(%.12p, %'zu, fd=%d) EBADF", p, size, fd);
    res = VIP(ebadf());
  } else if (UNLIKELY(!((fd != -1) ^ !!(flags & MAP_ANONYMOUS)))) {
    STRACE("fd anonymous mismatch");
    res = VIP(einval());
  } else if (UNLIKELY(!(!!(flags & MAP_PRIVATE) ^ !!(flags & MAP_SHARED)))) {
    STRACE("MAP_SHARED ^ MAP_PRIVATE");
    res = VIP(einval());
  } else if (UNLIKELY(off < 0)) {
    STRACE("neg off");
    res = VIP(einval());
  } else if (UNLIKELY(INT64_MAX - size < off)) {
    STRACE("too large");
    res = VIP(einval());
  } else if (UNLIKELY(!ALIGNED(off))) {
    STRACE("p isn't 64kb aligned");
    res = VIP(einval());
  } else if ((flags & MAP_FIXED_NOREPLACE) && IsMapped(p, size)) {
#ifdef SYSDEBUG
    if (OverlapsImageSpace(p, size)) {
      STRACE("overlaps image");
    } else {
      STRACE("overlaps existing");
    }
#endif
    res = VIP(efault());
  } else if (__isfdkind(fd, kFdZip)) {
    STRACE("fd is zipos handle");
    res = VIP(einval());
  } else if (__virtualmax &&
             (__builtin_add_overflow((virtualused = GetMemtrackSize(&_mmi)),
                                     size, &virtualneed) ||
              virtualneed > __virtualmax)) {
    STRACE("%'zu size + %'zu inuse exceeds virtual memory limit %'zu", size,
           virtualused, __virtualmax);
    res = VIP(enomem());
  } else {
    if (fd == -1) {
      size = ROUNDUP(size, FRAMESIZE);
      if (IsWindows()) {
        prot |= PROT_WRITE; /* kludge */
      }
    }
    n = (int)(size >> 16) + !!(size & (FRAMESIZE - 1));
    assert(n > 0);
    f = (flags & ~MAP_FIXED_NOREPLACE) | MAP_FIXED;
    if (flags & MAP_FIXED) {
      x = FRAME(p);
      if (IsWindows()) {
        if (UntrackMemoryIntervals(p, size)) {
          OnUnrecoverableMmapError("FIXED UNTRACK FAILED");
        }
      }
    } else if (!NeedAutomap(p, size)) {
      x = FRAME(p);
    } else if (!Automap(n, &x)) {
      STRACE("AUTOMAP OUT OF MEMORY D:");
      return VIP(enomem());
    }
    p = (char *)ADDR(x);
    if (IsOpenbsd() && (f & MAP_GROWSDOWN)) { /* openbsd:dubstack */
      dm = sys_mmap(p, size, prot, f & ~MAP_GROWSDOWN, fd, off);
      if (dm.addr == MAP_FAILED) res = MAP_FAILED;
    }
    if (!IsWindows()) {
      res = MapMemory(p, size, prot, flags, fd, off, f, x, n);
    } else {
      res = MapMemories(p, size, prot, flags, fd, off, f, x, n);
    }
  }
  STRACE("mmap(%p, %'zu, %s, %s, %d, %'ld) → %p% m", addr, size,
         DescribeProtFlags(prot), DescribeMapFlags(flags), fd, off, res);
  return res;
}
