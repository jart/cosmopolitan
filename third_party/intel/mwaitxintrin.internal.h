#ifndef _MWAITXINTRIN_H_INCLUDED
#define _MWAITXINTRIN_H_INCLUDED
#ifdef __x86_64__

#ifndef __MWAITX__
#pragma GCC push_options
#pragma GCC target("mwaitx")
#define __DISABLE_MWAITX__
#endif /* __MWAITX__ */

__funline void _mm_monitorx(void const* __P, unsigned int __E, unsigned int __H) {
  __builtin_ia32_monitorx(__P, __E, __H);
}

__funline void _mm_mwaitx(unsigned int __E, unsigned int __H, unsigned int __C) {
  __builtin_ia32_mwaitx(__E, __H, __C);
}

#ifdef __DISABLE_MWAITX__
#undef __DISABLE_MWAITX__
#pragma GCC pop_options
#endif /* __DISABLE_MWAITX__ */

#endif /* __x86_64__ */
#endif /* _MWAITXINTRIN_H_INCLUDED */
