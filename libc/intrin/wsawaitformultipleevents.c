/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/nt/winsock.h"

__msabi extern typeof(WSAWaitForMultipleEvents)
    *const __imp_WSAWaitForMultipleEvents;

/**
 * Waits for events on Windows sockets.
 *
 * @return 0 on success, or -1 on failure
 * @note this wrapper takes care of ABI, STRACE(), and __winerr()
 */
textwindows uint32_t WSAWaitForMultipleEvents(uint32_t cEvents,
                                              const int64_t *lphEvents,
                                              bool32 fWaitAll,
                                              uint32_t dwTimeout_ms,
                                              bool32 fAlertable) {
  uint32_t rc;
  rc = __imp_WSAWaitForMultipleEvents(cEvents, lphEvents, fWaitAll,
                                      dwTimeout_ms, fAlertable);
  POLLTRACE("WSAWaitForMultipleEvents(%u, %p, %hhhd, %'u, %hhhd) → %u% lm",
            cEvents, lphEvents, fWaitAll, dwTimeout_ms, fAlertable, rc);
  return rc;
}
