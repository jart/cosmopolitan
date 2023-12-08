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
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"

/**
 * Reads string from unlocked stream.
 * @see getdelim() for documentation
 */
ssize_t getdelim_unlocked(char **s, size_t *n, int delim, FILE *f) {
  ssize_t rc;
  char *p, *s2;
  size_t i, m, n2;
  if ((f->iomode & O_ACCMODE) == O_WRONLY) {
    f->state = errno = EBADF;
    return -1;
  }
  if (f->beg > f->end) {
    f->state = errno = EINVAL;
    return -1;
  }
  if (!*s) *n = 0;
  for (i = 0;; i += m) {
    m = f->end - f->beg;
    if ((p = memchr(f->buf + f->beg, delim, m))) {
      m = p + 1 - (f->buf + f->beg);
    }
    if (i + m + 1 > *n) {
      n2 = i + m + 1;
      s2 = realloc(*s, n2);
      if (s2) {
        *s = s2;
        *n = n2;
      } else {
        f->state = errno;
        return -1;
      }
    }
    memcpy(*s + i, f->buf + f->beg, m);
    (*s)[i + m] = '\0';
    if ((f->beg += m) == f->end) {
      f->beg = f->end = 0;
    }
    if (p) {
      return i + m;
    } else if (f->fd == -1) {
      break;
    } else if ((rc = read(f->fd, f->buf, f->size)) != -1) {
      if (!rc) break;
      f->end = rc;
    } else if (errno != EINTR) {
      f->state = errno;
      return -1;
    }
  }
  f->state = -1;
  if (i + m) {
    return i + m;
  } else {
    return -1;
  }
}
