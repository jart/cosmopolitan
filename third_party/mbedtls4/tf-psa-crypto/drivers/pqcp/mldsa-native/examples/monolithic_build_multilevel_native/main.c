/*
 * Copyright (c) The mlkem-native project authors
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mldsa_native_all.c"

#define MLD_CONFIG_API_CONSTANTS_ONLY
#include <mldsa_native.h>

#include "expected_signatures.h"
#include "test_only_rng/notrandombytes.h"

#define CHECK(x)                                              \
  do                                                          \
  {                                                           \
    int rc;                                                   \
    rc = (x);                                                 \
    if (!rc)                                                  \
    {                                                         \
      fprintf(stderr, "ERROR (%s,%d)\n", __FILE__, __LINE__); \
      return 1;                                               \
    }                                                         \
  } while (0)

#define TEST_MSG \
  "This is a test message for ML-DSA digital signature algorithm!"
#define TEST_MSG_LEN (sizeof(TEST_MSG) - 1)

#define TEST_CTX "test_context_123"
#define TEST_CTX_LEN (sizeof(TEST_CTX) - 1)

static int test_mldsa44(void)
{
  const char test_msg[] = TEST_MSG;
  const char test_ctx[] = TEST_CTX;

  uint8_t pk[MLDSA44_PUBLICKEYBYTES];
  uint8_t sk[MLDSA44_SECRETKEYBYTES];
  uint8_t sig[MLDSA44_BYTES];
  uint8_t sm[TEST_MSG_LEN + MLDSA44_BYTES]; /* signed message buffer */
  uint8_t m2[TEST_MSG_LEN + MLDSA44_BYTES]; /* recovered message buffer */
  size_t siglen;
  size_t smlen;
  size_t mlen;

  /* WARNING: Test-only
   * Normally, you would want to seed a PRNG with trustworthy entropy here. */
  randombytes_reset();

  printf("ML-DSA-44\n");
  printf("======================\n\n");

  printf("Message: %s\n", test_msg);
  printf("Context: %s\n\n", test_ctx);

  printf("Generating keypair ... ");

  /* Alice generates a public/private key pair */
  CHECK(mldsa44_keypair(pk, sk) == 0);

  printf("DONE\n");
  printf("Signing message... ");

  /* Alice signs the message */
  CHECK(mldsa44_signature(sig, &siglen, (const uint8_t *)test_msg, TEST_MSG_LEN,
                          (const uint8_t *)test_ctx, TEST_CTX_LEN, sk) == 0);

  printf("DONE\n");
  printf("Verifying signature... ");

  /* Bob verifies Alice's signature */
  CHECK(mldsa44_verify(sig, siglen, (const uint8_t *)test_msg, TEST_MSG_LEN,
                       (const uint8_t *)test_ctx, TEST_CTX_LEN, pk) == 0);

  printf("DONE\n");
  printf("Creating signed message... ");

  /* Alternative API: Create a signed message (signature + message combined) */
  CHECK(mldsa44_sign(sm, &smlen, (const uint8_t *)test_msg, TEST_MSG_LEN,
                     (const uint8_t *)test_ctx, TEST_CTX_LEN, sk) == 0);

  printf("DONE\n");
  printf("Opening signed message... ");

  /* Bob opens the signed message to recover the original message */
  CHECK(mldsa44_open(m2, &mlen, sm, smlen, (const uint8_t *)test_ctx,
                     TEST_CTX_LEN, pk) == 0);

  printf("DONE\n");
  printf("Compare messages... ");

  /* Verify the recovered message matches the original */
  CHECK(mlen == TEST_MSG_LEN);
  CHECK(memcmp(test_msg, m2, TEST_MSG_LEN) == 0);

  printf("DONE\n\n");

  printf("Results:\n");
  printf("--------\n");
  printf("Public key size:  %d bytes\n", MLDSA44_PUBLICKEYBYTES);
  printf("Secret key size:  %d bytes\n", MLDSA44_SECRETKEYBYTES);
  printf("Signature size:   %d bytes\n", MLDSA44_BYTES);
  printf("Message length:   %lu bytes\n", (unsigned long)TEST_MSG_LEN);
  printf("Signature length: %lu bytes\n", (unsigned long)siglen);
  printf("Signed msg length: %lu bytes\n", (unsigned long)smlen);

#if !defined(MLD_CONFIG_KEYGEN_PCT)
  /* Check against expected signature to make sure that
   * we integrated the library correctly */
  printf("Checking deterministic signature... ");
  {
    /* Compare the generated signature directly against the expected signature
     */
    CHECK(siglen == sizeof(expected_signature_44));
    CHECK(memcmp(sig, expected_signature_44, siglen) == 0);
  }
  printf("DONE\n");
#else  /* !MLD_CONFIG_KEYGEN_PCT */
  printf(
      "[WARNING] Skipping KAT test since PCT is enabled and modifies PRNG\n");
#endif /* MLD_CONFIG_KEYGEN_PCT */

  printf("Signature verification completed successfully!\n");

  printf("\nAll tests passed! ML-DSA signature verification successful.\n");
  return 0;
}

static int test_mldsa65(void)
{
  const char test_msg[] = TEST_MSG;
  const char test_ctx[] = TEST_CTX;

  uint8_t pk[MLDSA65_PUBLICKEYBYTES];
  uint8_t sk[MLDSA65_SECRETKEYBYTES];
  uint8_t sig[MLDSA65_BYTES];
  uint8_t sm[TEST_MSG_LEN + MLDSA65_BYTES]; /* signed message buffer */
  uint8_t m2[TEST_MSG_LEN + MLDSA65_BYTES]; /* recovered message buffer */
  size_t siglen;
  size_t smlen;
  size_t mlen;

  /* WARNING: Test-only
   * Normally, you would want to seed a PRNG with trustworthy entropy here. */
  randombytes_reset();

  printf("ML-DSA-65\n");
  printf("======================\n\n");

  printf("Message: %s\n", test_msg);
  printf("Context: %s\n\n", test_ctx);

  printf("Generating keypair ... ");

  /* Alice generates a public/private key pair */
  CHECK(mldsa65_keypair(pk, sk) == 0);

  printf("DONE\n");
  printf("Signing message... ");

  /* Alice signs the message */
  CHECK(mldsa65_signature(sig, &siglen, (const uint8_t *)test_msg, TEST_MSG_LEN,
                          (const uint8_t *)test_ctx, TEST_CTX_LEN, sk) == 0);

  printf("DONE\n");
  printf("Verifying signature... ");

  /* Bob verifies Alice's signature */
  CHECK(mldsa65_verify(sig, siglen, (const uint8_t *)test_msg, TEST_MSG_LEN,
                       (const uint8_t *)test_ctx, TEST_CTX_LEN, pk) == 0);

  printf("DONE\n");
  printf("Creating signed message... ");

  /* Alternative API: Create a signed message (signature + message combined) */
  CHECK(mldsa65_sign(sm, &smlen, (const uint8_t *)test_msg, TEST_MSG_LEN,
                     (const uint8_t *)test_ctx, TEST_CTX_LEN, sk) == 0);

  printf("DONE\n");
  printf("Opening signed message... ");

  /* Bob opens the signed message to recover the original message */
  CHECK(mldsa65_open(m2, &mlen, sm, smlen, (const uint8_t *)test_ctx,
                     TEST_CTX_LEN, pk) == 0);

  printf("DONE\n");
  printf("Compare messages... ");

  /* Verify the recovered message matches the original */
  CHECK(mlen == TEST_MSG_LEN);
  CHECK(memcmp(test_msg, m2, TEST_MSG_LEN) == 0);

  printf("DONE\n\n");

  printf("Results:\n");
  printf("--------\n");
  printf("Public key size:  %d bytes\n", MLDSA65_PUBLICKEYBYTES);
  printf("Secret key size:  %d bytes\n", MLDSA65_SECRETKEYBYTES);
  printf("Signature size:   %d bytes\n", MLDSA65_BYTES);
  printf("Message length:   %lu bytes\n", (unsigned long)TEST_MSG_LEN);
  printf("Signature length: %lu bytes\n", (unsigned long)siglen);
  printf("Signed msg length: %lu bytes\n", (unsigned long)smlen);

#if !defined(MLD_CONFIG_KEYGEN_PCT)
  /* Check against expected signature to make sure that
   * we integrated the library correctly */
  printf("Checking deterministic signature... ");
  {
    /* Compare the generated signature directly against the expected signature
     */
    CHECK(siglen == sizeof(expected_signature_65));
    CHECK(memcmp(sig, expected_signature_65, siglen) == 0);
  }
  printf("DONE\n");
#else  /* !MLD_CONFIG_KEYGEN_PCT */
  printf(
      "[WARNING] Skipping KAT test since PCT is enabled and modifies PRNG\n");
#endif /* MLD_CONFIG_KEYGEN_PCT */

  printf("Signature verification completed successfully!\n");

  printf("\nAll tests passed! ML-DSA signature verification successful.\n");
  return 0;
}


static int test_mldsa87(void)
{
  const char test_msg[] = TEST_MSG;
  const char test_ctx[] = TEST_CTX;

  uint8_t pk[MLDSA87_PUBLICKEYBYTES];
  uint8_t sk[MLDSA87_SECRETKEYBYTES];
  uint8_t sig[MLDSA87_BYTES];
  uint8_t sm[TEST_MSG_LEN + MLDSA87_BYTES]; /* signed message buffer */
  uint8_t m2[TEST_MSG_LEN + MLDSA87_BYTES]; /* recovered message buffer */
  size_t siglen;
  size_t smlen;
  size_t mlen;

  /* WARNING: Test-only
   * Normally, you would want to seed a PRNG with trustworthy entropy here. */
  randombytes_reset();

  printf("ML-DSA-87\n");
  printf("======================\n\n");

  printf("Message: %s\n", test_msg);
  printf("Context: %s\n\n", test_ctx);

  printf("Generating keypair ... ");

  /* Alice generates a public/private key pair */
  CHECK(mldsa87_keypair(pk, sk) == 0);

  printf("DONE\n");
  printf("Signing message... ");

  /* Alice signs the message */
  CHECK(mldsa87_signature(sig, &siglen, (const uint8_t *)test_msg, TEST_MSG_LEN,
                          (const uint8_t *)test_ctx, TEST_CTX_LEN, sk) == 0);

  printf("DONE\n");
  printf("Verifying signature... ");

  /* Bob verifies Alice's signature */
  CHECK(mldsa87_verify(sig, siglen, (const uint8_t *)test_msg, TEST_MSG_LEN,
                       (const uint8_t *)test_ctx, TEST_CTX_LEN, pk) == 0);

  printf("DONE\n");
  printf("Creating signed message... ");

  /* Alternative API: Create a signed message (signature + message combined) */
  CHECK(mldsa87_sign(sm, &smlen, (const uint8_t *)test_msg, TEST_MSG_LEN,
                     (const uint8_t *)test_ctx, TEST_CTX_LEN, sk) == 0);

  printf("DONE\n");
  printf("Opening signed message... ");

  /* Bob opens the signed message to recover the original message */
  CHECK(mldsa87_open(m2, &mlen, sm, smlen, (const uint8_t *)test_ctx,
                     TEST_CTX_LEN, pk) == 0);

  printf("DONE\n");
  printf("Compare messages... ");

  /* Verify the recovered message matches the original */
  CHECK(mlen == TEST_MSG_LEN);
  CHECK(memcmp(test_msg, m2, TEST_MSG_LEN) == 0);

  printf("DONE\n\n");

  printf("Results:\n");
  printf("--------\n");
  printf("Public key size:  %d bytes\n", MLDSA87_PUBLICKEYBYTES);
  printf("Secret key size:  %d bytes\n", MLDSA87_SECRETKEYBYTES);
  printf("Signature size:   %d bytes\n", MLDSA87_BYTES);
  printf("Message length:   %lu bytes\n", (unsigned long)TEST_MSG_LEN);
  printf("Signature length: %lu bytes\n", (unsigned long)siglen);
  printf("Signed msg length: %lu bytes\n", (unsigned long)smlen);

#if !defined(MLD_CONFIG_KEYGEN_PCT)
  /* Check against expected signature to make sure that
   * we integrated the library correctly */
  printf("Checking deterministic signature... ");
  {
    /* Compare the generated signature directly against the expected signature
     */
    CHECK(siglen == sizeof(expected_signature_87));
    CHECK(memcmp(sig, expected_signature_87, siglen) == 0);
  }
  printf("DONE\n");
#else  /* !MLD_CONFIG_KEYGEN_PCT */
  printf(
      "[WARNING] Skipping KAT test since PCT is enabled and modifies PRNG\n");
#endif /* MLD_CONFIG_KEYGEN_PCT */

  printf("Signature verification completed successfully!\n");

  printf("\nAll tests passed! ML-DSA signature verification successful.\n");
  return 0;
}

int main(void)
{
  printf("ML-DSA monolithic_build_multilevel Example\n");

  if (test_mldsa44() != 0)
  {
    return 1;
  }

  if (test_mldsa65() != 0)
  {
    return 1;
  }

  if (test_mldsa87() != 0)
  {
    return 1;
  }

  return 0;
}
