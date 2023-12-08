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
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/strace.internal.h"
#include "libc/str/str.h"

/**
 * Sends signal to process.
 *
 * The impact of this action can be terminating the process, or
 * interrupting it to request something happen.
 *
 * @param pid can be:
 *      >0 signals one process by id
 *      =0 signals all processes in current process group
 *      -1 signals all processes possible (except init)
 *     <-1 signals all processes in -pid process group
 * @param sig can be:
 *      >0 can be SIGINT, SIGTERM, SIGKILL, SIGUSR1, etc.
 *      =0 checks both if pid exists and we can signal it
 * @return 0 if something was accomplished, or -1 w/ errno
 * @raise ESRCH if `pid` couldn't be found
 * @raise EPERM if lacked permission to signal process
 * @raise EPERM if pledge() is in play without `proc` promised
 * @raise EINVAL if the provided `sig` is invalid or unsupported
 * @asyncsignalsafe
 */
int kill(int pid, int sig) {
  int rc;
  if (!IsWindows()) {
    rc = sys_kill(pid, sig, 1);
  } else {
    rc = sys_kill_nt(pid, sig);
  }
  STRACE("kill(%d, %G) → %d% m", pid, sig, rc);
  return rc;
}
