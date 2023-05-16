#ifndef _CLZEROINTRIN_H_INCLUDED
#define _CLZEROINTRIN_H_INCLUDED
#ifdef __x86_64__

#ifndef __CLZERO__
#pragma GCC push_options
#pragma GCC target("clzero")
#define __DISABLE_CLZERO__
#endif /* __CLZERO__ */

__funline void _mm_clzero(void* __I) {
  __builtin_ia32_clzero(__I);
}

#ifdef __DISABLE_CLZERO__
#undef __DISABLE_CLZERO__
#pragma GCC pop_options
#endif /* __DISABLE_CLZERO__ */

#endif /* __x86_64__ */
#endif /* _CLZEROINTRIN_H_INCLUDED */
