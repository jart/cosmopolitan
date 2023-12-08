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
#include "libc/thread/thread2.h"

/**
 * Joins thread if it's already terminated.
 *
 * Multiple threads joining the same thread is undefined behavior. If a
 * deferred or masked cancelation happens to the calling thread either
 * before or during the waiting process then the target thread will not
 * be joined. Calling pthread_join() on a non-joinable thread, e.g. one
 * that's been detached, is undefined behavior. If a thread attempts to
 * join itself, then the behavior is undefined.
 *
 * @param value_ptr if non-null will receive pthread_exit() argument
 *     if the thread called pthread_exit(), or `PTHREAD_CANCELED` if
 *     pthread_cancel() destroyed the thread instead
 * @return 0 on success, or errno on error
 * @raise ECANCELED if calling thread was cancelled in masked mode
 * @cancelationpoint
 * @returnserrno
 */
errno_t pthread_tryjoin_np(pthread_t thread, void **value_ptr) {
  return pthread_timedjoin_np(thread, value_ptr, &timespec_zero);
}
