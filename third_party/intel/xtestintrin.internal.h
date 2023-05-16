#ifndef _IMMINTRIN_H_INCLUDED
#error "Never use <xtestintrin.h> directly; include <immintrin.h> instead."
#endif

#ifndef _XTESTINTRIN_H_INCLUDED
#define _XTESTINTRIN_H_INCLUDED

#ifndef __RTM__
#pragma GCC push_options
#pragma GCC target("rtm")
#define __DISABLE_RTM__
#endif /* __RTM__ */

__funline int _xtest(void) {
  return __builtin_ia32_xtest();
}

#ifdef __DISABLE_RTM__
#undef __DISABLE_RTM__
#pragma GCC pop_options
#endif /* __DISABLE_RTM__ */

#endif /* _XTESTINTRIN_H_INCLUDED */
