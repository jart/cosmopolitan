/*
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */
#ifndef MLD_PACKING_H
#define MLD_PACKING_H

#include <stdint.h>
#include "polyvec.h"

#define mld_pack_pk MLD_NAMESPACE_KL(pack_pk)
/*************************************************
 * Name:        mld_pack_pk
 *
 * Description: Bit-pack public key pk = (rho, t1).
 *
 * Arguments:   - uint8_t pk[]: output byte array
 *              - const uint8_t rho[]: byte array containing rho
 *              - const mld_polyveck *t1: pointer to vector t1
 **************************************************/
MLD_INTERNAL_API
void mld_pack_pk(uint8_t pk[MLDSA_CRYPTO_PUBLICKEYBYTES],
                 const uint8_t rho[MLDSA_SEEDBYTES], const mld_polyveck *t1)
__contract__(
  requires(memory_no_alias(pk, MLDSA_CRYPTO_PUBLICKEYBYTES))
  requires(memory_no_alias(rho, MLDSA_SEEDBYTES))
  requires(memory_no_alias(t1, sizeof(mld_polyveck)))
  requires(forall(k0, 0, MLDSA_K,
    array_bound(t1->vec[k0].coeffs, 0, MLDSA_N, 0, 1 << 10)))
  assigns(memory_slice(pk, MLDSA_CRYPTO_PUBLICKEYBYTES))
);


#define mld_pack_sk MLD_NAMESPACE_KL(pack_sk)
/*************************************************
 * Name:        mld_pack_sk
 *
 * Description: Bit-pack secret key sk = (rho, tr, key, t0, s1, s2).
 *
 * Arguments:   - uint8_t sk[]: output byte array
 *              - const uint8_t rho[]: byte array containing rho
 *              - const uint8_t tr[]: byte array containing tr
 *              - const uint8_t key[]: byte array containing key
 *              - const mld_polyveck *t0: pointer to vector t0
 *              - const mld_polyvecl *s1: pointer to vector s1
 *              - const mld_polyveck *s2: pointer to vector s2
 **************************************************/
MLD_INTERNAL_API
void mld_pack_sk(uint8_t sk[MLDSA_CRYPTO_SECRETKEYBYTES],
                 const uint8_t rho[MLDSA_SEEDBYTES],
                 const uint8_t tr[MLDSA_TRBYTES],
                 const uint8_t key[MLDSA_SEEDBYTES], const mld_polyveck *t0,
                 const mld_polyvecl *s1, const mld_polyveck *s2)
__contract__(
  requires(memory_no_alias(sk, MLDSA_CRYPTO_SECRETKEYBYTES))
  requires(memory_no_alias(rho, MLDSA_SEEDBYTES))
  requires(memory_no_alias(tr, MLDSA_TRBYTES))
  requires(memory_no_alias(key, MLDSA_SEEDBYTES))
  requires(memory_no_alias(t0, sizeof(mld_polyveck)))
  requires(memory_no_alias(s1, sizeof(mld_polyvecl)))
  requires(memory_no_alias(s2, sizeof(mld_polyveck)))
  requires(forall(k0, 0, MLDSA_K,
    array_bound(t0->vec[k0].coeffs, 0, MLDSA_N, -(1<<(MLDSA_D-1)) + 1, (1<<(MLDSA_D-1)) + 1)))
  requires(forall(k1, 0, MLDSA_L,
    array_abs_bound(s1->vec[k1].coeffs, 0, MLDSA_N, MLDSA_ETA + 1)))
  requires(forall(k2, 0, MLDSA_K,
    array_abs_bound(s2->vec[k2].coeffs, 0, MLDSA_N, MLDSA_ETA + 1)))
  assigns(memory_slice(sk, MLDSA_CRYPTO_SECRETKEYBYTES))
);


#define mld_pack_sig_c_h MLD_NAMESPACE_KL(pack_sig_c_h)
/*************************************************
 * Name:        mld_pack_sig_c_h
 *
 * Description: Bit-pack c and h component of sig = (c, z, h).
 *              The z component is packed separately using mld_pack_sig_z.
 *
 * Arguments:   - uint8_t sig[]: output byte array
 *              - const uint8_t *c:  pointer to challenge hash length
 *                                   MLDSA_SEEDBYTES
 *              - const mld_polyveck *h: pointer to hint vector h
 *              - const unsigned int number_of_hints: total
 *                                   hints in *h
 *
 * Note that the number_of_hints argument is not present
 * in the reference implementation. It is added here to ease
 * proof of type safety.
 **************************************************/
MLD_INTERNAL_API
void mld_pack_sig_c_h(uint8_t sig[MLDSA_CRYPTO_BYTES],
                      const uint8_t c[MLDSA_CTILDEBYTES], const mld_polyveck *h,
                      const unsigned int number_of_hints)
__contract__(
  requires(memory_no_alias(sig, MLDSA_CRYPTO_BYTES))
  requires(memory_no_alias(c, MLDSA_CTILDEBYTES))
  requires(memory_no_alias(h, sizeof(mld_polyveck)))
  requires(forall(k1, 0, MLDSA_K,
    array_bound(h->vec[k1].coeffs, 0, MLDSA_N, 0, 2)))
  requires(number_of_hints <= MLDSA_OMEGA)
  assigns(memory_slice(sig, MLDSA_CRYPTO_BYTES))
);

#define mld_pack_sig_z MLD_NAMESPACE_KL(pack_sig_z)
/*************************************************
 * Name:        mld_pack_sig_z
 *
 * Description: Bit-pack single polynomial of z component of sig = (c, z, h).
 *              The c and h components are packed separately using
 *              mld_pack_sig_c_h.
 *
 * Arguments:   - uint8_t sig[]: output byte array
 *              - const mld_poly *zi: pointer to a single polynomial in z
 *              - const unsigned int i: index of zi in vector z
 *
 **************************************************/
MLD_INTERNAL_API
void mld_pack_sig_z(uint8_t sig[MLDSA_CRYPTO_BYTES], const mld_poly *zi,
                    unsigned i)
__contract__(
  requires(memory_no_alias(sig, MLDSA_CRYPTO_BYTES))
  requires(memory_no_alias(zi, sizeof(mld_poly)))
  requires(i < MLDSA_L)
  requires(array_bound(zi->coeffs, 0, MLDSA_N, -(MLDSA_GAMMA1 - 1), MLDSA_GAMMA1 + 1))
  assigns(memory_slice(sig, MLDSA_CRYPTO_BYTES))
);

#define mld_unpack_pk MLD_NAMESPACE_KL(unpack_pk)
/*************************************************
 * Name:        mld_unpack_pk
 *
 * Description: Unpack public key pk = (rho, t1).
 *
 * Arguments:   - const uint8_t rho[]: output byte array for rho
 *              - const mld_polyveck *t1: pointer to output vector t1
 *              - uint8_t pk[]: byte array containing bit-packed pk
 **************************************************/
MLD_INTERNAL_API
void mld_unpack_pk(uint8_t rho[MLDSA_SEEDBYTES], mld_polyveck *t1,
                   const uint8_t pk[MLDSA_CRYPTO_PUBLICKEYBYTES])
__contract__(
  requires(memory_no_alias(pk, MLDSA_CRYPTO_PUBLICKEYBYTES))
  requires(memory_no_alias(rho, MLDSA_SEEDBYTES))
  requires(memory_no_alias(t1, sizeof(mld_polyveck)))
  assigns(memory_slice(rho, MLDSA_SEEDBYTES))
  assigns(memory_slice(t1, sizeof(mld_polyveck)))
  ensures(forall(k0, 0, MLDSA_K,
    array_bound(t1->vec[k0].coeffs, 0, MLDSA_N, 0, 1 << 10)))
);


#define mld_unpack_sk MLD_NAMESPACE_KL(unpack_sk)
/*************************************************
 * Name:        mld_unpack_sk
 *
 * Description: Unpack secret key sk = (rho, tr, key, t0, s1, s2).
 *
 * Arguments:   - const uint8_t rho[]: output byte array for rho
 *              - const uint8_t tr[]: output byte array for tr
 *              - const uint8_t key[]: output byte array for key
 *              - const mld_polyveck *t0: pointer to output vector t0
 *              - const mld_polyvecl *s1: pointer to output vector s1
 *              - const mld_polyveck *s2: pointer to output vector s2
 *              - uint8_t sk[]: byte array containing bit-packed sk
 **************************************************/
MLD_INTERNAL_API
void mld_unpack_sk(uint8_t rho[MLDSA_SEEDBYTES], uint8_t tr[MLDSA_TRBYTES],
                   uint8_t key[MLDSA_SEEDBYTES], mld_polyveck *t0,
                   mld_polyvecl *s1, mld_polyveck *s2,
                   const uint8_t sk[MLDSA_CRYPTO_SECRETKEYBYTES])
__contract__(
  requires(memory_no_alias(rho, MLDSA_SEEDBYTES))
  requires(memory_no_alias(tr, MLDSA_TRBYTES))
  requires(memory_no_alias(key, MLDSA_SEEDBYTES))
  requires(memory_no_alias(t0, sizeof(mld_polyveck)))
  requires(memory_no_alias(s1, sizeof(mld_polyvecl)))
  requires(memory_no_alias(s2, sizeof(mld_polyveck)))
  requires(memory_no_alias(sk, MLDSA_CRYPTO_SECRETKEYBYTES))
  assigns(memory_slice(rho, MLDSA_SEEDBYTES))
  assigns(memory_slice(tr, MLDSA_TRBYTES))
  assigns(memory_slice(key, MLDSA_SEEDBYTES))
  assigns(memory_slice(t0, sizeof(mld_polyveck)))
  assigns(memory_slice(s1, sizeof(mld_polyvecl)))
  assigns(memory_slice(s2, sizeof(mld_polyveck)))
  ensures(forall(k0, 0, MLDSA_K,
    array_bound(t0->vec[k0].coeffs, 0, MLDSA_N, -(1<<(MLDSA_D-1)) + 1, (1<<(MLDSA_D-1)) + 1)))
  ensures(forall(k1, 0, MLDSA_L,
    array_bound(s1->vec[k1].coeffs, 0, MLDSA_N, MLD_POLYETA_UNPACK_LOWER_BOUND, MLDSA_ETA + 1)))
  ensures(forall(k2, 0, MLDSA_K,
    array_bound(s2->vec[k2].coeffs, 0, MLDSA_N, MLD_POLYETA_UNPACK_LOWER_BOUND, MLDSA_ETA + 1)))
);

#define mld_unpack_sig MLD_NAMESPACE_KL(unpack_sig)
/*************************************************
 * Name:        mld_unpack_sig
 *
 * Description: Unpack signature sig = (c, z, h).
 *
 * Arguments:   - uint8_t *c: pointer to output challenge hash
 *              - mld_polyvecl *z: pointer to output vector z
 *              - mld_polyveck *h: pointer to output hint vector h
 *              - const uint8_t sig[]: byte array containing
 *                bit-packed signature
 *
 * Returns 1 in case of malformed signature; otherwise 0.
 **************************************************/
MLD_INTERNAL_API
MLD_MUST_CHECK_RETURN_VALUE
int mld_unpack_sig(uint8_t c[MLDSA_CTILDEBYTES], mld_polyvecl *z,
                   mld_polyveck *h, const uint8_t sig[MLDSA_CRYPTO_BYTES])
__contract__(
  requires(memory_no_alias(sig, MLDSA_CRYPTO_BYTES))
  requires(memory_no_alias(c, MLDSA_CTILDEBYTES))
  requires(memory_no_alias(z, sizeof(mld_polyvecl)))
  requires(memory_no_alias(h, sizeof(mld_polyveck)))
  assigns(memory_slice(c, MLDSA_CTILDEBYTES))
  assigns(memory_slice(z, sizeof(mld_polyvecl)))
  assigns(memory_slice(h, sizeof(mld_polyveck)))
  ensures(forall(k0, 0, MLDSA_L,
    array_bound(z->vec[k0].coeffs, 0, MLDSA_N, -(MLDSA_GAMMA1 - 1), MLDSA_GAMMA1 + 1)))
  ensures(forall(k1, 0, MLDSA_K,
    array_bound(h->vec[k1].coeffs, 0, MLDSA_N, 0, 2)))
  ensures(return_value >= 0 && return_value <= 1)
);
#endif /* !MLD_PACKING_H */
