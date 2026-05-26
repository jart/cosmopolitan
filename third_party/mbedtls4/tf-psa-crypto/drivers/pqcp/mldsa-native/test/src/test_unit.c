/*
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "../notrandombytes/notrandombytes.h"

#include "../../mldsa/src/poly.h"
#include "../../mldsa/src/poly_kl.h"
#include "../../mldsa/src/polyvec.h"

#ifndef NUM_RANDOM_TESTS
#ifdef MLDSA_DEBUG
#define NUM_RANDOM_TESTS 1000
#else
#define NUM_RANDOM_TESTS 5000
#endif
#endif /* !NUM_RANDOM_TESTS */

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

/* Declarations for _c functions exposed by MLD_STATIC_TESTABLE= */
void mld_poly_ntt_c(mld_poly *a);
void mld_poly_invntt_tomont_c(mld_poly *a);
void mld_poly_caddq_c(mld_poly *a);
void mld_poly_decompose_c(mld_poly *a1, mld_poly *a0);
void mld_poly_use_hint_c(mld_poly *b, const mld_poly *a, const mld_poly *h);
uint32_t mld_poly_chknorm_c(const mld_poly *a, int32_t B);
void mld_poly_pointwise_montgomery_c(mld_poly *c, const mld_poly *a,
                                     const mld_poly *b);
void mld_polyvecl_pointwise_acc_montgomery_c(mld_poly *w, const mld_polyvecl *u,
                                             const mld_polyvecl *v);
void mld_polyz_unpack_c(mld_poly *r, const uint8_t a[MLDSA_POLYZ_PACKEDBYTES]);
#if defined(MLD_USE_NATIVE_NTT) || defined(MLD_USE_NATIVE_INTT) ||  \
    defined(MLD_USE_NATIVE_POLY_DECOMPOSE_32) ||                    \
    defined(MLD_USE_NATIVE_POLY_DECOMPOSE_88) ||                    \
    defined(MLD_USE_NATIVE_POLY_CADDQ) ||                           \
    defined(MLD_USE_NATIVE_POLY_USE_HINT_88) ||                     \
    defined(MLD_USE_NATIVE_POLY_USE_HINT_32) ||                     \
    defined(MLD_USE_NATIVE_POLY_CHKNORM) ||                         \
    defined(MLD_USE_NATIVE_POINTWISE_MONTGOMERY) ||                 \
    defined(MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L4) || \
    defined(MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L5) || \
    defined(MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L7) || \
    defined(MLD_USE_NATIVE_POLYZ_UNPACK_17) ||                      \
    defined(MLD_USE_NATIVE_POLYZ_UNPACK_19)
/* Backend unit test helper functions */
static void print_i32_array(const char *label, const int32_t *array, size_t len)
{
  size_t i;
  fprintf(stderr, "%s:\n", label);
  for (i = 0; i < len; i++)
  {
    if (i % 8 == 0)
    {
      fprintf(stderr, "  ");
    }
    fprintf(stderr, "%8d", array[i]);
    if (i % 8 == 7)
    {
      fprintf(stderr, "\n");
    }
    else
    {
      fprintf(stderr, " ");
    }
  }
  if (len % 8 != 0)
  {
    fprintf(stderr, "\n");
  }
}

static void generate_i32_array_zeros(int32_t *data, size_t len)
{
  memset(data, 0, len * sizeof(int32_t));
}

static void generate_i32_array_single(int32_t *data, size_t len, size_t pos,
                                      int32_t value)
{
  memset(data, 0, len * sizeof(int32_t));
  data[pos] = value;
}

/* This does not generate a uniformly random distribution, but it's
 * good enough for our test.
 *
 * The lower bound is inclusive, the upper bound exclusive, matching
 * the CBMC assertions in the code base. */
static void generate_i32_array_ranged(int32_t *data, size_t len, int min_incl,
                                      int max_excl)
{
  size_t i;

  randombytes((uint8_t *)data, len * sizeof(int32_t));
  for (i = 0; i < len; i++)
  {
    data[i] = (int32_t)((unsigned)min_incl +
                        ((unsigned)data[i] % (unsigned)(max_excl - min_incl)));
  }
}

static int compare_i32_arrays(const int32_t *a, const int32_t *b, unsigned len,
                              const char *test_name, const int32_t *input)
{
  unsigned i;
  for (i = 0; i < len; i++)
  {
    if (a[i] != b[i])
    {
      fprintf(stderr, "FAIL: %s\n", test_name);
      fprintf(stderr,
              "  First difference at index %u: native=%d, reference=%d\n", i,
              a[i], b[i]);
      if (input)
      {
        print_i32_array("Input", input, len);
      }
      print_i32_array("Native result", a, len);
      print_i32_array("Reference result", b, len);
      return 0;
    }
  }
  return 1;
}

#ifdef MLD_USE_NATIVE_NTT
static int test_ntt_core(const int32_t *input, const char *test_name)
{
  mld_poly test_poly, ref_poly;

  memcpy(test_poly.coeffs, input, MLDSA_N * sizeof(int32_t));
  memcpy(ref_poly.coeffs, input, MLDSA_N * sizeof(int32_t));

  mld_poly_ntt(&test_poly);
  mld_poly_ntt_c(&ref_poly);

#ifdef MLD_USE_NATIVE_NTT_CUSTOM_ORDER
  mld_poly_permute_bitrev_to_custom(ref_poly.coeffs);
#endif

  /* Normalize */
  mld_poly_reduce(&ref_poly);
  mld_poly_reduce(&test_poly);

  mld_poly_caddq_c(&ref_poly);
  mld_poly_caddq_c(&test_poly);

  CHECK(compare_i32_arrays(test_poly.coeffs, ref_poly.coeffs, MLDSA_N,
                           test_name, input));
  return 0;
}

static int test_native_ntt(void)
{
  int32_t test_data[MLDSA_N];
  int pos, i;

  generate_i32_array_zeros(test_data, MLDSA_N);
  CHECK(test_ntt_core(test_data, "ntt_zeros") == 0);

  for (pos = 0; pos < MLDSA_N; pos += MLDSA_N / 8)
  {
    generate_i32_array_single(test_data, MLDSA_N, (size_t)pos, 1);
    CHECK(test_ntt_core(test_data, "ntt_single") == 0);
  }

  for (i = 0; i < NUM_RANDOM_TESTS; i++)
  {
    generate_i32_array_ranged(test_data, MLDSA_N, -MLDSA_Q + 1, MLDSA_Q);
    CHECK(test_ntt_core(test_data, "ntt_random") == 0);
  }

  return 0;
}
#endif /* MLD_USE_NATIVE_NTT */

#ifdef MLD_USE_NATIVE_INTT
static int test_invntt_tomont_core(const int32_t *input, const char *test_name)
{
  mld_poly test_poly, ref_poly;

  memcpy(test_poly.coeffs, input, MLDSA_N * sizeof(int32_t));
  memcpy(ref_poly.coeffs, input, MLDSA_N * sizeof(int32_t));

#ifdef MLD_USE_NATIVE_NTT_CUSTOM_ORDER
  mld_poly_permute_bitrev_to_custom(test_poly.coeffs);
#endif

  mld_poly_invntt_tomont(&test_poly);
  mld_poly_invntt_tomont_c(&ref_poly);

  /* Normalize */
  mld_poly_reduce(&ref_poly);
  mld_poly_reduce(&test_poly);

  mld_poly_caddq_c(&ref_poly);
  mld_poly_caddq_c(&test_poly);

  CHECK(compare_i32_arrays(test_poly.coeffs, ref_poly.coeffs, MLDSA_N,
                           test_name, input));
  return 0;
}

static int test_native_invntt_tomont(void)
{
  int32_t test_data[MLDSA_N];
  int pos, i;

  generate_i32_array_zeros(test_data, MLDSA_N);
  CHECK(test_invntt_tomont_core(test_data, "invntt_tomont_zeros") == 0);

  for (pos = 0; pos < MLDSA_N; pos += MLDSA_N / 8)
  {
    generate_i32_array_single(test_data, MLDSA_N, (size_t)pos, 1);
    CHECK(test_invntt_tomont_core(test_data, "invntt_tomont_single") == 0);
  }

  for (i = 0; i < NUM_RANDOM_TESTS; i++)
  {
    generate_i32_array_ranged(test_data, MLDSA_N, -MLDSA_Q + 1, MLDSA_Q);
    CHECK(test_invntt_tomont_core(test_data, "invntt_tomont_random") == 0);
  }

  return 0;
}
#endif /* MLD_USE_NATIVE_INTT */

#if defined(MLD_USE_NATIVE_POLY_DECOMPOSE_32) || \
    defined(MLD_USE_NATIVE_POLY_DECOMPOSE_88)
static int test_poly_decompose_core(const mld_poly *input_poly,
                                    const char *test_name)
{
  mld_poly test_a1, test_a0, ref_a1, ref_a0;

  mld_memcpy(&test_a0, input_poly, sizeof(mld_poly));
  mld_memcpy(&ref_a0, input_poly, sizeof(mld_poly));

  mld_poly_decompose(&test_a1, &test_a0);
  mld_poly_decompose_c(&ref_a1, &ref_a0);

  CHECK(compare_i32_arrays(test_a1.coeffs, ref_a1.coeffs, MLDSA_N, test_name,
                           input_poly->coeffs));
  CHECK(compare_i32_arrays(test_a0.coeffs, ref_a0.coeffs, MLDSA_N, test_name,
                           input_poly->coeffs));
  return 0;
}
static int test_native_decompose(void)
{
  mld_poly test_poly;
  int i;

  generate_i32_array_zeros(test_poly.coeffs, MLDSA_N);
  CHECK(test_poly_decompose_core(&test_poly, "poly_decompose_zeros") == 0);

  for (i = 0; i < NUM_RANDOM_TESTS; i++)
  {
    generate_i32_array_ranged(test_poly.coeffs, MLDSA_N, 0, MLDSA_Q);
    CHECK(test_poly_decompose_core(&test_poly, "poly_decompose_random") == 0);
  }

  return 0;
}
#endif /* MLD_USE_NATIVE_POLY_DECOMPOSE_32 || MLD_USE_NATIVE_POLY_DECOMPOSE_88 \
        */

#if defined(MLD_USE_NATIVE_POLY_CADDQ)
static int test_caddq_core(const int32_t *input, const char *test_name)
{
  mld_poly test_poly, ref_poly;

  memcpy(test_poly.coeffs, input, MLDSA_N * sizeof(int32_t));
  memcpy(ref_poly.coeffs, input, MLDSA_N * sizeof(int32_t));

  mld_poly_caddq(&test_poly);
  mld_poly_caddq_c(&ref_poly);

  CHECK(compare_i32_arrays(test_poly.coeffs, ref_poly.coeffs, MLDSA_N,
                           test_name, input));
  return 0;
}
static int test_native_caddq(void)
{
  int32_t test_data[MLDSA_N];
  int pos, i;

  generate_i32_array_zeros(test_data, MLDSA_N);
  CHECK(test_caddq_core(test_data, "poly_caddq_zeros") == 0);

  for (pos = 0; pos < MLDSA_N; pos += MLDSA_N / 8)
  {
    generate_i32_array_single(test_data, MLDSA_N, (size_t)pos, 1);
    CHECK(test_caddq_core(test_data, "poly_caddq_single") == 0);
  }

  for (i = 0; i < NUM_RANDOM_TESTS; i++)
  {
    generate_i32_array_ranged(test_data, MLDSA_N, -MLDSA_Q + 1, MLDSA_Q);
    CHECK(test_caddq_core(test_data, "poly_caddq_random") == 0);
  }

  return 0;
}
#endif /* MLD_USE_NATIVE_POLY_CADDQ */

#if defined(MLD_USE_NATIVE_POLY_USE_HINT_88) || \
    defined(MLD_USE_NATIVE_POLY_USE_HINT_32)
static int test_poly_use_hint_core(const mld_poly *poly_a,
                                   const mld_poly *poly_h,
                                   const char *test_name)
{
  mld_poly test_b, ref_b;

  mld_poly_use_hint(&test_b, poly_a, poly_h);
  mld_poly_use_hint_c(&ref_b, poly_a, poly_h);

  CHECK(compare_i32_arrays(test_b.coeffs, ref_b.coeffs, MLDSA_N, test_name,
                           poly_a->coeffs));
  return 0;
}
static int test_native_use_hint(void)
{
  mld_poly poly_a, poly_h;
  int i;
  generate_i32_array_zeros(poly_a.coeffs, MLDSA_N);
  generate_i32_array_zeros(poly_h.coeffs, MLDSA_N);
  CHECK(test_poly_use_hint_core(&poly_a, &poly_h, "poly_use_hint_zeros") == 0);

  for (i = 0; i < NUM_RANDOM_TESTS; i++)
  {
    generate_i32_array_ranged(poly_a.coeffs, MLDSA_N, 0, MLDSA_Q);
    generate_i32_array_ranged(poly_h.coeffs, MLDSA_N, 0, 2);
    CHECK(test_poly_use_hint_core(&poly_a, &poly_h, "poly_use_hint_random") ==
          0);
  }

  return 0;
}
#endif /* MLD_USE_NATIVE_POLY_USE_HINT_88 || MLD_USE_NATIVE_POLY_USE_HINT_32 \
        */

#if defined(MLD_USE_NATIVE_POLY_CHKNORM)
static int test_poly_chknorm_core(const mld_poly *input_poly, int32_t B,
                                  const char *test_name)
{
  uint32_t test_result, ref_result;

  test_result = mld_poly_chknorm(input_poly, B);
  ref_result = mld_poly_chknorm_c(input_poly, B);

  if (test_result != ref_result)
  {
    fprintf(stderr, "FAIL: %s - result mismatch: native=%u, ref=%u\n",
            test_name, test_result, ref_result);
    return 1;
  }

  return 0;
}

static int test_native_poly_chknorm(void)
{
  mld_poly test_poly;
  int i;

  for (i = 0; i < NUM_RANDOM_TESTS; i++)
  {
    generate_i32_array_ranged(test_poly.coeffs, MLDSA_N, -REDUCE32_RANGE_MAX,
                              REDUCE32_RANGE_MAX);
    CHECK(test_poly_chknorm_core(&test_poly, MLDSA_Q - REDUCE32_RANGE_MAX,
                                 "poly_chknorm_MAX_B") == 0);
    CHECK(test_poly_chknorm_core(&test_poly, MLDSA_GAMMA1 - MLDSA_BETA,
                                 "poly_chknorm_gamma1_minus_beta") == 0);
    CHECK(test_poly_chknorm_core(&test_poly, MLDSA_GAMMA2 - MLDSA_BETA,
                                 "poly_chknorm_gamma2_minus_beta") == 0);
    CHECK(test_poly_chknorm_core(&test_poly, MLDSA_GAMMA2,
                                 "poly_chknorm_gamma2") == 0);
  }

  return 0;
}
#endif /* MLD_USE_NATIVE_POLY_CHKNORM */

#if defined(MLD_USE_NATIVE_POINTWISE_MONTGOMERY)
static int test_poly_pointwise_montgomery_core(const mld_poly *poly_a,
                                               const mld_poly *poly_b,
                                               const char *test_name)
{
  mld_poly test_poly_c, ref_poly_c;

  mld_poly_pointwise_montgomery(&test_poly_c, poly_a, poly_b);
  mld_poly_pointwise_montgomery_c(&ref_poly_c, poly_a, poly_b);

  CHECK(compare_i32_arrays(test_poly_c.coeffs, ref_poly_c.coeffs, MLDSA_N,
                           test_name, poly_a->coeffs));
  return 0;
}

static int test_native_pointwise_montgomery(void)
{
  mld_poly test_poly_a, test_poly_b;
  int pos, i;

  generate_i32_array_zeros(test_poly_a.coeffs, MLDSA_N);
  generate_i32_array_zeros(test_poly_b.coeffs, MLDSA_N);
  CHECK(test_poly_pointwise_montgomery_core(&test_poly_a, &test_poly_b,
                                            "pointwise_montgomery_zeros") == 0);

  for (pos = 0; pos < MLDSA_N; pos += MLDSA_N / 8)
  {
    generate_i32_array_single(test_poly_a.coeffs, MLDSA_N, (size_t)pos, 1);
    generate_i32_array_single(test_poly_b.coeffs, MLDSA_N, (size_t)pos, 1);
    CHECK(test_poly_pointwise_montgomery_core(
              &test_poly_a, &test_poly_b, "pointwise_montgomery_single") == 0);
  }

  for (i = 0; i < NUM_RANDOM_TESTS; i++)
  {
    generate_i32_array_ranged(test_poly_a.coeffs, MLDSA_N, -(MLD_NTT_BOUND - 1),
                              MLD_NTT_BOUND);
    generate_i32_array_ranged(test_poly_b.coeffs, MLDSA_N, -(MLD_NTT_BOUND - 1),
                              MLD_NTT_BOUND);
    CHECK(test_poly_pointwise_montgomery_core(
              &test_poly_a, &test_poly_b, "pointwise_montgomery_random") == 0);
  }

  return 0;
}
#endif /* MLD_USE_NATIVE_POINTWISE_MONTGOMERY */

#if defined(MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L4) || \
    defined(MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L5) || \
    defined(MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L7)
static int test_polyvecl_pointwise_acc_montgomery_core(const mld_polyvecl *u,
                                                       const mld_polyvecl *v,
                                                       const char *test_name)
{
  mld_poly test_w, ref_w;

  mld_polyvecl_pointwise_acc_montgomery(&test_w, u, v);
  mld_polyvecl_pointwise_acc_montgomery_c(&ref_w, u, v);

  CHECK(compare_i32_arrays(test_w.coeffs, ref_w.coeffs, MLDSA_N, test_name,
                           NULL));
  return 0;
}

static int test_native_polyvecl_pointwise_acc_montgomery(void)
{
  mld_polyvecl u, v;
  unsigned int i;

  /* Test with zeros */
  generate_i32_array_zeros((int32_t *)&u, MLDSA_L * MLDSA_N);
  generate_i32_array_zeros((int32_t *)&v, MLDSA_L * MLDSA_N);
  CHECK(test_polyvecl_pointwise_acc_montgomery_core(&u, &v,
                                                    "polyvecl_acc_zeros") == 0);

  /* Test with random values */
  for (i = 0; i < NUM_RANDOM_TESTS; i++)
  {
    generate_i32_array_ranged((int32_t *)&u, MLDSA_L * MLDSA_N, 0, MLDSA_Q);
    generate_i32_array_ranged((int32_t *)&v, MLDSA_L * MLDSA_N,
                              -MLD_NTT_BOUND + 1, MLD_NTT_BOUND);
    CHECK(test_polyvecl_pointwise_acc_montgomery_core(
              &u, &v, "polyvecl_acc_random") == 0);
  }

  return 0;
}
#endif /* MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L4 || \
          MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L5 || \
          MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L7 */


#if defined(MLD_USE_NATIVE_POLYZ_UNPACK_17) || \
    defined(MLD_USE_NATIVE_POLYZ_UNPACK_19)
static int test_mld_polyz_unpack_core(const uint8_t *input,
                                      const char *test_name)
{
  mld_poly test_poly, ref_poly;

  mld_polyz_unpack(&test_poly, input);
  mld_polyz_unpack_c(&ref_poly, input);

  CHECK(compare_i32_arrays(test_poly.coeffs, ref_poly.coeffs, MLDSA_N,
                           test_name, NULL));
  return 0;
}

static int test_native_polyz_unpack(void)
{
  uint8_t test_bytes[MLDSA_POLYZ_PACKEDBYTES];
  int i;

  memset(test_bytes, 0, MLDSA_POLYZ_PACKEDBYTES);
  CHECK(test_mld_polyz_unpack_core(test_bytes, "polyz_unpack_zeros") == 0);


  for (i = 0; i < NUM_RANDOM_TESTS; i++)
  {
    randombytes(test_bytes, MLDSA_POLYZ_PACKEDBYTES);
    CHECK(test_mld_polyz_unpack_core(test_bytes, "polyz_unpack_random") == 0);
  }

  return 0;
}
#endif /* MLD_USE_NATIVE_POLYZ_UNPACK_17 || MLD_USE_NATIVE_POLYZ_UNPACK_19 */


static int test_backend_units(void)
{
  /* Set fixed seed for reproducible tests */
  randombytes_reset();


#ifdef MLD_USE_NATIVE_NTT
  CHECK(test_native_ntt() == 0);
#endif

#ifdef MLD_USE_NATIVE_INTT
  CHECK(test_native_invntt_tomont() == 0);
#endif

#if defined(MLD_USE_NATIVE_POLY_DECOMPOSE_32) || \
    defined(MLD_USE_NATIVE_POLY_DECOMPOSE_88)
  CHECK(test_native_decompose() == 0);
#endif

#ifdef MLD_USE_NATIVE_POLY_CADDQ
  CHECK(test_native_caddq() == 0);
#endif

#if defined(MLD_USE_NATIVE_POLY_USE_HINT_88) || \
    defined(MLD_USE_NATIVE_POLY_USE_HINT_32)
  CHECK(test_native_use_hint() == 0);
#endif

#if defined(MLD_USE_NATIVE_POLY_CHKNORM)
  CHECK(test_native_poly_chknorm() == 0);
#endif

#if defined(MLD_USE_NATIVE_POINTWISE_MONTGOMERY)
  CHECK(test_native_pointwise_montgomery() == 0);
#endif

#if defined(MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L4) || \
    defined(MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L5) || \
    defined(MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L7)
  CHECK(test_native_polyvecl_pointwise_acc_montgomery() == 0);
#endif

#if defined(MLD_USE_NATIVE_POLYZ_UNPACK_17) || \
    defined(MLD_USE_NATIVE_POLYZ_UNPACK_19)
  CHECK(test_native_polyz_unpack() == 0);
#endif

  return 0;
}
#endif /* MLD_USE_NATIVE_NTT || MLD_USE_NATIVE_INTT ||                         \
          MLD_USE_NATIVE_POLY_DECOMPOSE_32 || MLD_USE_NATIVE_POLY_DECOMPOSE_88 \
          || MLD_USE_NATIVE_POLY_CADDQ || MLD_USE_NATIVE_POLY_USE_HINT_88 ||   \
          MLD_USE_NATIVE_POLY_USE_HINT_32 || MLD_USE_NATIVE_POLY_CHKNORM ||    \
          MLD_USE_NATIVE_POINTWISE_MONTGOMERY ||                               \
          MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L4 ||               \
          MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L5 ||               \
          MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L7 ||               \
          MLD_USE_NATIVE_POLYZ_UNPACK_17 || MLD_USE_NATIVE_POLYZ_UNPACK_19 */

int main(void)
{
  /* WARNING: Test-only
   * Normally, you would want to seed a PRNG with trustworthy entropy here. */
  randombytes_reset();

  /* Run backend unit tests */
#if defined(MLD_USE_NATIVE_NTT) || defined(MLD_USE_NATIVE_INTT) ||  \
    defined(MLD_USE_NATIVE_POLY_DECOMPOSE_32) ||                    \
    defined(MLD_USE_NATIVE_POLY_DECOMPOSE_88) ||                    \
    defined(MLD_USE_NATIVE_POLY_CADDQ) ||                           \
    defined(MLD_USE_NATIVE_POLY_USE_HINT_88) ||                     \
    defined(MLD_USE_NATIVE_POLY_USE_HINT_32) ||                     \
    defined(MLD_USE_NATIVE_POLY_CHKNORM) ||                         \
    defined(MLD_USE_NATIVE_POINTWISE_MONTGOMERY) ||                 \
    defined(MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L4) || \
    defined(MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L5) || \
    defined(MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L7) || \
    defined(MLD_USE_NATIVE_POLYZ_UNPACK_17) ||                      \
    defined(MLD_USE_NATIVE_POLYZ_UNPACK_19)
  CHECK(test_backend_units() == 0);
#endif /* MLD_USE_NATIVE_NTT || MLD_USE_NATIVE_INTT ||                         \
          MLD_USE_NATIVE_POLY_DECOMPOSE_32 || MLD_USE_NATIVE_POLY_DECOMPOSE_88 \
          || MLD_USE_NATIVE_POLY_CADDQ || MLD_USE_NATIVE_POLY_USE_HINT_88 ||   \
          MLD_USE_NATIVE_POLY_USE_HINT_32 || MLD_USE_NATIVE_POLY_CHKNORM ||    \
          MLD_USE_NATIVE_POINTWISE_MONTGOMERY ||                               \
          MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L4 ||               \
          MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L5 ||               \
          MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L7 ||               \
          MLD_USE_NATIVE_POLYZ_UNPACK_17 || MLD_USE_NATIVE_POLYZ_UNPACK_19 */


  return 0;
}
