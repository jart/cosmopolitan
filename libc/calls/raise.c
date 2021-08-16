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
#include "libc/calls/getconsolectrlevent.h"
#include "libc/calls/internal.h"
#include "libc/nt/console.h"
#include "libc/nt/runtime.h"
#include "libc/sysv/consts/sig.h"

/**
 * Sends signal to this process.
 *
 * @param sig can be SIGALRM, SIGINT, SIGTERM, SIGKILL, etc.
 * @return 0 on success or -1 w/ errno
 * @asyncsignalsafe
 */
int raise(int sig) {
  int event;
  if (sig == SIGTRAP) {
    DebugBreak();
    return 0;
  }
  if (sig == SIGFPE) {
    volatile int x = 0;
    x = 1 / x;
    return 0;
  }
  if (!IsWindows()) {
    return sys_kill(getpid(), sig, 1);
  } else if ((event = GetConsoleCtrlEvent(sig))) {
    if (GenerateConsoleCtrlEvent(event, 0)) {
      return 0;
    } else {
      return __winerr();
    }
  } else {
    ExitProcess(128 + sig);
  }
}
