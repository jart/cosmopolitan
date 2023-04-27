#if !defined _IMMINTRIN_H_INCLUDED
#error "Never use <movdirintrin.h> directly; include <x86intrin.h> instead."
#endif

#ifndef _MOVDIRINTRIN_H_INCLUDED
#define _MOVDIRINTRIN_H_INCLUDED

#ifndef __MOVDIRI__
#pragma GCC push_options
#pragma GCC target("movdiri")
#define __DISABLE_MOVDIRI__
#endif /* __MOVDIRI__ */

extern __inline void
    __attribute__((__gnu_inline__, __always_inline__, __artificial__))
    _directstoreu_u32(void *__P, unsigned int __A) {
  __builtin_ia32_directstoreu_u32((unsigned int *)__P, __A);
}
#ifdef __x86_64__
extern __inline void
    __attribute__((__gnu_inline__, __always_inline__, __artificial__))
    _directstoreu_u64(void *__P, unsigned long long __A) {
  __builtin_ia32_directstoreu_u64((unsigned long long *)__P, __A);
}
#endif

#ifdef __DISABLE_MOVDIRI__
#undef __DISABLE_MOVDIRI__
#pragma GCC pop_options
#endif /* __DISABLE_MOVDIRI__ */

#ifndef __MOVDIR64B__
#pragma GCC push_options
#pragma GCC target("movdir64b")
#define __DISABLE_MOVDIR64B__
#endif /* __MOVDIR64B__ */

extern __inline void
    __attribute__((__gnu_inline__, __always_inline__, __artificial__))
    _movdir64b(void *__P, const void *__Q) {
  __builtin_ia32_movdir64b(__P, __Q);
}

#ifdef __DISABLE_MOVDIR64B__
#undef __DISABLE_MOVDIR64B__
#pragma GCC pop_options
#endif /* __DISABLE_MOVDIR64B__ */
#endif /* _MOVDIRINTRIN_H_INCLUDED.  */
