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
#include "libc/fmt/itoa.h"
#include "third_party/mbedtls/iana.h"
#include "third_party/mbedtls/ssl.h"

/**
 * Returns SSL fatal alert description.
 * @see RFC5246 §7.2
 */
const char *GetAlertDescription(unsigned char x) {
  static char buf[4];
  switch (x) {
    case MBEDTLS_SSL_ALERT_MSG_CLOSE_NOTIFY: /* 0 */
      return "close_notify";
    case MBEDTLS_SSL_ALERT_MSG_UNEXPECTED_MESSAGE: /* 10 */
      return "unexpected_message";
    case MBEDTLS_SSL_ALERT_MSG_BAD_RECORD_MAC: /* 20 */
      return "bad_record_mac";
    case MBEDTLS_SSL_ALERT_MSG_DECRYPTION_FAILED: /* 21 */
      return "decryption_failed";
    case MBEDTLS_SSL_ALERT_MSG_RECORD_OVERFLOW: /* 22 */
      return "record_overflow";
    case MBEDTLS_SSL_ALERT_MSG_DECOMPRESSION_FAILURE: /* 30 */
      return "decompression_failure";
    case MBEDTLS_SSL_ALERT_MSG_HANDSHAKE_FAILURE: /* 40 */
      return "handshake_failure";
    case MBEDTLS_SSL_ALERT_MSG_NO_CERT: /* 41 */
      return "no_cert";
    case MBEDTLS_SSL_ALERT_MSG_BAD_CERT: /* 42 */
      return "bad_cert";
    case MBEDTLS_SSL_ALERT_MSG_UNSUPPORTED_CERT: /* 43 */
      return "unsupported_cert";
    case MBEDTLS_SSL_ALERT_MSG_CERT_REVOKED: /* 44 */
      return "cert_revoked";
    case MBEDTLS_SSL_ALERT_MSG_CERT_EXPIRED: /* 45 */
      return "cert_expired";
    case MBEDTLS_SSL_ALERT_MSG_CERT_UNKNOWN: /* 46 */
      return "cert_unknown";
    case MBEDTLS_SSL_ALERT_MSG_ILLEGAL_PARAMETER: /* 47 */
      return "illegal_parameter";
    case MBEDTLS_SSL_ALERT_MSG_UNKNOWN_CA: /* 48 */
      return "unknown_ca";
    case MBEDTLS_SSL_ALERT_MSG_ACCESS_DENIED: /* 49 */
      return "access_denied";
    case MBEDTLS_SSL_ALERT_MSG_DECODE_ERROR: /* 50 */
      return "decode_error";
    case MBEDTLS_SSL_ALERT_MSG_DECRYPT_ERROR: /* 51 */
      return "decrypt_error";
    case MBEDTLS_SSL_ALERT_MSG_EXPORT_RESTRICTION: /* 60 */
      return "export_restriction";
    case MBEDTLS_SSL_ALERT_MSG_PROTOCOL_VERSION: /* 70 */
      return "protocol_version";
    case MBEDTLS_SSL_ALERT_MSG_INSUFFICIENT_SECURITY: /* 71 */
      return "insufficient_security";
    case MBEDTLS_SSL_ALERT_MSG_INTERNAL_ERROR: /* 80 */
      return "internal_error";
    case MBEDTLS_SSL_ALERT_MSG_USER_CANCELED: /* 90 */
      return "user_canceled";
    case MBEDTLS_SSL_ALERT_MSG_NO_RENEGOTIATION: /* 100 */
      return "no_renegotiation";
    case MBEDTLS_SSL_ALERT_MSG_UNSUPPORTED_EXT: /* 110 */
      return "unsupported_extension";
    default:
      int64toarray_radix10(x, buf);
      return buf;
  }
}
