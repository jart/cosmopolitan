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
#include "libc/alg/alg.h"
#include "libc/alg/arraylist2.internal.h"
#include "libc/bits/safemacros.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

/**
 * Replaces all instances of NEEDLE in S with REPLACEMENT.
 *
 * @param needle can't be empty
 * @return newly allocated memory that must be free()'d or NULL w/ errno
 * @error ENOMEM, EINVAL
 */
char *(replacestr)(const char *s, const char *needle, const char *replacement) {
  char *p1, *p2, *res_p;
  size_t left, nlen, rlen, res_i, res_n;
  if (*needle) {
    p1 = s;
    left = strlen(s);
    nlen = strlen(needle);
    rlen = strlen(replacement);
    res_i = 0;
    res_n = max(left, 32);
    if ((res_p = malloc(res_n * sizeof(char)))) {
      do {
        if (!(p2 = memmem(p1, left, needle, nlen))) break;
        if (CONCAT(&res_p, &res_i, &res_n, p1, p2 - p1) == -1 ||
            CONCAT(&res_p, &res_i, &res_n, replacement, rlen) == -1) {
          goto oom;
        }
        p2 += nlen;
        left -= p2 - p1;
        p1 = p2;
      } while (left);
      if (CONCAT(&res_p, &res_i, &res_n, p1, left + 1) != -1) {
        return res_p;
      }
    }
  oom:
    free(res_p);
  } else {
    einval();
  }
  return NULL;
}
