/*-*- mode:c; indent-tabs-mode:nil; tab-width:2; coding:utf-8               -*-│
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
#include "libc/sysv/consts/sa.h"

/**
 * Installs kernel interrupt handler, e.g.
 *
 *     void GotCtrlC(int sig) { ... }
 *     CHECK_NE(SIG_ERR, signal(SIGINT, GotCtrlC));
 *
 * @return old signal handler on success or SIG_ERR w/ errno
 * @note this function has BSD semantics, i.e. SA_RESTART
 * @see sigaction() which has more features and docs
 */
sighandler_t signal(int sig, sighandler_t func) {
  struct sigaction old, sa = {.sa_handler = func, .sa_flags = SA_RESTART};
  if (sigaction(sig, &sa, &old) != -1) {
    return old.sa_handler;
  } else {
    return SIG_ERR;
  }
}
