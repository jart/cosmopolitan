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
#include "libc/str/str.h"
#include "libc/str/tab.h"
#include "net/http/http.h"

/**
 * Converts HTTP method to word encoding.
 *
 * For example, `ParseHttpMethod("GET", -1)` will return `kHttpGet`.
 *
 * @param len if -1 implies strlen
 * @return word encoded method, or 0 if invalid
 */
uint64_t ParseHttpMethod(const char *str, size_t len) {
  int s = 0;
  uint64_t w = 0;
  if (len == -1)
    len = str ? strlen(str) : 0;
  for (size_t i = 0; i < len; ++i) {
    int c = kToUpper[str[i] & 255];
    if (!kHttpToken[c])
      return 0;
    if (s == 64)
      return 0;
    w |= (uint64_t)c << s;
    s += 8;
  }
  return w;
}
