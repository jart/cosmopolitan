#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _IMMINTRIN_H_INCLUDED
#error "Never use <avx512pfintrin.h> directly; include <immintrin.h> instead."
#endif
#ifndef _AVX512PFINTRIN_H_INCLUDED
#define _AVX512PFINTRIN_H_INCLUDED
#ifndef __AVX512PF__
#pragma GCC push_options
#pragma GCC target("avx512pf")
#define __DISABLE_AVX512PF__
#endif
typedef long long __v8di __attribute__ ((__vector_size__ (64)));
typedef int __v16si __attribute__ ((__vector_size__ (64)));
typedef long long __m512i __attribute__ ((__vector_size__ (64), __may_alias__));
typedef unsigned char __mmask8;
typedef unsigned short __mmask16;
#ifdef __OPTIMIZE__
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm512_prefetch_i32gather_pd (__m256i __index, void const *__addr,
         int __scale, int __hint)
{
  __builtin_ia32_gatherpfdpd ((__mmask8) 0xFF, (__v8si) __index, __addr,
         __scale, __hint);
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm512_prefetch_i32gather_ps (__m512i __index, void const *__addr,
         int __scale, int __hint)
{
  __builtin_ia32_gatherpfdps ((__mmask16) 0xFFFF, (__v16si) __index, __addr,
         __scale, __hint);
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm512_mask_prefetch_i32gather_pd (__m256i __index, __mmask8 __mask,
       void const *__addr, int __scale, int __hint)
{
  __builtin_ia32_gatherpfdpd (__mask, (__v8si) __index, __addr, __scale,
         __hint);
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm512_mask_prefetch_i32gather_ps (__m512i __index, __mmask16 __mask,
       void const *__addr, int __scale, int __hint)
{
  __builtin_ia32_gatherpfdps (__mask, (__v16si) __index, __addr, __scale,
         __hint);
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm512_prefetch_i64gather_pd (__m512i __index, void const *__addr,
         int __scale, int __hint)
{
  __builtin_ia32_gatherpfqpd ((__mmask8) 0xFF, (__v8di) __index, __addr,
         __scale, __hint);
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm512_prefetch_i64gather_ps (__m512i __index, void const *__addr,
         int __scale, int __hint)
{
  __builtin_ia32_gatherpfqps ((__mmask8) 0xFF, (__v8di) __index, __addr,
         __scale, __hint);
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm512_mask_prefetch_i64gather_pd (__m512i __index, __mmask8 __mask,
       void const *__addr, int __scale, int __hint)
{
  __builtin_ia32_gatherpfqpd (__mask, (__v8di) __index, __addr, __scale,
         __hint);
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm512_mask_prefetch_i64gather_ps (__m512i __index, __mmask8 __mask,
       void const *__addr, int __scale, int __hint)
{
  __builtin_ia32_gatherpfqps (__mask, (__v8di) __index, __addr, __scale,
         __hint);
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm512_prefetch_i32scatter_pd (void *__addr, __m256i __index, int __scale,
          int __hint)
{
  __builtin_ia32_scatterpfdpd ((__mmask8) 0xFF, (__v8si) __index, __addr,
         __scale, __hint);
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm512_prefetch_i32scatter_ps (void *__addr, __m512i __index, int __scale,
          int __hint)
{
  __builtin_ia32_scatterpfdps ((__mmask16) 0xFFFF, (__v16si) __index, __addr,
         __scale, __hint);
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm512_mask_prefetch_i32scatter_pd (void *__addr, __mmask8 __mask,
        __m256i __index, int __scale, int __hint)
{
  __builtin_ia32_scatterpfdpd (__mask, (__v8si) __index, __addr, __scale,
          __hint);
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm512_mask_prefetch_i32scatter_ps (void *__addr, __mmask16 __mask,
        __m512i __index, int __scale, int __hint)
{
  __builtin_ia32_scatterpfdps (__mask, (__v16si) __index, __addr, __scale,
          __hint);
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm512_prefetch_i64scatter_pd (void *__addr, __m512i __index, int __scale,
          int __hint)
{
  __builtin_ia32_scatterpfqpd ((__mmask8) 0xFF, (__v8di) __index,__addr,
         __scale, __hint);
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm512_prefetch_i64scatter_ps (void *__addr, __m512i __index, int __scale,
          int __hint)
{
  __builtin_ia32_scatterpfqps ((__mmask8) 0xFF, (__v8di) __index, __addr,
         __scale, __hint);
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm512_mask_prefetch_i64scatter_pd (void *__addr, __mmask8 __mask,
        __m512i __index, int __scale, int __hint)
{
  __builtin_ia32_scatterpfqpd (__mask, (__v8di) __index, __addr, __scale,
          __hint);
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm512_mask_prefetch_i64scatter_ps (void *__addr, __mmask8 __mask,
        __m512i __index, int __scale, int __hint)
{
  __builtin_ia32_scatterpfqps (__mask, (__v8di) __index, __addr, __scale,
          __hint);
}
#else
#define _mm512_prefetch_i32gather_pd(INDEX, ADDR, SCALE, HINT) __builtin_ia32_gatherpfdpd ((__mmask8)0xFF, (__v8si)(__m256i) (INDEX), (void const *) (ADDR), (int) (SCALE), (int) (HINT))
#define _mm512_prefetch_i32gather_ps(INDEX, ADDR, SCALE, HINT) __builtin_ia32_gatherpfdps ((__mmask16)0xFFFF, (__v16si)(__m512i) (INDEX), (void const *) (ADDR), (int) (SCALE), (int) (HINT))
#define _mm512_mask_prefetch_i32gather_pd(INDEX, MASK, ADDR, SCALE, HINT) __builtin_ia32_gatherpfdpd ((__mmask8) (MASK), (__v8si)(__m256i) (INDEX), (void const *) (ADDR), (int) (SCALE), (int) (HINT))
#define _mm512_mask_prefetch_i32gather_ps(INDEX, MASK, ADDR, SCALE, HINT) __builtin_ia32_gatherpfdps ((__mmask16) (MASK), (__v16si)(__m512i) (INDEX), (void const *) (ADDR), (int) (SCALE), (int) (HINT))
#define _mm512_prefetch_i64gather_pd(INDEX, ADDR, SCALE, HINT) __builtin_ia32_gatherpfqpd ((__mmask8)0xFF, (__v8di)(__m512i) (INDEX), (void *) (ADDR), (int) (SCALE), (int) (HINT))
#define _mm512_prefetch_i64gather_ps(INDEX, ADDR, SCALE, HINT) __builtin_ia32_gatherpfqps ((__mmask8)0xFF, (__v8di)(__m512i) (INDEX), (void *) (ADDR), (int) (SCALE), (int) (HINT))
#define _mm512_mask_prefetch_i64gather_pd(INDEX, MASK, ADDR, SCALE, HINT) __builtin_ia32_gatherpfqpd ((__mmask8) (MASK), (__v8di)(__m512i) (INDEX), (void *) (ADDR), (int) (SCALE), (int) (HINT))
#define _mm512_mask_prefetch_i64gather_ps(INDEX, MASK, ADDR, SCALE, HINT) __builtin_ia32_gatherpfqps ((__mmask8) (MASK), (__v8di)(__m512i) (INDEX), (void *) (ADDR), (int) (SCALE), (int) (HINT))
#define _mm512_prefetch_i32scatter_pd(ADDR, INDEX, SCALE, HINT) __builtin_ia32_scatterpfdpd ((__mmask8)0xFF, (__v8si)(__m256i) (INDEX), (void *) (ADDR), (int) (SCALE), (int) (HINT))
#define _mm512_prefetch_i32scatter_ps(ADDR, INDEX, SCALE, HINT) __builtin_ia32_scatterpfdps ((__mmask16)0xFFFF, (__v16si)(__m512i) (INDEX), (void *) (ADDR), (int) (SCALE), (int) (HINT))
#define _mm512_mask_prefetch_i32scatter_pd(ADDR, MASK, INDEX, SCALE, HINT) __builtin_ia32_scatterpfdpd ((__mmask8) (MASK), (__v8si)(__m256i) (INDEX), (void *) (ADDR), (int) (SCALE), (int) (HINT))
#define _mm512_mask_prefetch_i32scatter_ps(ADDR, MASK, INDEX, SCALE, HINT) __builtin_ia32_scatterpfdps ((__mmask16) (MASK), (__v16si)(__m512i) (INDEX), (void *) (ADDR), (int) (SCALE), (int) (HINT))
#define _mm512_prefetch_i64scatter_pd(ADDR, INDEX, SCALE, HINT) __builtin_ia32_scatterpfqpd ((__mmask8)0xFF, (__v8di)(__m512i) (INDEX), (void *) (ADDR), (int) (SCALE), (int) (HINT))
#define _mm512_prefetch_i64scatter_ps(ADDR, INDEX, SCALE, HINT) __builtin_ia32_scatterpfqps ((__mmask8)0xFF, (__v8di)(__m512i) (INDEX), (void *) (ADDR), (int) (SCALE), (int) (HINT))
#define _mm512_mask_prefetch_i64scatter_pd(ADDR, MASK, INDEX, SCALE, HINT) __builtin_ia32_scatterpfqpd ((__mmask8) (MASK), (__v8di)(__m512i) (INDEX), (void *) (ADDR), (int) (SCALE), (int) (HINT))
#define _mm512_mask_prefetch_i64scatter_ps(ADDR, MASK, INDEX, SCALE, HINT) __builtin_ia32_scatterpfqps ((__mmask8) (MASK), (__v8di)(__m512i) (INDEX), (void *) (ADDR), (int) (SCALE), (int) (HINT))
#endif
#ifdef __DISABLE_AVX512PF__
#undef __DISABLE_AVX512PF__
#pragma GCC pop_options
#endif
#endif
#endif
