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
#include "libc/calls/sig.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/strace.internal.h"
#include "libc/runtime/internal.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/thread/tls.h"
#include "libc/thread/xnu.internal.h"

static textwindows inline bool HasWorkingConsole(void) {
  return !!(__ntconsolemode[0] | __ntconsolemode[1] | __ntconsolemode[2]);
}

static noubsan void RaiseSigFpe(void) {
  volatile int x = 0;
  x = 1 / x;
}

/**
 * Sends signal to self.
 *
 * This is basically the same as:
 *
 *     tkill(gettid(), sig);
 *
 * Note `SIG_DFL` still results in process death for most signals.
 *
 * This function is not entirely equivalent to kill() or tkill(). For
 * example, we raise `SIGTRAP` and `SIGFPE` the natural way, since that
 * helps us support Windows. So if the raised signal has a signal
 * handler, then the reported `si_code` might not be `SI_TKILL`.
 *
 * On Windows, if a signal results in the termination of the process
 * then we use the convention `_Exit(128 + sig)` to notify the parent of
 * the signal number.
 *
 * @param sig can be SIGALRM, SIGINT, SIGTERM, SIGKILL, etc.
 * @return 0 if signal was delivered and returned, or -1 w/ errno
 * @asyncsignalsafe
 */
int raise(int sig) {
  int rc, tid, event;
  STRACE("raise(%G) → ...", sig);
  if (sig == SIGTRAP) {
    DebugBreak();
    rc = 0;
  } else if (sig == SIGFPE) {
    RaiseSigFpe();
    rc = 0;
  } else if (!IsWindows() && !IsMetal()) {
    rc = sys_tkill(gettid(), sig, 0);
  } else {
    rc = __sig_raise(sig, SI_TKILL);
  }
  STRACE("...raise(%G) → %d% m", sig, rc);
  return rc;
}
