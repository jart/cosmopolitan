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
#include "libc/intrin/describentoverlapped.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/nt/enum/filelockflags.h"
#include "libc/nt/files.h"

__msabi extern typeof(LockFileEx) *const __imp_LockFileEx;

/**
 * Locks file on the New Technology.
 *
 * @return handle, or -1 on failure
 * @note this wrapper takes care of ABI, STRACE(), and __winerr()
 */
bool32 LockFileEx(int64_t hFile, uint32_t dwFlags, uint32_t dwReserved,
                  uint32_t nNumberOfBytesToLockLow,
                  uint32_t nNumberOfBytesToLockHigh,
                  struct NtOverlapped *lpOverlapped) {
  bool32 ok;
  if (~dwFlags & kNtLockfileFailImmediately) {
    NTTRACE("LockFileEx(%ld, %s, %#x, %'zu, %s) → ...", hFile,
            DescribeNtLockFileFlags(dwFlags), dwReserved,
            (uint64_t)nNumberOfBytesToLockHigh << 32 | nNumberOfBytesToLockLow,
            DescribeNtOverlapped(lpOverlapped));
  }
  ok = __imp_LockFileEx(hFile, dwFlags, dwReserved, nNumberOfBytesToLockLow,
                        nNumberOfBytesToLockHigh, lpOverlapped);
  if (!ok) __winerr();
  NTTRACE("LockFileEx(%ld, %s, %#x, %'zu, [%s]) → %hhhd% m", hFile,
          DescribeNtLockFileFlags(dwFlags), dwReserved,
          (uint64_t)nNumberOfBytesToLockHigh << 32 | nNumberOfBytesToLockLow,
          DescribeNtOverlapped(lpOverlapped), ok);
  return ok;
}
