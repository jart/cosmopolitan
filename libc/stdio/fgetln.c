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
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"

/**
 * Retrieves line from stream, e.g.
 *
 *     char *line;
 *     while ((line = chomp(fgetln(stdin, 0)))) {
 *       printf("%s\n", line);
 *     }
 *
 * The returned memory is owned by the stream. It'll be reused when
 * fgetln() is called again. It's free()'d upon fclose() / fflush()
 *
 * When reading from the console on Windows in `ICANON` mode, the
 * returned line will end with `\r\n` rather than `\n`.
 *
 * @param stream specifies non-null open input stream
 * @param len optionally receives byte length of line
 * @return nul-terminated line string, including the `\n` character
 *     unless a line happened before EOF without `\n`, otherwise it
 *     returns `NULL` and feof() and ferror() can examine the state
 * @see getdelim()
 */
char *fgetln(FILE *stream, size_t *len) {
  char *res;
  ssize_t rc;
  size_t n = 0;
  flockfile(stream);
  if ((rc = getdelim_unlocked(&stream->getln, &n, '\n', stream)) > 0) {
    if (len) *len = rc;
    res = stream->getln;
  } else {
    res = 0;
  }
  funlockfile(stream);
  return res;
}
