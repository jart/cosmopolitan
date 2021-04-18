/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/str/str.h"
#include "libc/x/x.h"
#include "net/http/http.h"

/**
 * Inserts spaces before lines.
 *
 * @param data is input value
 * @param size if -1 implies strlen
 * @param out_size if non-NULL receives output length
 * @param amt is number of spaces to use
 * @return allocated NUL-terminated buffer, or NULL w/ errno
 */
char *IndentLines(const char *data, size_t size, size_t *out_size, size_t amt) {
  char *r;
  const char *p;
  size_t i, n, m, a;
  if (size == -1) size = data ? strlen(data) : 0;
  r = 0;
  n = 0;
  do {
    if ((p = memchr(data, '\n', size))) {
      m = p + 1 - data;
      a = *data != '\r' && *data != '\n' ? amt : 0;
    } else {
      m = size;
      a = size ? amt : 0;
    }
    r = xrealloc(r, n + a + m + 1);
    memset(r + n, ' ', a);
    memcpy(r + n + a, data, m);
    n += a + m;
    data += m;
    size -= m;
  } while (p);
  if (out_size) {
    *out_size = n;
  }
  r[n] = '\0';
  return r;
}
