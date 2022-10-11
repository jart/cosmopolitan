/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/errno.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

/**
 * Sets cancelability state.
 *
 * @param state may be one of:
 *     - `PTHREAD_CANCEL_ENABLE` (default)
 *     - `PTHREAD_CANCEL_DISABLE`
 * @param oldstate optionally receives old value
 * @return 0 on success, or errno on error
 * @raise EINVAL if `state` has bad value
 * @see pthread_cancel() for docs
 */
int pthread_setcancelstate(int state, int *oldstate) {
  struct PosixThread *pt;
  switch (state) {
    case PTHREAD_CANCEL_ENABLE:
    case PTHREAD_CANCEL_DISABLE:
      pt = (struct PosixThread *)__get_tls()->tib_pthread;
      if (oldstate) *oldstate = pt->canceldisable;
      pt->canceldisable = state;
      return 0;
    default:
      return EINVAL;
  }
}
