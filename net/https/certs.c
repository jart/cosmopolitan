/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2025 Justine Alexandra Roberts Tunney                              │
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
#include "libc/log/log.h"
#include "libc/mem/gc.h"
#include "libc/str/slice.h"
#include "net/http/ip.h"
#include "net/https/https.h"
#include "third_party/mbedtls/x509.h"
#include "third_party/mbedtls/x509_crt.h"

/**
 * @fileoverview ssl certificate manager
 *
 * When you setup an MbedTLS SSL server, you can load use this to load
 * your certificate chains and private keys. You can load certificates
 * for multiple hostnames you want to serve. You can have both RSA and
 * ECDSA certificates. Then when an SSL handshake happens, this module
 * will select the most appropriate server certificate chain to use.
 */

void CertsDestroy(struct Certs *certs) {
  size_t i;
  // break up certificate chains to prevent double free
  for (i = 0; i < certs->n; ++i)
    if (certs->p[i].cert)
      certs->p[i].cert->next = 0;
  for (i = 0; i < certs->n; ++i) {
    mbedtls_x509_crt_free(certs->p[i].cert);
    free(certs->p[i].cert);
    mbedtls_pk_free(certs->p[i].key);
    free(certs->p[i].key);
  }
  free(certs->p);
  certs->p = 0;
  certs->n = 0;
}

void AppendCert(struct Certs *certs, mbedtls_x509_crt *cert,
                mbedtls_pk_context *key) {
  certs->p = realloc(certs->p, ++certs->n * sizeof(*certs->p));
  certs->p[certs->n - 1].cert = cert;
  certs->p[certs->n - 1].key = key;
}

static void InternCertificate(struct Certs *certs, mbedtls_x509_crt *cert,
                              mbedtls_x509_crt *prev) {
  int r;
  size_t i;
  if (cert->next)
    InternCertificate(certs, cert->next, cert);
  if (prev) {
    if (mbedtls_x509_crt_check_parent(prev, cert, 1)) {
      DEBUGF("(ssl) unbundling %`'s from %`'s",
             gc(FormatX509Name(&prev->subject)),
             gc(FormatX509Name(&cert->subject)));
      prev->next = 0;
    } else if ((r = mbedtls_x509_crt_check_signature(prev, cert, 0))) {
      WARNF("(ssl) invalid signature for %`'s -> %`'s (-0x%04x)",
            gc(FormatX509Name(&prev->subject)),
            gc(FormatX509Name(&cert->subject)), -r);
    }
  }
  if (mbedtls_x509_time_is_past(&cert->valid_to)) {
    WARNF("(ssl) certificate %`'s is expired",
          gc(FormatX509Name(&cert->subject)));
  } else if (mbedtls_x509_time_is_future(&cert->valid_from)) {
    WARNF("(ssl) certificate %`'s is from the future",
          gc(FormatX509Name(&cert->subject)));
  }
  for (i = 0; i < certs->n; ++i) {
    if (!certs->p[i].cert && certs->p[i].key &&
        !mbedtls_pk_check_pair(&cert->pk, certs->p[i].key)) {
      certs->p[i].cert = cert;
      return;
    }
  }
  LogCertificate("loaded certificate", cert);
  if (!cert->next && !IsSelfSigned(cert) && cert->max_pathlen) {
    for (i = 0; i < certs->n; ++i) {
      if (!certs->p[i].cert)
        continue;
      if (mbedtls_pk_can_do(&cert->pk, certs->p[i].cert->sig_pk) &&
          !mbedtls_x509_crt_check_parent(cert, certs->p[i].cert, 1) &&
          !IsSelfSigned(certs->p[i].cert)) {
        if (ChainCertificate(cert, certs->p[i].cert))
          break;
      }
    }
  }
  if (!IsSelfSigned(cert)) {
    for (i = 0; i < certs->n; ++i) {
      if (!certs->p[i].cert)
        continue;
      if (certs->p[i].cert->next)
        continue;
      if (certs->p[i].cert->max_pathlen &&
          mbedtls_pk_can_do(&certs->p[i].cert->pk, cert->sig_pk) &&
          !mbedtls_x509_crt_check_parent(certs->p[i].cert, cert, 1)) {
        ChainCertificate(certs->p[i].cert, cert);
      }
    }
  }
  AppendCert(certs, cert, 0);
}

/**
 * Loads SSL certificate chain.
 *
 * @param p points to ascii-armored or binary certificate data
 * @param n is byte length of `p` or -1 for strlen()
 */
void ProgramCertificate(struct Certs *certs, const char *p, size_t n) {
  int rc;
  unsigned char *waqapi;
  mbedtls_x509_crt *cert;
  if (n == -1)
    n = strlen(p);
  waqapi = malloc(n + 1);
  memcpy(waqapi, p, n);
  waqapi[n] = 0;
  cert = calloc(1, sizeof(mbedtls_x509_crt));
  rc = mbedtls_x509_crt_parse(cert, waqapi, n + 1);
  mbedtls_platform_zeroize(waqapi, n);
  free(waqapi);
  if (rc < 0) {
    WARNF("(ssl) failed to load certificate (grep -0x%04x)", rc);
    return;
  } else if (rc > 0) {
    VERBOSEF("(ssl) certificate bundle partially loaded");
  }
  InternCertificate(certs, cert, 0);
}

/**
 * Loads SSL private key.
 *
 * @param p points to ascii-armored or binary certificate data
 * @param n is byte length of `p` or -1 for strlen()
 */
void ProgramPrivateKey(struct Certs *certs, const char *p, size_t n) {
  int rc;
  size_t i;
  unsigned char *waqapi;
  mbedtls_pk_context *key;
  if (n == -1)
    n = strlen(p);
  waqapi = malloc(n + 1);
  memcpy(waqapi, p, n);
  waqapi[n] = 0;
  key = calloc(1, sizeof(mbedtls_pk_context));
  rc = mbedtls_pk_parse_key(key, waqapi, n + 1, 0, 0);
  mbedtls_platform_zeroize(waqapi, n);
  free(waqapi);
  if (rc != 0)
    FATALF("(ssl) error: load key (grep -0x%04x)", -rc);
  for (i = 0; i < certs->n; ++i) {
    if (certs->p[i].cert && !certs->p[i].key &&
        !mbedtls_pk_check_pair(&certs->p[i].cert->pk, key)) {
      certs->p[i].key = key;
      return;
    }
  }
  VERBOSEF("(ssl) loaded private key");
  AppendCert(certs, 0, key);
}

static bool CertHasCommonName(const mbedtls_x509_crt *cert, const void *s,
                              size_t n) {
  const mbedtls_x509_name *name;
  for (name = &cert->subject; name; name = name->next) {
    if (!MBEDTLS_OID_CMP(MBEDTLS_OID_AT_CN, &name->oid)) {
      if (SlicesEqualCase(s, n, name->val.p, name->val.len))
        return true;
      break;
    }
  }
  return false;
}

static bool TlsRouteFind(struct Certs *certs, mbedtls_pk_type_t type,
                         mbedtls_ssl_context *ssl, const unsigned char *host,
                         size_t size, int64_t ip) {
  int i;
  for (i = 0; i < certs->n; ++i) {
    if (IsServerCert(certs->p + i, type) &&
        (((certs->p[i].cert->ext_types & MBEDTLS_X509_EXT_SUBJECT_ALT_NAME) &&
          (ip == -1 ? CertHasHost(certs->p[i].cert, host, size)
                    : CertHasIp(certs->p[i].cert, ip))) ||
         CertHasCommonName(certs->p[i].cert, host, size))) {
      CHECK_EQ(0, mbedtls_ssl_set_hs_own_cert(ssl, certs->p[i].cert,
                                              certs->p[i].key));
      DEBUGF("(ssl) TlsRoute(%s, %`'.*s) %s %`'s", mbedtls_pk_type_name(type),
             size, host, mbedtls_pk_get_name(&certs->p[i].cert->pk),
             gc(FormatX509Name(&certs->p[i].cert->subject)));
      return true;
    }
  }
  return false;
}

static bool TlsRouteFirst(struct Certs *certs, mbedtls_pk_type_t type,
                          mbedtls_ssl_context *ssl) {
  int i;
  for (i = 0; i < certs->n; ++i) {
    if (IsServerCert(certs->p + i, type)) {
      CHECK_EQ(0, mbedtls_ssl_set_hs_own_cert(ssl, certs->p[i].cert,
                                              certs->p[i].key));
      DEBUGF("(ssl) TlsRoute(%s) %s %`'s", mbedtls_pk_type_name(type),
             mbedtls_pk_get_name(&certs->p[i].cert->pk),
             gc(FormatX509Name(&certs->p[i].cert->subject)));
      return true;
    }
  }
  return false;
}

/**
 * Chooses best serving certificate for SSL handshake.
 */
int TlsRoute(void *ctx, mbedtls_ssl_context *ssl, const unsigned char *host,
             size_t size) {
  int64_t ip;
  bool ok1, ok2;
  struct Certs *certs = ctx;
  ip = ParseIp((const char *)host, size);
  ok1 = TlsRouteFind(certs, MBEDTLS_PK_ECKEY, ssl, host, size, ip);
  ok2 = TlsRouteFind(certs, MBEDTLS_PK_RSA, ssl, host, size, ip);
  if (!ok1 && !ok2) {
    WARNF("(ssl) TlsRoute(%`'.*s) not found", size, host);
    ok1 = TlsRouteFirst(certs, MBEDTLS_PK_ECKEY, ssl);
    ok2 = TlsRouteFirst(certs, MBEDTLS_PK_RSA, ssl);
  }
  return ok1 || ok2 ? 0 : -1;
}
