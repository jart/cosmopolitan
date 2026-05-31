/*
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */

#if !defined(MLD_FIPS202_CUSTOM_TINY_SHA3_H)
#define MLD_FIPS202_CUSTOM_TINY_SHA3_H

#if !defined(__ASSEMBLER__)
#include "../api.h"
#include "src/sha3.h"
/* Replace (single) Keccak-F1600 by tiny-SHA3's */
#define MLD_USE_FIPS202_X1_NATIVE
static MLD_INLINE int mld_keccak_f1600_x1_native(uint64_t *state)
{
  tiny_sha3_keccakf(state);

  return MLD_NATIVE_FUNC_SUCCESS;
}
#endif /* !__ASSEMBLER__ */

#endif /* !MLD_FIPS202_CUSTOM_TINY_SHA3_H */
