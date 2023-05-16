#if !defined _IMMINTRIN_H_INCLUDED
#error "Never use <fxsrintrin.h> directly; include <immintrin.h> instead."
#endif

#ifndef _FXSRINTRIN_H_INCLUDED
#define _FXSRINTRIN_H_INCLUDED

#ifndef __FXSR__
#pragma GCC push_options
#pragma GCC target("fxsr")
#define __DISABLE_FXSR__
#endif /* __FXSR__ */

__funline void _fxsave(void *__P) {
  __builtin_ia32_fxsave(__P);
}

__funline void _fxrstor(void *__P) {
  __builtin_ia32_fxrstor(__P);
}

#ifdef __x86_64__
__funline void _fxsave64(void *__P) {
  __builtin_ia32_fxsave64(__P);
}

__funline void _fxrstor64(void *__P) {
  __builtin_ia32_fxrstor64(__P);
}
#endif

#ifdef __DISABLE_FXSR__
#undef __DISABLE_FXSR__
#pragma GCC pop_options
#endif /* __DISABLE_FXSR__ */

#endif /* _FXSRINTRIN_H_INCLUDED */
