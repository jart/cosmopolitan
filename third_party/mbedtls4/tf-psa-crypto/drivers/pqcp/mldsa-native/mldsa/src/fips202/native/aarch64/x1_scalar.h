/*
 * Copyright (c) The mlkem-native project authors
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */

#ifndef MLD_FIPS202_NATIVE_AARCH64_X1_SCALAR_H
#define MLD_FIPS202_NATIVE_AARCH64_X1_SCALAR_H

/* Part of backend API */
#define MLD_USE_FIPS202_X1_NATIVE
/* Guard for assembly file */
#define MLD_FIPS202_AARCH64_NEED_X1_SCALAR

#if !defined(__ASSEMBLER__)
#include "../api.h"
#include "src/fips202_native_aarch64.h"
static MLD_INLINE int mld_keccak_f1600_x1_native(uint64_t *state)
{
  mld_keccak_f1600_x1_scalar_asm(state, mld_keccakf1600_round_constants);
  return MLD_NATIVE_FUNC_SUCCESS;
}
#endif /* !__ASSEMBLER__ */

#endif /* !MLD_FIPS202_NATIVE_AARCH64_X1_SCALAR_H */
