/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/nt/enum/wait.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"

#ifdef __x86_64__

static textwindows bool CheckForExitedChildProcess(void) {
  int pids[64];
  uint32_t i, n;
  int64_t handles[64];
  if (!(n = __sample_pids(pids, handles, true))) return false;
  i = WaitForMultipleObjects(n, handles, false, 0);
  if (i == kNtWaitFailed) return false;
  if (i == kNtWaitTimeout) return false;
  if ((__sighandrvas[SIGCHLD] >= kSigactionMinRva) &&
      (__sighandflags[SIGCHLD] & SA_NOCLDWAIT)) {
    CloseHandle(handles[i]);
    __releasefd(pids[i]);
  } else {
    g_fds.p[pids[i]].zombie = true;
  }
  return true;
}

/**
 * Checks to see if SIGCHLD should be raised on Windows.
 * @return true if a signal was raised
 * @note yoinked by fork-nt.c
 */
textwindows void _check_sigchld(void) {
  bool should_signal;
  __fds_lock();
  should_signal = CheckForExitedChildProcess();
  __fds_unlock();
  if (should_signal) {
    __sig_add(0, SIGCHLD, CLD_EXITED);
  }
}

#endif /* __x86_64__ */
