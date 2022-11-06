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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

/**
 * Sets cancellation strategy.
 *
 * @param type may be one of:
 *     - `PTHREAD_CANCEL_DEFERRED` (default)
 *     - `PTHREAD_CANCEL_ASYNCHRONOUS`
 * @param oldtype optionally receives old value
 * @return 0 on success, or errno on error
 * @raise ENOTSUP on Windows if asynchronous
 * @raise EINVAL if `type` has bad value
 * @see pthread_cancel() for docs
 */
errno_t pthread_setcanceltype(int type, int *oldtype) {
  struct PosixThread *pt;
  switch (type) {
    case PTHREAD_CANCEL_ASYNCHRONOUS:
      if (IsWindows()) {
        return ENOTSUP;
      }
      // fallthrough
    case PTHREAD_CANCEL_DEFERRED:
      pt = (struct PosixThread *)__get_tls()->tib_pthread;
      if (oldtype) {
        if (pt->flags & PT_ASYNC) {
          *oldtype = PTHREAD_CANCEL_ASYNCHRONOUS;
        } else {
          *oldtype = PTHREAD_CANCEL_DEFERRED;
        }
      }
      if (type == PTHREAD_CANCEL_DEFERRED) {
        pt->flags &= ~PT_ASYNC;
      } else {
        pt->flags |= PT_ASYNC;
      }
      return 0;
    default:
      return EINVAL;
  }
}
