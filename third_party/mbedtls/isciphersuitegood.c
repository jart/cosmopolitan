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
#include "third_party/mbedtls/iana.h"

bool IsCipherSuiteGood(uint16_t x) {
  switch (x) {
    case 0x009E: /* TLS_DHE_RSA_WITH_AES_128_GCM_SHA256 (RFC5288) */
    case 0x009F: /* TLS_DHE_RSA_WITH_AES_256_GCM_SHA384 (RFC5288) */
    case 0x00AA: /* TLS_DHE_PSK_WITH_AES_128_GCM_SHA256 (RFC5487) */
    case 0x00AB: /* TLS_DHE_PSK_WITH_AES_256_GCM_SHA384 (RFC5487) */
    case 0x1301: /* TLS_AES_128_GCM_SHA256 (RFC8446) */
    case 0x1302: /* TLS_AES_256_GCM_SHA384 (RFC8446) */
    case 0x1303: /* TLS_CHACHA20_POLY1305_SHA256 (RFC8446) */
    case 0x1304: /* TLS_AES_128_CCM_SHA256 (RFC8446) */
    case 0xC02B: /* TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256 (RFC5289) */
    case 0xC02C: /* TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384 (RFC5289) */
    case 0xC02F: /* TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256 (RFC5289) */
    case 0xC030: /* TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384 (RFC5289) */
    case 0xC09E: /* TLS_DHE_RSA_WITH_AES_128_CCM (RFC6655) */
    case 0xC09F: /* TLS_DHE_RSA_WITH_AES_256_CCM (RFC6655) */
    case 0xC0A6: /* TLS_DHE_PSK_WITH_AES_128_CCM (RFC6655) */
    case 0xC0A7: /* TLS_DHE_PSK_WITH_AES_256_CCM (RFC6655) */
    case 0xCCA8: /* TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256 (RFC7905) */
    case 0xCCA9: /* TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256 (RFC7905) */
    case 0xCCAA: /* TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256 (RFC7905) */
    case 0xCCAC: /* TLS_ECDHE_PSK_WITH_CHACHA20_POLY1305_SHA256 (RFC7905) */
    case 0xCCAD: /* TLS_DHE_PSK_WITH_CHACHA20_POLY1305_SHA256 (RFC7905) */
    case 0xD001: /* TLS_ECDHE_PSK_WITH_AES_128_GCM_SHA256 (RFC8442) */
    case 0xD002: /* TLS_ECDHE_PSK_WITH_AES_256_GCM_SHA384 (RFC8442) */
    case 0xD005: /* TLS_ECDHE_PSK_WITH_AES_128_CCM_SHA256 (RFC8442) */
      return true;
    default:
      return false;
  }
}
