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
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"

/**
 * Reads string from stream.
 *
 * @param s is the caller's buffer (in/out) which is extended or
 *     allocated automatically, also NUL-terminated is guaranteed
 * @param n is the capacity of s (in/out)
 * @param delim is the stop char (and NUL is implicitly too)
 * @return number of bytes read >0, including delim, excluding NUL,
 *     or -1 w/ errno on EOF or error; see ferror() and feof()
 * @note this function can't punt EINTR to caller
 * @see getline(), chomp(), gettok_r()
 */
ssize_t getdelim(char **s, size_t *n, int delim, FILE *f) {
  char *p;
  ssize_t rc;
  size_t i, m;
  if ((f->iomode & O_ACCMODE) == O_WRONLY) {
    f->state = errno = EBADF;
    return -1;
  }
  if (f->beg > f->end || f->bufmode == _IONBF) {
    f->state = errno = EINVAL;
    return -1;
  }
  if (!*s) *n = 0;
  for (i = 0;; i += m) {
    m = f->end - f->beg;
    if ((p = memchr(f->buf + f->beg, delim, m))) m = p + 1 - (f->buf + f->beg);
    if (i + m + 1 > *n && !(*s = realloc(*s, (*n = i + m + 1)))) abort();
    memcpy(*s + i, f->buf + f->beg, m);
    (*s)[i + m] = '\0';
    if ((f->beg += m) == f->end) f->beg = f->end = 0;
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
