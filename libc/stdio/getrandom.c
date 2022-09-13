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
#include "libc/intrin/strace.internal.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/bits.h"
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

static bool have_getrandom;

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
 * - GRND_RANDOM: Halt the entire system while I tap an entropy pool
 *   so small that it's hard to use statistics to test if it's random
 * - GRND_NONBLOCK: Do not wait for i/o events or me to jiggle my
 *   mouse, and instead return immediately the moment data isn't
 *   available, even if the result needs to be -1 w/ EAGAIN
 *
 * This function is safe to use with fork() and vfork(). It will also
 * close any file descriptor it ends up needing before it returns.
 *
 * @note this function could block a nontrivial time on old computers
 * @note this function is indeed intended for cryptography
 * @note this function takes around 900 cycles
 * @asyncsignalsafe
 * @restartable
 * @vforksafe
 */
ssize_t getrandom(void *p, size_t n, unsigned f) {
  char cf;
  ssize_t rc;
  uint64_t x;
  int fd, cmd[2];
  size_t i, j, m;
  const char *via;
  sigset_t neu, old;
  if (n > 256) n = 256;
  if ((f & ~(GRND_RANDOM | GRND_NONBLOCK))) return einval();
  if (IsWindows()) {
    via = "RtlGenRandom";
    if (RtlGenRandom(p, n)) {
      rc = n;
    } else {
      rc = __winerr();
    }
  } else if (IsFreebsd() || IsNetbsd()) {
    via = "KERN_ARND";
    if (IsFreebsd()) {
      cmd[0] = 1;  /* CTL_KERN */
      cmd[1] = 37; /* KERN_ARND */
    } else {
      cmd[0] = 1;  /* CTL_KERN */
      cmd[1] = 81; /* KERN_ARND */
    }
    m = n;
    if ((rc = sysctl(cmd, 2, p, &m, 0, 0)) != -1) {
      rc = m;
    }
  } else if (have_getrandom) {
    via = "getrandom";
    if ((rc = sys_getrandom(p, n, f & (GRND_RANDOM | GRND_NONBLOCK))) != -1) {
      if (!rc && (IsXnu() || IsOpenbsd())) {
        rc = n;
      }
    }
  } else if ((fd = __sys_openat(
                  AT_FDCWD,
                  (via = (f & GRND_RANDOM) ? "/dev/random" : "/dev/urandom"),
                  O_RDONLY | ((f & GRND_NONBLOCK) ? O_NONBLOCK : 0), 0)) !=
             -1) {
    rc = sys_read(fd, p, n);
    sys_close(fd);
  } else {
    rc = enosys();
  }
  STRACE("getrandom(%p, %'zu, %#x) via %s → %'ld% m", p, n, f, via, rc);
  return rc;
}

static textstartup void getrandom_init(void) {
  int e, rc;
  e = errno;
  struct sigaction sa, oldsa;
  if (IsBsd()) {
    sa.sa_flags = 0;
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGSYS, &sa, &oldsa);
  }
  if (!(rc = sys_getrandom(0, 0, 0))) {
    have_getrandom = true;
  }
  STRACE("sys_getrandom(0,0,0) → %d% m", rc);
  if (IsBsd()) {
    sigaction(SIGSYS, &oldsa, 0);
  }
  errno = e;
}

const void *const g_getrandom_init[] initarray = {getrandom_init};
