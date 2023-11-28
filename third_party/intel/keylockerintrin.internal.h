#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#if !defined _IMMINTRIN_H_INCLUDED
# error "Never use <keylockerintrin.h> directly; include <x86intrin.h> instead."
#endif
#ifndef _KEYLOCKERINTRIN_H_INCLUDED
#define _KEYLOCKERINTRIN_H_INCLUDED
#ifndef __KL__
#pragma GCC push_options
#pragma GCC target("kl")
#define __DISABLE_KL__
#endif
extern __inline
void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_loadiwkey (unsigned int __I, __m128i __A, __m128i __B, __m128i __C)
{
  __builtin_ia32_loadiwkey ((__v2di) __B, (__v2di) __C, (__v2di) __A, __I);
}
extern __inline
unsigned int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_encodekey128_u32 (unsigned int __I, __m128i __A, void * __P)
{
  return __builtin_ia32_encodekey128_u32 (__I, (__v2di)__A, __P);
}
extern __inline
unsigned int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_encodekey256_u32 (unsigned int __I, __m128i __A, __m128i __B, void * __P)
{
  return __builtin_ia32_encodekey256_u32 (__I, (__v2di)__A, (__v2di)__B, __P);
}
extern __inline
unsigned char __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_aesdec128kl_u8 (__m128i * __A, __m128i __B, const void * __P)
{
  return __builtin_ia32_aesdec128kl_u8 ((__v2di *) __A, (__v2di) __B, __P);
}
extern __inline
unsigned char __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_aesdec256kl_u8 (__m128i * __A, __m128i __B, const void * __P)
{
  return __builtin_ia32_aesdec256kl_u8 ((__v2di *) __A, (__v2di) __B, __P);
}
extern __inline
unsigned char __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_aesenc128kl_u8 (__m128i * __A, __m128i __B, const void * __P)
{
  return __builtin_ia32_aesenc128kl_u8 ((__v2di *) __A, (__v2di) __B, __P);
}
extern __inline
unsigned char __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_aesenc256kl_u8 (__m128i * __A, __m128i __B, const void * __P)
{
  return __builtin_ia32_aesenc256kl_u8 ((__v2di *) __A, (__v2di) __B, __P);
}
#ifdef __DISABLE_KL__
#undef __DISABLE_KL__
#pragma GCC pop_options
#endif
#ifndef __WIDEKL__
#pragma GCC push_options
#pragma GCC target("widekl")
#define __DISABLE_WIDEKL__
#endif
extern __inline
unsigned char __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_aesdecwide128kl_u8(__m128i __A[8], const __m128i __B[8], const void * __P)
{
  return __builtin_ia32_aesdecwide128kl_u8 ((__v2di *) __A, (__v2di *) __B, __P);
}
extern __inline
unsigned char __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_aesdecwide256kl_u8(__m128i __A[8], const __m128i __B[8], const void * __P)
{
  return __builtin_ia32_aesdecwide256kl_u8 ((__v2di *) __A, (__v2di *) __B, __P);
}
extern __inline
unsigned char __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_aesencwide128kl_u8(__m128i __A[8], const __m128i __B[8], const void * __P)
{
  return __builtin_ia32_aesencwide128kl_u8 ((__v2di *) __A, (__v2di *) __B, __P);
}
extern __inline
unsigned char __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_aesencwide256kl_u8(__m128i __A[8], const __m128i __B[8], const void * __P)
{
  return __builtin_ia32_aesencwide256kl_u8 ((__v2di *) __A, (__v2di *) __B, __P);
}
#ifdef __DISABLE_WIDEKL__
#undef __DISABLE_WIDEKL__
#pragma GCC pop_options
#endif
#endif
#endif
