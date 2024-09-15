/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/sigtimedwait.h"

/**
 * Waits for signal synchronously.
 *
 * See sigtimedwait() for further details.
 *
 * @param set is signals for which we'll be waiting
 * @param out_sig shall receive signal number
 * @return 0 on success, or -1 w/ errno
 * @raise EINTR if an asynchronous signal was delivered instead
 * @raise ECANCELED if thread was cancelled in masked mode
 * @raise ENOSYS on OpenBSD, XNU, and Metal
 * @see sigtimedwait()
 * @cancelationpoint
 * @norestart
 */
int sigwait(const sigset_t *mask, int *out_sig) {
  int sig;
  if ((sig = sigtimedwait(mask, 0, 0)) == -1)
    return -1;
  if (out_sig)
    *out_sig = sig;
  return 0;
}
