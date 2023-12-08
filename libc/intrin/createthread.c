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
#include "libc/nt/runtime.h"
#include "libc/nt/struct/securityattributes.h"
#include "libc/nt/thread.h"

__msabi extern typeof(CreateThread) *const __imp_CreateThread;
__msabi extern typeof(GetLastError) *const __imp_GetLastError;

/**
 * Opens file on the New Technology.
 *
 * @param dwStackSize may be 0 for default per executable
 * @param dwCreationFlags is a bitmask that may consist of:
 *     - kNtCreateSuspended
 *     - kNtStackSizeParamIsAReservation
 * @return thread handle, or 0 on failure
 * @note this wrapper takes care of ABI, STRACE()
 */
textwindows int64_t
CreateThread(const struct NtSecurityAttributes *lpThreadAttributes,
             size_t dwStackSize, void *lpStartAddress, void *lpParameter,
             uint32_t dwCreationFlags, uint32_t *opt_lpThreadId) {
  int64_t hHandle;
  hHandle = __imp_CreateThread(lpThreadAttributes, dwStackSize, lpStartAddress,
                               lpParameter, dwCreationFlags, opt_lpThreadId);
  NTTRACE("CreateThread(%s, %'zu, %t, %p, %s, %p) → {%ld, %d}",
          DescribeNtSecurityAttributes(lpThreadAttributes), dwStackSize,
          lpStartAddress, lpParameter,
          DescribeThreadCreateFlags(dwCreationFlags), opt_lpThreadId, hHandle,
          __imp_GetLastError());
  return hHandle;
}
