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
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/x/x.h"
#include "net/http/escape.h"
#include "net/http/url.h"

/**
 * Escapes URL component using generic table.
 *
 * This function is agnostic to the underlying charset.
 * Always using UTF-8 is a good idea.
 *
 * @param p is input value
 * @param n if -1 implies strlen
 * @param z if non-NULL receives output length
 * @return allocated NUL-terminated buffer, or NULL w/ errno
 * @see kEscapeAuthority
 * @see kEscapeIpLiteral
 * @see kEscapePath
 * @see kEscapePathSegment
 * @see kEscapeParam
 * @see kEscapeFragment
 */
char *EscapeUrl(const char *p, size_t n, size_t *z, const char T[256]) {
  char *r, *q;
  struct UrlView v;
  if (n == -1) n = p ? strlen(p) : 0;
  if (z) *z = 0;
  if ((q = r = malloc(n * 6 + 1))) {
    v.p = (char *)p, v.n = n;
    q = EscapeUrlView(r, &v, T);
    if (z) *z = q - r;
    *q++ = '\0';
    if ((q = realloc(r, q - r))) r = q;
  }
  return r;
}
