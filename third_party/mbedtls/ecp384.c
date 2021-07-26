/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:4;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/log/check.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/runtime/gc.internal.h"
#include "libc/runtime/runtime.h"
#include "third_party/mbedtls/bignum_internal.h"
#include "third_party/mbedtls/ecp.h"
#include "third_party/mbedtls/ecp_internal.h"
#include "third_party/mbedtls/error.h"
#include "third_party/mbedtls/math.h"
#include "third_party/mbedtls/profile.h"
#include "third_party/mbedtls/traceme.h"
/* clang-format off */

static inline bool
mbedtls_p384_isz( uint64_t p[6] )
{
    return( !p[0] & !p[1] & !p[2] & !p[3] & !p[4] & !p[5] );
}

static inline bool
mbedtls_p384_gte( uint64_t p[7] ) {
    return( ((int64_t)p[6] > 0 ||
             (p[5] > 0xffffffffffffffff ||
              (p[5] == 0xffffffffffffffff &&
               (p[4] > 0xffffffffffffffff ||
                (p[4] == 0xffffffffffffffff &&
                 (p[3] > 0xffffffffffffffff ||
                  (p[3] == 0xffffffffffffffff &&
                   (p[2] > 0xfffffffffffffffe ||
                    (p[2] == 0xfffffffffffffffe &&
                     (p[1] > 0xffffffff00000000 ||
                      (p[1] == 0xffffffff00000000 &&
                       (p[0] > 0x00000000ffffffff ||
                        (p[0] == 0x00000000ffffffff))))))))))))) );
}

static inline void
mbedtls_p384_red( uint64_t p[7] )
{
#if defined(__x86_64__) && !defined(__STRICT_ANSI__)
    asm("subq\t%1,%0\n\t"
        "sbbq\t%2,8+%0\n\t"
        "sbbq\t%3,16+%0\n\t"
        "sbbq\t%4,24+%0\n\t"
        "sbbq\t%4,32+%0\n\t"
        "sbbq\t%4,40+%0\n\t"
        "sbbq\t$0,48+%0"
        : "+o"(*p)
        : "r"(0x00000000ffffffffl), "r"(0xffffffff00000000),
          "i"(0xfffffffffffffffel), "i"(0xffffffffffffffff)
        : "memory", "cc");
#else
    uint64_t c;
    SBB( p[0], p[0], 0x00000000ffffffff, 0, c );
    SBB( p[1], p[1], 0xffffffff00000000, c, c );
    SBB( p[2], p[2], 0xfffffffffffffffe, c, c );
    SBB( p[3], p[3], 0xffffffffffffffff, c, c );
    SBB( p[4], p[4], 0xffffffffffffffff, c, c );
    SBB( p[5], p[5], 0xffffffffffffffff, c, c );
    SBB( p[6], p[6], 0,                  c, c );
#endif
}

static inline void
mbedtls_p384_gro( uint64_t p[7] )
{
#if defined(__x86_64__) && !defined(__STRICT_ANSI__)
    asm("addq\t%1,%0\n\t"
        "adcq\t%2,8+%0\n\t"
        "adcq\t%3,16+%0\n\t"
        "adcq\t%4,24+%0\n\t"
        "adcq\t%4,32+%0\n\t"
        "adcq\t%4,40+%0\n\t"
        "adcq\t$0,48+%0"
        : "+o"(*p)
        : "r"(0x00000000ffffffffl), "r"(0xffffffff00000000),
          "i"(0xfffffffffffffffel), "i"(0xffffffffffffffff)
        : "memory", "cc");
#else
    uint64_t c;
    ADC( p[0], p[0], 0x00000000ffffffff, 0, c );
    ADC( p[1], p[1], 0xffffffff00000000, c, c );
    ADC( p[2], p[2], 0xfffffffffffffffe, c, c );
    ADC( p[3], p[3], 0xffffffffffffffff, c, c );
    ADC( p[4], p[4], 0xffffffffffffffff, c, c );
    ADC( p[5], p[5], 0xffffffffffffffff, c, c );
    ADC( p[6], p[6], 0,                  c, c );
#endif
}

static inline void
mbedtls_p384_rum( uint64_t p[7] )
{
    while( mbedtls_p384_gte( p ) )
        mbedtls_p384_red( p );
}

static inline void
mbedtls_p384_mod(uint64_t X[12])
{
    secp384r1(X);
    if ((int64_t)X[6] < 0) {
        do {
            mbedtls_p384_gro(X);
        } while ((int64_t)X[6] < 0);
    } else {
        while (mbedtls_p384_gte(X)) {
            mbedtls_p384_red(X);
        }
    }
}

static inline void
mbedtls_p384_sar( uint64_t p[7] )
{
    p[0] = p[0] >> 1 | p[1] << 63;
    p[1] = p[1] >> 1 | p[2] << 63;
    p[2] = p[2] >> 1 | p[3] << 63;
    p[3] = p[3] >> 1 | p[4] << 63;
    p[4] = p[4] >> 1 | p[5] << 63;
    p[5] = p[5] >> 1 | p[6] << 63;
    p[6] = (int64_t)p[6] >> 1;
}

static inline void
mbedtls_p384_shl( uint64_t p[7] )
{
    p[6] =             p[5] >> 63;
    p[5] = p[5] << 1 | p[4] >> 63;
    p[4] = p[4] << 1 | p[3] >> 63;
    p[3] = p[3] << 1 | p[2] >> 63;
    p[2] = p[2] << 1 | p[1] >> 63;
    p[1] = p[1] << 1 | p[0] >> 63;
    p[0] = p[0] << 1;
    mbedtls_p384_rum( p );
}

static void
mbedtls_p384_mul( uint64_t X[12],
                  const uint64_t A[6], size_t n,
                  const uint64_t B[6], size_t m )
{
    if( X86_HAVE(ADX) && X86_HAVE(BMI2) )
        Mul6x6Adx( X, A, B );
    else
    {
        if (A == X) A = gc(memcpy(malloc(6 * 8), A, 6 * 8));
        if (B == X) B = gc(memcpy(malloc(6 * 8), B, 6 * 8));
        Mul( X, A, n, B, m );
        mbedtls_platform_zeroize( X + n + m, (12 - n - m) * 8 );
    }
    mbedtls_p384_mod( X );
}

static void
mbedtls_p384_add( uint64_t X[7],
                  const uint64_t A[6],
                  const uint64_t B[6] )
{
#if defined(__x86_64__) && !defined(__STRICT_ANSI__)
    asm("xor\t%%rcx,%%rcx\n\t"
        "mov\t%1,%%rax\n\t"
        "add\t%2,%%rax\n\t"
        "mov\t%%rax,%0\n\t"
        "mov\t8+%1,%%rax\n\t"
        "adc\t8+%2,%%rax\n\t"
        "mov\t%%rax,8+%0\n\t"
        "mov\t16+%1,%%rax\n\t"
        "adc\t16+%2,%%rax\n\t"
        "mov\t%%rax,16+%0\n\t"
        "mov\t24+%1,%%rax\n\t"
        "adc\t24+%2,%%rax\n\t"
        "mov\t%%rax,24+%0\n\t"
        "mov\t32+%1,%%rax\n\t"
        "adc\t32+%2,%%rax\n\t"
        "mov\t%%rax,32+%0\n\t"
        "mov\t40+%1,%%rax\n\t"
        "adc\t40+%2,%%rax\n\t"
        "mov\t%%rax,40+%0\n\t"
        "adc\t$0,%%rcx\n\t"
        "mov\t%%rcx,48+%0"
        : "+o"(*X)
        : "o"(*A), "o"(*B)
        : "rax", "rcx", "memory", "cc");
#else
    uint64_t c;
    ADC( X[0], A[0], B[0], 0, c    );
    ADC( X[1], A[1], B[1], c, c    );
    ADC( X[2], A[2], B[2], c, c    );
    ADC( X[3], A[3], B[3], c, c    );
    ADC( X[4], A[4], B[4], c, c    );
    ADC( X[5], A[5], B[5], c, X[6] );
#endif
    mbedtls_p384_rum( X );
    DCHECK_EQ(0, X[6]);
}

static void
mbedtls_p384_sub( uint64_t X[7],
                  const uint64_t A[6],
                  const uint64_t B[6] )
{
#if defined(__x86_64__) && !defined(__STRICT_ANSI__)
    asm("xor\t%%rcx,%%rcx\n\t"
        "mov\t%1,%%rax\n\t"
        "sub\t%2,%%rax\n\t"
        "mov\t%%rax,%0\n\t"
        "mov\t8+%1,%%rax\n\t"
        "sbb\t8+%2,%%rax\n\t"
        "mov\t%%rax,8+%0\n\t"
        "mov\t16+%1,%%rax\n\t"
        "sbb\t16+%2,%%rax\n\t"
        "mov\t%%rax,16+%0\n\t"
        "mov\t24+%1,%%rax\n\t"
        "sbb\t24+%2,%%rax\n\t"
        "mov\t%%rax,24+%0\n\t"
        "mov\t32+%1,%%rax\n\t"
        "sbb\t32+%2,%%rax\n\t"
        "mov\t%%rax,32+%0\n\t"
        "mov\t40+%1,%%rax\n\t"
        "sbb\t40+%2,%%rax\n\t"
        "mov\t%%rax,40+%0\n\t"
        "sbb\t$0,%%rcx\n\t"
        "mov\t%%rcx,48+%0"
        : "+o"(*X)
        : "o"(*A), "o"(*B)
        : "rax", "rcx", "memory", "cc");
#else
    uint64_t c;
    SBB( X[0], A[0], B[0], 0, c );
    SBB( X[1], A[1], B[1], c, c );
    SBB( X[2], A[2], B[2], c, c );
    SBB( X[3], A[3], B[3], c, c );
    SBB( X[4], A[4], B[4], c, c );
    SBB( X[5], A[5], B[5], c, c );
    X[6] = -c;
#endif
    while( (int64_t)X[6] < 0 )
        mbedtls_p384_gro( X );
    DCHECK_EQ(0, X[6]);
}

static void
mbedtls_p384_hub( uint64_t A[7],
                  const uint64_t B[6] )
{
#if defined(__x86_64__) && !defined(__STRICT_ANSI__)
    asm("xor\t%%rcx,%%rcx\n\t"
        "mov\t%1,%%rax\n\t"
        "sub\t%%rax,%0\n\t"
        "mov\t8+%1,%%rax\n\t"
        "sbb\t%%rax,8+%0\n\t"
        "mov\t16+%1,%%rax\n\t"
        "sbb\t%%rax,16+%0\n\t"
        "mov\t24+%1,%%rax\n\t"
        "sbb\t%%rax,24+%0\n\t"
        "mov\t32+%1,%%rax\n\t"
        "sbb\t%%rax,32+%0\n\t"
        "mov\t40+%1,%%rax\n\t"
        "sbb\t%%rax,40+%0\n\t"
        "sbb\t$0,%%rcx\n\t"
        "mov\t%%rcx,48+%0"
        : "+o"(*A)
        : "o"(*B)
        : "rax", "rcx", "memory", "cc");
    while( (int64_t)A[6] < 0 )
        mbedtls_p384_gro( A );
    DCHECK_EQ(0, A[6]);
#else
    mbedtls_p384_sub(A, A, B);
#endif
}

static inline void
mbedtls_p384_cop( uint64_t X[6],
                  const uint64_t Y[6] )
{
    memcpy( X, Y, 6 * 8 );
}

static int
mbedtls_p384_dim( mbedtls_ecp_point *R )
{
    int ret;
    if( R->X.n < 6 && ( ret = mbedtls_mpi_grow( &R->X, 6 ) ) ) return ret;
    if( R->Y.n < 6 && ( ret = mbedtls_mpi_grow( &R->Y, 6 ) ) ) return ret;
    if( R->Z.n < 6 && ( ret = mbedtls_mpi_grow( &R->Z, 6 ) ) ) return ret;
    return 0;
}

int mbedtls_p384_double_jac( const mbedtls_ecp_group *G,
                             const mbedtls_ecp_point *P,
                             mbedtls_ecp_point *R )
{
    int ret;
    struct {
        uint64_t X[6], Y[6], Z[6];
        uint64_t M[12], S[12], T[12], U[12];
        size_t Xn, Yn, Zn;
    } s;
    MBEDTLS_ASSERT( G->A.p == 0 );
    MBEDTLS_ASSERT( P->X.s == 1 );
    MBEDTLS_ASSERT( P->Y.s == 1 );
    MBEDTLS_ASSERT( P->Z.s == 1 );
    MBEDTLS_ASSERT( G->P.p[0] == 0x00000000ffffffff );
    MBEDTLS_ASSERT( G->P.p[1] == 0xffffffff00000000 );
    MBEDTLS_ASSERT( G->P.p[2] == 0xfffffffffffffffe );
    MBEDTLS_ASSERT( G->P.p[3] == 0xffffffffffffffff );
    MBEDTLS_ASSERT( G->P.p[4] == 0xffffffffffffffff );
    MBEDTLS_ASSERT( G->P.p[5] == 0xffffffffffffffff );
    if ( ( ret = mbedtls_p384_dim( R ) ) ) return ret;
    mbedtls_platform_zeroize( &s, sizeof( s ) );
    s.Xn = mbedtls_mpi_limbs( &P->X );
    s.Yn = mbedtls_mpi_limbs( &P->Y );
    s.Zn = mbedtls_mpi_limbs( &P->Z );
    CHECK_LE( s.Xn, 6 );
    CHECK_LE( s.Yn, 6 );
    CHECK_LE( s.Zn, 6 );
    memcpy( s.X, P->X.p, s.Xn * 8 );
    memcpy( s.Y, P->Y.p, s.Yn * 8 );
    memcpy( s.Z, P->Z.p, s.Zn * 8 );
    mbedtls_p384_mul( s.S, s.Z, s.Zn, s.Z, s.Zn );
    mbedtls_p384_add( s.T, s.X, s.S );
    mbedtls_p384_sub( s.U, s.X, s.S );
    mbedtls_p384_mul( s.S, s.T, 6, s.U, 6 );
    mbedtls_mpi_mul_hlp1( 6, s.S, s.M, 3 );
    mbedtls_p384_rum( s.M );
    mbedtls_p384_mul( s.T, s.Y, s.Yn, s.Y, s.Yn );
    mbedtls_p384_shl( s.T );
    mbedtls_p384_mul( s.S, s.X, s.Xn, s.T, 6 );
    mbedtls_p384_shl( s.S );
    mbedtls_p384_mul( s.U, s.T, 6, s.T, 6 );
    mbedtls_p384_shl( s.U );
    mbedtls_p384_mul( s.T, s.M, 6, s.M, 6 );
    mbedtls_p384_hub( s.T, s.S );
    mbedtls_p384_hub( s.T, s.S );
    mbedtls_p384_hub( s.S, s.T );
    mbedtls_p384_mul( s.S, s.S, 6, s.M, 6 );
    mbedtls_p384_hub( s.S, s.U );
    mbedtls_p384_mul( s.U, s.Y, s.Yn, s.Z, s.Zn );
    mbedtls_p384_shl( s.U );
    mbedtls_p384_cop( R->X.p, s.T );
    mbedtls_p384_cop( R->Y.p, s.S );
    mbedtls_p384_cop( R->Z.p, s.U );
    mbedtls_platform_zeroize( &s, sizeof(s) );
    return 0;
}

int mbedtls_p384_add_mixed( const mbedtls_ecp_group *G,
                            const mbedtls_ecp_point *P,
                            const mbedtls_ecp_point *Q,
                            mbedtls_ecp_point *R )
{
    int ret;
    struct {
        uint64_t X[12], Y[12], Z[12];
        uint64_t T1[12], T2[12], T3[12], T4[12];
        size_t Xn, Yn, Zn, QXn, QYn;
    } s;
    MBEDTLS_ASSERT( P->X.s == 1 );
    MBEDTLS_ASSERT( P->Y.s == 1 );
    MBEDTLS_ASSERT( P->Z.s == 1 );
    MBEDTLS_ASSERT( Q->X.s == 1 );
    MBEDTLS_ASSERT( Q->Y.s == 1 );
    if ( ( ret = mbedtls_p384_dim( R ) ) ) return ret;
    mbedtls_platform_zeroize(&s, sizeof(s));
    s.Xn  = mbedtls_mpi_limbs( &P->X );
    s.Yn  = mbedtls_mpi_limbs( &P->Y );
    s.Zn  = mbedtls_mpi_limbs( &P->Z );
    s.QXn = mbedtls_mpi_limbs( &Q->X );
    s.QYn = mbedtls_mpi_limbs( &Q->Y );
    CHECK_LE( s.Xn,  6 );
    CHECK_LE( s.Yn,  6 );
    CHECK_LE( s.Zn,  6 );
    CHECK_LE( s.QXn, 6 );
    CHECK_LE( s.QYn, 6 );
    memcpy( s.X, P->X.p, s.Xn * 8 );
    memcpy( s.Y, P->Y.p, s.Yn * 8 );
    memcpy( s.Z, P->Z.p, s.Zn * 8 );
    mbedtls_p384_mul( s.T1,  s.Z,  s.Zn, s.Z,    s.Zn  );
    mbedtls_p384_mul( s.T2,  s.T1, 6,    s.Z,    s.Zn  );
    mbedtls_p384_mul( s.T1,  s.T1, 6,    Q->X.p, s.QXn );
    mbedtls_p384_mul( s.T2,  s.T2, 6,    Q->Y.p, s.QYn );
    mbedtls_p384_hub(        s.T1,       s.X           );
    mbedtls_p384_hub(        s.T2,       s.Y           );
    if( mbedtls_p384_isz( s.T1 ) )
    {
        if( mbedtls_p384_isz( s.T2 ) )
            return mbedtls_p384_double_jac( G, P, R );
        else
            return mbedtls_ecp_set_zero( R );
    }
    mbedtls_p384_mul( s.Z,    s.Z,   s.Zn, s.T1, 6    );
    mbedtls_p384_mul( s.T3,   s.T1,  6,    s.T1, 6    );
    mbedtls_p384_mul( s.T4,   s.T3,  6,    s.T1, 6    );
    mbedtls_p384_mul( s.T3,   s.T3,  6,    s.X,  s.Xn );
    mbedtls_p384_cop( s.T1,   s.T3                    );
    mbedtls_p384_shl( s.T1                            );
    mbedtls_p384_mul( s.X,    s.T2,  6,    s.T2, 6    );
    mbedtls_p384_hub(         s.X,         s.T1       );
    mbedtls_p384_hub(         s.X,         s.T4       );
    mbedtls_p384_hub(         s.T3,        s.X        );
    mbedtls_p384_mul( s.T3,   s.T3,  6,    s.T2, 6    );
    mbedtls_p384_mul( s.T4,   s.T4,  6,    s.Y,  s.Yn );
    mbedtls_p384_sub( s.Y,    s.T3,        s.T4       );
    mbedtls_p384_cop( R->X.p, s.X                     );
    mbedtls_p384_cop( R->Y.p, s.Y                     );
    mbedtls_p384_cop( R->Z.p, s.Z                     );
    mbedtls_platform_zeroize( &s, sizeof( s ) );
    return 0;
}

static int mbedtls_p384_inv_mod(mbedtls_mpi *X,
                                const mbedtls_mpi *A,
                                const mbedtls_mpi *N)
{
    int ret = MBEDTLS_ERR_THIS_CORRUPTION;
    mbedtls_mpi G, TA, TU, U1, U2, TB, TV, V1, V2;
    MBEDTLS_ASSERT( A->s == 1 );
    MBEDTLS_ASSERT( N->s == 1 );
    MBEDTLS_ASSERT( mbedtls_mpi_limbs( X ) <= 6 );
    MBEDTLS_ASSERT( mbedtls_mpi_limbs( A ) <= 6 );
    MBEDTLS_ASSERT( mbedtls_mpi_limbs( N ) <= 6 );
    MBEDTLS_ASSERT( mbedtls_mpi_cmp_int( N, 1 ) > 0 );
    mbedtls_mpi_init( &TA );
    mbedtls_mpi_init( &TU );
    mbedtls_mpi_init( &U1 );
    mbedtls_mpi_init( &U2 );
    mbedtls_mpi_init( &G  );
    mbedtls_mpi_init( &TB );
    mbedtls_mpi_init( &TV );
    mbedtls_mpi_init( &V1 );
    mbedtls_mpi_init( &V2 );
    MBEDTLS_MPI_CHK( mbedtls_mpi_grow( &TA,  7 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_grow( &TU,  7 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_grow( &U1,  7 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_grow( &U2,  7 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_grow( &G,   7 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_grow( &TB,  7 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_grow( &TV,  7 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_grow( &V1,  7 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_grow( &V2,  7 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_gcd( &G, A, N ) );
    if (!mbedtls_mpi_is_one( &G ))
    {
        ret = MBEDTLS_ERR_MPI_NOT_ACCEPTABLE;
        goto cleanup;
    }
    MBEDTLS_MPI_CHK( mbedtls_mpi_mod_mpi( &TA, A, N ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &TU, &TA ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &TB, N ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &TV, N ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_lset( &U1, 1 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_lset( &U2, 0 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_lset( &V1, 0 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_lset( &V2, 1 ) );
    do
    {
        MBEDTLS_ASSERT( mbedtls_mpi_limbs( &TU ) <= 7 );
        MBEDTLS_ASSERT( mbedtls_mpi_limbs( &U1 ) <= 7 );
        MBEDTLS_ASSERT( mbedtls_mpi_limbs( &U2 ) <= 7 );
        MBEDTLS_ASSERT( mbedtls_mpi_limbs( &TV ) <= 7 );
        MBEDTLS_ASSERT( mbedtls_mpi_limbs( &V2 ) <= 7 );
        MBEDTLS_ASSERT( mbedtls_mpi_limbs( &V1 ) <= 7 );
        MBEDTLS_ASSERT( mbedtls_mpi_limbs( &G  ) <= 7 );
        MBEDTLS_ASSERT( mbedtls_mpi_limbs( &TA ) <= 7 );
        MBEDTLS_ASSERT( mbedtls_mpi_limbs( &TB ) <= 7 );
        while ( !( TU.p[0] & 1 ) )
        {
            mbedtls_p384_sar( TU.p );
            if ((U1.p[0] & 1) || (U2.p[0] & 1))
            {
                MBEDTLS_MPI_CHK( mbedtls_mpi_add_mpi( &U1, &U1, &TB ) );
                MBEDTLS_MPI_CHK( mbedtls_mpi_sub_mpi( &U2, &U2, &TA ) );
            }
            mbedtls_p384_sar(U1.p);
            mbedtls_p384_sar(U2.p);
        }
        while ( !( TV.p[0] & 1 ) )
        {
            mbedtls_p384_sar(TV.p);
            if ((V1.p[0] & 1) || (V2.p[0] & 1))
            {
                MBEDTLS_MPI_CHK( mbedtls_mpi_add_mpi( &V1, &V1, &TB ) );
                MBEDTLS_MPI_CHK( mbedtls_mpi_sub_mpi( &V2, &V2, &TA ) );
            }
            mbedtls_p384_sar( V1.p );
            mbedtls_p384_sar( V2.p );
        }
        if (mbedtls_mpi_cmp_mpi( &TU, &TV ) >= 0)
        {
            MBEDTLS_MPI_CHK( mbedtls_mpi_sub_mpi( &TU, &TU, &TV ) );
            MBEDTLS_MPI_CHK( mbedtls_mpi_sub_mpi( &U1, &U1, &V1 ) );
            MBEDTLS_MPI_CHK( mbedtls_mpi_sub_mpi( &U2, &U2, &V2 ) );
        }
        else
        {
            MBEDTLS_MPI_CHK( mbedtls_mpi_sub_mpi( &TV, &TV, &TU ) );
            MBEDTLS_MPI_CHK( mbedtls_mpi_sub_mpi( &V1, &V1, &U1 ) );
            MBEDTLS_MPI_CHK( mbedtls_mpi_sub_mpi( &V2, &V2, &U2 ) );
        }
    } while ( TU.p[0] | TU.p[1] | TU.p[2] | TU.p[3] | TU.p[4] | TU.p[5] );
    MBEDTLS_ASSERT( mbedtls_mpi_limbs( &TU ) <= 7 );
    MBEDTLS_ASSERT( mbedtls_mpi_limbs( &U1 ) <= 7 );
    MBEDTLS_ASSERT( mbedtls_mpi_limbs( &U2 ) <= 7 );
    MBEDTLS_ASSERT( mbedtls_mpi_limbs( &TV ) <= 7 );
    MBEDTLS_ASSERT( mbedtls_mpi_limbs( &V2 ) <= 7 );
    MBEDTLS_ASSERT( mbedtls_mpi_limbs( &V1 ) <= 7 );
    MBEDTLS_ASSERT( mbedtls_mpi_limbs( &G  ) <= 7 );
    MBEDTLS_ASSERT( mbedtls_mpi_limbs( &TA ) <= 7 );
    MBEDTLS_ASSERT( mbedtls_mpi_limbs( &TB ) <= 7 );
    while (V1.s < 0)
        MBEDTLS_MPI_CHK( mbedtls_mpi_add_mpi( &V1, &V1, N ) );
    while (mbedtls_mpi_cmp_mpi( &V1, N ) >= 0)
        MBEDTLS_MPI_CHK( mbedtls_mpi_sub_mpi( &V1, &V1, N ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( X, &V1 ) );
cleanup:
    mbedtls_mpi_free( &TA );
    mbedtls_mpi_free( &TU );
    mbedtls_mpi_free( &U1 );
    mbedtls_mpi_free( &U2 );
    mbedtls_mpi_free( &G  );
    mbedtls_mpi_free( &TB );
    mbedtls_mpi_free( &TV );
    mbedtls_mpi_free( &V1 );
    mbedtls_mpi_free( &V2 );
    return ret;
}

int mbedtls_p384_normalize_jac_many( const mbedtls_ecp_group *grp,
                                     mbedtls_ecp_point *T[], size_t T_size )
{
    int ret = MBEDTLS_ERR_THIS_CORRUPTION;
    size_t i;
    uint64_t ta[12];
    mbedtls_mpi *c, u, Zi, ZZi;
    if( !( c = mbedtls_calloc( T_size, sizeof( mbedtls_mpi ) ) ) )
        return( MBEDTLS_ERR_ECP_ALLOC_FAILED );
    mbedtls_mpi_init( &u );
    mbedtls_mpi_init( &Zi );
    mbedtls_mpi_init( &ZZi );
    for( i = 0; i < T_size; i++ )
    {
        CHECK_EQ( 6, T[i]->X.n );
        CHECK_EQ( 6, T[i]->Y.n );
        CHECK_EQ( 6, T[i]->Z.n );
        mbedtls_mpi_init( c + i );
    }
    for( i = 0; i < T_size; i++ )
        MBEDTLS_MPI_CHK( mbedtls_mpi_grow( c + i, 12 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_grow( &u, 12 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_grow( &Zi, 12 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_grow( &ZZi, 12 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( c, &T[0]->Z ) );
    for( i = 1; i < T_size; i++ )
        mbedtls_p384_mul( c[i].p, c[i-1].p, 6, T[i]->Z.p, 6 );
    MBEDTLS_MPI_CHK( mbedtls_p384_inv_mod( &u, c + T_size - 1, &grp->P ) );
    for( i = T_size - 1; ; i-- )
    {
        if( !i )
            memcpy( Zi.p, u.p, 6 * 8 );
        else
        {
            mbedtls_p384_mul( Zi.p, u.p, 6, c[i-1].p,  6 );
            mbedtls_p384_mul( u.p,  u.p, 6, T[i]->Z.p, 6 );
        }
        mbedtls_p384_mul( ZZi.p,     Zi.p,      6, Zi.p,  6 );
        mbedtls_p384_mul( ta,        T[i]->X.p, 6, ZZi.p, 6 );
        memcpy(           T[i]->X.p, ta,        6 * 8       );
        mbedtls_p384_mul( ta,        T[i]->Y.p, 6, ZZi.p, 6 );
        mbedtls_p384_mul( ta,        ta,        6, Zi.p,  6 );
        memcpy(           T[i]->Y.p, ta,        6 * 8       );
        mbedtls_mpi_free( &T[i]->Z );
        if( !i ) break;
    }
cleanup:
    mbedtls_platform_zeroize( ta, sizeof( ta ) );
    for( i = 0; i < T_size; i++ )
        mbedtls_mpi_free( c + i );
    mbedtls_mpi_free( &ZZi );
    mbedtls_mpi_free( &Zi );
    mbedtls_mpi_free( &u );
    mbedtls_free( c );
    return( ret );
}
