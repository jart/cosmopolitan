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
#include "libc/calls/sig.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/nt/enum/wait.h"
#include "libc/nt/errors.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/sysv/errfuns.h"

textwindows int __wsablock(int64_t handle, struct NtOverlapped *overlapped,
                           uint32_t *flags, bool restartable,
                           uint32_t timeout) {
  int rc;
  uint32_t i, got;
  if (WSAGetLastError() != kNtErrorIoPending) {
    NTTRACE("sock i/o failed %lm");
    return __winsockerr();
  }
  for (;;) {
    i = WSAWaitForMultipleEvents(1, &overlapped->hEvent, true,
                                 __SIG_POLLING_INTERVAL_MS, true);
    if (i == kNtWaitFailed) {
      NTTRACE("WSAWaitForMultipleEvents failed %lm");
      return __winsockerr();
    } else if (i == kNtWaitTimeout || i == kNtWaitIoCompletion) {
      if (_check_interrupts(restartable, g_fds.p)) {
        return -1;
      }
      if (timeout) {
        if (timeout <= __SIG_POLLING_INTERVAL_MS) {
          return eagain();
        }
        timeout -= __SIG_POLLING_INTERVAL_MS;
      }
    } else {
      break;
    }
  }
  if (!WSAGetOverlappedResult(handle, overlapped, &got, false, flags)) {
    NTTRACE("WSAGetOverlappedResult failed %lm");
    return __winsockerr();
  }
  return got;
}
