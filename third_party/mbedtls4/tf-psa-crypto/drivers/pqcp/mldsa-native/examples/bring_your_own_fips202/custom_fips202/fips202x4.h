/*
 * Copyright (c) The mlkem-native project authors
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */

/*
 * This is a shim establishing the FIPS-202x4 API required by mldsa-native
 * using a serial implementation based on tiny_sha3.
 *
 * NOTE: This is a simple serial implementation that does not provide
 * any performance benefits from parallelization. For production use,
 * consider using an optimized parallel implementation.
 */

#ifndef FIPS_202X4_H
#define FIPS_202X4_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "fips202.h"

/* For MLD_KECCAK_WAY - typically 4 */
#ifndef MLD_KECCAK_WAY
#define MLD_KECCAK_WAY 4
#endif

typedef struct
{
  fips202_state_t state;
  mld_shake128ctx ctx[4];
} mld_shake128x4ctx;

typedef struct
{
  fips202_state_t state;
  mld_shake256ctx ctx[4];
} mld_shake256x4ctx;

/*************************************************
 * Name:        mld_shake128x4_init
 *
 * Description: Initialize 4 parallel SHAKE128 contexts
 *
 * Arguments:   - mld_shake128x4ctx *state: pointer to state
 **************************************************/
#define mld_shake128x4_init MLD_NAMESPACE(shake128x4_init)
static MLD_INLINE void mld_shake128x4_init(mld_shake128x4ctx *state)
{
  mld_shake128_init(&state->ctx[0]);
  mld_shake128_init(&state->ctx[1]);
  mld_shake128_init(&state->ctx[2]);
  mld_shake128_init(&state->ctx[3]);
  state->state = FIPS202_STATE_ABSORBING;
}

/*************************************************
 * Name:        mld_shake128x4_absorb_once
 *
 * Description: Absorb step for 4 parallel SHAKE128 contexts
 *
 * Arguments:   - mld_shake128x4ctx *state: pointer to state
 *              - const uint8_t *in0-3: pointers to input data
 *              - size_t inlen: length of input in bytes
 **************************************************/
#define mld_shake128x4_absorb_once MLD_NAMESPACE(shake128x4_absorb_once)
static MLD_INLINE void mld_shake128x4_absorb_once(
    mld_shake128x4ctx *state, const uint8_t *in0, const uint8_t *in1,
    const uint8_t *in2, const uint8_t *in3, size_t inlen)
{
  assert(state->state == FIPS202_STATE_ABSORBING);

  mld_shake128_absorb(&state->ctx[0], in0, inlen);
  mld_shake128_finalize(&state->ctx[0]);

  mld_shake128_absorb(&state->ctx[1], in1, inlen);
  mld_shake128_finalize(&state->ctx[1]);

  mld_shake128_absorb(&state->ctx[2], in2, inlen);
  mld_shake128_finalize(&state->ctx[2]);

  mld_shake128_absorb(&state->ctx[3], in3, inlen);
  mld_shake128_finalize(&state->ctx[3]);

  state->state = FIPS202_STATE_SQUEEZING;
}

/*************************************************
 * Name:        mld_shake128x4_squeezeblocks
 *
 * Description: Squeeze blocks from 4 parallel SHAKE128 contexts
 *
 * Arguments:   - uint8_t *out0-3: pointers to output buffers
 *              - size_t nblocks: number of blocks to squeeze
 *              - mld_shake128x4ctx *state: pointer to state
 **************************************************/
#define mld_shake128x4_squeezeblocks MLD_NAMESPACE(shake128x4_squeezeblocks)
static MLD_INLINE void mld_shake128x4_squeezeblocks(
    uint8_t *out0, uint8_t *out1, uint8_t *out2, uint8_t *out3, size_t nblocks,
    mld_shake128x4ctx *state)
{
  assert(state->state == FIPS202_STATE_SQUEEZING);

  mld_shake128_squeeze(out0, nblocks * SHAKE128_RATE, &state->ctx[0]);
  mld_shake128_squeeze(out1, nblocks * SHAKE128_RATE, &state->ctx[1]);
  mld_shake128_squeeze(out2, nblocks * SHAKE128_RATE, &state->ctx[2]);
  mld_shake128_squeeze(out3, nblocks * SHAKE128_RATE, &state->ctx[3]);
}

/*************************************************
 * Name:        mld_shake128x4_release
 *
 * Description: Release 4 parallel SHAKE128 contexts
 *
 * Arguments:   - mld_shake128x4ctx *state: pointer to state
 **************************************************/
#define mld_shake128x4_release MLD_NAMESPACE(shake128x4_release)
static MLD_INLINE void mld_shake128x4_release(mld_shake128x4ctx *state)
{
  mld_shake128_release(&state->ctx[0]);
  mld_shake128_release(&state->ctx[1]);
  mld_shake128_release(&state->ctx[2]);
  mld_shake128_release(&state->ctx[3]);
  state->state = FIPS202_STATE_RESET;
}

/*************************************************
 * Name:        mld_shake256x4_init
 *
 * Description: Initialize 4 parallel SHAKE256 contexts
 *
 * Arguments:   - mld_shake256x4ctx *state: pointer to state
 **************************************************/
#define mld_shake256x4_init MLD_NAMESPACE(shake256x4_init)
static MLD_INLINE void mld_shake256x4_init(mld_shake256x4ctx *state)
{
  mld_shake256_init(&state->ctx[0]);
  mld_shake256_init(&state->ctx[1]);
  mld_shake256_init(&state->ctx[2]);
  mld_shake256_init(&state->ctx[3]);
  state->state = FIPS202_STATE_ABSORBING;
}

/*************************************************
 * Name:        mld_shake256x4_absorb_once
 *
 * Description: Absorb step for 4 parallel SHAKE256 contexts
 *
 * Arguments:   - mld_shake256x4ctx *state: pointer to state
 *              - const uint8_t *in0-3: pointers to input data
 *              - size_t inlen: length of input in bytes
 **************************************************/
#define mld_shake256x4_absorb_once MLD_NAMESPACE(shake256x4_absorb_once)
static MLD_INLINE void mld_shake256x4_absorb_once(
    mld_shake256x4ctx *state, const uint8_t *in0, const uint8_t *in1,
    const uint8_t *in2, const uint8_t *in3, size_t inlen)
{
  assert(state->state == FIPS202_STATE_ABSORBING);

  mld_shake256_absorb(&state->ctx[0], in0, inlen);
  mld_shake256_finalize(&state->ctx[0]);

  mld_shake256_absorb(&state->ctx[1], in1, inlen);
  mld_shake256_finalize(&state->ctx[1]);

  mld_shake256_absorb(&state->ctx[2], in2, inlen);
  mld_shake256_finalize(&state->ctx[2]);

  mld_shake256_absorb(&state->ctx[3], in3, inlen);
  mld_shake256_finalize(&state->ctx[3]);

  state->state = FIPS202_STATE_SQUEEZING;
}

/*************************************************
 * Name:        mld_shake256x4_squeezeblocks
 *
 * Description: Squeeze blocks from 4 parallel SHAKE256 contexts
 *
 * Arguments:   - uint8_t *out0-3: pointers to output buffers
 *              - size_t nblocks: number of blocks to squeeze
 *              - mld_shake256x4ctx *state: pointer to state
 **************************************************/
#define mld_shake256x4_squeezeblocks MLD_NAMESPACE(shake256x4_squeezeblocks)
static MLD_INLINE void mld_shake256x4_squeezeblocks(
    uint8_t *out0, uint8_t *out1, uint8_t *out2, uint8_t *out3, size_t nblocks,
    mld_shake256x4ctx *state)
{
  assert(state->state == FIPS202_STATE_SQUEEZING);

  mld_shake256_squeeze(out0, nblocks * SHAKE256_RATE, &state->ctx[0]);
  mld_shake256_squeeze(out1, nblocks * SHAKE256_RATE, &state->ctx[1]);
  mld_shake256_squeeze(out2, nblocks * SHAKE256_RATE, &state->ctx[2]);
  mld_shake256_squeeze(out3, nblocks * SHAKE256_RATE, &state->ctx[3]);
}

/*************************************************
 * Name:        mld_shake256x4_release
 *
 * Description: Release 4 parallel SHAKE256 contexts
 *
 * Arguments:   - mld_shake256x4ctx *state: pointer to state
 **************************************************/
#define mld_shake256x4_release MLD_NAMESPACE(shake256x4_release)
static MLD_INLINE void mld_shake256x4_release(mld_shake256x4ctx *state)
{
  mld_shake256_release(&state->ctx[0]);
  mld_shake256_release(&state->ctx[1]);
  mld_shake256_release(&state->ctx[2]);
  mld_shake256_release(&state->ctx[3]);
  state->state = FIPS202_STATE_RESET;
}

#endif /* !FIPS_202X4_H */
