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
#include "libc/calls/sig.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/nt/errors.h"
#include "libc/nt/ipc.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/securityattributes.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thunk/msabi.h"

__msabi extern typeof(CreateNamedPipe) *const __imp_CreateNamedPipeW;
__msabi extern typeof(GetLastError) *const __imp_GetLastError;
__msabi extern typeof(Sleep) *const __imp_Sleep;

/**
 * Creates pipe.
 *
 * @return handle to server end
 * @note this wrapper takes care of ABI, STRACE(), and __winerr()
 */
textwindows int64_t CreateNamedPipe(
    const char16_t *lpName, uint32_t dwOpenMode, uint32_t dwPipeMode,
    uint32_t nMaxInstances, uint32_t nOutBufferSize, uint32_t nInBufferSize,
    uint32_t nDefaultTimeOutMs,
    const struct NtSecurityAttributes *opt_lpSecurityAttributes) {
  int64_t hServer;
  uint32_t micros = 1;
TryAgain:
  hServer = __imp_CreateNamedPipeW(lpName, dwOpenMode, dwPipeMode,
                                   nMaxInstances, nOutBufferSize, nInBufferSize,
                                   nDefaultTimeOutMs, opt_lpSecurityAttributes);
  if (hServer == -1 && __imp_GetLastError() == kNtErrorPipeBusy) {
    if (micros >= 1024) __imp_Sleep(micros / 1024);
    if (micros < 1024 * 1024) micros <<= 1;
    goto TryAgain;
  }
  if (hServer == -1) __winerr();
  NTTRACE("CreateNamedPipe(%#hs, %s, %s, %u, %'u, %'u, %'u, %s) → %ld% m",
          lpName, DescribeNtPipeOpenFlags(dwOpenMode),
          DescribeNtPipeModeFlags(dwPipeMode), nMaxInstances, nOutBufferSize,
          nInBufferSize, nDefaultTimeOutMs,
          DescribeNtSecurityAttributes(opt_lpSecurityAttributes), hServer);
  return hServer;
}
