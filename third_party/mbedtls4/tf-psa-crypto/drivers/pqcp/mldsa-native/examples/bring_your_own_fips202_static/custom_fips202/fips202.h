/*
 * Copyright (c) The mlkem-native project authors
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */

/*
 * This is a shim establishing the FIPS-202 API required by mldsa-native
 * from the API exposed by tiny_sha3.
 */

#ifndef FIPS202_H
#define FIPS202_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "../custom_fips202/tiny_sha3/sha3.h"

/* We need the MLD_INLINE definition from sys.h */
#ifndef MLD_INLINE
#if defined(__GNUC__) || defined(__clang__)
#define MLD_INLINE __attribute__((always_inline)) static inline
#else
#define MLD_INLINE static inline
#endif
#endif /* !MLD_INLINE */

#define SHAKE128_RATE 168
#define SHAKE256_RATE 136
#define SHA3_256_RATE 136
#define SHA3_512_RATE 72
#define SHA3_256_HASHBYTES 32
#define SHA3_512_HASHBYTES 64

typedef enum
{
  FIPS202_STATE_ABSORBING = 1,
  FIPS202_STATE_SQUEEZING = 2,
  FIPS202_STATE_FINALIZED = 3,
  FIPS202_STATE_RESET = 4
} fips202_state_t;

/* Static state for serial FIPS202 - SHAKE128 */
static struct
{
  fips202_state_t state;
  sha3_ctx_t ctx;
} static_shake128_state = {FIPS202_STATE_RESET, {{{0}}, 0, 0, 0}};

/* Static state for serial FIPS202 - SHAKE256 */
static struct
{
  fips202_state_t state;
  sha3_ctx_t ctx;
} static_shake256_state = {FIPS202_STATE_RESET, {{{0}}, 0, 0, 0}};

/* Dummy context types - the actual state is static */
typedef struct
{
  int dummy;
} mld_shake128ctx;

typedef struct
{
  int dummy;
} mld_shake256ctx;

/*************************************************
 * Name:        mld_shake128_init
 *
 * Description: Initializes state for use as SHAKE128 XOF
 *
 * Arguments:   - mld_shake128ctx *state: pointer to (uninitialized) state
 **************************************************/
#define mld_shake128_init MLD_NAMESPACE(shake128_init)
static MLD_INLINE void mld_shake128_init(mld_shake128ctx *state)
{
  (void)state;
  assert(static_shake128_state.state == FIPS202_STATE_RESET);
  shake128_init(&static_shake128_state.ctx);
  static_shake128_state.state = FIPS202_STATE_ABSORBING;
}

/*************************************************
 * Name:        mld_shake128_absorb
 *
 * Description: Absorb step of the SHAKE128 XOF. Absorbs arbitrarily many bytes.
 *              Can be called multiple times to absorb multiple chunks of data.
 *
 * Arguments:   - mld_shake128ctx *state: pointer to (initialized) output state
 *              - const uint8_t *in: pointer to input to be absorbed into s
 *              - size_t inlen: length of input in bytes
 **************************************************/
#define mld_shake128_absorb MLD_NAMESPACE(shake128_absorb)
static MLD_INLINE void mld_shake128_absorb(mld_shake128ctx *state,
                                           const uint8_t *in, size_t inlen)
{
  (void)state;
  assert(static_shake128_state.state == FIPS202_STATE_ABSORBING);
  shake_update(&static_shake128_state.ctx, in, inlen);
}

/*************************************************
 * Name:        mld_shake128_finalize
 *
 * Description: Concludes the absorb phase of the SHAKE128 XOF.
 *
 * Arguments:   - mld_shake128ctx *state: pointer to state
 **************************************************/
#define mld_shake128_finalize MLD_NAMESPACE(shake128_finalize)
static MLD_INLINE void mld_shake128_finalize(mld_shake128ctx *state)
{
  (void)state;
  assert(static_shake128_state.state == FIPS202_STATE_ABSORBING);
  shake_xof(&static_shake128_state.ctx);
  static_shake128_state.state = FIPS202_STATE_SQUEEZING;
}

/*************************************************
 * Name:        mld_shake128_squeeze
 *
 * Description: Squeeze step of SHAKE128 XOF. Squeezes arbitrarily many
 *              bytes. Can be called multiple times to keep squeezing.
 *
 * Arguments:   - uint8_t *out: pointer to output blocks
 *              - size_t outlen : number of bytes to be squeezed (written to
 *output)
 *              - mld_shake128ctx *s: pointer to input/output state
 **************************************************/
#define mld_shake128_squeeze MLD_NAMESPACE(shake128_squeeze)
static MLD_INLINE void mld_shake128_squeeze(uint8_t *out, size_t outlen,
                                            mld_shake128ctx *state)
{
  (void)state;
  assert(static_shake128_state.state == FIPS202_STATE_SQUEEZING);
  shake_out(&static_shake128_state.ctx, out, outlen);
}

/*************************************************
 * Name:        mld_shake128_release
 *
 * Description: Release and securely zero the SHAKE128 state.
 *
 * Arguments:   - mld_shake128ctx *state: pointer to state
 **************************************************/
#define mld_shake128_release MLD_NAMESPACE(shake128_release)
static MLD_INLINE void mld_shake128_release(mld_shake128ctx *state)
{
  (void)state;
  static_shake128_state.state = FIPS202_STATE_RESET;
}

/*************************************************
 * Name:        mld_shake256_init
 *
 * Description: Initializes state for use as SHAKE256 XOF
 *
 * Arguments:   - mld_shake256ctx *state: pointer to (uninitialized) state
 **************************************************/
#define mld_shake256_init MLD_NAMESPACE(shake256_init)
static MLD_INLINE void mld_shake256_init(mld_shake256ctx *state)
{
  (void)state;
  assert(static_shake256_state.state == FIPS202_STATE_RESET);
  shake256_init(&static_shake256_state.ctx);
  static_shake256_state.state = FIPS202_STATE_ABSORBING;
}

/*************************************************
 * Name:        mld_shake256_absorb
 *
 * Description: Absorb step of the SHAKE256 XOF. Absorbs arbitrarily many bytes.
 *              Can be called multiple times to absorb multiple chunks of data.
 *
 * Arguments:   - mld_shake256ctx *state: pointer to (initialized) output state
 *              - const uint8_t *in: pointer to input to be absorbed into s
 *              - size_t inlen: length of input in bytes
 **************************************************/
#define mld_shake256_absorb MLD_NAMESPACE(shake256_absorb)
static MLD_INLINE void mld_shake256_absorb(mld_shake256ctx *state,
                                           const uint8_t *in, size_t inlen)
{
  (void)state;
  assert(static_shake256_state.state == FIPS202_STATE_ABSORBING);
  shake_update(&static_shake256_state.ctx, in, inlen);
}

/*************************************************
 * Name:        mld_shake256_finalize
 *
 * Description: Concludes the absorb phase of the SHAKE256 XOF.
 *
 * Arguments:   - mld_shake256ctx *state: pointer to state
 **************************************************/
#define mld_shake256_finalize MLD_NAMESPACE(shake256_finalize)
static MLD_INLINE void mld_shake256_finalize(mld_shake256ctx *state)
{
  (void)state;
  assert(static_shake256_state.state == FIPS202_STATE_ABSORBING);
  shake_xof(&static_shake256_state.ctx);
  static_shake256_state.state = FIPS202_STATE_SQUEEZING;
}

/*************************************************
 * Name:        mld_shake256_squeeze
 *
 * Description: Squeeze step of SHAKE256 XOF. Squeezes arbitrarily many
 *              bytes. Can be called multiple times to keep squeezing.
 *
 * Arguments:   - uint8_t *out: pointer to output blocks
 *              - size_t outlen : number of bytes to be squeezed (written to
 *output)
 *              - mld_shake256ctx *s: pointer to input/output state
 **************************************************/
#define mld_shake256_squeeze MLD_NAMESPACE(shake256_squeeze)
static MLD_INLINE void mld_shake256_squeeze(uint8_t *out, size_t outlen,
                                            mld_shake256ctx *state)
{
  (void)state;
  assert(static_shake256_state.state == FIPS202_STATE_SQUEEZING);
  shake_out(&static_shake256_state.ctx, out, outlen);
}

/*************************************************
 * Name:        mld_shake256_release
 *
 * Description: Release and securely zero the SHAKE256 state.
 *
 * Arguments:   - mld_shake256ctx *state: pointer to state
 **************************************************/
#define mld_shake256_release MLD_NAMESPACE(shake256_release)
static MLD_INLINE void mld_shake256_release(mld_shake256ctx *state)
{
  (void)state;
  static_shake256_state.state = FIPS202_STATE_RESET;
}

/*************************************************
 * Name:        mld_shake256
 *
 * Description: SHAKE256 XOF with non-incremental API
 *
 * Arguments:   - uint8_t *out: pointer to output
 *              - size_t outlen: requested output length in bytes
 *              - const uint8_t *in: pointer to input
 *              - size_t inlen: length of input in bytes
 **************************************************/
#define mld_shake256 MLD_NAMESPACE(shake256)
static MLD_INLINE void mld_shake256(uint8_t *out, size_t outlen,
                                    const uint8_t *in, size_t inlen)
{
  sha3_ctx_t c;
  shake256_init(&c);
  shake_update(&c, in, inlen);
  shake_xof(&c);
  shake_out(&c, out, outlen);
}

#endif /* !FIPS202_H */
