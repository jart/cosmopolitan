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
 * @vforksafe
 */
int xsigaction(int sig, void *handler, uint64_t flags, uint64_t mask,
               struct sigaction *old) {
  /* This API is superior to sigaction() because (1) it offers feature
     parity; (2) compiler emits 1/3rd as much binary code at call-site;
     and (3) it removes typing that just whines without added safety. */
  struct sigaction sa;
  bzero(&sa, sizeof(sa));
  sa.sa_handler = handler;
  sa.sa_flags = flags;
  memcpy(&sa.sa_mask, &mask, sizeof(mask));
  return sigaction(sig, &sa, old);
}
