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
#include "libc/errno.h"
#include "libc/stdio/stdio.h"

/**
 * Reads UTF-8 content from stream into UTF-32 buffer.
 *
 * This function is similar to getline() except it'll truncate lines
 * exceeding size. The line ending marker is included and may be removed
 * using chomp().
 *
 * @param s is is nul-terminated string that's non-null
 * @param size is byte length of `s`
 * @param f is file stream object pointer
 * @see fgetws()
 */
wchar_t *fgetws_unlocked(wchar_t *s, int size, FILE *f) {
  wint_t c;
  wchar_t *p = s;
  if (size > 0) {
    while (--size > 0) {
      if ((c = fgetwc_unlocked(f)) == -1) {
        if (ferror_unlocked(f) == EINTR) continue;
        break;
      }
      *p++ = c;
      if (c == '\n') break;
    }
    *p = '\0';
  }
  return (intptr_t)p > (intptr_t)s ? s : NULL;
}
