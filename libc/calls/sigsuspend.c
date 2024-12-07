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
#include "libc/calls/cp.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/struct/timespec.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/strace.h"
#include "libc/nt/synchronization.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/tls.h"

/**
 * Blocks until SIG ∉ MASK is delivered to thread.
 *
 * This temporarily replaces the signal mask until a signal that it
 * doesn't contain is delivered.
 *
 * @param ignore is a bitset of signals to block temporarily, which if
 *     NULL is equivalent to passing an empty signal set
 * @return -1 w/ EINTR (or possibly EFAULT)
 * @cancelationpoint
 * @asyncsignalsafe
 * @norestart
 */
int sigsuspend(const sigset_t *ignore) {
  int rc;
  BEGIN_CANCELATION_POINT;

  if (IsXnu() || IsOpenbsd()) {
    // openbsd and xnu use a 32 signal register convention
    rc = sys_sigsuspend(ignore ? (void *)(intptr_t)(uint32_t)*ignore : 0, 8);
  } else {
    sigset_t waitmask = ignore ? *ignore : 0;
    if (IsWindows() || IsMetal()) {
      // we don't strictly need to block signals, but it reduces signal
      // delivery latency, by preventing other threads from delivering a
      // signal asynchronously. it takes about ~5us to deliver a signal
      // using SetEvent() whereas it takes ~30us to use SuspendThread(),
      // GetThreadContext(), SetThreadContext(), and ResumeThread().
      BLOCK_SIGNALS;
      rc = _park_norestart(timespec_max, waitmask);
      ALLOW_SIGNALS;
    } else {
      rc = sys_sigsuspend((uint64_t[2]){waitmask}, 8);
    }
  }

  END_CANCELATION_POINT;
  STRACE("sigsuspend(%s) → %d% m", DescribeSigset(0, ignore), rc);
  return rc;
}
