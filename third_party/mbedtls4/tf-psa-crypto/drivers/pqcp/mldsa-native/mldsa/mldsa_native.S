/*
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */

/*
 * WARNING: This file is auto-generated from scripts/autogen
 *          in the mldsa-native repository.
 *          Do not modify it directly.
 */

/******************************************************************************
 *
 * Single assembly unit for fixed-level build of mldsa-native
 *
 * This assembly unit bundles together all assembly files for a build
 * of mldsa-native for a fixed security level (MLDSA-44/65/87).
 *
 * # Multi-level build
 *
 * If you want an SCU build of mldsa-native with support for multiple security
 * levels, you should include this file once with
 * MLD_CONFIG_MULTILEVEL_WITH_SHARED set.
 *
 * (You could also follow the same pattern as for mldsa_native_monobuild.c
 *  and include it for every level, setting MLD_CONFIG_MULTILEVEL_NO_SHARED
 *  for all but one. For builds with MLD_CONFIG_MULTILEVEL_NO_SHARED, this
 *  file will then be ignored.)
 *
 * # Configuration
 *
 * The following options from the mldsa-native configuration are relevant:
 *
 * - MLD_CONFIG_FIPS202_CUSTOM_HEADER
 *   Set this option if you use a custom FIPS202 implementation.
 *
 * - MLD_CONFIG_USE_NATIVE_BACKEND_ARITH
 *   Set this option if you want to include the native arithmetic backends
 *   in your build.
 *
 * - MLD_CONFIG_USE_NATIVE_BACKEND_FIPS202
 *   Set this option if you want to include the native FIPS202 backends
 *   in your build.
 *
 * - MLD_CONFIG_MONOBUILD_KEEP_SHARED_HEADERS
 *   Set this option if you want to keep the directives defined in
 *   level-independent headers. This is needed for a multi-level build.
 */

/* If parts of the mldsa-native source tree are not used,
 * consider reducing this header via `unifdef`.
 *
 * Example:
 * ```bash
 * unifdef -UMLD_CONFIG_USE_NATIVE_BACKEND_ARITH mldsa_native.S
 * ```
 */

#include "src/common.h"

#if defined(MLD_CONFIG_USE_NATIVE_BACKEND_ARITH)
#if defined(MLD_SYS_AARCH64)
#include "src/native/aarch64/src/intt.S"
#include "src/native/aarch64/src/mld_polyvecl_pointwise_acc_montgomery_l4.S"
#include "src/native/aarch64/src/mld_polyvecl_pointwise_acc_montgomery_l5.S"
#include "src/native/aarch64/src/mld_polyvecl_pointwise_acc_montgomery_l7.S"
#include "src/native/aarch64/src/ntt.S"
#include "src/native/aarch64/src/pointwise_montgomery.S"
#include "src/native/aarch64/src/poly_caddq_asm.S"
#include "src/native/aarch64/src/poly_chknorm_asm.S"
#include "src/native/aarch64/src/poly_decompose_32_asm.S"
#include "src/native/aarch64/src/poly_decompose_88_asm.S"
#include "src/native/aarch64/src/poly_use_hint_32_asm.S"
#include "src/native/aarch64/src/poly_use_hint_88_asm.S"
#include "src/native/aarch64/src/polyz_unpack_17_asm.S"
#include "src/native/aarch64/src/polyz_unpack_19_asm.S"
#include "src/native/aarch64/src/rej_uniform_asm.S"
#include "src/native/aarch64/src/rej_uniform_eta2_asm.S"
#include "src/native/aarch64/src/rej_uniform_eta4_asm.S"
#endif /* MLD_SYS_AARCH64 */
#if defined(MLD_SYS_X86_64)
#include "src/native/x86_64/src/intt.S"
#include "src/native/x86_64/src/ntt.S"
#include "src/native/x86_64/src/nttunpack.S"
#include "src/native/x86_64/src/pointwise.S"
#include "src/native/x86_64/src/pointwise_acc_l4.S"
#include "src/native/x86_64/src/pointwise_acc_l5.S"
#include "src/native/x86_64/src/pointwise_acc_l7.S"
#endif /* MLD_SYS_X86_64 */
#endif /* MLD_CONFIG_USE_NATIVE_BACKEND_ARITH */

#if defined(MLD_CONFIG_USE_NATIVE_BACKEND_FIPS202)
#if defined(MLD_SYS_AARCH64)
#include "src/fips202/native/aarch64/src/keccak_f1600_x1_scalar_asm.S"
#include "src/fips202/native/aarch64/src/keccak_f1600_x1_v84a_asm.S"
#include "src/fips202/native/aarch64/src/keccak_f1600_x2_v84a_asm.S"
#include "src/fips202/native/aarch64/src/keccak_f1600_x4_v8a_scalar_hybrid_asm.S"
#include "src/fips202/native/aarch64/src/keccak_f1600_x4_v8a_v84a_scalar_hybrid_asm.S"
#endif /* MLD_SYS_AARCH64 */
#if defined(MLD_SYS_X86_64)
#endif
#endif /* MLD_CONFIG_USE_NATIVE_BACKEND_FIPS202 */


/* Macro #undef's
 *
 * The following undefines macros from headers
 * included by the source files imported above.
 *
 * This is to allow building and linking multiple builds
 * of mldsa-native for varying parameter sets through concatenation
 * of this file, as if the files had been compiled separately.
 * If this is not relevant to you, you may remove the following.
 *
 * NOTE: This is not needed for the assembly SCU since, at present,
 * there is no need to include it multiple times.
 * We keep it for uniformity with mldsa_native.c only.
 *
 * NOTE: To avoid having to distinguish between which headers are included
 * from the assembly files, we #undef the same set of directives
 * as in mldsa_native.c
 */

/*
 * Undefine macros from MLD_CONFIG_PARAMETER_SET-specific files
 */
/* mldsa/mldsa_native.h */
#undef CRYPTO_BYTES
#undef CRYPTO_PUBLICKEYBYTES
#undef CRYPTO_SECRETKEYBYTES
#undef MLDSA44_BYTES
#undef MLDSA44_CRHBYTES
#undef MLDSA44_PUBLICKEYBYTES
#undef MLDSA44_RNDBYTES
#undef MLDSA44_SECRETKEYBYTES
#undef MLDSA44_SEEDBYTES
#undef MLDSA44_TRBYTES
#undef MLDSA65_BYTES
#undef MLDSA65_CRHBYTES
#undef MLDSA65_PUBLICKEYBYTES
#undef MLDSA65_RNDBYTES
#undef MLDSA65_SECRETKEYBYTES
#undef MLDSA65_SEEDBYTES
#undef MLDSA65_TRBYTES
#undef MLDSA87_BYTES
#undef MLDSA87_CRHBYTES
#undef MLDSA87_PUBLICKEYBYTES
#undef MLDSA87_RNDBYTES
#undef MLDSA87_SECRETKEYBYTES
#undef MLDSA87_SEEDBYTES
#undef MLDSA87_TRBYTES
#undef MLDSA_BYTES
#undef MLDSA_BYTES_
#undef MLDSA_CRHBYTES
#undef MLDSA_PUBLICKEYBYTES
#undef MLDSA_PUBLICKEYBYTES_
#undef MLDSA_RNDBYTES
#undef MLDSA_SECRETKEYBYTES
#undef MLDSA_SECRETKEYBYTES_
#undef MLDSA_SEEDBYTES
#undef MLDSA_TRBYTES
#undef MLD_API_CONCAT
#undef MLD_API_CONCAT_
#undef MLD_API_CONCAT_UNDERSCORE
#undef MLD_API_LEGACY_CONFIG
#undef MLD_API_MUST_CHECK_RETURN_VALUE
#undef MLD_API_NAMESPACE
#undef MLD_API_QUALIFIER
#undef MLD_CONFIG_API_CONSTANTS_ONLY
#undef MLD_CONFIG_API_NAMESPACE_PREFIX
#undef MLD_CONFIG_API_NO_SUPERCOP
#undef MLD_CONFIG_API_PARAMETER_SET
#undef MLD_CONFIG_API_QUALIFIER
#undef MLD_DOMAIN_SEPARATION_MAX_BYTES
#undef MLD_ERR_FAIL
#undef MLD_ERR_OUT_OF_MEMORY
#undef MLD_ERR_RNG_FAIL
#undef MLD_H
#undef MLD_MAX3_
#undef MLD_PREHASH_NONE
#undef MLD_PREHASH_SHA2_224
#undef MLD_PREHASH_SHA2_256
#undef MLD_PREHASH_SHA2_384
#undef MLD_PREHASH_SHA2_512
#undef MLD_PREHASH_SHA2_512_224
#undef MLD_PREHASH_SHA2_512_256
#undef MLD_PREHASH_SHA3_224
#undef MLD_PREHASH_SHA3_256
#undef MLD_PREHASH_SHA3_384
#undef MLD_PREHASH_SHA3_512
#undef MLD_PREHASH_SHAKE_128
#undef MLD_PREHASH_SHAKE_256
#undef MLD_TOTAL_ALLOC_44
#undef MLD_TOTAL_ALLOC_44_KEYPAIR
#undef MLD_TOTAL_ALLOC_44_KEYPAIR_NO_PCT
#undef MLD_TOTAL_ALLOC_44_KEYPAIR_PCT
#undef MLD_TOTAL_ALLOC_44_SIGN
#undef MLD_TOTAL_ALLOC_44_VERIFY
#undef MLD_TOTAL_ALLOC_65
#undef MLD_TOTAL_ALLOC_65_KEYPAIR
#undef MLD_TOTAL_ALLOC_65_KEYPAIR_NO_PCT
#undef MLD_TOTAL_ALLOC_65_KEYPAIR_PCT
#undef MLD_TOTAL_ALLOC_65_SIGN
#undef MLD_TOTAL_ALLOC_65_VERIFY
#undef MLD_TOTAL_ALLOC_87
#undef MLD_TOTAL_ALLOC_87_KEYPAIR
#undef MLD_TOTAL_ALLOC_87_KEYPAIR_NO_PCT
#undef MLD_TOTAL_ALLOC_87_KEYPAIR_PCT
#undef MLD_TOTAL_ALLOC_87_SIGN
#undef MLD_TOTAL_ALLOC_87_VERIFY
#undef crypto_sign
#undef crypto_sign_keypair
#undef crypto_sign_open
#undef crypto_sign_signature
#undef crypto_sign_verify
/* mldsa/src/common.h */
#undef MLD_ADD_PARAM_SET
#undef MLD_ALLOC
#undef MLD_APPLY
#undef MLD_ASM_FN_SYMBOL
#undef MLD_ASM_NAMESPACE
#undef MLD_BUILD_INTERNAL
#undef MLD_COMMON_H
#undef MLD_CONCAT
#undef MLD_CONCAT_
#undef MLD_CONTEXT_PARAMETERS_0
#undef MLD_CONTEXT_PARAMETERS_1
#undef MLD_CONTEXT_PARAMETERS_2
#undef MLD_CONTEXT_PARAMETERS_3
#undef MLD_CONTEXT_PARAMETERS_4
#undef MLD_CONTEXT_PARAMETERS_5
#undef MLD_CONTEXT_PARAMETERS_6
#undef MLD_CONTEXT_PARAMETERS_7
#undef MLD_CONTEXT_PARAMETERS_8
#undef MLD_CONTEXT_PARAMETERS_9
#undef MLD_EMPTY_CU
#undef MLD_ERR_FAIL
#undef MLD_ERR_OUT_OF_MEMORY
#undef MLD_ERR_RNG_FAIL
#undef MLD_EXTERNAL_API
#undef MLD_FIPS202X4_HEADER_FILE
#undef MLD_FIPS202_HEADER_FILE
#undef MLD_FREE
#undef MLD_INTERNAL_API
#undef MLD_MULTILEVEL_BUILD
#undef MLD_NAMESPACE
#undef MLD_NAMESPACE_KL
#undef MLD_NAMESPACE_PREFIX
#undef MLD_NAMESPACE_PREFIX_KL
#undef MLD_UNION_OR_STRUCT
#undef mld_memcpy
#undef mld_memset
/* mldsa/src/packing.h */
#undef MLD_PACKING_H
#undef mld_pack_pk
#undef mld_pack_sig_c_h
#undef mld_pack_sig_z
#undef mld_pack_sk
#undef mld_unpack_pk
#undef mld_unpack_sig
#undef mld_unpack_sk
/* mldsa/src/params.h */
#undef MLDSA_BETA
#undef MLDSA_CRHBYTES
#undef MLDSA_CRYPTO_BYTES
#undef MLDSA_CRYPTO_PUBLICKEYBYTES
#undef MLDSA_CRYPTO_SECRETKEYBYTES
#undef MLDSA_CTILDEBYTES
#undef MLDSA_D
#undef MLDSA_ETA
#undef MLDSA_GAMMA1
#undef MLDSA_GAMMA2
#undef MLDSA_K
#undef MLDSA_L
#undef MLDSA_N
#undef MLDSA_OMEGA
#undef MLDSA_POLYETA_PACKEDBYTES
#undef MLDSA_POLYT0_PACKEDBYTES
#undef MLDSA_POLYT1_PACKEDBYTES
#undef MLDSA_POLYVECH_PACKEDBYTES
#undef MLDSA_POLYW1_PACKEDBYTES
#undef MLDSA_POLYZ_PACKEDBYTES
#undef MLDSA_Q
#undef MLDSA_Q_HALF
#undef MLDSA_RNDBYTES
#undef MLDSA_SEEDBYTES
#undef MLDSA_TAU
#undef MLDSA_TRBYTES
#undef MLD_PARAMS_H
/* mldsa/src/poly_kl.h */
#undef MLD_POLYETA_UNPACK_LOWER_BOUND
#undef MLD_POLY_KL_H
#undef mld_poly_challenge
#undef mld_poly_decompose
#undef mld_poly_make_hint
#undef mld_poly_uniform_eta
#undef mld_poly_uniform_eta_4x
#undef mld_poly_uniform_gamma1
#undef mld_poly_uniform_gamma1_4x
#undef mld_poly_use_hint
#undef mld_polyeta_pack
#undef mld_polyeta_unpack
#undef mld_polyw1_pack
#undef mld_polyz_pack
#undef mld_polyz_unpack
/* mldsa/src/polyvec.h */
#undef MLD_POLYVEC_H
#undef mld_polymat
#undef mld_polymat_get_row
#undef mld_polyvec_matrix_expand
#undef mld_polyvec_matrix_pointwise_montgomery
#undef mld_polyveck
#undef mld_polyveck_add
#undef mld_polyveck_caddq
#undef mld_polyveck_chknorm
#undef mld_polyveck_decompose
#undef mld_polyveck_invntt_tomont
#undef mld_polyveck_make_hint
#undef mld_polyveck_ntt
#undef mld_polyveck_pack_eta
#undef mld_polyveck_pack_t0
#undef mld_polyveck_pack_w1
#undef mld_polyveck_pointwise_poly_montgomery
#undef mld_polyveck_power2round
#undef mld_polyveck_reduce
#undef mld_polyveck_shiftl
#undef mld_polyveck_sub
#undef mld_polyveck_unpack_eta
#undef mld_polyveck_unpack_t0
#undef mld_polyveck_use_hint
#undef mld_polyvecl
#undef mld_polyvecl_chknorm
#undef mld_polyvecl_ntt
#undef mld_polyvecl_pack_eta
#undef mld_polyvecl_pointwise_acc_montgomery
#undef mld_polyvecl_uniform_gamma1
#undef mld_polyvecl_unpack_eta
#undef mld_polyvecl_unpack_z
/* mldsa/src/rounding.h */
#undef MLD_2_POW_D
#undef MLD_ROUNDING_H
#undef mld_decompose
#undef mld_make_hint
#undef mld_power2round
#undef mld_use_hint
/* mldsa/src/sign.h */
#undef MLD_DOMAIN_SEPARATION_MAX_BYTES
#undef MLD_PREHASH_NONE
#undef MLD_PREHASH_SHA2_224
#undef MLD_PREHASH_SHA2_256
#undef MLD_PREHASH_SHA2_384
#undef MLD_PREHASH_SHA2_512
#undef MLD_PREHASH_SHA2_512_224
#undef MLD_PREHASH_SHA2_512_256
#undef MLD_PREHASH_SHA3_224
#undef MLD_PREHASH_SHA3_256
#undef MLD_PREHASH_SHA3_384
#undef MLD_PREHASH_SHA3_512
#undef MLD_PREHASH_SHAKE_128
#undef MLD_PREHASH_SHAKE_256
#undef MLD_SIGN_H
#undef mld_prepare_domain_separation_prefix
#undef mld_sign
#undef mld_sign_keypair
#undef mld_sign_keypair_internal
#undef mld_sign_open
#undef mld_sign_pk_from_sk
#undef mld_sign_signature
#undef mld_sign_signature_extmu
#undef mld_sign_signature_internal
#undef mld_sign_signature_pre_hash_internal
#undef mld_sign_signature_pre_hash_shake256
#undef mld_sign_verify
#undef mld_sign_verify_extmu
#undef mld_sign_verify_internal
#undef mld_sign_verify_pre_hash_internal
#undef mld_sign_verify_pre_hash_shake256

#if !defined(MLD_CONFIG_MONOBUILD_KEEP_SHARED_HEADERS)
/*
 * Undefine macros from MLD_CONFIG_PARAMETER_SET-generic files
 */
/* mldsa/src/ct.h */
#undef MLD_CT_H
#undef MLD_USE_ASM_VALUE_BARRIER
#undef mld_ct_opt_blocker_u64
/* mldsa/src/debug.h */
#undef MLD_DEBUG_H
#undef mld_assert
#undef mld_assert_abs_bound
#undef mld_assert_abs_bound_2d
#undef mld_assert_bound
#undef mld_assert_bound_2d
#undef mld_debug_check_assert
#undef mld_debug_check_bounds
/* mldsa/src/poly.h */
#undef MLD_INTT_BOUND
#undef MLD_NTT_BOUND
#undef MLD_POLY_H
#undef mld_poly_add
#undef mld_poly_caddq
#undef mld_poly_chknorm
#undef mld_poly_invntt_tomont
#undef mld_poly_ntt
#undef mld_poly_pointwise_montgomery
#undef mld_poly_power2round
#undef mld_poly_reduce
#undef mld_poly_shiftl
#undef mld_poly_sub
#undef mld_poly_uniform
#undef mld_poly_uniform_4x
#undef mld_polyt0_pack
#undef mld_polyt0_unpack
#undef mld_polyt1_pack
#undef mld_polyt1_unpack
/* mldsa/src/randombytes.h */
#undef MLD_RANDOMBYTES_H
/* mldsa/src/reduce.h */
#undef MLD_MONT
#undef MLD_REDUCE32_DOMAIN_MAX
#undef MLD_REDUCE32_RANGE_MAX
#undef MLD_REDUCE_H
/* mldsa/src/symmetric.h */
#undef MLD_STREAM128_BLOCKBYTES
#undef MLD_STREAM256_BLOCKBYTES
#undef MLD_SYMMETRIC_H
#undef mld_xof128_absorb_once
#undef mld_xof128_ctx
#undef mld_xof128_init
#undef mld_xof128_release
#undef mld_xof128_squeezeblocks
#undef mld_xof128_x4_absorb
#undef mld_xof128_x4_ctx
#undef mld_xof128_x4_init
#undef mld_xof128_x4_release
#undef mld_xof128_x4_squeezeblocks
#undef mld_xof256_absorb_once
#undef mld_xof256_ctx
#undef mld_xof256_init
#undef mld_xof256_release
#undef mld_xof256_squeezeblocks
#undef mld_xof256_x4_absorb
#undef mld_xof256_x4_ctx
#undef mld_xof256_x4_init
#undef mld_xof256_x4_release
#undef mld_xof256_x4_squeezeblocks
/* mldsa/src/sys.h */
#undef MLD_ALIGN
#undef MLD_ALIGN_UP
#undef MLD_ALWAYS_INLINE
#undef MLD_CET_ENDBR
#undef MLD_CT_TESTING_DECLASSIFY
#undef MLD_CT_TESTING_SECRET
#undef MLD_DEFAULT_ALIGN
#undef MLD_HAVE_INLINE_ASM
#undef MLD_INLINE
#undef MLD_MUST_CHECK_RETURN_VALUE
#undef MLD_RESTRICT
#undef MLD_STATIC_TESTABLE
#undef MLD_SYS_AARCH64
#undef MLD_SYS_AARCH64_EB
#undef MLD_SYS_APPLE
#undef MLD_SYS_BIG_ENDIAN
#undef MLD_SYS_H
#undef MLD_SYS_LINUX
#undef MLD_SYS_LITTLE_ENDIAN
#undef MLD_SYS_PPC64LE
#undef MLD_SYS_RISCV32
#undef MLD_SYS_RISCV64
#undef MLD_SYS_WINDOWS
#undef MLD_SYS_X86_64
#undef MLD_SYS_X86_64_AVX2
/* mldsa/src/cbmc.h */
#undef MLD_CBMC_H
#undef __contract__
#undef __loop__

#if !defined(MLD_CONFIG_FIPS202_CUSTOM_HEADER)
/*
 * Undefine macros from FIPS-202 files
 */
/* mldsa/src/fips202/fips202.h */
#undef MLD_FIPS202_FIPS202_H
#undef MLD_KECCAK_LANES
#undef SHA3_256_HASHBYTES
#undef SHA3_256_RATE
#undef SHA3_512_HASHBYTES
#undef SHA3_512_RATE
#undef SHAKE128_RATE
#undef SHAKE256_RATE
#undef mld_shake128_absorb
#undef mld_shake128_finalize
#undef mld_shake128_init
#undef mld_shake128_release
#undef mld_shake128_squeeze
#undef mld_shake256
#undef mld_shake256_absorb
#undef mld_shake256_finalize
#undef mld_shake256_init
#undef mld_shake256_release
#undef mld_shake256_squeeze
/* mldsa/src/fips202/fips202x4.h */
#undef MLD_FIPS202_FIPS202X4_H
#undef mld_shake128x4_absorb_once
#undef mld_shake128x4_init
#undef mld_shake128x4_release
#undef mld_shake128x4_squeezeblocks
#undef mld_shake256x4_absorb_once
#undef mld_shake256x4_init
#undef mld_shake256x4_release
#undef mld_shake256x4_squeezeblocks
/* mldsa/src/fips202/keccakf1600.h */
#undef MLD_FIPS202_KECCAKF1600_H
#undef MLD_KECCAK_LANES
#undef MLD_KECCAK_WAY
#undef mld_keccakf1600_extract_bytes
#undef mld_keccakf1600_permute
#undef mld_keccakf1600_xor_bytes
#undef mld_keccakf1600x4_extract_bytes
#undef mld_keccakf1600x4_permute
#undef mld_keccakf1600x4_xor_bytes
#endif /* !MLD_CONFIG_FIPS202_CUSTOM_HEADER */

#if defined(MLD_CONFIG_USE_NATIVE_BACKEND_FIPS202)
/* mldsa/src/fips202/native/api.h */
#undef MLD_FIPS202_NATIVE_API_H
#undef MLD_NATIVE_FUNC_FALLBACK
#undef MLD_NATIVE_FUNC_SUCCESS
/* mldsa/src/fips202/native/auto.h */
#undef MLD_FIPS202_NATIVE_AUTO_H
#if defined(MLD_SYS_AARCH64)
/*
 * Undefine macros from native code (FIPS202, AArch64)
 */
/* mldsa/src/fips202/native/aarch64/auto.h */
#undef MLD_FIPS202_NATIVE_AARCH64_AUTO_H
/* mldsa/src/fips202/native/aarch64/src/fips202_native_aarch64.h */
#undef MLD_FIPS202_NATIVE_AARCH64_SRC_FIPS202_NATIVE_AARCH64_H
#undef mld_keccak_f1600_x1_scalar_asm
#undef mld_keccak_f1600_x1_v84a_asm
#undef mld_keccak_f1600_x2_v84a_asm
#undef mld_keccak_f1600_x4_scalar_v8a_hybrid_asm
#undef mld_keccak_f1600_x4_scalar_v8a_v84a_hybrid_asm
#undef mld_keccakf1600_round_constants
/* mldsa/src/fips202/native/aarch64/x1_scalar.h */
#undef MLD_FIPS202_AARCH64_NEED_X1_SCALAR
#undef MLD_FIPS202_NATIVE_AARCH64_X1_SCALAR_H
#undef MLD_USE_FIPS202_X1_NATIVE
/* mldsa/src/fips202/native/aarch64/x1_v84a.h */
#undef MLD_FIPS202_AARCH64_NEED_X1_V84A
#undef MLD_FIPS202_NATIVE_AARCH64_X1_V84A_H
#undef MLD_USE_FIPS202_X1_NATIVE
/* mldsa/src/fips202/native/aarch64/x2_v84a.h */
#undef MLD_FIPS202_AARCH64_NEED_X2_V84A
#undef MLD_FIPS202_NATIVE_AARCH64_X2_V84A_H
#undef MLD_USE_FIPS202_X4_NATIVE
/* mldsa/src/fips202/native/aarch64/x4_v8a_scalar.h */
#undef MLD_FIPS202_AARCH64_NEED_X4_V8A_SCALAR_HYBRID
#undef MLD_FIPS202_NATIVE_AARCH64_X4_V8A_SCALAR_H
#undef MLD_USE_FIPS202_X4_NATIVE
/* mldsa/src/fips202/native/aarch64/x4_v8a_v84a_scalar.h */
#undef MLD_FIPS202_AARCH64_NEED_X4_V8A_V84A_SCALAR_HYBRID
#undef MLD_FIPS202_NATIVE_AARCH64_X4_V8A_V84A_SCALAR_H
#undef MLD_USE_FIPS202_X4_NATIVE
#endif /* MLD_SYS_AARCH64 */
#if defined(MLD_SYS_X86_64)
/*
 * Undefine macros from native code (FIPS202, x86_64)
 */
/* mldsa/src/fips202/native/x86_64/src/KeccakP_1600_times4_SIMD256.h */
#undef MLD_FIPS202_NATIVE_X86_64_SRC_KECCAKP_1600_TIMES4_SIMD256_H
#undef mld_keccakf1600x4_permute24
/* mldsa/src/fips202/native/x86_64/xkcp.h */
#undef MLD_FIPS202_NATIVE_X86_64_XKCP_H
#undef MLD_FIPS202_X86_64_XKCP
#undef MLD_USE_FIPS202_X4_NATIVE
#endif /* MLD_SYS_X86_64 */
#endif /* MLD_CONFIG_USE_NATIVE_BACKEND_FIPS202 */
#if defined(MLD_CONFIG_USE_NATIVE_BACKEND_ARITH)
/* mldsa/src/native/api.h */
#undef MLD_INTT_BOUND
#undef MLD_NATIVE_API_H
#undef MLD_NATIVE_FUNC_FALLBACK
#undef MLD_NATIVE_FUNC_SUCCESS
#undef MLD_NTT_BOUND
#undef REDUCE32_RANGE_MAX
/* mldsa/src/native/meta.h */
#undef MLD_NATIVE_META_H
#if defined(MLD_SYS_AARCH64)
/*
 * Undefine macros from native code (Arith, AArch64)
 */
/* mldsa/src/native/aarch64/meta.h */
#undef MLD_ARITH_BACKEND_AARCH64
#undef MLD_NATIVE_AARCH64_META_H
#undef MLD_USE_NATIVE_INTT
#undef MLD_USE_NATIVE_NTT
#undef MLD_USE_NATIVE_POINTWISE_MONTGOMERY
#undef MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L4
#undef MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L5
#undef MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L7
#undef MLD_USE_NATIVE_POLYZ_UNPACK_17
#undef MLD_USE_NATIVE_POLYZ_UNPACK_19
#undef MLD_USE_NATIVE_POLY_CADDQ
#undef MLD_USE_NATIVE_POLY_CHKNORM
#undef MLD_USE_NATIVE_POLY_DECOMPOSE_32
#undef MLD_USE_NATIVE_POLY_DECOMPOSE_88
#undef MLD_USE_NATIVE_POLY_USE_HINT_32
#undef MLD_USE_NATIVE_POLY_USE_HINT_88
#undef MLD_USE_NATIVE_REJ_UNIFORM
#undef MLD_USE_NATIVE_REJ_UNIFORM_ETA2
#undef MLD_USE_NATIVE_REJ_UNIFORM_ETA4
/* mldsa/src/native/aarch64/src/arith_native_aarch64.h */
#undef MLD_AARCH64_REJ_UNIFORM_ETA2_BUFLEN
#undef MLD_AARCH64_REJ_UNIFORM_ETA4_BUFLEN
#undef MLD_NATIVE_AARCH64_SRC_ARITH_NATIVE_AARCH64_H
#undef mld_aarch64_intt_zetas_layer123456
#undef mld_aarch64_intt_zetas_layer78
#undef mld_aarch64_ntt_zetas_layer123456
#undef mld_aarch64_ntt_zetas_layer78
#undef mld_intt_asm
#undef mld_ntt_asm
#undef mld_poly_caddq_asm
#undef mld_poly_chknorm_asm
#undef mld_poly_decompose_32_asm
#undef mld_poly_decompose_88_asm
#undef mld_poly_pointwise_montgomery_asm
#undef mld_poly_use_hint_32_asm
#undef mld_poly_use_hint_88_asm
#undef mld_polyvecl_pointwise_acc_montgomery_l4_asm
#undef mld_polyvecl_pointwise_acc_montgomery_l5_asm
#undef mld_polyvecl_pointwise_acc_montgomery_l7_asm
#undef mld_polyz_unpack_17_asm
#undef mld_polyz_unpack_17_indices
#undef mld_polyz_unpack_19_asm
#undef mld_polyz_unpack_19_indices
#undef mld_rej_uniform_asm
#undef mld_rej_uniform_eta2_asm
#undef mld_rej_uniform_eta4_asm
#undef mld_rej_uniform_eta_table
#undef mld_rej_uniform_table
#endif /* MLD_SYS_AARCH64 */
#if defined(MLD_SYS_X86_64)
/*
 * Undefine macros from native code (Arith, X86_64)
 */
/* mldsa/src/native/x86_64/meta.h */
#undef MLD_ARITH_BACKEND_X86_64_DEFAULT
#undef MLD_NATIVE_X86_64_META_H
#undef MLD_USE_NATIVE_INTT
#undef MLD_USE_NATIVE_NTT
#undef MLD_USE_NATIVE_NTT_CUSTOM_ORDER
#undef MLD_USE_NATIVE_POINTWISE_MONTGOMERY
#undef MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L4
#undef MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L5
#undef MLD_USE_NATIVE_POLYVECL_POINTWISE_ACC_MONTGOMERY_L7
#undef MLD_USE_NATIVE_POLYZ_UNPACK_17
#undef MLD_USE_NATIVE_POLYZ_UNPACK_19
#undef MLD_USE_NATIVE_POLY_CADDQ
#undef MLD_USE_NATIVE_POLY_CHKNORM
#undef MLD_USE_NATIVE_POLY_DECOMPOSE_32
#undef MLD_USE_NATIVE_POLY_DECOMPOSE_88
#undef MLD_USE_NATIVE_POLY_USE_HINT_32
#undef MLD_USE_NATIVE_POLY_USE_HINT_88
#undef MLD_USE_NATIVE_REJ_UNIFORM
#undef MLD_USE_NATIVE_REJ_UNIFORM_ETA2
#undef MLD_USE_NATIVE_REJ_UNIFORM_ETA4
/* mldsa/src/native/x86_64/src/arith_native_x86_64.h */
#undef MLD_AVX2_REJ_UNIFORM_BUFLEN
#undef MLD_AVX2_REJ_UNIFORM_ETA2_BUFLEN
#undef MLD_AVX2_REJ_UNIFORM_ETA4_BUFLEN
#undef MLD_NATIVE_X86_64_SRC_ARITH_NATIVE_X86_64_H
#undef mld_invntt_avx2
#undef mld_ntt_avx2
#undef mld_nttunpack_avx2
#undef mld_pointwise_acc_l4_avx2
#undef mld_pointwise_acc_l5_avx2
#undef mld_pointwise_acc_l7_avx2
#undef mld_pointwise_avx2
#undef mld_poly_caddq_avx2
#undef mld_poly_chknorm_avx2
#undef mld_poly_decompose_32_avx2
#undef mld_poly_decompose_88_avx2
#undef mld_poly_use_hint_32_avx2
#undef mld_poly_use_hint_88_avx2
#undef mld_polyz_unpack_17_avx2
#undef mld_polyz_unpack_19_avx2
#undef mld_rej_uniform_avx2
#undef mld_rej_uniform_eta2_avx2
#undef mld_rej_uniform_eta4_avx2
#undef mld_rej_uniform_table
/* mldsa/src/native/x86_64/src/consts.h */
#undef MLD_AVX2_BACKEND_DATA_OFFSET_8XDIV
#undef MLD_AVX2_BACKEND_DATA_OFFSET_8XDIV_QINV
#undef MLD_AVX2_BACKEND_DATA_OFFSET_8XQ
#undef MLD_AVX2_BACKEND_DATA_OFFSET_8XQINV
#undef MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS
#undef MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS_QINV
#undef MLD_NATIVE_X86_64_SRC_CONSTS_H
#undef mld_qdata
#endif /* MLD_SYS_X86_64 */
#endif /* MLD_CONFIG_USE_NATIVE_BACKEND_ARITH */
#endif /* !MLD_CONFIG_MONOBUILD_KEEP_SHARED_HEADERS */
