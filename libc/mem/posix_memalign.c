/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"

/**
 * Allocates aligned memory, the POSIX way.
 *
 * Allocates a chunk of n bytes, aligned in accord with the alignment
 * argument. Differs from memalign() only in that it:
 *
 * 1. Assigns the allocated memory to *pp rather than returning it
 * 2. Fails and returns EINVAL if the alignment is not a power of two
 * 3. Fails and returns ENOMEM if memory cannot be allocated
 *
 * @param pp receives pointer, only on success
 * @param alignment must be 2-power multiple of sizeof(void *)
 * @param bytes is number of bytes to allocate
 * @return return 0 or EINVAL or ENOMEM w/o setting errno
 * @see memalign()
 * @returnserrno
 */
errno_t posix_memalign(void **pp, size_t alignment, size_t bytes) {
  int e;
  void *m;
  size_t q, r;
  q = alignment / sizeof(void *);
  r = alignment % sizeof(void *);
  if (!r && q && IS2POW(q)) {
    e = errno;
    m = memalign(alignment, bytes);
    if (m) {
      *pp = m;
      return 0;
    } else {
      errno = e;
      return ENOMEM;
    }
  } else {
    return EINVAL;
  }
}
