/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/stdio/syscall.h"
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/stdio/rand.h"

/**
 * Translation layer for some Linux system calls:
 *
 * - `SYS_gettid`
 * - `SYS_getrandom`
 *
 * @return system call result, or -1 w/ errno
 */
long syscall(long number, ...) {
  switch (number) {
    default:
      errno = ENOSYS;
      return -1;
    case SYS_gettid:
      return gettid();
    case SYS_getrandom: {
      va_list va;
      va_start(va, number);
      void *buf = va_arg(va, void *);
      size_t buflen = va_arg(va, size_t);
      unsigned flags = va_arg(va, unsigned);
      va_end(va);
      ssize_t rc = getrandom(buf, buflen, flags);
      return rc;
    }
  }
}
