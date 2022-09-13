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
#include "libc/calls/calls.h"
#include "libc/intrin/strace.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/nt/enum/threadaccess.h"
#include "libc/nt/runtime.h"
#include "libc/nt/thread.h"
#include "libc/sysv/errfuns.h"

static textwindows int sys_tkill_nt(int tid, int sig) {
  int rc;
  int64_t hand;
  if ((hand = OpenThread(kNtThreadTerminate, false, tid))) {
    if (TerminateThread(hand, 128 + sig)) {
      rc = 0;
    } else {
      rc = __winerr();
    }
    CloseHandle(hand);
  } else {
    rc = esrch();
  }
  return rc;
}

/**
 * Kills thread.
 *
 * @param tid is thread id
 * @param sig does nothing on xnu
 * @return 0 on success, or -1 w/ errno
 */
int tkill(int tid, int sig) {
  int rc;
  if (!IsWindows()) {
    rc = sys_tkill(tid, sig, 0);
  } else {
    rc = sys_tkill_nt(tid, sig);
  }
  STRACE("tkill(%d, %G) → %d% m", tid, sig, rc);
  return rc;
}
