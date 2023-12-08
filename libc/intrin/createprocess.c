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
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/thunk/msabi.h"

__msabi extern typeof(CreateProcess) *const __imp_CreateProcessW;

/**
 * Creates process on the New Technology.
 *
 * @note this wrapper takes care of ABI, STRACE(), and __winerr()
 */
textwindows bool32
CreateProcess(const char16_t *opt_lpApplicationName, char16_t *lpCommandLine,
              const struct NtSecurityAttributes *opt_lpProcessAttributes,
              const struct NtSecurityAttributes *opt_lpThreadAttributes,
              bool32 bInheritHandles, uint32_t dwCreationFlags,
              void *opt_lpEnvironment, const char16_t *opt_lpCurrentDirectory,
              const struct NtStartupInfo *lpStartupInfo,
              struct NtProcessInformation *opt_out_lpProcessInformation) {
  bool32 ok;
  ok = __imp_CreateProcessW(opt_lpApplicationName, lpCommandLine,
                            opt_lpProcessAttributes, opt_lpThreadAttributes,
                            bInheritHandles, dwCreationFlags, opt_lpEnvironment,
                            opt_lpCurrentDirectory, lpStartupInfo,
                            opt_out_lpProcessInformation);
  if (!ok) __winerr();
  NTTRACE("CreateProcess(%#!hs, %#!hs, %s, %s, %hhhd, %u, %p, %#!hs, %p, %p) → "
          "%hhhd% m",
          opt_lpApplicationName, lpCommandLine,
          DescribeNtSecurityAttributes(opt_lpProcessAttributes),
          DescribeNtSecurityAttributes(opt_lpThreadAttributes), bInheritHandles,
          dwCreationFlags, opt_lpEnvironment, opt_lpCurrentDirectory,
          lpStartupInfo, opt_out_lpProcessInformation, ok);
  return ok;
}
