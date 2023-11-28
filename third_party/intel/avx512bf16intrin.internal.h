#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _IMMINTRIN_H_INCLUDED
#error "Never use <avx512bf16intrin.h> directly; include <immintrin.h> instead."
#endif
#ifndef _AVX512BF16INTRIN_H_INCLUDED
#define _AVX512BF16INTRIN_H_INCLUDED
#ifndef __AVX512BF16__
#pragma GCC push_options
#pragma GCC target("avx512bf16")
#define __DISABLE_AVX512BF16__
#endif
typedef short __v32bh __attribute__ ((__vector_size__ (64)));
typedef short __m512bh __attribute__ ((__vector_size__ (64), __may_alias__));
extern __inline __m512bh
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_cvtne2ps_pbh (__m512 __A, __m512 __B)
{
  return (__m512bh)__builtin_ia32_cvtne2ps2bf16_v32hi(__A, __B);
}
extern __inline __m512bh
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_mask_cvtne2ps_pbh (__m512bh __A, __mmask32 __B, __m512 __C, __m512 __D)
{
  return (__m512bh)__builtin_ia32_cvtne2ps2bf16_v32hi_mask(__C, __D, __A, __B);
}
extern __inline __m512bh
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_maskz_cvtne2ps_pbh (__mmask32 __A, __m512 __B, __m512 __C)
{
  return (__m512bh)__builtin_ia32_cvtne2ps2bf16_v32hi_maskz(__B, __C, __A);
}
extern __inline __m256bh
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_cvtneps_pbh (__m512 __A)
{
  return (__m256bh)__builtin_ia32_cvtneps2bf16_v16sf(__A);
}
extern __inline __m256bh
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_mask_cvtneps_pbh (__m256bh __A, __mmask16 __B, __m512 __C)
{
  return (__m256bh)__builtin_ia32_cvtneps2bf16_v16sf_mask(__C, __A, __B);
}
extern __inline __m256bh
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_maskz_cvtneps_pbh (__mmask16 __A, __m512 __B)
{
  return (__m256bh)__builtin_ia32_cvtneps2bf16_v16sf_maskz(__B, __A);
}
extern __inline __m512
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_dpbf16_ps (__m512 __A, __m512bh __B, __m512bh __C)
{
  return (__m512)__builtin_ia32_dpbf16ps_v16sf(__A, __B, __C);
}
extern __inline __m512
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_mask_dpbf16_ps (__m512 __A, __mmask16 __B, __m512bh __C, __m512bh __D)
{
  return (__m512)__builtin_ia32_dpbf16ps_v16sf_mask(__A, __C, __D, __B);
}
extern __inline __m512
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_maskz_dpbf16_ps (__mmask16 __A, __m512 __B, __m512bh __C, __m512bh __D)
{
  return (__m512)__builtin_ia32_dpbf16ps_v16sf_maskz(__B, __C, __D, __A);
}
#ifdef __DISABLE_AVX512BF16__
#undef __DISABLE_AVX512BF16__
#pragma GCC pop_options
#endif
#endif
#endif
