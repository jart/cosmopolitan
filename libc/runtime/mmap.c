/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/struct/sigset.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/asancodes.h"
#include "libc/intrin/bsr.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/directmap.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/likely.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/limits.h"
#include "libc/log/backtrace.internal.h"
#include "libc/log/libfatal.internal.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/nt/enum/memflags.h"
#include "libc/nt/enum/pageflags.h"
#include "libc/nt/memory.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/processmemorycounters.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/stdckdint.h"
#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/ss.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/thread.h"

#define MAP_ANONYMOUS_linux   0x00000020
#define MAP_ANONYMOUS_openbsd 0x00001000
#define MAP_GROWSDOWN_linux   0x00000100
#define MAP_STACK_freebsd     0x00000400
#define MAP_STACK_openbsd     0x00004000

#define IP(X)      (intptr_t)(X)
#define VIP(X)     (void *)IP(X)
#define ALIGNED(p) (!(IP(p) & (FRAMESIZE - 1)))
#define SHADE(x)   (((intptr_t)(x) >> 3) + 0x7fff8000)
#define FRAME(x)   ((int)((intptr_t)(x) >> 16))

static inline pureconst unsigned long __rounddown2pow(unsigned long x) {
  return x ? 1ul << _bsrl(x) : 0;
}

static wontreturn void __mmap_die(const char *s) {
  if (_weaken(__die)) _weaken(__die)();
  STRACE("%s %m", s);
  _Exit(199);
}

static inline bool __overlaps_existing_mapping(char *p, size_t n) {
  int a, b, i;
  unassert(n > 0);
  a = FRAME(p);
  b = FRAME(p + (n - 1));
  i = __find_memory(&_mmi, a);
  if (i < _mmi.i) {
    if (a <= _mmi.p[i].x && _mmi.p[i].x <= b) return true;
    if (a <= _mmi.p[i].y && _mmi.p[i].y <= b) return true;
    if (_mmi.p[i].x <= a && b <= _mmi.p[i].y) return true;
  }
  return false;
}

static bool __choose_memory(int x, int n, int align, int *res) {
  // TODO: improve performance
  int i, start, end;
  unassert(align > 0);
  if (_mmi.i) {

    // find the start of the automap memory region
    i = __find_memory(&_mmi, x);
    if (i < _mmi.i) {

      // check to see if there's space available before the first entry
      if (!ckd_add(&start, x, align - 1)) {
        start &= -align;
        if (!ckd_add(&end, start, n - 1)) {
          if (end < _mmi.p[i].x) {
            *res = start;
            return true;
          }
        }
      }

      // check to see if there's space available between two entries
      while (++i < _mmi.i) {
        if (!ckd_add(&start, _mmi.p[i - 1].y, 1) &&
            !ckd_add(&start, start, align - 1)) {
          start &= -align;
          if (!ckd_add(&end, start, n - 1)) {
            if (end < _mmi.p[i].x) {
              *res = start;
              return true;
            }
          }
        }
      }
    }

    // otherwise append after the last entry if space is available
    if (!ckd_add(&start, _mmi.p[i - 1].y, 1) &&
        !ckd_add(&start, start, align - 1)) {
      start &= -align;
      if (!ckd_add(&end, start, n - 1)) {
        *res = start;
        return true;
      }
    }

  } else {
    // if memtrack is empty, then just assign the requested address
    // assuming it doesn't overflow
    if (!ckd_add(&start, x, align - 1)) {
      start &= -align;
      if (!ckd_add(&end, start, n - 1)) {
        *res = start;
        return true;
      }
    }
  }

  return false;
}

static bool __auto_map(int count, int align, int *res) {
  return __choose_memory(FRAME(kAutomapStart), count, align, res) &&
         *res + count <= FRAME(kAutomapStart + (kAutomapSize - 1));
}

static void *__finish_memory(void *addr, size_t size, int prot, int flags,
                             int fd, int64_t off, int f, int x, int n,
                             struct DirectMap dm) {
  if (!IsWindows() && (flags & MAP_FIXED)) {
    if (__untrack_memories(addr, size)) {
      __mmap_die("FIXED UNTRACK FAILED");
    }
  }
  if (__track_memory(&_mmi, x, x + (n - 1), dm.maphandle, prot, flags, false,
                     false, off, size)) {
    if (sys_munmap(addr, n) == -1) {
      __mmap_die("TRACK MUNMAP FAILED");
    }
    return MAP_FAILED;
  }
  if (_weaken(__asan_map_shadow) && !OverlapsShadowSpace(addr, size)) {
    _weaken(__asan_map_shadow)((intptr_t)addr, size);
  }
  return addr;
}

static void *__map_memory(void *addr, size_t size, int prot, int flags, int fd,
                          int64_t off, int f, int x, int n) {
  struct DirectMap dm;
  dm = sys_mmap(addr, size, prot, f, fd, off);
  if (VERY_UNLIKELY(dm.addr == MAP_FAILED)) {
    if (IsWindows() && (flags & MAP_FIXED)) {
      __mmap_die("can't recover from MAP_FIXED errors on Windows");
    }
    return MAP_FAILED;
  }
  if (VERY_UNLIKELY(dm.addr != addr)) {
    __mmap_die("KERNEL DIDN'T RESPECT MAP_FIXED");
  }
  return __finish_memory(addr, size, prot, flags, fd, off, f, x, n, dm);
}

/**
 * Maps memory from system, one frame at a time.
 *
 * This is useful on Windows since it allows us to partially unmap or
 * punch holes into existing mappings.
 */
static textwindows dontinline void *__map_memories(char *addr, size_t size,
                                                   int prot, int flags, int fd,
                                                   int64_t off, int f, int x,
                                                   int n) {
  size_t i, m;
  int64_t oi, sz;
  struct DirectMap dm;
  bool iscow, readonlyfile;
  m = (size_t)(n - 1) << 16;
  unassert(m < size);
  unassert(m + FRAMESIZE >= size);
  oi = fd == -1 ? 0 : off + m;
  sz = size - m;
  dm = sys_mmap(addr + m, sz, prot, f, fd, oi);
  if (dm.addr == MAP_FAILED) return MAP_FAILED;
  iscow = (flags & MAP_TYPE) != MAP_SHARED && fd != -1;
  readonlyfile = (flags & MAP_TYPE) == MAP_SHARED && fd != -1 &&
                 (g_fds.p[fd].flags & O_ACCMODE) == O_RDONLY;
  if (__track_memory(&_mmi, x + (n - 1), x + (n - 1), dm.maphandle, prot, flags,
                     readonlyfile, iscow, oi, sz) == -1) {
    __mmap_die("__map_memories unrecoverable #1");
  }
  for (i = 0; i < m; i += FRAMESIZE) {
    oi = fd == -1 ? 0 : off + i;
    sz = FRAMESIZE;
    dm = sys_mmap(addr + i, sz, prot, f, fd, oi);
    if (dm.addr == MAP_FAILED ||
        __track_memory(&_mmi, x + i / FRAMESIZE, x + i / FRAMESIZE,
                       dm.maphandle, prot, flags, readonlyfile, iscow, oi,
                       sz) == -1) {
      __mmap_die("__map_memories unrecoverable #2");
    }
  }
  if (_weaken(__asan_map_shadow) && !OverlapsShadowSpace(addr, size)) {
    _weaken(__asan_map_shadow)((intptr_t)addr, size);
  }
  return addr;
}

inline void *__mmap_unlocked(void *addr, size_t size, int prot, int flags,
                             int fd, int64_t off) {
  char *p = addr;
  struct DirectMap dm;
  size_t requested_size;
  bool needguard, clashes;
  int a, f, n, x, pagesize;
  size_t virtualused, virtualneed;

  if (VERY_UNLIKELY(!size)) {
    STRACE("can't mmap zero bytes");
    return VIP(einval());
  }

  if (VERY_UNLIKELY(!ALIGNED(p))) {
    STRACE("cosmo mmap is 64kb aligned");
    return VIP(einval());
  }

  if (VERY_UNLIKELY(!IsLegalSize(size))) {
    STRACE("mmap size isn't legal");
    return VIP(einval());
  }

  if (VERY_UNLIKELY(!IsLegalPointer(p))) {
    STRACE("mmap addr isn't 48-bit");
    return VIP(einval());
  }

  if ((flags & (MAP_SHARED | MAP_PRIVATE)) == (MAP_SHARED | MAP_PRIVATE)) {
    flags = MAP_SHARED;  // cf. MAP_SHARED_VALIDATE
  }

  requested_size = size;
  pagesize = getauxval(AT_PAGESZ);
  if (flags & MAP_ANONYMOUS) {
    fd = -1;
    off = 0;
    size = ROUNDUP(size, FRAMESIZE);
    if ((flags & MAP_TYPE) == MAP_FILE) {
      STRACE("need MAP_PRIVATE or MAP_SHARED");
      return VIP(einval());
    }
  } else if (__isfdkind(fd, kFdZip)) {
    STRACE("mmap fd is zipos handle");
    return VIP(einval());
  } else if (VERY_UNLIKELY(off < 0)) {
    STRACE("mmap negative offset");
    return VIP(einval());
  } else if (off & ((IsWindows() ? FRAMESIZE : pagesize) - 1)) {
    STRACE("mmap offset isn't properly aligned");
    return VIP(einval());
  } else if (VERY_UNLIKELY(INT64_MAX - size < off)) {
    STRACE("mmap too large");
    return VIP(einval());
  }

  if (__virtualmax < LONG_MAX &&
      (ckd_add(&virtualneed, (virtualused = __get_memtrack_size(&_mmi)),
               size) ||
       virtualneed > __virtualmax)) {
    STRACE("mmap %'zu size + %'zu inuse exceeds virtual memory limit %'zu",
           size, virtualused, __virtualmax);
    return VIP(enomem());
  }

  clashes = OverlapsImageSpace(p, size) || __overlaps_existing_mapping(p, size);

  if ((flags & MAP_FIXED_NOREPLACE) == MAP_FIXED_NOREPLACE && clashes) {
    STRACE("mmap noreplace overlaps existing");
    return VIP(eexist());
  }

  if (ckd_add(&n, (int)(size >> 16), (int)!!(size & (FRAMESIZE - 1)))) {
    STRACE("mmap range overflows");
    return VIP(einval());
  }

  a = MAX(1, __rounddown2pow(size) >> 16);
  f = (flags & ~MAP_FIXED_NOREPLACE) | MAP_FIXED;
  if (flags & MAP_FIXED) {
    x = FRAME(p);
    if (IsWindows()) {
      if (__untrack_memories(p, size)) {
        __mmap_die("FIXED UNTRACK FAILED");
      }
    }
  } else if (p && !clashes && !OverlapsShadowSpace(p, size)) {
    x = FRAME(p);
  } else if (!__auto_map(n, a, &x)) {
    STRACE("automap has no room for %d frames with %d alignment", n, a);
    return VIP(enomem());
  }

  needguard = false;
  p = (char *)ADDR_32_TO_48(x);
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
      npassert(!sys_munmap(p, size));
      int guardsize = pagesize, e = errno;
      if ((dm = sys_mmap(p + size - guardsize, guardsize, prot,
                         f | MAP_GROWSDOWN_linux, fd, off))
              .addr != MAP_FAILED) {
        npassert(sys_mmap(p, pagesize, PROT_NONE,
                          MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)
                     .addr == p);
        dm.addr = p;
        p = __finish_memory(p, size, prot, flags, fd, off, f, x, n, dm);
        if (IsAsan() && p != MAP_FAILED) {
          __asan_poison(p, pagesize, kAsanStackOverflow);
        }
        return p;
      } else if (errno == ENOTSUP) {
        // WSL and Blink don't support MAP_GROWSDOWN
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
    p = __map_memory(p, size, prot, flags, fd, off, f, x, n);
  } else {
    p = __map_memories(p, size, prot, flags, fd, off, f, x, n);
  }

  if (p != MAP_FAILED) {
    if (IsAsan()) {
      __asan_poison(p + requested_size, size - requested_size,
                    kAsanMmapSizeOverrun);
    }
    if (needguard) {
      if (!IsWindows()) {
        unassert(!mprotect(p, pagesize, PROT_NONE));
        if (IsAsan()) {
          __asan_poison(p, pagesize, kAsanStackOverflow);
        }
      } else {
        uint32_t oldattr;
        unassert(VirtualProtect(p, pagesize, kNtPageReadwrite | kNtPageGuard,
                                &oldattr));
      }
    }
  }

  return p;
}

/**
 * Creates virtual memory, e.g.
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
 * @param size must be >0 otherwise EINVAL is raised
 * @param prot can have PROT_READ/PROT_WRITE/PROT_EXEC/PROT_NONE/etc.
 * @param flags should have one of the following masked by `MAP_TYPE`
 *     - `MAP_FILE` in which case `MAP_ANONYMOUS` shouldn't be used
 *     - `MAP_PRIVATE` for copy-on-write behavior of writeable pages
 *     - `MAP_SHARED` to create shared memory between processes
 *     - `MAP_STACK` to create a grows-down alloc, where a guard page
 *       is automatically protected at the bottom, sized as AT_PAGESZ
 *     Your `flags` may optionally bitwise or any of the following:
 *     - `MAP_ANONYMOUS` in which case `fd` and `off` are ignored
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
 *     made available w/ automatic reading at the chosen address
 * @param off specifies absolute byte index of fd's file for mapping,
 *     should be zero if MAP_ANONYMOUS is specified, which SHOULD be
 *     aligned to FRAMESIZE
 * @return virtual base address of new mapping, or MAP_FAILED w/ errno
 */
void *mmap(void *addr, size_t size, int prot, int flags, int fd, int64_t off) {
  void *res;
#if SYSDEBUG
  size_t toto = 0;
#if _KERNTRACE || _NTTRACE
  if (IsWindows()) {
    STRACE("mmap(%p, %'zu, %s, %s, %d, %'ld) → ...", addr, size,
           DescribeProtFlags(prot), DescribeMapFlags(flags), fd, off);
  }
#endif
#endif
  __mmi_lock();
  if (!__isfdkind(fd, kFdZip)) {
    res = __mmap_unlocked(addr, size, prot, flags, fd, off);
  } else {
    res = _weaken(__zipos_mmap)(
        addr, size, prot, flags,
        (struct ZiposHandle *)(intptr_t)g_fds.p[fd].handle, off);
  }
#if SYSDEBUG
  toto = __strace > 0 ? __get_memtrack_size(&_mmi) : 0;
#endif
  __mmi_unlock();
#if SYSDEBUG
  STRACE("mmap(%p, %'zu, %s, %s, %d, %'ld) → %p% m (%'zu bytes total)", addr,
         size, DescribeProtFlags(prot), DescribeMapFlags(flags), fd, off, res,
         toto);
#endif
  return res;
}

__strong_reference(mmap, mmap64);
