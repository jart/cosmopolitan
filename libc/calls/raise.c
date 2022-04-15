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
#include "libc/calls/getconsolectrlevent.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/strace.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/nt/console.h"
#include "libc/nt/errors.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"

static textwindows inline bool HasWorkingConsole(void) {
  return !!(__ntconsolemode[0] | __ntconsolemode[1] | __ntconsolemode[2]);
}

/**
 * Sends signal to this process.
 *
 * @param sig can be SIGALRM, SIGINT, SIGTERM, SIGKILL, etc.
 * @return 0 on success or -1 w/ errno
 * @asyncsignalsafe
 */
int raise(int sig) {
  int rc, event;
  STRACE("raise(%G) → ...", sig);
  if (sig == SIGTRAP) {
    DebugBreak();
    rc = 0;
  } else if (sig == SIGFPE) {
    volatile int x = 0;
    x = 1 / x;
    rc = 0;
  } else if (!IsWindows()) {
    // XXX: should be tkill() or tgkill() on linux
    rc = sys_kill(getpid(), sig, 1);
  } else {
    if (HasWorkingConsole() && (event = GetConsoleCtrlEvent(sig)) != -1) {
      // XXX: MSDN says "If this parameter is zero, the signal is
      //      generated in all processes that share the console of the
      //      calling process." which seems to imply multiple process
      //      groups potentially. We just shouldn't use this because it
      //      doesn't make any sense and it's so evil.
      if (GenerateConsoleCtrlEvent(event, 0)) {
        // XXX: we shouldn't need to sleep here ctrl-c is evil on nt
        if (SleepEx(100, true) == kNtWaitIoCompletion) {
          STRACE("IOCP TRIGGERED EINTR");
        }
        __sig_check(false);
        rc = 0;
      } else {
        rc = __winerr();
      }
    } else {
      rc = __sig_raise(sig, SI_USER);
    }
  }
  STRACE("...raise(%G) → %d% m", sig, rc);
  return rc;
}
