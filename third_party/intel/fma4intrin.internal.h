#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _X86INTRIN_H_INCLUDED
# error "Never use <fma4intrin.h> directly; include <x86intrin.h> instead."
#endif
#ifndef _FMA4INTRIN_H_INCLUDED
#define _FMA4INTRIN_H_INCLUDED
#include "third_party/intel/ammintrin.internal.h"
#ifndef __FMA4__
#pragma GCC push_options
#pragma GCC target("fma4")
#define __DISABLE_FMA4__
#endif
extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_macc_ps (__m128 __A, __m128 __B, __m128 __C)
{
  return (__m128) __builtin_ia32_vfmaddps ((__v4sf)__A, (__v4sf)__B, (__v4sf)__C);
}
extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_macc_pd (__m128d __A, __m128d __B, __m128d __C)
{
  return (__m128d) __builtin_ia32_vfmaddpd ((__v2df)__A, (__v2df)__B, (__v2df)__C);
}
extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_macc_ss (__m128 __A, __m128 __B, __m128 __C)
{
  return (__m128) __builtin_ia32_vfmaddss ((__v4sf)__A, (__v4sf)__B, (__v4sf)__C);
}
extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_macc_sd (__m128d __A, __m128d __B, __m128d __C)
{
  return (__m128d) __builtin_ia32_vfmaddsd ((__v2df)__A, (__v2df)__B, (__v2df)__C);
}
extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_msub_ps (__m128 __A, __m128 __B, __m128 __C)
{
  return (__m128) __builtin_ia32_vfmaddps ((__v4sf)__A, (__v4sf)__B, -(__v4sf)__C);
}
extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_msub_pd (__m128d __A, __m128d __B, __m128d __C)
{
  return (__m128d) __builtin_ia32_vfmaddpd ((__v2df)__A, (__v2df)__B, -(__v2df)__C);
}
extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_msub_ss (__m128 __A, __m128 __B, __m128 __C)
{
  return (__m128) __builtin_ia32_vfmaddss ((__v4sf)__A, (__v4sf)__B, -(__v4sf)__C);
}
extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_msub_sd (__m128d __A, __m128d __B, __m128d __C)
{
  return (__m128d) __builtin_ia32_vfmaddsd ((__v2df)__A, (__v2df)__B, -(__v2df)__C);
}
extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_nmacc_ps (__m128 __A, __m128 __B, __m128 __C)
{
  return (__m128) __builtin_ia32_vfmaddps (-(__v4sf)__A, (__v4sf)__B, (__v4sf)__C);
}
extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_nmacc_pd (__m128d __A, __m128d __B, __m128d __C)
{
  return (__m128d) __builtin_ia32_vfmaddpd (-(__v2df)__A, (__v2df)__B, (__v2df)__C);
}
extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_nmacc_ss (__m128 __A, __m128 __B, __m128 __C)
{
  return (__m128) __builtin_ia32_vfmaddss (-(__v4sf)__A, (__v4sf)__B, (__v4sf)__C);
}
extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_nmacc_sd (__m128d __A, __m128d __B, __m128d __C)
{
  return (__m128d) __builtin_ia32_vfmaddsd (-(__v2df)__A, (__v2df)__B, (__v2df)__C);
}
extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_nmsub_ps (__m128 __A, __m128 __B, __m128 __C)
{
  return (__m128) __builtin_ia32_vfmaddps (-(__v4sf)__A, (__v4sf)__B, -(__v4sf)__C);
}
extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_nmsub_pd (__m128d __A, __m128d __B, __m128d __C)
{
  return (__m128d) __builtin_ia32_vfmaddpd (-(__v2df)__A, (__v2df)__B, -(__v2df)__C);
}
extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_nmsub_ss (__m128 __A, __m128 __B, __m128 __C)
{
  return (__m128) __builtin_ia32_vfmaddss (-(__v4sf)__A, (__v4sf)__B, -(__v4sf)__C);
}
extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_nmsub_sd (__m128d __A, __m128d __B, __m128d __C)
{
  return (__m128d) __builtin_ia32_vfmaddsd (-(__v2df)__A, (__v2df)__B, -(__v2df)__C);
}
extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_maddsub_ps (__m128 __A, __m128 __B, __m128 __C)
{
  return (__m128) __builtin_ia32_vfmaddsubps ((__v4sf)__A, (__v4sf)__B, (__v4sf)__C);
}
extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_maddsub_pd (__m128d __A, __m128d __B, __m128d __C)
{
  return (__m128d) __builtin_ia32_vfmaddsubpd ((__v2df)__A, (__v2df)__B, (__v2df)__C);
}
extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_msubadd_ps (__m128 __A, __m128 __B, __m128 __C)
{
  return (__m128) __builtin_ia32_vfmaddsubps ((__v4sf)__A, (__v4sf)__B, -(__v4sf)__C);
}
extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_msubadd_pd (__m128d __A, __m128d __B, __m128d __C)
{
  return (__m128d) __builtin_ia32_vfmaddsubpd ((__v2df)__A, (__v2df)__B, -(__v2df)__C);
}
extern __inline __m256 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_macc_ps (__m256 __A, __m256 __B, __m256 __C)
{
  return (__m256) __builtin_ia32_vfmaddps256 ((__v8sf)__A, (__v8sf)__B, (__v8sf)__C);
}
extern __inline __m256d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_macc_pd (__m256d __A, __m256d __B, __m256d __C)
{
  return (__m256d) __builtin_ia32_vfmaddpd256 ((__v4df)__A, (__v4df)__B, (__v4df)__C);
}
extern __inline __m256 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_msub_ps (__m256 __A, __m256 __B, __m256 __C)
{
  return (__m256) __builtin_ia32_vfmaddps256 ((__v8sf)__A, (__v8sf)__B, -(__v8sf)__C);
}
extern __inline __m256d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_msub_pd (__m256d __A, __m256d __B, __m256d __C)
{
  return (__m256d) __builtin_ia32_vfmaddpd256 ((__v4df)__A, (__v4df)__B, -(__v4df)__C);
}
extern __inline __m256 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_nmacc_ps (__m256 __A, __m256 __B, __m256 __C)
{
  return (__m256) __builtin_ia32_vfmaddps256 (-(__v8sf)__A, (__v8sf)__B, (__v8sf)__C);
}
extern __inline __m256d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_nmacc_pd (__m256d __A, __m256d __B, __m256d __C)
{
  return (__m256d) __builtin_ia32_vfmaddpd256 (-(__v4df)__A, (__v4df)__B, (__v4df)__C);
}
extern __inline __m256 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_nmsub_ps (__m256 __A, __m256 __B, __m256 __C)
{
  return (__m256) __builtin_ia32_vfmaddps256 (-(__v8sf)__A, (__v8sf)__B, -(__v8sf)__C);
}
extern __inline __m256d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_nmsub_pd (__m256d __A, __m256d __B, __m256d __C)
{
  return (__m256d) __builtin_ia32_vfmaddpd256 (-(__v4df)__A, (__v4df)__B, -(__v4df)__C);
}
extern __inline __m256 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maddsub_ps (__m256 __A, __m256 __B, __m256 __C)
{
  return (__m256) __builtin_ia32_vfmaddsubps256 ((__v8sf)__A, (__v8sf)__B, (__v8sf)__C);
}
extern __inline __m256d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maddsub_pd (__m256d __A, __m256d __B, __m256d __C)
{
  return (__m256d) __builtin_ia32_vfmaddsubpd256 ((__v4df)__A, (__v4df)__B, (__v4df)__C);
}
extern __inline __m256 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_msubadd_ps (__m256 __A, __m256 __B, __m256 __C)
{
  return (__m256) __builtin_ia32_vfmaddsubps256 ((__v8sf)__A, (__v8sf)__B, -(__v8sf)__C);
}
extern __inline __m256d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_msubadd_pd (__m256d __A, __m256d __B, __m256d __C)
{
  return (__m256d) __builtin_ia32_vfmaddsubpd256 ((__v4df)__A, (__v4df)__B, -(__v4df)__C);
}
#ifdef __DISABLE_FMA4__
#undef __DISABLE_FMA4__
#pragma GCC pop_options
#endif
#endif
#endif
