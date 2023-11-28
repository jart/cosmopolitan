#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _X86GPRINTRIN_H_INCLUDED
#define _X86GPRINTRIN_H_INCLUDED
#include "third_party/intel/ia32intrin.internal.h"
#ifndef __iamcu__
#include "third_party/intel/adxintrin.internal.h"
#include "third_party/intel/bmiintrin.internal.h"
#include "third_party/intel/bmi2intrin.internal.h"
#include "third_party/intel/cetintrin.internal.h"
#include "third_party/intel/cldemoteintrin.internal.h"
#include "third_party/intel/clflushoptintrin.internal.h"
#include "third_party/intel/clwbintrin.internal.h"
#include "third_party/intel/clzerointrin.internal.h"
#include "third_party/intel/enqcmdintrin.internal.h"
#include "third_party/intel/fxsrintrin.internal.h"
#include "third_party/intel/lzcntintrin.internal.h"
#include "third_party/intel/lwpintrin.internal.h"
#include "third_party/intel/movdirintrin.internal.h"
#include "third_party/intel/mwaitxintrin.internal.h"
#include "third_party/intel/pconfigintrin.internal.h"
#include "third_party/intel/popcntintrin.internal.h"
#include "third_party/intel/pkuintrin.internal.h"
#include "third_party/intel/rdseedintrin.internal.h"
#include "third_party/intel/rtmintrin.internal.h"
#include "third_party/intel/serializeintrin.internal.h"
#include "third_party/intel/sgxintrin.internal.h"
#include "third_party/intel/tbmintrin.internal.h"
#include "third_party/intel/tsxldtrkintrin.internal.h"
#include "third_party/intel/uintrintrin.internal.h"
#include "third_party/intel/waitpkgintrin.internal.h"
#include "third_party/intel/wbnoinvdintrin.internal.h"
#include "third_party/intel/xsaveintrin.internal.h"
#include "third_party/intel/xsavecintrin.internal.h"
#include "third_party/intel/xsaveoptintrin.internal.h"
#include "third_party/intel/xsavesintrin.internal.h"
#include "third_party/intel/xtestintrin.internal.h"
#include "third_party/intel/hresetintrin.internal.h"
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_wbinvd (void)
{
  __builtin_ia32_wbinvd ();
}
#ifndef __RDRND__
#pragma GCC push_options
#pragma GCC target("rdrnd")
#define __DISABLE_RDRND__
#endif
extern __inline int
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_rdrand16_step (unsigned short *__P)
{
  return __builtin_ia32_rdrand16_step (__P);
}
extern __inline int
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_rdrand32_step (unsigned int *__P)
{
  return __builtin_ia32_rdrand32_step (__P);
}
#ifdef __DISABLE_RDRND__
#undef __DISABLE_RDRND__
#pragma GCC pop_options
#endif
#ifndef __RDPID__
#pragma GCC push_options
#pragma GCC target("rdpid")
#define __DISABLE_RDPID__
#endif
extern __inline unsigned int
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_rdpid_u32 (void)
{
  return __builtin_ia32_rdpid ();
}
#ifdef __DISABLE_RDPID__
#undef __DISABLE_RDPID__
#pragma GCC pop_options
#endif
#ifdef __x86_64__
#ifndef __FSGSBASE__
#pragma GCC push_options
#pragma GCC target("fsgsbase")
#define __DISABLE_FSGSBASE__
#endif
extern __inline unsigned int
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_readfsbase_u32 (void)
{
  return __builtin_ia32_rdfsbase32 ();
}
extern __inline unsigned long long
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_readfsbase_u64 (void)
{
  return __builtin_ia32_rdfsbase64 ();
}
extern __inline unsigned int
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_readgsbase_u32 (void)
{
  return __builtin_ia32_rdgsbase32 ();
}
extern __inline unsigned long long
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_readgsbase_u64 (void)
{
  return __builtin_ia32_rdgsbase64 ();
}
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_writefsbase_u32 (unsigned int __B)
{
  __builtin_ia32_wrfsbase32 (__B);
}
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_writefsbase_u64 (unsigned long long __B)
{
  __builtin_ia32_wrfsbase64 (__B);
}
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_writegsbase_u32 (unsigned int __B)
{
  __builtin_ia32_wrgsbase32 (__B);
}
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_writegsbase_u64 (unsigned long long __B)
{
  __builtin_ia32_wrgsbase64 (__B);
}
#ifdef __DISABLE_FSGSBASE__
#undef __DISABLE_FSGSBASE__
#pragma GCC pop_options
#endif
#ifndef __RDRND__
#pragma GCC push_options
#pragma GCC target("rdrnd")
#define __DISABLE_RDRND__
#endif
extern __inline int
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_rdrand64_step (unsigned long long *__P)
{
  return __builtin_ia32_rdrand64_step (__P);
}
#ifdef __DISABLE_RDRND__
#undef __DISABLE_RDRND__
#pragma GCC pop_options
#endif
#endif
#ifndef __PTWRITE__
#pragma GCC push_options
#pragma GCC target("ptwrite")
#define __DISABLE_PTWRITE__
#endif
#ifdef __x86_64__
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_ptwrite64 (unsigned long long __B)
{
  __builtin_ia32_ptwrite64 (__B);
}
#endif
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_ptwrite32 (unsigned __B)
{
  __builtin_ia32_ptwrite32 (__B);
}
#ifdef __DISABLE_PTWRITE__
#undef __DISABLE_PTWRITE__
#pragma GCC pop_options
#endif
#endif
#endif
#endif
