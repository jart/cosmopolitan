/*
 * Copyright (c) The mlkem-native project authors
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */
#ifndef MLD_FIPS202_FIPS202X4_H
#define MLD_FIPS202_FIPS202X4_H

#include <stddef.h>
#include <stdint.h>

#include "../cbmc.h"
#include "../common.h"

#include "fips202.h"
#include "keccakf1600.h"

/* Context for non-incremental API */
typedef struct
{
  uint64_t ctx[MLD_KECCAK_LANES * MLD_KECCAK_WAY];
} mld_shake128x4ctx;

typedef struct
{
  uint64_t ctx[MLD_KECCAK_LANES * MLD_KECCAK_WAY];
} mld_shake256x4ctx;

#define mld_shake128x4_absorb_once MLD_NAMESPACE(shake128x4_absorb_once)
void mld_shake128x4_absorb_once(mld_shake128x4ctx *state, const uint8_t *in0,
                                const uint8_t *in1, const uint8_t *in2,
                                const uint8_t *in3, size_t inlen)
__contract__(
  requires(inlen <= MLD_MAX_BUFFER_SIZE)
  requires(memory_no_alias(state, sizeof(mld_shake128x4ctx)))
  requires(memory_no_alias(in0, inlen))
  requires(memory_no_alias(in1, inlen))
  requires(memory_no_alias(in2, inlen))
  requires(memory_no_alias(in3, inlen))
  assigns(memory_slice(state, sizeof(mld_shake128x4ctx)))
);

#define mld_shake128x4_squeezeblocks MLD_NAMESPACE(shake128x4_squeezeblocks)
void mld_shake128x4_squeezeblocks(uint8_t *out0, uint8_t *out1, uint8_t *out2,
                                  uint8_t *out3, size_t nblocks,
                                  mld_shake128x4ctx *state)
__contract__(
  requires(nblocks <= 8 /* somewhat arbitrary bound */)
  requires(memory_no_alias(state, sizeof(mld_shake128x4ctx)))
  requires(memory_no_alias(out0, nblocks * SHAKE128_RATE))
  requires(memory_no_alias(out1, nblocks * SHAKE128_RATE))
  requires(memory_no_alias(out2, nblocks * SHAKE128_RATE))
  requires(memory_no_alias(out3, nblocks * SHAKE128_RATE))
  assigns(memory_slice(out0, nblocks * SHAKE128_RATE),
    memory_slice(out1, nblocks * SHAKE128_RATE),
    memory_slice(out2, nblocks * SHAKE128_RATE),
    memory_slice(out3, nblocks * SHAKE128_RATE),
    memory_slice(state, sizeof(mld_shake128x4ctx)))
);

#define mld_shake128x4_init MLD_NAMESPACE(shake128x4_init)
void mld_shake128x4_init(mld_shake128x4ctx *state);

#define mld_shake128x4_release MLD_NAMESPACE(shake128x4_release)
void mld_shake128x4_release(mld_shake128x4ctx *state);


#define mld_shake256x4_absorb_once MLD_NAMESPACE(shake256x4_absorb_once)
void mld_shake256x4_absorb_once(mld_shake256x4ctx *state, const uint8_t *in0,
                                const uint8_t *in1, const uint8_t *in2,
                                const uint8_t *in3, size_t inlen)
__contract__(
  requires(inlen <= MLD_MAX_BUFFER_SIZE)
  requires(memory_no_alias(state, sizeof(mld_shake256x4ctx)))
  requires(memory_no_alias(in0, inlen))
  requires(memory_no_alias(in1, inlen))
  requires(memory_no_alias(in2, inlen))
  requires(memory_no_alias(in3, inlen))
  assigns(memory_slice(state, sizeof(mld_shake256x4ctx)))
);

#define mld_shake256x4_squeezeblocks MLD_NAMESPACE(shake256x4_squeezeblocks)
void mld_shake256x4_squeezeblocks(uint8_t *out0, uint8_t *out1, uint8_t *out2,
                                  uint8_t *out3, size_t nblocks,
                                  mld_shake256x4ctx *state)
__contract__(
  requires(nblocks <= 8 /* somewhat arbitrary bound */)
  requires(memory_no_alias(state, sizeof(mld_shake256x4ctx)))
  requires(memory_no_alias(out0, nblocks * SHAKE256_RATE))
  requires(memory_no_alias(out1, nblocks * SHAKE256_RATE))
  requires(memory_no_alias(out2, nblocks * SHAKE256_RATE))
  requires(memory_no_alias(out3, nblocks * SHAKE256_RATE))
  assigns(memory_slice(out0, nblocks * SHAKE256_RATE),
    memory_slice(out1, nblocks * SHAKE256_RATE),
    memory_slice(out2, nblocks * SHAKE256_RATE),
    memory_slice(out3, nblocks * SHAKE256_RATE),
    memory_slice(state, sizeof(mld_shake256x4ctx)))
);

#define mld_shake256x4_init MLD_NAMESPACE(shake256x4_init)
void mld_shake256x4_init(mld_shake256x4ctx *state);

#define mld_shake256x4_release MLD_NAMESPACE(shake256x4_release)
void mld_shake256x4_release(mld_shake256x4ctx *state);


#endif /* !MLD_FIPS202_FIPS202X4_H */
