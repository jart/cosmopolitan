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
#include "libc/calls/struct/sigaction.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sig.h"
#include "libc/x/x.h"

/**
 * Installs handler for kernel interrupt, e.g.:
 *
 *     onctrlc(sig) { exit(128+sig); }
 *     CHECK_NE(-1, xsigaction(SIGINT, onctrlc, SA_RESETHAND, 0, 0));
 *
 * @param sig can be SIGINT, SIGTERM, etc.
 * @param handler is SIG_DFL, SIG_IGN, or a pointer to a 0≤arity≤3
 *     callback function passed (sig, siginfo_t *, ucontext_t *).
 * @param flags can have SA_RESETHAND, SA_RESTART, SA_SIGINFO, etc.
 * @param mask is 1ul«SIG₁[…|1ul«SIGₙ] bitset to block in handler
 * @param old optionally receives previous handler
 * @return 0 on success, or -1 w/ errno
 * @see libc/sysv/consts.sh
 * @asyncsignalsafe
 */
int xsigaction(int sig, void *handler, uint64_t flags, uint64_t mask,
               struct sigaction *old) {
  /* This API is superior to sigaction() because (1) it offers feature
     parity; (2) compiler emits 1/3rd as much binary code at call-site;
     and (3) it removes typing that just whines without added saftey. */
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = handler;
  sa.sa_flags = flags;
  memcpy(&sa.sa_mask, &mask, sizeof(mask));
  return sigaction(sig, &sa, old);
}
