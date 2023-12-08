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
#include "libc/str/slice.h"
#include "net/https/https.h"

bool CertHasHost(const mbedtls_x509_crt *cert, const void *s, size_t n) {
  const mbedtls_x509_sequence *cur;
  for (cur = &cert->subject_alt_names; cur; cur = cur->next) {
    if ((cur->buf.tag & MBEDTLS_ASN1_TAG_VALUE_MASK) ==
        MBEDTLS_X509_SAN_DNS_NAME) {
      if (cur->buf.len > 2 && cur->buf.p[0] == '*' && cur->buf.p[1] == '.') {
        // handle subject alt name like *.foo.com
        // - match examples
        //   - bar.foo.com
        //   - zoo.foo.com
        // - does not match
        //   - foo.com
        //   - zoo.bar.foo.com
        if (n > cur->buf.len - 1 &&
            SlicesEqualCase((char *)s + n - (cur->buf.len - 1),
                            cur->buf.len - 1, cur->buf.p + 1,
                            cur->buf.len - 1) &&
            !memchr(s, '.', n - (cur->buf.len - 1))) {
          return true;
        }
      } else {
        // handle subject alt name like foo.com
        if (SlicesEqualCase(s, n, cur->buf.p, cur->buf.len)) {
          return true;
        }
      }
    }
  }
  return false;
}
