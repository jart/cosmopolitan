/*
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "../notrandombytes/notrandombytes.h"
#include "mldsa_native.h"
#include "src/sys.h"

/* Additional SUPERCOP-style macros for functions not in the standard set */
#define crypto_sign_keypair_internal MLD_API_NAMESPACE(keypair_internal)
#define crypto_sign_signature_extmu MLD_API_NAMESPACE(signature_extmu)
#define crypto_sign_verify_extmu MLD_API_NAMESPACE(verify_extmu)
#define crypto_sign_signature_pre_hash_shake256 \
  MLD_API_NAMESPACE(signature_pre_hash_shake256)
#define crypto_sign_verify_pre_hash_shake256 \
  MLD_API_NAMESPACE(verify_pre_hash_shake256)
#define crypto_sign_pk_from_sk MLD_API_NAMESPACE(pk_from_sk)

#ifndef NTESTS
#define NTESTS 100
#endif
#define MLEN 59
#define CTXLEN 1

#define CHECK(x)                                              \
  do                                                          \
  {                                                           \
    int r;                                                    \
    r = (x);                                                  \
    if (!r)                                                   \
    {                                                         \
      fprintf(stderr, "ERROR (%s,%d)\n", __FILE__, __LINE__); \
      return 1;                                               \
    }                                                         \
  } while (0)


static int test_sign_core(uint8_t pk[CRYPTO_PUBLICKEYBYTES],
                          uint8_t sk[CRYPTO_SECRETKEYBYTES],
                          uint8_t sm[MLEN + CRYPTO_BYTES], uint8_t m[MLEN],
                          uint8_t m2[MLEN + CRYPTO_BYTES], uint8_t ctx[CTXLEN])
{
  size_t smlen;
  size_t mlen;
  int rc;


  CHECK(crypto_sign_keypair(pk, sk) == 0);
  CHECK(randombytes(ctx, CTXLEN) == 0);
  MLD_CT_TESTING_SECRET(ctx, CTXLEN);
  CHECK(randombytes(m, MLEN) == 0);
  MLD_CT_TESTING_SECRET(m, MLEN);

  CHECK(crypto_sign(sm, &smlen, m, MLEN, ctx, CTXLEN, sk) == 0);

  rc = crypto_sign_open(m2, &mlen, sm, smlen, ctx, CTXLEN, pk);

  /* Constant time: Declassify outputs to check them. */
  MLD_CT_TESTING_DECLASSIFY(rc, sizeof(int));
  MLD_CT_TESTING_DECLASSIFY(m, MLEN);
  MLD_CT_TESTING_DECLASSIFY(m2, (MLEN + CRYPTO_BYTES));

  if (rc)
  {
    printf("ERROR: crypto_sign_open\n");
    return 1;
  }

  if (memcmp(m, m2, MLEN))
  {
    printf("ERROR: crypto_sign_open - wrong message\n");
    return 1;
  }

  if (smlen != MLEN + CRYPTO_BYTES)
  {
    printf("ERROR: crypto_sign_open - wrong smlen\n");
    return 1;
  }

  if (mlen != MLEN)
  {
    printf("ERROR: crypto_sign_open - wrong mlen\n");
    return 1;
  }

  return 0;
}

static int test_sign(void)
{
  uint8_t pk[CRYPTO_PUBLICKEYBYTES];
  uint8_t sk[CRYPTO_SECRETKEYBYTES];
  uint8_t sm[MLEN + CRYPTO_BYTES];
  uint8_t m[MLEN];
  uint8_t m2[MLEN + CRYPTO_BYTES];
  uint8_t ctx[CTXLEN];

  return test_sign_core(pk, sk, sm, m, m2, ctx);
}

static int test_sign_unaligned(void)
{
  MLD_ALIGN uint8_t pk[CRYPTO_PUBLICKEYBYTES + 1];
  MLD_ALIGN uint8_t sk[CRYPTO_SECRETKEYBYTES + 1];
  MLD_ALIGN uint8_t sm[MLEN + CRYPTO_BYTES + 1];
  MLD_ALIGN uint8_t m[MLEN + 1];
  MLD_ALIGN uint8_t m2[MLEN + CRYPTO_BYTES + 1];
  MLD_ALIGN uint8_t ctx[CTXLEN + 1];

  return test_sign_core(pk + 1, sk + 1, sm + 1, m + 1, m2 + 1, ctx + 1);
}

static int test_sign_extmu(void)
{
  uint8_t pk[CRYPTO_PUBLICKEYBYTES];
  uint8_t sk[CRYPTO_SECRETKEYBYTES];
  uint8_t sig[CRYPTO_BYTES];
  uint8_t mu[MLDSA_CRHBYTES];
  size_t siglen;

  CHECK(crypto_sign_keypair(pk, sk) == 0);
  CHECK(randombytes(mu, MLDSA_CRHBYTES) == 0);
  MLD_CT_TESTING_SECRET(mu, sizeof(mu));

  CHECK(crypto_sign_signature_extmu(sig, &siglen, mu, sk) == 0);
  CHECK(crypto_sign_verify_extmu(sig, siglen, mu, pk) == 0);

  return 0;
}


static int test_sign_pre_hash(void)
{
  uint8_t pk[CRYPTO_PUBLICKEYBYTES];
  uint8_t sk[CRYPTO_SECRETKEYBYTES];
  uint8_t sig[CRYPTO_BYTES];
  uint8_t m[MLEN];
  uint8_t ctx[CTXLEN];
  uint8_t rnd[MLDSA_RNDBYTES];
  size_t siglen;


  CHECK(crypto_sign_keypair(pk, sk) == 0);
  CHECK(randombytes(ctx, CTXLEN) == 0);
  MLD_CT_TESTING_SECRET(ctx, sizeof(ctx));
  CHECK(randombytes(m, MLEN) == 0);
  MLD_CT_TESTING_SECRET(m, sizeof(m));
  CHECK(randombytes(rnd, MLDSA_RNDBYTES) == 0);
  MLD_CT_TESTING_SECRET(rnd, sizeof(rnd));

  CHECK(crypto_sign_signature_pre_hash_shake256(sig, &siglen, m, MLEN, ctx,
                                                CTXLEN, rnd, sk) == 0);
  CHECK(crypto_sign_verify_pre_hash_shake256(sig, siglen, m, MLEN, ctx, CTXLEN,
                                             pk) == 0);

  return 0;
}

static int test_pk_from_sk(void)
{
  uint8_t pk[CRYPTO_PUBLICKEYBYTES];
  uint8_t pk_derived[CRYPTO_PUBLICKEYBYTES];
  uint8_t sk[CRYPTO_SECRETKEYBYTES];
  uint8_t sk_corrupted[CRYPTO_SECRETKEYBYTES];
  int rc;

  /* Generate a keypair */
  CHECK(crypto_sign_keypair(pk, sk) == 0);

  /* Derive public key from secret key */
  CHECK(crypto_sign_pk_from_sk(pk_derived, sk) == 0);

  /* Verify derived public key matches original */
  if (memcmp(pk, pk_derived, CRYPTO_PUBLICKEYBYTES) != 0)
  {
    printf("ERROR: pk_from_sk - derived public key does not match original\n");
    return 1;
  }

  /* Test with corrupted t0 in secret key - should fail validation */
  memcpy(sk_corrupted, sk, CRYPTO_SECRETKEYBYTES);
  /* Corrupt a byte in the t0 portion of the secret key */
  sk_corrupted[MLDSA_SEEDBYTES + MLDSA_TRBYTES + MLDSA_SEEDBYTES + 10] ^= 1;

  rc = crypto_sign_pk_from_sk(pk_derived, sk_corrupted);

  /* Constant time: Declassify to check result */
  MLD_CT_TESTING_DECLASSIFY(&rc, sizeof(int));

  if (rc != -1)
  {
    printf("ERROR: pk_from_sk - should fail with corrupted t0 in secret key\n");
    return 1;
  }

  /* Test with corrupted tr in secret key - should fail validation */
  memcpy(sk_corrupted, sk, CRYPTO_SECRETKEYBYTES);
  /* Corrupt a byte in the tr portion of the secret key */
  /* tr starts at offset 2 * MLDSA_SEEDBYTES (after rho and key) */
  sk_corrupted[2 * MLDSA_SEEDBYTES + 10] ^= 1;

  rc = crypto_sign_pk_from_sk(pk_derived, sk_corrupted);

  /* Constant time: Declassify to check result */
  MLD_CT_TESTING_DECLASSIFY(&rc, sizeof(int));

  if (rc != -1)
  {
    printf(
        "ERROR: crypto_sign_pk_from_sk - should fail with corrupted tr in "
        "secret key\n");
    return 1;
  }

  return 0;
}

static int test_wrong_pk(void)
{
  uint8_t pk[CRYPTO_PUBLICKEYBYTES];
  uint8_t sk[CRYPTO_SECRETKEYBYTES];
  uint8_t sm[MLEN + CRYPTO_BYTES];
  uint8_t m[MLEN];
  uint8_t m2[MLEN + CRYPTO_BYTES] = {0};
  uint8_t ctx[CTXLEN];
  size_t smlen;
  size_t mlen;
  int rc;
  size_t idx;
  size_t i;

  CHECK(crypto_sign_keypair(pk, sk) == 0);
  CHECK(randombytes(ctx, CTXLEN) == 0);
  MLD_CT_TESTING_SECRET(ctx, sizeof(ctx));
  CHECK(randombytes(m, MLEN) == 0);
  MLD_CT_TESTING_SECRET(m, sizeof(m));

  CHECK(crypto_sign(sm, &smlen, m, MLEN, ctx, CTXLEN, sk) == 0);

  /* flip bit in public key */
  CHECK(randombytes((uint8_t *)&idx, sizeof(size_t)) == 0);
  idx %= CRYPTO_PUBLICKEYBYTES;

  pk[idx] ^= 1;

  rc = crypto_sign_open(m2, &mlen, sm, smlen, ctx, CTXLEN, pk);

  /* Constant time: Declassify outputs to check them. */
  MLD_CT_TESTING_DECLASSIFY(rc, sizeof(int));
  MLD_CT_TESTING_DECLASSIFY(m2, sizeof(m2));

  if (!rc)
  {
    printf("ERROR: wrong_pk: crypto_sign_open\n");
    return 1;
  }

  for (i = 0; i < MLEN; i++)
  {
    if (m2[i] != 0)
    {
      printf("ERROR: wrong_pk: crypto_sign_open - message should be zero\n");
      return 1;
    }
  }
  return 0;
}

static int test_wrong_sig(void)
{
  uint8_t pk[CRYPTO_PUBLICKEYBYTES];
  uint8_t sk[CRYPTO_SECRETKEYBYTES];
  uint8_t sm[MLEN + CRYPTO_BYTES];
  uint8_t m[MLEN];
  uint8_t m2[MLEN + CRYPTO_BYTES] = {0};
  uint8_t ctx[CTXLEN];
  size_t smlen;
  size_t mlen;
  int rc;
  size_t idx;
  size_t i;

  CHECK(crypto_sign_keypair(pk, sk) == 0);
  CHECK(randombytes(ctx, CTXLEN) == 0);
  MLD_CT_TESTING_SECRET(ctx, sizeof(ctx));
  CHECK(randombytes(m, MLEN) == 0);
  MLD_CT_TESTING_SECRET(m, sizeof(m));

  CHECK(crypto_sign(sm, &smlen, m, MLEN, ctx, CTXLEN, sk) == 0);

  /* flip bit in signed message */
  CHECK(randombytes((uint8_t *)&idx, sizeof(size_t)) == 0);
  idx %= MLEN + CRYPTO_BYTES;

  sm[idx] ^= 1;

  rc = crypto_sign_open(m2, &mlen, sm, smlen, ctx, CTXLEN, pk);

  /* Constant time: Declassify outputs to check them. */
  MLD_CT_TESTING_DECLASSIFY(rc, sizeof(int));
  MLD_CT_TESTING_DECLASSIFY(m2, sizeof(m2));

  if (!rc)
  {
    printf("ERROR: wrong_sig: crypto_sign_open\n");
    return 1;
  }

  for (i = 0; i < MLEN; i++)
  {
    if (m2[i] != 0)
    {
      printf("ERROR: wrong_sig: crypto_sign_open - message should be zero\n");
      return 1;
    }
  }
  return 0;
}


static int test_wrong_ctx(void)
{
  uint8_t pk[CRYPTO_PUBLICKEYBYTES];
  uint8_t sk[CRYPTO_SECRETKEYBYTES];
  uint8_t sm[MLEN + CRYPTO_BYTES];
  uint8_t m[MLEN];
  uint8_t m2[MLEN + CRYPTO_BYTES] = {0};
  uint8_t ctx[CTXLEN];
  size_t smlen;
  size_t mlen;
  int rc;
  size_t idx;
  size_t i;

  CHECK(crypto_sign_keypair(pk, sk) == 0);
  CHECK(randombytes(ctx, CTXLEN) == 0);
  MLD_CT_TESTING_SECRET(ctx, sizeof(ctx));
  CHECK(randombytes(m, MLEN) == 0);
  MLD_CT_TESTING_SECRET(m, sizeof(m));

  CHECK(crypto_sign(sm, &smlen, m, MLEN, ctx, CTXLEN, sk) == 0);

  /* flip bit in ctx */
  CHECK(randombytes((uint8_t *)&idx, sizeof(size_t)) == 0);
  idx %= CTXLEN;

  ctx[idx] ^= 1;

  rc = crypto_sign_open(m2, &mlen, sm, smlen, ctx, CTXLEN, pk);

  /* Constant time: Declassify outputs to check them. */
  MLD_CT_TESTING_DECLASSIFY(rc, sizeof(int));
  MLD_CT_TESTING_DECLASSIFY(m2, sizeof(m2));

  if (!rc)
  {
    printf("ERROR: wrong_sig: crypto_sign_open\n");
    return 1;
  }

  for (i = 0; i < MLEN; i++)
  {
    if (m2[i] != 0)
    {
      printf("ERROR: wrong_sig: crypto_sign_open - message should be zero\n");
      return 1;
    }
  }
  return 0;
}

int main(void)
{
  unsigned i;
  int r;

  /* WARNING: Test-only
   * Normally, you would want to seed a PRNG with trustworthy entropy here. */
  randombytes_reset();

  for (i = 0; i < NTESTS; i++)
  {
    r = test_sign();
    r |= test_sign_unaligned();
    r |= test_wrong_pk();
    r |= test_wrong_sig();
    r |= test_wrong_ctx();
    r |= test_sign_extmu();
    r |= test_sign_pre_hash();
    r |= test_pk_from_sk();
    if (r)
    {
      return 1;
    }
  }

  printf("CRYPTO_SECRETKEYBYTES:  %d\n", CRYPTO_SECRETKEYBYTES);
  printf("CRYPTO_PUBLICKEYBYTES:  %d\n", CRYPTO_PUBLICKEYBYTES);
  printf("CRYPTO_BYTES: %d\n", CRYPTO_BYTES);

  return 0;
}
