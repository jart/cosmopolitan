/*
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */

/* References
 * ==========
 *
 * - [FIPS204]
 *   FIPS 204 Module-Lattice-Based Digital Signature Standard
 *   National Institute of Standards and Technology
 *   https://csrc.nist.gov/pubs/fips/204/final
 *
 * - [mupq]
 *   Common files for pqm4, pqm3, pqriscv
 *   Kannwischer, Petri, Rijneveld, Schwabe, Stoffelen
 *   https://github.com/mupq/mupq
 *
 * - [supercop]
 *   SUPERCOP benchmarking framework
 *   Daniel J. Bernstein
 *   http://bench.cr.yp.to/supercop.html
 *
 * - [tweetfips]
 *   'tweetfips202' FIPS202 implementation
 *   Van Assche, Bernstein, Schwabe
 *   https://keccak.team/2015/tweetfips202.html
 */

/* Based on the CC0 implementation from @[mupq] and the public domain
 * implementation @[supercop, crypto_hash/keccakc512/simple/]
 * by Ronny Van Keer, and the public domain @[tweetfips] implementation. */

#include <stddef.h>
#include <stdint.h>

#include "../common.h"
#include "../ct.h"
#include "fips202.h"
#include "keccakf1600.h"
#if !defined(MLD_CONFIG_MULTILEVEL_NO_SHARED)

/*************************************************
 * Name:        keccak_init
 *
 * Description: Initializes the Keccak state.
 *
 * Arguments:   - uint64_t *s: pointer to Keccak state
 **************************************************/
static void keccak_init(uint64_t s[MLD_KECCAK_LANES])
__contract__(
  requires(memory_no_alias(s, sizeof(uint64_t) * MLD_KECCAK_LANES))
  assigns(memory_slice(s, sizeof(uint64_t) * MLD_KECCAK_LANES))
)
{
  mld_memset(s, 0, sizeof(uint64_t) * MLD_KECCAK_LANES);
}

/*************************************************
 * Name:        keccak_absorb
 *
 * Description: Absorb step of Keccak; incremental.
 *
 * Arguments:   - uint64_t *s: pointer to Keccak state
 *              - unsigned int pos: position in current block to be absorbed
 *              - unsigned int r: rate in bytes (e.g., 168 for SHAKE128)
 *              - const uint8_t *in: pointer to input to be absorbed into s
 *              - size_t inlen: length of input in bytes
 *
 * Returns new position pos in current block
 **************************************************/
static unsigned int keccak_absorb(uint64_t s[MLD_KECCAK_LANES],
                                  unsigned int pos, unsigned int r,
                                  const uint8_t *in, size_t inlen)
__contract__(
  requires(inlen <= MLD_MAX_BUFFER_SIZE)
  requires(r < sizeof(uint64_t) * MLD_KECCAK_LANES)
  requires(pos <= r)
  requires(memory_no_alias(s, sizeof(uint64_t) * MLD_KECCAK_LANES))
  requires(memory_no_alias(in, inlen))
  assigns(memory_slice(s, sizeof(uint64_t) * MLD_KECCAK_LANES))
  ensures(return_value < r))
{
  while (inlen >= r - pos)
  __loop__(
    assigns(pos, in, inlen,
      memory_slice(s, sizeof(uint64_t) *  MLD_KECCAK_LANES))
    invariant(inlen <= loop_entry(inlen))
    invariant(pos <= r)
    invariant(in == loop_entry(in) + (loop_entry(inlen) - inlen)))
  {
    mld_keccakf1600_xor_bytes(s, in, pos, r - pos);
    inlen -= r - pos;
    in += r - pos;
    mld_keccakf1600_permute(s);
    pos = 0;
  }
  /* Safety: At this point, inlen < r, so the truncation to unsigned is safe. */
  mld_keccakf1600_xor_bytes(s, in, pos, (unsigned)inlen);

  /* Safety: At this point, inlen < r and pos <= r so the truncation to unsigned
   * is safe. */
  return (unsigned)(pos + inlen);
}

/*************************************************
 * Name:        keccak_finalize
 *
 * Description: Finalize absorb step.
 *
 * Arguments:   - uint64_t *s: pointer to Keccak state
 *              - unsigned int pos: position in current block to be absorbed
 *              - unsigned int r: rate in bytes (e.g., 168 for SHAKE128)
 *              - uint8_t p: domain separation byte
 **************************************************/
static void keccak_finalize(uint64_t s[MLD_KECCAK_LANES], unsigned int pos,
                            unsigned int r, uint8_t p)
__contract__(
  requires(pos <= r && r < sizeof(uint64_t) * MLD_KECCAK_LANES)
  requires((r / 8) >= 1)
  requires(memory_no_alias(s, sizeof(uint64_t) * MLD_KECCAK_LANES))
  assigns(memory_slice(s, sizeof(uint64_t) * MLD_KECCAK_LANES))
)
{
  uint8_t b = 0x80;
  mld_keccakf1600_xor_bytes(s, &p, pos, 1);
  mld_keccakf1600_xor_bytes(s, &b, r - 1, 1);
}

/*************************************************
 * Name:        keccak_squeeze
 *
 * Description: Squeeze step of Keccak. Squeezes arbitratrily many bytes.
 *              Modifies the state. Can be called multiple times to keep
 *              squeezing, i.e., is incremental.
 *
 * Arguments:   - uint8_t *out: pointer to output data
 *              - size_t outlen: number of bytes to be squeezed (written to out)
 *              - uint64_t *s: pointer to input/output Keccak state
 *              - unsigned int pos: number of bytes in current block already
 *squeezed
 *              - unsigned int r: rate in bytes (e.g., 168 for SHAKE128)
 *
 * Returns new position pos in current block
 **************************************************/
static unsigned int keccak_squeeze(uint8_t *out, size_t outlen,
                                   uint64_t s[MLD_KECCAK_LANES],
                                   unsigned int pos, unsigned int r)
__contract__(
  requires((r == SHAKE128_RATE && pos <= SHAKE128_RATE) ||
           (r == SHAKE256_RATE && pos <= SHAKE256_RATE) ||
           (r == SHA3_512_RATE && pos <= SHA3_512_RATE))
  requires(outlen <= 8 * r /* somewhat arbitrary bound */)
  requires(memory_no_alias(s, sizeof(uint64_t) * MLD_KECCAK_LANES))
  requires(memory_no_alias(out, outlen))
  assigns(memory_slice(s, sizeof(uint64_t) * MLD_KECCAK_LANES))
  assigns(memory_slice(out, outlen))
  ensures(return_value <= r))
{
  unsigned int i;
  size_t out_offset = 0;

  /* Reference: This code is re-factored from the reference implementation
   * to facilitate proof with CBMC and to improve readability.
   *
   * Take a mutable copy of outlen to count down the number of bytes
   * still to squeeze. The initial value of outlen is needed for the CBMC
   * assigns() clauses. */
  size_t bytes_to_go = outlen;

  while (bytes_to_go > 0)
  __loop__(
    assigns(i, bytes_to_go, pos, out_offset, memory_slice(s, sizeof(uint64_t) * MLD_KECCAK_LANES), memory_slice(out, outlen))
    invariant(bytes_to_go <= outlen)
    invariant(out_offset == outlen - bytes_to_go)
    invariant(pos <= r)
  )
  {
    if (pos == r)
    {
      mld_keccakf1600_permute(s);
      pos = 0;
    }
    /* Safety: If bytes_to_go < r - pos, truncation to unsigned is safe. */
    i = bytes_to_go < r - pos ? (unsigned)bytes_to_go : r - pos;
    mld_keccakf1600_extract_bytes(s, out + out_offset, pos, i);
    bytes_to_go -= i;
    pos += i;
    out_offset += i;
  }

  return pos;
}

void mld_shake128_init(mld_shake128ctx *state)
{
  keccak_init(state->s);
  state->pos = 0;
}

void mld_shake128_absorb(mld_shake128ctx *state, const uint8_t *in,
                         size_t inlen)
{
  state->pos = keccak_absorb(state->s, state->pos, SHAKE128_RATE, in, inlen);
}

void mld_shake128_finalize(mld_shake128ctx *state)
{
  keccak_finalize(state->s, state->pos, SHAKE128_RATE, 0x1F);
  state->pos = SHAKE128_RATE;
}

void mld_shake128_squeeze(uint8_t *out, size_t outlen, mld_shake128ctx *state)
{
  state->pos = keccak_squeeze(out, outlen, state->s, state->pos, SHAKE128_RATE);
}

void mld_shake128_release(mld_shake128ctx *state)
{
  /* @[FIPS204, Section 3.6.3] Destruction of intermediate values. */
  mld_zeroize(state, sizeof(mld_shake128ctx));
}

void mld_shake256_init(mld_shake256ctx *state)
{
  keccak_init(state->s);
  state->pos = 0;
}

void mld_shake256_absorb(mld_shake256ctx *state, const uint8_t *in,
                         size_t inlen)
{
  state->pos = keccak_absorb(state->s, state->pos, SHAKE256_RATE, in, inlen);
}

void mld_shake256_finalize(mld_shake256ctx *state)
{
  keccak_finalize(state->s, state->pos, SHAKE256_RATE, 0x1F);
  state->pos = SHAKE256_RATE;
}

void mld_shake256_squeeze(uint8_t *out, size_t outlen, mld_shake256ctx *state)
{
  state->pos = keccak_squeeze(out, outlen, state->s, state->pos, SHAKE256_RATE);
}

void mld_shake256_release(mld_shake256ctx *state)
{
  /* @[FIPS204, Section 3.6.3] Destruction of intermediate values. */
  mld_zeroize(state, sizeof(mld_shake256ctx));
}

void mld_shake256(uint8_t *out, size_t outlen, const uint8_t *in, size_t inlen)
{
  mld_shake256ctx state;

  mld_shake256_init(&state);
  mld_shake256_absorb(&state, in, inlen);
  mld_shake256_finalize(&state);
  mld_shake256_squeeze(out, outlen, &state);
  mld_shake256_release(&state);
}

#endif /* !MLD_CONFIG_MULTILEVEL_NO_SHARED */
