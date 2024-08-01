#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _IMMINTRIN_H_INCLUDED
#error "Never use <avx512vnniintrin.h> directly; include <immintrin.h> instead."
#endif
#ifndef __AVX512VNNIINTRIN_H_INCLUDED
#define __AVX512VNNIINTRIN_H_INCLUDED
#if !defined(__AVX512VNNI__) || !defined (__EVEX512__)
#pragma GCC push_options
#pragma GCC target("avx512vnni,evex512")
#define __DISABLE_AVX512VNNI__
#endif
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_dpbusd_epi32 (__m512i __A, __m512i __B, __m512i __C)
{
  return (__m512i) __builtin_ia32_vpdpbusd_v16si ((__v16si)__A, (__v16si) __B,
        (__v16si) __C);
}
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_mask_dpbusd_epi32 (__m512i __A, __mmask16 __B, __m512i __C, __m512i __D)
{
  return (__m512i)__builtin_ia32_vpdpbusd_v16si_mask ((__v16si)__A,
    (__v16si) __C, (__v16si) __D, (__mmask16)__B);
}
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_maskz_dpbusd_epi32 (__mmask16 __A, __m512i __B, __m512i __C,
       __m512i __D)
{
  return (__m512i)__builtin_ia32_vpdpbusd_v16si_maskz ((__v16si)__B,
    (__v16si) __C, (__v16si) __D, (__mmask16)__A);
}
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_dpbusds_epi32 (__m512i __A, __m512i __B, __m512i __C)
{
  return (__m512i) __builtin_ia32_vpdpbusds_v16si ((__v16si)__A, (__v16si) __B,
        (__v16si) __C);
}
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_mask_dpbusds_epi32 (__m512i __A, __mmask16 __B, __m512i __C,
       __m512i __D)
{
  return (__m512i)__builtin_ia32_vpdpbusds_v16si_mask ((__v16si)__A,
    (__v16si) __C, (__v16si) __D, (__mmask16)__B);
}
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_maskz_dpbusds_epi32 (__mmask16 __A, __m512i __B, __m512i __C,
       __m512i __D)
{
  return (__m512i)__builtin_ia32_vpdpbusds_v16si_maskz ((__v16si)__B,
    (__v16si) __C, (__v16si) __D, (__mmask16)__A);
}
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_dpwssd_epi32 (__m512i __A, __m512i __B, __m512i __C)
{
  return (__m512i) __builtin_ia32_vpdpwssd_v16si ((__v16si)__A, (__v16si) __B,
        (__v16si) __C);
}
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_mask_dpwssd_epi32 (__m512i __A, __mmask16 __B, __m512i __C, __m512i __D)
{
  return (__m512i)__builtin_ia32_vpdpwssd_v16si_mask ((__v16si)__A,
    (__v16si) __C, (__v16si) __D, (__mmask16)__B);
}
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_maskz_dpwssd_epi32 (__mmask16 __A, __m512i __B, __m512i __C,
       __m512i __D)
{
  return (__m512i)__builtin_ia32_vpdpwssd_v16si_maskz ((__v16si)__B,
    (__v16si) __C, (__v16si) __D, (__mmask16)__A);
}
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_dpwssds_epi32 (__m512i __A, __m512i __B, __m512i __C)
{
  return (__m512i) __builtin_ia32_vpdpwssds_v16si ((__v16si)__A, (__v16si) __B,
        (__v16si) __C);
}
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_mask_dpwssds_epi32 (__m512i __A, __mmask16 __B, __m512i __C,
       __m512i __D)
{
  return (__m512i)__builtin_ia32_vpdpwssds_v16si_mask ((__v16si)__A,
    (__v16si) __C, (__v16si) __D, (__mmask16)__B);
}
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_maskz_dpwssds_epi32 (__mmask16 __A, __m512i __B, __m512i __C,
       __m512i __D)
{
  return (__m512i)__builtin_ia32_vpdpwssds_v16si_maskz ((__v16si)__B,
    (__v16si) __C, (__v16si) __D, (__mmask16)__A);
}
#ifdef __DISABLE_AVX512VNNI__
#undef __DISABLE_AVX512VNNI__
#pragma GCC pop_options
#endif
#endif
#endif
