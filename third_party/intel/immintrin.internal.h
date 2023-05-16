#ifndef _IMMINTRIN_H_INCLUDED
#define _IMMINTRIN_H_INCLUDED
#ifdef __x86_64__

/* clang-format off */
#include "third_party/intel/mmintrin.internal.h"
#include "third_party/intel/xmmintrin.internal.h"
#include "third_party/intel/emmintrin.internal.h"
#include "third_party/intel/pmmintrin.internal.h"
#include "third_party/intel/tmmintrin.internal.h"
#include "third_party/intel/smmintrin.internal.h"
#include "third_party/intel/wmmintrin.internal.h"
#include "third_party/intel/fxsrintrin.internal.h"
#include "third_party/intel/xsaveintrin.internal.h"
#include "third_party/intel/xsaveoptintrin.internal.h"
#include "third_party/intel/xsavesintrin.internal.h"
#include "third_party/intel/xsavecintrin.internal.h"
#include "third_party/intel/avxintrin.internal.h"
#include "third_party/intel/avx2intrin.internal.h"
#include "third_party/intel/avx512fintrin.internal.h"
#include "third_party/intel/avx512erintrin.internal.h"
#include "third_party/intel/avx512pfintrin.internal.h"
#include "third_party/intel/avx512cdintrin.internal.h"
#include "third_party/intel/avx512vlintrin.internal.h"
#include "third_party/intel/avx512bwintrin.internal.h"
#include "third_party/intel/avx512dqintrin.internal.h"
#include "third_party/intel/avx512vlbwintrin.internal.h"
#include "third_party/intel/avx512vldqintrin.internal.h"
#include "third_party/intel/avx512ifmaintrin.internal.h"
#include "third_party/intel/avx512ifmavlintrin.internal.h"
#include "third_party/intel/avx512vbmiintrin.internal.h"
#include "third_party/intel/avx512vbmivlintrin.internal.h"
#include "third_party/intel/avx5124fmapsintrin.internal.h"
#include "third_party/intel/avx5124vnniwintrin.internal.h"
#include "third_party/intel/avx512vpopcntdqintrin.internal.h"
#include "third_party/intel/avx512vbmi2intrin.internal.h"
#include "third_party/intel/avx512vbmi2vlintrin.internal.h"
#include "third_party/intel/avx512vnniintrin.internal.h"
#include "third_party/intel/avx512vnnivlintrin.internal.h"
#include "third_party/intel/avx512vpopcntdqvlintrin.internal.h"
#include "third_party/intel/avx512bitalgintrin.internal.h"
#include "third_party/intel/shaintrin.internal.h"
#include "third_party/intel/lzcntintrin.internal.h"
#include "third_party/intel/bmiintrin.internal.h"
#include "third_party/intel/bmi2intrin.internal.h"
#include "third_party/intel/fmaintrin.internal.h"
#include "third_party/intel/f16cintrin.internal.h"
#include "third_party/intel/rtmintrin.internal.h"
#include "third_party/intel/xtestintrin.internal.h"
#include "third_party/intel/cetintrin.internal.h"
#include "third_party/intel/gfniintrin.internal.h"
#include "third_party/intel/vaesintrin.internal.h"
#include "third_party/intel/vpclmulqdqintrin.internal.h"
#include "third_party/intel/movdirintrin.internal.h"
#include "third_party/intel/sgxintrin.internal.h"
#include "third_party/intel/pconfigintrin.internal.h"
#include "third_party/intel/waitpkgintrin.internal.h"
#include "third_party/intel/cldemoteintrin.internal.h"
#include "third_party/intel/rdseedintrin.internal.h"
#include "third_party/intel/prfchwintrin.internal.h"
#include "third_party/intel/adxintrin.internal.h"
#include "third_party/intel/clwbintrin.internal.h"
#include "third_party/intel/clflushoptintrin.internal.h"
#include "third_party/intel/wbnoinvdintrin.internal.h"
#include "third_party/intel/pkuintrin.internal.h"
/* clang-format on */

__funline void _wbinvd(void) {
  __builtin_ia32_wbinvd();
}

#ifndef __RDRND__
#pragma GCC push_options
#pragma GCC target("rdrnd")
#define __DISABLE_RDRND__
#endif /* __RDRND__ */
__funline int _rdrand16_step(unsigned short *__P) {
  return __builtin_ia32_rdrand16_step(__P);
}

__funline int _rdrand32_step(unsigned int *__P) {
  return __builtin_ia32_rdrand32_step(__P);
}
#ifdef __DISABLE_RDRND__
#undef __DISABLE_RDRND__
#pragma GCC pop_options
#endif /* __DISABLE_RDRND__ */

#ifndef __RDPID__
#pragma GCC push_options
#pragma GCC target("rdpid")
#define __DISABLE_RDPID__
#endif /* __RDPID__ */
__funline unsigned int _rdpid_u32(void) {
  return __builtin_ia32_rdpid();
}
#ifdef __DISABLE_RDPID__
#undef __DISABLE_RDPID__
#pragma GCC pop_options
#endif /* __DISABLE_RDPID__ */

#ifdef __x86_64__

#ifndef __FSGSBASE__
#pragma GCC push_options
#pragma GCC target("fsgsbase")
#define __DISABLE_FSGSBASE__
#endif /* __FSGSBASE__ */
__funline unsigned int _readfsbase_u32(void) {
  return __builtin_ia32_rdfsbase32();
}

__funline unsigned long long _readfsbase_u64(void) {
  return __builtin_ia32_rdfsbase64();
}

__funline unsigned int _readgsbase_u32(void) {
  return __builtin_ia32_rdgsbase32();
}

__funline unsigned long long _readgsbase_u64(void) {
  return __builtin_ia32_rdgsbase64();
}

__funline void _writefsbase_u32(unsigned int __B) {
  __builtin_ia32_wrfsbase32(__B);
}

__funline void _writefsbase_u64(unsigned long long __B) {
  __builtin_ia32_wrfsbase64(__B);
}

__funline void _writegsbase_u32(unsigned int __B) {
  __builtin_ia32_wrgsbase32(__B);
}

__funline void _writegsbase_u64(unsigned long long __B) {
  __builtin_ia32_wrgsbase64(__B);
}
#ifdef __DISABLE_FSGSBASE__
#undef __DISABLE_FSGSBASE__
#pragma GCC pop_options
#endif /* __DISABLE_FSGSBASE__ */

#ifndef __RDRND__
#pragma GCC push_options
#pragma GCC target("rdrnd")
#define __DISABLE_RDRND__
#endif /* __RDRND__ */
__funline int _rdrand64_step(unsigned long long *__P) {
  return __builtin_ia32_rdrand64_step(__P);
}
#ifdef __DISABLE_RDRND__
#undef __DISABLE_RDRND__
#pragma GCC pop_options
#endif /* __DISABLE_RDRND__ */

#endif /* __x86_64__  */

#ifndef __PTWRITE__
#pragma GCC push_options
#pragma GCC target("ptwrite")
#define __DISABLE_PTWRITE__
#endif

#ifdef __x86_64__
__funline void _ptwrite64(unsigned long long __B) {
  __builtin_ia32_ptwrite64(__B);
}
#endif /* __x86_64__ */

__funline void _ptwrite32(unsigned __B) {
  __builtin_ia32_ptwrite32(__B);
}
#ifdef __DISABLE_PTWRITE__
#undef __DISABLE_PTWRITE__
#pragma GCC pop_options
#endif /* __DISABLE_PTWRITE__ */

#endif /* __x86_64__ */
#endif /* _IMMINTRIN_H_INCLUDED */
