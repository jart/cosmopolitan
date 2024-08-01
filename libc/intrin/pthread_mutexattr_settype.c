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
#include "libc/errno.h"
#include "libc/thread/lock.h"
#include "libc/thread/thread.h"

/**
 * Sets mutex type.
 *
 * @param type can be one of
 *     - `PTHREAD_MUTEX_NORMAL`
 *     - `PTHREAD_MUTEX_DEFAULT`
 *     - `PTHREAD_MUTEX_RECURSIVE`
 *     - `PTHREAD_MUTEX_ERRORCHECK`
 * @return 0 on success, or error on failure
 * @raises EINVAL if `type` is invalid
 */
errno_t pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type) {
  switch (type) {
    case PTHREAD_MUTEX_NORMAL:
    case PTHREAD_MUTEX_RECURSIVE:
    case PTHREAD_MUTEX_ERRORCHECK:
      attr->_word = MUTEX_SET_TYPE(attr->_word, type);
      return 0;
    default:
      return EINVAL;
  }
}
