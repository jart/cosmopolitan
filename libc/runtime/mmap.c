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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/asancodes.h"
#include "libc/intrin/bits.h"
#include "libc/intrin/bsr.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/directmap.internal.h"
#include "libc/intrin/likely.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/limits.h"
#include "libc/log/backtrace.internal.h"
#include "libc/log/libfatal.internal.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/processmemorycounters.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/thread.h"
#include "libc/zipos/zipos.internal.h"

#define MAP_ANONYMOUS_linux   0x00000020
#define MAP_ANONYMOUS_openbsd 0x00001000
#define MAP_GROWSDOWN_linux   0x00000100
#define MAP_STACK_freebsd     0x00000400
#define MAP_STACK_openbsd     0x00004000

#define IP(X)      (intptr_t)(X)
#define VIP(X)     (void *)IP(X)
#define ALIGNED(p) (!(IP(p) & (FRAMESIZE - 1)))
#define ADDR(x)    ((int64_t)((uint64_t)(x) << 32) >> 16)
#define SHADE(x)   (((intptr_t)(x) >> 3) + 0x7fff8000)
#define FRAME(x)   ((int)((intptr_t)(x) >> 16))

static unsigned long RoundDownTwoPow(unsigned long x) {
  return x ? 1ul << _bsrl(x) : 0;
}

static wontreturn void OnUnrecoverableMmapError(const char *s) {
  if (_weaken(__die)) _weaken(__die)();
  STRACE("%s %m", s);
  _Exitr(199);
}

static noasan inline bool OverlapsExistingMapping(char *p, size_t n) {
  int a, b, i;
  _unassert(n > 0);
  a = FRAME(p);
  b = FRAME(p + (n - 1));
  i = FindMemoryInterval(&_mmi, a);
  if (i < _mmi.i) {
    if (a <= _mmi.p[i].x && _mmi.p[i].x <= b) return true;
    if (a <= _mmi.p[i].y && _mmi.p[i].y <= b) return true;
    if (_mmi.p[i].x <= a && b <= _mmi.p[i].y) return true;
  }
  return false;
}

static noasan bool ChooseMemoryInterval(int x, int n, int align, int *res) {
  // TODO: improve performance
  int i, start, end;
  _unassert(align > 0);
  if (_mmi.i) {

    // find the start of the automap memory region
    i = FindMemoryInterval(&_mmi, x);
    if (i < _mmi.i) {

      // check to see if there's space available before the first entry
      if (!__builtin_add_overflow(x, align - 1, &start)) {
        start &= -align;
        if (!__builtin_add_overflow(start, n - 1, &end)) {
          if (end < _mmi.p[i].x) {
            *res = start;
            return true;
          }
        }
      }

      // check to see if there's space available between two entries
      while (++i < _mmi.i) {
        if (!__builtin_add_overflow(_mmi.p[i - 1].y, 1, &start) &&
            !__builtin_add_overflow(start, align - 1, &start)) {
          start &= -align;
          if (!__builtin_add_overflow(start, n - 1, &end)) {
            if (end < _mmi.p[i].x) {
              *res = start;
              return true;
            }
          }
        }
      }
    }

    // otherwise append after the last entry if space is available
    if (!__builtin_add_overflow(_mmi.p[i - 1].y, 1, &start) &&
        !__builtin_add_overflow(start, align - 1, &start)) {
      start &= -align;
      if (!__builtin_add_overflow(start, n - 1, &end)) {
        *res = start;
        return true;
      }
    }

  } else {
    // if memtrack is empty, then just assign the requested address
    // assuming it doesn't overflow
    if (!__builtin_add_overflow(x, align - 1, &start)) {
      start &= -align;
      if (!__builtin_add_overflow(start, n - 1, &end)) {
        *res = start;
        return true;
      }
    }
  }

  return false;
}

noasan static bool Automap(int count, int align, int *res) {
  return ChooseMemoryInterval(FRAME(kAutomapStart), count, align, res) &&
         *res + count <= FRAME(kAutomapStart + (kAutomapSize - 1));
}

static noasan void *FinishMemory(void *addr, size_t size, int prot, int flags,
                                 int fd, int64_t off, int f, int x, int n,
                                 struct DirectMap dm) {
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
  if (_weaken(__asan_map_shadow) && !OverlapsShadowSpace(addr, size)) {
    _weaken(__asan_map_shadow)((intptr_t)addr, size);
  }
  return addr;
}

static noasan void *MapMemory(void *addr, size_t size, int prot, int flags,
                              int fd, int64_t off, int f, int x, int n) {
  struct DirectMap dm;
  dm = sys_mmap(addr, size, prot, f, fd, off);
  if (VERY_UNLIKELY(dm.addr == MAP_FAILED)) {
    if (IsWindows() && (flags & MAP_FIXED)) {
      OnUnrecoverableMmapError(
          "can't recover from MAP_FIXED errors on Windows");
    }
    return MAP_FAILED;
  }
  if (VERY_UNLIKELY(dm.addr != addr)) {
    OnUnrecoverableMmapError("KERNEL DIDN'T RESPECT MAP_FIXED");
  }
  return FinishMemory(addr, size, prot, flags, fd, off, f, x, n, dm);
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
  _unassert(m < size);
  _unassert(m + FRAMESIZE >= size);
  oi = fd == -1 ? 0 : off + m;
  sz = size - m;
  dm = sys_mmap(addr + m, sz, prot, f, fd, oi);
  if (dm.addr == MAP_FAILED) return MAP_FAILED;
  iscow = (flags & MAP_TYPE) != MAP_SHARED && fd != -1;
  readonlyfile = (flags & MAP_TYPE) == MAP_SHARED && fd != -1 &&
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
  if (_weaken(__asan_map_shadow) && !OverlapsShadowSpace(addr, size)) {
    _weaken(__asan_map_shadow)((intptr_t)addr, size);
  }
  return addr;
}

noasan inline void *_Mmap(void *addr, size_t size, int prot, int flags, int fd,
                          int64_t off) {
  char *p = addr;
  struct DirectMap dm;
  int a, b, i, f, m, n, x;
  bool needguard, clashes;
  size_t virtualused, virtualneed;

  if (VERY_UNLIKELY(!size)) {
    STRACE("size=0");
    return VIP(einval());
  }

  if (VERY_UNLIKELY(!IsLegalPointer(p))) {
    STRACE("p isn't 48-bit");
    return VIP(einval());
  }

  if (VERY_UNLIKELY(!ALIGNED(p))) {
    STRACE("p isn't 64kb aligned");
    return VIP(einval());
  }

  if (VERY_UNLIKELY(fd < -1)) {
    STRACE("mmap(%.12p, %'zu, fd=%d) EBADF", p, size, fd);
    return VIP(ebadf());
  }

  if (VERY_UNLIKELY(!((fd != -1) ^ !!(flags & MAP_ANONYMOUS)))) {
    STRACE("fd anonymous mismatch");
    return VIP(einval());
  }

  if (VERY_UNLIKELY(!(!!(flags & MAP_PRIVATE) ^ !!(flags & MAP_SHARED)))) {
    STRACE("MAP_SHARED ^ MAP_PRIVATE");
    return VIP(einval());
  }

  if (VERY_UNLIKELY(off < 0)) {
    STRACE("neg off");
    return VIP(einval());
  }

  if (VERY_UNLIKELY(!ALIGNED(off))) {
    STRACE("p isn't 64kb aligned");
    return VIP(einval());
  }

  if (fd == -1) {
    size = ROUNDUP(size, FRAMESIZE);
    if (IsWindows()) {
      prot |= PROT_WRITE; /* kludge */
    }
  } else if (__isfdkind(fd, kFdZip)) {
    STRACE("fd is zipos handle");
    return VIP(einval());
  }

  if (VERY_UNLIKELY(!IsLegalSize(size))) {
    STRACE("size isn't 48-bit");
    return VIP(einval());
  }

  if (VERY_UNLIKELY(INT64_MAX - size < off)) {
    STRACE("too large");
    return VIP(einval());
  }

  if (__virtualmax < LONG_MAX &&
      (__builtin_add_overflow((virtualused = GetMemtrackSize(&_mmi)), size,
                              &virtualneed) ||
       virtualneed > __virtualmax)) {
    STRACE("%'zu size + %'zu inuse exceeds virtual memory limit %'zu", size,
           virtualused, __virtualmax);
    return VIP(enomem());
  }

  clashes = OverlapsImageSpace(p, size) || OverlapsExistingMapping(p, size);

  if ((flags & MAP_FIXED_NOREPLACE) == MAP_FIXED_NOREPLACE && clashes) {
    STRACE("noreplace overlaps existing");
    return VIP(eexist());
  }

  if (__builtin_add_overflow((int)(size >> 16), (int)!!(size & (FRAMESIZE - 1)),
                             &n)) {
    STRACE("memory range overflows");
    return VIP(einval());
  }

  a = max(1, RoundDownTwoPow(size) >> 16);

  f = (flags & ~MAP_FIXED_NOREPLACE) | MAP_FIXED;
  if (flags & MAP_FIXED) {
    x = FRAME(p);
    if (IsWindows()) {
      if (UntrackMemoryIntervals(p, size)) {
        OnUnrecoverableMmapError("FIXED UNTRACK FAILED");
      }
    }
  } else if (p && !clashes && !OverlapsArenaSpace(p, size) &&
             !OverlapsShadowSpace(p, size)) {
    x = FRAME(p);
  } else if (!Automap(n, a, &x)) {
    STRACE("automap has no room for %d frames with %d alignment", n, a);
    return VIP(enomem());
  }

  needguard = false;
  p = (char *)ADDR(x);
  if ((f & MAP_TYPE) == MAP_STACK) {
    if (~f & MAP_ANONYMOUS) {
      STRACE("MAP_STACK must be anonymous");
      return VIP(einval());
    }
    f &= ~MAP_TYPE;
    f |= MAP_PRIVATE;
    if (IsOpenbsd()) {  // openbsd:dubstack
      // on openbsd this is less about scalability of threads, and more
      // about defining the legal intervals for the RSP register. sadly
      // openbsd doesn't let us create a new fixed stack mapping. but..
      // openbsd does allow us to overwrite existing fixed mappings, to
      // authorize its usage as a stack.
      if (sys_mmap(p, size, prot, f, fd, off).addr == MAP_FAILED) {
        return MAP_FAILED;
      }
      f |= MAP_STACK_openbsd;
      needguard = true;
    } else if (IsLinux()) {
      // make sure there's no existing stuff existing between our stack
      // starting page and the bottom guard page, since that would stop
      // our stack page from growing down.
      _npassert(!sys_munmap(p, size));
      // by default MAP_GROWSDOWN will auto-allocate 10mb of pages. it's
      // supposed to stop growing if an adjacent allocation exists, to
      // prevent your stacks from overlapping on each other. we're not
      // able to easily assume a mapping beneath this one exists. even
      // if we could, the linux kernel requires for muh security reasons
      // that stacks be at least 1mb away from each other, so it's not
      // possible to avoid this call if our goal is to have 60kb stacks
      // with 4kb guards like a sane multithreaded production system.
      // however this 1mb behavior oddly enough is smart enough to not
      // apply if the mapping is a manually-created guard page.
      int e = errno;
      if ((dm = sys_mmap(p + size - GUARDSIZE, GUARDSIZE, prot,
                         f | MAP_GROWSDOWN_linux, fd, off))
              .addr != MAP_FAILED) {
        _npassert(sys_mmap(p, GUARDSIZE, PROT_NONE,
                           MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)
                      .addr == p);
        dm.addr = p;
        return FinishMemory(p, size, prot, flags, fd, off, f, x, n, dm);
      } else if (errno == ENOTSUP) {
        // WSL doesn't support MAP_GROWSDOWN
        needguard = true;
        errno = e;
      }
    } else {
      if (IsFreebsd()) {
        f |= MAP_STACK_freebsd;
      }
      needguard = true;
    }
  }

  if (!IsWindows()) {
    p = MapMemory(p, size, prot, flags, fd, off, f, x, n);
  } else {
    p = MapMemories(p, size, prot, flags, fd, off, f, x, n);
  }

  if (p != MAP_FAILED) {
    if (needguard) {
      if (!IsWindows()) {
        // make windows fork() code simpler
        mprotect(p, GUARDSIZE, PROT_NONE);
      }
      if (IsAsan()) {
        __repstosb((void *)(((intptr_t)p >> 3) + 0x7fff8000),
                   kAsanStackOverflow, GUARDSIZE / 8);
      }
    }
  }

  return p;
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
 * @param flags should have one of the following masked by `MAP_TYPE`
 *     - `MAP_FILE` in which case `fd != -1` should be the case
 *     - `MAP_PRIVATE` for copy-on-write behavior of writeable pages
 *     - `MAP_SHARED` to create shared memory between processes
 *     - `MAP_STACK` to create a grows-down alloc, where a guard page
 *       is automatically protected at the bottom: FreeBSD's behavior
 *       is polyfilled across platforms; uses MAP_GROWSDOWN on Linux
 *       too for extra oomph (do not use MAP_GROWSDOWN!) and this is
 *       completely mandatory on OpenBSD but helps perf elsewhere if
 *       you need to create 10,000 threads.  This flag is the reason
 *       why `STACK_FRAME_UNLIMITED` toil is important, because this
 *       only allocates a 4096-byte guard page, thus we need the GCC
 *       compile-time checks to ensure some char[8192] vars will not
 *       create an undetectable overflow into another thread's stack
 *     Your `flags` may optionally bitwise or any of the following:
 *     - `MAP_ANONYMOUS` in which case `fd == -1` should be the case
 *     - `MAP_FIXED` in which case `addr` becomes more than a hint
 *     - `MAP_FIXED_NOREPLACE` to protect existing mappings; this is
 *       always polyfilled by mmap() which tracks its own memory and
 *       removed before passing to the kernel, in order to support
 *       old versions; if you believe mappings exist which only the
 *       kernel knows, then this flag may be passed to sys_mmap() on
 *       Linux 4.17+ and FreeBSD (where it has multiple bits)
 *     - `MAP_CONCEAL` is FreeBSD/NetBSD/OpenBSD-only
 *     - `MAP_NORESERVE` is Linux/XNU/NetBSD-only
 *     - `MAP_POPULATE` is Linux/FreeBSD-only
 *     - `MAP_NONBLOCK` is Linux-only
 *     - `MAP_NOSYNC` is FreeBSD-only
 *     - `MAP_INHERIT` is NetBSD-only
 *     - `MAP_LOCKED` is Linux-only
 * @param fd is an open()'d file descriptor, whose contents shall be
 *     made available w/ automatic reading at the chosen address and
 *     must be -1 if MAP_ANONYMOUS is specified
 * @param off specifies absolute byte index of fd's file for mapping,
 *     should be zero if MAP_ANONYMOUS is specified, and sadly needs
 *     to be 64kb aligned too
 * @return virtual base address of new mapping, or MAP_FAILED w/ errno
 */
void *mmap(void *addr, size_t size, int prot, int flags, int fd, int64_t off) {
  void *res;
  size_t toto = 0;
#if defined(SYSDEBUG) && (_KERNTRACE || _NTTRACE)
  if (IsWindows()) {
    STRACE("mmap(%p, %'zu, %s, %s, %d, %'ld) → ...", addr, size,
           DescribeProtFlags(prot), DescribeMapFlags(flags), fd, off);
  }
#endif
  __mmi_lock();
  if (!__isfdkind(fd, kFdZip)) {
    res = _Mmap(addr, size, prot, flags, fd, off);
  } else {
    res = _weaken(__zipos_Mmap)(
        addr, size, prot, flags,
        (struct ZiposHandle *)(intptr_t)g_fds.p[fd].handle, off);
  }
#if SYSDEBUG
  toto = __strace > 0 ? GetMemtrackSize(&_mmi) : 0;
#endif
  __mmi_unlock();
  STRACE("mmap(%p, %'zu, %s, %s, %d, %'ld) → %p% m (%'zu bytes total)", addr,
         size, DescribeProtFlags(prot), DescribeMapFlags(flags), fd, off, res,
         toto);
  return res;
}
