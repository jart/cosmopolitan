#ifndef COSMOPOLITAN_LIBC_BITS_AVXINTRIN_H_
#define COSMOPOLITAN_LIBC_BITS_AVXINTRIN_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

typedef float __m256 _Vector_size(32) mayalias;
typedef double __m256d _Vector_size(32) mayalias;
typedef long long __m256i _Vector_size(32) mayalias;

typedef float __m256_u _Vector_size(32) forcealign(1) mayalias;
typedef double __m256d_u _Vector_size(32) forcealign(1) mayalias;
typedef long long __m256i_u _Vector_size(32) forcealign(1) mayalias;

typedef double __v4df _Vector_size(32);
typedef float __v8sf _Vector_size(32);
typedef long long __v4di _Vector_size(32);
typedef unsigned long long __v4du _Vector_size(32);
typedef int __v8si _Vector_size(32);
typedef unsigned __v8su _Vector_size(32);
typedef short __v16hi _Vector_size(32);
typedef unsigned short __v16hu _Vector_size(32);
typedef char __v32qi _Vector_size(32);
typedef unsigned char __v32qu _Vector_size(32);

#define _mm256_setzero_ps()       ((__m256)(__v8sf){0})
#define _mm256_load_ps(FLOATPTR)  (*(__m256 *)(FLOATPTR))
#define _mm256_loadu_ps(FLOATPTR) (*(__m256_u *)(FLOATPTR))
#define _mm256_store_ps(FLOATPTR, M256_0) \
  (*(__m256 *)(FLOATPTR) = (__m256)(M256_0))
#define _mm256_storeu_ps(FLOATPTR, M256_0) \
  (*(__m256_u *)(FLOATPTR) = (__m256)(M256_0))
#define _mm256_extractf128_ps(M256_0, INT_1)                           \
  ((__m128)__builtin_ia32_vextractf128_ps256((__v8sf)(__m256)(M256_0), \
                                             (int)(INT_1)))
#define _mm256_insertf128_ps(M256_0, M128_1, IMM_2) \
  ((__m256)__builtin_ia32_vinsertf128_ps256(        \
      (__v8sf)(__m256)(M256_0), (__v4sf)(__m128)(M128_1), (int)(IMM_2)))

#ifdef __llvm__
#define _mm256_castps128_ps256(M128_0)                                       \
  ((__m256)__builtin_shufflevector((__v4sf)(__m128)(M128_0),                 \
                                   (__v4sf)(__m128)(M128_0), 0, 1, 2, 3, -1, \
                                   -1, -1, -1))
#else
#define _mm256_castps128_ps256(M128_0) \
  ((__m256)__builtin_ia32_ps256_ps((__v4sf)(__m128)(M128_0)))
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_AVXINTRIN_H_ */
