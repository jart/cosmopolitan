#if !defined _IMMINTRIN_H_INCLUDED && !defined _MM3DNOW_H_INCLUDED
#error \
    "Never use <prfchwintrin.h> directly; include <immintrin.h> or <mm3dnow.h> instead."
#endif

#ifndef _PRFCHWINTRIN_H_INCLUDED
#define _PRFCHWINTRIN_H_INCLUDED

__funline void _m_prefetchw(void *__P) {
  __builtin_prefetch(__P, 1, 3 /* _MM_HINT_T0 */);
}

#endif /* _PRFCHWINTRIN_H_INCLUDED */
