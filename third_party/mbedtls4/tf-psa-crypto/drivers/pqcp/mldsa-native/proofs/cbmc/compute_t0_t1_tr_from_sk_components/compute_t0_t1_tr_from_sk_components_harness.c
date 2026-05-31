// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "sign.h"

int mld_compute_t0_t1_tr_from_sk_components(
    mld_polyveck *t0, mld_polyveck *t1, uint8_t tr[MLDSA_TRBYTES],
    uint8_t pk[MLDSA_CRYPTO_PUBLICKEYBYTES], const uint8_t rho[MLDSA_SEEDBYTES],
    const mld_polyvecl *s1, const mld_polyveck *s2);

void harness(void)
{
  mld_polyveck *t0;
  mld_polyveck *t1;
  uint8_t *tr;
  uint8_t *pk;
  uint8_t *rho;
  mld_polyvecl *s1;
  mld_polyveck *s2;

  mld_compute_t0_t1_tr_from_sk_components(t0, t1, tr, pk, rho, s1, s2);
}
