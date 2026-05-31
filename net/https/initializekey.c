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
#include "libc/mem/mem.h"
#include "net/https/https.h"

static mbedtls_pk_context *InitWCert(struct Cert *ca,
                                     mbedtls_x509write_cert *wcert,
                                     mbedtls_md_type_t md_alg) {
  mbedtls_pk_context *k = calloc(1, sizeof(mbedtls_pk_context));
  mbedtls_x509write_crt_init(wcert);
  mbedtls_x509write_crt_set_issuer_key(wcert, ca ? ca->key : k);
  mbedtls_x509write_crt_set_subject_key(wcert, k);
  mbedtls_x509write_crt_set_md_alg(wcert, md_alg);
  mbedtls_x509write_crt_set_version(wcert, MBEDTLS_X509_CRT_VERSION_3);
  return k;
}

mbedtls_pk_context *GenerateECPKey(struct Cert *ca,
                                   mbedtls_x509write_cert *wcert,
                                   mbedtls_md_type_t md_alg,
                                   psa_ecc_family_t family, size_t bits) {
  mbedtls_pk_context *k = InitWCert(ca, wcert, md_alg);
  psa_key_attributes_t attrs = PSA_KEY_ATTRIBUTES_INIT;
  psa_set_key_usage_flags(&attrs, PSA_KEY_USAGE_SIGN_HASH |
                                      PSA_KEY_USAGE_VERIFY_HASH |
                                      PSA_KEY_USAGE_EXPORT);
  psa_set_key_algorithm(&attrs, PSA_ALG_ECDSA(PSA_ALG_ANY_HASH));
  psa_set_key_type(&attrs, PSA_KEY_TYPE_ECC_KEY_PAIR(family));
  psa_set_key_bits(&attrs, bits);
  mbedtls_svc_key_id_t key_id = MBEDTLS_SVC_KEY_ID_INIT;
  CHECK_EQ(PSA_SUCCESS, psa_generate_key(&attrs, &key_id));
  CHECK_EQ(0, mbedtls_pk_copy_from_psa(key_id, k));
  psa_destroy_key(key_id);
  return k;
}

mbedtls_pk_context *GenerateRSAKey(struct Cert *ca,
                                   mbedtls_x509write_cert *wcert,
                                   mbedtls_md_type_t md_alg, size_t bits) {
  mbedtls_pk_context *k = InitWCert(ca, wcert, md_alg);
  psa_key_attributes_t attrs = PSA_KEY_ATTRIBUTES_INIT;
  psa_set_key_usage_flags(&attrs, PSA_KEY_USAGE_SIGN_HASH |
                                      PSA_KEY_USAGE_VERIFY_HASH |
                                      PSA_KEY_USAGE_EXPORT);
  psa_set_key_algorithm(&attrs, PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH));
  psa_set_key_type(&attrs, PSA_KEY_TYPE_RSA_KEY_PAIR);
  psa_set_key_bits(&attrs, bits);
  mbedtls_svc_key_id_t key_id = MBEDTLS_SVC_KEY_ID_INIT;
  CHECK_EQ(PSA_SUCCESS, psa_generate_key(&attrs, &key_id));
  CHECK_EQ(0, mbedtls_pk_copy_from_psa(key_id, k));
  psa_destroy_key(key_id);
  return k;
}
