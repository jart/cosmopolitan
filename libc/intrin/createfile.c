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
#include "libc/nt/createfile.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/errors.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thunk/msabi.h"

__msabi extern typeof(CreateFile) *const __imp_CreateFileW;
__msabi extern typeof(GetLastError) *const __imp_GetLastError;
__msabi extern typeof(Sleep) *const __imp_Sleep;

/**
 * Opens file on the New Technology.
 *
 * @return handle, or -1 on failure w/ `errno` set appropriately
 * @note this wrapper takes care of ABI, STRACE(), and __winerr()
 */
textwindows int64_t
CreateFile(const char16_t *lpFileName,                         //
           uint32_t dwDesiredAccess,                           //
           uint32_t dwShareMode,                               //
           const struct NtSecurityAttributes *opt_lpSecurity,  //
           int dwCreationDisposition,                          //
           uint32_t dwFlagsAndAttributes,                      //
           int64_t opt_hTemplateFile) {
  int64_t hHandle;
  uint32_t micros = 1;
  char buf_accessflags[512];
  char buf_shareflags[64];
  char buf_secattr[32];
  char buf_flagattr[256];
TryAgain:
  hHandle = __imp_CreateFileW(lpFileName, dwDesiredAccess, dwShareMode,
                              opt_lpSecurity, dwCreationDisposition,
                              dwFlagsAndAttributes, opt_hTemplateFile);
  NTTRACE("CreateFile(%#hs, %s, %s, %s, %s, %s, %ld) → {%ld, %d}", lpFileName,
          (DescribeNtFileAccessFlags)(buf_accessflags, dwDesiredAccess),
          (DescribeNtFileShareFlags)(buf_shareflags, dwShareMode),
          (DescribeNtSecurityAttributes)(buf_secattr, opt_lpSecurity),
          DescribeNtCreationDisposition(dwCreationDisposition),
          (DescribeNtFileFlagAttr)(buf_flagattr, dwFlagsAndAttributes),
          opt_hTemplateFile, hHandle, __imp_GetLastError());
  if (hHandle == -1) {
    switch (__imp_GetLastError()) {
      case kNtErrorPipeBusy:
        if (micros >= 1024) __imp_Sleep(micros / 1024);
        if (micros < 1024 * 1024) micros <<= 1;
        goto TryAgain;
      case kNtErrorAccessDenied:
        // GetNtOpenFlags() always greedily requests execute permissions
        // because the POSIX flag O_EXEC doesn't mean the same thing. It
        // seems however this causes the opening of certain files to not
        // work, possibly due to Windows Defender or some security thing
        // In that case, we'll cross our fingers the file isn't a binary
        if ((dwDesiredAccess & kNtGenericExecute) &&
            (dwCreationDisposition == kNtOpenExisting ||
             dwCreationDisposition == kNtTruncateExisting)) {
          NTTRACE("CreateFile removed kNtGenericExecute");
          dwDesiredAccess &= ~kNtGenericExecute;
          goto TryAgain;
        }
        break;
      default:
        break;
    }
    __winerr();
  }
  return hHandle;
}
