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
#include "libc/calls/internal.h"
#include "libc/calls/strace.internal.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/typedef/sigaction_f.h"
#include "libc/nt/enum/status.h"
#include "libc/nt/enum/wait.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"

/**
 * Checks to see if SIGCHLD should be raised on Windows.
 * @return true if a signal was raised
 */
bool _check_sigchld(void) {
  siginfo_t si;
  int pids[64];
  uint32_t i, n;
  int64_t handles[64];
  if (__sighandrvas[SIGCHLD] < kSigactionMinRva) return false;
  if (!(n = __sample_pids(pids, handles))) return false;
  i = WaitForMultipleObjects(n, handles, false, 0);
  if (i == kNtWaitTimeout) return false;
  if (i == kNtWaitFailed) {
    STRACE("%s failed %u", "WaitForMultipleObjects", GetLastError());
    return false;
  }
  STRACE("SIGCHLD fd=%d handle=%ld", pids[i], handles[i]);
  bzero(&si, sizeof(si));
  si.si_signo = SIGCHLD;
  si.si_code = CLD_EXITED;
  si.si_pid = pids[i];
  ((sigaction_f)(_base + __sighandrvas[SIGCHLD]))(SIGCHLD, &si, 0);
  return true;
}
