/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/dce.h"
#include "libc/sysv/errfuns.h"

/**
 * Changes program signal blocking state, e.g.:
 *
 *   sigset_t oldmask;
 *   sigprocmask(SIG_BLOCK, &kSigsetFull, &oldmask);
 *   sigprocmask(SIG_SETMASK, &oldmask, NULL);
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
