/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:4;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright The Mbed TLS Contributors                                          │
│                                                                              │
│ Licensed under the Apache License, Version 2.0 (the "License");              │
│ you may not use this file except in compliance with the License.             │
│ You may obtain a copy of the License at                                      │
│                                                                              │
│     http://www.apache.org/licenses/LICENSE-2.0                               │
│                                                                              │
│ Unless required by applicable law or agreed to in writing, software          │
│ distributed under the License is distributed on an "AS IS" BASIS,            │
│ WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.     │
│ See the License for the specific language governing permissions and          │
│ limitations under the License.                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/mbedtls/rsa.h"
#include "libc/intrin/strace.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "third_party/mbedtls/common.h"
#include "third_party/mbedtls/error.h"
#include "third_party/mbedtls/md.h"
#include "third_party/mbedtls/oid.h"
#include "third_party/mbedtls/platform.h"
#include "third_party/mbedtls/profile.h"
#include "third_party/mbedtls/rsa_internal.h"
#include "third_party/mbedtls/sha1.h"
__static_yoink("mbedtls_notice");

/**
 * @fileoverview The RSA public-key cryptosystem
 *
 *     LENGTH         SECURITY
 *     -------------- --------
 *     RSA 512        57
 *     RSA 1024       80
 *     RSA 2048       110
 *     RSA 4096       150
 *     RSA 8192       202
 *     RSA 16384      270
 *     RSA 32768      359
 *     RSA 65536      475
 *     RSA 131072     626
 *
 * (1.923*cbrt(L*log(2))*cbrt(log(L*log(2))*log(L*log(2)))-4.69)/log(2)
 *
 * The following sources were referenced in the design of this implementation
 * of the RSA algorithm:
 *
 * [1] A method for obtaining digital signatures and public-key cryptosystems
 *     R Rivest, A Shamir, and L Adleman
 *     http://people.csail.mit.edu/rivest/pubs.html#RSA78
 *
 * [2] Handbook of Applied Cryptography - 1997, Chapter 8
 *     Menezes, van Oorschot and Vanstone
 *
 * [3] Malware Guard Extension: Using SGX to Conceal Cache Attacks
 *     Michael Schwarz, Samuel Weiser, Daniel Gruss, Clémentine Maurice and
 *     Stefan Mangard
 *     https://arxiv.org/abs/1702.08719v2
 */

#if defined(MBEDTLS_RSA_C)

/* Parameter validation macros */
#define RSA_VALIDATE_RET( cond )                                       \
    MBEDTLS_INTERNAL_VALIDATE_RET( cond, MBEDTLS_ERR_RSA_BAD_INPUT_DATA )
#define RSA_VALIDATE( cond )                                           \
    MBEDTLS_INTERNAL_VALIDATE( cond )

int mbedtls_rsa_import( mbedtls_rsa_context *ctx,
                        const mbedtls_mpi *N,
                        const mbedtls_mpi *P, const mbedtls_mpi *Q,
                        const mbedtls_mpi *D, const mbedtls_mpi *E )
{
    int ret = MBEDTLS_ERR_THIS_CORRUPTION;
    RSA_VALIDATE_RET( ctx != NULL );
    if( ( N && ( ret = mbedtls_mpi_copy( &ctx->N, N ) ) != 0 ) ||
        ( P && ( ret = mbedtls_mpi_copy( &ctx->P, P ) ) != 0 ) ||
        ( Q && ( ret = mbedtls_mpi_copy( &ctx->Q, Q ) ) != 0 ) ||
        ( D && ( ret = mbedtls_mpi_copy( &ctx->D, D ) ) != 0 ) ||
        ( E && ( ret = mbedtls_mpi_copy( &ctx->E, E ) ) != 0 ) )
    {
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA + ret );
    }
    if( N )
        ctx->len = mbedtls_mpi_size( &ctx->N );
    return( 0 );
}

int mbedtls_rsa_import_raw( mbedtls_rsa_context *ctx,
                            unsigned char const *N, size_t N_len,
                            unsigned char const *P, size_t P_len,
                            unsigned char const *Q, size_t Q_len,
                            unsigned char const *D, size_t D_len,
                            unsigned char const *E, size_t E_len )
{
    int ret = 0;
    RSA_VALIDATE_RET( ctx != NULL );
    if( N )
    {
        MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( &ctx->N, N, N_len ) );
        ctx->len = mbedtls_mpi_size( &ctx->N );
    }
    if( P ) MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( &ctx->P, P, P_len ) );
    if( Q ) MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( &ctx->Q, Q, Q_len ) );
    if( D ) MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( &ctx->D, D, D_len ) );
    if( E ) MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( &ctx->E, E, E_len ) );
cleanup:
    if( ret )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA + ret );
    return( 0 );
}

/*
 * Checks whether the context fields are set in such a way
 * that the RSA primitives will be able to execute without error.
 * It does *not* make guarantees for consistency of the parameters.
 */
static int rsa_check_context( mbedtls_rsa_context const *ctx, int is_priv,
                              int blinding_needed )
{
#if !defined(MBEDTLS_RSA_NO_CRT)
    /* blinding_needed is only used for NO_CRT to decide whether
     * P,Q need to be present or not. */
    ((void) blinding_needed);
#endif

    if( ctx->len != mbedtls_mpi_size( &ctx->N ) ||
        ctx->len > MBEDTLS_MPI_MAX_SIZE )
    {
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    }

    /*
     * 1. Modular exponentiation needs positive, odd moduli.
     */

    /* Modular exponentiation wrt. N is always used for
     * RSA public key operations. */
    if( mbedtls_mpi_cmp_int( &ctx->N, 0 ) <= 0 ||
        mbedtls_mpi_get_bit( &ctx->N, 0 ) == 0  )
    {
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    }

#if !defined(MBEDTLS_RSA_NO_CRT)
    /* Modular exponentiation for P and Q is only
     * used for private key operations and if CRT
     * is used. */
    if( is_priv &&
        ( mbedtls_mpi_cmp_int( &ctx->P, 0 ) <= 0 ||
          mbedtls_mpi_get_bit( &ctx->P, 0 ) == 0 ||
          mbedtls_mpi_cmp_int( &ctx->Q, 0 ) <= 0 ||
          mbedtls_mpi_get_bit( &ctx->Q, 0 ) == 0  ) )
    {
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    }
#endif /* !MBEDTLS_RSA_NO_CRT */

    /*
     * 2. Exponents must be positive
     */

    /* Always need E for public key operations */
    if( mbedtls_mpi_cmp_int( &ctx->E, 0 ) <= 0 )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

#if defined(MBEDTLS_RSA_NO_CRT)
    /* For private key operations, use D or DP & DQ
     * as (unblinded) exponents. */
    if( is_priv && mbedtls_mpi_cmp_int( &ctx->D, 0 ) <= 0 )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
#else
    if( is_priv &&
        ( mbedtls_mpi_cmp_int( &ctx->DP, 0 ) <= 0 ||
          mbedtls_mpi_cmp_int( &ctx->DQ, 0 ) <= 0  ) )
    {
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    }
#endif /* MBEDTLS_RSA_NO_CRT */

    /* Blinding shouldn't make exponents negative either,
     * so check that P, Q >= 1 if that hasn't yet been
     * done as part of 1. */
#if defined(MBEDTLS_RSA_NO_CRT)
    if( is_priv && blinding_needed &&
        ( mbedtls_mpi_cmp_int( &ctx->P, 0 ) <= 0 ||
          mbedtls_mpi_cmp_int( &ctx->Q, 0 ) <= 0 ) )
    {
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    }
#endif

    /* It wouldn't lead to an error if it wasn't satisfied,
     * but check for QP >= 1 nonetheless. */
#if !defined(MBEDTLS_RSA_NO_CRT)
    if( is_priv &&
        mbedtls_mpi_cmp_int( &ctx->QP, 0 ) <= 0 )
    {
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    }
#endif

    return( 0 );
}

int mbedtls_rsa_complete( mbedtls_rsa_context *ctx )
{
    int ret = 0;
    int have_N, have_P, have_Q, have_D, have_E;
#if !defined(MBEDTLS_RSA_NO_CRT)
    int have_DP, have_DQ, have_QP;
#endif
    int n_missing, pq_missing, d_missing, is_pub, is_priv;
    RSA_VALIDATE_RET( ctx != NULL );
    have_N = ( mbedtls_mpi_cmp_int( &ctx->N, 0 ) != 0 );
    have_P = ( mbedtls_mpi_cmp_int( &ctx->P, 0 ) != 0 );
    have_Q = ( mbedtls_mpi_cmp_int( &ctx->Q, 0 ) != 0 );
    have_D = ( mbedtls_mpi_cmp_int( &ctx->D, 0 ) != 0 );
    have_E = ( mbedtls_mpi_cmp_int( &ctx->E, 0 ) != 0 );
#if !defined(MBEDTLS_RSA_NO_CRT)
    have_DP = ( mbedtls_mpi_cmp_int( &ctx->DP, 0 ) != 0 );
    have_DQ = ( mbedtls_mpi_cmp_int( &ctx->DQ, 0 ) != 0 );
    have_QP = ( mbedtls_mpi_cmp_int( &ctx->QP, 0 ) != 0 );
#endif

    /*
     * Check whether provided parameters are enough
     * to deduce all others. The following incomplete
     * parameter sets for private keys are supported:
     *
     * (1) P, Q missing.
     * (2) D and potentially N missing.
     *
     */

    n_missing  =              have_P &&  have_Q &&  have_D && have_E;
    pq_missing =   have_N && !have_P && !have_Q &&  have_D && have_E;
    d_missing  =              have_P &&  have_Q && !have_D && have_E;
    is_pub     =   have_N && !have_P && !have_Q && !have_D && have_E;

    /* These three alternatives are mutually exclusive */
    is_priv = n_missing || pq_missing || d_missing;

    if( !is_priv && !is_pub )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

    /*
     * Step 1: Deduce N if P, Q are provided.
     */

    if( !have_N && have_P && have_Q )
    {
        if( ( ret = mbedtls_mpi_mul_mpi( &ctx->N, &ctx->P,
                                         &ctx->Q ) ) != 0 )
        {
            return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA + ret );
        }

        ctx->len = mbedtls_mpi_size( &ctx->N );
    }

    /*
     * Step 2: Deduce and verify all remaining core parameters.
     */

    if( pq_missing )
    {
        ret = mbedtls_rsa_deduce_primes( &ctx->N, &ctx->E, &ctx->D,
                                         &ctx->P, &ctx->Q );
        if( ret != 0 )
            return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA + ret );

    }
    else if( d_missing )
    {
        if( ( ret = mbedtls_rsa_deduce_private_exponent( &ctx->P,
                                                         &ctx->Q,
                                                         &ctx->E,
                                                         &ctx->D ) ) != 0 )
        {
            return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA + ret );
        }
    }

    /*
     * Step 3: Deduce all additional parameters specific
     *         to our current RSA implementation.
     */

#if !defined(MBEDTLS_RSA_NO_CRT)
    if( is_priv && ! ( have_DP && have_DQ && have_QP ) )
    {
        ret = mbedtls_rsa_deduce_crt( &ctx->P,  &ctx->Q,  &ctx->D,
                                      &ctx->DP, &ctx->DQ, &ctx->QP );
        if( ret != 0 )
            return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA + ret );
    }
#endif /* MBEDTLS_RSA_NO_CRT */

    /*
     * Step 3: Basic sanity checks
     */

    return( rsa_check_context( ctx, is_priv, 1 ) );
}

int mbedtls_rsa_export_raw( const mbedtls_rsa_context *ctx,
                            unsigned char *N, size_t N_len,
                            unsigned char *P, size_t P_len,
                            unsigned char *Q, size_t Q_len,
                            unsigned char *D, size_t D_len,
                            unsigned char *E, size_t E_len )
{
    int ret = 0;
    int is_priv;
    RSA_VALIDATE_RET( ctx != NULL );
    /* Check if key is private or public */
    is_priv =
        mbedtls_mpi_cmp_int( &ctx->N, 0 ) != 0 &&
        mbedtls_mpi_cmp_int( &ctx->P, 0 ) != 0 &&
        mbedtls_mpi_cmp_int( &ctx->Q, 0 ) != 0 &&
        mbedtls_mpi_cmp_int( &ctx->D, 0 ) != 0 &&
        mbedtls_mpi_cmp_int( &ctx->E, 0 ) != 0;
    if( !is_priv )
    {
        /* If we're trying to export private parameters for a public key,
         * something must be wrong. */
        if( P != NULL || Q != NULL || D != NULL )
            return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    }
    if( N ) MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( &ctx->N, N, N_len ) );
    if( P ) MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( &ctx->P, P, P_len ) );
    if( Q ) MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( &ctx->Q, Q, Q_len ) );
    if( D ) MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( &ctx->D, D, D_len ) );
    if( E ) MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( &ctx->E, E, E_len ) );
cleanup:
    return( ret );
}

int mbedtls_rsa_export( const mbedtls_rsa_context *ctx,
                        mbedtls_mpi *N, mbedtls_mpi *P, mbedtls_mpi *Q,
                        mbedtls_mpi *D, mbedtls_mpi *E )
{
    int ret = MBEDTLS_ERR_THIS_CORRUPTION;
    int is_priv;
    RSA_VALIDATE_RET( ctx != NULL );
    /* Check if key is private or public */
    is_priv =
        mbedtls_mpi_cmp_int( &ctx->N, 0 ) != 0 &&
        mbedtls_mpi_cmp_int( &ctx->P, 0 ) != 0 &&
        mbedtls_mpi_cmp_int( &ctx->Q, 0 ) != 0 &&
        mbedtls_mpi_cmp_int( &ctx->D, 0 ) != 0 &&
        mbedtls_mpi_cmp_int( &ctx->E, 0 ) != 0;
    if( !is_priv )
    {
        /* If we're trying to export private parameters for a public key,
         * something must be wrong. */
        if( P != NULL || Q != NULL || D != NULL )
            return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    }
    /* Export all requested core parameters. */
    if( ( N != NULL && ( ret = mbedtls_mpi_copy( N, &ctx->N ) ) != 0 ) ||
        ( P != NULL && ( ret = mbedtls_mpi_copy( P, &ctx->P ) ) != 0 ) ||
        ( Q != NULL && ( ret = mbedtls_mpi_copy( Q, &ctx->Q ) ) != 0 ) ||
        ( D != NULL && ( ret = mbedtls_mpi_copy( D, &ctx->D ) ) != 0 ) ||
        ( E != NULL && ( ret = mbedtls_mpi_copy( E, &ctx->E ) ) != 0 ) )
    {
        return( ret );
    }
    return( 0 );
}

/*
 * Export CRT parameters
 * This must also be implemented if CRT is not used, for being able to
 * write DER encoded RSA keys. The helper function mbedtls_rsa_deduce_crt
 * can be used in this case.
 */
int mbedtls_rsa_export_crt( const mbedtls_rsa_context *ctx,
                            mbedtls_mpi *DP, mbedtls_mpi *DQ, mbedtls_mpi *QP )
{
    int ret = MBEDTLS_ERR_THIS_CORRUPTION;
    int is_priv;
    RSA_VALIDATE_RET( ctx != NULL );
    /* Check if key is private or public */
    is_priv =
        mbedtls_mpi_cmp_int( &ctx->N, 0 ) != 0 &&
        mbedtls_mpi_cmp_int( &ctx->P, 0 ) != 0 &&
        mbedtls_mpi_cmp_int( &ctx->Q, 0 ) != 0 &&
        mbedtls_mpi_cmp_int( &ctx->D, 0 ) != 0 &&
        mbedtls_mpi_cmp_int( &ctx->E, 0 ) != 0;
    if( !is_priv )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
#if !defined(MBEDTLS_RSA_NO_CRT)
    /* Export all requested blinding parameters. */
    if( ( DP != NULL && ( ret = mbedtls_mpi_copy( DP, &ctx->DP ) ) != 0 ) ||
        ( DQ != NULL && ( ret = mbedtls_mpi_copy( DQ, &ctx->DQ ) ) != 0 ) ||
        ( QP != NULL && ( ret = mbedtls_mpi_copy( QP, &ctx->QP ) ) != 0 ) )
    {
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA + ret );
    }
#else
    if( ( ret = mbedtls_rsa_deduce_crt( &ctx->P, &ctx->Q, &ctx->D,
                                        DP, DQ, QP ) ) != 0 )
    {
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA + ret );
    }
#endif
    return( 0 );
}

/*
 * Initialize an RSA context
 */
void mbedtls_rsa_init( mbedtls_rsa_context *ctx,
                       int padding,
                       int hash_id )
{
    RSA_VALIDATE( ctx != NULL );
    RSA_VALIDATE( padding == MBEDTLS_RSA_PKCS_V15 ||
                  padding == MBEDTLS_RSA_PKCS_V21 );
    mbedtls_platform_zeroize( ctx, sizeof( mbedtls_rsa_context ) );
    mbedtls_rsa_set_padding( ctx, padding, hash_id );
}

/*
 * Set padding for an existing RSA context
 */
void mbedtls_rsa_set_padding( mbedtls_rsa_context *ctx, int padding,
                              int hash_id )
{
    RSA_VALIDATE( ctx != NULL );
    RSA_VALIDATE( padding == MBEDTLS_RSA_PKCS_V15 ||
                  padding == MBEDTLS_RSA_PKCS_V21 );
    ctx->padding = padding;
    ctx->hash_id = hash_id;
}

/*
 * Get length in bytes of RSA modulus
 */

size_t mbedtls_rsa_get_len( const mbedtls_rsa_context *ctx )
{
    return( ctx->len );
}


#if defined(MBEDTLS_GENPRIME)

/*
 * Generate an RSA keypair
 *
 * This generation method follows the RSA key pair generation procedure of
 * FIPS 186-4 if 2^16 < exponent < 2^256 and nbits = 2048 or nbits = 3072.
 */
int mbedtls_rsa_gen_key( mbedtls_rsa_context *ctx,
                         int (*f_rng)(void *, unsigned char *, size_t),
                         void *p_rng,
                         unsigned int nbits, int exponent )
{
    int ret = MBEDTLS_ERR_THIS_CORRUPTION;
    mbedtls_mpi H, G, L;
    int prime_quality = 0;
    RSA_VALIDATE_RET( ctx );
    RSA_VALIDATE_RET( f_rng );

    /*
     * If the modulus is 1024 bit long or shorter, then the security strength of
     * the RSA algorithm is less than or equal to 80 bits and therefore an error
     * rate of 2^-80 is sufficient.
     */
    if( nbits > 1024 )
        prime_quality = MBEDTLS_MPI_GEN_PRIME_FLAG_LOW_ERR;

    mbedtls_mpi_init( &H );
    mbedtls_mpi_init( &G );
    mbedtls_mpi_init( &L );

    if( nbits < 128 || exponent < 3 || nbits % 2 != 0 )
    {
        ret = MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
        goto cleanup;
    }

    /*
     * find primes P and Q with Q < P so that:
     * 1.  |P-Q| > 2^( nbits / 2 - 100 )
     * 2.  GCD( E, (P-1)*(Q-1) ) == 1
     * 3.  E^-1 mod LCM(P-1, Q-1) > 2^( nbits / 2 )
     */
    MBEDTLS_MPI_CHK( mbedtls_mpi_lset( &ctx->E, exponent ) );

    do
    {
        MBEDTLS_MPI_CHK( mbedtls_mpi_gen_prime( &ctx->P, nbits >> 1,
                                                prime_quality, f_rng, p_rng ) );

        MBEDTLS_MPI_CHK( mbedtls_mpi_gen_prime( &ctx->Q, nbits >> 1,
                                                prime_quality, f_rng, p_rng ) );

        /* make sure the difference between p and q is not too small (FIPS 186-4 §B.3.3 step 5.4) */
        MBEDTLS_MPI_CHK( mbedtls_mpi_sub_mpi( &H, &ctx->P, &ctx->Q ) );
        if( mbedtls_mpi_bitlen( &H ) <= ( ( nbits >= 200 ) ? ( ( nbits >> 1 ) - 99 ) : 0 ) )
            continue;

        /* not required by any standards, but some users rely on the fact that P > Q */
        if( H.s < 0 )
            mbedtls_mpi_swap( &ctx->P, &ctx->Q );

        /* Temporarily replace P,Q by P-1, Q-1 */
        MBEDTLS_MPI_CHK( mbedtls_mpi_sub_int( &ctx->P, &ctx->P, 1 ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_sub_int( &ctx->Q, &ctx->Q, 1 ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &H, &ctx->P, &ctx->Q ) );

        /* check GCD( E, (P-1)*(Q-1) ) == 1 (FIPS 186-4 §B.3.1 criterion 2(a)) */
        MBEDTLS_MPI_CHK( mbedtls_mpi_gcd( &G, &ctx->E, &H  ) );
        if( !mbedtls_mpi_is_one( &G ) )
            continue;

        /* compute smallest possible D = E^-1 mod LCM(P-1, Q-1) (FIPS 186-4 §B.3.1 criterion 3(b)) */
        MBEDTLS_MPI_CHK( mbedtls_mpi_gcd( &G, &ctx->P, &ctx->Q ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_div_mpi( &L, NULL, &H, &G ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_inv_mod( &ctx->D, &ctx->E, &L ) );

        if( mbedtls_mpi_bitlen( &ctx->D ) <= ( ( nbits + 1 ) / 2 ) ) // (FIPS 186-4 §B.3.1 criterion 3(a))
            continue;

        break;
    }
    while( 1 );

    /* Restore P,Q */
    MBEDTLS_MPI_CHK( mbedtls_mpi_add_int( &ctx->P,  &ctx->P, 1 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_add_int( &ctx->Q,  &ctx->Q, 1 ) );

    MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &ctx->N, &ctx->P, &ctx->Q ) );

    ctx->len = mbedtls_mpi_size( &ctx->N );

#if !defined(MBEDTLS_RSA_NO_CRT)
    /*
     * DP = D mod (P - 1)
     * DQ = D mod (Q - 1)
     * QP = Q^-1 mod P
     */
    MBEDTLS_MPI_CHK( mbedtls_rsa_deduce_crt( &ctx->P, &ctx->Q, &ctx->D,
                                             &ctx->DP, &ctx->DQ, &ctx->QP ) );
#endif /* MBEDTLS_RSA_NO_CRT */

    /* Double-check */
    MBEDTLS_MPI_CHK( mbedtls_rsa_check_privkey( ctx ) );

cleanup:
    mbedtls_mpi_free( &H );
    mbedtls_mpi_free( &G );
    mbedtls_mpi_free( &L );
    if( ret != 0 )
    {
        mbedtls_rsa_free( ctx );
        if( ( -ret & ~0x7f ) == 0 )
            ret = MBEDTLS_ERR_RSA_KEY_GEN_FAILED + ret;
    }
    STRACE("%s() → %d", "mbedtls_rsa_gen_key", ret);
    return( ret );
}

#endif /* MBEDTLS_GENPRIME */

/*
 * Check a public RSA key
 */
int mbedtls_rsa_check_pubkey( const mbedtls_rsa_context *ctx )
{
    RSA_VALIDATE_RET( ctx );
    if( rsa_check_context( ctx, 0 /* public */, 0 /* no blinding */ ) != 0 )
        return( MBEDTLS_ERR_RSA_KEY_CHECK_FAILED );
    if( mbedtls_mpi_bitlen( &ctx->N ) < 128 )
    {
        return( MBEDTLS_ERR_RSA_KEY_CHECK_FAILED );
    }
    if( mbedtls_mpi_get_bit( &ctx->E, 0 ) == 0 ||
        mbedtls_mpi_bitlen(  &ctx->E )     < 2  ||
        mbedtls_mpi_cmp_mpi( &ctx->E, &ctx->N ) >= 0 )
    {
        return( MBEDTLS_ERR_RSA_KEY_CHECK_FAILED );
    }
    return( 0 );
}

/*
 * Check for the consistency of all fields in an RSA private key context
 */
int mbedtls_rsa_check_privkey( const mbedtls_rsa_context *ctx )
{
    RSA_VALIDATE_RET( ctx );
    if( mbedtls_rsa_check_pubkey( ctx ) != 0 ||
        rsa_check_context( ctx, 1 /* private */, 1 /* blinding */ ) != 0 )
    {
        return( MBEDTLS_ERR_RSA_KEY_CHECK_FAILED );
    }
    if( mbedtls_rsa_validate_params( &ctx->N, &ctx->P, &ctx->Q,
                                     &ctx->D, &ctx->E, NULL, NULL ) != 0 )
    {
        return( MBEDTLS_ERR_RSA_KEY_CHECK_FAILED );
    }
#if !defined(MBEDTLS_RSA_NO_CRT)
    else if( mbedtls_rsa_validate_crt( &ctx->P, &ctx->Q, &ctx->D,
                                       &ctx->DP, &ctx->DQ, &ctx->QP ) != 0 )
    {
        return( MBEDTLS_ERR_RSA_KEY_CHECK_FAILED );
    }
#endif
    return( 0 );
}

/*
 * Check if contexts holding a public and private key match
 */
int mbedtls_rsa_check_pub_priv( const mbedtls_rsa_context *pub,
                                const mbedtls_rsa_context *prv )
{
    RSA_VALIDATE_RET( pub );
    RSA_VALIDATE_RET( prv );
    if( mbedtls_rsa_check_pubkey( pub )  != 0 ||
        mbedtls_rsa_check_privkey( prv ) != 0 )
    {
        return( MBEDTLS_ERR_RSA_KEY_CHECK_FAILED );
    }
    if( mbedtls_mpi_cmp_mpi( &pub->N, &prv->N ) != 0 ||
        mbedtls_mpi_cmp_mpi( &pub->E, &prv->E ) != 0 )
    {
        return( MBEDTLS_ERR_RSA_KEY_CHECK_FAILED );
    }
    return( 0 );
}

/*
 * Do an RSA public key operation
 */
int mbedtls_rsa_public( mbedtls_rsa_context *ctx,
                        const unsigned char *input,
                        unsigned char *output )
{
    int ret = MBEDTLS_ERR_THIS_CORRUPTION;
    size_t olen;
    mbedtls_mpi T;
    RSA_VALIDATE_RET( ctx );
    RSA_VALIDATE_RET( input );
    RSA_VALIDATE_RET( output );
    if( rsa_check_context( ctx, 0 /* public */, 0 /* no blinding */ ) )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    mbedtls_mpi_init( &T );
    MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( &T, input, ctx->len ) );
    if( mbedtls_mpi_cmp_mpi( &T, &ctx->N ) >= 0 )
    {
        ret = MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
        goto cleanup;
    }
    olen = ctx->len;
    MBEDTLS_MPI_CHK( mbedtls_mpi_exp_mod( &T, &T, &ctx->E, &ctx->N, &ctx->RN ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( &T, output, olen ) );
cleanup:
    mbedtls_mpi_free( &T );
    if( ret != 0 )
        return( MBEDTLS_ERR_RSA_PUBLIC_FAILED + ret );
    return( 0 );
}

/*
 * TODO(jart): Why is MbedTLS release source so different from Git source?
 *             This function takes 806us to execute.
 */
/*
 * Generate or update blinding values, see section 10 of:
 *  KOCHER, Paul C. Timing attacks on implementations of Diffie-Hellman, RSA,
 *  DSS, and other systems. In : Advances in Cryptology-CRYPTO'96. Springer
 *  Berlin Heidelberg, 1996. p. 104-113.
 */
static int rsa_prepare_blinding( mbedtls_rsa_context *ctx,
                                 int (*f_rng)(void *, unsigned char *, size_t),
                                 void *p_rng )
{
    int ret, count = 0;
    mbedtls_mpi R;
    mbedtls_mpi_init( &R );
    if( ctx->Vf.p )
    {
        /* We already have blinding values, just update them by squaring */
        MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &ctx->Vi, &ctx->Vi, &ctx->Vi ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mod_mpi( &ctx->Vi, &ctx->Vi, &ctx->N ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &ctx->Vf, &ctx->Vf, &ctx->Vf ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mod_mpi( &ctx->Vf, &ctx->Vf, &ctx->N ) );
        goto cleanup;
    }
    /* Unblinding value: Vf = random number, invertible mod N */
    do {
        if( count++ > 10 )
        {
            ret = MBEDTLS_ERR_RSA_RNG_FAILED;
            goto cleanup;
        }
        MBEDTLS_MPI_CHK( mbedtls_mpi_fill_random( &ctx->Vf, ctx->len - 1, f_rng, p_rng ) );
        /* Compute Vf^-1 as R * (R Vf)^-1 to avoid leaks from inv_mod. */
        MBEDTLS_MPI_CHK( mbedtls_mpi_fill_random( &R, ctx->len - 1, f_rng, p_rng ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &ctx->Vi, &ctx->Vf, &R ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mod_mpi( &ctx->Vi, &ctx->Vi, &ctx->N ) );
        /* At this point, Vi is invertible mod N if and only if both Vf and R
         * are invertible mod N. If one of them isn't, we don't need to know
         * which one, we just loop and choose new values for both of them.
         * (Each iteration succeeds with overwhelming probability.) */
        ret = mbedtls_mpi_inv_mod( &ctx->Vi, &ctx->Vi, &ctx->N );
        if( ret != 0 && ret != MBEDTLS_ERR_MPI_NOT_ACCEPTABLE )
            goto cleanup;
    } while( ret == MBEDTLS_ERR_MPI_NOT_ACCEPTABLE );
    /* Finish the computation of Vf^-1 = R * (R Vf)^-1 */
    MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &ctx->Vi, &ctx->Vi, &R ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_mod_mpi( &ctx->Vi, &ctx->Vi, &ctx->N ) );
    /* Blinding value: Vi = Vf^(-e) mod N
     * (Vi already contains Vf^-1 at this point) */
    MBEDTLS_MPI_CHK( mbedtls_mpi_exp_mod( &ctx->Vi, &ctx->Vi, &ctx->E, &ctx->N, &ctx->RN ) );
cleanup:
    mbedtls_mpi_free( &R );
    return( ret );
}

/*
 * Exponent blinding supposed to prevent side-channel attacks using multiple
 * traces of measurements to recover the RSA key. The more collisions are there,
 * the more bits of the key can be recovered. See [3].
 *
 * Collecting n collisions with m bit long blinding value requires 2^(m-m/n)
 * observations on avarage.
 *
 * For example with 28 byte blinding to achieve 2 collisions the adversary has
 * to make 2^112 observations on avarage.
 *
 * (With the currently (as of 2017 April) known best algorithms breaking 2048
 * bit RSA requires approximately as much time as trying out 2^112 random keys.
 * Thus in this sense with 28 byte blinding the security is not reduced by
 * side-channel attacks like the one in [3])
 *
 * This countermeasure does not help if the key recovery is possible with a
 * single trace.
 */
#define RSA_EXPONENT_BLINDING 28

/*
 * Do an RSA private key operation
 */
int mbedtls_rsa_private( mbedtls_rsa_context *ctx,
                         int (*f_rng)(void *, unsigned char *, size_t),
                         void *p_rng,
                         const unsigned char *input,
                         unsigned char *output )
{
    RSA_VALIDATE_RET( ctx );
    RSA_VALIDATE_RET( input  );
    RSA_VALIDATE_RET( output );

    int ret = MBEDTLS_ERR_THIS_CORRUPTION;
    size_t olen;

    /* Temporary holding the result */
    mbedtls_mpi T;

    /* Temporaries holding P-1, Q-1 and the
     * exponent blinding factor, respectively. */
    mbedtls_mpi P1, Q1, R;

#if !defined(MBEDTLS_RSA_NO_CRT)
    /* Temporaries holding the results mod p resp. mod q. */
    mbedtls_mpi TP, TQ;

    /* Temporaries holding the blinded exponents for
     * the mod p resp. mod q computation (if used). */
    mbedtls_mpi DP_blind, DQ_blind;

    /* Pointers to actual exponents to be used - either the unblinded
     * or the blinded ones, depending on the presence of a PRNG. */
    mbedtls_mpi *DP = &ctx->DP;
    mbedtls_mpi *DQ = &ctx->DQ;
#else
    /* Temporary holding the blinded exponent (if used). */
    mbedtls_mpi D_blind;

    /* Pointer to actual exponent to be used - either the unblinded
     * or the blinded one, depending on the presence of a PRNG. */
    mbedtls_mpi *D = &ctx->D;
#endif /* MBEDTLS_RSA_NO_CRT */

    /* Temporaries holding the initial input and the double
     * checked result; should be the same in the end. */
    mbedtls_mpi I, C;

    if( rsa_check_context( ctx, 1             /* private key checks */,
                                f_rng != NULL /* blinding y/n       */ ) != 0 )
    {
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    }

    /* MPI Initialization */
    mbedtls_mpi_init( &T );

    mbedtls_mpi_init( &P1 );
    mbedtls_mpi_init( &Q1 );
    mbedtls_mpi_init( &R );

    if( f_rng )
    {
#if defined(MBEDTLS_RSA_NO_CRT)
        mbedtls_mpi_init( &D_blind );
#else
        mbedtls_mpi_init( &DP_blind );
        mbedtls_mpi_init( &DQ_blind );
#endif
    }

#if !defined(MBEDTLS_RSA_NO_CRT)
    mbedtls_mpi_init( &TP ); mbedtls_mpi_init( &TQ );
#endif

    mbedtls_mpi_init( &I );
    mbedtls_mpi_init( &C );

    /* End of MPI initialization */

    MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( &T, input, ctx->len ) );
    if( mbedtls_mpi_cmp_mpi( &T, &ctx->N ) >= 0 )
    {
        ret = MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
        goto cleanup;
    }

    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &I, &T ) );

    if( f_rng )
    {

        /*
         * Blinding
         * T = T * Vi mod N
         */
        MBEDTLS_MPI_CHK( rsa_prepare_blinding( ctx, f_rng, p_rng ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &T, &T, &ctx->Vi ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mod_mpi( &T, &T, &ctx->N ) );

        /*
         * Exponent blinding
         */
        MBEDTLS_MPI_CHK( mbedtls_mpi_sub_int( &P1, &ctx->P, 1 ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_sub_int( &Q1, &ctx->Q, 1 ) );

#if defined(MBEDTLS_RSA_NO_CRT)
        /*
         * D_blind = ( P - 1 ) * ( Q - 1 ) * R + D
         */
        MBEDTLS_MPI_CHK( mbedtls_mpi_fill_random( &R, RSA_EXPONENT_BLINDING,
                         f_rng, p_rng ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &D_blind, &P1, &Q1 ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &D_blind, &D_blind, &R ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_add_mpi( &D_blind, &D_blind, &ctx->D ) );

        D = &D_blind;

#else
        /*
         * DP_blind = ( P - 1 ) * R + DP
         */
        MBEDTLS_MPI_CHK( mbedtls_mpi_fill_random( &R, RSA_EXPONENT_BLINDING,
                         f_rng, p_rng ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &DP_blind, &P1, &R ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_add_mpi( &DP_blind, &DP_blind,
                    &ctx->DP ) );
        DP = &DP_blind;

        /*
         * DQ_blind = ( Q - 1 ) * R + DQ
         */
        MBEDTLS_MPI_CHK( mbedtls_mpi_fill_random( &R, RSA_EXPONENT_BLINDING,
                         f_rng, p_rng ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &DQ_blind, &Q1, &R ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_add_mpi( &DQ_blind, &DQ_blind,
                    &ctx->DQ ) );

        DQ = &DQ_blind;

#endif /* MBEDTLS_RSA_NO_CRT */
    }

#if defined(MBEDTLS_RSA_NO_CRT)
    MBEDTLS_MPI_CHK( mbedtls_mpi_exp_mod( &T, &T, D, &ctx->N, &ctx->RN ) );
#else

    /*
     * Faster decryption using the CRT
     *
     * TP = input ^ dP mod P
     * TQ = input ^ dQ mod Q
     */
    MBEDTLS_MPI_CHK( mbedtls_mpi_exp_mod( &TP, &T, DP, &ctx->P, &ctx->RP ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_exp_mod( &TQ, &T, DQ, &ctx->Q, &ctx->RQ ) );

    /*
     * T = (TP - TQ) * (Q^-1 mod P) mod P
     */
    MBEDTLS_MPI_CHK( mbedtls_mpi_sub_mpi( &T, &TP, &TQ ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &TP, &T, &ctx->QP ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_mod_mpi( &T, &TP, &ctx->P ) );

    /*
     * T = TQ + T * Q
     */
    MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &TP, &T, &ctx->Q ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_add_mpi( &T, &TQ, &TP ) );
#endif /* MBEDTLS_RSA_NO_CRT */

    if( f_rng )
    {
        /*
         * Unblind
         * T = T * Vf mod N
         */
        MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &T, &T, &ctx->Vf ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mod_mpi( &T, &T, &ctx->N ) );
    }

    /* Verify the result to prevent glitching attacks. */
    MBEDTLS_MPI_CHK( mbedtls_mpi_exp_mod( &C, &T, &ctx->E,
                                          &ctx->N, &ctx->RN ) );
    if( mbedtls_mpi_cmp_mpi( &C, &I ) != 0 )
    {
        ret = MBEDTLS_ERR_RSA_VERIFY_FAILED;
        goto cleanup;
    }

    olen = ctx->len;
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( &T, output, olen ) );

cleanup:

    mbedtls_mpi_free( &P1 );
    mbedtls_mpi_free( &Q1 );
    mbedtls_mpi_free( &R );

    if( f_rng )
    {
#if defined(MBEDTLS_RSA_NO_CRT)
        mbedtls_mpi_free( &D_blind );
#else
        mbedtls_mpi_free( &DP_blind );
        mbedtls_mpi_free( &DQ_blind );
#endif
    }

    mbedtls_mpi_free( &T );

#if !defined(MBEDTLS_RSA_NO_CRT)
    mbedtls_mpi_free( &TP ); mbedtls_mpi_free( &TQ );
#endif

    mbedtls_mpi_free( &C );
    mbedtls_mpi_free( &I );

    if( ret != 0 && ret >= -0x007f )
        return( MBEDTLS_ERR_RSA_PRIVATE_FAILED + ret );

    return( ret );
}

/**
 * Generate and apply the MGF1 operation (from PKCS#1 v2.1) to a buffer.
 *
 * \param dst       buffer to mask
 * \param dlen      length of destination buffer
 * \param src       source of the mask generation
 * \param slen      length of the source buffer
 * \param md_ctx    message digest context to use
 */
static int mgf_mask( unsigned char *dst, size_t dlen, unsigned char *src,
                      size_t slen, mbedtls_md_context_t *md_ctx )
{
    unsigned char mask[MBEDTLS_MD_MAX_SIZE];
    unsigned char counter[4];
    unsigned char *p;
    unsigned int hlen;
    size_t i, use_len;
    int ret = 0;
    mbedtls_platform_zeroize( mask, MBEDTLS_MD_MAX_SIZE );
    memset( counter, 0, 4 );
    hlen = mbedtls_md_get_size( md_ctx->md_info );
    /* Generate and apply dbMask */
    p = dst;
    while( dlen > 0 )
    {
        use_len = hlen;
        if( dlen < hlen )
            use_len = dlen;
        if( ( ret = mbedtls_md_starts( md_ctx ) ) != 0 )
            goto exit;
        if( ( ret = mbedtls_md_update( md_ctx, src, slen ) ) != 0 )
            goto exit;
        if( ( ret = mbedtls_md_update( md_ctx, counter, 4 ) ) != 0 )
            goto exit;
        if( ( ret = mbedtls_md_finish( md_ctx, mask ) ) != 0 )
            goto exit;
        for( i = 0; i < use_len; ++i )
            *p++ ^= mask[i];
        counter[3]++;
        dlen -= use_len;
    }
exit:
    mbedtls_platform_zeroize( mask, sizeof( mask ) );
    return( ret );
}

/*
 * Implementation of the PKCS#1 v2.1 RSAES-OAEP-ENCRYPT function
 */
int mbedtls_rsa_rsaes_oaep_encrypt( mbedtls_rsa_context *ctx,
                                    int (*f_rng)(void *, unsigned char *, size_t),
                                    void *p_rng, int mode,
                                    const unsigned char *label, size_t label_len,
                                    size_t ilen, const unsigned char *input,
                                    unsigned char *output )
{
    size_t olen;
    int ret = MBEDTLS_ERR_THIS_CORRUPTION;
    unsigned char *p = output;
    unsigned int hlen;
    const mbedtls_md_info_t *md_info;
    mbedtls_md_context_t md_ctx;
    RSA_VALIDATE_RET( ctx != NULL );
    RSA_VALIDATE_RET( mode == MBEDTLS_RSA_PRIVATE ||
                      mode == MBEDTLS_RSA_PUBLIC );
    RSA_VALIDATE_RET( output != NULL );
    RSA_VALIDATE_RET( ilen == 0 || input != NULL );
    RSA_VALIDATE_RET( label_len == 0 || label != NULL );
    if( mode == MBEDTLS_RSA_PRIVATE && ctx->padding != MBEDTLS_RSA_PKCS_V21 )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    if( !f_rng )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    md_info = mbedtls_md_info_from_type( (mbedtls_md_type_t) ctx->hash_id );
    if( !md_info )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    olen = ctx->len;
    hlen = mbedtls_md_get_size( md_info );
    /* first comparison checks for overflow */
    if( ilen + 2 * hlen + 2 < ilen || olen < ilen + 2 * hlen + 2 )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    mbedtls_platform_zeroize( output, olen );
    *p++ = 0;
    /* Generate a random octet string seed */
    if( ( ret = f_rng( p_rng, p, hlen ) ) != 0 )
        return( MBEDTLS_ERR_RSA_RNG_FAILED + ret );
    p += hlen;
    /* Construct DB */
    if( ( ret = mbedtls_md( md_info, label, label_len, p ) ) != 0 )
        return( ret );
    p += hlen;
    p += olen - 2 * hlen - 2 - ilen;
    *p++ = 1;
    if( ilen != 0 )
        memcpy( p, input, ilen );
    mbedtls_md_init( &md_ctx );
    if( ( ret = mbedtls_md_setup( &md_ctx, md_info, 0 ) ) != 0 )
        goto exit;
    /* maskedDB: Apply dbMask to DB */
    if( ( ret = mgf_mask( output + hlen + 1, olen - hlen - 1, output + 1, hlen,
                          &md_ctx ) ) != 0 )
        goto exit;
    /* maskedSeed: Apply seedMask to seed */
    if( ( ret = mgf_mask( output + 1, hlen, output + hlen + 1, olen - hlen - 1,
                          &md_ctx ) ) != 0 )
        goto exit;
exit:
    mbedtls_md_free( &md_ctx );
    if( ret != 0 )
        return( ret );
    return( ( mode == MBEDTLS_RSA_PUBLIC )
            ? mbedtls_rsa_public(  ctx, output, output )
            : mbedtls_rsa_private( ctx, f_rng, p_rng, output, output ) );
}

#if defined(MBEDTLS_PKCS1_V15)
/*
 * Implementation of the PKCS#1 v2.1 RSAES-PKCS1-V1_5-ENCRYPT function
 */
int mbedtls_rsa_rsaes_pkcs1_v15_encrypt( mbedtls_rsa_context *ctx,
                                         int (*f_rng)(void *, unsigned char *, size_t),
                                         void *p_rng,
                                         int mode, size_t ilen,
                                         const unsigned char *input,
                                         unsigned char *output )
{
    size_t nb_pad, olen;
    int ret = MBEDTLS_ERR_THIS_CORRUPTION;
    unsigned char *p = output;
    RSA_VALIDATE_RET( ctx != NULL );
    RSA_VALIDATE_RET( mode == MBEDTLS_RSA_PRIVATE ||
                      mode == MBEDTLS_RSA_PUBLIC );
    RSA_VALIDATE_RET( output != NULL );
    RSA_VALIDATE_RET( ilen == 0 || input != NULL );
    if( mode == MBEDTLS_RSA_PRIVATE && ctx->padding != MBEDTLS_RSA_PKCS_V15 )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    olen = ctx->len;
    /* first comparison checks for overflow */
    if( ilen + 11 < ilen || olen < ilen + 11 )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    nb_pad = olen - 3 - ilen;
    *p++ = 0;
    if( mode == MBEDTLS_RSA_PUBLIC )
    {
        if( !f_rng )
            return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
        *p++ = MBEDTLS_RSA_CRYPT;
        while( nb_pad-- > 0 )
        {
            int rng_dl = 100;
            do {
                ret = f_rng( p_rng, p, 1 );
            } while( *p == 0 && --rng_dl && ret == 0 );
            /* Check if RNG failed to generate data */
            if( rng_dl == 0 || ret != 0 )
                return( MBEDTLS_ERR_RSA_RNG_FAILED + ret );
            p++;
        }
    }
    else
    {
        *p++ = MBEDTLS_RSA_SIGN;
        while( nb_pad-- > 0 )
            *p++ = 0xFF;
    }
    *p++ = 0;
    if( ilen != 0 )
        memcpy( p, input, ilen );
    return( ( mode == MBEDTLS_RSA_PUBLIC )
            ? mbedtls_rsa_public(  ctx, output, output )
            : mbedtls_rsa_private( ctx, f_rng, p_rng, output, output ) );
}
#endif /* MBEDTLS_PKCS1_V15 */

/*
 * Add the message padding, then do an RSA operation
 */
int mbedtls_rsa_pkcs1_encrypt( mbedtls_rsa_context *ctx,
                               int (*f_rng)(void *, unsigned char *, size_t),
                               void *p_rng, int mode, size_t ilen,
                               const unsigned char *input,
                               unsigned char *output )
{
    RSA_VALIDATE_RET( ctx != NULL );
    RSA_VALIDATE_RET( mode == MBEDTLS_RSA_PRIVATE ||
                      mode == MBEDTLS_RSA_PUBLIC );
    RSA_VALIDATE_RET( output != NULL );
    RSA_VALIDATE_RET( ilen == 0 || input != NULL );
    switch( ctx->padding )
    {
#if defined(MBEDTLS_PKCS1_V15)
        case MBEDTLS_RSA_PKCS_V15:
            return mbedtls_rsa_rsaes_pkcs1_v15_encrypt( ctx, f_rng, p_rng, mode, ilen,
                                                input, output );
#endif
#if defined(MBEDTLS_PKCS1_V21)
        case MBEDTLS_RSA_PKCS_V21:
            return mbedtls_rsa_rsaes_oaep_encrypt( ctx, f_rng, p_rng, mode, NULL, 0,
                                           ilen, input, output );
#endif
        default:
            return( MBEDTLS_ERR_RSA_INVALID_PADDING );
    }
}

/*
 * Implementation of the PKCS#1 v2.1 RSAES-OAEP-DECRYPT function
 */
int mbedtls_rsa_rsaes_oaep_decrypt( mbedtls_rsa_context *ctx,
                                    int (*f_rng)(void *, unsigned char *, size_t),
                                    void *p_rng, int mode,
                                    const unsigned char *label,
                                    size_t label_len, size_t *olen,
                                    const unsigned char *input,
                                    unsigned char *output,
                                    size_t output_max_len )
{
    int ret = MBEDTLS_ERR_THIS_CORRUPTION;
    size_t ilen, i, pad_len;
    unsigned char *p, bad, pad_done;
    unsigned char buf[MBEDTLS_MPI_MAX_SIZE];
    unsigned char lhash[MBEDTLS_MD_MAX_SIZE];
    unsigned int hlen;
    const mbedtls_md_info_t *md_info;
    mbedtls_md_context_t md_ctx;
    RSA_VALIDATE_RET( ctx != NULL );
    RSA_VALIDATE_RET( mode == MBEDTLS_RSA_PRIVATE ||
                      mode == MBEDTLS_RSA_PUBLIC );
    RSA_VALIDATE_RET( output_max_len == 0 || output != NULL );
    RSA_VALIDATE_RET( label_len == 0 || label != NULL );
    RSA_VALIDATE_RET( input != NULL );
    RSA_VALIDATE_RET( olen != NULL );
    /*
     * Parameters sanity checks
     */
    if( mode == MBEDTLS_RSA_PRIVATE && ctx->padding != MBEDTLS_RSA_PKCS_V21 )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    ilen = ctx->len;
    if( ilen < 16 || ilen > sizeof( buf ) )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    md_info = mbedtls_md_info_from_type( (mbedtls_md_type_t) ctx->hash_id );
    if( !md_info )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    hlen = mbedtls_md_get_size( md_info );
    // checking for integer underflow
    if( 2 * hlen + 2 > ilen )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    /*
     * RSA operation
     */
    ret = ( mode == MBEDTLS_RSA_PUBLIC )
          ? mbedtls_rsa_public(  ctx, input, buf )
          : mbedtls_rsa_private( ctx, f_rng, p_rng, input, buf );
    if( ret )
        goto cleanup;
    /*
     * Unmask data and generate lHash
     */
    mbedtls_md_init( &md_ctx );
    if( ( ret = mbedtls_md_setup( &md_ctx, md_info, 0 ) ) != 0 )
    {
        mbedtls_md_free( &md_ctx );
        goto cleanup;
    }
    /* seed: Apply seedMask to maskedSeed */
    if( ( ret = mgf_mask( buf + 1, hlen, buf + hlen + 1, ilen - hlen - 1,
                          &md_ctx ) ) != 0 ||
    /* DB: Apply dbMask to maskedDB */
        ( ret = mgf_mask( buf + hlen + 1, ilen - hlen - 1, buf + 1, hlen,
                          &md_ctx ) ) != 0 )
    {
        mbedtls_md_free( &md_ctx );
        goto cleanup;
    }
    mbedtls_md_free( &md_ctx );
    /* Generate lHash */
    if( ( ret = mbedtls_md( md_info, label, label_len, lhash ) ) != 0 )
        goto cleanup;
    /*
     * Check contents, in "constant-time"
     */
    p = buf;
    bad = 0;
    bad |= *p++; /* First byte must be 0 */
    p += hlen; /* Skip seed */
    /* Check lHash */
    for( i = 0; i < hlen; i++ )
        bad |= lhash[i] ^ *p++;
    /* Get zero-padding len, but always read till end of buffer
     * (minus one, for the 01 byte) */
    pad_len = 0;
    pad_done = 0;
    for( i = 0; i < ilen - 2 * hlen - 2; i++ )
    {
        pad_done |= p[i];
        pad_len += ((pad_done | (unsigned char)-pad_done) >> 7) ^ 1;
    }
    p += pad_len;
    bad |= *p++ ^ 0x01;
    /*
     * The only information "leaked" is whether the padding was correct or not
     * (eg, no data is copied if it was not correct). This meets the
     * recommendations in PKCS#1 v2.2: an opponent cannot distinguish between
     * the different error conditions.
     */
    if( bad )
    {
        ret = MBEDTLS_ERR_RSA_INVALID_PADDING;
        goto cleanup;
    }
    if( ilen - ( p - buf ) > output_max_len )
    {
        ret = MBEDTLS_ERR_RSA_OUTPUT_TOO_LARGE;
        goto cleanup;
    }
    *olen = ilen - (p - buf);
    memcpy( output, p, *olen );
    ret = 0;
cleanup:
    mbedtls_platform_zeroize( buf, sizeof( buf ) );
    mbedtls_platform_zeroize( lhash, sizeof( lhash ) );
    return( ret );
}

#if defined(MBEDTLS_PKCS1_V15)
/**
 * Does -!!value without branches.
 *
 * \param value     The value to analyze.
 * \return          Zero if \p value is zero, otherwise all-bits-one.
 */
forceinline unsigned all_or_nothing_int( unsigned value )
{
    /* MSVC has a warning about unary minus on unsigned, but this is
     * well-defined and precisely what we want to do here */
#if defined(_MSC_VER)
#pragma warning( push )
#pragma warning( disable : 4146 )
#endif
    return( - ( ( value | - value ) >> ( sizeof( value ) * 8 - 1 ) ) );
#if defined(_MSC_VER)
#pragma warning( pop )
#endif
}

/**
 *  Check whether a size is out of bounds, without branches.
 *
 * This is equivalent to `size > max`, but is likely to be compiled to
 * to code using bitwise operation rather than a branch.
 *
 * \param size      Size to check.
 * \param max       Maximum desired value for \p size.
 * \return          \c 0 if `size <= max`.
 * \return          \c 1 if `size > max`.
 */
forceinline unsigned size_greater_than( size_t size, size_t max )
{
    /* Return the sign bit (1 for negative) of (max - size). */
    return( ( max - size ) >> ( sizeof( size_t ) * 8 - 1 ) );
}

/**
 * Choose between two integer values, without branches.
 *
 * This is equivalent to `cond ? if1 : if0`, but is likely to be compiled
 * to code using bitwise operation rather than a branch.
 *
 * \param cond      Condition to test.
 * \param if1       Value to use if \p cond is nonzero.
 * \param if0       Value to use if \p cond is zero.
 * \return          \c if1 if \p cond is nonzero, otherwise \c if0.
 */
forceinline unsigned if_int( unsigned cond, unsigned if1, unsigned if0 )
{
    unsigned mask = all_or_nothing_int( cond );
    return( ( __conceal( "r",  mask ) & if1 ) |
            ( __conceal( "r", ~mask ) & if0 ) );
}

/**
 * Shift some data towards the left inside a buffer without leaking
 * the length of the data through side channels.
 *
 *     mem_move_to_left(start, total, offset);
 *
 * is functionally equivalent to
 *
 *     memmove(start, start + offset, total - offset);
 *     memset(start + offset, 0, total - offset);
 *
 * but it strives to use a memory access pattern (and thus total timing)
 * that does not depend on \p offset. This timing independence comes at
 * the expense of performance.
 *
 * \param start     Pointer to the start of the buffer.
 * \param total     Total size of the buffer.
 * \param offset    Offset from which to copy \p total - \p offset bytes.
 */
static void mem_move_to_left( void *start,
                              size_t total,
                              size_t offset )
{
    volatile unsigned char *buf = start;
    size_t i, n;
    if( total == 0 )
        return;
    for( i = 0; i < total; i++ )
    {
        unsigned no_op = size_greater_than( total - offset, i );
        /* The first `total - offset` passes are a no-op. The last
         * `offset` passes shift the data one byte to the left and
         * zero out the last byte. */
        for( n = 0; n < total - 1; n++ )
        {
            unsigned char current = buf[n];
            unsigned char next = buf[n+1];
            buf[n] = if_int( no_op, current, next );
        }
        buf[total-1] = if_int( no_op, buf[total-1], 0 );
    }
}

/*
 * Implementation of the PKCS#1 v2.1 RSAES-PKCS1-V1_5-DECRYPT function
 */
int mbedtls_rsa_rsaes_pkcs1_v15_decrypt( mbedtls_rsa_context *ctx,
                                 int (*f_rng)(void *, unsigned char *, size_t),
                                 void *p_rng,
                                 int mode, size_t *olen,
                                 const unsigned char *input,
                                 unsigned char *output,
                                 size_t output_max_len )
{
    int ret = MBEDTLS_ERR_THIS_CORRUPTION;
    size_t ilen, i, plaintext_max_size;
    unsigned char buf[MBEDTLS_MPI_MAX_SIZE];
    /* The following variables take sensitive values: their value must
     * not leak into the observable behavior of the function other than
     * the designated outputs (output, olen, return value). Otherwise
     * this would open the execution of the function to
     * side-channel-based variants of the Bleichenbacher padding oracle
     * attack. Potential side channels include overall timing, memory
     * access patterns (especially visible to an adversary who has access
     * to a shared memory cache), and branches (especially visible to
     * an adversary who has access to a shared code cache or to a shared
     * branch predictor). */
    size_t pad_count = 0;
    unsigned bad = 0;
    unsigned char pad_done = 0;
    size_t plaintext_size = 0;
    unsigned output_too_large;

    RSA_VALIDATE_RET( ctx != NULL );
    RSA_VALIDATE_RET( mode == MBEDTLS_RSA_PRIVATE ||
                      mode == MBEDTLS_RSA_PUBLIC );
    RSA_VALIDATE_RET( output_max_len == 0 || output != NULL );
    RSA_VALIDATE_RET( input != NULL );
    RSA_VALIDATE_RET( olen != NULL );

    ilen = ctx->len;
    plaintext_max_size = ( output_max_len > ilen - 11 ?
                           ilen - 11 :
                           output_max_len );

    if( mode == MBEDTLS_RSA_PRIVATE && ctx->padding != MBEDTLS_RSA_PKCS_V15 )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

    if( ilen < 16 || ilen > sizeof( buf ) )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

    ret = ( mode == MBEDTLS_RSA_PUBLIC )
          ? mbedtls_rsa_public(  ctx, input, buf )
          : mbedtls_rsa_private( ctx, f_rng, p_rng, input, buf );

    if( ret != 0 )
        goto cleanup;

    /* Check and get padding length in constant time and constant
     * memory trace. The first byte must be 0. */
    bad |= buf[0];

    if( mode == MBEDTLS_RSA_PRIVATE )
    {
        /* Decode EME-PKCS1-v1_5 padding: 0x00 || 0x02 || PS || 0x00
         * where PS must be at least 8 nonzero bytes. */
        bad |= buf[1] ^ MBEDTLS_RSA_CRYPT;

        /* Read the whole buffer. Set pad_done to nonzero if we find
         * the 0x00 byte and remember the padding length in pad_count. */
        for( i = 2; i < ilen; i++ )
        {
            pad_done  |= ((buf[i] | (unsigned char)-buf[i]) >> 7) ^ 1;
            pad_count += ((pad_done | (unsigned char)-pad_done) >> 7) ^ 1;
        }
    }
    else
    {
        /* Decode EMSA-PKCS1-v1_5 padding: 0x00 || 0x01 || PS || 0x00
         * where PS must be at least 8 bytes with the value 0xFF. */
        bad |= buf[1] ^ MBEDTLS_RSA_SIGN;

        /* Read the whole buffer. Set pad_done to nonzero if we find
         * the 0x00 byte and remember the padding length in pad_count.
         * If there's a non-0xff byte in the padding, the padding is bad. */
        for( i = 2; i < ilen; i++ )
        {
            pad_done |= if_int( buf[i], 0, 1 );
            pad_count += if_int( pad_done, 0, 1 );
            bad |= if_int( pad_done, 0, buf[i] ^ 0xFF );
        }
    }

    /* If pad_done is still zero, there's no data, only unfinished padding. */
    bad |= if_int( pad_done, 0, 1 );

    /* There must be at least 8 bytes of padding. */
    bad |= size_greater_than( 8, pad_count );

    /* If the padding is valid, set plaintext_size to the number of
     * remaining bytes after stripping the padding. If the padding
     * is invalid, avoid leaking this fact through the size of the
     * output: use the maximum message size that fits in the output
     * buffer. Do it without branches to avoid leaking the padding
     * validity through timing. RSA keys are small enough that all the
     * size_t values involved fit in unsigned int. */
    plaintext_size = if_int( bad,
                             (unsigned) plaintext_max_size,
                             (unsigned) ( ilen - pad_count - 3 ) );

    /* Set output_too_large to 0 if the plaintext fits in the output
     * buffer and to 1 otherwise. */
    output_too_large = size_greater_than( plaintext_size,
                                          plaintext_max_size );

    /* Set ret without branches to avoid timing attacks. Return:
     * - INVALID_PADDING if the padding is bad (bad != 0).
     * - OUTPUT_TOO_LARGE if the padding is good but the decrypted
     *   plaintext does not fit in the output buffer.
     * - 0 if the padding is correct. */
    ret = - (int) if_int( bad, - MBEDTLS_ERR_RSA_INVALID_PADDING,
                  if_int( output_too_large, - MBEDTLS_ERR_RSA_OUTPUT_TOO_LARGE,
                          0 ) );

    /* If the padding is bad or the plaintext is too large, zero the
     * data that we're about to copy to the output buffer.
     * We need to copy the same amount of data
     * from the same buffer whether the padding is good or not to
     * avoid leaking the padding validity through overall timing or
     * through memory or cache access patterns. */
    bad = all_or_nothing_int( bad | output_too_large );
    for( i = 11; i < ilen; i++ )
        buf[i] &= ~bad;

    /* If the plaintext is too large, truncate it to the buffer size.
     * Copy anyway to avoid revealing the length through timing, because
     * revealing the length is as bad as revealing the padding validity
     * for a Bleichenbacher attack. */
    plaintext_size = if_int( output_too_large,
                             (unsigned) plaintext_max_size,
                             (unsigned) plaintext_size );

    /* Move the plaintext to the leftmost position where it can start in
     * the working buffer, i.e. make it start plaintext_max_size from
     * the end of the buffer. Do this with a memory access trace that
     * does not depend on the plaintext size. After this move, the
     * starting location of the plaintext is no longer sensitive
     * information. */
    mem_move_to_left( buf + ilen - plaintext_max_size,
                      plaintext_max_size,
                      plaintext_max_size - plaintext_size );

    /* Finally copy the decrypted plaintext plus trailing zeros into the output
     * buffer. If output_max_len is 0, then output may be an invalid pointer
     * and the result of memcpy() would be undefined; prevent undefined
     * behavior making sure to depend only on output_max_len (the size of the
     * user-provided output buffer), which is independent from plaintext
     * length, validity of padding, success of the decryption, and other
     * secrets. */
    if( output_max_len != 0 )
        memcpy( output, buf + ilen - plaintext_max_size, plaintext_max_size );

    /* Report the amount of data we copied to the output buffer. In case
     * of errors (bad padding or output too large), the value of *olen
     * when this function returns is not specified. Making it equivalent
     * to the good case limits the risks of leaking the padding validity. */
    *olen = plaintext_size;

cleanup:
    mbedtls_platform_zeroize( buf, sizeof( buf ) );
    return( ret );
}
#endif /* MBEDTLS_PKCS1_V15 */

/*
 * Do an RSA operation, then remove the message padding
 */
int mbedtls_rsa_pkcs1_decrypt( mbedtls_rsa_context *ctx,
                               int (*f_rng)(void *, unsigned char *, size_t),
                               void *p_rng, int mode, size_t *olen,
                               const unsigned char *input,
                               unsigned char *output,
                               size_t output_max_len)
{
    RSA_VALIDATE_RET( ctx != NULL );
    RSA_VALIDATE_RET( mode == MBEDTLS_RSA_PRIVATE ||
                      mode == MBEDTLS_RSA_PUBLIC );
    RSA_VALIDATE_RET( output_max_len == 0 || output != NULL );
    RSA_VALIDATE_RET( input != NULL );
    RSA_VALIDATE_RET( olen != NULL );
    switch( ctx->padding )
    {
#if defined(MBEDTLS_PKCS1_V15)
        case MBEDTLS_RSA_PKCS_V15:
            return mbedtls_rsa_rsaes_pkcs1_v15_decrypt( ctx, f_rng, p_rng, mode, olen,
                                                        input, output, output_max_len );
#endif
#if defined(MBEDTLS_PKCS1_V21)
        case MBEDTLS_RSA_PKCS_V21:
            return mbedtls_rsa_rsaes_oaep_decrypt( ctx, f_rng, p_rng, mode, NULL, 0,
                                                   olen, input, output,
                                                   output_max_len );
#endif
        default:
            return( MBEDTLS_ERR_RSA_INVALID_PADDING );
    }
}

/*
 * Implementation of the PKCS#1 v2.1 RSASSA-PSS-SIGN function
 */
int mbedtls_rsa_rsassa_pss_sign( mbedtls_rsa_context *ctx,
                                 int (*f_rng)(void *, unsigned char *, size_t),
                                 void *p_rng, int mode,
                                 mbedtls_md_type_t md_alg,
                                 unsigned int hashlen,
                                 const unsigned char *hash,
                                 unsigned char *sig )
{
    size_t olen;
    unsigned char *p = sig;
    unsigned char salt[MBEDTLS_MD_MAX_SIZE];
    size_t slen, min_slen, hlen, offset = 0;
    int ret = MBEDTLS_ERR_THIS_CORRUPTION;
    size_t msb;
    const mbedtls_md_info_t *md_info;
    mbedtls_md_context_t md_ctx;
    RSA_VALIDATE_RET( ctx != NULL );
    RSA_VALIDATE_RET( mode == MBEDTLS_RSA_PRIVATE ||
                      mode == MBEDTLS_RSA_PUBLIC );
    RSA_VALIDATE_RET( ( md_alg  == MBEDTLS_MD_NONE &&
                        hashlen == 0 ) ||
                      hash != NULL );
    RSA_VALIDATE_RET( sig != NULL );
    if( mode == MBEDTLS_RSA_PRIVATE && ctx->padding != MBEDTLS_RSA_PKCS_V21 )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    if( !f_rng )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    olen = ctx->len;
    if( md_alg != MBEDTLS_MD_NONE )
    {
        /* Gather length of hash to sign */
        md_info = mbedtls_md_info_from_type( md_alg );
        if( !md_info )
            return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
        hashlen = mbedtls_md_get_size( md_info );
    }
    md_info = mbedtls_md_info_from_type( (mbedtls_md_type_t) ctx->hash_id );
    if( !md_info )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    hlen = mbedtls_md_get_size( md_info );
    /* Calculate the largest possible salt length. Normally this is the hash
     * length, which is the maximum length the salt can have. If there is not
     * enough room, use the maximum salt length that fits. The constraint is
     * that the hash length plus the salt length plus 2 bytes must be at most
     * the key length. This complies with FIPS 186-4 §5.5 (e) and RFC 8017
     * (PKCS#1 v2.2) §9.1.1 step 3. */
    min_slen = hlen - 2;
    if( olen < hlen + min_slen + 2 )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    else if( olen >= hlen + hlen + 2 )
        slen = hlen;
    else
        slen = olen - hlen - 2;
    mbedtls_platform_zeroize( sig, olen );
    /* Generate salt of length slen */
    if( ( ret = f_rng( p_rng, salt, slen ) ) != 0 )
        return( MBEDTLS_ERR_RSA_RNG_FAILED + ret );
    /* Note: EMSA-PSS encoding is over the length of N - 1 bits */
    msb = mbedtls_mpi_bitlen( &ctx->N ) - 1;
    p += olen - hlen - slen - 2;
    *p++ = 0x01;
    memcpy( p, salt, slen );
    p += slen;
    mbedtls_md_init( &md_ctx );
    if( ( ret = mbedtls_md_setup( &md_ctx, md_info, 0 ) ) != 0 )
        goto exit;
    /* Generate H = Hash( M' ) */
    if( ( ret = mbedtls_md_starts( &md_ctx ) ) != 0 )
        goto exit;
    if( ( ret = mbedtls_md_update( &md_ctx, p, 8 ) ) != 0 )
        goto exit;
    if( ( ret = mbedtls_md_update( &md_ctx, hash, hashlen ) ) != 0 )
        goto exit;
    if( ( ret = mbedtls_md_update( &md_ctx, salt, slen ) ) != 0 )
        goto exit;
    if( ( ret = mbedtls_md_finish( &md_ctx, p ) ) != 0 )
        goto exit;
    /* Compensate for boundary condition when applying mask */
    if( msb % 8 == 0 )
        offset = 1;
    /* maskedDB: Apply dbMask to DB */
    if( ( ret = mgf_mask( sig + offset, olen - hlen - 1 - offset, p, hlen,
                          &md_ctx ) ) != 0 )
        goto exit;
    msb = mbedtls_mpi_bitlen( &ctx->N ) - 1;
    sig[0] &= 0xFF >> ( olen * 8 - msb );
    p += hlen;
    *p++ = 0xBC;
    mbedtls_platform_zeroize( salt, sizeof( salt ) );
exit:
    mbedtls_md_free( &md_ctx );
    if( ret != 0 )
        return( ret );
    return( ( mode == MBEDTLS_RSA_PUBLIC )
            ? mbedtls_rsa_public(  ctx, sig, sig )
            : mbedtls_rsa_private( ctx, f_rng, p_rng, sig, sig ) );
}

#if defined(MBEDTLS_PKCS1_V15)
/*
 * Implementation of the PKCS#1 v2.1 RSASSA-PKCS1-V1_5-SIGN function
 */
/* Construct a PKCS v1.5 encoding of a hashed message
 *
 * This is used both for signature generation and verification.
 *
 * Parameters:
 * - md_alg:  Identifies the hash algorithm used to generate the given hash;
 *            MBEDTLS_MD_NONE if raw data is signed.
 * - hashlen: Length of hash in case hashlen is MBEDTLS_MD_NONE.
 * - hash:    Buffer containing the hashed message or the raw data.
 * - dst_len: Length of the encoded message.
 * - dst:     Buffer to hold the encoded message.
 *
 * Assumptions:
 * - hash has size hashlen if md_alg == MBEDTLS_MD_NONE.
 * - hash has size corresponding to md_alg if md_alg != MBEDTLS_MD_NONE.
 * - dst points to a buffer of size at least dst_len.
 *
 */
static int rsa_rsassa_pkcs1_v15_encode( mbedtls_md_type_t md_alg,
                                        unsigned int hashlen,
                                        const unsigned char *hash,
                                        size_t dst_len,
                                        unsigned char *dst )
{
    size_t oid_size  = 0;
    size_t nb_pad    = dst_len;
    unsigned char *p = dst;
    const char *oid  = NULL;
    /* Are we signing hashed or raw data? */
    if( md_alg != MBEDTLS_MD_NONE )
    {
        const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type( md_alg );
        if( !md_info )
            return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
        if( mbedtls_oid_get_oid_by_md( md_alg, &oid, &oid_size ) != 0 )
            return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
        hashlen = mbedtls_md_get_size( md_info );
        /* Double-check that 8 + hashlen + oid_size can be used as a
         * 1-byte ASN.1 length encoding and that there's no overflow. */
        if( 8 + hashlen + oid_size  >= 0x80         ||
            10 + hashlen            <  hashlen      ||
            10 + hashlen + oid_size <  10 + hashlen )
            return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
        /*
         * Static bounds check:
         * - Need 10 bytes for five tag-length pairs.
         *   (Insist on 1-byte length encodings to protect against variants of
         *    Bleichenbacher's forgery attack against lax PKCS#1v1.5 verification)
         * - Need hashlen bytes for hash
         * - Need oid_size bytes for hash alg OID.
         */
        if( nb_pad < 10 + hashlen + oid_size )
            return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
        nb_pad -= 10 + hashlen + oid_size;
    }
    else
    {
        if( nb_pad < hashlen )
            return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
        nb_pad -= hashlen;
    }
    /* Need space for signature header and padding delimiter (3 bytes),
     * and 8 bytes for the minimal padding */
    if( nb_pad < 3 + 8 )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    nb_pad -= 3;
    /* Now nb_pad is the amount of memory to be filled
     * with padding, and at least 8 bytes long. */
    /* Write signature header and padding */
    *p++ = 0;
    *p++ = MBEDTLS_RSA_SIGN;
    memset( p, 0xFF, nb_pad );
    p += nb_pad;
    *p++ = 0;
    /* Are we signing raw data? */
    if( md_alg == MBEDTLS_MD_NONE )
    {
        memcpy( p, hash, hashlen );
        return( 0 );
    }
    /* Signing hashed data, add corresponding ASN.1 structure
     *
     * DigestInfo ::= SEQUENCE {
     *   digestAlgorithm DigestAlgorithmIdentifier,
     *   digest Digest }
     * DigestAlgorithmIdentifier ::= AlgorithmIdentifier
     * Digest ::= OCTET STRING
     *
     * Schematic:
     * TAG-SEQ + LEN [ TAG-SEQ + LEN [ TAG-OID  + LEN [ OID  ]
     *                                 TAG-NULL + LEN [ NULL ] ]
     *                 TAG-OCTET + LEN [ HASH ] ]
     */
    *p++ = MBEDTLS_ASN1_SEQUENCE | MBEDTLS_ASN1_CONSTRUCTED;
    *p++ = (unsigned char)( 0x08 + oid_size + hashlen );
    *p++ = MBEDTLS_ASN1_SEQUENCE | MBEDTLS_ASN1_CONSTRUCTED;
    *p++ = (unsigned char)( 0x04 + oid_size );
    *p++ = MBEDTLS_ASN1_OID;
    *p++ = (unsigned char) oid_size;
    memcpy( p, oid, oid_size );
    p += oid_size;
    *p++ = MBEDTLS_ASN1_NULL;
    *p++ = 0x00;
    *p++ = MBEDTLS_ASN1_OCTET_STRING;
    *p++ = (unsigned char) hashlen;
    memcpy( p, hash, hashlen );
    p += hashlen;
    /* Just a sanity-check, should be automatic
     * after the initial bounds check. */
    if( p != dst + dst_len )
    {
        mbedtls_platform_zeroize( dst, dst_len );
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    }
    return( 0 );
}

/*
 * Do an RSA operation to sign the message digest
 */
int mbedtls_rsa_rsassa_pkcs1_v15_sign( mbedtls_rsa_context *ctx,
                                       int (*f_rng)(void *, unsigned char *, size_t),
                                       void *p_rng, int mode,
                                       mbedtls_md_type_t md_alg,
                                       unsigned int hashlen,
                                       const unsigned char *hash,
                                       unsigned char *sig )
{
    int ret = MBEDTLS_ERR_THIS_CORRUPTION;
    unsigned char *sig_try = NULL, *verif = NULL;
    RSA_VALIDATE_RET( ctx );
    RSA_VALIDATE_RET( sig );
    RSA_VALIDATE_RET( mode == MBEDTLS_RSA_PRIVATE ||
                      mode == MBEDTLS_RSA_PUBLIC );
    RSA_VALIDATE_RET( hash || ( !hashlen && md_alg  == MBEDTLS_MD_NONE ) );
    if( mode == MBEDTLS_RSA_PRIVATE && ctx->padding != MBEDTLS_RSA_PKCS_V15 )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    /*
     * Prepare PKCS1-v1.5 encoding (padding and hash identifier)
     */
    if( ( ret = rsa_rsassa_pkcs1_v15_encode( md_alg, hashlen, hash,
                                             ctx->len, sig ) ) != 0 )
        return( ret );
    /*
     * Call respective RSA primitive
     */
    if( mode == MBEDTLS_RSA_PUBLIC )
    {
        /* Skip verification on a public key operation */
        return( mbedtls_rsa_public( ctx, sig, sig ) );
    }
    /* Private key operation
     *
     * In order to prevent Lenstra's attack, make the signature in a
     * temporary buffer and check it before returning it.
     */
    sig_try = mbedtls_calloc( 1, ctx->len );
    if( !sig_try )
        return( MBEDTLS_ERR_MPI_ALLOC_FAILED );
    verif = mbedtls_calloc( 1, ctx->len );
    if( !verif )
    {
        mbedtls_free( sig_try );
        return( MBEDTLS_ERR_MPI_ALLOC_FAILED );
    }
    MBEDTLS_MPI_CHK( mbedtls_rsa_private( ctx, f_rng, p_rng, sig, sig_try ) );
    MBEDTLS_MPI_CHK( mbedtls_rsa_public( ctx, sig_try, verif ) );
    if( timingsafe_bcmp( verif, sig, ctx->len ) )
    {
        ret = MBEDTLS_ERR_RSA_PRIVATE_FAILED;
        goto cleanup;
    }
    memcpy( sig, sig_try, ctx->len );
cleanup:
    mbedtls_free( sig_try );
    mbedtls_free( verif );
    return( ret );
}
#endif /* MBEDTLS_PKCS1_V15 */

/*
 * Do an RSA operation to sign the message digest
 */
int mbedtls_rsa_pkcs1_sign( mbedtls_rsa_context *ctx,
                            int (*f_rng)(void *, unsigned char *, size_t),
                            void *p_rng, int mode,
                            mbedtls_md_type_t md_alg,
                            unsigned int hashlen,
                            const unsigned char *hash,
                            unsigned char *sig )
{
    RSA_VALIDATE_RET( ctx );
    RSA_VALIDATE_RET( sig );
    RSA_VALIDATE_RET( mode == MBEDTLS_RSA_PRIVATE ||
                      mode == MBEDTLS_RSA_PUBLIC );
    RSA_VALIDATE_RET( hash || ( !hashlen && md_alg  == MBEDTLS_MD_NONE ) );
    switch( ctx->padding )
    {
#if defined(MBEDTLS_PKCS1_V15)
        case MBEDTLS_RSA_PKCS_V15:
            return mbedtls_rsa_rsassa_pkcs1_v15_sign( ctx, f_rng, p_rng, mode, md_alg,
                                                      hashlen, hash, sig );
#endif
#if defined(MBEDTLS_PKCS1_V21)
        case MBEDTLS_RSA_PKCS_V21:
            return mbedtls_rsa_rsassa_pss_sign( ctx, f_rng, p_rng, mode, md_alg,
                                                hashlen, hash, sig );
#endif
        default:
            return( MBEDTLS_ERR_RSA_INVALID_PADDING );
    }
}


#if defined(MBEDTLS_PKCS1_V15)

/**
 * \brief          This function performs a PKCS#1 v2.1 PSS verification
 *                 operation (RSASSA-PSS-VERIFY).
 *
 *                 The hash function for the MGF mask generating function
 *                 is that specified in \p mgf1_hash_id.
 *
 * \note           The \p sig buffer must be as large as the size
 *                 of \p ctx->N. For example, 128 Bytes if RSA-1024 is used.
 *
 * \note           The \p hash_id in the RSA context is ignored.
 *
 * \param ctx      The initialized RSA public key context to use.
 * \param f_rng    The RNG function to use. If \p mode is #MBEDTLS_RSA_PRIVATE,
 *                 this is used for blinding and should be provided; see
 *                 mbedtls_rsa_private() for more. Otherwise, it is ignored.
 * \param p_rng    The RNG context to be passed to \p f_rng. This may be
 *                 \c NULL if \p f_rng is \c NULL or doesn't need a context.
 * \param mode     The mode of operation. This must be either
 *                 #MBEDTLS_RSA_PUBLIC or #MBEDTLS_RSA_PRIVATE.
 * \param md_alg   The message-digest algorithm used to hash the original data.
 *                 Use #MBEDTLS_MD_NONE for signing raw data.
 * \param hashlen  The length of the message digest.
 *                 This is only used if \p md_alg is #MBEDTLS_MD_NONE.
 * \param hash     The buffer holding the message digest or raw data.
 *                 If \p md_alg is #MBEDTLS_MD_NONE, this must be a readable
 *                 buffer of length \p hashlen Bytes. If \p md_alg is not
 *                 #MBEDTLS_MD_NONE, it must be a readable buffer of length
 *                 the size of the hash corresponding to \p md_alg.
 * \param mgf1_hash_id      The message digest used for mask generation.
 * \param expected_salt_len The length of the salt used in padding. Use
 *                          #MBEDTLS_RSA_SALT_LEN_ANY to accept any salt length.
 * \param sig      The buffer holding the signature. This must be a readable
 *                 buffer of length \c ctx->len Bytes. For example, \c 256 Bytes
 *                 for an 2048-bit RSA modulus.
 *
 * \return         \c 0 if the verify operation was successful.
 * \return         An \c MBEDTLS_ERR_RSA_XXX error code on failure.
 */
int mbedtls_rsa_rsassa_pss_verify_ext( mbedtls_rsa_context *ctx,
                                       int (*f_rng)(void *, unsigned char *, size_t),
                                       void *p_rng, int mode,
                                       mbedtls_md_type_t md_alg,
                                       unsigned int hashlen,
                                       const unsigned char *hash,
                                       mbedtls_md_type_t mgf1_hash_id,
                                       int expected_salt_len,
                                       const unsigned char *sig )
{
    int ret = MBEDTLS_ERR_THIS_CORRUPTION;
    size_t siglen;
    unsigned char *p;
    unsigned char *hash_start;
    unsigned char result[MBEDTLS_MD_MAX_SIZE];
    unsigned char zeros[8];
    unsigned int hlen;
    size_t observed_salt_len, msb;
    const mbedtls_md_info_t *md_info;
    mbedtls_md_context_t md_ctx;
    unsigned char buf[MBEDTLS_MPI_MAX_SIZE];
    RSA_VALIDATE_RET( ctx );
    RSA_VALIDATE_RET( mode == MBEDTLS_RSA_PRIVATE ||
                      mode == MBEDTLS_RSA_PUBLIC );
    RSA_VALIDATE_RET( sig );
    RSA_VALIDATE_RET( ( md_alg  == MBEDTLS_MD_NONE &&
                        hashlen == 0 ) ||
                      hash );
    if( mode == MBEDTLS_RSA_PRIVATE && ctx->padding != MBEDTLS_RSA_PKCS_V21 )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    siglen = ctx->len;
    if( siglen < 16 || siglen > sizeof( buf ) )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    ret = ( mode == MBEDTLS_RSA_PUBLIC )
          ? mbedtls_rsa_public(  ctx, sig, buf )
          : mbedtls_rsa_private( ctx, f_rng, p_rng, sig, buf );
    if( ret != 0 )
        return( ret );
    p = buf;
    if( buf[siglen - 1] != 0xBC )
        return( MBEDTLS_ERR_RSA_INVALID_PADDING );
    if( md_alg != MBEDTLS_MD_NONE )
    {
        /* Gather length of hash to sign */
        md_info = mbedtls_md_info_from_type( md_alg );
        if( !md_info )
            return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
        hashlen = mbedtls_md_get_size( md_info );
    }
    md_info = mbedtls_md_info_from_type( mgf1_hash_id );
    if( !md_info )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    hlen = mbedtls_md_get_size( md_info );
    memset( zeros, 0, 8 );
    /*
     * Note: EMSA-PSS verification is over the length of N - 1 bits
     */
    msb = mbedtls_mpi_bitlen( &ctx->N ) - 1;
    if( buf[0] >> ( 8 - siglen * 8 + msb ) )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    /* Compensate for boundary condition when applying mask */
    if( msb % 8 == 0 )
    {
        p++;
        siglen -= 1;
    }
    if( siglen < hlen + 2 )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    hash_start = p + siglen - hlen - 1;
    mbedtls_md_init( &md_ctx );
    if( ( ret = mbedtls_md_setup( &md_ctx, md_info, 0 ) ) != 0 )
        goto exit;
    ret = mgf_mask( p, siglen - hlen - 1, hash_start, hlen, &md_ctx );
    if( ret != 0 )
        goto exit;
    buf[0] &= 0xFF >> ( siglen * 8 - msb );
    while( p < hash_start - 1 && *p == 0 )
        p++;
    if( *p++ != 0x01 )
    {
        ret = MBEDTLS_ERR_RSA_INVALID_PADDING;
        goto exit;
    }
    observed_salt_len = hash_start - p;
    if( expected_salt_len != MBEDTLS_RSA_SALT_LEN_ANY &&
        observed_salt_len != (size_t) expected_salt_len )
    {
        ret = MBEDTLS_ERR_RSA_INVALID_PADDING;
        goto exit;
    }
    /*
     * Generate H = Hash( M' )
     */
    if(( ret = mbedtls_md_starts( &md_ctx ) )) goto exit;
    if(( ret = mbedtls_md_update( &md_ctx, zeros, 8 ) )) goto exit;
    if(( ret = mbedtls_md_update( &md_ctx, hash, hashlen ) )) goto exit;
    if(( ret = mbedtls_md_update( &md_ctx, p, observed_salt_len ) )) goto exit;
    if(( ret = mbedtls_md_finish( &md_ctx, result ) )) goto exit;
    if ( timingsafe_bcmp( hash_start, result, hlen ) )
    {
        ret = MBEDTLS_ERR_RSA_VERIFY_FAILED;
        goto exit;
    }
exit:
    mbedtls_md_free( &md_ctx );
    return( ret );
}

/**
 * \brief          This function performs a PKCS#1 v2.1 PSS verification
 *                 operation (RSASSA-PSS-VERIFY).
 *
 *                 The hash function for the MGF mask generating function
 *                 is that specified in the RSA context.
 *
 * \note           The \p hash_id in the RSA context is the one used for the
 *                 verification. \p md_alg in the function call is the type of
 *                 hash that is verified. According to <em>RFC-3447: Public-Key
 *                 Cryptography Standards (PKCS) #1 v2.1: RSA Cryptography
 *                 Specifications</em> it is advised to keep both hashes the
 *                 same. If \p hash_id in the RSA context is unset,
 *                 the \p md_alg from the function call is used.
 *
 * \deprecated     It is deprecated and discouraged to call this function
 *                 in #MBEDTLS_RSA_PRIVATE mode. Future versions of the library
 *                 are likely to remove the \p mode argument and have it
 *                 implicitly set to #MBEDTLS_RSA_PUBLIC.
 *
 * \param ctx      The initialized RSA public key context to use.
 * \param f_rng    The RNG function to use. If \p mode is #MBEDTLS_RSA_PRIVATE,
 *                 this is used for blinding and should be provided; see
 *                 mbedtls_rsa_private() for more. Otherwise, it is ignored.
 * \param p_rng    The RNG context to be passed to \p f_rng. This may be
 *                 \c NULL if \p f_rng is \c NULL or doesn't need a context.
 * \param mode     The mode of operation. This must be either
 *                 #MBEDTLS_RSA_PUBLIC or #MBEDTLS_RSA_PRIVATE (deprecated).
 * \param md_alg   The message-digest algorithm used to hash the original data.
 *                 Use #MBEDTLS_MD_NONE for signing raw data.
 * \param hashlen  The length of the message digest.
 *                 This is only used if \p md_alg is #MBEDTLS_MD_NONE.
 * \param hash     The buffer holding the message digest or raw data.
 *                 If \p md_alg is #MBEDTLS_MD_NONE, this must be a readable
 *                 buffer of length \p hashlen Bytes. If \p md_alg is not
 *                 #MBEDTLS_MD_NONE, it must be a readable buffer of length
 *                 the size of the hash corresponding to \p md_alg.
 * \param sig      The buffer holding the signature. This must be a readable
 *                 buffer of length \c ctx->len Bytes. For example, \c 256 Bytes
 *                 for an 2048-bit RSA modulus.
 *
 * \return         \c 0 if the verify operation was successful.
 * \return         An \c MBEDTLS_ERR_RSA_XXX error code on failure.
 */
int mbedtls_rsa_rsassa_pss_verify( mbedtls_rsa_context *ctx,
                                   int (*f_rng)(void *, unsigned char *, size_t),
                                   void *p_rng, int mode,
                                   mbedtls_md_type_t md_alg,
                                   unsigned int hashlen,
                                   const unsigned char *hash,
                                   const unsigned char *sig )
{
    mbedtls_md_type_t mgf1_hash_id;
    RSA_VALIDATE_RET( ctx );
    RSA_VALIDATE_RET( sig );
    RSA_VALIDATE_RET( mode == MBEDTLS_RSA_PRIVATE ||
                      mode == MBEDTLS_RSA_PUBLIC );
    RSA_VALIDATE_RET( hash || ( !hashlen && md_alg  == MBEDTLS_MD_NONE ) );
    mgf1_hash_id = ( ctx->hash_id != MBEDTLS_MD_NONE )
                             ? (mbedtls_md_type_t) ctx->hash_id
                             : md_alg;
    return( mbedtls_rsa_rsassa_pss_verify_ext( ctx, f_rng, p_rng, mode,
                                               md_alg, hashlen, hash,
                                               mgf1_hash_id, MBEDTLS_RSA_SALT_LEN_ANY,
                                               sig ) );
}

/**
 * \brief          This function performs a PKCS#1 v1.5 verification
 *                 operation (RSASSA-PKCS1-v1_5-VERIFY).
 *
 * \deprecated     It is deprecated and discouraged to call this function
 *                 in #MBEDTLS_RSA_PRIVATE mode. Future versions of the library
 *                 are likely to remove the \p mode argument and have it
 *                 set to #MBEDTLS_RSA_PUBLIC.
 *
 * \param ctx      The initialized RSA public key context to use.
 * \param f_rng    The RNG function to use. If \p mode is #MBEDTLS_RSA_PRIVATE,
 *                 this is used for blinding and should be provided; see
 *                 mbedtls_rsa_private() for more. Otherwise, it is ignored.
 * \param p_rng    The RNG context to be passed to \p f_rng. This may be
 *                 \c NULL if \p f_rng is \c NULL or doesn't need a context.
 * \param mode     The mode of operation. This must be either
 *                 #MBEDTLS_RSA_PUBLIC or #MBEDTLS_RSA_PRIVATE (deprecated).
 * \param md_alg   The message-digest algorithm used to hash the original data.
 *                 Use #MBEDTLS_MD_NONE for signing raw data.
 * \param hashlen  The length of the message digest.
 *                 This is only used if \p md_alg is #MBEDTLS_MD_NONE.
 * \param hash     The buffer holding the message digest or raw data.
 *                 If \p md_alg is #MBEDTLS_MD_NONE, this must be a readable
 *                 buffer of length \p hashlen Bytes. If \p md_alg is not
 *                 #MBEDTLS_MD_NONE, it must be a readable buffer of length
 *                 the size of the hash corresponding to \p md_alg.
 * \param sig      The buffer holding the signature. This must be a readable
 *                 buffer of length \c ctx->len Bytes. For example, \c 256 Bytes
 *                 for an 2048-bit RSA modulus.
 *
 * \return         \c 0 if the verify operation was successful.
 * \return         An \c MBEDTLS_ERR_RSA_XXX error code on failure.
 */
int mbedtls_rsa_rsassa_pkcs1_v15_verify( mbedtls_rsa_context *ctx,
                                         int (*f_rng)(void *, unsigned char *, size_t),
                                         void *p_rng, int mode,
                                         mbedtls_md_type_t md_alg,
                                         unsigned int hashlen,
                                         const unsigned char *hash,
                                         const unsigned char *sig )
{
    int ret = 0;
    size_t sig_len;
    unsigned char *encoded = NULL, *encoded_expected = NULL;
    RSA_VALIDATE_RET( ctx != NULL );
    RSA_VALIDATE_RET( mode == MBEDTLS_RSA_PRIVATE ||
                      mode == MBEDTLS_RSA_PUBLIC );
    RSA_VALIDATE_RET( sig != NULL );
    RSA_VALIDATE_RET( ( md_alg  == MBEDTLS_MD_NONE &&
                        hashlen == 0 ) ||
                      hash != NULL );
    sig_len = ctx->len;
    if( mode == MBEDTLS_RSA_PRIVATE && ctx->padding != MBEDTLS_RSA_PKCS_V15 )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    /*
     * Prepare expected PKCS1 v1.5 encoding of hash.
     */
    if( ( encoded          = mbedtls_calloc( 1, sig_len ) ) == NULL ||
        ( encoded_expected = mbedtls_calloc( 1, sig_len ) ) == NULL )
    {
        ret = MBEDTLS_ERR_MPI_ALLOC_FAILED;
        goto cleanup;
    }
    if( ( ret = rsa_rsassa_pkcs1_v15_encode( md_alg, hashlen, hash, sig_len,
                                             encoded_expected ) ) != 0 )
        goto cleanup;
    /*
     * Apply RSA primitive to get what should be PKCS1 encoded hash.
     */
    ret = ( mode == MBEDTLS_RSA_PUBLIC )
          ? mbedtls_rsa_public(  ctx, sig, encoded )
          : mbedtls_rsa_private( ctx, f_rng, p_rng, sig, encoded );
    if( ret != 0 )
        goto cleanup;
    /*
     * Compare
     */
    if( ( ret = timingsafe_bcmp( encoded, encoded_expected, sig_len ) ) )
    {
        ret = MBEDTLS_ERR_RSA_VERIFY_FAILED;
        goto cleanup;
    }
cleanup:
    if( encoded )
    {
        mbedtls_platform_zeroize( encoded, sig_len );
        mbedtls_free( encoded );
    }
    if( encoded_expected )
    {
        mbedtls_platform_zeroize( encoded_expected, sig_len );
        mbedtls_free( encoded_expected );
    }
    return( ret );
}

#endif /* MBEDTLS_PKCS1_V15 */

/*
 * Do an RSA operation and check the message digest
 */
int mbedtls_rsa_pkcs1_verify( mbedtls_rsa_context *ctx,
                              int (*f_rng)(void *, unsigned char *, size_t),
                              void *p_rng, int mode,
                              mbedtls_md_type_t md_alg,
                              unsigned int hashlen,
                              const unsigned char *hash,
                              const unsigned char *sig )
{
    RSA_VALIDATE_RET( ctx );
    RSA_VALIDATE_RET( sig );
    RSA_VALIDATE_RET( mode == MBEDTLS_RSA_PRIVATE ||
                      mode == MBEDTLS_RSA_PUBLIC );
    RSA_VALIDATE_RET( hash || ( !hashlen && md_alg  == MBEDTLS_MD_NONE ) );
    switch( ctx->padding )
    {
#if defined(MBEDTLS_PKCS1_V15)
        case MBEDTLS_RSA_PKCS_V15:
            return mbedtls_rsa_rsassa_pkcs1_v15_verify( ctx, f_rng, p_rng, mode, md_alg,
                                                hashlen, hash, sig );
#endif
#if defined(MBEDTLS_PKCS1_V21)
        case MBEDTLS_RSA_PKCS_V21:
            return mbedtls_rsa_rsassa_pss_verify( ctx, f_rng, p_rng, mode, md_alg,
                                          hashlen, hash, sig );
#endif
        default:
            return( MBEDTLS_ERR_RSA_INVALID_PADDING );
    }
}

/**
 * \brief          This function copies the components of an RSA context.
 *
 * \param dst      The destination context. This must be initialized.
 * \param src      The source context. This must be initialized.
 *
 * \return         \c 0 on success.
 * \return         #MBEDTLS_ERR_MPI_ALLOC_FAILED on memory allocation failure.
 */
int mbedtls_rsa_copy( mbedtls_rsa_context *dst, const mbedtls_rsa_context *src )
{
    int ret = MBEDTLS_ERR_THIS_CORRUPTION;
    RSA_VALIDATE_RET( dst );
    RSA_VALIDATE_RET( src );
    dst->len = src->len;
    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &dst->N, &src->N ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &dst->E, &src->E ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &dst->D, &src->D ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &dst->P, &src->P ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &dst->Q, &src->Q ) );
#if !defined(MBEDTLS_RSA_NO_CRT)
    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &dst->DP, &src->DP ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &dst->DQ, &src->DQ ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &dst->QP, &src->QP ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &dst->RP, &src->RP ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &dst->RQ, &src->RQ ) );
#endif
    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &dst->RN, &src->RN ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &dst->Vi, &src->Vi ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &dst->Vf, &src->Vf ) );
    dst->padding = src->padding;
    dst->hash_id = src->hash_id;
cleanup:
    if( ret != 0 )
        mbedtls_rsa_free( dst );
    return( ret );
}

/**
 * \brief          This function frees the components of an RSA key.
 *
 * \param ctx      The RSA context to free. May be \c NULL, in which case
 *                 this function is a no-op. If it is not \c NULL, it must
 *                 point to an initialized RSA context.
 */
void mbedtls_rsa_free( mbedtls_rsa_context *ctx )
{
    if( !ctx )
        return;
    mbedtls_mpi_free( &ctx->Vi );
    mbedtls_mpi_free( &ctx->Vf );
    mbedtls_mpi_free( &ctx->RN );
    mbedtls_mpi_free( &ctx->D  );
    mbedtls_mpi_free( &ctx->Q  );
    mbedtls_mpi_free( &ctx->P  );
    mbedtls_mpi_free( &ctx->E  );
    mbedtls_mpi_free( &ctx->N  );
#if !defined(MBEDTLS_RSA_NO_CRT)
    mbedtls_mpi_free( &ctx->RQ );
    mbedtls_mpi_free( &ctx->RP );
    mbedtls_mpi_free( &ctx->QP );
    mbedtls_mpi_free( &ctx->DQ );
    mbedtls_mpi_free( &ctx->DP );
#endif /* MBEDTLS_RSA_NO_CRT */
}

#if defined(MBEDTLS_SELF_TEST)

/*
 * Example RSA-1024 keypair, for test purposes
 */
#define PT_LEN  24
#define KEY_LEN 128
#define RSA_E   "10001"
#define RSA_N   "9292758453063D803DD603D5E777D788" \
                "8ED1D5BF35786190FA2F23EBC0848AEA" \
                "DDA92CA6C3D80B32C4D109BE0F36D6AE" \
                "7130B9CED7ACDF54CFC7555AC14EEBAB" \
                "93A89813FBF3C4F8066D2D800F7C38A8" \
                "1AE31942917403FF4946B0A83D3D3E05" \
                "EE57C6F5F5606FB5D4BC6CD34EE0801A" \
                "5E94BB77B07507233A0BC7BAC8F90F79"
#define RSA_D   "24BF6185468786FDD303083D25E64EFC" \
                "66CA472BC44D253102F8B4A9D3BFA750" \
                "91386C0077937FE33FA3252D28855837" \
                "AE1B484A8A9A45F7EE8C0C634F99E8CD" \
                "DF79C5CE07EE72C7F123142198164234" \
                "CABB724CF78B8173B9F880FC86322407" \
                "AF1FEDFDDE2BEB674CA15F3E81A1521E" \
                "071513A1E85B5DFA031F21ECAE91A34D"
#define RSA_P   "C36D0EB7FCD285223CFB5AABA5BDA3D8" \
                "2C01CAD19EA484A87EA4377637E75500" \
                "FCB2005C5C7DD6EC4AC023CDA285D796" \
                "C3D9E75E1EFC42488BB4F1D13AC30A57"
#define RSA_Q   "C000DF51A7C77AE8D7C7370C1FF55B69" \
                "E211C2B9E5DB1ED0BF61D0D9899620F4" \
                "910E4168387E3C30AA1E00C339A79508" \
                "8452DD96A9A5EA5D9DCA68DA636032AF"
#define RSA_PT  "\xAA\xBB\xCC\x03\x02\x01\x00\xFF\xFF\xFF\xFF\xFF" \
                "\x11\x22\x33\x0A\x0B\x0C\xCC\xDD\xDD\xDD\xDD\xDD"

#if defined(MBEDTLS_PKCS1_V15)
static int myrand( void *rng_state, unsigned char *output, size_t len )
{
#if !defined(__OpenBSD__) && !defined(__NetBSD__)
    size_t i;
    for( i = 0; i < len; ++i )
        output[i] = rand();
#else
    arc4random_buf( output, len );
#endif /* !OpenBSD && !NetBSD */
    return( 0 );
}
#endif /* MBEDTLS_PKCS1_V15 */

/**
 * \brief          The RSA checkup routine.
 *
 * \return         \c 0 on success.
 * \return         \c 1 on failure.
 */
int mbedtls_rsa_self_test( int verbose )
{
    int ret = 0;
#if defined(MBEDTLS_PKCS1_V15)
    size_t len;
    mbedtls_rsa_context rsa;
    unsigned char rsa_plaintext[PT_LEN];
    unsigned char rsa_decrypted[PT_LEN];
    unsigned char rsa_ciphertext[KEY_LEN];
#if defined(MBEDTLS_SHA1_C)
    unsigned char sha1sum[20];
#endif

    mbedtls_mpi K;

    mbedtls_mpi_init( &K );
    mbedtls_rsa_init( &rsa, MBEDTLS_RSA_PKCS_V15, 0 );

    MBEDTLS_MPI_CHK( mbedtls_mpi_read_string( &K, 16, RSA_N  ) );
    MBEDTLS_MPI_CHK( mbedtls_rsa_import( &rsa, &K, NULL, NULL, NULL, NULL ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_read_string( &K, 16, RSA_P  ) );
    MBEDTLS_MPI_CHK( mbedtls_rsa_import( &rsa, NULL, &K, NULL, NULL, NULL ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_read_string( &K, 16, RSA_Q  ) );
    MBEDTLS_MPI_CHK( mbedtls_rsa_import( &rsa, NULL, NULL, &K, NULL, NULL ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_read_string( &K, 16, RSA_D  ) );
    MBEDTLS_MPI_CHK( mbedtls_rsa_import( &rsa, NULL, NULL, NULL, &K, NULL ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_read_string( &K, 16, RSA_E  ) );
    MBEDTLS_MPI_CHK( mbedtls_rsa_import( &rsa, NULL, NULL, NULL, NULL, &K ) );

    MBEDTLS_MPI_CHK( mbedtls_rsa_complete( &rsa ) );

    if( verbose != 0 )
        mbedtls_printf( "  RSA key validation: " );

    if( mbedtls_rsa_check_pubkey(  &rsa ) != 0 ||
        mbedtls_rsa_check_privkey( &rsa ) != 0 )
    {
        if( verbose != 0 )
            mbedtls_printf( "failed\n" );

        ret = 1;
        goto cleanup;
    }

    if( verbose != 0 )
        mbedtls_printf( "passed\n  PKCS#1 encryption : " );

    memcpy( rsa_plaintext, RSA_PT, PT_LEN );

    if( mbedtls_rsa_pkcs1_encrypt( &rsa, myrand, NULL, MBEDTLS_RSA_PUBLIC,
                                   PT_LEN, rsa_plaintext,
                                   rsa_ciphertext ) != 0 )
    {
        if( verbose != 0 )
            mbedtls_printf( "failed\n" );

        ret = 1;
        goto cleanup;
    }

    if( verbose != 0 )
        mbedtls_printf( "passed\n  PKCS#1 decryption : " );

    if( mbedtls_rsa_pkcs1_decrypt( &rsa, myrand, NULL, MBEDTLS_RSA_PRIVATE,
                                   &len, rsa_ciphertext, rsa_decrypted,
                                   sizeof(rsa_decrypted) ) != 0 )
    {
        if( verbose != 0 )
            mbedtls_printf( "failed\n" );

        ret = 1;
        goto cleanup;
    }

    if( timingsafe_bcmp( rsa_decrypted, rsa_plaintext, len ) != 0 )
    {
        if( verbose != 0 )
            mbedtls_printf( "failed\n" );

        ret = 1;
        goto cleanup;
    }

    if( verbose != 0 )
        mbedtls_printf( "passed\n" );

#if defined(MBEDTLS_SHA1_C)
    if( verbose != 0 )
        mbedtls_printf( "  PKCS#1 data sign  : " );

    if( mbedtls_sha1_ret( rsa_plaintext, PT_LEN, sha1sum ) != 0 )
    {
        if( verbose != 0 )
            mbedtls_printf( "failed\n" );

        return( 1 );
    }

    if( mbedtls_rsa_pkcs1_sign( &rsa, myrand, NULL,
                                MBEDTLS_RSA_PRIVATE, MBEDTLS_MD_SHA1, 0,
                                sha1sum, rsa_ciphertext ) != 0 )
    {
        if( verbose != 0 )
            mbedtls_printf( "failed\n" );

        ret = 1;
        goto cleanup;
    }

    if( verbose != 0 )
        mbedtls_printf( "passed\n  PKCS#1 sig. verify: " );

    if( mbedtls_rsa_pkcs1_verify( &rsa, NULL, NULL,
                                  MBEDTLS_RSA_PUBLIC, MBEDTLS_MD_SHA1, 0,
                                  sha1sum, rsa_ciphertext ) != 0 )
    {
        if( verbose != 0 )
            mbedtls_printf( "failed\n" );

        ret = 1;
        goto cleanup;
    }

    if( verbose != 0 )
        mbedtls_printf( "passed\n" );
#endif /* MBEDTLS_SHA1_C */

    if( verbose != 0 )
        mbedtls_printf( "\n" );

cleanup:
    mbedtls_mpi_free( &K );
    mbedtls_rsa_free( &rsa );
#else /* MBEDTLS_PKCS1_V15 */
    ((void) verbose);
#endif /* MBEDTLS_PKCS1_V15 */
    return( ret );
}

#endif /* MBEDTLS_SELF_TEST */

#endif /* MBEDTLS_RSA_C */
