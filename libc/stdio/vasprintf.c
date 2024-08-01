/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"

/**
 * Formats string w/ dynamic memory allocation.
 * @see xasprintf() for a better API
 */
int vasprintf(char **strp, const char *fmt, va_list va) {
  va_list vb;
  size_t size;
  char *p, *p2;
  int wrote, rc = -1;
  if ((p = malloc((size = 512)))) {
    va_copy(vb, va);
    wrote = vsnprintf(p, size, fmt, va);
    if (wrote < size) {
      if ((p2 = realloc(p, wrote + 1))) {
        p = p2;
        rc = wrote;
      }
    } else {
      size = wrote + 1;
      if ((p2 = realloc(p, size))) {
        p = p2;
        wrote = vsnprintf(p, size, fmt, vb);
        unassert(wrote == size - 1);
        rc = wrote;
      }
    }
    va_end(vb);
  }
  if (rc != -1) {
    *strp = p;
    return rc;
  } else {
    free(p);
    return -1;
  }
}
