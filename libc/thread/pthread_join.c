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
 * Waits for thread to terminate.
 *
 * @param value_ptr if non-null will receive pthread_exit() argument
 *     if the thread called pthread_exit(), or `PTHREAD_CANCELED` if
 *     pthread_cancel() destroyed the thread instead
 * @return 0 on success, or errno with error
 * @raise EDEADLK if `thread` is the current thread
 * @raise EINVAL if `thread` is detached
 * @cancellationpoint
 * @returnserrno
 * @threadsafe
 */
int pthread_join(pthread_t thread, void **value_ptr) {
  struct PosixThread *pt;
  if (thread == __get_tls()->tib_pthread) {
    return EDEADLK;
  }
  if (!(pt = (struct PosixThread *)thread) ||  //
      pt->status == kPosixThreadZombie ||      //
      pt->status == kPosixThreadDetached) {
    return EINVAL;
  }
  _pthread_wait(pt);
  if (value_ptr) {
    *value_ptr = pt->rc;
  }
  _pthread_free(pt);
  return 0;
}
