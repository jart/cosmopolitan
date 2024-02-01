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
#include "libc/calls/blockcancel.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/cp.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/asmflag.h"
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
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/grnd.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/thread.h"

__static_yoink("rdrand_init");

int sys_getentropy(void *, size_t) asm("sys_getrandom");
ssize_t sys_getrandom_metal(char *, size_t, int);

static bool have_getrandom;

static void GetRandomEntropy(char *p, size_t n) {
  unassert(n <= 256);
  if (sys_getentropy(p, n)) notpossible;
}

static void GetRandomArnd(char *p, size_t n) {
  size_t m;
  int cmd[2];
  cmd[0] = 1;                      // CTL_KERN
  cmd[1] = IsFreebsd() ? 37 : 81;  // KERN_ARND
  unassert((m = n) <= 256);
  if (sys_sysctl(cmd, 2, p, &n, 0, 0) == -1) notpossible;
  if (m != n) notpossible;
}

static ssize_t GetRandomBsd(char *p, size_t n, void impl(char *, size_t)) {
  size_t m, i;
  for (i = 0;;) {
    m = MIN(n - i, 256);
    impl(p + i, m);
    if ((i += m) == n) {
      return n;
    }
  }
}

static ssize_t GetDevUrandom(char *p, size_t n) {
  int fd;
  ssize_t rc;
  BLOCK_SIGNALS;
  BLOCK_CANCELATION;
  fd = sys_openat(AT_FDCWD, "/dev/urandom", O_RDONLY | O_CLOEXEC, 0);
  if (fd != -1) {
    rc = sys_read(fd, p, n);
  } else {
    rc = -1;
  }
  ALLOW_CANCELATION;
  ALLOW_SIGNALS;
  return rc;
}

ssize_t __getrandom(void *p, size_t n, unsigned f) {
  ssize_t rc;
  if (IsWindows()) {
    rc = ProcessPrng(p, n) ? n : __winerr();
  } else if (have_getrandom) {
    if (IsXnu() || IsOpenbsd()) {
      rc = GetRandomBsd(p, n, GetRandomEntropy);
    } else {
      BEGIN_CANCELATION_POINT;
      rc = sys_getrandom(p, n, f);
      END_CANCELATION_POINT;
    }
  } else if (IsFreebsd() || IsNetbsd()) {
    rc = GetRandomBsd(p, n, GetRandomArnd);
#ifdef __x86_64__
  } else if (IsMetal()) {
    rc = sys_getrandom_metal(p, n, f);
#endif
  } else {
    BEGIN_CANCELATION_POINT;
    rc = GetDevUrandom(p, n);
    END_CANCELATION_POINT;
  }
  return rc;
}

/**
 * Returns cryptographic random data.
 *
 * This random number seed generator obtains information from:
 *
 * - ProcessPrng() on Windows
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
 * Unlike getentropy() this function is a cancelation point. But it
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
 * @cancelationpoint
 * @asyncsignalsafe
 * @restartable
 * @vforksafe
 */
ssize_t getrandom(void *p, size_t n, unsigned f) {
  ssize_t rc;
  if ((!p && n) || (IsAsan() && !__asan_is_valid(p, n))) {
    rc = efault();
  } else if (f & ~(GRND_RANDOM | GRND_NONBLOCK)) {
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
  BLOCK_CANCELATION;
  e = errno;
  if (!(rc = sys_getrandom(0, 0, 0))) {
    have_getrandom = true;
  } else {
    errno = e;
  }
  ALLOW_CANCELATION;
  STRACE("sys_getrandom(0,0,0) → %d% m", rc);
}
