#if defined(__aarch64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _GCC_ARM_ACLE_H
#define _GCC_ARM_ACLE_H
#ifdef __cplusplus
extern "C" {
#endif
#pragma GCC push_options
#pragma GCC target ("arch=armv8.3-a")
__funline int32_t
__jcvt (double __a)
{
  return __builtin_aarch64_jcvtzs (__a);
}
#pragma GCC pop_options
#pragma GCC push_options
#pragma GCC target ("arch=armv8.5-a")
__funline float
__rint32zf (float __a)
{
  return __builtin_aarch64_frint32zsf (__a);
}
__funline double
__rint32z (double __a)
{
  return __builtin_aarch64_frint32zdf (__a);
}
__funline float
__rint64zf (float __a)
{
  return __builtin_aarch64_frint64zsf (__a);
}
__funline double
__rint64z (double __a)
{
  return __builtin_aarch64_frint64zdf (__a);
}
__funline float
__rint32xf (float __a)
{
  return __builtin_aarch64_frint32xsf (__a);
}
__funline double
__rint32x (double __a)
{
  return __builtin_aarch64_frint32xdf (__a);
}
__funline float
__rint64xf (float __a)
{
  return __builtin_aarch64_frint64xsf (__a);
}
__funline double
__rint64x (double __a)
{
  return __builtin_aarch64_frint64xdf (__a);
}
#pragma GCC pop_options
#pragma GCC push_options
#pragma GCC target ("+nothing+crc")
__funline uint32_t
__crc32b (uint32_t __a, uint8_t __b)
{
  return __builtin_aarch64_crc32b (__a, __b);
}
__funline uint32_t
__crc32cb (uint32_t __a, uint8_t __b)
{
  return __builtin_aarch64_crc32cb (__a, __b);
}
__funline uint32_t
__crc32ch (uint32_t __a, uint16_t __b)
{
  return __builtin_aarch64_crc32ch (__a, __b);
}
__funline uint32_t
__crc32cw (uint32_t __a, uint32_t __b)
{
  return __builtin_aarch64_crc32cw (__a, __b);
}
__funline uint32_t
__crc32cd (uint32_t __a, uint64_t __b)
{
  return __builtin_aarch64_crc32cx (__a, __b);
}
__funline uint32_t
__crc32h (uint32_t __a, uint16_t __b)
{
  return __builtin_aarch64_crc32h (__a, __b);
}
__funline uint32_t
__crc32w (uint32_t __a, uint32_t __b)
{
  return __builtin_aarch64_crc32w (__a, __b);
}
__funline uint32_t
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
__funline uint64_t
__tstart (void)
{
  return __builtin_aarch64_tstart ();
}
__funline void
__tcommit (void)
{
  __builtin_aarch64_tcommit ();
}
__funline void
__tcancel (const uint64_t __reason)
{
  __builtin_aarch64_tcancel (__reason);
}
__funline uint64_t
__ttest (void)
{
  return __builtin_aarch64_ttest ();
}
#pragma GCC pop_options
#endif
#pragma GCC push_options
#pragma GCC target ("+nothing+rng")
__funline int
__rndr (uint64_t *__res)
{
  return __builtin_aarch64_rndr (__res);
}
__funline int
__rndrrs (uint64_t *__res)
{
  return __builtin_aarch64_rndrrs (__res);
}
#pragma GCC pop_options
#pragma GCC push_options
#pragma GCC target ("arch=armv8.5-a+memtag")
#define __arm_mte_create_random_tag(__ptr, __u64_mask) __builtin_aarch64_memtag_irg(__ptr, __u64_mask)
#define __arm_mte_exclude_tag(__ptr, __u64_excluded) __builtin_aarch64_memtag_gmi(__ptr, __u64_excluded)
#define __arm_mte_ptrdiff(__ptr_a, __ptr_b) __builtin_aarch64_memtag_subp(__ptr_a, __ptr_b)
#define __arm_mte_increment_tag(__ptr, __u_offset) __builtin_aarch64_memtag_inc_tag(__ptr, __u_offset)
#define __arm_mte_set_tag(__tagged_address) __builtin_aarch64_memtag_set_tag(__tagged_address)
#define __arm_mte_get_tag(__address) __builtin_aarch64_memtag_get_tag(__address)
#pragma GCC pop_options
#ifdef __cplusplus
}
#endif
#endif
#endif
