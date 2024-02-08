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
#include "libc/intrin/bswap.h"
#include "libc/macros.internal.h"
#include "libc/mem/gc.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/nt/typedef/imagetlscallback.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/str/blake2.h"
#include "libc/str/highwayhash64.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"
#include "libc/x/xasprintf.h"
#include "third_party/mbedtls/aes.h"
#include "third_party/mbedtls/base64.h"
#include "third_party/mbedtls/bignum.h"
#include "third_party/mbedtls/bignum_internal.h"
#include "third_party/mbedtls/chacha20.h"
#include "third_party/mbedtls/chachapoly.h"
#include "third_party/mbedtls/ctr_drbg.h"
#include "third_party/mbedtls/des.h"
#include "third_party/mbedtls/dhm.h"
#include "third_party/mbedtls/ecp.h"
#include "third_party/mbedtls/ecp_internal.h"
#include "third_party/mbedtls/entropy.h"
#include "third_party/mbedtls/error.h"
#include "third_party/mbedtls/gcm.h"
#include "third_party/mbedtls/hmac_drbg.h"
#include "third_party/mbedtls/math.h"
#include "third_party/mbedtls/md5.h"
#include "third_party/mbedtls/memory_buffer_alloc.h"
#include "third_party/mbedtls/nist_kw.h"
#include "third_party/mbedtls/pkcs5.h"
#include "third_party/mbedtls/poly1305.h"
#include "third_party/mbedtls/profile.h"
#include "third_party/mbedtls/rsa.h"
#include "third_party/mbedtls/select.h"
#include "third_party/mbedtls/sha1.h"
#include "third_party/mbedtls/sha256.h"
#include "third_party/mbedtls/sha512.h"
#include "third_party/mbedtls/x509.h"
#include "third_party/zlib/zlib.h"

uint64_t rng[12];
mbedtls_ecp_group grp;

int GetEntropy(void *c, unsigned char *p, size_t n) {
  rngset(p, n, _rand64, -1);
  return 0;
}

void SetUp(void) {
  rngset(rng, sizeof(rng), _rand64, -1);
}

#ifdef MBEDTLS_SELF_TEST
TEST(mbedtls, selfTest) {
#ifdef MBEDTLS_DES_C
  EXPECT_EQ(0, mbedtls_des_self_test(0));
#endif
#ifdef MBEDTLS_CTR_DRBG_C
  EXPECT_EQ(0, mbedtls_ctr_drbg_self_test(0));
#endif
#ifdef MBEDTLS_HMAC_DRBG_C
  EXPECT_EQ(0, mbedtls_hmac_drbg_self_test(0));
#endif
#ifdef MBEDTLS_ENTROPY_C
  EXPECT_EQ(0, mbedtls_entropy_self_test(0));
#endif
#ifdef MBEDTLS_POLY1305_C
  EXPECT_EQ(0, mbedtls_poly1305_self_test(0));
#endif
#ifdef MBEDTLS_RSA_C
  EXPECT_EQ(0, mbedtls_rsa_self_test(0));
#endif
#ifdef MBEDTLS_BIGNUM_C
  EXPECT_EQ(0, mbedtls_mpi_self_test(0));
#endif
#ifdef MBEDTLS_PKCS5_C
  EXPECT_EQ(0, mbedtls_pkcs5_self_test(0));
#endif
#ifdef MBEDTLS_DHM_C
  EXPECT_EQ(0, mbedtls_dhm_self_test(0));
#endif
#ifdef MBEDTLS_GCM_C
  EXPECT_EQ(0, mbedtls_gcm_self_test(0));
#endif
#ifdef MBEDTLS_X509_USE_C
  EXPECT_EQ(0, mbedtls_x509_self_test(0));
#endif
#ifdef MBEDTLS_CHACHA20_C
  EXPECT_EQ(0, mbedtls_chacha20_self_test(0));
#endif
#ifdef MBEDTLS_CHACHAPOLY_C
  EXPECT_EQ(0, mbedtls_chachapoly_self_test(0));
#endif
#ifdef MBEDTLS_MEMORY_BUFFER_ALLOC_C
  EXPECT_EQ(0, mbedtls_memory_buffer_alloc_self_test(0));
#endif
#ifdef MBEDTLS_AES_C
  EXPECT_EQ(0, mbedtls_aes_self_test(0));
#endif
#ifdef MBEDTLS_BASE64_C
  EXPECT_EQ(0, mbedtls_base64_self_test(0));
#endif
#ifdef MBEDTLS_ECP_C
  EXPECT_EQ(0, mbedtls_ecp_self_test(0));
#endif
#ifdef MBEDTLS_MD5_C
  EXPECT_EQ(0, mbedtls_md5_self_test(0));
#endif
#ifdef MBEDTLS_SHA1_C
  EXPECT_EQ(0, mbedtls_sha1_self_test(0));
#endif
#ifdef MBEDTLS_SHA256_C
  EXPECT_EQ(0, mbedtls_sha256_self_test(0));
#endif
#ifdef MBEDTLS_SHA512_C
  EXPECT_EQ(0, mbedtls_sha512_self_test(0));
#endif
#ifdef MBEDTLS_NIST_KW_C
  EXPECT_EQ(0, mbedtls_nist_kw_self_test(0));
#endif
}
#endif /* MBEDTLS_SELF_TEST */

static void P256_MPI(mbedtls_mpi *N) {
  memcpy(N->p, rng, 8 * 8);
  ASSERT_EQ(0, mbedtls_mpi_mod_mpi(N, N, &grp.P));
}

static void P256_JUSTINE(mbedtls_mpi *N) {
  memcpy(N->p, rng, 8 * 8);
  secp256r1(N->p);
}

static void P384_MPI(mbedtls_mpi *N) {
  memcpy(N->p, rng, 12 * 8);
  ASSERT_EQ(0, mbedtls_mpi_mod_mpi(N, N, &grp.P));
}

static void P384_JUSTINE(mbedtls_mpi *N) {
  memcpy(N->p, rng, 12 * 8);
  secp384r1(N->p);
}

BENCH(p256, bench) {
#ifdef MBEDTLS_ECP_C
  mbedtls_ecp_group_init(&grp);
  mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256R1);
  mbedtls_mpi x = {1, 8, gc(calloc(8, 8))};
  rngset(x.p, 8 * 8, _rand64, -1);
  EZBENCH2("P-256 modulus MbedTLS MPI lib", donothing, P256_MPI(&x));
  EZBENCH2("P-256 modulus Justine rewrite", donothing, P256_JUSTINE(&x));
  mbedtls_ecp_group_free(&grp);
#endif
}

BENCH(p384, bench) {
#ifdef MBEDTLS_ECP_C
  mbedtls_ecp_group_init(&grp);
  mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP384R1);
  mbedtls_mpi x = {1, 12, gc(calloc(12, 8))};
  EZBENCH2("P-384 modulus MbedTLS MPI lib", donothing, P384_MPI(&x));
  EZBENCH2("P-384 modulus Justine rewrite", donothing, P384_JUSTINE(&x));
  mbedtls_ecp_group_free(&grp);
#endif
}

TEST(md, test) {
  uint8_t d[16];
  uint8_t want[16] = {0x90, 0x01, 0x50, 0x98, 0x3C, 0xD2, 0x4F, 0xB0,
                      0xD6, 0x96, 0x3F, 0x7D, 0x28, 0xE1, 0x7F, 0x72};
  mbedtls_md_context_t ctx;
  const mbedtls_md_info_t *digest;
  digest = mbedtls_md_info_from_type(MBEDTLS_MD_MD5);
  mbedtls_md_init(&ctx);
  mbedtls_md_setup(&ctx, digest, 0);
  mbedtls_md_starts(&ctx);
  mbedtls_md_update(&ctx, (const unsigned char *)"abc", 3);
  mbedtls_md_finish(&ctx, d);
  mbedtls_md_free(&ctx);
  EXPECT_EQ(0, memcmp(want, d, 16));
}

TEST(md5, test) {
  uint8_t d[16];
  uint8_t want[16] = {0x90, 0x01, 0x50, 0x98, 0x3C, 0xD2, 0x4F, 0xB0,
                      0xD6, 0x96, 0x3F, 0x7D, 0x28, 0xE1, 0x7F, 0x72};
  mbedtls_md5_ret("abc", 3, d);
  EXPECT_EQ(0, memcmp(want, d, 16));
}

TEST(sha1, test) {
  uint8_t d[20];
  uint8_t want[20] = {0xA9, 0x99, 0x3E, 0x36, 0x47, 0x06, 0x81,
                      0x6A, 0xBA, 0x3E, 0x25, 0x71, 0x78, 0x50,
                      0xC2, 0x6C, 0x9C, 0xD0, 0xD8, 0x9D};
  mbedtls_sha1_ret("abc", 3, d);
  EXPECT_EQ(0, memcmp(want, d, 20));
}

TEST(sha224, test) {
  uint8_t d[50];
  uint8_t want[28] = {0x23, 0x09, 0x7D, 0x22, 0x34, 0x05, 0xD8,
                      0x22, 0x86, 0x42, 0xA4, 0x77, 0xBD, 0xA2,
                      0x55, 0xB3, 0x2A, 0xAD, 0xBC, 0xE4, 0xBD,
                      0xA0, 0xB3, 0xF7, 0xE3, 0x6C, 0x9D, 0xA7};
  mbedtls_sha256_ret("abc", 3, d, 1);
  EXPECT_EQ(0, memcmp(want, d, 28));
}

TEST(sha256, test) {
  uint8_t d[32];
  uint8_t want[32] = {0xBA, 0x78, 0x16, 0xBF, 0x8F, 0x01, 0xCF, 0xEA,
                      0x41, 0x41, 0x40, 0xDE, 0x5D, 0xAE, 0x22, 0x23,
                      0xB0, 0x03, 0x61, 0xA3, 0x96, 0x17, 0x7A, 0x9C,
                      0xB4, 0x10, 0xFF, 0x61, 0xF2, 0x00, 0x15, 0xAD};
  mbedtls_sha256_ret("abc", 3, d, 0);
  EXPECT_EQ(0, memcmp(want, d, 32));
}

TEST(sha384, test) {
  uint8_t d[70];
  uint8_t want[48] = {
      0xCB, 0x00, 0x75, 0x3F, 0x45, 0xA3, 0x5E, 0x8B, 0xB5, 0xA0, 0x3D, 0x69,
      0x9A, 0xC6, 0x50, 0x07, 0x27, 0x2C, 0x32, 0xAB, 0x0E, 0xDE, 0xD1, 0x63,
      0x1A, 0x8B, 0x60, 0x5A, 0x43, 0xFF, 0x5B, 0xED, 0x80, 0x86, 0x07, 0x2B,
      0xA1, 0xE7, 0xCC, 0x23, 0x58, 0xBA, 0xEC, 0xA1, 0x34, 0xC8, 0x25, 0xA7};
  mbedtls_sha512_ret("abc", 3, d, 1);
  EXPECT_EQ(0, memcmp(want, d, 48));
}

TEST(sha512, test) {
  uint8_t d[64];
  uint8_t want[64] = {
      0xDD, 0xAF, 0x35, 0xA1, 0x93, 0x61, 0x7A, 0xBA, 0xCC, 0x41, 0x73,
      0x49, 0xAE, 0x20, 0x41, 0x31, 0x12, 0xE6, 0xFA, 0x4E, 0x89, 0xA9,
      0x7E, 0xA2, 0x0A, 0x9E, 0xEE, 0xE6, 0x4B, 0x55, 0xD3, 0x9A, 0x21,
      0x92, 0x99, 0x2A, 0x27, 0x4F, 0xC1, 0xA8, 0x36, 0xBA, 0x3C, 0x23,
      0xA3, 0xFE, 0xEB, 0xBD, 0x45, 0x4D, 0x44, 0x23, 0x64, 0x3C, 0xE8,
      0x0E, 0x2A, 0x9A, 0xC9, 0x4F, 0xA5, 0x4C, 0xA4, 0x9F};
  mbedtls_sha512_ret("abc", 3, d, 0);
  EXPECT_EQ(0, memcmp(want, d, 64));
}

static const uint64_t kTestKey1[4] = {
    0x0706050403020100,
    0x0F0E0D0C0B0A0908,
    0x1716151413121110,
    0x1F1E1D1C1B1A1918,
};

BENCH(mbedtls, bench) {
  uint8_t d[64];
  EZBENCH_N("md5", kHyperionSize, mbedtls_md5_ret(kHyperion, kHyperionSize, d));
  EZBENCH_N("sha1", kHyperionSize,
            mbedtls_sha1_ret(kHyperion, kHyperionSize, d));
  EZBENCH_N("sha256", kHyperionSize,
            mbedtls_sha256_ret(kHyperion, kHyperionSize, d, 0));
  EZBENCH_N("sha384", kHyperionSize,
            mbedtls_sha512_ret(kHyperion, kHyperionSize, d, 1));
  EZBENCH_N("sha512", kHyperionSize,
            mbedtls_sha512_ret(kHyperion, kHyperionSize, d, 0));
  EZBENCH_N("blake2b256", kHyperionSize,
            BLAKE2B256(kHyperion, kHyperionSize, d));
  EZBENCH_N("crc32_z", kHyperionSize, crc32_z(0, kHyperion, kHyperionSize));
  EZBENCH_N("highwayhash64", kHyperionSize,
            HighwayHash64(kHyperion, kHyperionSize, kTestKey1));
}

char *mpi2str(mbedtls_mpi *m) {
  size_t n;
  char s[1024];
  ASSERT_EQ(0, mbedtls_mpi_write_string(m, 10, s, sizeof(s), &n));
  return xasprintf("%.*s", n, s);
}

mbedtls_mpi *str2mpi(const char *s) {
  mbedtls_mpi *m;
  m = calloc(1, sizeof(mbedtls_mpi));
  ASSERT_EQ(0, mbedtls_mpi_read_string(m, 10, s));
  return m;
}

char *mpi2str16(mbedtls_mpi *m) {
  size_t n;
  char s[1024];
  ASSERT_EQ(0, mbedtls_mpi_write_string(m, 16, s, sizeof(s), &n));
  return xasprintf("%.*s", n, s);
}

mbedtls_mpi *str2mpi16(const char *s) {
  mbedtls_mpi *m;
  m = calloc(1, sizeof(mbedtls_mpi));
  ASSERT_EQ(0, mbedtls_mpi_read_string(m, 16, s));
  return m;
}

void CtrDrbgOverhead(void) {
  mbedtls_ctr_drbg_context rng;
  mbedtls_ctr_drbg_init(&rng);
  ASSERT_EQ(0, mbedtls_ctr_drbg_seed(&rng, GetEntropy, 0, "justine", 7));
  mbedtls_ctr_drbg_free(&rng);
}

void CtrDrbgReseed(mbedtls_ctr_drbg_context *rng) {
  ASSERT_EQ(0, mbedtls_ctr_drbg_reseed(
                   rng,
                   "justinejustinejustinejustinejustinejustinejustinejustinejus"
                   "tinejustinejustinejustinejustine",
                   64));
}

BENCH(CtrDrbg, bench) {
  mbedtls_ctr_drbg_context rng;
  mbedtls_ctr_drbg_init(&rng);
  ASSERT_EQ(0, mbedtls_ctr_drbg_seed(&rng, GetEntropy, 0, "justine", 7));
  /* EZBENCH2("CtrDrbgOverhead", donothing, CtrDrbgOverhead()); */
  /* EZBENCH2("CtrDrbgReseed", donothing, CtrDrbgReseed(&rng)); */
  mbedtls_ctr_drbg_free(&rng);
}

static void *realloc3(void *opaque, void *ptr, size_t size) {
  return realloc(ptr, size);
}

#if 0
BENCH(quickjs_remainder, bench) {
  char *t;
  bf_context_t s;
  bfdec_t x, y, r;
  bf_context_init(&s, realloc3, 0);
  bfdec_init(&s, &x);
  bfdec_init(&s, &y);
  bfdec_init(&s, &r);
  bfdec_atof(
      &x,
      "131820409343094310010388979423659136318401916109327276909280345024175692"
      "811283445510797521231721220331409407564807168230384468176942405812817310"
      "624525121840385446744443868889563289706427719939300365865529242495144888"
      "321833894158323756200092849226089461110385787540779132654409185831255860"
      "504316472846036364908238500078268116724689002106891044880894853471921527"
      "088201197650061259448583977618746693012787452335047965869945140544352170"
      "538037327032402834008159261693483647994727160945768940072431686625688866"
      "030658324868306061250176433564697324072528745672177336948242366753233417"
      "556818392219546938204560720202538843712268268448586361942128751395665874"
      "453900680147479758139717481147704392488266886671292379541285558418744606"
      "657296304926586001793382725791100208812287673612006034789731201688939975"
      "743537276539989692230927982557016660679726989062369216287647728379155260"
      "864643891615705346169567037448405029752790940875872989684235165316260908"
      "983893514490200568512210790489667188789433092320719785756398772086212370"
      "409401269127676106581410793787580434036114254547441805771508552049371634"
      "609025127325512605396392214570059772472666763440181556475095153967113514"
      "87546062479444592779055555421362722504575706910949375",
      NULL, BF_PREC_INF, BF_RNDZ | BF_ATOF_NO_NAN_INF);
  bfdec_atof(
      &y,
      "402702961953621844286950607555369624422784868935557056881131335461307658"
      "701727371551406721502307932123276358395008895125652043531209418099658895"
      "323804953421455502359439932416245276659698167468088937570774479761417692"
      "998541764456595941884384880600102787969744607942278005344329659944902212"
      "055120534831056155566296908941240558524043054812784309119298489621361046"
      "430678313566109525110538452853054430839857155846105630169165566758950183"
      "947324955260740763926892668470396323574248496692684009312249052922911490"
      "770564765036629340924434941440277974966684311625406958698534967519709470"
      "161586090763966964691950363765288731535684692199342872936240602328625671"
      "612857527958722799557444770545725755417136296135977255647153119878181440"
      "110593529655379472903525700943247456832124407971558524965730661045026185"
      "674462056105044630573746839553952570745211879290387589347246867522065584"
      "726369942916093728137773105488374703562705889962546268226061545128021323"
      "184760695318697037612212579413382773618361971983327301685232523283210570"
      "2331094682317528819996876363073536047370469375",
      NULL, BF_PREC_INF, BF_RNDZ | BF_ATOF_NO_NAN_INF);
  bfdec_rem(&r, &x, &y, BF_PREC_INF, 0, BF_RNDF);
  t = gc(bfdec_ftoa(0, &r, BF_PREC_INF, BF_RNDZ | BF_FTOA_FORMAT_FREE));
  ASSERT_STREQ("327339060789614187001318969682759915221664204604306478"
               "948329136809613379640467455488327009232590415715088668"
               "4127560071009217256545885393053328527589375",
               t);
  EZBENCH2("quickjs remainder", donothing,
           bfdec_rem(&r, &x, &y, BF_PREC_INF, 0, BF_RNDF));
  bfdec_delete(&r);
  bfdec_delete(&y);
  bfdec_delete(&x);
  bf_context_end(&s);
}
#endif

BENCH(mpi_remainder, bench) {
  mbedtls_mpi *x, *y, r;
  x = gc(str2mpi(
      "131820409343094310010388979423659136318401916109327276909280345024175692"
      "811283445510797521231721220331409407564807168230384468176942405812817310"
      "624525121840385446744443868889563289706427719939300365865529242495144888"
      "321833894158323756200092849226089461110385787540779132654409185831255860"
      "504316472846036364908238500078268116724689002106891044880894853471921527"
      "088201197650061259448583977618746693012787452335047965869945140544352170"
      "538037327032402834008159261693483647994727160945768940072431686625688866"
      "624525121840385446744443868889563289706427719939300365865529242495144888"
      "321833894158323756200092849226089461110385787540779132654409185831255860"
      "504316472846036364908238500078268116724689002106891044880894853471921527"
      "088201197650061259448583977618746693012787452335047965869945140544352170"
      "538037327032402834008159261693483647994727160945768940072431686625688866"
      "624525121840385446744443868889563289706427719939300365865529242495144888"
      "321833894158323756200092849226089461110385787540779132654409185831255860"
      "504316472846036364908238500078268116724689002106891044880894853471921527"
      "088201197650061259448583977618746693012787452335047965869945140544352170"
      "538037327032402834008159261693483647994727160945768940072431686625688866"
      "624525121840385446744443868889563289706427719939300365865529242495144888"
      "321833894158323756200092849226089461110385787540779132654409185831255860"
      "504316472846036364908238500078268116724689002106891044880894853471921527"
      "088201197650061259448583977618746693012787452335047965869945140544352170"
      "538037327032402834008159261693483647994727160945768940072431686625688866"
      "030658324868306061250176433564697324072528745672177336948242366753233417"
      "556818392219546938204560720202538843712268268448586361942128751395665874"
      "453900680147479758139717481147704392488266886671292379541285558418744606"
      "657296304926586001793382725791100208812287673612006034789731201688939975"
      "743537276539989692230927982557016660679726989062369216287647728379155260"
      "864643891615705346169567037448405029752790940875872989684235165316260908"
      "430678313566109525110538452853054430839857155846105630169165566758950183"
      "947324955260740763926892668470396323574248496692684009312249052922911490"
      "770564765036629340924434941440277974966684311625406958698534967519709470"
      "161586090763966964691950363765288731535684692199342872936240602328625671"
      "612857527958722799557444770545725755417136296135977255647153119878181440"
      "430678313566109525110538452853054430839857155846105630169165566758950183"
      "947324955260740763926892668470396323574248496692684009312249052922911490"
      "770564765036629340924434941440277974966684311625406958698534967519709470"
      "161586090763966964691950363765288731535684692199342872936240602328625671"
      "612857527958722799557444770545725755417136296135977255647153119878181440"
      "983893514490200568512210790489667188789433092320719785756398772086212370"
      "409401269127676106581410793787580434036114254547441805771508552049371634"
      "609025127325512605396392214570059772472666763440181556475095153967113514"
      "87546062479444592779055555421362722504575706910949375"));
  y = gc(str2mpi(
      "402702961953621844286950607555369624422784868935557056881131335461307658"
      "701727371551406721502307932123276358395008895125652043531209418099658895"
      "323804953421455502359439932416245276659698167468088937570774479761417692"
      "998541764456595941884384880600102787969744607942278005344329659944902212"
      "055120534831056155566296908941240558524043054812784309119298489621361046"
      "430678313566109525110538452853054430839857155846105630169165566758950183"
      "947324955260740763926892668470396323574248496692684009312249052922911490"
      "770564765036629340924434941440277974966684311625406958698534967519709470"
      "161586090763966964691950363765288731535684692199342872936240602328625671"
      "612857527958722799557444770545725755417136296135977255647153119878181440"
      "430678313566109525110538452853054430839857155846105630169165566758950183"
      "947324955260740763926892668470396323574248496692684009312249052922911490"
      "770564765036629340924434941440277974966684311625406958698534967519709470"
      "161586090763966964691950363765288731535684692199342872936240602328625671"
      "612857527958722799557444770545725755417136296135977255647153119878181440"
      "430678313566109525110538452853054430839857155846105630169165566758950183"
      "947324955260740763926892668470396323574248496692684009312249052922911490"
      "770564765036629340924434941440277974966684311625406958698534967519709470"
      "161586090763966964691950363765288731535684692199342872936240602328625671"
      "612857527958722799557444770545725755417136296135977255647153119878181440"
      "110593529655379472903525700943247456832124407971558524965730661045026185"
      "674462056105044630573746839553952570745211879290387589347246867522065584"
      "726369942916093728137773105488374703562705889962546268226061545128021323"
      "184760695318697037612212579413382773618361971983327301685232523283210570"
      "2331094682317528819996876363073536047370469375"));
  mbedtls_mpi_init(&r);
  EZBENCH2("mpi_remainder", donothing, mbedtls_mpi_mod_mpi(&r, x, y));
  mbedtls_mpi_free(&r);
  mbedtls_mpi_free(x);
  mbedtls_mpi_free(y);
}

BENCH(mpi_mul_int, bench) {
  mbedtls_mpi *x, y;
  x = gc(str2mpi(
      "131820409343094310010388979423659136318401916109327276909280345024175692"
      "811283445510797521231721220331409407564807168230384468176942405812817310"
      "624525121840385446744443868889563289706427719939300365865529242495144888"
      "321833894158323756200092849226089461110385787540779132654409185831255860"
      "504316472846036364908238500078268116724689002106891044880894853471921527"
      "088201197650061259448583977618746693012787452335047965869945140544352170"
      "538037327032402834008159261693483647994727160945768940072431686625688866"
      "624525121840385446744443868889563289706427719939300365865529242495144888"
      "321833894158323756200092849226089461110385787540779132654409185831255860"
      "504316472846036364908238500078268116724689002106891044880894853471921527"
      "088201197650061259448583977618746693012787452335047965869945140544352170"
      "538037327032402834008159261693483647994727160945768940072431686625688866"
      "624525121840385446744443868889563289706427719939300365865529242495144888"
      "321833894158323756200092849226089461110385787540779132654409185831255860"
      "504316472846036364908238500078268116724689002106891044880894853471921527"
      "088201197650061259448583977618746693012787452335047965869945140544352170"
      "538037327032402834008159261693483647994727160945768940072431686625688866"
      "624525121840385446744443868889563289706427719939300365865529242495144888"
      "321833894158323756200092849226089461110385787540779132654409185831255860"
      "504316472846036364908238500078268116724689002106891044880894853471921527"
      "088201197650061259448583977618746693012787452335047965869945140544352170"
      "538037327032402834008159261693483647994727160945768940072431686625688866"
      "030658324868306061250176433564697324072528745672177336948242366753233417"
      "556818392219546938204560720202538843712268268448586361942128751395665874"
      "453900680147479758139717481147704392488266886671292379541285558418744606"
      "657296304926586001793382725791100208812287673612006034789731201688939975"
      "743537276539989692230927982557016660679726989062369216287647728379155260"
      "864643891615705346169567037448405029752790940875872989684235165316260908"
      "430678313566109525110538452853054430839857155846105630169165566758950183"
      "947324955260740763926892668470396323574248496692684009312249052922911490"
      "770564765036629340924434941440277974966684311625406958698534967519709470"
      "161586090763966964691950363765288731535684692199342872936240602328625671"
      "612857527958722799557444770545725755417136296135977255647153119878181440"
      "430678313566109525110538452853054430839857155846105630169165566758950183"
      "947324955260740763926892668470396323574248496692684009312249052922911490"
      "770564765036629340924434941440277974966684311625406958698534967519709470"
      "161586090763966964691950363765288731535684692199342872936240602328625671"
      "612857527958722799557444770545725755417136296135977255647153119878181440"
      "983893514490200568512210790489667188789433092320719785756398772086212370"
      "409401269127676106581410793787580434036114254547441805771508552049371634"
      "609025127325512605396392214570059772472666763440181556475095153967113514"
      "87546062479444592779055555421362722504575706910949375"));
  mbedtls_mpi_init(&y);
  EZBENCH2("mpi_mul_int", donothing, ({
             mbedtls_mpi_copy(&y, x);
             mbedtls_mpi_mul_int(&y, &y, 31337);
           }));
  EZBENCH2("mpi_mul_mpi (scalar)", donothing, ({
             mbedtls_mpi b = {1, 1, (uint64_t[]){31337}};
             mbedtls_mpi_copy(&y, x);
             mbedtls_mpi_mul_mpi(&y, &y, &b);
           }));
  mbedtls_mpi_free(&y);
  mbedtls_mpi_free(x);
}

BENCH(mpi_shift_r, bench) {
  mbedtls_mpi x;
  mbedtls_mpi_init(&x);
  mbedtls_mpi_fill_random(&x, 2048 / 8, GetEntropy, 0);
  EZBENCH2("mpi_shift_r (0)", donothing, mbedtls_mpi_shift_r(&x, 0));
  EZBENCH2("mpi_shift_r (1)", donothing, mbedtls_mpi_shift_r(&x, 1));
  EZBENCH2("mpi_shift_r (65)", donothing, mbedtls_mpi_shift_r(&x, 65));
  EZBENCH2("mpi_shift_r (1024)", donothing, mbedtls_mpi_shift_r(&x, 1024));
  EZBENCH2("mpi_shift_r (1983)", donothing, mbedtls_mpi_shift_r(&x, 1983));
  EZBENCH2("mpi_shift_r (2047)", donothing, mbedtls_mpi_shift_r(&x, 2047));
  EZBENCH2("mpi_shift_r (2048)", donothing, mbedtls_mpi_shift_r(&x, 2048));
  mbedtls_mpi_free(&x);
}

TEST(mpi_shift_r, doesntCrash_dontUnderstandWeirdUpstreamBehavior) {
  mbedtls_mpi x = {1, 0, 0};
  EXPECT_EQ(0, mbedtls_mpi_shift_r(&x, 1));
}

TEST(mpi_shift_l, doesntCrash_dontUnderstandWeirdUpstreamBehavior) {
  mbedtls_mpi x = {1, 0, 0};
  EXPECT_EQ(0, mbedtls_mpi_shift_l(&x, 1));
  mbedtls_mpi_free(&x);
}

TEST(mpi_shift_r, fun0) {
  mbedtls_mpi x = {1, 1, (uint64_t[]){2}};
  EXPECT_EQ(0, mbedtls_mpi_shift_r(&x, 1));
  EXPECT_EQ(1, x.n);
  EXPECT_EQ(1, x.p[0]);
}

TEST(mpi_shift_r, fun1) {
  mbedtls_mpi x = {1, 7, (uint64_t[]){2, 4, 8, 16, 32, 64, 128}};
  EXPECT_EQ(0, mbedtls_mpi_shift_r(&x, 129));
  EXPECT_EQ(7, x.n);
  EXPECT_EQ(4, x.p[0]);
  EXPECT_EQ(8, x.p[1]);
  EXPECT_EQ(16, x.p[2]);
  EXPECT_EQ(32, x.p[3]);
  EXPECT_EQ(64, x.p[4]);
  EXPECT_EQ(0, x.p[5]);
  EXPECT_EQ(0, x.p[6]);
}

TEST(mpi_shift_r, fun2) {
  mbedtls_mpi x = {1, 3, (uint64_t[]){0, 1, 0}};
  EXPECT_EQ(0, mbedtls_mpi_shift_r(&x, 1));
  EXPECT_EQ(3, x.n);
  EXPECT_EQ(0x8000000000000000, x.p[0]);
  EXPECT_EQ(0, x.p[1]);
  EXPECT_EQ(0, x.p[2]);
}

TEST(mpi_shift_l, fun0) {
  mbedtls_mpi x = {1, 1, (uint64_t[]){2}};
  EXPECT_EQ(0, mbedtls_mpi_shift_l(&x, 1));
  EXPECT_EQ(1, x.n);
  EXPECT_EQ(4, x.p[0]);
}

TEST(mpi_shift_r, funbye) {
  mbedtls_mpi x = {1, 1, (uint64_t[]){2}};
  EXPECT_EQ(0, mbedtls_mpi_shift_r(&x, 100));
  EXPECT_EQ(1, x.n);
  EXPECT_EQ(0, x.p[0]);
}

TEST(mpi_shift_l, fun1) {
  mbedtls_mpi x = {1, 9, (uint64_t[]){2, 4, 8, 16, 32, 64, 128, 0, 0}};
  EXPECT_EQ(0, mbedtls_mpi_shift_l(&x, 129));
  EXPECT_EQ(9, x.n);
  EXPECT_EQ(0, x.p[0]);
  EXPECT_EQ(0, x.p[1]);
  EXPECT_EQ(2 << 1, x.p[2]);
  EXPECT_EQ(4 << 1, x.p[3]);
  EXPECT_EQ(8 << 1, x.p[4]);
  EXPECT_EQ(16 << 1, x.p[5]);
  EXPECT_EQ(32 << 1, x.p[6]);
  EXPECT_EQ(64 << 1, x.p[7]);
  EXPECT_EQ(128 << 1, x.p[8]);
}

TEST(mpi_shift_l, fun2) {
  mbedtls_mpi x = {1, 3,
                   (uint64_t[9]){
                       0x8000000000000003,
                       0x8000000000000002,
                       0x0000000000000001,
                   }};
  EXPECT_EQ(0, mbedtls_mpi_shift_l(&x, 1));
  EXPECT_EQ(3, x.n);
  EXPECT_EQ(6, x.p[0]);
  EXPECT_EQ(5, x.p[1]);
  EXPECT_EQ(3, x.p[2]);
  EXPECT_EQ(0, x.p[3]);
}

int BenchShiftLeft(mbedtls_mpi *X, size_t k) {
  X->n = 2048 / 64;
  X->p[X->n - 1] |= 1;
  return mbedtls_mpi_shift_l(X, k);
}

BENCH(mpi_shift_l, bench) {
  mbedtls_mpi x;
  mbedtls_mpi_init(&x);
  mbedtls_mpi_fill_random(&x, 2048 / 8, GetEntropy, 0);
  EZBENCH2("mpi_shift_l (0)", donothing, BenchShiftLeft(&x, 0));
  EZBENCH2("mpi_shift_l (1)", donothing, BenchShiftLeft(&x, 1));
  EZBENCH2("mpi_shift_l (65)", donothing, BenchShiftLeft(&x, 65));
  EZBENCH2("mpi_shift_l (1024)", donothing, BenchShiftLeft(&x, 1024));
  EZBENCH2("mpi_shift_l (1983)", donothing, BenchShiftLeft(&x, 1983));
  EZBENCH2("mpi_shift_l (2047)", donothing, BenchShiftLeft(&x, 2047));
  EZBENCH2("mpi_shift_l (2048)", donothing, BenchShiftLeft(&x, 2048));
  mbedtls_mpi_free(&x);
}

BENCH(gcd, bench) {
  mbedtls_mpi g = {1, 16, (uint64_t[32]){0}};
  mbedtls_mpi x = {1, 16, (uint64_t[32]){1500}};
  mbedtls_mpi y = {1, 16, (uint64_t[32]){700}};
  mbedtls_mpi_gcd(&g, &x, &y);
  EXPECT_EQ(100, g.p[0]);
  mbedtls_mpi_fill_random(&x, 16 * 8, GetEntropy, 0);
  mbedtls_mpi_fill_random(&y, 16 * 8, GetEntropy, 0);
  EZBENCH2("mbedtls_mpi_gcd (16)", donothing, mbedtls_mpi_gcd(&g, &x, &y));
}

BENCH(inv_mod, bench3) {
  mbedtls_mpi g = {0};
  mbedtls_mpi *x = gc(str2mpi16(
      "837B3E23091602B5D14D619D9B2CD79DD039BC9A9F46F0CA1FFD01B398EE42C8EE2142CB"
      "B295109FC4278DB8AB84A6ADBF319D3297216C349D0EB92925E2794C5FF1AAF664034CB2"
      "5C15CDA49B7947278AA96BEF9D995C5F99AA4809B12568A1513D8E0A37BB338DC44A1722"
      "F7821CFB11EBF2578151A3C8ECA1280AA4B1500463777FBBC00AE603A1A8F3C099524622"
      "7AFFD15FB66B320DF53CEA4D6C9935D0593BFC7A75ABAFDD3016F7C596FA58248BC041CF"
      "68ED274FA7F7D5BC3E014DDC7BEA4A60DF24805B5F94C998CAF28441FB4A5831755CE935"
      "2F17F5416647A81A78899E5B2C4D3F6C84A81CEB463C1593392ABCF6BF708A55578EB0EF"
      "E9ABF572"));
  mbedtls_mpi *y = gc(str2mpi16(
      "C14DA3DDE7CD1DD104D74972B899AC0E78E43A3C4ACF3A1316D05AE4CDA30088A7EE1E6B"
      "96A752B490EF2D727A3E249AFCB634AC24F577E026648C9CB0287DA1DAEA8CE6C91C96BC"
      "FEC10452B336D4A3FAE1B176D890C161B4665236A22653AAAB745E077D1982DB2AD81FA0"
      "D90D1C2D4966F75B257346E80B8A4F690CB50090E1DA8210667DAE542B8B657991A1E261"
      "C3CD404908EE680CF18B86D246BFD0B8AA11031E7F56A81A1E44180F0F858BDA8B445EE2"
      "18C6622FC7668DFA5DD87DF327892901C5900E3F27F130C84A0EEFD6DEC7C7276BC7053D"
      "7AC4023C9A1D3E0FE834985BCB734B5296D811A22C808869395AD30FB0DE592F11C7F7EA"
      "12013097"));
  mbedtls_mpi_inv_mod(&g, x, y);
  EZBENCH2("mbedtls_mpi_inv_mod (actual)", donothing,
           mbedtls_mpi_inv_mod(&g, x, y));
  mbedtls_mpi_free(&g);
  mbedtls_mpi_free(x);
  mbedtls_mpi_free(y);
}

TEST(ShiftRightAvx, test1) {
  if (!X86_HAVE(AVX)) return;
  int i;
  for (i = 0; i < 10; ++i) {
    uint64_t mem[1] = {_rand64()};
    uint64_t want[1];
    uint64_t got[1];
    memcpy(want, mem, sizeof(mem));
    memcpy(got, mem, sizeof(mem));
    ShiftRightPure(want, 1, 1);
    ShiftRightAvx(got, 1, 1);
    EXPECT_EQ(want[0], got[0]);
  }
}

TEST(ShiftRightAvx, test2) {
  if (!X86_HAVE(AVX)) return;
  int i;
  for (i = 0; i < 10; ++i) {
    uint64_t mem[2] = {_rand64(), _rand64()};
    uint64_t want[2];
    uint64_t got[2];
    memcpy(want, mem, sizeof(mem));
    memcpy(got, mem, sizeof(mem));
    ShiftRightPure(want, 2, 1);
    ShiftRightAvx(got, 2, 1);
    EXPECT_EQ(want[0], got[0]);
    EXPECT_EQ(want[1], got[1]);
  }
}

TEST(ShiftRightAvx, test3) {
  if (!X86_HAVE(AVX)) return;
  int i;
  for (i = 0; i < 10; ++i) {
    uint64_t mem[3] = {_rand64(), _rand64(), _rand64()};
    uint64_t want[3];
    uint64_t got[3];
    memcpy(want, mem, sizeof(mem));
    memcpy(got, mem, sizeof(mem));
    ShiftRightPure(want, 3, 1);
    ShiftRightAvx(got, 3, 1);
    EXPECT_EQ(want[0], got[0]);
    EXPECT_EQ(want[1], got[1]);
    EXPECT_EQ(want[2], got[2]);
  }
}

TEST(ShiftRightAvx, test4) {
  if (!X86_HAVE(AVX)) return;
  int i;
  for (i = 0; i < 10; ++i) {
    uint64_t mem[4] = {_rand64(), _rand64(), _rand64(), _rand64()};
    uint64_t want[4];
    uint64_t got[4];
    memcpy(want, mem, sizeof(mem));
    memcpy(got, mem, sizeof(mem));
    ShiftRightPure(want, 4, 1);
    ShiftRightAvx(got, 4, 1);
    EXPECT_EQ(want[0], got[0]);
    EXPECT_EQ(want[1], got[1]);
    EXPECT_EQ(want[2], got[2]);
    EXPECT_EQ(want[3], got[3]);
  }
}

TEST(ShiftRightAvx, test8) {
  if (!X86_HAVE(AVX)) return;
  int i;
  for (i = 0; i < 10; ++i) {
    uint64_t mem[8] = {_rand64(), _rand64(), _rand64(), _rand64(),
                       _rand64(), _rand64(), _rand64(), _rand64()};
    uint64_t want[8];
    uint64_t got[8];
    memcpy(want, mem, sizeof(mem));
    memcpy(got, mem, sizeof(mem));
    ShiftRightPure(want, 8, 1);
    ShiftRightAvx(got, 8, 1);
    EXPECT_EQ(want[0], got[0]);
    EXPECT_EQ(want[1], got[1]);
    EXPECT_EQ(want[2], got[2]);
    EXPECT_EQ(want[3], got[3]);
    EXPECT_EQ(want[4], got[4]);
    EXPECT_EQ(want[5], got[5]);
    EXPECT_EQ(want[6], got[6]);
    EXPECT_EQ(want[7], got[7]);
  }
}

TEST(ShiftRightAvx, test9) {
  if (!X86_HAVE(AVX)) return;
  int i;
  for (i = 0; i < 10; ++i) {
    uint64_t mem[9] = {_rand64(), _rand64(), _rand64(), _rand64(), _rand64(),
                       _rand64(), _rand64(), _rand64(), _rand64()};
    uint64_t want[9];
    uint64_t got[9];
    memcpy(want, mem, sizeof(mem));
    memcpy(got, mem, sizeof(mem));
    ShiftRightPure(want, 9, 1);
    ShiftRightAvx(got, 9, 1);
    EXPECT_EQ(want[0], got[0]);
    EXPECT_EQ(want[1], got[1]);
    EXPECT_EQ(want[2], got[2]);
    EXPECT_EQ(want[3], got[3]);
  }
}

BENCH(ShiftRight, bench) {
  if (!X86_HAVE(AVX)) return;
  uint64_t x[64];
  rngset(x, sizeof(x), _rand64, -1);
  EZBENCH2("ShiftRight", donothing, ShiftRight(x, 64, 1));
  EZBENCH2("ShiftRightAvx", donothing, ShiftRightAvx(x, 64, 1));
  EZBENCH2("ShiftRightPure", donothing, ShiftRightPure(x, 64, 1));
}

BENCH(Zeroize, bench) {
  uint64_t x[64];
  rngset(x, sizeof(x), _rand64, -1);
  EZBENCH2("memset (64)", donothing, memset(x, 0, sizeof(x)));
  EZBENCH2("Zeroize (64)", donothing, mbedtls_platform_zeroize(x, 64));
}

int mbedtls_mpi_read_binary2(mbedtls_mpi *X, const unsigned char *p, size_t n);

TEST(endian, big1) {
  /* uint8_t b[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, */
  /*                0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F}; */
  uint8_t b[] = {
      0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07,  //
      0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //
      0x03,                                            //
  };
  mbedtls_mpi x;
  mbedtls_mpi_init(&x);
  ASSERT_EQ(0, mbedtls_mpi_read_binary(&x, b, ARRAYLEN(b)));
  EXPECT_GE(x.n, 3);
  EXPECT_EQ(0x0000000000000003, x.p[0]);
  EXPECT_EQ(0x0000000000000702, x.p[1]);
  EXPECT_EQ(0x0000000000000001, x.p[2]);
  mbedtls_mpi_free(&x);
}

TEST(endian, big2) {
  uint8_t b[] = {
      0x01,  //
  };
  mbedtls_mpi x;
  mbedtls_mpi_init(&x);
  ASSERT_EQ(0, mbedtls_mpi_read_binary(&x, b, ARRAYLEN(b)));
  EXPECT_EQ(1, x.n);
  EXPECT_EQ(0x0000000000000001, x.p[0]);
  mbedtls_mpi_free(&x);
}

TEST(endian, big3) {
  uint8_t b[] = {
      0x01, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,  //
      0x01, 0x02,                                      //
  };
  mbedtls_mpi x;
  mbedtls_mpi_init(&x);
  ASSERT_EQ(0, mbedtls_mpi_read_binary(&x, b, ARRAYLEN(b)));
  EXPECT_EQ(2, x.n);
  EXPECT_EQ(0x0101010101010102, x.p[0]);
  EXPECT_EQ(0x0000000000000102, x.p[1]);
  mbedtls_mpi_free(&x);
}

TEST(endian, big4) {
  uint8_t b[] = {
      0x11, 0x68, 0x5b, 0xb5, 0x76, 0x6f, 0xb5, 0x72,  //
      0x43, 0xd2, 0x3f, 0xd6, 0xc0, 0x1b, 0xa3, 0x2e,  //
      0x40, 0x77, 0x12, 0xc8, 0x59, 0x4e, 0x63, 0xab,  //
      0xea, 0xeb, 0x4a, 0x58, 0x50, 0xbd, 0xed, 0x30,  //
      0x10, 0x76, 0xa9, 0xfa, 0x01, 0xa1, 0x07, 0xe8,  //
      0xa3, 0xd5, 0xaf, 0x4e, 0x1f, 0xf6, 0xaf,        //
  };
  mbedtls_mpi x;
  mbedtls_mpi_init(&x);
  ASSERT_EQ(0, mbedtls_mpi_read_binary(&x, b, ARRAYLEN(b)));
  EXPECT_GE(x.n, 6);
  EXPECT_EQ(0xe8a3d5af4e1ff6af, x.p[0]);
  EXPECT_EQ(0x301076a9fa01a107, x.p[1]);
  EXPECT_EQ(0xabeaeb4a5850bded, x.p[2]);
  EXPECT_EQ(0x2e407712c8594e63, x.p[3]);
  EXPECT_EQ(0x7243d23fd6c01ba3, x.p[4]);
  mbedtls_mpi_free(&x);
}

TEST(Mul4x4, test) {
  int N, M;
  mbedtls_mpi A, B, C, D;
  if (!X86_HAVE(BMI2) || !X86_HAVE(ADX)) return;
  N = 4;
  M = 4;
  mbedtls_mpi_init(&A);
  mbedtls_mpi_init(&B);
  mbedtls_mpi_init(&C);
  mbedtls_mpi_init(&D);
  mbedtls_mpi_fill_random(&A, N * 8, GetEntropy, 0);
  mbedtls_mpi_fill_random(&B, M * 8, GetEntropy, 0);
  mbedtls_mpi_fill_random(&C, (N + M) * 8, GetEntropy, 0);
  mbedtls_mpi_fill_random(&D, (N + M) * 8, GetEntropy, 0);
  mbedtls_mpi_mul_mpi(&C, &A, &B);
  Mul4x4(D.p, A.p, B.p);
  EXPECT_EQ(0, memcmp(D.p, C.p, (N + M) * 8));
  mbedtls_mpi_free(&D);
  mbedtls_mpi_free(&C);
  mbedtls_mpi_free(&B);
  mbedtls_mpi_free(&A);
}

BENCH(Mul4x4, bench) {
  int i, N, M;
  mbedtls_mpi A, B, C, D, E;
  if (!X86_HAVE(BMI2) || !X86_HAVE(ADX)) return;
  N = 4;
  M = 4;
  mbedtls_mpi_init(&A);
  mbedtls_mpi_init(&B);
  mbedtls_mpi_init(&C);
  mbedtls_mpi_init(&D);
  mbedtls_mpi_init(&E);
  mbedtls_mpi_fill_random(&A, N * 8, GetEntropy, 0);
  mbedtls_mpi_fill_random(&B, M * 8, GetEntropy, 0);
  mbedtls_mpi_fill_random(&C, (N + M) * 8, GetEntropy, 0);
  mbedtls_mpi_fill_random(&D, (N + M) * 8, GetEntropy, 0);
  mbedtls_mpi_fill_random(&E, (N + M) * 8, GetEntropy, 0);
  Mul(C.p, A.p, N, B.p, M);
  Mul4x4Adx(D.p, A.p, B.p);
  Mul4x4Pure(E.p, A.p, B.p);
  if (memcmp(E.p, C.p, (N + M) * 8)) {
    printf("\n");
    printf(
        "# X                    Y                   MPI RESULT          PURE "
        "RESULT\n");
    for (i = 0; i < N + M; ++i) {
      printf("0x%016lx * 0x%016lx = 0x%016lx vs. 0x%016lx %d\n",
             i < N ? A.p[i] : 0, i < M ? B.p[i] : 0, E.p[i], C.p[i],
             E.p[i] == C.p[i]);
    }
    printf("\n");
  }
  if (memcmp(D.p, C.p, (N + M) * 8)) {
    printf("\n");
    printf("# X                    Y                   MPI RESULT ADX "
           "RESULT\n");
    for (i = 0; i < N + M; ++i) {
      printf("0x%016lx * 0x%016lx = 0x%016lx vs. 0x%016lx %d\n",
             i < N ? A.p[i] : 0, i < M ? B.p[i] : 0, D.p[i], C.p[i],
             D.p[i] == C.p[i]);
    }
    printf("\n");
  }
  EXPECT_EQ(0, memcmp(D.p, C.p, (N + M) * 8));
  EXPECT_EQ(0, memcmp(E.p, C.p, (N + M) * 8));
  EZBENCH2("orig multiply 4x4", donothing, Mul(C.p, A.p, N, B.p, M));
  EZBENCH2("mpi multiply 4x4", donothing, mbedtls_mpi_mul_mpi(&C, &A, &B));
  EZBENCH2("Mul4x4Adx", donothing, Mul4x4Adx(D.p, A.p, B.p));
  EZBENCH2("Mul4x4Pure", donothing, Mul4x4Pure(E.p, A.p, B.p));
  mbedtls_mpi_free(&E);
  mbedtls_mpi_free(&D);
  mbedtls_mpi_free(&C);
  mbedtls_mpi_free(&B);
  mbedtls_mpi_free(&A);
}

BENCH(Mul6x6, bench) {
  int i, N, M;
  mbedtls_mpi A, B, C, D;
  if (!X86_HAVE(BMI2) || !X86_HAVE(ADX)) return;
  N = 6;
  M = 6;
  mbedtls_mpi_init(&A);
  mbedtls_mpi_init(&B);
  mbedtls_mpi_init(&C);
  mbedtls_mpi_init(&D);
  mbedtls_mpi_fill_random(&A, N * 8, GetEntropy, 0);
  mbedtls_mpi_fill_random(&B, M * 8, GetEntropy, 0);
  mbedtls_mpi_fill_random(&C, (N + M) * 8, GetEntropy, 0);
  mbedtls_mpi_fill_random(&D, (N + M) * 8, GetEntropy, 0);
  Mul(C.p, A.p, N, B.p, M);
  Mul6x6Adx(D.p, A.p, B.p);
  if (memcmp(D.p, C.p, (N + M) * 8)) {
    printf("\n");
    printf("# X                    Y                   MPI RESULT ADX "
           "RESULT\n");
    for (i = 0; i < N + M; ++i) {
      printf("0x%016lx * 0x%016lx = 0x%016lx vs. 0x%016lx %d\n",
             i < N ? A.p[i] : 0, i < M ? B.p[i] : 0, D.p[i], C.p[i],
             D.p[i] == C.p[i]);
    }
    printf("\n");
  }
  EXPECT_EQ(0, memcmp(D.p, C.p, (N + M) * 8));
  EZBENCH2("orig multiply 6x6", donothing, Mul(C.p, A.p, N, B.p, M));
  EZBENCH2("mpi multiply 6x6", donothing, mbedtls_mpi_mul_mpi(&C, &A, &B));
  EZBENCH2("Mul6x6Adx", donothing, Mul6x6Adx(D.p, A.p, B.p));
  mbedtls_mpi_free(&D);
  mbedtls_mpi_free(&C);
  mbedtls_mpi_free(&B);
  mbedtls_mpi_free(&A);
}

BENCH(Mul10x10, bench) {
  int N, M;
  mbedtls_mpi A, B, C;
  if (!X86_HAVE(BMI2) || !X86_HAVE(ADX)) return;
  N = 10;
  M = 10;
  mbedtls_mpi_init(&A);
  mbedtls_mpi_init(&B);
  mbedtls_mpi_init(&C);
  mbedtls_mpi_fill_random(&A, N * 8, GetEntropy, 0);
  mbedtls_mpi_fill_random(&B, M * 8, GetEntropy, 0);
  mbedtls_mpi_fill_random(&C, (N + M) * 8, GetEntropy, 0);
  mbedtls_mpi_mul_mpi(&C, &A, &B);
  EZBENCH2("mpi multiply 10x10", donothing, mbedtls_mpi_mul_mpi(&C, &A, &B));
  mbedtls_mpi_free(&C);
  mbedtls_mpi_free(&B);
  mbedtls_mpi_free(&A);
}

BENCH(Mul16x16, bench) {
  int N, M;
  mbedtls_mpi A, B, C;
  if (!X86_HAVE(BMI2) || !X86_HAVE(ADX)) return;
  N = 16;
  M = 16;
  mbedtls_mpi_init(&A);
  mbedtls_mpi_init(&B);
  mbedtls_mpi_init(&C);
  mbedtls_mpi_fill_random(&A, N * 8, GetEntropy, 0);
  mbedtls_mpi_fill_random(&B, M * 8, GetEntropy, 0);
  mbedtls_mpi_fill_random(&C, (N + M) * 8, GetEntropy, 0);
  mbedtls_mpi_mul_mpi(&C, &A, &B);
  EZBENCH2("mpi multiply 16x16", donothing, mbedtls_mpi_mul_mpi(&C, &A, &B));
  mbedtls_mpi_free(&C);
  mbedtls_mpi_free(&B);
  mbedtls_mpi_free(&A);
}

BENCH(Mul32x32, bench) {
  int i, N, M;
  mbedtls_mpi A, B, C, D, K;
  if (!X86_HAVE(BMI2) || !X86_HAVE(ADX)) return;
  N = 32;
  M = 32;
  mbedtls_mpi_init(&A);
  mbedtls_mpi_init(&B);
  mbedtls_mpi_init(&C);
  mbedtls_mpi_init(&D);
  mbedtls_mpi_init(&K);
  for (i = 0; i < 8; ++i) {
    mbedtls_mpi_fill_random(&A, N * 8, GetEntropy, 0);
    mbedtls_mpi_fill_random(&B, M * 8, GetEntropy, 0);
    mbedtls_mpi_fill_random(&C, (N + M) * 8, GetEntropy, 0);
    mbedtls_mpi_fill_random(&D, (N + M) * 8, GetEntropy, 0);
    mbedtls_mpi_fill_random(&K, (N + M) * 2 * 8, GetEntropy, 0);
    mbedtls_mpi_mul_mpi(&C, &A, &B);
    Karatsuba(D.p, A.p, B.p, N, K.p);
    if (memcmp(D.p, C.p, (N + M) * 8)) {
      printf("\n");
      printf("# X                    Y                   MPI RESULT OTH "
             "RESULT\n");
      for (i = 0; i < N + M; ++i) {
        printf("0x%016lx * 0x%016lx = 0x%016lx vs. 0x%016lx %d\n",
               i < N ? A.p[i] : 0, i < M ? B.p[i] : 0, D.p[i], C.p[i],
               D.p[i] == C.p[i]);
      }
      printf("\n");
      exit(1);
    }
  }
  EZBENCH2("mpi multiply 32x32", donothing, mbedtls_mpi_mul_mpi(&C, &A, &B));
  EZBENCH2("karatsuba 32x32", donothing, Karatsuba(C.p, A.p, B.p, N, K.p));
  mbedtls_mpi_free(&K);
  mbedtls_mpi_free(&D);
  mbedtls_mpi_free(&C);
  mbedtls_mpi_free(&B);
  mbedtls_mpi_free(&A);
}

BENCH(Mul16x1, bench) {
  int N, M;
  mbedtls_mpi A, B, C;
  if (!X86_HAVE(BMI2) || !X86_HAVE(ADX)) return;
  N = 16;
  M = 1;
  mbedtls_mpi_init(&A);
  mbedtls_mpi_init(&B);
  mbedtls_mpi_init(&C);
  mbedtls_mpi_fill_random(&A, N * 8, GetEntropy, 0);
  mbedtls_mpi_fill_random(&B, M * 8, GetEntropy, 0);
  mbedtls_mpi_fill_random(&C, (N + M) * 8, GetEntropy, 0);
  mbedtls_mpi_mul_mpi(&C, &A, &B);
  EZBENCH2("mpi multiply 16x1", donothing, mbedtls_mpi_mul_mpi(&C, &A, &B));
  mbedtls_mpi_free(&C);
  mbedtls_mpi_free(&B);
  mbedtls_mpi_free(&A);
}

BENCH(Mul32x1, bench) {
  int N, M;
  mbedtls_mpi A, B, C;
  if (!X86_HAVE(BMI2) || !X86_HAVE(ADX)) return;
  N = 32;
  M = 1;
  mbedtls_mpi_init(&A);
  mbedtls_mpi_init(&B);
  mbedtls_mpi_init(&C);
  mbedtls_mpi_fill_random(&A, N * 8, GetEntropy, 0);
  mbedtls_mpi_fill_random(&B, M * 8, GetEntropy, 0);
  mbedtls_mpi_fill_random(&C, (N + M) * 8, GetEntropy, 0);
  mbedtls_mpi_mul_mpi(&C, &A, &B);
  EZBENCH2("mpi multiply 32x1", donothing, mbedtls_mpi_mul_mpi(&C, &A, &B));
  mbedtls_mpi_free(&C);
  mbedtls_mpi_free(&B);
  mbedtls_mpi_free(&A);
}

BENCH(isprime, bench1024) {
  mbedtls_mpi P;
  mbedtls_mpi_init(&P);
  EXPECT_EQ(0, mbedtls_mpi_gen_prime(&P, 1024, 0, GetEntropy, 0));
  EXPECT_EQ(0, mbedtls_mpi_is_prime_ext(&P, 3, GetEntropy, 0));
  EZBENCH2("isprime (1024)", donothing,
           mbedtls_mpi_is_prime_ext(&P, 3, GetEntropy, 0));
  mbedtls_mpi_free(&P);
}

BENCH(cmpint, bench) {
  mbedtls_mpi x = {1, 8, (uint64_t[8]){0}};
  mbedtls_mpi y = {1, 8, (uint64_t[8]){1}};
  mbedtls_mpi z = {1, 8, (uint64_t[8]){1, 1, 1, 1, 1, 1, 1, 1}};
  EZBENCH2("cmpint 1.1", donothing, mbedtls_mpi_cmp_int(&x, 0));
  EZBENCH2("cmpint 1.2", donothing, mbedtls_mpi_cmp_int(&x, 1));
  EZBENCH2("cmpint 2.1", donothing, mbedtls_mpi_cmp_int(&y, 0));
  EZBENCH2("cmpint 2.2", donothing, mbedtls_mpi_cmp_int(&y, 1));
  EZBENCH2("cmpint 3.1", donothing, mbedtls_mpi_cmp_int(&z, 0));
  EZBENCH2("cmpint 3.2", donothing, mbedtls_mpi_cmp_int(&z, 1));
}

TEST(pbkdf2, test) {
  unsigned char dk[20];
  mbedtls_md_context_t ctx;
  mbedtls_md_init(&ctx);
  ASSERT_EQ(
      0, mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA1), 1));
  EXPECT_EQ(
      0, mbedtls_pkcs5_pbkdf2_hmac(&ctx, "password", 8, "salt", 4, 1, 20, dk));
  EXPECT_BINEQ("0c60c80f961f0e71f3a9b524af6012062fe037a6", dk);
  mbedtls_md_free(&ctx);
}
