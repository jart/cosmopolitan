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
#include "libc/calls/calls.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/likely.h"
#include "libc/intrin/strace.internal.h"
#include "libc/runtime/runtime.h"

#define IsPeek(request) (IsLinux() && (request)-1u < 3)

/**
 * Traces process.
 *
 * @param op can be PTRACE_xxx
 * @param pid is child process id
 * @param addr points inside child address space
 * @param data is address of output word when peeking
 * @note de facto linux only
 * @vforksafe
 */
int sys_ptrace(int op, ...) {
  va_list va;
  int rc, pid;
  long addr, *data;
  va_start(va, op);
  pid = va_arg(va, int);
  addr = va_arg(va, long);
  data = va_arg(va, long *);
  va_end(va);
  rc = __sys_ptrace(op, pid, addr, data);
#if SYSDEBUG
  if (UNLIKELY(__strace > 0) && strace_enabled(0) > 0) {
    if (rc != -1 && IsPeek(op) && data) {
      STRACE("sys_ptrace(%s, %d, %p, [%p]) → %p% m", DescribePtrace(op), pid,
             addr, *data, rc);
    } else {
      STRACE("sys_ptrace(%s, %d, %p, %p) → %p% m", DescribePtrace(op), pid,
             addr, data, rc);
    }
  }
#endif
  return rc;
}
