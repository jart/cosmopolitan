/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/nt/struct/overlapped.h"
#include "libc/nt/thread.h"
#include "libc/nt/thunk/msabi.h"

__msabi extern typeof(CancelIoEx) *const __imp_CancelIoEx;

/**
 * Cancels Windows i/o operation.
 */
bool32 CancelIoEx(int64_t hFile, struct NtOverlapped *opt_lpOverlapped) {
  bool32 ok;
  ok = __imp_CancelIoEx(hFile, opt_lpOverlapped);
  if (!ok) __winerr();
  NTTRACE("CancelIoEx(%ld, %s) → %hhhd% m", hFile,
          DescribeNtOverlapped(opt_lpOverlapped), ok);
  return ok;
}
