#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#if !defined _X86INTRIN_H_INCLUDED && !defined _IMMINTRIN_H_INCLUDED
# error "Never use <f16intrin.h> directly; include <x86intrin.h> or <immintrin.h> instead."
#endif
#ifndef _F16CINTRIN_H_INCLUDED
#define _F16CINTRIN_H_INCLUDED
#ifndef __F16C__
#pragma GCC push_options
#pragma GCC target("f16c")
#define __DISABLE_F16C__
#endif
extern __inline float __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_cvtsh_ss (unsigned short __S)
{
  __v8hi __H = __extension__ (__v8hi){ (short) __S, 0, 0, 0, 0, 0, 0, 0 };
  __v4sf __A = __builtin_ia32_vcvtph2ps (__H);
  return __builtin_ia32_vec_ext_v4sf (__A, 0);
}
extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtph_ps (__m128i __A)
{
  return (__m128) __builtin_ia32_vcvtph2ps ((__v8hi) __A);
}
extern __inline __m256 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cvtph_ps (__m128i __A)
{
  return (__m256) __builtin_ia32_vcvtph2ps256 ((__v8hi) __A);
}
#ifdef __OPTIMIZE__
extern __inline unsigned short __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_cvtss_sh (float __F, const int __I)
{
  __v4sf __A = __extension__ (__v4sf){ __F, 0, 0, 0 };
  __v8hi __H = __builtin_ia32_vcvtps2ph (__A, __I);
  return (unsigned short) __builtin_ia32_vec_ext_v8hi (__H, 0);
}
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtps_ph (__m128 __A, const int __I)
{
  return (__m128i) __builtin_ia32_vcvtps2ph ((__v4sf) __A, __I);
}
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cvtps_ph (__m256 __A, const int __I)
{
  return (__m128i) __builtin_ia32_vcvtps2ph256 ((__v8sf) __A, __I);
}
#else
#define _cvtss_sh(__F, __I) (__extension__ ({ __v4sf __A = __extension__ (__v4sf){ __F, 0, 0, 0 }; __v8hi __H = __builtin_ia32_vcvtps2ph (__A, __I); (unsigned short) __builtin_ia32_vec_ext_v8hi (__H, 0); }))
#define _mm_cvtps_ph(A, I) ((__m128i) __builtin_ia32_vcvtps2ph ((__v4sf)(__m128) (A), (int) (I)))
#define _mm256_cvtps_ph(A, I) ((__m128i) __builtin_ia32_vcvtps2ph256 ((__v8sf)(__m256) (A), (int) (I)))
#endif
#ifdef __DISABLE_F16C__
#undef __DISABLE_F16C__
#pragma GCC pop_options
#endif
#endif
#endif
