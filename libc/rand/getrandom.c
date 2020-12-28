/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/bits/safemacros.h"
#include "libc/calls/internal.h"
#include "libc/errno.h"
#include "libc/rand/rand.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"

/**
 * Returns random bytes appropriate for random seeding.
 *
 * @param size should be the smallest value that meets your requirements
 * @param flags may be GRND_{RANDOM,NONBLOCK}
 * @return number of bytes copied on success; or -1 w/ errno, which
 *     indicates only that the request couldn't be serviced by the host
 *     kernel; this wrapper will still fill the buffer with random bytes
 *     from fallback sources no matter what
 */
ssize_t getrandom(void *buf, size_t size, unsigned flags) {
  ssize_t rc = getrandom$sysv(buf, size, flags);
  size_t i = rc == -1 ? 0 : (size_t)rc;
  if (i > size) abort();
  if (i < size) {
    unsigned char *p = buf;
    int olderr = errno;
    do {
      uint64_t i64 = rand64();
      memcpy(&p[i], &i64, min(sizeof(i64), size - i));
    } while ((i += sizeof(uint64_t)) < size);
    errno = olderr;
  }
  return rc;
}
