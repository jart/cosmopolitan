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
#include "libc/thread/thread.h"

/**
 * Sets size of protected region at bottom of thread stack.
 *
 * Cosmopolitan sets this value to `sysconf(_SC_PAGESIZE)` by default.
 *
 * You may set `guardsize` to disable the stack guard feature and gain a
 * slight performance advantage by avoiding mprotect() calls. Note that
 * it could make your code more prone to silent unreported corruption.
 *
 * @param guardsize contains guard size in bytes, which is implicitly
 *     rounded up to `sysconf(_SC_PAGESIZE)`, or zero to disable
 * @return 0 on success, or errno on error
 */
errno_t pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize) {
  attr->__guardsize = guardsize;
  return 0;
}
