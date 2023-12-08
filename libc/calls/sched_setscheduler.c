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
#include "libc/calls/sched-sysv.internal.h"
#include "libc/calls/struct/sched_param.h"
#include "libc/calls/struct/sched_param.internal.h"
#include "libc/dce.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/tls.h"

/**
 * Sets scheduling policy of process, e.g.
 *
 *     struct sched_param p = {sched_get_priority_max(SCHED_OTHER)};
 *     LOGIFNEG1(sched_setscheduler(0, SCHED_OTHER, &p));
 *
 * Processes with numerically higher priority values are scheduled
 * before processes with numerically lower priority values.
 *
 * @param pid is the id of the process whose scheduling policy should be
 *     changed. Setting `pid` to zero means the same thing as getpid().
 *     This applies to all threads associated with the process. Linux is
 *     special; the kernel treats this as a thread id (noting that
 *     `getpid() == gettid()` is always the case on Linux for the main
 *     thread) and will only take effect for the specified tid.
 *     Therefore this function is POSIX-compliant iif `!__threaded`.
 *
 * @param policy specifies the kernel's timesharing strategy.
 *
 *     The `policy` must have one of:
 *
 *     - `SCHED_OTHER` (or `SCHED_NORMAL`) for the default policy
 *
 *     - `SCHED_RR` for real-time round-robin scheduling
 *
 *     - `SCHED_FIFO` for real-time first-in first-out scheduling
 *
 *     - `SCHED_BATCH` for "batch" style execution of processes if
 *       supported (Linux), otherwise it's treated as `SCHED_OTHER`
 *
 *     - `SCHED_IDLE` for running very low priority background jobs if
 *       it's supported (Linux), otherwise this is `SCHED_OTHER`.
 *       Pledging away scheduling privilege is permanent for your
 *       process; if a subsequent attempt is made to restore the
 *       `SCHED_OTHER` policy then this system call will `EPERM` (but on
 *       older kernels like RHEL7 this isn't the case). This policy
 *       isn't available on old Linux kernels like RHEL5, where it'll
 *       raise `EINVAL`.
 *
 *     The `policy` may optionally bitwise-or any one of:
 *
 *     - `SCHED_RESET_ON_FORK` will cause the scheduling policy to be
 *       automatically reset to `SCHED_NORMAL` upon fork() if supported;
 *       otherwise this flag is polyfilled as zero, so that it may be
 *       safely used (without having to check if the o/s is Linux).
 *
 * @param param must be set to the scheduler parameter, which should be
 *     greater than or equal to sched_get_priority_min(policy) and less
 *     than or equal to sched_get_priority_max(policy). Linux allows the
 *     static priority range 1 to 99 for the `SCHED_FIFO` and `SCHED_RR`
 *     policies, and the priority 0 is used for the remaining policies.
 *     You should still consider calling the function, because on NetBSD
 *     the correct priority might be -1.
 *
 * @return the former scheduling policy of the specified process. If
 *     this function fails, then the scheduling policy is not changed,
 *     and -1 w/ errno is returned.
 *
 * @raise ENOSYS on XNU, Windows, OpenBSD
 * @raise EPERM if not authorized to use scheduler in question (e.g.
 *     trying to use a real-time scheduler as non-root on Linux) or
 *     possibly because pledge() was used and isn't allowing this
 * @raise EINVAL if `param` is NULL
 * @raise EINVAL if `policy` is invalid
 * @raise EINVAL if `param` has value out of ranges defined by `policy`
 * @vforksafe
 */
int sched_setscheduler(int pid, int policy, const struct sched_param *param) {
  int rc, old;
  struct sched_param p;

  if (IsNetbsd()) {
    rc = sys_sched_getscheduler_netbsd(pid, &p);
  } else {
    rc = sys_sched_getscheduler(pid);
  }

  if (rc != -1) {
    old = rc;
    if (IsNetbsd()) {
      rc = sys_sched_setparam_netbsd(pid, P_ALL_LWPS, policy, param);
    } else {
      rc = sys_sched_setscheduler(pid, policy, param);
    }
    if (rc != -1) {
      rc = old;
    }
  }

  STRACE("sched_setscheduler(%d, %s, %s) → %d% m", pid,
         DescribeSchedPolicy(policy), DescribeSchedParam(param), rc);
  return rc;
}
