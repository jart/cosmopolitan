#if !defined _IMMINTRIN_H_INCLUDED
#error "Never use <adxintrin.h> directly; include <immintrin.h> instead."
#endif

#ifndef _ADXINTRIN_H_INCLUDED
#define _ADXINTRIN_H_INCLUDED

__funline unsigned char _subborrow_u32(unsigned char __CF, unsigned int __X,
                                     unsigned int __Y, unsigned int *__P) {
  return __builtin_ia32_sbb_u32(__CF, __X, __Y, __P);
}

__funline unsigned char _addcarry_u32(unsigned char __CF, unsigned int __X,
                                    unsigned int __Y, unsigned int *__P) {
  return __builtin_ia32_addcarryx_u32(__CF, __X, __Y, __P);
}

__funline unsigned char _addcarryx_u32(unsigned char __CF, unsigned int __X,
                                     unsigned int __Y, unsigned int *__P) {
  return __builtin_ia32_addcarryx_u32(__CF, __X, __Y, __P);
}

#ifdef __x86_64__
__funline unsigned char _subborrow_u64(unsigned char __CF, unsigned long long __X,
                                     unsigned long long __Y,
                                     unsigned long long *__P) {
  return __builtin_ia32_sbb_u64(__CF, __X, __Y, __P);
}

__funline unsigned char _addcarry_u64(unsigned char __CF, unsigned long long __X,
                                    unsigned long long __Y,
                                    unsigned long long *__P) {
  return __builtin_ia32_addcarryx_u64(__CF, __X, __Y, __P);
}

__funline unsigned char _addcarryx_u64(unsigned char __CF, unsigned long long __X,
                                     unsigned long long __Y,
                                     unsigned long long *__P) {
  return __builtin_ia32_addcarryx_u64(__CF, __X, __Y, __P);
}
#endif

#endif /* _ADXINTRIN_H_INCLUDED */
