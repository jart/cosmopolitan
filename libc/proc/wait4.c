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
#include "libc/calls/cp.internal.h"
#include "libc/calls/struct/rusage.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/proc/proc.internal.h"
#include "libc/sysv/errfuns.h"

/**
 * Waits for status to change on process.
 *
 * @param pid >0 targets specific process, =0 means any proc in a group,
 *     -1 means any child process, <-1 means any proc in specific group
 * @param opt_out_wstatus optionally returns status code, and *wstatus
 *     may be inspected using WEEXITSTATUS(), etc.
 * @param options can have WNOHANG, WUNTRACED, WCONTINUED, etc.
 * @param opt_out_rusage optionally returns accounting data
 * @return process id of terminated child or -1 w/ errno
 * @cancelationpoint
 * @asyncsignalsafe
 * @restartable
 */
int wait4(int pid, int *opt_out_wstatus, int options,
          struct rusage *opt_out_rusage) {
  int rc, ws = 0;
  BEGIN_CANCELATION_POINT;

  if (IsAsan() &&
      ((opt_out_wstatus &&
        !__asan_is_valid(opt_out_wstatus, sizeof(*opt_out_wstatus))) ||
       (opt_out_rusage &&
        !__asan_is_valid(opt_out_rusage, sizeof(*opt_out_rusage))))) {
    rc = efault();
  } else if (!IsWindows()) {
    rc = sys_wait4(pid, &ws, options, opt_out_rusage);
  } else {
    rc = sys_wait4_nt(pid, &ws, options, opt_out_rusage);
  }
  if (rc != -1 && opt_out_wstatus) {
    *opt_out_wstatus = ws;
  }

  END_CANCELATION_POINT;
  STRACE("wait4(%d, [%#x], %d, %p) → %d% m", pid, ws, options, opt_out_rusage,
         rc);
  return rc;
}
