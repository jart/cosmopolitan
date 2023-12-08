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
#include "net/http/escape.h"

#define CHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"

/**
 * Encodes binary to base64 ascii representation.
 *
 * @param data is input value
 * @param size if -1 implies strlen
 * @param out_size if non-NULL receives output length
 * @return allocated NUL-terminated buffer, or NULL w/ errno
 */
char *EncodeBase64(const char *data, size_t size, size_t *out_size) {
  size_t n;
  unsigned w;
  char *r, *q;
  const unsigned char *p, *pe;
  if (size == -1) size = data ? strlen(data) : 0;
  if ((n = size) % 3) n += 3 - size % 3;
  n /= 3, n *= 4;
  if ((r = malloc(n + 1))) {
    for (q = r, p = (void *)data, pe = p + size; p < pe; p += 3) {
      w = p[0] << 020;
      if (p + 1 < pe) w |= p[1] << 010;
      if (p + 2 < pe) w |= p[2] << 000;
      *q++ = CHARS[(w >> 18) & 077];
      *q++ = CHARS[(w >> 12) & 077];
      *q++ = p + 1 < pe ? CHARS[(w >> 6) & 077] : '=';
      *q++ = p + 2 < pe ? CHARS[w & 077] : '=';
    }
    *q++ = '\0';
  } else {
    n = 0;
  }
  if (out_size) {
    *out_size = n;
  }
  return r;
}
