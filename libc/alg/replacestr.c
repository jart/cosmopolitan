/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/alg/alg.h"
#include "libc/alg/arraylist2.internal.h"
#include "libc/bits/safemacros.internal.h"
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
