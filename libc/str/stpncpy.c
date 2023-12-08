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
#include "libc/macros.internal.h"
#include "libc/str/str.h"

/**
 * Prepares static search buffer.
 *
 * 1. If SRC is too long, it's truncated and *not* NUL-terminated.
 * 2. If SRC is too short, the remainder is zero-filled.
 *
 * @param dst is output buffer
 * @param src is a nul-terminated string
 * @param dstlen is size of `dst` buffer
 * @return pointer to first nul-terminator, otherwise dest + stride
 * @asyncsignalsafe
 * @see strncpy()
 * @see memccpy()
 * @see strlcpy()
 */
char *stpncpy(char *dst, const char *src, size_t dstlen) {
  size_t srclen, cpylen, zerlen;
  srclen = strlen(src);
  cpylen = MIN(srclen, dstlen);
  if (cpylen) memcpy(dst, src, cpylen);
  zerlen = dstlen - cpylen;
  if (zerlen) bzero(dst + cpylen, zerlen);
  return dst + cpylen;
}
