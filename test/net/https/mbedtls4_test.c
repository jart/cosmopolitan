/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
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
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include "libc/macros.h"
#include "libc/testlib/testlib.h"
#include "third_party/mbedtls4/include/mbedtls/debug.h"
#include "third_party/mbedtls4/include/mbedtls/error.h"
#include "third_party/mbedtls4/include/mbedtls/net_sockets.h"
#include "third_party/mbedtls4/include/mbedtls/oid.h"
#include "third_party/mbedtls4/include/mbedtls/ssl.h"
#include "third_party/mbedtls4/include/mbedtls/ssl_ticket.h"
#include "third_party/mbedtls4/include/mbedtls/version.h"
#include "third_party/mbedtls4/include/mbedtls/x509.h"
#include "third_party/mbedtls4/include/mbedtls/x509_crt.h"
#include "third_party/mbedtls4/tf-psa-crypto/include/mbedtls/asn1.h"
#include "third_party/mbedtls4/tf-psa-crypto/include/mbedtls/md.h"
#include "third_party/mbedtls4/tf-psa-crypto/include/mbedtls/pk.h"
#include "third_party/mbedtls4/tf-psa-crypto/include/mbedtls/platform_util.h"
#include "third_party/mbedtls4/tf-psa-crypto/include/psa/crypto.h"
#include "third_party/mbedtls4/tf-psa-crypto/drivers/builtin/include/mbedtls/private/bignum.h"

/* ── test vectors ────────────────────────────────────────────────────────── */

/* SHA-256("Cosmopolitan + mbedTLS 4")
 * echo -n 'Cosmopolitan + mbedTLS 4' | sha256sum */
static const char kMsg[] = "Cosmopolitan + mbedTLS 4";
static const uint8_t kSha256[32] = {
    0x31, 0x95, 0x6a, 0xd7, 0xb5, 0x9f, 0x31, 0xcc,
    0x7e, 0x6d, 0x75, 0x7e, 0x88, 0x33, 0x5f, 0x0d,
    0xab, 0xff, 0x54, 0x9f, 0xee, 0xa7, 0xdb, 0x7e,
    0xb3, 0x88, 0xac, 0x08, 0x8f, 0xc7, 0x9b, 0x73,
};

/* HMAC-SHA256(key="key", msg="The quick brown fox jumps over the lazy dog")
 * standard IETF/OpenSSL test vector */
static const uint8_t kHmacKey[] = "key";
static const uint8_t kHmacMsg[] =
    "The quick brown fox jumps over the lazy dog";
static const uint8_t kHmacSha256[32] = {
    0xf7, 0xbc, 0x83, 0xf4, 0x30, 0x53, 0x84, 0x24,
    0xb1, 0x32, 0x98, 0xe6, 0xaa, 0x6f, 0xb1, 0x43,
    0xef, 0x4d, 0x59, 0xa1, 0x49, 0x46, 0x17, 0x59,
    0x97, 0x47, 0x9d, 0xbc, 0x2d, 0x1a, 0x3c, 0xd8,
};

/* AES-256-GCM NIST test vector (SP 800-38D) */
static const uint8_t kAesKey[32] = {
    0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
    0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
    0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
    0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4,
};
static const uint8_t kAesNonce[12] = {
    0xca, 0xfe, 0xba, 0xbe, 0xfa, 0xce,
    0xdb, 0xad, 0xde, 0xca, 0xf8, 0x88,
};
static const uint8_t kAesPlain[] = "Hello, mbedTLS4!";

/* Missing hash vectors (from mbedtls_test.c): SHA-1/224/384/512/MD5 of "abc",
 * and PBKDF2-HMAC-SHA1(password, salt, 1, 20) from RFC 6070 test vector 1. */
static const uint8_t kAbsSha1[20] = {
    0xa9, 0x99, 0x3e, 0x36, 0x47, 0x06, 0x81, 0x6a, 0xba, 0x3e,
    0x25, 0x71, 0x78, 0x50, 0xc2, 0x6c, 0x9c, 0xd0, 0xd8, 0x9d,
};
static const uint8_t kAbsSha224[28] = {
    0x23, 0x09, 0x7d, 0x22, 0x34, 0x05, 0xd8, 0x22,
    0x86, 0x42, 0xa4, 0x77, 0xbd, 0xa2, 0x55, 0xb3,
    0x2a, 0xad, 0xbc, 0xe4, 0xbd, 0xa0, 0xb3, 0xf7,
    0xe3, 0x6c, 0x9d, 0xa7,
};
static const uint8_t kAbsSha384[48] = {
    0xcb, 0x00, 0x75, 0x3f, 0x45, 0xa3, 0x5e, 0x8b,
    0xb5, 0xa0, 0x3d, 0x69, 0x9a, 0xc6, 0x50, 0x07,
    0x27, 0x2c, 0x32, 0xab, 0x0e, 0xde, 0xd1, 0x63,
    0x1a, 0x8b, 0x60, 0x5a, 0x43, 0xff, 0x5b, 0xed,
    0x80, 0x86, 0x07, 0x2b, 0xa1, 0xe7, 0xcc, 0x23,
    0x58, 0xba, 0xec, 0xa1, 0x34, 0xc8, 0x25, 0xa7,
};
static const uint8_t kAbsSha512[64] = {
    0xdd, 0xaf, 0x35, 0xa1, 0x93, 0x61, 0x7a, 0xba,
    0xcc, 0x41, 0x73, 0x49, 0xae, 0x20, 0x41, 0x31,
    0x12, 0xe6, 0xfa, 0x4e, 0x89, 0xa9, 0x7e, 0xa2,
    0x0a, 0x9e, 0xee, 0xe6, 0x4b, 0x55, 0xd3, 0x9a,
    0x21, 0x92, 0x99, 0x2a, 0x27, 0x4f, 0xc1, 0xa8,
    0x36, 0xba, 0x3c, 0x23, 0xa3, 0xfe, 0xeb, 0xbd,
    0x45, 0x4d, 0x44, 0x23, 0x64, 0x3c, 0xe8, 0x0e,
    0x2a, 0x9a, 0xc9, 0x4f, 0xa5, 0x4c, 0xa4, 0x9f,
};
static const uint8_t kAbsMd5[16] = {
    0x90, 0x01, 0x50, 0x98, 0x3c, 0xd2, 0x4f, 0xb0,
    0xd6, 0x96, 0x3f, 0x7d, 0x28, 0xe1, 0x7f, 0x72,
};
/* PBKDF2-HMAC-SHA1("password", "salt", 1 iter, 20 bytes) — RFC 6070 §2 vec 1 */
static const uint8_t kPbkdf2Dk[20] = {
    0x0c, 0x60, 0xc8, 0x0f, 0x96, 0x1f, 0x0e, 0x71,
    0xf3, 0xa9, 0xb5, 0x24, 0xaf, 0x60, 0x12, 0x06,
    0x2f, 0xe0, 0x37, 0xa6,
};

/* PolarSSL self-signed CA cert (v3, RSA-2048, serial=3) */
static const char kCaPem[] =
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIIDQTCCAimgAwIBAgIBAzANBgkqhkiG9w0BAQsFADA7MQswCQYDVQQGEwJOTDER\r\n"
    "MA8GA1UECgwIUG9sYXJTU0wxGTAXBgNVBAMMEFBvbGFyU1NMIFRlc3QgQ0EwHhcN\r\n"
    "MTkwMjEwMTQ0NDAwWhcNMjkwMjEwMTQ0NDAwWjA7MQswCQYDVQQGEwJOTDERMA8G\r\n"
    "A1UECgwIUG9sYXJTU0wxGTAXBgNVBAMMEFBvbGFyU1NMIFRlc3QgQ0EwggEiMA0G\r\n"
    "CSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDA3zf8F7vglp0/ht6WMn1EpRagzSHx\r\n"
    "mdTs6st8GFgIlKXsm8WL3xoemTiZhx57wI053zhdcHgH057Zk+i5clHFzqMwUqny\r\n"
    "50BwFMtEonILwuVA+T7lpg6z+exKY8C4KQB0nFc7qKUEkHHxvYPZP9al4jwqj+8n\r\n"
    "YMPGn8u67GB9t+aEMr5P+1gmIgNb1LTV+/Xjli5wwOQuvfwu7uJBVcA0Ln0kcmnL\r\n"
    "R7EUQIN9Z/SG9jGr8XmksrUuEvmEF/Bibyc+E1ixVA0hmnM3oTDPb5Lc9un8rNsu\r\n"
    "KNF+AksjoBXyOGVkCeoMbo4bF6BxyLObyavpw/LPh5aPgAIynplYb6LVAgMBAAGj\r\n"
    "UDBOMAwGA1UdEwQFMAMBAf8wHQYDVR0OBBYEFLRa5KWz3tJS9rnVppUP6z68x/3/\r\n"
    "MB8GA1UdIwQYMBaAFLRa5KWz3tJS9rnVppUP6z68x/3/MA0GCSqGSIb3DQEBCwUA\r\n"
    "A4IBAQA4qFSCth2q22uJIdE4KGHJsJjVEfw2/xn+MkTvCMfxVrvmRvqCtjE4tKDl\r\n"
    "oK4MxFOek07oDZwvtAT9ijn1hHftTNS7RH9zd/fxNpfcHnMZXVC4w4DNA1fSANtW\r\n"
    "5sY1JB5Je9jScrsLSS+mAjyv0Ow3Hb2Bix8wu7xNNrV5fIf7Ubm+wt6SqEBxu3Kb\r\n"
    "+EfObAT4huf3czznhH3C17ed6NSbXwoXfby7stWUDeRJv08RaFOykf/Aae7bY5PL\r\n"
    "yTVrkAnikMntJ9YI+hNNYt3inqq11A5cN0+rVTst8UKCxzQ4GpvroSwPKTFkbMw4\r\n"
    "/anT1dVxr/BtwJfiESoK3/4CeXR1\r\n"
    "-----END CERTIFICATE-----\r\n";

/* ── helpers ─────────────────────────────────────────────────────────────── */

static int AllZero(const uint8_t *p, size_t n) {
  for (size_t i = 0; i < n; ++i)
    if (p[i]) return 0;
  return 1;
}

typedef struct {
  mbedtls_x509_crt *crt;
  mbedtls_pk_context *pk;
} SniCtx;

static int loopback_sni_cb(void *p, mbedtls_ssl_context *ssl,
                            const unsigned char *name, size_t len) {
  (void)name;
  (void)len;
  SniCtx *ctx = (SniCtx *)p;
  return mbedtls_ssl_set_hs_own_cert(ssl, ctx->crt, ctx->pk);
}

static int loopback_send(void *ctx, const unsigned char *buf, size_t len) {
  int fd = *(int *)ctx;
  ssize_t r = write(fd, buf, len);
  return r < 0 ? MBEDTLS_ERR_NET_SEND_FAILED : (int)r;
}

static int loopback_recv(void *ctx, unsigned char *buf, size_t len) {
  int fd = *(int *)ctx;
  ssize_t r = read(fd, buf, len);
  if (r == 0) return MBEDTLS_ERR_SSL_CONN_EOF;
  return r < 0 ? MBEDTLS_ERR_NET_RECV_FAILED : (int)r;
}

static void debug_noop(void *ctx, int level, const char *file, int line,
                       const char *str) {
  (void)ctx;
  (void)level;
  (void)file;
  (void)line;
  (void)str;
}

static int dummy_psk_cb(void *p, mbedtls_ssl_context *ssl,
                        const unsigned char *id, size_t id_len) {
  (void)p;
  (void)id;
  (void)id_len;
  static const unsigned char psk[16] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
  };
  return mbedtls_ssl_set_hs_psk(ssl, psk, sizeof(psk));
}

/* Helper to generate a minimal self-signed EC cert PEM into out_cert/out_key */
static int GenSelfSignedCert(const char *cn,
                              char *out_cert, size_t cert_sz,
                              char *out_key, size_t key_sz) {
  psa_key_attributes_t kattr = PSA_KEY_ATTRIBUTES_INIT;
  psa_set_key_usage_flags(&kattr,
      PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH |
      PSA_KEY_USAGE_EXPORT);
  psa_set_key_algorithm(&kattr, PSA_ALG_ECDSA(PSA_ALG_SHA_256));
  psa_set_key_type(&kattr, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
  psa_set_key_bits(&kattr, 256);
  psa_key_id_t key_id = 0;
  if (psa_generate_key(&kattr, &key_id) != PSA_SUCCESS) return -1;

  mbedtls_pk_context pk;
  mbedtls_pk_init(&pk);
  if (mbedtls_pk_copy_from_psa(key_id, &pk) != 0) {
    psa_destroy_key(key_id);
    return -1;
  }

  if (out_key && key_sz > 0) {
    if (mbedtls_pk_write_key_pem(&pk, (unsigned char *)out_key, key_sz) != 0) {
      mbedtls_pk_free(&pk);
      psa_destroy_key(key_id);
      return -1;
    }
  }

  mbedtls_x509write_cert wctx;
  mbedtls_x509write_crt_init(&wctx);
  mbedtls_x509write_crt_set_version(&wctx, MBEDTLS_X509_CRT_VERSION_3);

  static const unsigned char serial[] = {0x01};
  mbedtls_x509write_crt_set_serial_raw(&wctx, serial, sizeof(serial));
  mbedtls_x509write_crt_set_validity(&wctx, "20240101000000", "20340101000000");

  char dn[128];
  snprintf(dn, sizeof(dn), "CN=%s,O=mbedtls4test,C=US", cn);
  mbedtls_x509write_crt_set_subject_name(&wctx, dn);
  mbedtls_x509write_crt_set_issuer_name(&wctx, dn);

  mbedtls_x509write_crt_set_subject_key(&wctx, &pk);
  mbedtls_x509write_crt_set_issuer_key(&wctx, &pk);
  mbedtls_x509write_crt_set_md_alg(&wctx, MBEDTLS_MD_SHA256);
  mbedtls_x509write_crt_set_basic_constraints(&wctx, 1, -1);
  mbedtls_x509write_crt_set_subject_key_identifier(&wctx);
  mbedtls_x509write_crt_set_authority_key_identifier(&wctx);
  mbedtls_x509write_crt_set_key_usage(&wctx,
      MBEDTLS_X509_KU_DIGITAL_SIGNATURE | MBEDTLS_X509_KU_KEY_CERT_SIGN);

  int rc = mbedtls_x509write_crt_pem(&wctx, (unsigned char *)out_cert, cert_sz);
  mbedtls_x509write_crt_free(&wctx);
  mbedtls_pk_free(&pk);
  psa_destroy_key(key_id);
  return rc;
}

/* ── one-time suite setup ────────────────────────────────────────────────── */

void SetUpOnce(void) {
  /* psa_crypto_init() is idempotent; initialises the PSA crypto subsystem
   * once before any TEST() function runs. */
  psa_crypto_init();
}

void TearDownOnce(void) {
  mbedtls_psa_crypto_free();
}

/* ── PSA lifecycle ───────────────────────────────────────────────────────── */

TEST(psa, init) {
  EXPECT_EQ(PSA_SUCCESS, (int)psa_crypto_init());
}

/* ── PSA RNG ─────────────────────────────────────────────────────────────── */

TEST(psa, rng) {
  uint8_t buf[32] = {0};
  ASSERT_EQ(PSA_SUCCESS, (int)psa_generate_random(buf, sizeof(buf)));
  EXPECT_FALSE(AllZero(buf, sizeof(buf)));
}

/* ── PSA hash ────────────────────────────────────────────────────────────── */

TEST(psa, hashOneShot) {
  uint8_t digest[32];
  size_t digest_len = 0;
  EXPECT_EQ(PSA_SUCCESS,
            (int)psa_hash_compute(PSA_ALG_SHA_256, (const uint8_t *)kMsg,
                                  strlen(kMsg), digest, sizeof(digest),
                                  &digest_len));
  EXPECT_EQ(32, (int)digest_len);
  EXPECT_EQ(0, memcmp(digest, kSha256, sizeof(kSha256)));
}

TEST(psa, hashMultiPart) {
  psa_hash_operation_t op = PSA_HASH_OPERATION_INIT;
  EXPECT_EQ(PSA_SUCCESS, (int)psa_hash_setup(&op, PSA_ALG_SHA_256));
  EXPECT_EQ(PSA_SUCCESS,
            (int)psa_hash_update(&op, (const uint8_t *)kMsg, strlen(kMsg)));
  uint8_t digest[32];
  size_t digest_len = 0;
  EXPECT_EQ(PSA_SUCCESS,
            (int)psa_hash_finish(&op, digest, sizeof(digest), &digest_len));
  EXPECT_EQ(0, memcmp(digest, kSha256, sizeof(kSha256)));
}

/* ── mbedtls_md wrapper ──────────────────────────────────────────────────── */

TEST(md, sha256) {
  const mbedtls_md_info_t *info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
  ASSERT_NE(NULL, info);
  EXPECT_EQ(32, (int)mbedtls_md_get_size(info));
  uint8_t digest[32];
  EXPECT_EQ(0, mbedtls_md(info, (const unsigned char *)kMsg, strlen(kMsg),
                           digest));
  EXPECT_EQ(0, memcmp(digest, kSha256, sizeof(kSha256)));
}

TEST(md, sha512Info) {
  const mbedtls_md_info_t *info512 =
      mbedtls_md_info_from_type(MBEDTLS_MD_SHA512);
  ASSERT_NE(NULL, info512);
  EXPECT_EQ(64, (int)mbedtls_md_get_size(info512));
}

/* ── PSA HMAC-SHA256 ─────────────────────────────────────────────────────── */

TEST(psa, hmacSha256) {
  psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
  psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_SIGN_MESSAGE);
  psa_set_key_algorithm(&attr, PSA_ALG_HMAC(PSA_ALG_SHA_256));
  psa_set_key_type(&attr, PSA_KEY_TYPE_HMAC);
  psa_key_id_t key_id = 0;
  ASSERT_EQ(PSA_SUCCESS,
            (int)psa_import_key(&attr, kHmacKey, sizeof(kHmacKey) - 1,
                                 &key_id));
  uint8_t mac[32];
  size_t mac_len = 0;
  EXPECT_EQ(PSA_SUCCESS,
            (int)psa_mac_compute(key_id, PSA_ALG_HMAC(PSA_ALG_SHA_256),
                                 kHmacMsg, sizeof(kHmacMsg) - 1,
                                 mac, sizeof(mac), &mac_len));
  EXPECT_EQ(32, (int)mac_len);
  EXPECT_EQ(0, memcmp(mac, kHmacSha256, sizeof(kHmacSha256)));
  psa_destroy_key(key_id);
}

/* ── PSA EC P-256 keygen / sign / verify ─────────────────────────────────── */

TEST(psa, eccP256) {
  psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
  psa_set_key_usage_flags(&attr,
      PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH);
  psa_set_key_algorithm(&attr, PSA_ALG_ECDSA(PSA_ALG_SHA_256));
  psa_set_key_type(&attr, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
  psa_set_key_bits(&attr, 256);
  psa_key_id_t key_id = 0;
  ASSERT_EQ(PSA_SUCCESS, (int)psa_generate_key(&attr, &key_id));

  uint8_t hash[32], sig[128];
  size_t sig_len = 0;
  memset(hash, 0xab, sizeof(hash));
  EXPECT_EQ(PSA_SUCCESS,
            (int)psa_sign_hash(key_id, PSA_ALG_ECDSA(PSA_ALG_SHA_256),
                               hash, sizeof(hash),
                               sig, sizeof(sig), &sig_len));
  EXPECT_EQ(PSA_SUCCESS,
            (int)psa_verify_hash(key_id, PSA_ALG_ECDSA(PSA_ALG_SHA_256),
                                 hash, sizeof(hash), sig, sig_len));
  /* tamper: flipping a signature byte must be rejected */
  sig[0] ^= 0xff;
  EXPECT_NE(PSA_SUCCESS,
            (int)psa_verify_hash(key_id, PSA_ALG_ECDSA(PSA_ALG_SHA_256),
                                 hash, sizeof(hash), sig, sig_len));
  psa_destroy_key(key_id);
}

/* ── PSA AES-256-GCM encrypt/decrypt ─────────────────────────────────────── */

TEST(psa, aesGcm) {
  psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
  psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
  psa_set_key_algorithm(&attr, PSA_ALG_GCM);
  psa_set_key_type(&attr, PSA_KEY_TYPE_AES);
  psa_set_key_bits(&attr, 256);
  psa_key_id_t key_id = 0;
  ASSERT_EQ(PSA_SUCCESS,
            (int)psa_import_key(&attr, kAesKey, sizeof(kAesKey), &key_id));

  size_t pt_len = sizeof(kAesPlain) - 1;
  uint8_t ct[sizeof(kAesPlain) + 16];
  uint8_t out[sizeof(kAesPlain)];
  size_t ct_len = 0, out_len = 0;

  EXPECT_EQ(PSA_SUCCESS,
            (int)psa_aead_encrypt(key_id, PSA_ALG_GCM,
                                  kAesNonce, sizeof(kAesNonce),
                                  NULL, 0,
                                  kAesPlain, pt_len,
                                  ct, sizeof(ct), &ct_len));
  EXPECT_EQ(PSA_SUCCESS,
            (int)psa_aead_decrypt(key_id, PSA_ALG_GCM,
                                  kAesNonce, sizeof(kAesNonce),
                                  NULL, 0,
                                  ct, ct_len,
                                  out, sizeof(out), &out_len));
  EXPECT_EQ((int)pt_len, (int)out_len);
  EXPECT_EQ(0, memcmp(out, kAesPlain, pt_len));
  /* tamper: flip a ciphertext byte → auth tag mismatch */
  ct[0] ^= 0xff;
  EXPECT_NE(PSA_SUCCESS,
            (int)psa_aead_decrypt(key_id, PSA_ALG_GCM,
                                  kAesNonce, sizeof(kAesNonce),
                                  NULL, 0,
                                  ct, ct_len,
                                  out, sizeof(out), &out_len));
  psa_destroy_key(key_id);
}

/* ── X.509 certificate parsing ───────────────────────────────────────────── */

TEST(x509, parse) {
  mbedtls_x509_crt crt;
  mbedtls_x509_crt_init(&crt);
  ASSERT_EQ(0, mbedtls_x509_crt_parse(&crt,
                                       (const unsigned char *)kCaPem,
                                       sizeof(kCaPem)));

  char subject[256], issuer[256];
  mbedtls_x509_dn_gets(subject, sizeof(subject), &crt.subject);
  mbedtls_x509_dn_gets(issuer, sizeof(issuer), &crt.issuer);
  EXPECT_NE(NULL, strstr(subject, "PolarSSL Test CA"));
  EXPECT_NE(NULL, strstr(issuer, "PolarSSL Test CA"));
  EXPECT_EQ(0, strcmp(subject, issuer));
  EXPECT_EQ(3, crt.version);
  EXPECT_EQ(1, (int)crt.serial.len);
  EXPECT_EQ(3, (int)crt.serial.p[0]);
  EXPECT_EQ(1, mbedtls_x509_crt_get_ca_istrue(&crt));
  EXPECT_NE(0, mbedtls_x509_crt_has_ext_type(&crt,
                                              MBEDTLS_X509_EXT_BASIC_CONSTRAINTS));
  EXPECT_NE(0, mbedtls_x509_crt_has_ext_type(&crt,
                                              MBEDTLS_X509_EXT_SUBJECT_KEY_IDENTIFIER));
  EXPECT_EQ(2048, (int)mbedtls_pk_get_bitlen(&crt.pk));
  mbedtls_x509_crt_free(&crt);
}

/* ── X.509 certificate generation ───────────────────────────────────────── */

TEST(x509, write) {
  /* 1. Generate EC P-256 key via PSA */
  psa_key_attributes_t kattr = PSA_KEY_ATTRIBUTES_INIT;
  psa_set_key_usage_flags(&kattr,
      PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH |
      PSA_KEY_USAGE_EXPORT);
  psa_set_key_algorithm(&kattr, PSA_ALG_ECDSA(PSA_ALG_SHA_256));
  psa_set_key_type(&kattr, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
  psa_set_key_bits(&kattr, 256);
  psa_key_id_t key_id = 0;
  ASSERT_EQ(PSA_SUCCESS, (int)psa_generate_key(&kattr, &key_id));

  /* 2. Bridge PSA key into PK context */
  mbedtls_pk_context pk;
  mbedtls_pk_init(&pk);
  EXPECT_EQ(0, mbedtls_pk_copy_from_psa(key_id, &pk));

  /* 3. Build self-signed cert */
  mbedtls_x509write_cert wctx;
  mbedtls_x509write_crt_init(&wctx);
  mbedtls_x509write_crt_set_version(&wctx, MBEDTLS_X509_CRT_VERSION_3);
  static const unsigned char kSerial[] = {0x01};
  EXPECT_EQ(0, mbedtls_x509write_crt_set_serial_raw(&wctx, kSerial,
                                                     sizeof(kSerial)));
  EXPECT_EQ(0, mbedtls_x509write_crt_set_validity(&wctx, "20240101000000",
                                                   "20340101000000"));
  EXPECT_EQ(0, mbedtls_x509write_crt_set_subject_name(
                   &wctx, "CN=mbedTLS4 Test,O=Cosmopolitan,C=US"));
  EXPECT_EQ(0, mbedtls_x509write_crt_set_issuer_name(
                   &wctx, "CN=mbedTLS4 Test,O=Cosmopolitan,C=US"));
  mbedtls_x509write_crt_set_subject_key(&wctx, &pk);
  mbedtls_x509write_crt_set_issuer_key(&wctx, &pk);
  mbedtls_x509write_crt_set_md_alg(&wctx, MBEDTLS_MD_SHA256);
  EXPECT_EQ(0, mbedtls_x509write_crt_set_basic_constraints(&wctx, 1, -1));
  EXPECT_EQ(0, mbedtls_x509write_crt_set_subject_key_identifier(&wctx));
  EXPECT_EQ(0, mbedtls_x509write_crt_set_authority_key_identifier(&wctx));
  EXPECT_EQ(0, mbedtls_x509write_crt_set_key_usage(&wctx,
      MBEDTLS_X509_KU_DIGITAL_SIGNATURE | MBEDTLS_X509_KU_KEY_CERT_SIGN));

  /* 4. Serialize to PEM */
  static unsigned char pem_buf[4096];
  int rc = mbedtls_x509write_crt_pem(&wctx, pem_buf, sizeof(pem_buf));
  EXPECT_EQ(0, rc);
  EXPECT_EQ(0, strncmp((char *)pem_buf, "-----BEGIN CERTIFICATE-----", 27));
  mbedtls_x509write_crt_free(&wctx);

  /* 5. Parse back and verify round-trip fields */
  if (rc == 0) {
    mbedtls_x509_crt crt;
    mbedtls_x509_crt_init(&crt);
    rc = mbedtls_x509_crt_parse(&crt, pem_buf, strlen((char *)pem_buf) + 1);
    EXPECT_EQ(0, rc);
    if (rc == 0) {
      EXPECT_EQ(3, crt.version);
      EXPECT_EQ(1, (int)crt.serial.len);
      EXPECT_EQ(1, (int)crt.serial.p[0]);
      char subject[256], issuer[256];
      mbedtls_x509_dn_gets(subject, sizeof(subject), &crt.subject);
      mbedtls_x509_dn_gets(issuer, sizeof(issuer), &crt.issuer);
      EXPECT_NE(NULL, strstr(subject, "mbedTLS4 Test"));
      EXPECT_NE(NULL, strstr(subject, "Cosmopolitan"));
      EXPECT_EQ(0, strcmp(subject, issuer));
      EXPECT_EQ(1, mbedtls_x509_crt_get_ca_istrue(&crt));
      EXPECT_NE(0, mbedtls_x509_crt_has_ext_type(&crt,
                       MBEDTLS_X509_EXT_BASIC_CONSTRAINTS));
      EXPECT_NE(0, mbedtls_x509_crt_has_ext_type(&crt,
                       MBEDTLS_X509_EXT_SUBJECT_KEY_IDENTIFIER));
      EXPECT_NE(0, mbedtls_x509_crt_has_ext_type(&crt,
                       MBEDTLS_X509_EXT_AUTHORITY_KEY_IDENTIFIER));
      EXPECT_EQ(256, (int)mbedtls_pk_get_bitlen(&crt.pk));
      uint32_t flags = 0;
      EXPECT_EQ(0, mbedtls_x509_crt_verify(&crt, &crt, NULL, NULL, &flags,
                                            NULL, NULL));
      EXPECT_EQ(0, (int)flags);
    }
    mbedtls_x509_crt_free(&crt);
  }
  mbedtls_pk_free(&pk);
  psa_destroy_key(key_id);
}

/* ── TLS context / config / ciphersuites ─────────────────────────────────── */

TEST(ssl, ctx) {
  mbedtls_ssl_context ssl;
  mbedtls_ssl_config conf;
  mbedtls_ssl_init(&ssl);
  mbedtls_ssl_config_init(&conf);
  EXPECT_EQ(0, mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_CLIENT,
                                            MBEDTLS_SSL_TRANSPORT_STREAM,
                                            MBEDTLS_SSL_PRESET_DEFAULT));
  mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
  EXPECT_EQ(0, mbedtls_ssl_setup(&ssl, &conf));
  const int *suites = mbedtls_ssl_list_ciphersuites();
  EXPECT_NE(NULL, suites);
  EXPECT_NE(0, suites ? suites[0] : 0);
  if (suites && suites[0]) {
    const char *name = mbedtls_ssl_get_ciphersuite_name(suites[0]);
    EXPECT_NE(NULL, name);
    EXPECT_EQ(suites[0], name ? mbedtls_ssl_get_ciphersuite_id(name) : 0);
  }
  EXPECT_NE(NULL, mbedtls_ssl_get_version(&ssl));
  EXPECT_EQ(NULL, mbedtls_ssl_get_ciphersuite(&ssl));
  EXPECT_EQ((uint32_t)-1, mbedtls_ssl_get_verify_result(&ssl));
  EXPECT_EQ(MBEDTLS_ERR_SSL_BAD_INPUT_DATA,
            mbedtls_ssl_get_fatal_alert(&ssl));
  mbedtls_ssl_config_free(&conf);
  mbedtls_ssl_free(&ssl);
}

/* ── Error formatting ────────────────────────────────────────────────────── */

TEST(mbedtls4, errors) {
  char ebuf[256];
  mbedtls_strerror(MBEDTLS_ERR_SSL_WANT_READ, ebuf, sizeof(ebuf));
  EXPECT_NE('\0', ebuf[0]);
  EXPECT_NE('?', ebuf[0]);
  mbedtls_strerror(0, ebuf, sizeof(ebuf)); /* must not crash */
}

/* ── Library version ─────────────────────────────────────────────────────── */

TEST(mbedtls4, version) {
  unsigned int ver_num = mbedtls_version_get_number();
  EXPECT_EQ(4, (int)((ver_num >> 24) & 0xff));
  const char *full = mbedtls_version_get_string_full();
  ASSERT_NE(NULL, full);
  EXPECT_NE(NULL, strstr(full, "4."));
}

/* ── mbedtls_md multi-step + HMAC ────────────────────────────────────────── */

TEST(md, multiStep) {
  const mbedtls_md_info_t *info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
  ASSERT_NE(NULL, info);

#if defined(MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS)
  const int *list = mbedtls_md_list();
  EXPECT_NE(NULL, list);
  EXPECT_NE(0, list ? list[0] : 0);
  EXPECT_EQ(info, mbedtls_md_info_from_string("SHA256"));
#endif

  EXPECT_EQ(MBEDTLS_MD_SHA256, (int)mbedtls_md_get_type(info));

  mbedtls_md_context_t ctx;
  mbedtls_md_init(&ctx);
  EXPECT_EQ(0, mbedtls_md_setup(&ctx, info, 0));
  EXPECT_EQ(0, mbedtls_md_starts(&ctx));
  EXPECT_EQ(0, mbedtls_md_update(&ctx, (const unsigned char *)kMsg,
                                   strlen(kMsg)));
  uint8_t digest[32];
  EXPECT_EQ(0, mbedtls_md_finish(&ctx, digest));
  EXPECT_EQ(0, memcmp(digest, kSha256, 32));

  /* md_clone: clone midway through another digest */
  mbedtls_md_context_t ctx2;
  mbedtls_md_init(&ctx2);
  EXPECT_EQ(0, mbedtls_md_setup(&ctx2, info, 0));
  EXPECT_EQ(0, mbedtls_md_starts(&ctx));
  EXPECT_EQ(0, mbedtls_md_update(&ctx, (const unsigned char *)kMsg,
                                   strlen(kMsg)));
  EXPECT_EQ(0, mbedtls_md_clone(&ctx2, &ctx));
  uint8_t digest2[32];
  EXPECT_EQ(0, mbedtls_md_finish(&ctx2, digest2));
  EXPECT_EQ(0, memcmp(digest2, kSha256, 32));
  mbedtls_md_free(&ctx2);
  mbedtls_md_free(&ctx);

#if defined(MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS)
  uint8_t hmac_out[32];
  EXPECT_EQ(0, mbedtls_md_hmac(info, kHmacKey, sizeof(kHmacKey) - 1,
                                 kHmacMsg, sizeof(kHmacMsg) - 1, hmac_out));
  EXPECT_EQ(0, memcmp(hmac_out, kHmacSha256, 32));
#endif
}

/* ── mbedtls_platform_zeroize ────────────────────────────────────────────── */

TEST(mbedtls4, platformZeroize) {
  unsigned char buf[64];
  memset(buf, 0xAB, sizeof(buf));
  mbedtls_platform_zeroize(buf, sizeof(buf));
  EXPECT_TRUE(AllZero(buf, sizeof(buf)));
}

/* ── PK extended: get_key_type, can_do_psa, write/parse/check_pair ───────── */

TEST(pk, extended) {
  psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
  psa_set_key_usage_flags(&attr,
      PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH |
      PSA_KEY_USAGE_EXPORT);
  psa_set_key_algorithm(&attr, PSA_ALG_ECDSA(PSA_ALG_SHA_256));
  psa_set_key_type(&attr, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
  psa_set_key_bits(&attr, 256);
  psa_key_id_t key_id = 0;
  ASSERT_EQ(PSA_SUCCESS, (int)psa_generate_key(&attr, &key_id));

  mbedtls_pk_context pk;
  mbedtls_pk_init(&pk);
  ASSERT_EQ(0, mbedtls_pk_copy_from_psa(key_id, &pk));

  psa_key_type_t ktype = mbedtls_pk_get_key_type(&pk);
  EXPECT_TRUE(PSA_KEY_TYPE_IS_ECC_KEY_PAIR(ktype));
  int can = mbedtls_pk_can_do_psa(&pk, PSA_ALG_ECDSA(PSA_ALG_SHA_256),
                                    PSA_KEY_USAGE_SIGN_HASH);
  EXPECT_TRUE(can == 0 || can == 1);

  static unsigned char pem_key[4096];
  EXPECT_EQ(0, mbedtls_pk_write_key_pem(&pk, pem_key, sizeof(pem_key)));
  EXPECT_EQ(0, strncmp((char *)pem_key, "-----BEGIN", 10));

  mbedtls_pk_context pk2;
  mbedtls_pk_init(&pk2);
  size_t pem_key_len = strlen((char *)pem_key) + 1;
  EXPECT_EQ(0, mbedtls_pk_parse_key(&pk2, pem_key, pem_key_len, NULL, 0));

  mbedtls_pk_context pub;
  mbedtls_pk_init(&pub);
  static unsigned char pub_pem[2048];
  if (mbedtls_pk_write_pubkey_pem(&pk2, pub_pem, sizeof(pub_pem)) == 0) {
    if (mbedtls_pk_parse_public_key(&pub, pub_pem,
                                    strlen((char *)pub_pem) + 1) == 0) {
      EXPECT_EQ(0, mbedtls_pk_check_pair(&pub, &pk2));
    }
  }
  mbedtls_pk_free(&pub);
  mbedtls_pk_free(&pk2);
  mbedtls_pk_free(&pk);
  psa_destroy_key(key_id);
}

/* ── X.509 check functions ───────────────────────────────────────────────── */

TEST(x509, checks) {
  mbedtls_x509_crt crt;
  mbedtls_x509_crt_init(&crt);
  ASSERT_EQ(0, mbedtls_x509_crt_parse(&crt,
                                       (const unsigned char *)kCaPem,
                                       sizeof(kCaPem)));

  char info_buf[2048];
  int info_rc = mbedtls_x509_crt_info(info_buf, sizeof(info_buf), "  ", &crt);
  EXPECT_NE(0, info_rc > 0 ? 1 : 0);
  EXPECT_NE('\0', info_buf[0]);

  EXPECT_EQ(0, mbedtls_x509_crt_check_key_usage(&crt,
                                                  MBEDTLS_X509_KU_KEY_CERT_SIGN));
  EXPECT_EQ(0, mbedtls_x509_crt_check_key_usage(&crt,
                                                  MBEDTLS_X509_KU_DIGITAL_SIGNATURE));
  int eku_rc = mbedtls_x509_crt_check_extended_key_usage(
      &crt, MBEDTLS_OID_SERVER_AUTH,
      MBEDTLS_OID_SIZE(MBEDTLS_OID_SERVER_AUTH));
  EXPECT_TRUE(eku_rc == 0 || eku_rc == MBEDTLS_ERR_X509_BAD_INPUT_DATA);
  EXPECT_EQ(0, mbedtls_x509_time_is_future(&crt.valid_from));
  EXPECT_EQ(0, mbedtls_x509_time_is_past(&crt.valid_to));
  mbedtls_x509_crt_free(&crt);
}

/* ── X.509 write: DER output, NS cert type, SAN ─────────────────────────── */

TEST(x509, writeExtended) {
  psa_key_attributes_t kattr = PSA_KEY_ATTRIBUTES_INIT;
  psa_set_key_usage_flags(&kattr,
      PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH |
      PSA_KEY_USAGE_EXPORT);
  psa_set_key_algorithm(&kattr, PSA_ALG_ECDSA(PSA_ALG_SHA_256));
  psa_set_key_type(&kattr, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
  psa_set_key_bits(&kattr, 256);
  psa_key_id_t key_id = 0;
  ASSERT_EQ(PSA_SUCCESS, (int)psa_generate_key(&kattr, &key_id));

  mbedtls_pk_context pk;
  mbedtls_pk_init(&pk);
  ASSERT_EQ(0, mbedtls_pk_copy_from_psa(key_id, &pk));

  mbedtls_x509write_cert wctx;
  mbedtls_x509write_crt_init(&wctx);
  mbedtls_x509write_crt_set_version(&wctx, MBEDTLS_X509_CRT_VERSION_3);
  static const unsigned char serial[] = {0x02};
  mbedtls_x509write_crt_set_serial_raw(&wctx, serial, sizeof(serial));
  mbedtls_x509write_crt_set_validity(&wctx, "20240101000000", "20340101000000");
  mbedtls_x509write_crt_set_subject_name(&wctx,
      "CN=mbedtls4.test,O=mbedtls4test,C=US");
  mbedtls_x509write_crt_set_issuer_name(&wctx,
      "CN=mbedtls4.test,O=mbedtls4test,C=US");
  mbedtls_x509write_crt_set_subject_key(&wctx, &pk);
  mbedtls_x509write_crt_set_issuer_key(&wctx, &pk);
  mbedtls_x509write_crt_set_md_alg(&wctx, MBEDTLS_MD_SHA256);
  mbedtls_x509write_crt_set_basic_constraints(&wctx, 0, -1);
  mbedtls_x509write_crt_set_subject_key_identifier(&wctx);
  mbedtls_x509write_crt_set_authority_key_identifier(&wctx);

  /* EKU: serverAuth OID */
  mbedtls_asn1_sequence eku;
  memset(&eku, 0, sizeof(eku));
  eku.buf.tag = MBEDTLS_ASN1_OID;
  eku.buf.p   = (unsigned char *)MBEDTLS_OID_SERVER_AUTH;
  eku.buf.len = MBEDTLS_OID_SIZE(MBEDTLS_OID_SERVER_AUTH);
  EXPECT_EQ(0, mbedtls_x509write_crt_set_ext_key_usage(&wctx, &eku));

  /* NS cert type */
  EXPECT_EQ(0, mbedtls_x509write_crt_set_ns_cert_type(&wctx,
                   MBEDTLS_X509_NS_CERT_TYPE_SSL_SERVER));

  /* SAN: DNS name */
  static const char kSanDns[] = "mbedtls4.test";
  mbedtls_x509_san_list san;
  memset(&san, 0, sizeof(san));
  san.node.type = MBEDTLS_X509_SAN_DNS_NAME;
  san.node.san.unstructured_name.tag = MBEDTLS_ASN1_IA5_STRING;
  san.node.san.unstructured_name.p   = (unsigned char *)kSanDns;
  san.node.san.unstructured_name.len = strlen(kSanDns);
  EXPECT_EQ(0, mbedtls_x509write_crt_set_subject_alternative_name(&wctx, &san));

  static unsigned char der_buf[2048];
  int der_len = mbedtls_x509write_crt_der(&wctx, der_buf, sizeof(der_buf));
  EXPECT_NE(0, der_len > 0 ? 1 : 0);

  static unsigned char pem_buf[4096];
  int rc = mbedtls_x509write_crt_pem(&wctx, pem_buf, sizeof(pem_buf));
  mbedtls_x509write_crt_free(&wctx);
  EXPECT_EQ(0, rc);
  if (rc == 0) {
    mbedtls_x509_crt crt;
    mbedtls_x509_crt_init(&crt);
    rc = mbedtls_x509_crt_parse(&crt, pem_buf, strlen((char *)pem_buf) + 1);
    EXPECT_EQ(0, rc);
    if (rc == 0) {
      EXPECT_NE(0, mbedtls_x509_crt_has_ext_type(&crt,
                       MBEDTLS_X509_EXT_SUBJECT_ALT_NAME));
    }
    mbedtls_x509_crt_free(&crt);
  }
  mbedtls_pk_free(&pk);
  psa_destroy_key(key_id);
}

/* ── SSL configuration functions ─────────────────────────────────────────── */

TEST(ssl, conf) {
  static unsigned char srv_cert_pem[4096];
  static unsigned char srv_key_pem[4096];
  ASSERT_EQ(0, GenSelfSignedCert("ssl-conf-test",
                                  (char *)srv_cert_pem, sizeof(srv_cert_pem),
                                  (char *)srv_key_pem,  sizeof(srv_key_pem)));

  mbedtls_x509_crt srv_crt;
  mbedtls_x509_crt_init(&srv_crt);
  ASSERT_EQ(0, mbedtls_x509_crt_parse(&srv_crt, srv_cert_pem,
                                       strlen((char *)srv_cert_pem) + 1));

  mbedtls_pk_context srv_pk;
  mbedtls_pk_init(&srv_pk);
  ASSERT_EQ(0, mbedtls_pk_parse_key(&srv_pk, srv_key_pem,
                                     strlen((char *)srv_key_pem) + 1,
                                     NULL, 0));

  mbedtls_ssl_config cfg;
  mbedtls_ssl_config_init(&cfg);
  EXPECT_EQ(0, mbedtls_ssl_config_defaults(&cfg, MBEDTLS_SSL_IS_SERVER,
                                            MBEDTLS_SSL_TRANSPORT_STREAM,
                                            MBEDTLS_SSL_PRESET_DEFAULT));
  mbedtls_ssl_conf_dbg(&cfg, debug_noop, NULL);
  EXPECT_EQ(0, mbedtls_ssl_conf_own_cert(&cfg, &srv_crt, &srv_pk));
  mbedtls_ssl_conf_ca_chain(&cfg, &srv_crt, NULL);

  const int *suites = mbedtls_ssl_list_ciphersuites();
  static int custom_suites[2];
  custom_suites[0] = suites ? suites[0] : 0;
  custom_suites[1] = 0;
  if (custom_suites[0])
    mbedtls_ssl_conf_ciphersuites(&cfg, custom_suites);

  static const char *alpn_protos[] = {"h2", "http/1.1", NULL};
  EXPECT_EQ(0, mbedtls_ssl_conf_alpn_protocols(&cfg, alpn_protos));

  static const unsigned char psk_key[16] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
  };
  static const unsigned char psk_id[] = "test-psk";
  EXPECT_EQ(0, mbedtls_ssl_conf_psk(&cfg, psk_key, sizeof(psk_key),
                                     psk_id, sizeof(psk_id) - 1));
  mbedtls_ssl_conf_psk_cb(&cfg, dummy_psk_cb, NULL);

  SniCtx sni_ctx = {&srv_crt, &srv_pk};
  mbedtls_ssl_conf_sni(&cfg, loopback_sni_cb, &sni_ctx);

  mbedtls_ssl_context ssl;
  mbedtls_ssl_init(&ssl);
  EXPECT_EQ(0, mbedtls_ssl_setup(&ssl, &cfg));
  EXPECT_EQ(0, mbedtls_ssl_set_hostname(&ssl, "example.com"));
  static int dummy_bio_fd = -1;
  mbedtls_ssl_set_bio(&ssl, &dummy_bio_fd, loopback_send, loopback_recv, NULL);
  EXPECT_EQ(0, mbedtls_ssl_set_hs_psk(&ssl, psk_key, sizeof(psk_key)));
  EXPECT_EQ(0, mbedtls_ssl_session_reset(&ssl));

  mbedtls_ssl_free(&ssl);
  mbedtls_ssl_config_free(&cfg);
  mbedtls_x509_crt_free(&srv_crt);
  mbedtls_pk_free(&srv_pk);
}

/* ── SSL session tickets ─────────────────────────────────────────────────── */

TEST(ssl, tickets) {
  mbedtls_ssl_ticket_context tkt;
  mbedtls_ssl_ticket_init(&tkt);
  int rc = mbedtls_ssl_ticket_setup(&tkt, PSA_ALG_GCM, PSA_KEY_TYPE_AES,
                                    128, 86400);
  EXPECT_EQ(0, rc);

  mbedtls_ssl_config cfg;
  mbedtls_ssl_config_init(&cfg);
  mbedtls_ssl_config_defaults(&cfg, MBEDTLS_SSL_IS_SERVER,
                               MBEDTLS_SSL_TRANSPORT_STREAM,
                               MBEDTLS_SSL_PRESET_DEFAULT);
#if defined(MBEDTLS_SSL_SESSION_TICKETS) && defined(MBEDTLS_SSL_SRV_C)
  mbedtls_ssl_conf_session_tickets_cb(&cfg, mbedtls_ssl_ticket_write,
                                      mbedtls_ssl_ticket_parse, &tkt);
#endif
  if (rc == 0)
    mbedtls_ssl_ticket_free(&tkt);
  mbedtls_ssl_config_free(&cfg);
}

/* ── TLS loopback: handshake, read, write, close_notify, get_peer_cert ───── */

TEST(ssl, loopback) {
  static char cert_pem[4096];
  static char key_pem[4096];
  ASSERT_EQ(0, GenSelfSignedCert("loopback.test", cert_pem, sizeof(cert_pem),
                                  key_pem, sizeof(key_pem)));

  int sv[2];
  ASSERT_EQ(0, socketpair(AF_UNIX, SOCK_STREAM, 0, sv));

  pid_t child = fork();
  if (child < 0) {
    close(sv[0]);
    close(sv[1]);
    ASSERT_NE((pid_t)-1, child); /* force failure */
  }

  if (child == 0) {
    /* ── CLIENT (child process) ── */
    close(sv[0]);
    int fd = sv[1];

    mbedtls_x509_crt ca_crt;
    mbedtls_x509_crt_init(&ca_crt);
    if (mbedtls_x509_crt_parse(&ca_crt, (const unsigned char *)cert_pem,
                                strlen(cert_pem) + 1) != 0) {
      mbedtls_x509_crt_free(&ca_crt);
      close(fd);
      _exit(1);
    }

    mbedtls_ssl_config cli_cfg;
    mbedtls_ssl_config_init(&cli_cfg);
    mbedtls_ssl_config_defaults(&cli_cfg, MBEDTLS_SSL_IS_CLIENT,
                                 MBEDTLS_SSL_TRANSPORT_STREAM,
                                 MBEDTLS_SSL_PRESET_DEFAULT);
    mbedtls_ssl_conf_authmode(&cli_cfg, MBEDTLS_SSL_VERIFY_REQUIRED);
    mbedtls_ssl_conf_ca_chain(&cli_cfg, &ca_crt, NULL);

    mbedtls_ssl_context cli_ssl;
    mbedtls_ssl_init(&cli_ssl);
    if (mbedtls_ssl_setup(&cli_ssl, &cli_cfg) != 0 ||
        mbedtls_ssl_set_hostname(&cli_ssl, "loopback.test") != 0) {
      mbedtls_ssl_free(&cli_ssl);
      mbedtls_ssl_config_free(&cli_cfg);
      mbedtls_x509_crt_free(&ca_crt);
      close(fd);
      _exit(1);
    }
    mbedtls_ssl_set_bio(&cli_ssl, &fd, loopback_send, loopback_recv, NULL);

    int hs;
    do { hs = mbedtls_ssl_handshake(&cli_ssl); }
    while (hs == MBEDTLS_ERR_SSL_WANT_READ || hs == MBEDTLS_ERR_SSL_WANT_WRITE);
    if (hs != 0 || !mbedtls_ssl_get_peer_cert(&cli_ssl)) {
      mbedtls_ssl_free(&cli_ssl);
      mbedtls_ssl_config_free(&cli_cfg);
      mbedtls_x509_crt_free(&ca_crt);
      close(fd);
      _exit(1);
    }

    static const unsigned char ping[] = "PING";
    int wr;
    do { wr = mbedtls_ssl_write(&cli_ssl, ping, 4); }
    while (wr == MBEDTLS_ERR_SSL_WANT_WRITE);
    if (wr != 4) {
      mbedtls_ssl_free(&cli_ssl);
      mbedtls_ssl_config_free(&cli_cfg);
      mbedtls_x509_crt_free(&ca_crt);
      close(fd);
      _exit(1);
    }

    unsigned char rbuf[8];
    int rd;
    do { rd = mbedtls_ssl_read(&cli_ssl, rbuf, sizeof(rbuf)); }
    while (rd == MBEDTLS_ERR_SSL_WANT_READ);
    if (rd != 4 || memcmp(rbuf, "PONG", 4) != 0) {
      mbedtls_ssl_free(&cli_ssl);
      mbedtls_ssl_config_free(&cli_cfg);
      mbedtls_x509_crt_free(&ca_crt);
      close(fd);
      _exit(1);
    }

    int cn;
    do { cn = mbedtls_ssl_close_notify(&cli_ssl); }
    while (cn == MBEDTLS_ERR_SSL_WANT_WRITE);
    /* Drain server's close_notify so it doesn't get EPIPE. */
    unsigned char drain[64];
    int dr;
    do { dr = mbedtls_ssl_read(&cli_ssl, drain, sizeof(drain)); }
    while (dr > 0 || dr == MBEDTLS_ERR_SSL_WANT_READ);

    mbedtls_ssl_free(&cli_ssl);
    mbedtls_ssl_config_free(&cli_cfg);
    mbedtls_x509_crt_free(&ca_crt);
    close(fd);
    _exit(0);
  }

  /* ── SERVER (parent process) ── */
  close(sv[1]);
  int fd = sv[0];

  mbedtls_x509_crt srv_crt;
  mbedtls_x509_crt_init(&srv_crt);
  mbedtls_pk_context srv_pk;
  mbedtls_pk_init(&srv_pk);

  int srv_ok =
      mbedtls_x509_crt_parse(&srv_crt, (const unsigned char *)cert_pem,
                              strlen(cert_pem) + 1) == 0 &&
      mbedtls_pk_parse_key(&srv_pk, (const unsigned char *)key_pem,
                            strlen(key_pem) + 1, NULL, 0) == 0;

  SniCtx sni_ctx = {&srv_crt, &srv_pk};
  mbedtls_ssl_config srv_cfg;
  mbedtls_ssl_config_init(&srv_cfg);
  if (srv_ok) {
    mbedtls_ssl_config_defaults(&srv_cfg, MBEDTLS_SSL_IS_SERVER,
                                 MBEDTLS_SSL_TRANSPORT_STREAM,
                                 MBEDTLS_SSL_PRESET_DEFAULT);
    mbedtls_ssl_conf_sni(&srv_cfg, loopback_sni_cb, &sni_ctx);
    mbedtls_ssl_conf_own_cert(&srv_cfg, &srv_crt, &srv_pk);
    mbedtls_ssl_conf_authmode(&srv_cfg, MBEDTLS_SSL_VERIFY_NONE);
  }

  mbedtls_ssl_context srv_ssl;
  mbedtls_ssl_init(&srv_ssl);
  int setup_ok = srv_ok && mbedtls_ssl_setup(&srv_ssl, &srv_cfg) == 0;
  if (setup_ok)
    mbedtls_ssl_set_bio(&srv_ssl, &fd, loopback_send, loopback_recv, NULL);

  int hs_rc = -1;
  if (setup_ok) {
    do { hs_rc = mbedtls_ssl_handshake(&srv_ssl); }
    while (hs_rc == MBEDTLS_ERR_SSL_WANT_READ ||
           hs_rc == MBEDTLS_ERR_SSL_WANT_WRITE);
  }
  EXPECT_EQ(0, hs_rc);

  int read_ok = 0;
  if (hs_rc == 0) {
    unsigned char rbuf[8];
    int rd;
    do { rd = mbedtls_ssl_read(&srv_ssl, rbuf, sizeof(rbuf)); }
    while (rd == MBEDTLS_ERR_SSL_WANT_READ);
    read_ok = (rd == 4 && memcmp(rbuf, "PING", 4) == 0);
  }
  EXPECT_TRUE(hs_rc == 0 ? read_ok : 1);

  int write_ok = 0;
  if (read_ok) {
    static const unsigned char pong[] = "PONG";
    int wr;
    do { wr = mbedtls_ssl_write(&srv_ssl, pong, 4); }
    while (wr == MBEDTLS_ERR_SSL_WANT_WRITE);
    write_ok = (wr == 4);
  }
  EXPECT_TRUE(hs_rc == 0 ? write_ok : 1);

  int cn_rc = -1;
  if (hs_rc == 0) {
    do { cn_rc = mbedtls_ssl_close_notify(&srv_ssl); }
    while (cn_rc == MBEDTLS_ERR_SSL_WANT_WRITE);
  }
  EXPECT_TRUE(hs_rc == 0 ? (cn_rc == 0) : 1);

  mbedtls_ssl_free(&srv_ssl);
  mbedtls_ssl_config_free(&srv_cfg);
  mbedtls_x509_crt_free(&srv_crt);
  mbedtls_pk_free(&srv_pk);
  /* Keep fd open until child exits to avoid SIGPIPE on close_notify. */
  int status = 0;
  waitpid(child, &status, 0);
  close(fd);
  EXPECT_TRUE(WIFEXITED(status));
  EXPECT_EQ(0, WEXITSTATUS(status));
}

/* ── Additional hash tests (from mbedtls_test.c, adapted for mbedtls4) ───── */

TEST(hash, sha1) {
  uint8_t digest[20];
  size_t digest_len = 0;
  EXPECT_EQ(PSA_SUCCESS,
            (int)psa_hash_compute(PSA_ALG_SHA_1,
                                  (const uint8_t *)"abc", 3,
                                  digest, sizeof(digest), &digest_len));
  EXPECT_EQ(20, (int)digest_len);
  EXPECT_EQ(0, memcmp(digest, kAbsSha1, sizeof(kAbsSha1)));
}

TEST(hash, sha224) {
  uint8_t digest[28];
  size_t digest_len = 0;
  EXPECT_EQ(PSA_SUCCESS,
            (int)psa_hash_compute(PSA_ALG_SHA_224,
                                  (const uint8_t *)"abc", 3,
                                  digest, sizeof(digest), &digest_len));
  EXPECT_EQ(28, (int)digest_len);
  EXPECT_EQ(0, memcmp(digest, kAbsSha224, sizeof(kAbsSha224)));
}

TEST(hash, sha384) {
  uint8_t digest[48];
  size_t digest_len = 0;
  EXPECT_EQ(PSA_SUCCESS,
            (int)psa_hash_compute(PSA_ALG_SHA_384,
                                  (const uint8_t *)"abc", 3,
                                  digest, sizeof(digest), &digest_len));
  EXPECT_EQ(48, (int)digest_len);
  EXPECT_EQ(0, memcmp(digest, kAbsSha384, sizeof(kAbsSha384)));
}

TEST(hash, sha512) {
  uint8_t digest[64];
  size_t digest_len = 0;
  EXPECT_EQ(PSA_SUCCESS,
            (int)psa_hash_compute(PSA_ALG_SHA_512,
                                  (const uint8_t *)"abc", 3,
                                  digest, sizeof(digest), &digest_len));
  EXPECT_EQ(64, (int)digest_len);
  EXPECT_EQ(0, memcmp(digest, kAbsSha512, sizeof(kAbsSha512)));
}

TEST(hash, md5) {
  const mbedtls_md_info_t *info = mbedtls_md_info_from_type(MBEDTLS_MD_MD5);
  ASSERT_NE(NULL, info);
  EXPECT_EQ(16, (int)mbedtls_md_get_size(info));
  uint8_t digest[16];
  EXPECT_EQ(0, mbedtls_md(info, (const unsigned char *)"abc", 3, digest));
  EXPECT_EQ(0, memcmp(digest, kAbsMd5, sizeof(kAbsMd5)));
}

TEST(hash, pbkdf2) {
  /* RFC 6070 test vector 1:
   * PBKDF2-HMAC-SHA1(password, salt, c=1, dkLen=20) */
  psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
  psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_DERIVE);
  psa_set_key_algorithm(&attr, PSA_ALG_PBKDF2_HMAC(PSA_ALG_SHA_1));
  psa_set_key_type(&attr, PSA_KEY_TYPE_PASSWORD);
  psa_key_id_t key_id = 0;
  ASSERT_EQ(PSA_SUCCESS,
            (int)psa_import_key(&attr,
                                 (const uint8_t *)"password",
                                 sizeof("password") - 1,
                                 &key_id));

  psa_key_derivation_operation_t op = PSA_KEY_DERIVATION_OPERATION_INIT;
  EXPECT_EQ(PSA_SUCCESS,
            (int)psa_key_derivation_setup(&op,
                                          PSA_ALG_PBKDF2_HMAC(PSA_ALG_SHA_1)));
  EXPECT_EQ(PSA_SUCCESS,
            (int)psa_key_derivation_input_integer(
                &op, PSA_KEY_DERIVATION_INPUT_COST, 1));
  EXPECT_EQ(PSA_SUCCESS,
            (int)psa_key_derivation_input_bytes(
                &op, PSA_KEY_DERIVATION_INPUT_SALT,
                (const uint8_t *)"salt", 4));
  EXPECT_EQ(PSA_SUCCESS,
            (int)psa_key_derivation_input_key(
                &op, PSA_KEY_DERIVATION_INPUT_PASSWORD, key_id));

  uint8_t dk[20];
  EXPECT_EQ(PSA_SUCCESS, (int)psa_key_derivation_output_bytes(&op, dk, sizeof(dk)));
  EXPECT_EQ(0, memcmp(dk, kPbkdf2Dk, sizeof(kPbkdf2Dk)));

  psa_key_derivation_abort(&op);
  psa_destroy_key(key_id);
}

/* ── MPI bignum ──────────────────────────────────────────────────────────── */

TEST(mpi_shift_r, doesntCrash) {
  mbedtls_mpi x;
  mbedtls_mpi_init(&x);
  EXPECT_EQ(0, mbedtls_mpi_shift_r(&x, 1));
  mbedtls_mpi_free(&x);
}

TEST(mpi_shift_l, doesntCrash) {
  mbedtls_mpi x;
  mbedtls_mpi_init(&x);
  EXPECT_EQ(0, mbedtls_mpi_shift_l(&x, 1));
  mbedtls_mpi_free(&x);
}

TEST(mpi_shift_r, fun0) {
  mbedtls_mpi x;
  mbedtls_mpi_init(&x);
  ASSERT_EQ(0, mbedtls_mpi_lset(&x, 2));
  EXPECT_EQ(0, mbedtls_mpi_shift_r(&x, 1));
  EXPECT_EQ(1, (int)x.n);
  EXPECT_EQ(1, (int)x.p[0]);
  mbedtls_mpi_free(&x);
}

TEST(mpi_shift_r, fun1) {
  mbedtls_mpi x;
  mbedtls_mpi_init(&x);
  ASSERT_EQ(0, mbedtls_mpi_grow(&x, 7));
  x.p[0] = 2;   x.p[1] = 4;   x.p[2] = 8;
  x.p[3] = 16;  x.p[4] = 32;  x.p[5] = 64; x.p[6] = 128;
  EXPECT_EQ(0, mbedtls_mpi_shift_r(&x, 129));
  EXPECT_EQ(7,  (int)x.n);
  EXPECT_EQ(4,  (int)x.p[0]);
  EXPECT_EQ(8,  (int)x.p[1]);
  EXPECT_EQ(16, (int)x.p[2]);
  EXPECT_EQ(32, (int)x.p[3]);
  EXPECT_EQ(64, (int)x.p[4]);
  EXPECT_EQ(0,  (int)x.p[5]);
  EXPECT_EQ(0,  (int)x.p[6]);
  mbedtls_mpi_free(&x);
}

TEST(mpi_shift_r, fun2) {
  mbedtls_mpi x;
  mbedtls_mpi_init(&x);
  ASSERT_EQ(0, mbedtls_mpi_grow(&x, 3));
  x.p[0] = 0; x.p[1] = 1; x.p[2] = 0;
  EXPECT_EQ(0, mbedtls_mpi_shift_r(&x, 1));
  EXPECT_EQ(3, (int)x.n);
  EXPECT_EQ(0x8000000000000000ULL, x.p[0]);
  EXPECT_EQ(0, (int)x.p[1]);
  EXPECT_EQ(0, (int)x.p[2]);
  mbedtls_mpi_free(&x);
}

TEST(mpi_shift_l, fun0) {
  mbedtls_mpi x;
  mbedtls_mpi_init(&x);
  ASSERT_EQ(0, mbedtls_mpi_lset(&x, 2));
  EXPECT_EQ(0, mbedtls_mpi_shift_l(&x, 1));
  EXPECT_EQ(1, (int)x.n);
  EXPECT_EQ(4, (int)x.p[0]);
  mbedtls_mpi_free(&x);
}

TEST(mpi_shift_r, funbye) {
  mbedtls_mpi x;
  mbedtls_mpi_init(&x);
  ASSERT_EQ(0, mbedtls_mpi_lset(&x, 2));
  EXPECT_EQ(0, mbedtls_mpi_shift_r(&x, 100));
  EXPECT_EQ(1, (int)x.n);
  EXPECT_EQ(0, (int)x.p[0]);
  mbedtls_mpi_free(&x);
}

TEST(mpi_shift_l, fun1) {
  mbedtls_mpi x;
  mbedtls_mpi_init(&x);
  ASSERT_EQ(0, mbedtls_mpi_grow(&x, 9));
  x.p[0] = 2;   x.p[1] = 4;   x.p[2] = 8;
  x.p[3] = 16;  x.p[4] = 32;  x.p[5] = 64; x.p[6] = 128;
  x.p[7] = 0;   x.p[8] = 0;
  EXPECT_EQ(0, mbedtls_mpi_shift_l(&x, 129));
  EXPECT_EQ(9,        (int)x.n);
  EXPECT_EQ(0,        (int)x.p[0]);
  EXPECT_EQ(0,        (int)x.p[1]);
  EXPECT_EQ(2 << 1,   (int)x.p[2]);
  EXPECT_EQ(4 << 1,   (int)x.p[3]);
  EXPECT_EQ(8 << 1,   (int)x.p[4]);
  EXPECT_EQ(16 << 1,  (int)x.p[5]);
  EXPECT_EQ(32 << 1,  (int)x.p[6]);
  EXPECT_EQ(64 << 1,  (int)x.p[7]);
  EXPECT_EQ(128 << 1, (int)x.p[8]);
  mbedtls_mpi_free(&x);
}

TEST(mpi_shift_l, fun2) {
  mbedtls_mpi x;
  mbedtls_mpi_init(&x);
  ASSERT_EQ(0, mbedtls_mpi_grow(&x, 3));
  x.p[0] = 0x8000000000000003ULL;
  x.p[1] = 0x8000000000000002ULL;
  x.p[2] = 0x0000000000000001ULL;
  EXPECT_EQ(0, mbedtls_mpi_shift_l(&x, 1));
  EXPECT_EQ(3, (int)x.n);
  EXPECT_EQ(6, (int)x.p[0]);
  EXPECT_EQ(5, (int)x.p[1]);
  EXPECT_EQ(3, (int)x.p[2]);
  mbedtls_mpi_free(&x);
}

/* ── MPI endian ──────────────────────────────────────────────────────────── */

TEST(endian, big1) {
  uint8_t b[] = {
      0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07,
      0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x03,
  };
  mbedtls_mpi x;
  mbedtls_mpi_init(&x);
  ASSERT_EQ(0, mbedtls_mpi_read_binary(&x, b, ARRAYLEN(b)));
  EXPECT_GE((int)x.n, 3);
  EXPECT_EQ(0x0000000000000003ULL, x.p[0]);
  EXPECT_EQ(0x0000000000000702ULL, x.p[1]);
  EXPECT_EQ(0x0000000000000001ULL, x.p[2]);
  mbedtls_mpi_free(&x);
}

TEST(endian, big2) {
  uint8_t b[] = {0x01};
  mbedtls_mpi x;
  mbedtls_mpi_init(&x);
  ASSERT_EQ(0, mbedtls_mpi_read_binary(&x, b, ARRAYLEN(b)));
  EXPECT_EQ(1, (int)x.n);
  EXPECT_EQ(0x0000000000000001ULL, x.p[0]);
  mbedtls_mpi_free(&x);
}

TEST(endian, big3) {
  uint8_t b[] = {
      0x01, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
      0x01, 0x02,
  };
  mbedtls_mpi x;
  mbedtls_mpi_init(&x);
  ASSERT_EQ(0, mbedtls_mpi_read_binary(&x, b, ARRAYLEN(b)));
  EXPECT_EQ(2, (int)x.n);
  EXPECT_EQ(0x0101010101010102ULL, x.p[0]);
  EXPECT_EQ(0x0000000000000102ULL, x.p[1]);
  mbedtls_mpi_free(&x);
}

TEST(endian, big4) {
  uint8_t b[] = {
      0x11, 0x68, 0x5b, 0xb5, 0x76, 0x6f, 0xb5, 0x72,
      0x43, 0xd2, 0x3f, 0xd6, 0xc0, 0x1b, 0xa3, 0x2e,
      0x40, 0x77, 0x12, 0xc8, 0x59, 0x4e, 0x63, 0xab,
      0xea, 0xeb, 0x4a, 0x58, 0x50, 0xbd, 0xed, 0x30,
      0x10, 0x76, 0xa9, 0xfa, 0x01, 0xa1, 0x07, 0xe8,
      0xa3, 0xd5, 0xaf, 0x4e, 0x1f, 0xf6, 0xaf,
  };
  mbedtls_mpi x;
  mbedtls_mpi_init(&x);
  ASSERT_EQ(0, mbedtls_mpi_read_binary(&x, b, ARRAYLEN(b)));
  EXPECT_GE((int)x.n, 6);
  EXPECT_EQ(0xe8a3d5af4e1ff6afULL, x.p[0]);
  EXPECT_EQ(0x301076a9fa01a107ULL, x.p[1]);
  EXPECT_EQ(0xabeaeb4a5850bdedULL, x.p[2]);
  EXPECT_EQ(0x2e407712c8594e63ULL, x.p[3]);
  EXPECT_EQ(0x7243d23fd6c01ba3ULL, x.p[4]);
  mbedtls_mpi_free(&x);
}
