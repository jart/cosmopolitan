/*
 * Copyright (c) The mlkem-native project authors
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */
#ifndef MLD_FIPS202_NATIVE_AARCH64_SRC_FIPS202_NATIVE_AARCH64_H
#define MLD_FIPS202_NATIVE_AARCH64_SRC_FIPS202_NATIVE_AARCH64_H

#include <stdint.h>
#include "../../../../cbmc.h"
#include "../../../../common.h"


#define mld_keccakf1600_round_constants \
  MLD_NAMESPACE(keccakf1600_round_constants)
extern const uint64_t mld_keccakf1600_round_constants[];

#define mld_keccak_f1600_x1_scalar_asm MLD_NAMESPACE(keccak_f1600_x1_scalar_asm)
void mld_keccak_f1600_x1_scalar_asm(uint64_t *state, uint64_t const *rc)
__contract__(
  requires(memory_no_alias(state, sizeof(uint64_t) * 25 * 1))
  requires(rc == mld_keccakf1600_round_constants)
  assigns(memory_slice(state, sizeof(uint64_t) * 25 * 1))
);

#define mld_keccak_f1600_x1_v84a_asm MLD_NAMESPACE(keccak_f1600_x1_v84a_asm)
void mld_keccak_f1600_x1_v84a_asm(uint64_t *state, uint64_t const *rc)
__contract__(
  requires(memory_no_alias(state, sizeof(uint64_t) * 25 * 1))
  requires(rc == mld_keccakf1600_round_constants)
  assigns(memory_slice(state, sizeof(uint64_t) * 25 * 1))
);

#define mld_keccak_f1600_x2_v84a_asm MLD_NAMESPACE(keccak_f1600_x2_v84a_asm)
void mld_keccak_f1600_x2_v84a_asm(uint64_t *state, uint64_t const *rc)
__contract__(
  requires(memory_no_alias(state, sizeof(uint64_t) * 25 * 2))
  requires(rc == mld_keccakf1600_round_constants)
  assigns(memory_slice(state, sizeof(uint64_t) * 25 * 2))
);

#define mld_keccak_f1600_x4_scalar_v8a_hybrid_asm \
  MLD_NAMESPACE(keccak_f1600_x4_scalar_v8a_hybrid_asm)
void mld_keccak_f1600_x4_scalar_v8a_hybrid_asm(uint64_t *state,
                                               uint64_t const *rc)
__contract__(
  requires(memory_no_alias(state, sizeof(uint64_t) * 25 * 4))
  requires(rc == mld_keccakf1600_round_constants)
  assigns(memory_slice(state, sizeof(uint64_t) * 25 * 4))
);

#define mld_keccak_f1600_x4_scalar_v8a_v84a_hybrid_asm \
  MLD_NAMESPACE(keccak_f1600_x4_scalar_v8a_v84a_hybrid_asm)
void mld_keccak_f1600_x4_scalar_v8a_v84a_hybrid_asm(uint64_t *state,
                                                    uint64_t const *rc)
__contract__(
  requires(memory_no_alias(state, sizeof(uint64_t) * 25 * 4))
  requires(rc == mld_keccakf1600_round_constants)
  assigns(memory_slice(state, sizeof(uint64_t) * 25 * 4))
);

#endif /* !MLD_FIPS202_NATIVE_AARCH64_SRC_FIPS202_NATIVE_AARCH64_H */
