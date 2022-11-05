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
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/bits.h"
#include "libc/intrin/strace.internal.h"
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

static bool have_getrandom;

static ssize_t GetDevRandom(char *p, size_t n) {
  int fd;
  ssize_t rc;
  fd = __sys_openat(AT_FDCWD, "/dev/urandom", O_RDONLY | O_CLOEXEC, 0);
  if (fd == -1) return -1;
  pthread_cleanup_push((void *)sys_close, (void *)(intptr_t)fd);
  rc = sys_read(fd, p, n);
  pthread_cleanup_pop(1);
  return rc;
}

static ssize_t GetKernArnd(char *p, size_t n) {
  size_t m, i = 0;
  int cmd[2];
  if (IsFreebsd()) {
    cmd[0] = 1;  /* CTL_KERN */
    cmd[1] = 37; /* KERN_ARND */
  } else {
    cmd[0] = 1;  /* CTL_KERN */
    cmd[1] = 81; /* KERN_ARND */
  }
  for (;;) {
    m = n - i;
    if (sys_sysctl(cmd, 2, p + i, &m, 0, 0) != -1) {
      if ((i += m) == n) {
        return n;
      }
    } else {
      return i ? i : -1;
    }
  }
}

/**
 * Returns cryptographic random data.
 *
 * This random number seed generator obtains information from:
 *
 * - getrandom() on Linux
 * - RtlGenRandom() on Windows
 * - getentropy() on XNU and OpenBSD
 * - sysctl(KERN_ARND) on FreeBSD and NetBSD
 *
 * The following flags may be specified:
 *
 * - `GRND_RANDOM`: Halt the entire system while I tap an entropy pool
 *   so small that it's hard to use statistics to test if it's random
 * - `GRND_NONBLOCK`: Do not wait for i/o events or me to jiggle my
 *   mouse, and instead return immediately the moment data isn't
 *   available, even if the result needs to be -1 w/ EAGAIN
 *
 * This function is safe to use with fork() and vfork(). It will also
 * close any file descriptor it ends up needing before it returns.
 *
 * @note this function could block a nontrivial time on old computers
 * @note this function is indeed intended for cryptography
 * @note this function takes around 900 cycles
 * @raise EINVAL if `f` is invalid
 * @raise ENOSYS on bare metal
 * @cancellationpoint
 * @asyncsignalsafe
 * @restartable
 * @vforksafe
 */
ssize_t getrandom(void *p, size_t n, unsigned f) {
  ssize_t rc;
  const char *via;
  if ((f & ~(GRND_RANDOM | GRND_NONBLOCK))) {
    rc = einval();
    via = "n/a";
  } else if (IsWindows()) {
    via = "RtlGenRandom";
    rc = RtlGenRandom(p, n) ? n : __winerr();
  } else if (IsFreebsd() || IsNetbsd()) {
    via = "KERN_ARND";
    rc = GetKernArnd(p, n);
  } else if (have_getrandom) {
    via = "getrandom";
    if ((rc = sys_getrandom(p, n, f & (GRND_RANDOM | GRND_NONBLOCK))) != -1) {
      if (!rc && (IsXnu() || IsOpenbsd())) {
        rc = n;
      }
    }
  } else {
    via = "/dev/urandom";
    rc = GetDevRandom(p, n);
  }
  STRACE("getrandom(%p, %'zu, %#x) via %s → %'ld% m", p, n, f, via, rc);
  return rc;
}

__attribute__((__constructor__)) static textstartup void getrandom_init(void) {
  int e, rc;
  if (IsWindows()) return;
  e = errno;
  if (!(rc = sys_getrandom(0, 0, 0))) {
    have_getrandom = true;
  } else {
    errno = e;
  }
  STRACE("sys_getrandom(0,0,0) → %d% m", rc);
}
