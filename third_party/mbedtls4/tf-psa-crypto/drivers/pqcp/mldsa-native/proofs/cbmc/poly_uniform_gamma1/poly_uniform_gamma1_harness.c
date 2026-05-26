// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "poly_kl.h"

void harness(void)
{
/* mld_poly_uniform_gamma1 is only defined for ML-DSA-65 */
#if MLD_CONFIG_PARAMETER_SET == 65
  mld_poly *a;
  const uint8_t *seed;
  uint16_t nonce;

  mld_poly_uniform_gamma1(a, seed, nonce);
#endif /* MLD_CONFIG_PARAMETER_SET == 65 */
}
