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
#include "libc/calls/sig.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/strace.internal.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/errfuns.h"

/**
 * Sends signal to self.
 *
 * This is basically the same as:
 *
 *     pthread_kill(pthread_self(), sig);
 *
 * Note `SIG_DFL` still results in process death for most signals.
 *
 * POSIX defines raise() errors as returning non-zero and makes setting
 * `errno` optional. Every platform we've tested in our support vector
 * returns -1 with `errno` on error (like a normal system call).
 *
 * @param sig can be SIGALRM, SIGINT, SIGTERM, SIGKILL, etc.
 * @return 0 on success, or -1 w/ errno
 * @raise EINVAL if `sig` is invalid
 * @asyncsignalsafe
 */
int raise(int sig) {
  int rc;
  if (IsXnuSilicon()) {
    rc = _sysret(__syslib->__raise(sig));
  } else if (IsWindows()) {
    if (0 <= sig && sig <= 64) {
      __sig_raise(sig, SI_TKILL);
      rc = 0;
    } else {
      rc = einval();
    }
  } else {
    rc = sys_tkill(gettid(), sig, 0);
  }
  STRACE("raise(%G) → %d% m", sig, rc);
  return rc;
}
