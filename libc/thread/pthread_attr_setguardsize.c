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
#include "libc/thread/thread.h"

/**
 * Sets size of unmapped pages at bottom of stack.
 *
 * This value will be rounded up to the host microprocessor page size,
 * which is usually 4096 or 16384. It's important to write code in such
 * a way that that code can't skip over the guard area. GCC has warnings
 * like `-Wframe-larger-than=4096 -Walloca-larger-than=4096` which help
 * guarantee your code is safe in this regard. It should be assumed the
 * guard pages exist beneath the stack pointer, rather than the bottom
 * of the stack, since guard pages are also used to grow down commit,
 * which can be poked using CheckLargeStackAllocation().
 *
 * @param guardsize contains guard size in bytes
 * @return 0 on success, or errno on error
 * @raise EINVAL if `guardsize` is zero
 */
errno_t pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize) {
  if (!guardsize) return EINVAL;
  attr->__guardsize = guardsize;
  return 0;
}
