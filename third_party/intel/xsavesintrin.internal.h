#if !defined _IMMINTRIN_H_INCLUDED
#error "Never use <xsavesintrin.h> directly; include <immintrin.h> instead."
#endif

#ifndef _XSAVESINTRIN_H_INCLUDED
#define _XSAVESINTRIN_H_INCLUDED

#ifndef __XSAVES__
#pragma GCC push_options
#pragma GCC target("xsaves")
#define __DISABLE_XSAVES__
#endif /* __XSAVES__ */

__funline void _xsaves(void *__P, long long __M) {
  __builtin_ia32_xsaves(__P, __M);
}

__funline void _xrstors(void *__P, long long __M) {
  __builtin_ia32_xrstors(__P, __M);
}

#ifdef __x86_64__
__funline void _xrstors64(void *__P, long long __M) {
  __builtin_ia32_xrstors64(__P, __M);
}

__funline void _xsaves64(void *__P, long long __M) {
  __builtin_ia32_xsaves64(__P, __M);
}
#endif

#ifdef __DISABLE_XSAVES__
#undef __DISABLE_XSAVES__
#pragma GCC pop_options
#endif /* __DISABLE_XSAVES__ */

#endif /* _XSAVESINTRIN_H_INCLUDED */
