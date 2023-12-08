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
#include "libc/thread/thread.h"

/**
 * Sets contention scope attribute.
 *
 * @param contentionscope may be one of:
 *     - `PTHREAD_SCOPE_SYSTEM` to fight the system for resources
 *     - `PTHREAD_SCOPE_PROCESS` to fight familiar threads for resources
 * @return 0 on success, or errno on error
 * @raise ENOTSUP if `contentionscope` isn't supported on host OS
 * @raise EINVAL if `contentionscope` was invalid
 */
errno_t pthread_attr_setscope(pthread_attr_t *attr, int contentionscope) {
  switch (contentionscope) {
    case PTHREAD_SCOPE_SYSTEM:
      attr->__contentionscope = contentionscope;
      return 0;
    case PTHREAD_SCOPE_PROCESS:
      // Linux almost certainly doesn't support thread scoping
      // FreeBSD has THR_SYSTEM_SCOPE but it's not implemented
      // OpenBSD pthreads claims support but really ignores it
      // NetBSD pthreads claims support, but really ignores it
      // XNU sources appear to make no mention of thread scope
      // WIN32 documentation says nothing about thread scoping
      return ENOTSUP;
    default:
      return EINVAL;
  }
}
