/*
 * Copyright (c) The mldsa-native project authors
 * Copyright (c) The mlkem-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */
#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../mldsa/src/poly.h"
#include "../../mldsa/src/polyvec.h"
#include "../../mldsa/src/randombytes.h"
#include "hal.h"

#define NWARMUP 50
#define NITERATIONS 300
#define NTESTS 20

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

static int cmp_uint64_t(const void *a, const void *b)
{
  return (int)((*((const uint64_t *)a)) - (*((const uint64_t *)b)));
}

#define BENCH(txt, code)                                                     \
  for (i = 0; i < NTESTS; i++)                                               \
  {                                                                          \
    CHECK(mld_randombytes((uint8_t *)data0, sizeof(data0)) == 0);            \
    CHECK(mld_randombytes((uint8_t *)&polyvecl_a, sizeof(polyvecl_a)) == 0); \
    CHECK(mld_randombytes((uint8_t *)&polyvecl_b, sizeof(polyvecl_b)) == 0); \
    CHECK(mld_randombytes((uint8_t *)&polymat, sizeof(polymat)) == 0);       \
    for (j = 0; j < NWARMUP; j++)                                            \
    {                                                                        \
      code;                                                                  \
    }                                                                        \
                                                                             \
    t0 = get_cyclecounter();                                                 \
    for (j = 0; j < NITERATIONS; j++)                                        \
    {                                                                        \
      code;                                                                  \
    }                                                                        \
    t1 = get_cyclecounter();                                                 \
    (cyc)[i] = t1 - t0;                                                      \
  }                                                                          \
  qsort((cyc), NTESTS, sizeof(uint64_t), cmp_uint64_t);                      \
  printf(txt " cycles=%" PRIu64 "\n", (cyc)[NTESTS >> 1] / NITERATIONS);

static int bench(void)
{
  MLD_ALIGN int32_t data0[256];
  MLD_ALIGN mld_poly poly_out;
  MLD_ALIGN mld_polyvecl polyvecl_a, polyvecl_b;
  MLD_ALIGN mld_polyveck polyveck_out;
  MLD_ALIGN mld_polymat polymat;
  uint64_t cyc[NTESTS];
  unsigned i, j;
  uint64_t t0, t1;

  /* ntt */
  BENCH("poly_ntt", mld_poly_ntt((mld_poly *)data0))
  BENCH("poly_invntt_tomont", mld_poly_invntt_tomont((mld_poly *)data0))

  /* pointwise */
  BENCH("polyvecl_pointwise_acc_montgomery",
        mld_polyvecl_pointwise_acc_montgomery(&poly_out, &polyvecl_a,
                                              &polyvecl_b))
  BENCH("polyvec_matrix_pointwise_montgomery",
        mld_polyvec_matrix_pointwise_montgomery(&polyveck_out, &polymat,
                                                &polyvecl_b))

  return 0;
}

int main(void)
{
  enable_cyclecounter();
  bench();
  disable_cyclecounter();

  return 0;
}
