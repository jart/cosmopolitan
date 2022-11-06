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
#include "libc/calls/blockcancel.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/cp.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/asmflag.h"
#include "libc/intrin/bits.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/kcpuids.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/nexgen32e/vendor.internal.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/nexgen32e/x86info.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/xorshift.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/grnd.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/thread.h"

STATIC_YOINK("rdrand_init");

int sys_getentropy(void *, size_t) asm("sys_getrandom");

static bool have_getrandom;

static bool GetRandomRdseed(uint64_t *out) {
  int i;
  char cf;
  uint64_t x;
  for (i = 0; i < 10; ++i) {
    asm volatile(CFLAG_ASM("rdseed\t%1")
                 : CFLAG_CONSTRAINT(cf), "=r"(x)
                 : /* no inputs */
                 : "cc");
    if (cf) {
      *out = x;
      return true;
    }
    asm volatile("pause");
  }
  return false;
}

static bool GetRandomRdrand(uint64_t *out) {
  int i;
  char cf;
  uint64_t x;
  for (i = 0; i < 10; ++i) {
    asm volatile(CFLAG_ASM("rdrand\t%1")
                 : CFLAG_CONSTRAINT(cf), "=r"(x)
                 : /* no inputs */
                 : "cc");
    if (cf) {
      *out = x;
      return true;
    }
    asm volatile("pause");
  }
  return false;
}

static ssize_t GetRandomCpu(char *p, size_t n, int f, bool impl(uint64_t *)) {
  uint64_t x;
  size_t i, j;
  for (i = 0; i < n; i += j) {
  TryAgain:
    if (!impl(&x)) {
      if (f || i >= 256) break;
      goto TryAgain;
    }
    for (j = 0; j < 8 && i + j < n; ++j) {
      p[i + j] = x;
      x >>= 8;
    }
  }
  return n;
}

static ssize_t GetRandomMetal(char *p, size_t n, int f) {
  if (f & GRND_RANDOM) {
    if (X86_HAVE(RDSEED)) {
      return GetRandomCpu(p, n, f, GetRandomRdseed);
    } else {
      return enosys();
    }
  } else {
    if (X86_HAVE(RDRND)) {
      return GetRandomCpu(p, n, f, GetRandomRdrand);
    } else {
      return enosys();
    }
  }
}

static void GetRandomEntropy(char *p, size_t n) {
  _unassert(n <= 256);
  if (sys_getentropy(p, n)) notpossible;
}

static void GetRandomArnd(char *p, size_t n) {
  size_t m;
  int cmd[2];
  cmd[0] = 1;                      // CTL_KERN
  cmd[1] = IsFreebsd() ? 37 : 81;  // KERN_ARND
  _unassert((m = n) <= 256);
  if (sys_sysctl(cmd, 2, p, &n, 0, 0) == -1) notpossible;
  if (m != n) notpossible;
}

static ssize_t GetRandomBsd(char *p, size_t n, void impl(char *, size_t)) {
  errno_t e;
  size_t m, i;
  if (_weaken(pthread_testcancel_np) &&
      (e = _weaken(pthread_testcancel_np)())) {
    errno = e;
    return -1;
  }
  for (i = 0;;) {
    m = MIN(n - i, 256);
    impl(p + i, m);
    if ((i += m) == n) {
      return n;
    }
    if (_weaken(pthread_testcancel)) {
      _weaken(pthread_testcancel)();
    }
  }
}

static ssize_t GetDevUrandom(char *p, size_t n) {
  int fd;
  ssize_t rc;
  fd = sys_openat(AT_FDCWD, "/dev/urandom", O_RDONLY | O_CLOEXEC, 0);
  if (fd == -1) return -1;
  pthread_cleanup_push((void *)sys_close, (void *)(intptr_t)fd);
  rc = sys_read(fd, p, n);
  pthread_cleanup_pop(1);
  return rc;
}

ssize_t __getrandom(void *p, size_t n, unsigned f) {
  ssize_t rc;
  if (IsWindows()) {
    if (_check_interrupts(true, 0)) return -1;
    rc = RtlGenRandom(p, n) ? n : __winerr();
  } else if (have_getrandom) {
    if (IsXnu() || IsOpenbsd()) {
      rc = GetRandomBsd(p, n, GetRandomEntropy);
    } else {
      BEGIN_CANCELLATION_POINT;
      rc = sys_getrandom(p, n, f);
      END_CANCELLATION_POINT;
    }
  } else if (IsFreebsd() || IsNetbsd()) {
    rc = GetRandomBsd(p, n, GetRandomArnd);
  } else if (IsMetal()) {
    rc = GetRandomMetal(p, n, f);
  } else {
    BEGIN_CANCELLATION_POINT;
    rc = GetDevUrandom(p, n);
    END_CANCELLATION_POINT;
  }
  return rc;
}

/**
 * Returns cryptographic random data.
 *
 * This random number seed generator obtains information from:
 *
 * - RtlGenRandom() on Windows
 * - getentropy() on XNU and OpenBSD
 * - getrandom() on Linux, FreeBSD, and NetBSD
 * - sysctl(KERN_ARND) on older versions of FreeBSD and NetBSD
 *
 * Unlike getentropy() this function is interruptible. However EINTR
 * shouldn't be possible if `f` is zero and `n` is no more than 256,
 * noting that kernels are a bit vague with their promises here, and
 * if you're willing to trade some performance for a more assurances
 * that EINTR won't happen, then either consider using getentropy(),
 * or using the `SA_RESTART` flag on your signal handlers.
 *
 * Unlike getentropy() you may specify an `n` greater than 256. When
 * larger amounts are specified, the caller must be prepared for the
 * case where fewer than `n` bytes are returned. In that case, it is
 * likely that a signal delivery occured. Cancellations in mask mode
 * also need to be suppressed while processing the bytes beyond 256.
 * On BSD OSes, this entire process is uninterruptible so be careful
 * when using large sizes if interruptibility is needed.
 *
 * Unlike getentropy() this function is a cancellation point. But it
 * shouldn't be a problem, unless you're using masked mode, in which
 * case extra care must be taken to consider the result.
 *
 * It's recommended that `f` be set to zero, although it may include
 * the following flags:
 *
 * - `GRND_NONBLOCK` when you want to elevate the insecurity of your
 *   random data
 *
 * - `GRND_RANDOM` if you want to have the best possible chance your
 *   program will freeze and the system operator is paged to address
 *   the outage by driving to the data center and jiggling the mouse
 *
 * @note this function could block a nontrivial time on old computers
 * @note this function is indeed intended for cryptography
 * @note this function takes around 900 cycles
 * @raise EINVAL if `f` is invalid
 * @raise ECANCELED if thread was cancelled in masked mode
 * @raise EFAULT if the `n` bytes at `p` aren't valid memory
 * @raise EINTR if we needed to block and a signal was delivered instead
 * @cancellationpoint
 * @asyncsignalsafe
 * @restartable
 * @vforksafe
 */
ssize_t getrandom(void *p, size_t n, unsigned f) {
  ssize_t rc;
  if ((!p && n) || (IsAsan() && !__asan_is_valid(p, n))) {
    rc = efault();
  } else if ((f & ~(GRND_RANDOM | GRND_NONBLOCK))) {
    rc = einval();
  } else {
    rc = __getrandom(p, n, f);
  }
  STRACE("getrandom(%p, %'zu, %#x) → %'ld% m", p, n, f, rc);
  return rc;
}

__attribute__((__constructor__)) static textstartup void getrandom_init(void) {
  int e, rc;
  if (IsWindows() || IsMetal()) return;
  BLOCK_CANCELLATIONS;
  e = errno;
  if (!(rc = sys_getrandom(0, 0, 0))) {
    have_getrandom = true;
  } else {
    errno = e;
  }
  ALLOW_CANCELLATIONS;
  STRACE("sys_getrandom(0,0,0) → %d% m", rc);
}
