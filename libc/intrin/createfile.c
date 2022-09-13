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
#include "libc/intrin/strace.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/nt/createfile.h"
#include "libc/nt/thunk/msabi.h"

__msabi extern typeof(CreateFile) *const __imp_CreateFileW;

/**
 * Opens file on the New Technology.
 *
 * @return handle, or -1 on failure
 * @note this wrapper takes care of ABI, STRACE(), and __winerr()
 */
textwindows int64_t CreateFile(
    const char16_t *lpFileName, uint32_t dwDesiredAccess, uint32_t dwShareMode,
    struct NtSecurityAttributes *opt_lpSecurityAttributes,
    int dwCreationDisposition, uint32_t dwFlagsAndAttributes,
    int64_t opt_hTemplateFile) {
  int64_t hHandle;
  hHandle = __imp_CreateFileW(lpFileName, dwDesiredAccess, dwShareMode,
                              opt_lpSecurityAttributes, dwCreationDisposition,
                              dwFlagsAndAttributes, opt_hTemplateFile);
  if (hHandle == -1) __winerr();
  NTTRACE("CreateFile(%#hs, %s, %s, %s, %s, %s, %ld) → %ld% m", lpFileName,
          DescribeNtFileAccessFlags(dwDesiredAccess),
          DescribeNtFileShareFlags(dwShareMode),
          DescribeNtSecurityAttributes(opt_lpSecurityAttributes),
          DescribeNtCreationDisposition(dwCreationDisposition),
          DescribeNtFileFlagAttr(dwFlagsAndAttributes), opt_hTemplateFile,
          hHandle);
  return hHandle;
}
