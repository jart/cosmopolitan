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
#include "libc/assert.h"
#include "libc/fmt/fmt.h"
#include "libc/mem/mem.h"

/**
 * Formats string w/ dynamic memory allocation.
 *
 * @param *strp is output-only and must be free'd, even on error; since
 *     that's the behavior that'll make your code most portable
 * @return complete bytes written (excluding NUL) or -1 w/ errno
 * @see xasprintf() for a better API
 */
int(vasprintf)(char **strp, const char *fmt, va_list va) {
  char *p;
  size_t size;
  va_list vb;
  int wrote, rc = -1;
  if ((*strp = malloc((size = 512)))) {
    va_copy(vb, va);
    wrote = (vsnprintf)(*strp, size, fmt, va);
    if (wrote < size) {
      if ((p = realloc(*strp, wrote + 1))) *strp = p;
      rc = wrote;
    } else {
      size = wrote + 1;
      if ((p = realloc(*strp, size))) {
        *strp = p;
        wrote = (vsnprintf)(*strp, size, fmt, vb);
        assert(wrote == size - 1);
        rc = wrote;
      }
    }
    va_end(vb);
  }
  return rc;
}
