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
#include "libc/intrin/pcmpgtb.h"
#include "libc/intrin/pmovmskb.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "net/http/http.h"

/**
 * Decodes ISO-8859-1 to UTF-8.
 *
 * @param data is input value
 * @param size if -1 implies strlen
 * @param out_size if non-NULL receives output length
 * @return allocated NUL-terminated buffer, or NULL w/ errno
 */
char *DecodeLatin1(const char *data, size_t size, size_t *out_size) {
  int c;
  size_t n;
  char *r, *q;
  const char *p, *e;
  if (size == -1) size = data ? strlen(data) : 0;
  if ((r = malloc(size * 2 + 1))) {
    q = r;
    p = data;
    e = p + size;
    while (p < e) {
      c = *p++ & 0xff;
      if (c < 0200) {
        *q++ = c;
      } else {
        *q++ = 0300 | c >> 6;
        *q++ = 0200 | c & 077;
      }
    }
    n = q - r;
    *q++ = '\0';
    if ((q = realloc(r, n + 1))) r = q;
  } else {
    n = 0;
  }
  if (out_size) {
    *out_size = n;
  }
  return r;
}
