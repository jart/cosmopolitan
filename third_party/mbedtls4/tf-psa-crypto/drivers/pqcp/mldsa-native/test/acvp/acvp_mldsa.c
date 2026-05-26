/*
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/common.h"

#include "mldsa_native.h"

/* Additional SUPERCOP-style macros for functions not in the standard set */
#define crypto_sign_keypair_internal MLD_API_NAMESPACE(keypair_internal)
#define crypto_sign_signature_internal MLD_API_NAMESPACE(signature_internal)
#define crypto_sign_verify_internal MLD_API_NAMESPACE(verify_internal)
#define crypto_sign_verify_extmu MLD_API_NAMESPACE(verify_extmu)
#define crypto_sign_signature_pre_hash_internal \
  MLD_API_NAMESPACE(signature_pre_hash_internal)
#define crypto_sign_verify_pre_hash_internal \
  MLD_API_NAMESPACE(verify_pre_hash_internal)
#define crypto_sign_signature_pre_hash_shake256 \
  MLD_API_NAMESPACE(signature_pre_hash_shake256)
#define crypto_sign_verify_pre_hash_shake256 \
  MLD_API_NAMESPACE(verify_pre_hash_shake256)

#define USAGE "acvp_mldsa{lvl} [keyGen|sigGen|sigVer] {test specific arguments}"
#define KEYGEN_USAGE "acvp_mldsa{lvl} keyGen seed=HEX"
#define SIGGEN_USAGE \
  "acvp_mldsa{lvl} sigGen message=HEX sk=HEX context=HEX rnd=HEX"
#define SIGGEN_INTERNAL_USAGE \
  "acvp_mldsa{lvl} sigGenInternal message=HEX sk=HEX externalMu=0/1 rnd=HEX"
#define SIGGEN_DETERMINISTIC_USAGE \
  "acvp_mldsa{lvl} sigGenDeterministic message=HEX sk=HEX context=HEX"
#define SIGGEN_INTERNAL_DETERMINISTIC_USAGE                         \
  "acvp_mldsa{lvl} sigGenInternalDeterministic message=HEX sk=HEX " \
  "externalMu=0/1"
#define SIGGEN_PREHASH_DETERMINISTIC_USAGE                                \
  "acvp_mldsa{lvl} sigGenPreHashDeterministic ph=HEX context=HEX sk=HEX " \
  "hashAlg=STRING"
#define SIGGEN_PREHASH_SHAKE256_DETERMINISTIC_USAGE                 \
  "acvp_mldsa{lvl} sigGenPreHashShake256Deterministic message=HEX " \
  "context=HEX sk=HEX"
#define SIGVER_USAGE \
  "acvp_mldsa{lvl} sigVer message=HEX context=HEX signature=HEX pk=HEX"
#define SIGVER_INTERNAL_USAGE                                        \
  "acvp_mldsa{lvl} sigVerInternal message=HEX signature=HEX pk=HEX " \
  "externalMu=0/1"
#define SIGGEN_PREHASH_USAGE                                 \
  "acvp_mldsa{lvl} sigGenPreHash ph=HEX context=HEX sk=HEX " \
  "hashAlg=STRING rnd=HEX"
#define SIGVER_PREHASH_USAGE                                               \
  "acvp_mldsa{lvl} sigVerPreHash ph=HEX context=HEX signature=HEX pk=HEX " \
  "hashAlg=STRING"
#define SIGGEN_PREHASH_SHAKE256_USAGE                              \
  "acvp_mldsa{lvl} sigGenPreHashShake256 message=HEX context=HEX " \
  "sk=HEX rnd=HEX"
#define SIGVER_PREHASH_SHAKE256_USAGE                              \
  "acvp_mldsa{lvl} sigVerPreHashShake256 message=HEX context=HEX " \
  "signature=HEX pk=HEX"


/* maximum message length used in the ACVP tests */
#define MAX_MSG_LENGTH 65536
/* maximum context length according to FIPS-204 */
#define MAX_CTX_LENGTH 255

#define CHECK(x)                                              \
  do                                                          \
  {                                                           \
    int rc;                                                   \
    rc = (x);                                                 \
    if (!rc)                                                  \
    {                                                         \
      fprintf(stderr, "ERROR (%s,%d)\n", __FILE__, __LINE__); \
      exit(1);                                                \
    }                                                         \
  } while (0)

typedef enum
{
  keyGen,
  sigGen,
  sigGenInternal,
  sigGenDeterministic,
  sigGenInternalDeterministic,
  sigGenPreHashDeterministic,
  sigGenPreHashShake256Deterministic,
  sigVer,
  sigVerInternal,
  sigGenPreHash,
  sigVerPreHash,
  sigGenPreHashShake256,
  sigVerPreHashShake256
} acvp_mode;

/* Decode hex character [0-9A-Fa-f] into 0-15 */
static unsigned char decode_hex_char(char hex)
{
  if (hex >= '0' && hex <= '9')
  {
    return (unsigned char)(hex - '0');
  }
  else if (hex >= 'A' && hex <= 'F')
  {
    return (unsigned char)(10 + (unsigned char)(hex - 'A'));
  }
  else if (hex >= 'a' && hex <= 'f')
  {
    return (unsigned char)(10 + (unsigned char)(hex - 'a'));
  }
  else
  {
    return 0xFF;
  }
}

static int decode_hex(const char *prefix, unsigned char *out, size_t out_len,
                      const char *hex)
{
  size_t i;
  size_t hex_len = strlen(hex);
  size_t prefix_len = strlen(prefix);

  /*
   * Check that hex starts with `prefix=`
   * Use memcmp, not strcmp
   */
  if (hex_len < prefix_len + 1 || memcmp(prefix, hex, prefix_len) != 0 ||
      hex[prefix_len] != '=')
  {
    goto hex_usage;
  }

  hex += prefix_len + 1;
  hex_len -= prefix_len + 1;

  if (hex_len != 2 * out_len)
  {
    goto hex_usage;
  }

  for (i = 0; i < out_len; i++, hex += 2, out++)
  {
    unsigned hex0 = decode_hex_char(hex[0]);
    unsigned hex1 = decode_hex_char(hex[1]);
    if (hex0 == 0xFF || hex1 == 0xFF)
    {
      goto hex_usage;
    }

    *out = ((hex0 << 4) | hex1) & 0XFF;
  }

  return 0;

hex_usage:
  fprintf(stderr,
          "Argument %s invalid: Expected argument of the form '%s=HEX' with "
          "HEX being a hex encoding of %u bytes\n",
          hex, prefix, (unsigned)out_len);
  return 1;
}


static int decode_keyed_int(const char *prefix_string, int *out,
                            const char *str)
{
  size_t str_len = strlen(str);
  size_t prefix_len = strlen(prefix_string);
  char *endptr;
  long val;

  /*
   * Check that str starts with `prefix=`
   * Use memcmp, not strcmp
   */
  if (str_len < prefix_len + 1 || memcmp(prefix_string, str, prefix_len) != 0 ||
      str[prefix_len] != '=')
  {
    goto int_usage;
  }

  str += prefix_len + 1;

  /* Parse the integer value */
  val = strtol(str, &endptr, 10);

  /* Check for parsing errors */
  if (*endptr != '\0' || endptr == str)
  {
    goto int_usage;
  }

  /* Check for overflow */
  if (val > INT_MAX || val < INT_MIN)
  {
    goto int_usage;
  }

  *out = (int)val;
  return 0;

int_usage:
  fprintf(stderr,
          "Argument %s invalid: Expected argument of the form '%s=INT' with "
          "INT being a decimal integer\n",
          str, prefix_string);
  return 1;
}

static int parse_str(const char *prefix, char *out, size_t out_max_len,
                     const char *str)
{
  size_t str_len = strlen(str);
  size_t prefix_len = strlen(prefix);
  size_t value_len;

  /*
   * Check that str starts with `prefix=`
   * Use memcmp, not strcmp
   */
  if (str_len < prefix_len + 1 || memcmp(prefix, str, prefix_len) != 0 ||
      str[prefix_len] != '=')
  {
    goto str_usage;
  }

  str += prefix_len + 1;
  value_len = strlen(str);

  if (value_len >= out_max_len)
  {
    fprintf(stderr,
            "Argument %s invalid: String value too long (max %u characters)\n",
            str - prefix_len - 1, (unsigned)(out_max_len - 1));
    return 1;
  }

  strncpy(out, str, out_max_len - 1);
  out[out_max_len - 1] = '\0';
  return 0;

str_usage:
  fprintf(stderr,
          "Argument %s invalid: Expected argument of the form '%s=STRING'\n",
          str, prefix);
  return 1;
}

static void print_hex(const char *name, const unsigned char *raw, size_t len)
{
  if (name != NULL)
  {
    printf("%s=", name);
  }
  for (; len > 0; len--, raw++)
  {
    printf("%02X", *raw);
  }
  printf("\n");
}

static void acvp_mldsa_keyGen_AFT(const unsigned char seed[MLDSA_RNDBYTES])
{
  unsigned char pk[CRYPTO_PUBLICKEYBYTES];
  unsigned char sk[CRYPTO_SECRETKEYBYTES];

  CHECK(crypto_sign_keypair_internal(pk, sk, seed) == 0);

  print_hex("pk", pk, sizeof(pk));
  print_hex("sk", sk, sizeof(sk));
}

static void acvp_mldsa_sigGen_AFT(const unsigned char *message, size_t mlen,
                                  const unsigned char rnd[MLDSA_SEEDBYTES],
                                  const unsigned char sk[CRYPTO_SECRETKEYBYTES],
                                  const unsigned char *context, size_t ctxlen)
{
  unsigned char sig[CRYPTO_BYTES];
  size_t siglen;
  unsigned char pre[MAX_CTX_LENGTH + 2];

  CHECK(ctxlen <= 255);
  pre[0] = 0;
  /* Safety: Truncation is safe due to the check above. */
  pre[1] = (uint8_t)ctxlen;
  memcpy(pre + 2, context, ctxlen);

  CHECK(crypto_sign_signature_internal(sig, &siglen, message, mlen, pre,
                                       ctxlen + 2, rnd, sk, 0) == 0);
  print_hex("signature", sig, sizeof(sig));
}

static void acvp_mldsa_sigGenInternal_AFT(
    const unsigned char *message, size_t mlen,
    const unsigned char rnd[MLDSA_SEEDBYTES],
    const unsigned char sk[CRYPTO_SECRETKEYBYTES], int externalMu)
{
  unsigned char sig[CRYPTO_BYTES];
  size_t siglen;
  CHECK(crypto_sign_signature_internal(sig, &siglen, message, mlen, NULL, 0,
                                       rnd, sk, externalMu) == 0);
  print_hex("signature", sig, sizeof(sig));
}

/* Deterministic signing functions - use all-zero rnd for deterministic mode */

static void acvp_mldsa_sigGenDeterministic_AFT(
    const unsigned char *message, size_t mlen,
    const unsigned char sk[CRYPTO_SECRETKEYBYTES], const unsigned char *context,
    size_t ctxlen)
{
  unsigned char sig[CRYPTO_BYTES];
  size_t siglen;
  unsigned char rnd[MLDSA_SEEDBYTES] = {0}; /* Zero rnd for deterministic */

  unsigned char pre[MAX_CTX_LENGTH + 2];

  CHECK(ctxlen <= 255);
  pre[0] = 0;
  /* Safety: Truncation is safe due to the check above. */
  pre[1] = (uint8_t)ctxlen;
  memcpy(pre + 2, context, ctxlen);

  CHECK(crypto_sign_signature_internal(sig, &siglen, message, mlen, pre,
                                       ctxlen + 2, rnd, sk, 0) == 0);
  print_hex("signature", sig, sizeof(sig));
}

static void acvp_mldsa_sigGenInternalDeterministic_AFT(
    const unsigned char *message, size_t mlen,
    const unsigned char sk[CRYPTO_SECRETKEYBYTES], int externalMu)
{
  unsigned char sig[CRYPTO_BYTES];
  size_t siglen;
  unsigned char rnd[MLDSA_SEEDBYTES] = {0}; /* Zero rnd for deterministic */

  CHECK(crypto_sign_signature_internal(sig, &siglen, message, mlen, NULL, 0,
                                       rnd, sk, externalMu) == 0);
  print_hex("signature", sig, sizeof(sig));
}


static int acvp_mldsa_sigVer_AFT(const unsigned char *message, size_t mlen,
                                 const unsigned char *context, size_t ctxlen,
                                 const unsigned char signature[CRYPTO_BYTES],
                                 const unsigned char pk[CRYPTO_PUBLICKEYBYTES])
{
  return crypto_sign_verify(signature, CRYPTO_BYTES, message, mlen, context,
                            ctxlen, pk);
}


static int acvp_mldsa_sigVerInternal_AFT(
    const unsigned char *message, size_t mlen,
    const unsigned char signature[CRYPTO_BYTES],
    const unsigned char pk[CRYPTO_PUBLICKEYBYTES], int externalMu)
{
  if (externalMu)
  {
    return crypto_sign_verify_extmu(signature, CRYPTO_BYTES, message, pk);
  }
  else
  {
    return crypto_sign_verify_internal(signature, CRYPTO_BYTES, message, mlen,
                                       NULL, 0, pk, 0);
  }
}

static int str_to_hash_alg(const char *hashAlg)
{
  if (strcmp(hashAlg, "SHA2-224") == 0)
  {
    return MLD_PREHASH_SHA2_224;
  }
  if (strcmp(hashAlg, "SHA2-256") == 0)
  {
    return MLD_PREHASH_SHA2_256;
  }
  if (strcmp(hashAlg, "SHA2-384") == 0)
  {
    return MLD_PREHASH_SHA2_384;
  }
  if (strcmp(hashAlg, "SHA2-512") == 0)
  {
    return MLD_PREHASH_SHA2_512;
  }
  if (strcmp(hashAlg, "SHA2-512/224") == 0)
  {
    return MLD_PREHASH_SHA2_512_224;
  }
  if (strcmp(hashAlg, "SHA2-512/256") == 0)
  {
    return MLD_PREHASH_SHA2_512_256;
  }
  if (strcmp(hashAlg, "SHA3-224") == 0)
  {
    return MLD_PREHASH_SHA3_224;
  }
  if (strcmp(hashAlg, "SHA3-256") == 0)
  {
    return MLD_PREHASH_SHA3_256;
  }
  if (strcmp(hashAlg, "SHA3-384") == 0)
  {
    return MLD_PREHASH_SHA3_384;
  }
  if (strcmp(hashAlg, "SHA3-512") == 0)
  {
    return MLD_PREHASH_SHA3_512;
  }
  if (strcmp(hashAlg, "SHAKE-128") == 0)
  {
    return MLD_PREHASH_SHAKE_128;
  }
  if (strcmp(hashAlg, "SHAKE-256") == 0)
  {
    return MLD_PREHASH_SHAKE_256;
  }
  /* Invalid hash algorithm */
  fprintf(stderr, "Error: Unsupported hash algorithm: %s\n", hashAlg);
  exit(1);
}

static int acvp_mldsa_sigGenPreHash_AFT(
    const unsigned char *ph, size_t phlen, const unsigned char *context,
    size_t ctxlen, const unsigned char rng[MLDSA_RNDBYTES],
    const unsigned char sk[CRYPTO_SECRETKEYBYTES], const char *hashAlg)
{
  unsigned char signature[CRYPTO_BYTES];
  size_t siglen;

  if (crypto_sign_signature_pre_hash_internal(signature, &siglen, ph, phlen,
                                              context, ctxlen, rng, sk,
                                              str_to_hash_alg(hashAlg)) != 0)
  {
    return 1;
  }

  print_hex("signature", signature, siglen);
  return 0;
}

static int acvp_mldsa_sigVerPreHash_AFT(
    const unsigned char *ph, size_t phlen, const unsigned char *context,
    size_t ctxlen, const unsigned char signature[CRYPTO_BYTES],
    const unsigned char pk[CRYPTO_PUBLICKEYBYTES], const char *hashAlg)
{
  return crypto_sign_verify_pre_hash_internal(signature, CRYPTO_BYTES, ph,
                                              phlen, context, ctxlen, pk,
                                              str_to_hash_alg(hashAlg));
}

static int acvp_mldsa_sigGenPreHashShake256_AFT(
    const unsigned char *message, size_t mlen, const unsigned char *context,
    size_t ctxlen, const unsigned char rnd[MLDSA_RNDBYTES],
    const unsigned char sk[CRYPTO_SECRETKEYBYTES])
{
  unsigned char signature[CRYPTO_BYTES];
  size_t siglen;

  if (crypto_sign_signature_pre_hash_shake256(signature, &siglen, message, mlen,
                                              context, ctxlen, rnd, sk) != 0)
  {
    return 1;
  }

  print_hex("signature", signature, siglen);
  return 0;
}

static int acvp_mldsa_sigVerPreHashShake256_AFT(
    const unsigned char *message, size_t mlen, const unsigned char *context,
    size_t ctxlen, const unsigned char signature[CRYPTO_BYTES],
    const unsigned char pk[CRYPTO_PUBLICKEYBYTES])
{
  return crypto_sign_verify_pre_hash_shake256(signature, CRYPTO_BYTES, message,
                                              mlen, context, ctxlen, pk);
}

/* Deterministic prehash signing functions */
static int acvp_mldsa_sigGenPreHashDeterministic_AFT(
    const unsigned char *ph, size_t phlen, const unsigned char *context,
    size_t ctxlen, const unsigned char sk[CRYPTO_SECRETKEYBYTES],
    const char *hashAlg)
{
  unsigned char signature[CRYPTO_BYTES];
  size_t siglen;
  unsigned char rnd[MLDSA_RNDBYTES] = {0}; /* Zero rnd for deterministic */

  if (crypto_sign_signature_pre_hash_internal(signature, &siglen, ph, phlen,
                                              context, ctxlen, rnd, sk,
                                              str_to_hash_alg(hashAlg)) != 0)
  {
    return 1;
  }

  print_hex("signature", signature, siglen);
  return 0;
}

static int acvp_mldsa_sigGenPreHashShake256Deterministic_AFT(
    const unsigned char *message, size_t mlen, const unsigned char *context,
    size_t ctxlen, const unsigned char sk[CRYPTO_SECRETKEYBYTES])
{
  unsigned char signature[CRYPTO_BYTES];
  size_t siglen;
  unsigned char rnd[MLDSA_RNDBYTES] = {0}; /* Zero rnd for deterministic */

  if (crypto_sign_signature_pre_hash_shake256(signature, &siglen, message, mlen,
                                              context, ctxlen, rnd, sk) != 0)
  {
    return 1;
  }

  print_hex("signature", signature, siglen);
  return 0;
}


int main(int argc, char *argv[])
{
  acvp_mode mode;

  if (argc == 0)
  {
    goto usage;
  }
  argc--, argv++;

  if (argc == 0)
  {
    goto usage;
  }

  if (strcmp(*argv, "keyGen") == 0)
  {
    mode = keyGen;
  }
  else if (strcmp(*argv, "sigGen") == 0)
  {
    mode = sigGen;
  }
  else if (strcmp(*argv, "sigGenInternal") == 0)
  {
    mode = sigGenInternal;
  }
  else if (strcmp(*argv, "sigGenDeterministic") == 0)
  {
    mode = sigGenDeterministic;
  }
  else if (strcmp(*argv, "sigGenInternalDeterministic") == 0)
  {
    mode = sigGenInternalDeterministic;
  }
  else if (strcmp(*argv, "sigGenPreHashDeterministic") == 0)
  {
    mode = sigGenPreHashDeterministic;
  }
  else if (strcmp(*argv, "sigGenPreHashShake256Deterministic") == 0)
  {
    mode = sigGenPreHashShake256Deterministic;
  }
  else if (strcmp(*argv, "sigVer") == 0)
  {
    mode = sigVer;
  }
  else if (strcmp(*argv, "sigVerInternal") == 0)
  {
    mode = sigVerInternal;
  }
  else if (strcmp(*argv, "sigGenPreHash") == 0)
  {
    mode = sigGenPreHash;
  }
  else if (strcmp(*argv, "sigVerPreHash") == 0)
  {
    mode = sigVerPreHash;
  }
  else if (strcmp(*argv, "sigGenPreHashShake256") == 0)
  {
    mode = sigGenPreHashShake256;
  }
  else if (strcmp(*argv, "sigVerPreHashShake256") == 0)
  {
    mode = sigVerPreHashShake256;
  }
  else
  {
    goto usage;
  }
  argc--, argv++;

  switch (mode)
  {
    case keyGen:
    {
      unsigned char seed[MLDSA_SEEDBYTES];
      /* Parse seed */
      if (argc == 0 || decode_hex("seed", seed, sizeof(seed), *argv) != 0)
      {
        goto keygen_usage;
      }
      argc--, argv++;

      /* Call function under test */
      acvp_mldsa_keyGen_AFT(seed);
      break;
    }

    case sigGen:
    {
      unsigned char message[MAX_MSG_LENGTH];
      unsigned char rnd[MLDSA_RNDBYTES];
      unsigned char context[MAX_CTX_LENGTH];
      unsigned char sk[CRYPTO_SECRETKEYBYTES];
      size_t mlen, ctxlen;

      /* Parse message */
      if (argc == 0)
      {
        goto siggen_usage;
      }
      mlen = (strlen(*argv) - strlen("message=")) / 2;
      if (mlen > MAX_MSG_LENGTH ||
          decode_hex("message", message, mlen, *argv) != 0)
      {
        goto siggen_usage;
      }
      argc--, argv++;

      /* Parse sk */
      if (argc == 0 || decode_hex("sk", sk, sizeof(sk), *argv) != 0)
      {
        goto siggen_usage;
      }
      argc--, argv++;

      /* Parse context */
      if (argc == 0)
      {
        goto siggen_usage;
      }
      ctxlen = (strlen(*argv) - strlen("context=")) / 2;
      if (mlen > MAX_MSG_LENGTH ||
          decode_hex("context", context, ctxlen, *argv) != 0)
      {
        goto siggen_usage;
      }
      argc--, argv++;

      /* Parse rnd */
      if (argc == 0 || decode_hex("rnd", rnd, sizeof(rnd), *argv) != 0)
      {
        goto siggen_usage;
      }
      argc--, argv++;

      /* Call function under test */
      acvp_mldsa_sigGen_AFT(message, mlen, rnd, sk, context, ctxlen);
      break;
    }
    case sigGenInternal:
    {
      unsigned char message[MAX_MSG_LENGTH + MAX_CTX_LENGTH + 2];
      unsigned char rnd[MLDSA_RNDBYTES];
      unsigned char sk[CRYPTO_SECRETKEYBYTES];
      int externalMu;
      size_t mlen;

      /* Parse message */
      if (argc == 0)
      {
        goto siggen_internal_usage;
      }
      mlen = (strlen(*argv) - strlen("message=")) / 2;
      if (mlen > sizeof(message) ||
          decode_hex("message", message, mlen, *argv) != 0)
      {
        goto siggen_internal_usage;
      }
      argc--, argv++;

      /* Parse sk */
      if (argc == 0 || decode_hex("sk", sk, sizeof(sk), *argv) != 0)
      {
        goto siggen_internal_usage;
      }
      argc--, argv++;

      /* Parse externalMu */
      if (argc == 0 ||
          decode_keyed_int("externalMu", &externalMu, *argv) != 0 ||
          externalMu > 1 || externalMu < 0)
      {
        goto siggen_internal_usage;
      }
      argc--, argv++;

      /* Parse rnd */
      if (argc == 0 || decode_hex("rnd", rnd, sizeof(rnd), *argv) != 0)
      {
        goto siggen_internal_usage;
      }
      argc--, argv++;


      /* Call function under test */
      acvp_mldsa_sigGenInternal_AFT(message, mlen, rnd, sk, externalMu);
      break;
    }

    case sigGenDeterministic:
    {
      unsigned char message[MAX_MSG_LENGTH];
      unsigned char context[MAX_CTX_LENGTH];
      unsigned char sk[CRYPTO_SECRETKEYBYTES];
      size_t mlen, ctxlen;

      /* Parse message */
      if (argc == 0)
      {
        goto siggen_deterministic_usage;
      }
      mlen = (strlen(*argv) - strlen("message=")) / 2;
      if (mlen > MAX_MSG_LENGTH ||
          decode_hex("message", message, mlen, *argv) != 0)
      {
        goto siggen_deterministic_usage;
      }
      argc--, argv++;

      /* Parse sk */
      if (argc == 0 || decode_hex("sk", sk, sizeof(sk), *argv) != 0)
      {
        goto siggen_deterministic_usage;
      }
      argc--, argv++;

      /* Parse context */
      if (argc == 0)
      {
        goto siggen_deterministic_usage;
      }
      ctxlen = (strlen(*argv) - strlen("context=")) / 2;
      if (mlen > MAX_MSG_LENGTH ||
          decode_hex("context", context, ctxlen, *argv) != 0)
      {
        goto siggen_deterministic_usage;
      }
      argc--, argv++;

      /* Call function under test */
      acvp_mldsa_sigGenDeterministic_AFT(message, mlen, sk, context, ctxlen);
      break;
    }

    case sigGenInternalDeterministic:
    {
      unsigned char message[MAX_MSG_LENGTH + MAX_CTX_LENGTH + 2];
      unsigned char sk[CRYPTO_SECRETKEYBYTES];
      int externalMu;
      size_t mlen;

      /* Parse message */
      if (argc == 0)
      {
        goto siggen_internal_deterministic_usage;
      }
      mlen = (strlen(*argv) - strlen("message=")) / 2;
      if (mlen > sizeof(message) ||
          decode_hex("message", message, mlen, *argv) != 0)
      {
        goto siggen_internal_deterministic_usage;
      }
      argc--, argv++;

      /* Parse sk */
      if (argc == 0 || decode_hex("sk", sk, sizeof(sk), *argv) != 0)
      {
        goto siggen_internal_deterministic_usage;
      }
      argc--, argv++;

      /* Parse externalMu */
      if (argc == 0 ||
          decode_keyed_int("externalMu", &externalMu, *argv) != 0 ||
          externalMu > 1 || externalMu < 0)
      {
        goto siggen_internal_deterministic_usage;
      }
      argc--, argv++;

      /* Call function under test */
      acvp_mldsa_sigGenInternalDeterministic_AFT(message, mlen, sk, externalMu);
      break;
    }

    case sigVer:
    {
      unsigned char message[MAX_MSG_LENGTH];
      unsigned char context[MAX_CTX_LENGTH];
      unsigned char signature[CRYPTO_BYTES];
      unsigned char pk[CRYPTO_PUBLICKEYBYTES];
      size_t mlen, ctxlen;

      /* Parse message */
      if (argc == 0)
      {
        goto sigver_usage;
      }
      mlen = (strlen(*argv) - strlen("message=")) / 2;
      if (mlen > MAX_MSG_LENGTH ||
          decode_hex("message", message, mlen, *argv) != 0)
      {
        goto sigver_usage;
      }
      argc--, argv++;

      /* Parse context */
      if (argc == 0)
      {
        goto sigver_usage;
      }
      ctxlen = (strlen(*argv) - strlen("context=")) / 2;
      if (mlen > MAX_MSG_LENGTH ||
          decode_hex("context", context, ctxlen, *argv) != 0)
      {
        goto sigver_usage;
      }
      argc--, argv++;

      /* Parse signature */
      if (argc == 0 ||
          decode_hex("signature", signature, sizeof(signature), *argv) != 0)
      {
        goto sigver_usage;
      }
      argc--, argv++;


      /* Parse pk */
      if (argc == 0 || decode_hex("pk", pk, sizeof(pk), *argv) != 0)
      {
        goto sigver_usage;
      }
      argc--, argv++;


      /* Call function under test */
      return acvp_mldsa_sigVer_AFT(message, mlen, context, ctxlen, signature,
                                   pk);
    }


    case sigVerInternal:
    {
      unsigned char message[MAX_MSG_LENGTH];
      unsigned char signature[CRYPTO_BYTES];
      unsigned char pk[CRYPTO_PUBLICKEYBYTES];
      size_t mlen;
      int externalMu;

      /* Parse message */
      if (argc == 0)
      {
        goto sigver_internal_usage;
      }
      mlen = (strlen(*argv) - strlen("message=")) / 2;
      if (mlen > MAX_MSG_LENGTH ||
          decode_hex("message", message, mlen, *argv) != 0)
      {
        goto sigver_internal_usage;
      }
      argc--, argv++;

      /* Parse signature */
      if (argc == 0 ||
          decode_hex("signature", signature, sizeof(signature), *argv) != 0)
      {
        goto sigver_internal_usage;
      }
      argc--, argv++;


      /* Parse pk */
      if (argc == 0 || decode_hex("pk", pk, sizeof(pk), *argv) != 0)
      {
        goto sigver_internal_usage;
      }
      argc--, argv++;

      /* Parse externalMu */
      if (argc == 0 ||
          decode_keyed_int("externalMu", &externalMu, *argv) != 0 ||
          externalMu > 1 || externalMu < 0)
      {
        goto sigver_internal_usage;
      }
      argc--, argv++;



      /* Call function under test */
      return acvp_mldsa_sigVerInternal_AFT(message, mlen, signature, pk,
                                           externalMu);
    }

    case sigGenPreHash:
    {
      unsigned char ph[64];
      unsigned char context[MAX_CTX_LENGTH];
      unsigned char rnd[MLDSA_RNDBYTES];
      unsigned char sk[CRYPTO_SECRETKEYBYTES];
      char hashAlg[100];
      size_t phlen;
      size_t ctxlen;

      /* Parse ph */
      if (argc == 0)
      {
        goto siggen_prehash_usage;
      }
      phlen = (strlen(*argv) - strlen("ph=")) / 2;
      if (phlen > 64 || decode_hex("ph", ph, phlen, *argv) != 0)
      {
        goto siggen_prehash_usage;
      }
      argc--, argv++;

      /* Parse context */
      if (argc == 0)
      {
        goto siggen_prehash_usage;
      }
      ctxlen = (strlen(*argv) - strlen("context=")) / 2;
      if (ctxlen > MAX_CTX_LENGTH ||
          decode_hex("context", context, ctxlen, *argv) != 0)
      {
        goto siggen_prehash_usage;
      }
      argc--, argv++;

      /* Parse sk */
      if (argc == 0 || decode_hex("sk", sk, sizeof(sk), *argv) != 0)
      {
        goto siggen_prehash_usage;
      }
      argc--, argv++;

      /* Parse hashAlg */
      if (argc == 0 ||
          parse_str("hashAlg", hashAlg, sizeof(hashAlg), *argv) != 0)
      {
        goto siggen_prehash_usage;
      }
      argc--, argv++;

      /* Parse rnd */
      if (argc == 0 || decode_hex("rnd", rnd, sizeof(rnd), *argv) != 0)
      {
        goto siggen_prehash_usage;
      }
      argc--, argv++;

      /* Call function under test */
      return acvp_mldsa_sigGenPreHash_AFT(ph, phlen, context, ctxlen, rnd, sk,
                                          hashAlg);
    }

    case sigVerPreHash:
    {
      unsigned char ph[64];
      unsigned char context[MAX_CTX_LENGTH];
      unsigned char signature[CRYPTO_BYTES];
      unsigned char pk[CRYPTO_PUBLICKEYBYTES];
      char hashAlg[100];
      size_t phlen;
      size_t ctxlen;

      /* Parse message */
      if (argc == 0)
      {
        goto sigver_prehash_usage;
      }
      phlen = (strlen(*argv) - strlen("ph=")) / 2;
      if (phlen > 64 || decode_hex("ph", ph, phlen, *argv) != 0)
      {
        goto sigver_prehash_usage;
      }
      argc--, argv++;

      /* Parse context */
      if (argc == 0)
      {
        goto sigver_prehash_usage;
      }
      ctxlen = (strlen(*argv) - strlen("context=")) / 2;
      if (ctxlen > MAX_CTX_LENGTH ||
          decode_hex("context", context, ctxlen, *argv) != 0)
      {
        goto sigver_prehash_usage;
      }
      argc--, argv++;

      /* Parse signature */
      if (argc == 0 ||
          decode_hex("signature", signature, sizeof(signature), *argv) != 0)
      {
        goto sigver_prehash_usage;
      }
      argc--, argv++;


      /* Parse pk */
      if (argc == 0 || decode_hex("pk", pk, sizeof(pk), *argv) != 0)
      {
        goto sigver_prehash_usage;
      }
      argc--, argv++;

      /* Parse hashAlg */
      if (argc == 0 ||
          parse_str("hashAlg", hashAlg, sizeof(hashAlg), *argv) != 0)
      {
        goto sigver_prehash_usage;
      }
      argc--, argv++;



      /* Call function under test */
      return acvp_mldsa_sigVerPreHash_AFT(ph, phlen, context, ctxlen, signature,
                                          pk, hashAlg);
    }

    case sigGenPreHashShake256:
    {
      unsigned char message[MAX_MSG_LENGTH];
      unsigned char context[MAX_CTX_LENGTH];
      unsigned char rnd[MLDSA_RNDBYTES];
      unsigned char sk[CRYPTO_SECRETKEYBYTES];
      size_t mlen;
      size_t ctxlen;

      /* Parse message */
      if (argc == 0)
      {
        goto siggen_prehash_shake256_usage;
      }
      mlen = (strlen(*argv) - strlen("message=")) / 2;
      if (mlen > MAX_MSG_LENGTH ||
          decode_hex("message", message, mlen, *argv) != 0)
      {
        goto siggen_prehash_shake256_usage;
      }
      argc--, argv++;

      /* Parse context */
      if (argc == 0)
      {
        goto siggen_prehash_shake256_usage;
      }
      ctxlen = (strlen(*argv) - strlen("context=")) / 2;
      if (ctxlen > MAX_CTX_LENGTH ||
          decode_hex("context", context, ctxlen, *argv) != 0)
      {
        goto siggen_prehash_shake256_usage;
      }
      argc--, argv++;

      /* Parse sk */
      if (argc == 0 || decode_hex("sk", sk, sizeof(sk), *argv) != 0)
      {
        goto siggen_prehash_shake256_usage;
      }
      argc--, argv++;

      /* Parse rnd */
      if (argc == 0 || decode_hex("rnd", rnd, sizeof(rnd), *argv) != 0)
      {
        goto siggen_prehash_shake256_usage;
      }
      argc--, argv++;

      /* Call function under test */
      return acvp_mldsa_sigGenPreHashShake256_AFT(message, mlen, context,
                                                  ctxlen, rnd, sk);
    }

    case sigGenPreHashDeterministic:
    {
      unsigned char ph[64];
      unsigned char context[MAX_CTX_LENGTH];
      unsigned char sk[CRYPTO_SECRETKEYBYTES];
      char hashAlg[100];
      size_t phlen;
      size_t ctxlen;

      /* Parse ph */
      if (argc == 0)
      {
        goto siggen_prehash_deterministic_usage;
      }
      phlen = (strlen(*argv) - strlen("ph=")) / 2;
      if (phlen > 64 || decode_hex("ph", ph, phlen, *argv) != 0)
      {
        goto siggen_prehash_deterministic_usage;
      }
      argc--, argv++;

      /* Parse context */
      if (argc == 0)
      {
        goto siggen_prehash_deterministic_usage;
      }
      ctxlen = (strlen(*argv) - strlen("context=")) / 2;
      if (ctxlen > MAX_CTX_LENGTH ||
          decode_hex("context", context, ctxlen, *argv) != 0)
      {
        goto siggen_prehash_deterministic_usage;
      }
      argc--, argv++;

      /* Parse sk */
      if (argc == 0 || decode_hex("sk", sk, sizeof(sk), *argv) != 0)
      {
        goto siggen_prehash_deterministic_usage;
      }
      argc--, argv++;

      /* Parse hashAlg */
      if (argc == 0 ||
          parse_str("hashAlg", hashAlg, sizeof(hashAlg), *argv) != 0)
      {
        goto siggen_prehash_deterministic_usage;
      }
      argc--, argv++;

      /* Call function under test */
      return acvp_mldsa_sigGenPreHashDeterministic_AFT(ph, phlen, context,
                                                       ctxlen, sk, hashAlg);
    }

    case sigGenPreHashShake256Deterministic:
    {
      unsigned char message[MAX_MSG_LENGTH];
      unsigned char context[MAX_CTX_LENGTH];
      unsigned char sk[CRYPTO_SECRETKEYBYTES];
      size_t mlen;
      size_t ctxlen;

      /* Parse message */
      if (argc == 0)
      {
        goto siggen_prehash_shake256_deterministic_usage;
      }
      mlen = (strlen(*argv) - strlen("message=")) / 2;
      if (mlen > MAX_MSG_LENGTH ||
          decode_hex("message", message, mlen, *argv) != 0)
      {
        goto siggen_prehash_shake256_deterministic_usage;
      }
      argc--, argv++;

      /* Parse context */
      if (argc == 0)
      {
        goto siggen_prehash_shake256_deterministic_usage;
      }
      ctxlen = (strlen(*argv) - strlen("context=")) / 2;
      if (ctxlen > MAX_CTX_LENGTH ||
          decode_hex("context", context, ctxlen, *argv) != 0)
      {
        goto siggen_prehash_shake256_deterministic_usage;
      }
      argc--, argv++;

      /* Parse sk */
      if (argc == 0 || decode_hex("sk", sk, sizeof(sk), *argv) != 0)
      {
        goto siggen_prehash_shake256_deterministic_usage;
      }
      argc--, argv++;

      /* Call function under test */
      return acvp_mldsa_sigGenPreHashShake256Deterministic_AFT(
          message, mlen, context, ctxlen, sk);
    }

    case sigVerPreHashShake256:
    {
      unsigned char message[MAX_MSG_LENGTH];
      unsigned char context[MAX_CTX_LENGTH];
      unsigned char signature[CRYPTO_BYTES];
      unsigned char pk[CRYPTO_PUBLICKEYBYTES];
      size_t mlen;
      size_t ctxlen;

      /* Parse message */
      if (argc == 0)
      {
        goto sigver_prehash_shake256_usage;
      }
      mlen = (strlen(*argv) - strlen("message=")) / 2;
      if (mlen > MAX_MSG_LENGTH ||
          decode_hex("message", message, mlen, *argv) != 0)
      {
        goto sigver_prehash_shake256_usage;
      }
      argc--, argv++;

      /* Parse context */
      if (argc == 0)
      {
        goto sigver_prehash_shake256_usage;
      }
      ctxlen = (strlen(*argv) - strlen("context=")) / 2;
      if (ctxlen > MAX_CTX_LENGTH ||
          decode_hex("context", context, ctxlen, *argv) != 0)
      {
        goto sigver_prehash_shake256_usage;
      }
      argc--, argv++;

      /* Parse signature */
      if (argc == 0 ||
          decode_hex("signature", signature, sizeof(signature), *argv) != 0)
      {
        goto sigver_prehash_shake256_usage;
      }
      argc--, argv++;

      /* Parse pk */
      if (argc == 0 || decode_hex("pk", pk, sizeof(pk), *argv) != 0)
      {
        goto sigver_prehash_shake256_usage;
      }
      argc--, argv++;

      /* Call function under test */
      return acvp_mldsa_sigVerPreHashShake256_AFT(message, mlen, context,
                                                  ctxlen, signature, pk);
    }
  }

  return (0);

usage:
  fprintf(stderr, USAGE "\n");
  return (1);

keygen_usage:
  fprintf(stderr, KEYGEN_USAGE "\n");
  return (1);

siggen_usage:
  fprintf(stderr, SIGGEN_USAGE "\n");
  return (1);

siggen_internal_usage:
  fprintf(stderr, SIGGEN_INTERNAL_USAGE "\n");
  return (1);

siggen_deterministic_usage:
  fprintf(stderr, SIGGEN_DETERMINISTIC_USAGE "\n");
  return (1);

siggen_internal_deterministic_usage:
  fprintf(stderr, SIGGEN_INTERNAL_DETERMINISTIC_USAGE "\n");
  return (1);

siggen_prehash_deterministic_usage:
  fprintf(stderr, SIGGEN_PREHASH_DETERMINISTIC_USAGE "\n");
  return (1);

siggen_prehash_shake256_deterministic_usage:
  fprintf(stderr, SIGGEN_PREHASH_SHAKE256_DETERMINISTIC_USAGE "\n");
  return (1);

sigver_usage:
  fprintf(stderr, SIGVER_USAGE "\n");
  return (1);

sigver_internal_usage:
  fprintf(stderr, SIGVER_INTERNAL_USAGE "\n");
  return (1);

siggen_prehash_usage:
  fprintf(stderr, SIGGEN_PREHASH_USAGE "\n");
  return (1);

sigver_prehash_usage:
  fprintf(stderr, SIGVER_PREHASH_USAGE "\n");
  return (1);

siggen_prehash_shake256_usage:
  fprintf(stderr, SIGGEN_PREHASH_SHAKE256_USAGE "\n");
  return (1);

sigver_prehash_shake256_usage:
  fprintf(stderr, SIGVER_PREHASH_SHAKE256_USAGE "\n");
  return (1);
}
