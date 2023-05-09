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
#include "libc/intrin/strace.internal.h"
#include "libc/nt/enum/wait.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"

#ifdef __x86_64__

/**
 * Checks to see if SIGCHLD should be raised on Windows.
 * @return true if a signal was raised
 * @note yoinked by fork-nt.c
 */
void _check_sigchld(void) {
  siginfo_t si;
  int pids[64];
  uint32_t i, n;
  int64_t handles[64];
  __fds_lock();
  n = __sample_pids(pids, handles, true);
  __fds_unlock();
  if (!n) return;
  i = WaitForMultipleObjects(n, handles, false, 0);
  if (i == kNtWaitTimeout) return;
  if (i == kNtWaitFailed) {
    NTTRACE("%s failed %u", "WaitForMultipleObjects", GetLastError());
    return;
  }
  if (__sighandrvas[SIGCHLD] == (intptr_t)SIG_IGN ||
      __sighandrvas[SIGCHLD] == (intptr_t)SIG_DFL) {
    NTTRACE("new zombie fd=%d handle=%ld", pids[i], handles[i]);
    return;
  }
  if (__sighandflags[SIGCHLD] & SA_NOCLDWAIT) {
    NTTRACE("SIGCHILD SA_NOCLDWAIT fd=%d handle=%ld", pids[i], handles[i]);
    CloseHandle(handles[i]);
    __releasefd(pids[i]);
  }
  __fds_lock();
  g_fds.p[pids[i]].zombie = true;
  __fds_unlock();
  __sig_add(0, SIGCHLD, CLD_EXITED);
}

#endif /* __x86_64__ */
