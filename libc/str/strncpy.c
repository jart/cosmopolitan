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
#include "libc/str/str.h"

/**
 * Prepares static search buffer.
 *
 * 1. If SRC is too long, it's truncated and *not* NUL-terminated.
 * 2. If SRC is too short, the remainder is zero-filled.
 *
 * Please note this function isn't designed to prevent untrustworthy
 * data from modifying memory without authorization; the memccpy()
 * function can be used for that purpose.
 *
 * Here's an example of the only use case we know of for strncpy:
 *
 *     static const struct People {
 *       char name[8];
 *       int age;
 *     } kPeople[] = {
 *         {"alice", 29},  //
 *         {"bob", 42},    //
 *     };
 *
 *     int GetAge(const char *name) {
 *       char k[8];
 *       int m, l, r;
 *       l = 0;
 *       r = ARRAYLEN(kPeople) - 1;
 *       strncpy(k, name, 8);
 *       while (l <= r) {
 *         m = (l + r) >> 1;
 *         if (READ64BE(kPeople[m].name) < READ64BE(k)) {
 *           l = m + 1;
 *         } else if (READ64BE(kPeople[m].name) > READ64BE(k)) {
 *           r = m - 1;
 *         } else {
 *           return kPeople[m].age;
 *         }
 *       }
 *       return -1;
 *     }
 *
 * @return dest
 * @see stpncpy(), memccpy()
 * @asyncsignalsafe
 * @vforksafe
 */
char *strncpy(char *dest, const char *src, size_t stride) {
  size_t i;
  for (i = 0; i < stride; ++i) {
    if (!(dest[i] = src[i])) break;
  }
  bzero(dest + i, stride - i);
  return dest;
}
