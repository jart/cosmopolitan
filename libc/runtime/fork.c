/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/intrin/strace.internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/nt/process.h"
#include "libc/runtime/internal.h"
#include "libc/sysv/consts/sig.h"
#include "libc/thread/tls.h"

/**
 * Creates new process.
 *
 * @return 0 to child, child pid to parent, or -1 on error
 * @asyncsignalsafe
 */
int fork(void) {
  axdx_t ad;
  sigset_t old, all;
  int ax, dx, parent;
  sigfillset(&all);
  sigprocmask(SIG_BLOCK, &all, &old);
  if (!IsWindows()) {
    ad = sys_fork();
    ax = ad.ax;
    dx = ad.dx;
    if (IsXnu() && ax != -1) {
      // eax always returned with childs pid
      // edx is 0 for parent and 1 for child
      ax &= dx - 1;
    }
  } else {
    ax = sys_fork_nt();
  }
  if (!ax) {
    if (!IsWindows()) {
      dx = sys_getpid().ax;
    } else {
      dx = GetCurrentProcessId();
    }
    parent = __pid;
    __pid = dx;
    if (__tls_enabled) {
      __get_tls()->tib_tid = IsLinux() ? dx : sys_gettid();
    }
    STRACE("fork() → 0 (child of %d)", parent);
    sigprocmask(SIG_SETMASK, &old, 0);
  } else {
    STRACE("fork() → %d% m", ax);
    sigprocmask(SIG_SETMASK, &old, 0);
  }
  return ax;
}
