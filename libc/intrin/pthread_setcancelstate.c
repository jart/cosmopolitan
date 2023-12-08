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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

/**
 * Sets cancelability state.
 *
 * This function may be used to temporarily disable cancelation for the
 * calling thread, which is necessary in cases when a @cancelationpoint
 * function is invoked from an @asyncsignalsafe function.
 *
 * Cosmopolitan Libc supports the Musl Libc `PTHREAD_CANCEL_MASKED`
 * non-POSIX extension. Any thread may use this setting, in which case
 * the thread won't be abruptly destroyed upon a cancelation and have
 * its stack unwound; instead, the thread will encounter an `ECANCELED`
 * errno the next time it calls a cancelation point.
 *
 * @param state may be one of:
 *     - `PTHREAD_CANCEL_ENABLE` (default)
 *     - `PTHREAD_CANCEL_DISABLE`
 *     - `PTHREAD_CANCEL_MASKED`
 * @param oldstate optionally receives old value
 * @return 0 on success, or errno on error
 * @raise EINVAL if `state` has bad value
 * @asyncsignalsafe
 */
errno_t pthread_setcancelstate(int state, int *oldstate) {
  errno_t err;
  struct PosixThread *pt;
  if (__tls_enabled && (pt = _pthread_self())) {
    switch (state) {
      case PTHREAD_CANCEL_ENABLE:
      case PTHREAD_CANCEL_DISABLE:
      case PTHREAD_CANCEL_MASKED:
        if (oldstate) {
          if (pt->pt_flags & PT_NOCANCEL) {
            *oldstate = PTHREAD_CANCEL_DISABLE;
          } else if (pt->pt_flags & PT_MASKED) {
            *oldstate = PTHREAD_CANCEL_MASKED;
          } else {
            *oldstate = PTHREAD_CANCEL_ENABLE;
          }
        }
        pt->pt_flags &= ~(PT_NOCANCEL | PT_MASKED);
        if (state == PTHREAD_CANCEL_MASKED) {
          pt->pt_flags |= PT_MASKED;
        } else if (state == PTHREAD_CANCEL_DISABLE) {
          pt->pt_flags |= PT_NOCANCEL;
        }
        err = 0;
        break;
      default:
        err = EINVAL;
        break;
    }
  } else {
    if (oldstate) {
      *oldstate = 0;
    }
    err = 0;
  }
#if IsModeDbg()
  STRACE("pthread_setcancelstate(%s, [%s]) → %s",
         DescribeCancelState(0, &state), DescribeCancelState(err, oldstate),
         DescribeErrno(err));
#endif
  return err;
}

int _pthread_block_cancelation(void) {
  int oldstate;
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &oldstate);
  return oldstate;
}

void _pthread_allow_cancelation(int oldstate) {
  pthread_setcancelstate(oldstate, 0);
}
