#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _IMMINTRIN_H_INCLUDED
# error "Never use <fmaintrin.h> directly; include <immintrin.h> instead."
#endif
#ifndef _FMAINTRIN_H_INCLUDED
#define _FMAINTRIN_H_INCLUDED
#ifndef __FMA__
#pragma GCC push_options
#pragma GCC target("fma")
#define __DISABLE_FMA__
#endif
extern __inline __m128d
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_fmadd_pd (__m128d __A, __m128d __B, __m128d __C)
{
  return (__m128d)__builtin_ia32_vfmaddpd ((__v2df)__A, (__v2df)__B,
                                           (__v2df)__C);
}
extern __inline __m256d
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_fmadd_pd (__m256d __A, __m256d __B, __m256d __C)
{
  return (__m256d)__builtin_ia32_vfmaddpd256 ((__v4df)__A, (__v4df)__B,
                                              (__v4df)__C);
}
extern __inline __m128
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_fmadd_ps (__m128 __A, __m128 __B, __m128 __C)
{
  return (__m128)__builtin_ia32_vfmaddps ((__v4sf)__A, (__v4sf)__B,
                                          (__v4sf)__C);
}
extern __inline __m256
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_fmadd_ps (__m256 __A, __m256 __B, __m256 __C)
{
  return (__m256)__builtin_ia32_vfmaddps256 ((__v8sf)__A, (__v8sf)__B,
                                             (__v8sf)__C);
}
extern __inline __m128d
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_fmadd_sd (__m128d __A, __m128d __B, __m128d __C)
{
  return (__m128d) __builtin_ia32_vfmaddsd3 ((__v2df)__A, (__v2df)__B,
                                             (__v2df)__C);
}
extern __inline __m128
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_fmadd_ss (__m128 __A, __m128 __B, __m128 __C)
{
  return (__m128) __builtin_ia32_vfmaddss3 ((__v4sf)__A, (__v4sf)__B,
                                            (__v4sf)__C);
}
extern __inline __m128d
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_fmsub_pd (__m128d __A, __m128d __B, __m128d __C)
{
  return (__m128d)__builtin_ia32_vfmsubpd ((__v2df)__A, (__v2df)__B,
                                           (__v2df)__C);
}
extern __inline __m256d
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_fmsub_pd (__m256d __A, __m256d __B, __m256d __C)
{
  return (__m256d)__builtin_ia32_vfmsubpd256 ((__v4df)__A, (__v4df)__B,
                                              (__v4df)__C);
}
extern __inline __m128
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_fmsub_ps (__m128 __A, __m128 __B, __m128 __C)
{
  return (__m128)__builtin_ia32_vfmsubps ((__v4sf)__A, (__v4sf)__B,
                                          (__v4sf)__C);
}
extern __inline __m256
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_fmsub_ps (__m256 __A, __m256 __B, __m256 __C)
{
  return (__m256)__builtin_ia32_vfmsubps256 ((__v8sf)__A, (__v8sf)__B,
                                             (__v8sf)__C);
}
extern __inline __m128d
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_fmsub_sd (__m128d __A, __m128d __B, __m128d __C)
{
  return (__m128d)__builtin_ia32_vfmsubsd3 ((__v2df)__A, (__v2df)__B,
                                            (__v2df)__C);
}
extern __inline __m128
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_fmsub_ss (__m128 __A, __m128 __B, __m128 __C)
{
  return (__m128)__builtin_ia32_vfmsubss3 ((__v4sf)__A, (__v4sf)__B,
                                           (__v4sf)__C);
}
extern __inline __m128d
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_fnmadd_pd (__m128d __A, __m128d __B, __m128d __C)
{
  return (__m128d)__builtin_ia32_vfnmaddpd ((__v2df)__A, (__v2df)__B,
         (__v2df)__C);
}
extern __inline __m256d
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_fnmadd_pd (__m256d __A, __m256d __B, __m256d __C)
{
  return (__m256d)__builtin_ia32_vfnmaddpd256 ((__v4df)__A, (__v4df)__B,
            (__v4df)__C);
}
extern __inline __m128
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_fnmadd_ps (__m128 __A, __m128 __B, __m128 __C)
{
  return (__m128)__builtin_ia32_vfnmaddps ((__v4sf)__A, (__v4sf)__B,
        (__v4sf)__C);
}
extern __inline __m256
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_fnmadd_ps (__m256 __A, __m256 __B, __m256 __C)
{
  return (__m256)__builtin_ia32_vfnmaddps256 ((__v8sf)__A, (__v8sf)__B,
           (__v8sf)__C);
}
extern __inline __m128d
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_fnmadd_sd (__m128d __A, __m128d __B, __m128d __C)
{
  return (__m128d)__builtin_ia32_vfnmaddsd3 ((__v2df)__A, (__v2df)__B,
          (__v2df)__C);
}
extern __inline __m128
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_fnmadd_ss (__m128 __A, __m128 __B, __m128 __C)
{
  return (__m128)__builtin_ia32_vfnmaddss3 ((__v4sf)__A, (__v4sf)__B,
         (__v4sf)__C);
}
extern __inline __m128d
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_fnmsub_pd (__m128d __A, __m128d __B, __m128d __C)
{
  return (__m128d)__builtin_ia32_vfnmsubpd ((__v2df)__A, (__v2df)__B,
         (__v2df)__C);
}
extern __inline __m256d
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_fnmsub_pd (__m256d __A, __m256d __B, __m256d __C)
{
  return (__m256d)__builtin_ia32_vfnmsubpd256 ((__v4df)__A, (__v4df)__B,
            (__v4df)__C);
}
extern __inline __m128
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_fnmsub_ps (__m128 __A, __m128 __B, __m128 __C)
{
  return (__m128)__builtin_ia32_vfnmsubps ((__v4sf)__A, (__v4sf)__B,
        (__v4sf)__C);
}
extern __inline __m256
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_fnmsub_ps (__m256 __A, __m256 __B, __m256 __C)
{
  return (__m256)__builtin_ia32_vfnmsubps256 ((__v8sf)__A, (__v8sf)__B,
           (__v8sf)__C);
}
extern __inline __m128d
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_fnmsub_sd (__m128d __A, __m128d __B, __m128d __C)
{
  return (__m128d)__builtin_ia32_vfnmsubsd3 ((__v2df)__A, (__v2df)__B,
          (__v2df)__C);
}
extern __inline __m128
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_fnmsub_ss (__m128 __A, __m128 __B, __m128 __C)
{
  return (__m128)__builtin_ia32_vfnmsubss3 ((__v4sf)__A, (__v4sf)__B,
         (__v4sf)__C);
}
extern __inline __m128d
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_fmaddsub_pd (__m128d __A, __m128d __B, __m128d __C)
{
  return (__m128d)__builtin_ia32_vfmaddsubpd ((__v2df)__A, (__v2df)__B,
                                              (__v2df)__C);
}
extern __inline __m256d
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_fmaddsub_pd (__m256d __A, __m256d __B, __m256d __C)
{
  return (__m256d)__builtin_ia32_vfmaddsubpd256 ((__v4df)__A,
                                                 (__v4df)__B,
                                                 (__v4df)__C);
}
extern __inline __m128
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_fmaddsub_ps (__m128 __A, __m128 __B, __m128 __C)
{
  return (__m128)__builtin_ia32_vfmaddsubps ((__v4sf)__A, (__v4sf)__B,
                                             (__v4sf)__C);
}
extern __inline __m256
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_fmaddsub_ps (__m256 __A, __m256 __B, __m256 __C)
{
  return (__m256)__builtin_ia32_vfmaddsubps256 ((__v8sf)__A,
                                                (__v8sf)__B,
                                                (__v8sf)__C);
}
extern __inline __m128d
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_fmsubadd_pd (__m128d __A, __m128d __B, __m128d __C)
{
  return (__m128d)__builtin_ia32_vfmaddsubpd ((__v2df)__A, (__v2df)__B,
                                              -(__v2df)__C);
}
extern __inline __m256d
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_fmsubadd_pd (__m256d __A, __m256d __B, __m256d __C)
{
  return (__m256d)__builtin_ia32_vfmaddsubpd256 ((__v4df)__A,
                                                 (__v4df)__B,
                                                 -(__v4df)__C);
}
extern __inline __m128
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_fmsubadd_ps (__m128 __A, __m128 __B, __m128 __C)
{
  return (__m128)__builtin_ia32_vfmaddsubps ((__v4sf)__A, (__v4sf)__B,
                                             -(__v4sf)__C);
}
extern __inline __m256
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_fmsubadd_ps (__m256 __A, __m256 __B, __m256 __C)
{
  return (__m256)__builtin_ia32_vfmaddsubps256 ((__v8sf)__A,
                                                (__v8sf)__B,
                                                -(__v8sf)__C);
}
#ifdef __DISABLE_FMA__
#undef __DISABLE_FMA__
#pragma GCC pop_options
#endif
#endif
#endif
