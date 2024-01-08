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
#include "libc/log/log.h"
#include "libc/mem/gc.h"
#include "libc/x/xasprintf.h"
#include "net/https/https.h"

struct Cert FinishCertificate(struct Cert *ca, mbedtls_x509write_cert *wcert,
                              mbedtls_pk_context *key) {
  int i, n, rc;
  unsigned char *p;
  mbedtls_x509_crt *cert;
  p = malloc((n = FRAMESIZE));
  i = mbedtls_x509write_crt_der(wcert, p, n, GenerateHardRandom, 0);
  if (i < 0) FATALF("write key (grep -0x%04x)", -i);
  cert = calloc(1, sizeof(mbedtls_x509_crt));
  mbedtls_x509_crt_parse(cert, p + n - i, i);
  if (ca) cert->next = ca->cert;
  mbedtls_x509write_crt_free(wcert);
  free(p);
  if ((rc = mbedtls_pk_check_pair(&cert->pk, key))) {
    FATALF("generate key (grep -0x%04x)", -rc);
  }
  LogCertificate(
      gc(xasprintf("generated %s certificate", mbedtls_pk_get_name(&cert->pk))),
      cert);
  return (struct Cert){cert, key};
}
