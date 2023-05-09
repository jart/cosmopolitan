#ifndef _AARCH64_FP16_H_
#define _AARCH64_FP16_H_
#ifdef __aarch64__
#include "libc/inttypes.h"
#include "libc/limits.h"
#include "libc/literal.h"

#pragma GCC push_options
#pragma GCC target("arch=armv8.2-a+fp16")

#define FUNC(T)                   \
  __extension__ extern __inline T \
      __attribute__((__always_inline__, __gnu_inline__, __artificial__))

typedef __fp16 float16_t;

FUNC(float16_t)
vabsh_f16(float16_t __a) {
  return __builtin_aarch64_abshf(__a);
}

FUNC(uint16_t)
vceqzh_f16(float16_t __a) {
  return __builtin_aarch64_cmeqhf_uss(__a, 0.0f);
}

FUNC(uint16_t)
vcgezh_f16(float16_t __a) {
  return __builtin_aarch64_cmgehf_uss(__a, 0.0f);
}

FUNC(uint16_t)
vcgtzh_f16(float16_t __a) {
  return __builtin_aarch64_cmgthf_uss(__a, 0.0f);
}

FUNC(uint16_t)
vclezh_f16(float16_t __a) {
  return __builtin_aarch64_cmlehf_uss(__a, 0.0f);
}

FUNC(uint16_t)
vcltzh_f16(float16_t __a) {
  return __builtin_aarch64_cmlthf_uss(__a, 0.0f);
}

FUNC(float16_t)
vcvth_f16_s16(int16_t __a) {
  return __builtin_aarch64_floathihf(__a);
}

FUNC(float16_t)
vcvth_f16_s32(int32_t __a) {
  return __builtin_aarch64_floatsihf(__a);
}

FUNC(float16_t)
vcvth_f16_s64(int64_t __a) {
  return __builtin_aarch64_floatdihf(__a);
}

FUNC(float16_t)
vcvth_f16_u16(uint16_t __a) {
  return __builtin_aarch64_floatunshihf_us(__a);
}

FUNC(float16_t)
vcvth_f16_u32(uint32_t __a) {
  return __builtin_aarch64_floatunssihf_us(__a);
}

FUNC(float16_t)
vcvth_f16_u64(uint64_t __a) {
  return __builtin_aarch64_floatunsdihf_us(__a);
}

FUNC(int16_t)
vcvth_s16_f16(float16_t __a) {
  return __builtin_aarch64_fix_trunchfhi(__a);
}

FUNC(int32_t)
vcvth_s32_f16(float16_t __a) {
  return __builtin_aarch64_fix_trunchfsi(__a);
}

FUNC(int64_t)
vcvth_s64_f16(float16_t __a) {
  return __builtin_aarch64_fix_trunchfdi(__a);
}

FUNC(uint16_t)
vcvth_u16_f16(float16_t __a) {
  return __builtin_aarch64_fixuns_trunchfhi_us(__a);
}

FUNC(uint32_t)
vcvth_u32_f16(float16_t __a) {
  return __builtin_aarch64_fixuns_trunchfsi_us(__a);
}

FUNC(uint64_t)
vcvth_u64_f16(float16_t __a) {
  return __builtin_aarch64_fixuns_trunchfdi_us(__a);
}

FUNC(int16_t)
vcvtah_s16_f16(float16_t __a) {
  return __builtin_aarch64_lroundhfhi(__a);
}

FUNC(int32_t)
vcvtah_s32_f16(float16_t __a) {
  return __builtin_aarch64_lroundhfsi(__a);
}

FUNC(int64_t)
vcvtah_s64_f16(float16_t __a) {
  return __builtin_aarch64_lroundhfdi(__a);
}

FUNC(uint16_t)
vcvtah_u16_f16(float16_t __a) {
  return __builtin_aarch64_lrounduhfhi_us(__a);
}

FUNC(uint32_t)
vcvtah_u32_f16(float16_t __a) {
  return __builtin_aarch64_lrounduhfsi_us(__a);
}

FUNC(uint64_t)
vcvtah_u64_f16(float16_t __a) {
  return __builtin_aarch64_lrounduhfdi_us(__a);
}

FUNC(int16_t)
vcvtmh_s16_f16(float16_t __a) {
  return __builtin_aarch64_lfloorhfhi(__a);
}

FUNC(int32_t)
vcvtmh_s32_f16(float16_t __a) {
  return __builtin_aarch64_lfloorhfsi(__a);
}

FUNC(int64_t)
vcvtmh_s64_f16(float16_t __a) {
  return __builtin_aarch64_lfloorhfdi(__a);
}

FUNC(uint16_t)
vcvtmh_u16_f16(float16_t __a) {
  return __builtin_aarch64_lflooruhfhi_us(__a);
}

FUNC(uint32_t)
vcvtmh_u32_f16(float16_t __a) {
  return __builtin_aarch64_lflooruhfsi_us(__a);
}

FUNC(uint64_t)
vcvtmh_u64_f16(float16_t __a) {
  return __builtin_aarch64_lflooruhfdi_us(__a);
}

FUNC(int16_t)
vcvtnh_s16_f16(float16_t __a) {
  return __builtin_aarch64_lfrintnhfhi(__a);
}

FUNC(int32_t)
vcvtnh_s32_f16(float16_t __a) {
  return __builtin_aarch64_lfrintnhfsi(__a);
}

FUNC(int64_t)
vcvtnh_s64_f16(float16_t __a) {
  return __builtin_aarch64_lfrintnhfdi(__a);
}

FUNC(uint16_t)
vcvtnh_u16_f16(float16_t __a) {
  return __builtin_aarch64_lfrintnuhfhi_us(__a);
}

FUNC(uint32_t)
vcvtnh_u32_f16(float16_t __a) {
  return __builtin_aarch64_lfrintnuhfsi_us(__a);
}

FUNC(uint64_t)
vcvtnh_u64_f16(float16_t __a) {
  return __builtin_aarch64_lfrintnuhfdi_us(__a);
}

FUNC(int16_t)
vcvtph_s16_f16(float16_t __a) {
  return __builtin_aarch64_lceilhfhi(__a);
}

FUNC(int32_t)
vcvtph_s32_f16(float16_t __a) {
  return __builtin_aarch64_lceilhfsi(__a);
}

FUNC(int64_t)
vcvtph_s64_f16(float16_t __a) {
  return __builtin_aarch64_lceilhfdi(__a);
}

FUNC(uint16_t)
vcvtph_u16_f16(float16_t __a) {
  return __builtin_aarch64_lceiluhfhi_us(__a);
}

FUNC(uint32_t)
vcvtph_u32_f16(float16_t __a) {
  return __builtin_aarch64_lceiluhfsi_us(__a);
}

FUNC(uint64_t)
vcvtph_u64_f16(float16_t __a) {
  return __builtin_aarch64_lceiluhfdi_us(__a);
}

FUNC(float16_t)
vnegh_f16(float16_t __a) {
  return __builtin_aarch64_neghf(__a);
}

FUNC(float16_t)
vrecpeh_f16(float16_t __a) {
  return __builtin_aarch64_frecpehf(__a);
}

FUNC(float16_t)
vrecpxh_f16(float16_t __a) {
  return __builtin_aarch64_frecpxhf(__a);
}

FUNC(float16_t)
vrndh_f16(float16_t __a) {
  return __builtin_aarch64_btrunchf(__a);
}

FUNC(float16_t)
vrndah_f16(float16_t __a) {
  return __builtin_aarch64_roundhf(__a);
}

FUNC(float16_t)
vrndih_f16(float16_t __a) {
  return __builtin_aarch64_nearbyinthf(__a);
}

FUNC(float16_t)
vrndmh_f16(float16_t __a) {
  return __builtin_aarch64_floorhf(__a);
}

FUNC(float16_t)
vrndnh_f16(float16_t __a) {
  return __builtin_aarch64_frintnhf(__a);
}

FUNC(float16_t)
vrndph_f16(float16_t __a) {
  return __builtin_aarch64_ceilhf(__a);
}

FUNC(float16_t)
vrndxh_f16(float16_t __a) {
  return __builtin_aarch64_rinthf(__a);
}

FUNC(float16_t)
vrsqrteh_f16(float16_t __a) {
  return __builtin_aarch64_rsqrtehf(__a);
}

FUNC(float16_t)
vsqrth_f16(float16_t __a) {
  return __builtin_aarch64_sqrthf(__a);
}

FUNC(float16_t)
vaddh_f16(float16_t __a, float16_t __b) {
  return __a + __b;
}

FUNC(float16_t)
vabdh_f16(float16_t __a, float16_t __b) {
  return __builtin_aarch64_fabdhf(__a, __b);
}

FUNC(uint16_t)
vcageh_f16(float16_t __a, float16_t __b) {
  return __builtin_aarch64_facgehf_uss(__a, __b);
}

FUNC(uint16_t)
vcagth_f16(float16_t __a, float16_t __b) {
  return __builtin_aarch64_facgthf_uss(__a, __b);
}

FUNC(uint16_t)
vcaleh_f16(float16_t __a, float16_t __b) {
  return __builtin_aarch64_faclehf_uss(__a, __b);
}

FUNC(uint16_t)
vcalth_f16(float16_t __a, float16_t __b) {
  return __builtin_aarch64_faclthf_uss(__a, __b);
}

FUNC(uint16_t)
vceqh_f16(float16_t __a, float16_t __b) {
  return __builtin_aarch64_cmeqhf_uss(__a, __b);
}

FUNC(uint16_t)
vcgeh_f16(float16_t __a, float16_t __b) {
  return __builtin_aarch64_cmgehf_uss(__a, __b);
}

FUNC(uint16_t)
vcgth_f16(float16_t __a, float16_t __b) {
  return __builtin_aarch64_cmgthf_uss(__a, __b);
}

FUNC(uint16_t)
vcleh_f16(float16_t __a, float16_t __b) {
  return __builtin_aarch64_cmlehf_uss(__a, __b);
}

FUNC(uint16_t)
vclth_f16(float16_t __a, float16_t __b) {
  return __builtin_aarch64_cmlthf_uss(__a, __b);
}

FUNC(float16_t)
vcvth_n_f16_s16(int16_t __a, const int __b) {
  return __builtin_aarch64_scvtfhi(__a, __b);
}

FUNC(float16_t)
vcvth_n_f16_s32(int32_t __a, const int __b) {
  return __builtin_aarch64_scvtfsihf(__a, __b);
}

FUNC(float16_t)
vcvth_n_f16_s64(int64_t __a, const int __b) {
  return __builtin_aarch64_scvtfdihf(__a, __b);
}

FUNC(float16_t)
vcvth_n_f16_u16(uint16_t __a, const int __b) {
  return __builtin_aarch64_ucvtfhi_sus(__a, __b);
}

FUNC(float16_t)
vcvth_n_f16_u32(uint32_t __a, const int __b) {
  return __builtin_aarch64_ucvtfsihf_sus(__a, __b);
}

FUNC(float16_t)
vcvth_n_f16_u64(uint64_t __a, const int __b) {
  return __builtin_aarch64_ucvtfdihf_sus(__a, __b);
}

FUNC(int16_t)
vcvth_n_s16_f16(float16_t __a, const int __b) {
  return __builtin_aarch64_fcvtzshf(__a, __b);
}

FUNC(int32_t)
vcvth_n_s32_f16(float16_t __a, const int __b) {
  return __builtin_aarch64_fcvtzshfsi(__a, __b);
}

FUNC(int64_t)
vcvth_n_s64_f16(float16_t __a, const int __b) {
  return __builtin_aarch64_fcvtzshfdi(__a, __b);
}

FUNC(uint16_t)
vcvth_n_u16_f16(float16_t __a, const int __b) {
  return __builtin_aarch64_fcvtzuhf_uss(__a, __b);
}

FUNC(uint32_t)
vcvth_n_u32_f16(float16_t __a, const int __b) {
  return __builtin_aarch64_fcvtzuhfsi_uss(__a, __b);
}

FUNC(uint64_t)
vcvth_n_u64_f16(float16_t __a, const int __b) {
  return __builtin_aarch64_fcvtzuhfdi_uss(__a, __b);
}

FUNC(float16_t)
vdivh_f16(float16_t __a, float16_t __b) {
  return __a / __b;
}

FUNC(float16_t)
vmaxh_f16(float16_t __a, float16_t __b) {
  return __builtin_aarch64_fmaxhf(__a, __b);
}

FUNC(float16_t)
vmaxnmh_f16(float16_t __a, float16_t __b) {
  return __builtin_aarch64_fmaxhf(__a, __b);
}

FUNC(float16_t)
vminh_f16(float16_t __a, float16_t __b) {
  return __builtin_aarch64_fminhf(__a, __b);
}

FUNC(float16_t)
vminnmh_f16(float16_t __a, float16_t __b) {
  return __builtin_aarch64_fminhf(__a, __b);
}

FUNC(float16_t)
vmulh_f16(float16_t __a, float16_t __b) {
  return __a * __b;
}

FUNC(float16_t)
vmulxh_f16(float16_t __a, float16_t __b) {
  return __builtin_aarch64_fmulxhf(__a, __b);
}

FUNC(float16_t)
vrecpsh_f16(float16_t __a, float16_t __b) {
  return __builtin_aarch64_frecpshf(__a, __b);
}

FUNC(float16_t)
vrsqrtsh_f16(float16_t __a, float16_t __b) {
  return __builtin_aarch64_rsqrtshf(__a, __b);
}

FUNC(float16_t)
vsubh_f16(float16_t __a, float16_t __b) {
  return __a - __b;
}

FUNC(float16_t)
vfmah_f16(float16_t __a, float16_t __b, float16_t __c) {
  return __builtin_aarch64_fmahf(__b, __c, __a);
}

FUNC(float16_t)
vfmsh_f16(float16_t __a, float16_t __b, float16_t __c) {
  return __builtin_aarch64_fnmahf(__b, __c, __a);
}

#pragma GCC pop_options

#undef FUNC
#endif /* __aarch64__ */
#endif /* _AARCH64_FP16_H_ */
