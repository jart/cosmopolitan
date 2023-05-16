#if !defined _IMMINTRIN_H_INCLUDED
#error "Never use <xsaveintrin.h> directly; include <immintrin.h> instead."
#endif

#ifndef _XSAVEINTRIN_H_INCLUDED
#define _XSAVEINTRIN_H_INCLUDED

#ifndef __XSAVE__
#pragma GCC push_options
#pragma GCC target("xsave")
#define __DISABLE_XSAVE__
#endif /* __XSAVE__ */

__funline void _xsave(void *__P, long long __M) {
  __builtin_ia32_xsave(__P, __M);
}

__funline void _xrstor(void *__P, long long __M) {
  __builtin_ia32_xrstor(__P, __M);
}

__funline void _xsetbv(unsigned int __A, long long __V) {
  __builtin_ia32_xsetbv(__A, __V);
}

__funline long long _xgetbv(unsigned int __A) {
  return __builtin_ia32_xgetbv(__A);
}

#ifdef __x86_64__
__funline void _xsave64(void *__P, long long __M) {
  __builtin_ia32_xsave64(__P, __M);
}

__funline void _xrstor64(void *__P, long long __M) {
  __builtin_ia32_xrstor64(__P, __M);
}
#endif

#ifdef __DISABLE_XSAVE__
#undef __DISABLE_XSAVE__
#pragma GCC pop_options
#endif /* __DISABLE_XSAVE__ */

#endif /* _XSAVEINTRIN_H_INCLUDED */
