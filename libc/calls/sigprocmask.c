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
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/strace.internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/dce.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/log/log.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"

static const char *DescribeHow(char buf[12], int how) {
  if (how == SIG_BLOCK) return "SIG_BLOCK";
  if (how == SIG_UNBLOCK) return "SIG_UNBLOCK";
  if (how == SIG_SETMASK) return "SIG_SETMASK";
  FormatInt32(buf, how);
  return buf;
}

/**
 * Changes program signal blocking state, e.g.:
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
 * @asyncsignalsafe
 * @vforksafe
 */
int sigprocmask(int how, const sigset_t *opt_set, sigset_t *opt_out_oldset) {
  int x, rc;
  char howbuf[12];
  char buf[2][41];
  sigset_t old, *oldp;
  const sigset_t *arg;
  if (!(IsAsan() &&
        ((opt_set && !__asan_is_valid(opt_set, sizeof(*opt_set))) ||
         (opt_out_oldset &&
          !__asan_is_valid(opt_out_oldset, sizeof(*opt_out_oldset)))))) {
    if (!IsWindows() && !IsOpenbsd()) {
      if (opt_out_oldset) {
        bzero(&old, sizeof(old));
        oldp = &old;
      } else {
        oldp = 0;
      }
      if (sys_sigprocmask(how, opt_set, oldp, 8) != -1) {
        if (opt_out_oldset) {
          memcpy(opt_out_oldset, &old, sizeof(old));
        }
        rc = 0;
      } else {
        rc = -1;
      }
    } else if (IsOpenbsd()) {
      if (opt_set) {
        /* openbsd only supports 32 signals so it passses them in a reg */
        arg = (sigset_t *)(uintptr_t)(*(uint32_t *)opt_set);
      } else {
        how = 1; /* SIG_BLOCK */
        arg = 0; /* changes nothing */
      }
      if ((x = sys_sigprocmask(how, arg, 0, 0)) != -1) {
        if (opt_out_oldset) {
          bzero(opt_out_oldset, sizeof(*opt_out_oldset));
          memcpy(opt_out_oldset, &x, sizeof(x));
        }
        rc = 0;
      } else {
        rc = -1;
      }
    } else {
      if (opt_out_oldset) bzero(opt_out_oldset, sizeof(*opt_out_oldset));
      rc = 0; /* TODO(jart): Implement me! */
    }
  } else {
    rc = efault();
  }
  STRACE("sigprocmask(%s, %s, [%s]) → %d% m", DescribeHow(howbuf, how),
         __strace_sigset(buf[0], sizeof(buf[0]), 0, opt_set),
         __strace_sigset(buf[1], sizeof(buf[1]), rc, opt_out_oldset), rc);
  return rc;
}
