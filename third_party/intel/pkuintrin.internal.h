#if !defined _IMMINTRIN_H_INCLUDED
#error "Never use <pkuintrin.h> directly; include <immintrin.h> instead."
#endif

#ifndef _PKUINTRIN_H_INCLUDED
#define _PKUINTRIN_H_INCLUDED

#ifndef __PKU__
#pragma GCC push_options
#pragma GCC target("pku")
#define __DISABLE_PKU__
#endif /* __PKU__ */

__funline unsigned int _rdpkru_u32(void) {
  return __builtin_ia32_rdpkru();
}

__funline void _wrpkru(unsigned int __key) {
  __builtin_ia32_wrpkru(__key);
}

#ifdef __DISABLE_PKU__
#undef __DISABLE_PKU__
#pragma GCC pop_options
#endif /* __DISABLE_PKU__ */

#endif /* _PKUINTRIN_H_INCLUDED */
