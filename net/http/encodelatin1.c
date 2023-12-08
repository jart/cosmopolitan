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
#include "libc/intrin/pcmpgtb.h"
#include "libc/intrin/pmovmskb.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "net/http/escape.h"

/**
 * Encodes UTF-8 to ISO-8859-1.
 *
 * @param p is input value
 * @param n if -1 implies strlen
 * @param z if non-NULL receives output length
 * @param f can kControlC0, kControlC1, kControlWs to forbid
 * @return allocated NUL-terminated buffer, or NULL w/ errno
 * @error EILSEQ means UTF-8 found we can't or won't re-encode
 * @error ENOMEM means malloc() failed
 */
char *EncodeLatin1(const char *p, size_t n, size_t *z, int f) {
  int c;
  size_t i;
  char t[256];
  char *r, *q;
  bzero(t, sizeof(t));
  if (f & kControlC0) memset(t + 0x00, 1, 0x20 - 0x00), t[0x7F] = 1;
  if (f & kControlC1) memset(t + 0x80, 1, 0xA0 - 0x80);
  t['\t'] = t['\r'] = t['\n'] = t['\v'] = !!(f & kControlWs);
  if (z) *z = 0;
  if (n == -1) n = p ? strlen(p) : 0;
  if ((q = r = malloc(n + 1))) {
    for (i = 0; i < n;) {
      c = p[i++] & 0xff;
      if (c >= 0300) {
        if ((c <= 0303) && i < n && (p[i] & 0300) == 0200) {
          c = (c & 037) << 6 | (p[i++] & 077);
        } else {
          goto Invalid;
        }
      }
      if (t[c]) {
        goto Invalid;
      }
      *q++ = c;
    }
    if (z) *z = q - r;
    *q++ = '\0';
    if ((q = realloc(r, q - r))) r = q;
  }
  return r;
Invalid:
  free(r);
  errno = EILSEQ;
  return NULL;
}
