#ifndef COSMOPOLITAN_LIBC_BITS_XMMINTRIN_H_
#define COSMOPOLITAN_LIBC_BITS_XMMINTRIN_H_
#include "libc/bits/emmintrin.internal.h"
#include "libc/bits/progn.internal.h"
#include "libc/dce.h"

#define _MM_EXCEPT_MASK       0x003f
#define _MM_EXCEPT_INVALID    0x0001
#define _MM_EXCEPT_DENORM     0x0002
#define _MM_EXCEPT_DIV_ZERO   0x0004
#define _MM_EXCEPT_OVERFLOW   0x0008
#define _MM_EXCEPT_UNDERFLOW  0x0010
#define _MM_EXCEPT_INEXACT    0x0020
#define _MM_MASK_MASK         0x1f80
#define _MM_MASK_INVALID      0x0080
#define _MM_MASK_DENORM       0x0100
#define _MM_MASK_DIV_ZERO     0x0200
#define _MM_MASK_OVERFLOW     0x0400
#define _MM_MASK_UNDERFLOW    0x0800
#define _MM_MASK_INEXACT      0x1000
#define _MM_ROUND_MASK        0x6000
#define _MM_ROUND_NEAREST     0x0000
#define _MM_ROUND_DOWN        0x2000
#define _MM_ROUND_UP          0x4000
#define _MM_ROUND_TOWARD_ZERO 0x6000
#define _MM_FLUSH_ZERO_MASK   0x8000
#define _MM_FLUSH_ZERO_ON     0x8000
#define _MM_FLUSH_ZERO_OFF    0x0000

#define _MM_SHUFFLE(A, B, C, D) (((A) << 6) | ((B) << 4) | ((C) << 2) | (D))

#if !(__ASSEMBLER__ + __LINKER__ + 0)

typedef int __v4si _Vector_size(16);
typedef unsigned int __v4su _Vector_size(16);
typedef float __v4sf _Vector_size(16);
typedef float __m128 _Vector_size(16) forcealign(16) mayalias;
typedef float __m128_u _Vector_size(16) forcealign(1) mayalias;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § it's a trap! » sse » simd ops                             ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define _mm_add_ps(M128_0, M128_1) \
  ((__m128)((__v4sf)(M128_0) + (__v4sf)(M128_1)))
#define _mm_sub_ps(M128_0, M128_1) \
  ((__m128)((__v4sf)(M128_0) - (__v4sf)(M128_1)))
#define _mm_mul_ps(M128_0, M128_1) \
  ((__m128)((__v4sf)(M128_0) * (__v4sf)(M128_1)))
#define _mm_div_ps(M128_0, M128_1) \
  ((__m128)((__v4sf)(M128_0) / (__v4sf)(M128_1)))
#define _mm_and_ps(M128_0, M128_1) \
  ((__m128)((__v4su)(M128_0) & (__v4su)(M128_1)))
#define _mm_or_ps(M128_0, M128_1) \
  ((__m128)((__v4su)(M128_0) | (__v4su)(M128_1)))
#define _mm_xor_ps(M128_0, M128_1) /* XORPD [u32 simd xor] */ \
  ((__m128)((__v4su)(M128_0) ^ (__v4su)(M128_1)))
#define _mm_andnot_ps(M128_0, M128_1) /* ANDNPS [u32 simd nand] */ \
  ((__m128)(~(__v4su)(M128_0) & (__v4su)(M128_1)))
#define _mm_rcp_ps(M128)   __builtin_ia32_rcpps((__v4sf)(M128))
#define _mm_sqrt_ps(M128)  __builtin_ia32_sqrtps((__v4sf)(M128))
#define _mm_rsqrt_ps(M128) __builtin_ia32_rsqrtps((__v4sf)(M128))

#define _mm_min_ps(M128_0, M128_1) \
  __builtin_ia32_minps((__v4sf)(M128_0), (__v4sf)(M128_1))
#define _mm_max_ps(M128_0, M128_1) \
  __builtin_ia32_maxps((__v4sf)(M128_0), (__v4sf)(M128_1))
#define _mm_min_ss(M128_0, M128_1) \
  __builtin_ia32_minss((__v4sf)(M128_0), (__v4sf)(M128_1))
#define _mm_max_ss(M128_0, M128_1) \
  __builtin_ia32_maxss((__v4sf)(M128_0), (__v4sf)(M128_1))
#define _mm_cmpeq_ps(M128_0, M128_1) \
  __builtin_ia32_cmpeqps((__v4sf)(M128_0), (__v4sf)(M128_1))
#define _mm_cmpneq_ps(M128_0, M128_1) \
  __builtin_ia32_cmpneqps((__v4sf)(M128_0), (__v4sf)(M128_1))
#define _mm_cmplt_ps(M128_0, M128_1) \
  __builtin_ia32_cmpltps((__v4sf)(M128_0), (__v4sf)(M128_1))
#define _mm_cmpnlt_ps(M128_0, M128_1) \
  __builtin_ia32_cmpnltps((__v4sf)(M128_0), (__v4sf)(M128_1))
#define _mm_cmple_ps(M128_0, M128_1) \
  __builtin_ia32_cmpleps((__v4sf)(M128_0), (__v4sf)(M128_1))
#define _mm_cmpnle_ps(M128_0, M128_1) \
  __builtin_ia32_cmpnleps((__v4sf)(M128_0), (__v4sf)(M128_1))
#define _mm_cmpgt_ps(M128_0, M128_1) \
  __builtin_ia32_cmpltps((__v4sf)(M128_1), (__v4sf)(M128_0))
#define _mm_cmpngt_ps(M128_0, M128_1) \
  __builtin_ia32_cmpnltps((__v4sf)(M128_1), (__v4sf)(M128_0))
#define _mm_cmpge_ps(M128_0, M128_1) \
  __builtin_ia32_cmpleps((__v4sf)(M128_1), (__v4sf)(M128_0))
#define _mm_cmpnge_ps(M128_0, M128_1) \
  __builtin_ia32_cmpnleps((__v4sf)(M128_1), (__v4sf)(M128_0))
#define _mm_cmpord_ps(M128_0, M128_1) \
  __builtin_ia32_cmpordps((__v4sf)(M128_0), (__v4sf)(M128_1))
#define _mm_cmpunord_ps(M128_0, M128_1) \
  __builtin_ia32_cmpunordps((__v4sf)(M128_0), (__v4sf)(M128_1))

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § it's a trap! » sse » scalar ops                           ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define _mm_add_ss(m128_0, m128_1) \
  ({                               \
    __m128 a = m128_0;             \
    __m128 b = m128_0;             \
    a[0] += b[0];                  \
    a;                             \
  })

#define _mm_sub_ss(m128_0, m128_1) \
  ({                               \
    __m128 a = m128_0;             \
    __m128 b = m128_0;             \
    a[0] -= b[0];                  \
    a;                             \
  })

#define _mm_mul_ss(m128_0, m128_1) \
  ({                               \
    __m128 a = m128_0;             \
    __m128 b = m128_0;             \
    a[0] *= b[0];                  \
    a;                             \
  })

#define _mm_div_ss(m128_0, m128_1) \
  ({                               \
    __m128 a = m128_0;             \
    __m128 b = m128_0;             \
    a[0] /= b[0];                  \
    a;                             \
  })

#define _mm_rcp_ss(M128)   __builtin_ia32_rcpss((__v4sf)(M128))   /*~1/x*/
#define _mm_sqrt_ss(M128)  __builtin_ia32_sqrtss((__v4sf)(M128))  /*sqrt𝑥*/
#define _mm_rsqrt_ss(M128) __builtin_ia32_rsqrtss((__v4sf)(M128)) /*~1/sqrt𝑥*/

#define _mm_min_ss(M128_0, M128_1) \
  __builtin_ia32_minss((__v4sf)(M128_0), (__v4sf)(M128_1))
#define _mm_max_ss(M128_0, M128_1) \
  __builtin_ia32_maxss((__v4sf)(M128_0), (__v4sf)(M128_1))
#define _mm_cmpeq_ss(M128_0, M128_1) \
  __builtin_ia32_cmpeqss((__v4sf)(M128_0), (__v4sf)(M128_1))
#define _mm_cmpneq_ss(M128_0, M128_1) \
  __builtin_ia32_cmpneqss((__v4sf)(M128_0), (__v4sf)(M128_1))
#define _mm_cmplt_ss(M128_0, M128_1) \
  __builtin_ia32_cmpltss((__v4sf)(M128_0), (__v4sf)(M128_1))
#define _mm_cmpnlt_ss(M128_0, M128_1) \
  __builtin_ia32_cmpnltss((__v4sf)(M128_0), (__v4sf)(M128_1))
#define _mm_cmple_ss(M128_0, M128_1) \
  __builtin_ia32_cmpless((__v4sf)(M128_0), (__v4sf)(M128_1))
#define _mm_cmpnle_ss(M128_0, M128_1) \
  __builtin_ia32_cmpnless((__v4sf)(M128_0), (__v4sf)(M128_1))
#define _mm_cmpgt_ss(M128_0, M128_1) \
  __builtin_ia32_cmpltss((__v4sf)(M128_1), (__v4sf)(M128_0))
#define _mm_cmpngt_ss(M128_0, M128_1) \
  __builtin_ia32_cmpnltss((__v4sf)(M128_1), (__v4sf)(M128_0))
#define _mm_cmpge_ss(M128_0, M128_1) \
  __builtin_ia32_cmpless((__v4sf)(M128_1), (__v4sf)(M128_0))
#define _mm_cmpnge_ss(M128_0, M128_1) \
  __builtin_ia32_cmpnless((__v4sf)(M128_1), (__v4sf)(M128_0))
#define _mm_cmpord_ss(M128_0, M128_1) \
  __builtin_ia32_cmpordss((__v4sf)(M128_0), (__v4sf)(M128_1))
#define _mm_cmpunord_ss(M128_0, M128_1) \
  __builtin_ia32_cmpunordss((__v4sf)(M128_0), (__v4sf)(M128_1))

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § it's a trap! » sse » memory ops                           ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define _mm_set1_ps(M128_0)    ((__m128)(__v4sf){M128_0, M128_0, M128_0, M128_0})
#define _mm_setzero_ps()       ((__m128)(__v4sf){0})
#define _mm_cvtss_f32(M128_0)  (((__v4sf)(M128_0))[0])
#define _mm_load_ps(FLOATPTR)  (*(__m128 *)(FLOATPTR))
#define _mm_loadu_ps(FLOATPTR) (*(__m128_u *)(FLOATPTR))
#define _mm_set_ps(WHO, DESIGNED, THIS, SHEESH) \
  ((__m128)(__v4sf){SHEESH, THIS, DESIGNED, WHO})
#define _mm_set_ss(FLOAT)               ((__m128)(__v4sf){FLOAT, 0, 0, 0})
#define _mm_load_ss(FLOATPTR)           _mm_set_ss(*(FLOATPTR))
#define _mm_store_ss(FLOATPTR, M128_0)  ((FLOATPTR)[0] = ((__v4sf)(M128_0))[0])
#define _mm_store_ps(FLOATPTR, M128_0)  (*(__m128 *)(FLOATPTR) = (M128_0))
#define _mm_storeu_ps(FLOATPTR, M128_0) (*(__m128_u *)(FLOATPTR) = (M128_0))
#define _mm_shuffle_ps(M128_0, M128_1, MASK) \
  ((__m128)__builtin_ia32_shufps((__v4sf)(M128_0), (__v4sf)(M128_1), (MASK)))

#ifdef __llvm__
#define _mm_movehl_ps(M128_0, M128_1)                        \
  ((__m128)__builtin_shufflevector((__v4sf)(__m128)(M128_0), \
                                   (__v4sf)(__m128)(M128_1), 6, 7, 2, 3))
/* instrinsics unstable & constantly breaking, consider ansi c or asm. */
/* each version of llvm has a different incompatible impl for this one */
#else
#define _mm_movehl_ps(M128_0, M128_1)                       \
  ((__m128)__builtin_ia32_movhlps((__v4sf)(__m128)(M128_0), \
                                  (__v4sf)(__m128)(M128_1)))
#define _mm_storel_pi(M64PTR, M128_0) \
  __builtin_ia32_storelps((__v2sf *)(M64PTR), (__v4sf)(M128_0))
#endif

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § it's a trap! » sse » cast ops                             ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define _mm_cvtps_epi32(M128_0) \
  ((__m128i)__builtin_ia32_cvtps2dq((__v4sf)(M128_0)))

#ifdef __llvm__
#define _mm_cvtepi32_ps(M128I_0) \
  ((__m128) __builtin_convertvector((__v4si)(__m128i)(M128I_0), __v4sf))
#else
#define _mm_cvtepi32_ps(M128I_0) \
  ((__m128)__builtin_ia32_cvtdq2ps((__v4si)(M128I_0)))
#endif

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § it's a trap! » sse » misc                                 ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define _mm_getcsr()        (__builtin_ia32_stmxcsr())
#define _mm_setcsr(U32CONF) (__builtin_ia32_ldmxcsr(U32CONF))

#define _MM_GET_ROUNDING_MODE() (_mm_getcsr() & _MM_ROUND_MASK)
#define _MM_SET_ROUNDING_MODE(MODE) \
  (_mm_setcsr((_mm_getcsr() & ~_MM_ROUND_MASK) | (MODE)))

#define XMM_DESTROY(VAR)                                   \
  do {                                                     \
    if (!IsTrustworthy()) {                                \
      asm volatile("xorps\t%1,%0" : "=x"(VAR) : "0"(VAR)); \
    }                                                      \
  } while (0)

/*
** Ternary:
**
** Integer: _mm_or_si128(_mm_and_si128(a, cond), _mm_andnot_si128(cond, b))
** 32-bit float: _mm_or_ps(_mm_and_ps(a, cond), _mm_andnot_ps(cond, b))
** 64-bit float: _mm_or_pd(_mm_and_pd(a, cond), _mm_andnot_pd(cond, b))
** Integer (SSE4.1+): _mm_blendv_epi8(a, b, cond)
** 32-bit float (SSE4.1+): _mm_blendv_ps(a, b, cond)
** 64-bit float (SSE4.1+): _mm_blendv_pd(a, b, cond)
*/

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_XMMINTRIN_H_ */
