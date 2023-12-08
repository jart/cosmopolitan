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
#include "libc/str/str.h"
#include "libc/thread/thread2.h"

/**
 * Gets signal mask on thread attributes object.
 *
 * @param attr is the thread attributes object
 * @param sigmask will receive the output signal mask on success, or
 *     null if a simple presence check is desired
 * @return 0 on success, errno on error, or `PTHREAD_ATTR_NO_SIGMASK_NP`
 *     if there wasn't any signal mask present in `attr`
 */
errno_t pthread_attr_getsigmask_np(const pthread_attr_t *attr,
                                   sigset_t *sigmask) {
  _Static_assert(sizeof(attr->__sigmask) == sizeof(*sigmask), "");
  if (!attr->__havesigmask) return PTHREAD_ATTR_NO_SIGMASK_NP;
  if (sigmask) *sigmask = attr->__sigmask;
  return 0;
}
