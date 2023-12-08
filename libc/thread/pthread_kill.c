/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/sig.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"

/**
 * Sends signal to thread.
 *
 * @return 0 on success, or errno on error
 * @raise ESRCH if `tid` was valid but no such thread existed
 * @raise EAGAIN if `RLIMIT_SIGPENDING` was exceeded
 * @raise EINVAL if `sig` wasn't a legal signal
 * @raise EPERM if permission was denied
 * @asyncsignalsafe
 */
errno_t pthread_kill(pthread_t thread, int sig) {
  int err = 0;
  struct PosixThread *pt;
  pt = (struct PosixThread *)thread;
  if (!(1 <= sig && sig <= 64)) {
    err = EINVAL;
  } else if (thread == __get_tls()->tib_pthread) {
    err = raise(sig);  // XNU will EDEADLK it otherwise
  } else if (atomic_load_explicit(&pt->pt_status, memory_order_acquire) >=
             kPosixThreadTerminated) {
    err = ESRCH;
  } else if (IsWindows()) {
    err = __sig_kill(pt, sig, SI_TKILL);
  } else {
    if (IsXnuSilicon()) {
      err = __syslib->__pthread_kill(_pthread_syshand(pt), sig);
    } else {
      int e = errno;
      if (sys_tkill(_pthread_tid(pt), sig, pt->tib)) {
        err = errno;
        errno = e;
      }
    }
    if (err == ESRCH) {
      err = 0;  // we already reported this
    }
  }
  STRACE("pthread_kill(%d, %G) → %s", _pthread_tid(pt), sig,
         DescribeErrno(err));
  return err;
}
