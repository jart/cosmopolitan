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
#include "libc/intrin/describeflags.h"
#include "libc/intrin/describentoverlapped.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/likely.h"
#include "libc/intrin/strace.h"
#include "libc/nt/runtime.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/nt/winsock.h"
#include "libc/runtime/runtime.h"

__msabi extern typeof(WSARecvFrom) *const __imp_WSARecvFrom;

/**
 * Receives data from Windows socket.
 * @return 0 on success, or -1 on failure
 */
textwindows int WSARecvFrom(
    uint64_t s, const struct NtIovec *inout_lpBuffers, uint32_t dwBufferCount,
    uint32_t *opt_out_lpNumberOfBytesRecvd, uint32_t *inout_lpFlags,
    void *opt_out_fromsockaddr, uint32_t *opt_inout_fromsockaddrlen,
    struct NtOverlapped *opt_inout_lpOverlapped,
    const NtWsaOverlappedCompletionRoutine opt_lpCompletionRoutine) {
  int rc;
#if SYSDEBUG && _NTTRACE
  uint32_t NumberOfBytesRecvd;
  if (opt_out_lpNumberOfBytesRecvd) {
    NumberOfBytesRecvd = *opt_out_lpNumberOfBytesRecvd;
  }
  rc = __imp_WSARecvFrom(s, inout_lpBuffers, dwBufferCount, &NumberOfBytesRecvd,
                         inout_lpFlags, opt_out_fromsockaddr,
                         opt_inout_fromsockaddrlen, opt_inout_lpOverlapped,
                         opt_lpCompletionRoutine);
  if (opt_out_lpNumberOfBytesRecvd) {
    *opt_out_lpNumberOfBytesRecvd = NumberOfBytesRecvd;
  }
  if (UNLIKELY(__strace > 0) && strace_enabled(0) > 0) {
    kprintf(STRACE_PROLOGUE "WSARecvFrom(%lu, [", s);
    DescribeIovNt(inout_lpBuffers, dwBufferCount,
                  rc != -1 ? NumberOfBytesRecvd : 0);
    kprintf("], %u, [%'u], %p, %p, %p, %s, %p) → %d %d\n", dwBufferCount,
            NumberOfBytesRecvd, opt_out_fromsockaddr, opt_inout_fromsockaddrlen,
            inout_lpFlags, DescribeNtOverlapped(opt_inout_lpOverlapped),
            opt_lpCompletionRoutine, rc, GetLastError());
  }
#else
  rc = __imp_WSARecvFrom(s, inout_lpBuffers, dwBufferCount,
                         opt_out_lpNumberOfBytesRecvd, inout_lpFlags,
                         opt_out_fromsockaddr, opt_inout_fromsockaddrlen,
                         opt_inout_lpOverlapped, opt_lpCompletionRoutine);
#endif
  return rc;
}
