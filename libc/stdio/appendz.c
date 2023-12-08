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
#include "libc/assert.h"
#include "libc/dce.h"
#include "libc/mem/mem.h"
#include "libc/stdio/append.h"

#define W sizeof(size_t)

/**
 * Returns size of append buffer.
 *
 * @param p must be 0 or have been allocated by an append*() function
 * @return i is number of bytes stored in buffer
 * @return n is number of bytes in allocation
 */
struct appendz appendz(char *p) {
  struct appendz z;
  if (p) {
    z.n = malloc_usable_size(p);
    unassert(z.n >= W * 2 && !(z.n & (W - 1)));
    z.i = *(size_t *)(p + z.n - W);
    if (!IsTiny() && W == 8) {
      // This check should fail if an append*() function was passed a
      // pointer that was allocated manually by malloc(). Append ptrs
      // can be free()'d safely, but they need to be allocated by the
      // append library, because we write a special value to the end.
      unassert((z.i >> 48) == APPEND_COOKIE);
      z.i &= 0x0000ffffffffffff;
    }
    unassert(z.n >= z.i);
  } else {
    z.i = 0;
    z.n = 0;
  }
  return z;
}
