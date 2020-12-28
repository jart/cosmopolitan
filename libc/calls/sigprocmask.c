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
#include "libc/calls/internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/dce.h"
#include "libc/sysv/errfuns.h"

/**
 * Changes program signal blocking state, e.g.:
 *
 *     sigset_t oldmask;
 *     sigprocmask(SIG_BLOCK, &kSigsetFull, &oldmask);
 *     sigprocmask(SIG_SETMASK, &oldmask, NULL);
 *
 * @param how can be SIG_BLOCK (U), SIG_UNBLOCK (/), SIG_SETMASK (=)
 * @param set is the new mask content (optional)
 * @param oldset will receive the old mask (optional) and can't overlap
 * @return 0 on success, or -1 w/ errno
 * @asyncsignalsafe
 */
int sigprocmask(int how, const sigset_t *opt_set, sigset_t *opt_out_oldset) {
  if (!IsWindows()) {
    return sigprocmask$sysv(how, opt_set, opt_out_oldset, 8);
  } else {
    return enosys(); /* TODO(jart): Implement me! */
  }
}
