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
#include "libc/calls/struct/cpuset.h"
#include "libc/errno.h"
#include "libc/thread/posixthread.internal.h"

/**
 * Gets CPU affinity for thread.
 *
 * While Windows allows us to change the thread affinity mask, it's only
 * possible to read the process affinity mask. Therefore this function
 * won't reflect the changes made by psched_setaffinity_np() on Windows.
 *
 * @param bitsetsize is byte length of bitset, which should be 128
 * @return 0 on success, or errno on error
 * @raise ENOSYS if not Linux or Windows
 */
errno_t pthread_getaffinity_np(pthread_t thread, size_t bitsetsize,
                               cpu_set_t *bitset) {
  int rc, e = errno;
  struct PosixThread *pt = (struct PosixThread *)thread;
  if (!sched_getaffinity(pt->tid, bitsetsize, bitset)) {
    return 0;
  } else {
    rc = errno;
    errno = e;
    return rc;
  }
}
