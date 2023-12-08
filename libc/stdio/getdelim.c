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
#include "libc/stdio/stdio.h"

/**
 * Reads string from stream, e.g.
 *
 *     char *line = NULL;
 *     size_t linesize = 0;
 *     while (getdelim(&line, &linesize, '\n', stdin) > 0) {
 *       chomp(line);
 *       printf("%s\n", line);
 *     }
 *     free(line);
 *
 * @param s is the caller's buffer (in/out) which is extended or
 *     allocated automatically, also NUL-terminated is guaranteed
 * @param n is the capacity of s (in/out)
 * @param delim is the stop char (and NUL is implicitly too)
 * @return number of bytes read >0, including delim, excluding NUL,
 *     or -1 w/ errno on EOF or error; see ferror() and feof()
 * @note this function will ignore EINTR if it occurs mid-line
 * @raises EBADF if stream isn't open for reading
 * @see fgetln(), getline(), chomp(), gettok_r()
 */
ssize_t getdelim(char **s, size_t *n, int delim, FILE *f) {
  ssize_t rc;
  flockfile(f);
  rc = getdelim_unlocked(s, n, delim, f);
  funlockfile(f);
  return rc;
}
