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
#include "libc/intrin/describentoverlapped.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/nt/files.h"
#include "libc/nt/struct/overlapped.h"
#include "libc/nt/thunk/msabi.h"

__msabi extern typeof(DeviceIoControl) *const __imp_DeviceIoControl;

/**
 * Does device file stuff on the New Technology.
 *
 * @return handle, or -1 on failure
 * @note this wrapper takes care of ABI, STRACE(), and __winerr()
 */
textwindows bool32 DeviceIoControl(int64_t hDevice, uint32_t dwIoControlCode,
                                   void *lpInBuffer, uint32_t nInBufferSize,
                                   void *lpOutBuffer, uint32_t nOutBufferSize,
                                   uint32_t *lpBytesReturned,
                                   struct NtOverlapped *lpOverlapped) {
  bool32 ok;
  ok = __imp_DeviceIoControl(hDevice, dwIoControlCode, lpInBuffer,
                             nInBufferSize, lpOutBuffer, nOutBufferSize,
                             lpBytesReturned, lpOverlapped);
  if (!ok) __winerr();
  NTTRACE("DeviceIoControl(%ld, %#x, %p, %'zu, %p, %'zu, %p, %s) → %hhhd% m",
          hDevice, dwIoControlCode, lpInBuffer, nInBufferSize, lpOutBuffer,
          nOutBufferSize, lpBytesReturned, DescribeNtOverlapped(lpOverlapped),
          ok);
  return ok;
}
