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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/mem/alloca.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

static const char *DescribeCancelType(char buf[12], int err, int *t) {
  if (err) return "n/a";
  if (!t) return "NULL";
  if (*t == PTHREAD_CANCEL_DEFERRED) return "PTHREAD_CANCEL_DEFERRED";
  if (*t == PTHREAD_CANCEL_ASYNCHRONOUS) return "PTHREAD_CANCEL_ASYNCHRONOUS";
  FormatInt32(buf, *t);
  return buf;
}

/**
 * Sets cancelation strategy.
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
  int err;
  struct PosixThread *pt;
  switch (type) {
    case PTHREAD_CANCEL_ASYNCHRONOUS:
    case PTHREAD_CANCEL_DEFERRED:
      pt = _pthread_self();
      if (oldtype) {
        if (pt->pt_flags & PT_ASYNC) {
          *oldtype = PTHREAD_CANCEL_ASYNCHRONOUS;
        } else {
          *oldtype = PTHREAD_CANCEL_DEFERRED;
        }
      }
      if (type == PTHREAD_CANCEL_DEFERRED) {
        pt->pt_flags &= ~PT_ASYNC;
      } else {
        pt->pt_flags |= PT_ASYNC;
      }
      err = 0;
      break;
    default:
      err = EINVAL;
      break;
  }
  STRACE("pthread_setcanceltype(%s, [%s]) → %s",
         DescribeCancelType(alloca(12), 0, &type),
         DescribeCancelType(alloca(12), err, oldtype), DescribeErrno(err));
  return err;
}
