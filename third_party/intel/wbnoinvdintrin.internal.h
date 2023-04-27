#ifndef _IMMINTRIN_H_INCLUDED
#error "Never use <wbnoinvdintrin.h> directly; include <immintrin.h> instead."
#endif

#ifndef _WBNOINVDINTRIN_H_INCLUDED
#define _WBNOINVDINTRIN_H_INCLUDED

#ifndef __WBNOINVD__
#pragma GCC push_options
#pragma GCC target("wbnoinvd")
#define __DISABLE_WBNOINVD__
#endif /* __WBNOINVD__ */

extern __inline void
    __attribute__((__gnu_inline__, __always_inline__, __artificial__))
    _wbnoinvd(void) {
  __builtin_ia32_wbnoinvd();
}

#ifdef __DISABLE_WBNOINVD__
#undef __DISABLE_WBNOINVD__
#pragma GCC pop_options
#endif /* __DISABLE_WBNOINVD__ */

#endif /* _WBNOINVDINTRIN_H_INCLUDED */
