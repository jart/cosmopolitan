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
#include "third_party/mbedtls/md.h"

const char *mbedtls_md_type_name(mbedtls_md_type_t t) {
  switch (t) {
    case MBEDTLS_MD_NONE:
      return "NONE";
    case MBEDTLS_MD_MD2:
      return "MD2";
    case MBEDTLS_MD_MD4:
      return "MD4";
    case MBEDTLS_MD_MD5:
      return "MD5";
    case MBEDTLS_MD_SHA1:
      return "SHA1";
    case MBEDTLS_MD_SHA224:
      return "SHA224";
    case MBEDTLS_MD_SHA256:
      return "SHA256";
    case MBEDTLS_MD_SHA384:
      return "SHA384";
    case MBEDTLS_MD_SHA512:
      return "SHA512";
    default:
      return 0;
  }
}
