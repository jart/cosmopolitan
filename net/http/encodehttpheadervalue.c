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
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/str/thompike.h"
#include "net/http/escape.h"

/**
 * Encodes HTTP header value.
 *
 * This operation involves the following:
 *
 * 1. Trim whitespace.
 * 2. Turn UTF-8 into ISO-8859-1.
 * 3. Make sure no C0 or C1 control codes are present (except tab).
 *
 * If the input value isn't thompson-pike encoded then this
 * implementation will fall back to latin1 in most cases.
 *
 * @param data is input value
 * @param size if -1 implies strlen
 * @param out_size if non-NULL receives output length on success
 * @return allocated NUL-terminated string, or NULL w/ errno
 */
char *EncodeHttpHeaderValue(const char *data, size_t size, size_t *out_size) {
  bool t;
  wint_t x;
  size_t n;
  char *r, *q;
  const char *p, *e;
  if (size == -1) size = data ? strlen(data) : 0;
  if ((r = malloc(size + 1))) {
    t = 0;
    q = r;
    p = data;
    e = p + size;
    while (p < e) {
      x = *p++ & 0xff;
      if (x >= 0300) {
        if (p < e && ThomPikeCont(*p)) {
          if (ThomPikeLen(x) == 2) {
            x = ThomPikeMerge(ThomPikeByte(x), *p++);
          } else {
            x = 0;
          }
        }
      }
      if (!t) {
        if (x == ' ' || x == '\t') {
          continue;
        } else {
          t = true;
        }
      }
      if ((0x20 <= x && x <= 0x7E) || (0xA0 <= x && x <= 0xFF) || x == '\t') {
        *q++ = x;
      } else {
        free(r);
        errno = EILSEQ;
        return NULL;
      }
    }
    while (q > r && (q[-1] == ' ' || q[-1] == '\t')) --q;
    n = q - r;
    *q++ = '\0';
    if ((q = realloc(r, q - r))) r = q;
  } else {
    n = 0;
  }
  if (out_size) {
    *out_size = n;
  }
  return r;
}
