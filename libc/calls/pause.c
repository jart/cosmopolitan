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
#include "libc/calls/calls.h"
#include "libc/calls/cp.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sock/internal.h"

/**
 * Waits for signal.
 *
 * This suspends execution until an unmasked signal is delivered. If the
 * signal delivery kills the process, this won't return. The signal mask
 * of the current thread is used. If a signal handler exists, this shall
 * return after it's been invoked.
 *
 * This function is equivalent to:
 *
 *     select(0, 0, 0, 0, 0);
 *
 * However this has a tinier footprint and better logging.
 *
 * @return -1 w/ errno
 * @raise ECANCELED if this thread was canceled in masked mode
 * @raise EINTR if interrupted by a signal
 * @cancelationpoint
 * @see sigsuspend()
 * @norestart
 */
int pause(void) {
  int rc;
  STRACE("pause() → [...]");
  BEGIN_CANCELATION_POINT;

  if (!IsWindows()) {
    // We'll polyfill pause() using select() with a null timeout, which
    // should hopefully do the same thing, which means wait forever but
    // the usual signal interrupt rules apply.
    //
    //     "If the readfds, writefds, and errorfds arguments are all null
    //      pointers and the timeout argument is not a null pointer, the
    //      pselect() or select() function shall block for the time
    //      specified, or until interrupted by a signal. If the readfds,
    //      writefds, and errorfds arguments are all null pointers and the
    //      timeout argument is a null pointer, the pselect() or select()
    //      function shall block until interrupted by a signal." ──Quoth
    //      IEEE 1003.1-2017 §functions/select
    //
#ifdef __aarch64__
    rc = sys_pselect(0, 0, 0, 0, 0, 0);
#else
    rc = sys_select(0, 0, 0, 0, 0);
#endif
  } else {
    rc = sys_pause_nt();
  }

  END_CANCELATION_POINT;
  STRACE("[...] pause → %d% m", rc);
  return rc;
}
