#if !defined _IMMINTRIN_H_INCLUDED
#error "Never use <clflushoptintrin.h> directly; include <immintrin.h> instead."
#endif

#ifndef _CLFLUSHOPTINTRIN_H_INCLUDED
#define _CLFLUSHOPTINTRIN_H_INCLUDED

#ifndef __CLFLUSHOPT__
#pragma GCC push_options
#pragma GCC target("clflushopt")
#define __DISABLE_CLFLUSHOPT__
#endif /* __CLFLUSHOPT__ */

__funline void _mm_clflushopt(void *__A) {
  __builtin_ia32_clflushopt(__A);
}

#ifdef __DISABLE_CLFLUSHOPT__
#undef __DISABLE_CLFLUSHOPT__
#pragma GCC pop_options
#endif /* __DISABLE_CLFLUSHOPT__ */

#endif /* _CLFLUSHOPTINTRIN_H_INCLUDED */
