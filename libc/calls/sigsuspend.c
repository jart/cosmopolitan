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
#include "libc/calls/cp.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/log/backtrace.internal.h"
#include "libc/nt/errors.h"
#include "libc/nt/synchronization.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"

/**
 * Blocks until SIG ∉ MASK is delivered to thread.
 *
 * This temporarily replaces the signal mask until a signal that it
 * doesn't contain is delivered.
 *
 * @param ignore is a bitset of signals to block temporarily, which if
 *     NULL is equivalent to passing an empty signal set
 * @return -1 w/ EINTR (or possibly EFAULT)
 * @cancellationpoint
 * @asyncsignalsafe
 * @norestart
 */
int sigsuspend(const sigset_t *ignore) {
  int rc;
  long ms, totoms;
  sigset_t save, *arg, mask = {0};
  STRACE("sigsuspend(%s) → ...", DescribeSigset(0, ignore));
  BEGIN_CANCELLATION_POINT;

  if (IsAsan() && ignore && !__asan_is_valid(ignore, sizeof(*ignore))) {
    rc = efault();
  } else if (IsXnu() || IsOpenbsd()) {
    // openbsd and xnu only support 32 signals
    // they use a register calling convention for sigsuspend
    if (ignore) {
      arg = (sigset_t *)(uintptr_t)(*(uint32_t *)ignore);
    } else {
      arg = 0;
    }
    rc = sys_sigsuspend(arg, 8);
  } else if (IsLinux() || IsFreebsd() || IsNetbsd() || IsWindows()) {
    if (ignore) {
      arg = ignore;
    } else {
      arg = &mask;
    }
    if (!IsWindows()) {
      rc = sys_sigsuspend(arg, 8);
    } else {
      __sig_mask(SIG_SETMASK, arg, &save);
      ms = 0;
      totoms = 0;
      do {
        if ((rc = _check_interrupts(false, g_fds.p))) {
          break;
        }
        if (SleepEx(__SIG_POLLING_INTERVAL_MS, true) == kNtWaitIoCompletion) {
          POLLTRACE("IOCP EINTR");
          continue;
        }
#if defined(SYSDEBUG) && defined(_POLLTRACE)
        ms += __SIG_POLLING_INTERVAL_MS;
        if (ms >= __SIG_LOGGING_INTERVAL_MS) {
          totoms += ms, ms = 0;
          POLLTRACE("... sigsuspending for %'lums...", totoms);
        }
#endif
      } while (1);
      __sig_mask(SIG_SETMASK, &save, 0);
    }
  } else {
    // TODO(jart): sigsuspend metal support
    rc = enosys();
  }

  END_CANCELLATION_POINT;
  STRACE("...sigsuspend → %d% m", rc);
  return rc;
}
