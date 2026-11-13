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
#include "libc/serialize.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"
#include "net/https/https.h"
#include "third_party/mbedtls/asn1.h"
#include "third_party/mbedtls/san.h"

static const uint8_t kV6[16] = {0x20, 0x01, 0x0d, 0xb8, 0, 0, 0, 0,
                                0,    0,    0,    0,    0, 0, 0, 1};
static const uint8_t kV6Other[16] = {0x20, 0x01, 0x0d, 0xb8, 0, 0, 0, 0,
                                     0,    0,    0,    0,    0, 0, 0, 2};

static void SetSan(mbedtls_x509_crt *crt, const void *p, size_t len) {
  mbedtls_x509_crt_init(crt);
  crt->subject_alt_names.buf.tag =
      MBEDTLS_ASN1_CONTEXT_SPECIFIC | MBEDTLS_X509_SAN_IP_ADDRESS;
  crt->subject_alt_names.buf.p = (unsigned char *)p;
  crt->subject_alt_names.buf.len = len;
  crt->subject_alt_names.next = 0;
}

TEST(CertHasIp6, matchesExactAddress) {
  mbedtls_x509_crt crt;
  SetSan(&crt, kV6, sizeof(kV6));
  EXPECT_TRUE(CertHasIp6(&crt, kV6));
  EXPECT_FALSE(CertHasIp6(&crt, kV6Other));
}

TEST(CertHasIp6, ipv4SanDoesNotMatchIpv6) {
  uint8_t ip4[4] = {127, 0, 0, 1};
  mbedtls_x509_crt crt;
  SetSan(&crt, ip4, sizeof(ip4));
  EXPECT_FALSE(CertHasIp6(&crt, kV6));
  EXPECT_TRUE(CertHasIp(&crt, 0x7f000001u));
}

TEST(CertHasIp, ipv6SanDoesNotMatchIpv4) {
  mbedtls_x509_crt crt;
  SetSan(&crt, kV6, sizeof(kV6));
  EXPECT_FALSE(CertHasIp(&crt, READ32BE(kV6)));
}

TEST(CertHasIp6, otherLengthsDoNotMatch) {
  uint8_t shortbuf[15] = {0};
  mbedtls_x509_crt crt;
  SetSan(&crt, shortbuf, sizeof(shortbuf));
  EXPECT_FALSE(CertHasIp6(&crt, kV6));
}

TEST(mbedtls_x509_parse_subject_alt_name, acceptsIp6Length) {
  mbedtls_x509_buf buf;
  mbedtls_x509_subject_alternative_name san;
  buf.tag = MBEDTLS_ASN1_CONTEXT_SPECIFIC | MBEDTLS_X509_SAN_IP_ADDRESS;
  buf.p = (unsigned char *)kV6;
  buf.len = sizeof(kV6);
  ASSERT_EQ(0, mbedtls_x509_parse_subject_alt_name(&buf, &san));
  EXPECT_EQ(MBEDTLS_X509_SAN_IP_ADDRESS, san.type);
}

TEST(mbedtls_x509_parse_subject_alt_name, shortIpAddressIsSafe) {
  mbedtls_x509_buf buf;
  mbedtls_x509_subject_alternative_name san;
  buf.tag = MBEDTLS_ASN1_CONTEXT_SPECIFIC | MBEDTLS_X509_SAN_IP_ADDRESS;
  buf.p = (unsigned char *)kV6;
  buf.len = 1;
  ASSERT_EQ(0, mbedtls_x509_parse_subject_alt_name(&buf, &san));
  EXPECT_EQ(MBEDTLS_X509_SAN_IP_ADDRESS, san.type);
}

TEST(mbedtls_x509write_crt_set_subject_alternative_name,
     writesIp6TagAndLength) {
  unsigned char *p, *end;
  size_t len;
  mbedtls_x509write_cert wc;
  struct mbedtls_san san[2];
  uint32_t ip4 = 0x04030201;
  mbedtls_x509write_crt_init(&wc);
  san[0].tag = MBEDTLS_X509_SAN_IP6_ADDRESS_INTERNAL;
  memcpy(san[0].ip6, kV6, sizeof(kV6));
  san[1].tag = MBEDTLS_X509_SAN_IP_ADDRESS;
  san[1].ip4 = ip4;
  ASSERT_EQ(0, mbedtls_x509write_crt_set_subject_alternative_name(&wc, san, 2));
  ASSERT_NE(NULL, wc.extensions);
  p = wc.extensions->val.p + 1;
  end = wc.extensions->val.p + wc.extensions->val.len;
  ASSERT_EQ(
      0, mbedtls_asn1_get_tag(
             &p, end, &len, MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE));
  ASSERT_EQ(0, mbedtls_asn1_get_tag(&p, end, &len,
                                    MBEDTLS_ASN1_CONTEXT_SPECIFIC |
                                        MBEDTLS_X509_SAN_IP_ADDRESS));
  EXPECT_EQ(16, len);
  EXPECT_EQ(0, memcmp(p, kV6, sizeof(kV6)));
  p += len;
  ASSERT_EQ(0, mbedtls_asn1_get_tag(&p, end, &len,
                                    MBEDTLS_ASN1_CONTEXT_SPECIFIC |
                                        MBEDTLS_X509_SAN_IP_ADDRESS));
  EXPECT_EQ(4, len);
  EXPECT_EQ(0, memcmp(p, "\x04\x03\x02\x01", 4));
  p += len;
  EXPECT_EQ(end, p);
  mbedtls_x509write_crt_free(&wc);
}

static struct Cert MakeCertificateWithSans(void) {
  mbedtls_x509write_cert wc;
  struct mbedtls_san san[2];
  mbedtls_pk_context *key;
  char notbefore[16], notafter[16];
  key = InitializeKey(0, &wc, MBEDTLS_MD_SHA256, MBEDTLS_PK_ECKEY);
  ASSERT_EQ(0, mbedtls_ecp_gen_key(MBEDTLS_ECP_DP_SECP256R1,
                                   mbedtls_pk_ec(*key), GenerateHardRandom, 0));
  GenerateCertificateSerial(&wc);
  san[0].tag = MBEDTLS_X509_SAN_IP6_ADDRESS_INTERNAL;
  memcpy(san[0].ip6, kV6, sizeof(kV6));
  san[1].tag = MBEDTLS_X509_SAN_IP_ADDRESS;
  san[1].ip4 = 0xc0000201u;
  ASSERT_EQ(0, mbedtls_x509write_crt_set_subject_alternative_name(&wc, san, 2));
  ChooseCertificateLifetime(notbefore, notafter);
  ASSERT_EQ(0, mbedtls_x509write_crt_set_validity(&wc, notbefore, notafter));
  ASSERT_EQ(0, mbedtls_x509write_crt_set_basic_constraints(&wc, 0, -1));
  ASSERT_EQ(0, mbedtls_x509write_crt_set_subject_key_identifier(&wc));
  ASSERT_EQ(0, mbedtls_x509write_crt_set_authority_key_identifier(&wc));
  ASSERT_EQ(0, mbedtls_x509write_crt_set_key_usage(
                   &wc, MBEDTLS_X509_KU_DIGITAL_SIGNATURE));
  ASSERT_EQ(0, mbedtls_x509write_crt_set_ext_key_usage(
                   &wc, MBEDTLS_X509_NS_CERT_TYPE_SSL_SERVER));
  ASSERT_EQ(0, mbedtls_x509write_crt_set_subject_name(&wc, "CN=example.com"));
  ASSERT_EQ(0, mbedtls_x509write_crt_set_issuer_name(&wc, "CN=example.com"));
  return FinishCertificate(0, &wc, key);
}

TEST(CertHasIp6, roundTripsThroughDerAndVerifiesNames) {
  uint32_t flags;
  struct Cert c;
  c = MakeCertificateWithSans();
  ASSERT_NE(NULL, c.cert);
  ASSERT_EQ(MBEDTLS_X509_EXT_SUBJECT_ALT_NAME,
            c.cert->ext_types & MBEDTLS_X509_EXT_SUBJECT_ALT_NAME);
  EXPECT_TRUE(CertHasIp6(c.cert, kV6));
  EXPECT_FALSE(CertHasIp6(c.cert, kV6Other));
  EXPECT_TRUE(CertHasIp(c.cert, 0xc0000201u));
  flags = 0;
  ASSERT_EQ(0, mbedtls_x509_crt_verify(c.cert, c.cert, 0, "2001:db8::1", &flags,
                                       0, 0));
  flags = 0;
  EXPECT_NE(0, mbedtls_x509_crt_verify(c.cert, c.cert, 0, "2001:db8::2", &flags,
                                       0, 0));
  EXPECT_NE(0, flags & MBEDTLS_X509_BADCERT_CN_MISMATCH);
  mbedtls_x509_crt_free(c.cert);
  free(c.cert);
  mbedtls_pk_free(c.key);
  free(c.key);
}
