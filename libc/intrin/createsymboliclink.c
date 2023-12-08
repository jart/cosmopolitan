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
#include "libc/nt/files.h"

__msabi extern typeof(CreateSymbolicLink) *const __imp_CreateSymbolicLinkW;

/**
 * Creates symbolic link on the New Technology.
 * @note you need to elevate process privileges before calling this
 * @note this wrapper takes care of ABI, STRACE(), and __winerr()
 */
bool32 CreateSymbolicLink(const char16_t *lpSymlinkFileName,
                          const char16_t *lpTargetPathName, uint32_t dwFlags) {
  bool32 ok;
  ok = __imp_CreateSymbolicLinkW(lpSymlinkFileName, lpTargetPathName, dwFlags);
  if (!ok) __winerr();
  NTTRACE("CreateSymbolicLink(%#hs, %#hs, %s) → %hhhd% m", lpSymlinkFileName,
          lpTargetPathName, DescribeNtSymlinkFlags(dwFlags), ok);
  return ok;
}
