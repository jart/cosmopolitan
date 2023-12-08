/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/sysv/consts/ptrace.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sysv/errfuns.h"

/**
 * Traces process.
 *
 * This API is terrible. Consider using sys_ptrace().
 *
 * @param request can be PTRACE_xxx
 * @note de facto linux only atm
 * @vforksafe
 */
long ptrace(int request, ...) {
  // TODO(jart): FreeBSD addr and data args are different
  int pid;
  va_list va;
  bool ispeek;
  long rc, peek, addr, *data;
  va_start(va, request);
  pid = va_arg(va, int);
  addr = va_arg(va, long);
  data = va_arg(va, long *);
  va_end(va);
  if (request == -1) {
    rc = einval(); /* see consts.sh */
  } else {
    ispeek = IsLinux() && request - 1u < 3;
    if (ispeek) data = &peek;
    rc = __sys_ptrace(request, pid, addr, data);
    if (rc != -1 && ispeek) rc = peek;
  }
  STRACE("ptrace(%s, %d, %p, %p) → %p% m", DescribePtrace(request), pid, addr,
         data, rc);
  return rc;
}
