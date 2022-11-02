/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/directmap.internal.h"
#include "libc/intrin/nopl.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/macros.internal.h"
#include "libc/runtime/brk.internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

struct Brk __brk;

static bool OverlapsMmappedMemory(unsigned char *p, size_t n) {
  int a, b, i;
  _unassert(n);
  a = (intptr_t)p >> 16;
  b = (intptr_t)(p + n - 1) >> 16;
  i = FindMemoryInterval(&_mmi, a);
  if (i < _mmi.i) {
    if (a <= _mmi.p[i].x && _mmi.p[i].x <= b) return true;
    if (a <= _mmi.p[i].y && _mmi.p[i].y <= b) return true;
    if (_mmi.p[i].x <= a && b <= _mmi.p[i].y) return true;
  }
  return false;
}

static unsigned char *brk_unlocked(unsigned char *p) {
  int rc;
  struct DirectMap dm;
  _unassert(!((intptr_t)__brk.p & (PAGESIZE - 1)));
  if (p >= __brk.p) {
    p = (unsigned char *)ROUNDUP((intptr_t)p, PAGESIZE);
  } else {
    p = (unsigned char *)ROUNDDOWN((intptr_t)p, PAGESIZE);
  }
  if (IsWindows()) {
    rc = enosys();
  } else if (p < _end) {
    rc = einval();
  } else if (p > __brk.p) {
    if (!OverlapsMmappedMemory(__brk.p, p - __brk.p)) {
      // we always polyfill this system call because
      // 1. Linux has brk() but its behavior is poorly documented
      // 2. FreeBSD has sbrk(int):int but it's foreseeable it could go away
      // 3. XNU/OpenBSD/NetBSD have all deleted this interface in the kernel
      dm = sys_mmap(__brk.p, p - __brk.p, PROT_READ | PROT_WRITE,
                    MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
      rc = (int)(intptr_t)dm.addr;  // safe b/c __brk.p is page-aligned
    } else {
      rc = eexist();
    }
  } else if (p < __brk.p) {
    rc = sys_munmap(p, __brk.p - p);
  } else {
    rc = 0;
  }
  if (rc != -1) {
    __brk.p = p;
    return 0;
  } else {
    return (unsigned char *)-1;
  }
}

void brk_lock(void) {
  pthread_mutex_lock(&__brk.m);
}

void brk_unlock(void) {
  pthread_mutex_unlock(&__brk.m);
}

void brk_funlock(void) {
  pthread_mutex_init(&__brk.m, 0);
}

__attribute__((__constructor__)) static void brk_init(void) {
  brk_funlock();
  pthread_atfork(brk_lock, brk_unlock, brk_funlock);
}

#ifdef _NOPL0
#define brk_lock()   _NOPL0("__threadcalls", brk_lock)
#define brk_unlock() _NOPL0("__threadcalls", brk_unlock)
#endif

/**
 * Sets end of data section.
 *
 * Your program break starts right after end of `.bss` as defined
 * by the external linker-defined variable `end`. Setting it to a
 * higher address will allocate more memory. After using this you
 * may dealocate memory by specifying it back to a lower address.
 *
 * The only virtue of brk(), and sbrk(), aside from compatibility
 * with legacy software, is it's tinier than mmap() because since
 * this API only supports Unix, we don't bother doing the complex
 * memory interval tracking that mmap() does.
 *
 * @param neu is the new end address of data segment, which shall
 *     be rounded automatically to a 4096-byte granularity
 * @return 0 on success, or -1 w/ errno
 * @raise EINVAL if `neu` is less than the `end` of `.bss`
 * @raise EEXIST if expanded break would overlap existing mmap() memory
 * @raise ENOMEM if `RLIMIT_DATA` / `RLIMIT_AS` / `RLIMIT_RSS` is exceeded
 * @raise ENOSYS on Windows because WIN32 puts random stuff after your break
 * @threadsafe
 */
int brk(void *neu) {
  unsigned char *rc;
  brk_lock();
  if (!__brk.p) __brk.p = _end;
  rc = brk_unlocked(neu);
  brk_unlock();
  STRACE("brk(%p) → %d% m", neu, rc);
  return (int)(intptr_t)rc;
}

/**
 * Adjusts end of data section.
 *
 * This shrinks or increases the program break by delta bytes. On
 * success, the previous program break is returned. It's possible
 * to pass 0 to this function to obtain the current program break
 * which is initially set to the linker-defined external variable
 * `end` which is the end of the `.bss` segment. Your allocations
 * are rounded automatically to a 4096-byte granularity.
 *
 * The only virtue of sbrk(), and brk(), aside from compatibility
 * with legacy software, is it's tinier than mmap() because since
 * this API only supports Unix, we don't bother doing the complex
 * memory interval tracking that mmap() does.
 *
 * @param delta is the number of bytes to allocate (or free if negative)
 *     noting that your delta may be tuned to a number further from zero
 *     to accommodate the page size granularity of this allocator
 * @return previous break on success, or `(void *)-1` w/ errno
 * @raise EINVAL if new break would be less than the `end` of `.bss`
 * @raise EEXIST if expanded break would overlap existing mmap() memory
 * @raise EOVERFLOW if `delta` added to break overflows the address space
 * @raise ENOMEM if `RLIMIT_DATA` / `RLIMIT_AS` / `RLIMIT_RSS` is exceeded
 * @raise ENOSYS on Windows because WIN32 puts random stuff after your break
 * @threadsafe
 */
void *sbrk(intptr_t delta) {
  intptr_t neu;
  unsigned char *rc, *old;
  brk_lock();
  if (!__brk.p) __brk.p = _end;
  old = __brk.p;
  if (!__builtin_add_overflow((intptr_t)__brk.p, delta, &neu) &&
      IsLegalPointer((unsigned char *)neu)) {
    rc = brk_unlocked((unsigned char *)neu);
    if (!rc) rc = old;
  } else {
    rc = (void *)eoverflow();
  }
  brk_unlock();
  STRACE("sbrk(%'ld) → %p% m", delta, rc);
  return rc;
}
