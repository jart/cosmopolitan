#ifndef _X86INTRIN_H_INCLUDED
#error "Never use <tbmintrin.h> directly; include <x86intrin.h> instead."
#endif

#ifndef _TBMINTRIN_H_INCLUDED
#define _TBMINTRIN_H_INCLUDED

#ifndef __TBM__
#pragma GCC push_options
#pragma GCC target("tbm")
#define __DISABLE_TBM__
#endif /* __TBM__ */

#ifdef __OPTIMIZE__
__funline unsigned int __bextri_u32(unsigned int __X, const unsigned int __I) {
  return __builtin_ia32_bextri_u32(__X, __I);
}
#else
#define __bextri_u32(X, I)                                    \
  ((unsigned int)__builtin_ia32_bextri_u32((unsigned int)(X), \
                                           (unsigned int)(I)))
#endif /*__OPTIMIZE__ */

__funline unsigned int __blcfill_u32(unsigned int __X) {
  return __X & (__X + 1);
}

__funline unsigned int __blci_u32(unsigned int __X) {
  return __X | ~(__X + 1);
}

__funline unsigned int __blcic_u32(unsigned int __X) {
  return ~__X & (__X + 1);
}

__funline unsigned int __blcmsk_u32(unsigned int __X) {
  return __X ^ (__X + 1);
}

__funline unsigned int __blcs_u32(unsigned int __X) {
  return __X | (__X + 1);
}

__funline unsigned int __blsfill_u32(unsigned int __X) {
  return __X | (__X - 1);
}

__funline unsigned int __blsic_u32(unsigned int __X) {
  return ~__X | (__X - 1);
}

__funline unsigned int __t1mskc_u32(unsigned int __X) {
  return ~__X | (__X + 1);
}

__funline unsigned int __tzmsk_u32(unsigned int __X) {
  return ~__X & (__X - 1);
}

#ifdef __x86_64__
#ifdef __OPTIMIZE__
__funline unsigned long long __bextri_u64(unsigned long long __X,
                                        const unsigned int __I) {
  return __builtin_ia32_bextri_u64(__X, __I);
}
#else
#define __bextri_u64(X, I)                                                \
  ((unsigned long long)__builtin_ia32_bextri_u64((unsigned long long)(X), \
                                                 (unsigned long long)(I)))
#endif /*__OPTIMIZE__ */

__funline unsigned long long __blcfill_u64(unsigned long long __X) {
  return __X & (__X + 1);
}

__funline unsigned long long __blci_u64(unsigned long long __X) {
  return __X | ~(__X + 1);
}

__funline unsigned long long __blcic_u64(unsigned long long __X) {
  return ~__X & (__X + 1);
}

__funline unsigned long long __blcmsk_u64(unsigned long long __X) {
  return __X ^ (__X + 1);
}

__funline unsigned long long __blcs_u64(unsigned long long __X) {
  return __X | (__X + 1);
}

__funline unsigned long long __blsfill_u64(unsigned long long __X) {
  return __X | (__X - 1);
}

__funline unsigned long long __blsic_u64(unsigned long long __X) {
  return ~__X | (__X - 1);
}

__funline unsigned long long __t1mskc_u64(unsigned long long __X) {
  return ~__X | (__X + 1);
}

__funline unsigned long long __tzmsk_u64(unsigned long long __X) {
  return ~__X & (__X - 1);
}

#endif /* __x86_64__  */

#ifdef __DISABLE_TBM__
#undef __DISABLE_TBM__
#pragma GCC pop_options
#endif /* __DISABLE_TBM__ */

#endif /* _TBMINTRIN_H_INCLUDED */
