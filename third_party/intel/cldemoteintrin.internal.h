#if !defined _IMMINTRIN_H_INCLUDED
#error "Never use <cldemoteintrin.h> directly; include <immintrin.h> instead."
#endif

#ifndef _CLDEMOTE_H_INCLUDED
#define _CLDEMOTE_H_INCLUDED

#ifndef __CLDEMOTE__
#pragma GCC push_options
#pragma GCC target("cldemote")
#define __DISABLE_CLDEMOTE__
#endif /* __CLDEMOTE__ */
__funline void _cldemote(void *__A) {
  __builtin_ia32_cldemote(__A);
}
#ifdef __DISABLE_CLDEMOTE__
#undef __DISABLE_CLDEMOTE__
#pragma GCC pop_options
#endif /* __DISABLE_CLDEMOTE__ */

#endif /* _CLDEMOTE_H_INCLUDED */
