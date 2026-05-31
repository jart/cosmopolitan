/*
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "src/common.h"

#include "../notrandombytes/notrandombytes.h"
#include "mldsa_native.h"
#include "src/fips202/fips202.h"
#include "src/sys.h"

/* Additional SUPERCOP-style macros for functions not in the standard set */
#define crypto_sign_keypair_internal MLD_API_NAMESPACE(keypair_internal)
#define crypto_sign_signature_internal MLD_API_NAMESPACE(signature_internal)

#if defined(MLD_SYS_WINDOWS)
#include <fcntl.h>
#include <io.h>
#endif

#define MAXMLEN 2048
#define CTXLEN 0

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



static void print_hex(const uint8_t *data, size_t size)
{
  size_t i;
  for (i = 0; i < size; i++)
  {
    printf("%02x", data[i]);
  }
  printf("\n");
}

int main(void)
{
  unsigned i;
  int rc;
  uint8_t pk[CRYPTO_PUBLICKEYBYTES];
  uint8_t sk[CRYPTO_SECRETKEYBYTES];
  uint8_t s[CRYPTO_BYTES];
  uint8_t *m;
  /* empty ctx */
  uint8_t pre[2] = {0, 0};
  size_t slen;

  const uint8_t seed[64] = {
      32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
      48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
      64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
      80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
  };
  uint8_t coins[MLDSA_SEEDBYTES + MLDSA_RNDBYTES + MAXMLEN];

#if defined(MLD_SYS_WINDOWS)
  /* Disable automatic CRLF conversion on Windows to match testvector hashes */
  _setmode(_fileno(stdout), _O_BINARY);
#endif

  /*
   * We cannot rely on randombytes in the KAT test as randombytes() is used
   * inside of crypto_sign_signature() which is called as a part of
   * key generation in case PCT (pairwise-consistency test) is enabled.
   * To allow KAT tests to still pass successfully, we derandomize the
   * KAT test to only use deterministic randomness derived using SHAKE.
   */

  mld_shake256(coins, sizeof(coins), seed, sizeof(seed));

  for (i = 0; i < MAXMLEN; i = (i == 0) ? i + 1 : i << 2)
  {
    mld_shake256(coins, sizeof(coins), coins, sizeof(coins));
    m = coins + MLDSA_SEEDBYTES + MLDSA_RNDBYTES;

    CHECK(crypto_sign_keypair_internal(pk, sk, coins) == 0);

    print_hex(pk, CRYPTO_PUBLICKEYBYTES);
    print_hex(sk, CRYPTO_SECRETKEYBYTES);

    CHECK(crypto_sign_signature_internal(s, &slen, m, i, pre, sizeof(pre),
                                         coins + MLDSA_SEEDBYTES, sk, 0) == 0);

    print_hex(s, slen);

    rc = crypto_sign_verify(s, slen, m, i, NULL, CTXLEN, pk);

    if (rc)
    {
      printf("ERROR: signature verification failed\n");
      return -1;
    }
  }
  return 0;
}
