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
#include "libc/log/check.h"
#include "net/https/https.h"

mbedtls_pk_context *InitializeKey(struct Cert *ca,
                                  mbedtls_x509write_cert *wcert,
                                  mbedtls_md_type_t md_alg, int type) {
  mbedtls_pk_context *k = calloc(1, sizeof(mbedtls_pk_context));
  mbedtls_x509write_crt_init(wcert);
  mbedtls_x509write_crt_set_issuer_key(wcert, ca ? ca->key : k);
  mbedtls_x509write_crt_set_subject_key(wcert, k);
  mbedtls_x509write_crt_set_md_alg(wcert, md_alg);
  mbedtls_x509write_crt_set_version(wcert, MBEDTLS_X509_CRT_VERSION_3);
  CHECK_EQ(0, mbedtls_pk_setup(k, mbedtls_pk_info_from_type(type)));
  return k;
}
