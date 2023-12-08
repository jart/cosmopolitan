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
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "net/https/https.h"
#include "third_party/mbedtls/x509.h"

static const struct thatispacked SslVerifyString {
  int code;
  const char *str;
} kSslVerifyStrings[] = {
    {MBEDTLS_X509_BADCERT_BAD_KEY, "badcert_bad_key"},
    {MBEDTLS_X509_BADCERT_BAD_MD, "badcert_bad_md"},
    {MBEDTLS_X509_BADCERT_BAD_PK, "badcert_bad_pk"},
    {MBEDTLS_X509_BADCERT_CN_MISMATCH, "badcert_cn_mismatch"},
    {MBEDTLS_X509_BADCERT_EXPIRED, "badcert_expired"},
    {MBEDTLS_X509_BADCERT_EXT_KEY_USAGE, "badcert_ext_key_usage"},
    {MBEDTLS_X509_BADCERT_FUTURE, "badcert_future"},
    {MBEDTLS_X509_BADCERT_KEY_USAGE, "badcert_key_usage"},
    {MBEDTLS_X509_BADCERT_MISSING, "badcert_missing"},
    {MBEDTLS_X509_BADCERT_NOT_TRUSTED, "badcert_not_trusted"},
    {MBEDTLS_X509_BADCERT_NS_CERT_TYPE, "badcert_ns_cert_type"},
    {MBEDTLS_X509_BADCERT_OTHER, "badcert_other"},
    {MBEDTLS_X509_BADCERT_REVOKED, "badcert_revoked"},
    {MBEDTLS_X509_BADCERT_SKIP_VERIFY, "badcert_skip_verify"},
    {MBEDTLS_X509_BADCRL_BAD_KEY, "badcrl_bad_key"},
    {MBEDTLS_X509_BADCRL_BAD_MD, "badcrl_bad_md"},
    {MBEDTLS_X509_BADCRL_BAD_PK, "badcrl_bad_pk"},
    {MBEDTLS_X509_BADCRL_EXPIRED, "badcrl_expired"},
    {MBEDTLS_X509_BADCRL_FUTURE, "badcrl_future"},
    {MBEDTLS_X509_BADCRL_NOT_TRUSTED, "badcrl_not_trusted"},
};

char *DescribeSslVerifyFailure(int flags) {
  int i;
  char *p, *q;
  p = malloc(1024);
  q = stpcpy(p, "verify failed");
  for (i = 0; i < ARRAYLEN(kSslVerifyStrings); ++i) {
    if (!(flags & kSslVerifyStrings[i].code)) continue;
    q = stpcpy(stpcpy(q, " "), kSslVerifyStrings[i].str);
  }
  return p;
}
