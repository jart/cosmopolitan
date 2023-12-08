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
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/dce.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"

/**
 * Changes signal blocking state of calling thread, e.g.:
 *
 *     sigset_t neu,old;
 *     sigfillset(&neu);
 *     sigprocmask(SIG_BLOCK, &neu, &old);
 *     sigprocmask(SIG_SETMASK, &old, NULL);
 *
 * @param how can be SIG_BLOCK (U), SIG_UNBLOCK (/), SIG_SETMASK (=)
 * @param set is the new mask content (optional)
 * @param oldset will receive the old mask (optional) and can't overlap
 * @return 0 on success, or -1 w/ errno
 * @raise EFAULT if `set` or `oldset` is bad memory
 * @raise EINVAL if `how` is invalid
 * @asyncsignalsafe
 * @restartable
 * @vforksafe
 */
int sigprocmask(int how, const sigset_t *opt_set, sigset_t *opt_out_oldset) {
  int rc;
  sigset_t old = {0};
  if (IsAsan() &&
      ((opt_set && !__asan_is_valid(opt_set, sizeof(*opt_set))) ||
       (opt_out_oldset &&
        !__asan_is_valid(opt_out_oldset, sizeof(*opt_out_oldset))))) {
    rc = efault();
  } else if (IsMetal() || IsWindows()) {
    rc = __sig_mask(how, opt_set, &old);
  } else {
    rc = sys_sigprocmask(how, opt_set, opt_out_oldset ? &old : 0);
  }
  if (rc != -1 && opt_out_oldset) {
    *opt_out_oldset = old;
  }
  STRACE("sigprocmask(%s, %s, [%s]) → %d% m", DescribeHow(how),
         DescribeSigset(0, opt_set), DescribeSigset(rc, opt_out_oldset), rc);
  return rc;
}
