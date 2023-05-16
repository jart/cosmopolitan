#if !defined _IMMINTRIN_H_INCLUDED
#error "Never use <cetintrin.h> directly; include <x86intrin.h> instead."
#endif

#ifndef _CETINTRIN_H_INCLUDED
#define _CETINTRIN_H_INCLUDED

#ifndef __SHSTK__
#pragma GCC push_options
#pragma GCC target("shstk")
#define __DISABLE_SHSTK__
#endif /* __SHSTK__ */

#ifdef __x86_64__
__funline unsigned long long _get_ssp(void) {
  return __builtin_ia32_rdsspq();
}
#else
__funline unsigned int _get_ssp(void) {
  return __builtin_ia32_rdsspd();
}
#endif

__funline void _inc_ssp(unsigned int __B) {
#ifdef __x86_64__
  __builtin_ia32_incsspq((unsigned long long)__B);
#else
  __builtin_ia32_incsspd(__B);
#endif
}

__funline void _saveprevssp(void) {
  __builtin_ia32_saveprevssp();
}

__funline void _rstorssp(void *__B) {
  __builtin_ia32_rstorssp(__B);
}

__funline void _wrssd(unsigned int __B, void *__C) {
  __builtin_ia32_wrssd(__B, __C);
}

#ifdef __x86_64__
__funline void _wrssq(unsigned long long __B, void *__C) {
  __builtin_ia32_wrssq(__B, __C);
}
#endif

__funline void _wrussd(unsigned int __B, void *__C) {
  __builtin_ia32_wrussd(__B, __C);
}

#ifdef __x86_64__
__funline void _wrussq(unsigned long long __B, void *__C) {
  __builtin_ia32_wrussq(__B, __C);
}
#endif

__funline void _setssbsy(void) {
  __builtin_ia32_setssbsy();
}

__funline void _clrssbsy(void *__B) {
  __builtin_ia32_clrssbsy(__B);
}

#ifdef __DISABLE_SHSTK__
#undef __DISABLE_SHSTK__
#pragma GCC pop_options
#endif /* __DISABLE_SHSTK__ */

#endif /* _CETINTRIN_H_INCLUDED.  */
