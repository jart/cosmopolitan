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
#include "libc/nt/memory.h"
#include "libc/nt/struct/securityattributes.h"
#include "libc/nt/thunk/msabi.h"

__msabi extern typeof(CreateFileMapping) *const __imp_CreateFileMappingW;

/**
 * Creates file mapping object on the New Technology.
 *
 * @param opt_hFile may be -1 for MAP_ANONYMOUS behavior
 * @return handle, or 0 on failure
 * @note this wrapper takes care of ABI, STRACE(), and __winerr()
 * @see MapViewOfFileEx()
 */
textwindows int64_t CreateFileMapping(
    int64_t opt_hFile,
    const struct NtSecurityAttributes *opt_lpFileMappingAttributes,
    uint32_t flProtect, uint32_t dwMaximumSizeHigh, uint32_t dwMaximumSizeLow,
    const char16_t *opt_lpName) {
  int64_t hHandle;
  hHandle = __imp_CreateFileMappingW(opt_hFile, opt_lpFileMappingAttributes,
                                     flProtect, dwMaximumSizeHigh,
                                     dwMaximumSizeLow, opt_lpName);
  if (!hHandle) __winerr();
  NTTRACE("CreateFileMapping(%ld, %s, %s, %'zu, %#hs) → %ld% m", opt_hFile,
          DescribeNtSecurityAttributes(opt_lpFileMappingAttributes),
          DescribeNtPageFlags(flProtect),
          (uint64_t)dwMaximumSizeHigh << 32 | dwMaximumSizeLow, opt_lpName,
          hHandle);
  return hHandle;
}
