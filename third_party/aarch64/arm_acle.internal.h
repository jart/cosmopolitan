#if defined(__aarch64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _GCC_ARM_ACLE_H
#define _GCC_ARM_ACLE_H
#pragma GCC aarch64 "arm_acle.h"
#ifdef __cplusplus
extern "C" {
#endif
#define _GCC_ARM_ACLE_ROR_FN(NAME, TYPE) __extension__ extern __inline TYPE __attribute__ ((__always_inline__, __gnu_inline__, __artificial__)) NAME (TYPE __value, uint32_t __rotate) { size_t __size = sizeof (TYPE) * __CHAR_BIT__; __rotate = __rotate % __size; return __value >> __rotate | __value << ((__size - __rotate) % __size); }
_GCC_ARM_ACLE_ROR_FN (__ror, uint32_t)
_GCC_ARM_ACLE_ROR_FN (__rorl, unsigned long)
_GCC_ARM_ACLE_ROR_FN (__rorll, uint64_t)
#undef _GCC_ARM_ACLE_ROR_FN
#define _GCC_ARM_ACLE_DATA_FN(NAME, BUILTIN, ITYPE, RTYPE) __extension__ extern __inline RTYPE __attribute__ ((__always_inline__, __gnu_inline__, __artificial__)) __##NAME (ITYPE __value) { return __builtin_##BUILTIN (__value); }
_GCC_ARM_ACLE_DATA_FN (clz, clz, uint32_t, unsigned int)
_GCC_ARM_ACLE_DATA_FN (clzl, clzl, unsigned long, unsigned int)
_GCC_ARM_ACLE_DATA_FN (clzll, clzll, uint64_t, unsigned int)
_GCC_ARM_ACLE_DATA_FN (cls, clrsb, uint32_t, unsigned int)
_GCC_ARM_ACLE_DATA_FN (clsl, clrsbl, unsigned long, unsigned int)
_GCC_ARM_ACLE_DATA_FN (clsll, clrsbll, uint64_t, unsigned int)
_GCC_ARM_ACLE_DATA_FN (rev16, aarch64_rev16, uint32_t, uint32_t)
_GCC_ARM_ACLE_DATA_FN (rev16l, aarch64_rev16l, unsigned long, unsigned long)
_GCC_ARM_ACLE_DATA_FN (rev16ll, aarch64_rev16ll, uint64_t, uint64_t)
_GCC_ARM_ACLE_DATA_FN (rbit, aarch64_rbit, uint32_t, uint32_t)
_GCC_ARM_ACLE_DATA_FN (rbitl, aarch64_rbitl, unsigned long, unsigned long)
_GCC_ARM_ACLE_DATA_FN (rbitll, aarch64_rbitll, uint64_t, uint64_t)
_GCC_ARM_ACLE_DATA_FN (revsh, bswap16, int16_t, int16_t)
_GCC_ARM_ACLE_DATA_FN (rev, bswap32, uint32_t, uint32_t)
_GCC_ARM_ACLE_DATA_FN (revll, bswap64, uint64_t, uint64_t)
#undef _GCC_ARM_ACLE_DATA_FN
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
__pld (void const volatile *__addr)
{
  return __builtin_aarch64_pld (__addr);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
__pli (void const volatile *__addr)
{
  return __builtin_aarch64_pli (__addr);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
__plix (unsigned int __cache, unsigned int __rettn,
 void const volatile *__addr)
{
  return __builtin_aarch64_plix (__cache, __rettn, __addr);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
__pldx (unsigned int __access, unsigned int __cache, unsigned int __rettn,
 void const volatile *__addr)
{
  return __builtin_aarch64_pldx (__access, __cache, __rettn, __addr);
}
__extension__ extern __inline unsigned long
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
__revl (unsigned long __value)
{
  if (sizeof (unsigned long) == 8)
    return __revll (__value);
  else
    return __rev (__value);
}
#pragma GCC push_options
#pragma GCC target ("arch=armv8.3-a")
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
__jcvt (double __a)
{
  return __builtin_aarch64_jcvtzs (__a);
}
#pragma GCC pop_options
#pragma GCC push_options
#pragma GCC target ("arch=armv8.5-a")
__extension__ extern __inline float
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
__rint32zf (float __a)
{
  return __builtin_aarch64_frint32zsf (__a);
}
__extension__ extern __inline double
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
__rint32z (double __a)
{
  return __builtin_aarch64_frint32zdf (__a);
}
__extension__ extern __inline float
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
__rint64zf (float __a)
{
  return __builtin_aarch64_frint64zsf (__a);
}
__extension__ extern __inline double
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
__rint64z (double __a)
{
  return __builtin_aarch64_frint64zdf (__a);
}
__extension__ extern __inline float
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
__rint32xf (float __a)
{
  return __builtin_aarch64_frint32xsf (__a);
}
__extension__ extern __inline double
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
__rint32x (double __a)
{
  return __builtin_aarch64_frint32xdf (__a);
}
__extension__ extern __inline float
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
__rint64xf (float __a)
{
  return __builtin_aarch64_frint64xsf (__a);
}
__extension__ extern __inline double
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
__rint64x (double __a)
{
  return __builtin_aarch64_frint64xdf (__a);
}
#pragma GCC pop_options
#pragma GCC push_options
#pragma GCC target ("+nothing+crc")
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
__crc32b (uint32_t __a, uint8_t __b)
{
  return __builtin_aarch64_crc32b (__a, __b);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
__crc32cb (uint32_t __a, uint8_t __b)
{
  return __builtin_aarch64_crc32cb (__a, __b);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
__crc32ch (uint32_t __a, uint16_t __b)
{
  return __builtin_aarch64_crc32ch (__a, __b);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
__crc32cw (uint32_t __a, uint32_t __b)
{
  return __builtin_aarch64_crc32cw (__a, __b);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
__crc32cd (uint32_t __a, uint64_t __b)
{
  return __builtin_aarch64_crc32cx (__a, __b);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
__crc32h (uint32_t __a, uint16_t __b)
{
  return __builtin_aarch64_crc32h (__a, __b);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
__crc32w (uint32_t __a, uint32_t __b)
{
  return __builtin_aarch64_crc32w (__a, __b);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
__crc32d (uint32_t __a, uint64_t __b)
{
  return __builtin_aarch64_crc32x (__a, __b);
}
#pragma GCC pop_options
#ifdef __ARM_FEATURE_TME
#pragma GCC push_options
#pragma GCC target ("+nothing+tme")
#define _TMFAILURE_REASON 0x00007fffu
#define _TMFAILURE_RTRY 0x00008000u
#define _TMFAILURE_CNCL 0x00010000u
#define _TMFAILURE_MEM 0x00020000u
#define _TMFAILURE_IMP 0x00040000u
#define _TMFAILURE_ERR 0x00080000u
#define _TMFAILURE_SIZE 0x00100000u
#define _TMFAILURE_NEST 0x00200000u
#define _TMFAILURE_DBG 0x00400000u
#define _TMFAILURE_INT 0x00800000u
#define _TMFAILURE_TRIVIAL 0x01000000u
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
__tstart (void)
{
  return __builtin_aarch64_tstart ();
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
__tcommit (void)
{
  __builtin_aarch64_tcommit ();
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
__tcancel (const uint64_t __reason)
{
  __builtin_aarch64_tcancel (__reason);
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
__ttest (void)
{
  return __builtin_aarch64_ttest ();
}
#pragma GCC pop_options
#endif
#ifdef __ARM_FEATURE_LS64
typedef __arm_data512_t data512_t;
#endif
#pragma GCC push_options
#pragma GCC target ("+nothing+rng")
__extension__ extern __inline int
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
__rndr (uint64_t *__res)
{
  return __builtin_aarch64_rndr (__res);
}
__extension__ extern __inline int
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
__rndrrs (uint64_t *__res)
{
  return __builtin_aarch64_rndrrs (__res);
}
#pragma GCC pop_options
#pragma GCC push_options
#pragma GCC target ("+nothing+memtag")
#define __arm_mte_create_random_tag(__ptr, __u64_mask) __builtin_aarch64_memtag_irg(__ptr, __u64_mask)
#define __arm_mte_exclude_tag(__ptr, __u64_excluded) __builtin_aarch64_memtag_gmi(__ptr, __u64_excluded)
#define __arm_mte_ptrdiff(__ptr_a, __ptr_b) __builtin_aarch64_memtag_subp(__ptr_a, __ptr_b)
#define __arm_mte_increment_tag(__ptr, __u_offset) __builtin_aarch64_memtag_inc_tag(__ptr, __u_offset)
#define __arm_mte_set_tag(__tagged_address) __builtin_aarch64_memtag_set_tag(__tagged_address)
#define __arm_mte_get_tag(__address) __builtin_aarch64_memtag_get_tag(__address)
#pragma GCC pop_options
#define __arm_rsr(__regname) __builtin_aarch64_rsr (__regname)
#define __arm_rsrp(__regname) __builtin_aarch64_rsrp (__regname)
#define __arm_rsr64(__regname) __builtin_aarch64_rsr64 (__regname)
#define __arm_rsrf(__regname) __builtin_aarch64_rsrf (__regname)
#define __arm_rsrf64(__regname) __builtin_aarch64_rsrf64 (__regname)
#define __arm_wsr(__regname, __value) __builtin_aarch64_wsr (__regname, __value)
#define __arm_wsrp(__regname, __value) __builtin_aarch64_wsrp (__regname, __value)
#define __arm_wsr64(__regname, __value) __builtin_aarch64_wsr64 (__regname, __value)
#define __arm_wsrf(__regname, __value) __builtin_aarch64_wsrf (__regname, __value)
#define __arm_wsrf64(__regname, __value) __builtin_aarch64_wsrf64 (__regname, __value)
#pragma GCC push_options
#pragma GCC target ("+nothing+d128")
#define __arm_rsr128(__regname) __builtin_aarch64_rsr128 (__regname)
#define __arm_wsr128(__regname, __value) __builtin_aarch64_wsr128 (__regname, __value)
#pragma GCC pop_options
#ifdef __cplusplus
}
#endif
#endif
#endif
