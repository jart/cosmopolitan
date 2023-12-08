/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/state.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/runtime/internal.h"

/**
 * Returns process id.
 *
 * This function does not need to issue a system call. The PID is
 * tracked by a global variable which is updated at fork(). The only
 * exception is when the process is vfork()'d in which case a system
 * call shall be issued. This optimization helps make functions like
 * _rand64() fork-safe, however it could lead to race conditions in
 * programs that mix fork() with threads. In that case, apps should
 * consider using `sys_getpid().ax` instead to force a system call.
 *
 * On Linux, and only Linux, getpid() is guaranteed to equal gettid()
 * for the main thread.
 *
 * @return process id (always successful)
 * @asyncsignalsafe
 * @vforksafe
 */
int getpid(void) {
  int rc;
  if (IsMetal()) {
    rc = 1;
  } else if (!__vforked) {
    rc = __pid;
  } else {
    rc = sys_getpid().ax;
  }
  return rc;
}
