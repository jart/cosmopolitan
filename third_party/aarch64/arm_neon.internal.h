#if defined(__aarch64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _AARCH64_NEON_H_
#define _AARCH64_NEON_H_
#pragma GCC push_options
#pragma GCC target ("+nothing+simd")
#pragma GCC aarch64 "arm_neon.h"
#pragma GCC diagnostic ignored "-Wmissing-braces"
#define __AARCH64_UINT64_C(__C) ((uint64_t) __C)
#define __AARCH64_INT64_C(__C) ((int64_t) __C)
typedef __Int8x8_t int8x8_t;
typedef __Int16x4_t int16x4_t;
typedef __Int32x2_t int32x2_t;
typedef __Int64x1_t int64x1_t;
typedef __Float16x4_t float16x4_t;
typedef __Float32x2_t float32x2_t;
typedef __Poly8x8_t poly8x8_t;
typedef __Poly16x4_t poly16x4_t;
typedef __Uint8x8_t uint8x8_t;
typedef __Uint16x4_t uint16x4_t;
typedef __Uint32x2_t uint32x2_t;
typedef __Float64x1_t float64x1_t;
typedef __Uint64x1_t uint64x1_t;
typedef __Int8x16_t int8x16_t;
typedef __Int16x8_t int16x8_t;
typedef __Int32x4_t int32x4_t;
typedef __Int64x2_t int64x2_t;
typedef __Float16x8_t float16x8_t;
typedef __Float32x4_t float32x4_t;
typedef __Float64x2_t float64x2_t;
typedef __Poly8x16_t poly8x16_t;
typedef __Poly16x8_t poly16x8_t;
typedef __Poly64x2_t poly64x2_t;
typedef __Poly64x1_t poly64x1_t;
typedef __Uint8x16_t uint8x16_t;
typedef __Uint16x8_t uint16x8_t;
typedef __Uint32x4_t uint32x4_t;
typedef __Uint64x2_t uint64x2_t;
typedef __Poly8_t poly8_t;
typedef __Poly16_t poly16_t;
typedef __Poly64_t poly64_t;
typedef __Poly128_t poly128_t;
typedef __fp16 float16_t;
typedef float float32_t;
typedef double float64_t;
typedef __Bfloat16x4_t bfloat16x4_t;
typedef __Bfloat16x8_t bfloat16x8_t;
#define __aarch64_vdup_lane_any(__size, __q, __a, __b) vdup##__q##_n_##__size (__aarch64_vget_lane_any (__a, __b))
#define __aarch64_vdup_lane_f16(__a, __b) __aarch64_vdup_lane_any (f16, , __a, __b)
#define __aarch64_vdup_lane_f32(__a, __b) __aarch64_vdup_lane_any (f32, , __a, __b)
#define __aarch64_vdup_lane_f64(__a, __b) __aarch64_vdup_lane_any (f64, , __a, __b)
#define __aarch64_vdup_lane_p8(__a, __b) __aarch64_vdup_lane_any (p8, , __a, __b)
#define __aarch64_vdup_lane_p16(__a, __b) __aarch64_vdup_lane_any (p16, , __a, __b)
#define __aarch64_vdup_lane_p64(__a, __b) __aarch64_vdup_lane_any (p64, , __a, __b)
#define __aarch64_vdup_lane_s8(__a, __b) __aarch64_vdup_lane_any (s8, , __a, __b)
#define __aarch64_vdup_lane_s16(__a, __b) __aarch64_vdup_lane_any (s16, , __a, __b)
#define __aarch64_vdup_lane_s32(__a, __b) __aarch64_vdup_lane_any (s32, , __a, __b)
#define __aarch64_vdup_lane_s64(__a, __b) __aarch64_vdup_lane_any (s64, , __a, __b)
#define __aarch64_vdup_lane_u8(__a, __b) __aarch64_vdup_lane_any (u8, , __a, __b)
#define __aarch64_vdup_lane_u16(__a, __b) __aarch64_vdup_lane_any (u16, , __a, __b)
#define __aarch64_vdup_lane_u32(__a, __b) __aarch64_vdup_lane_any (u32, , __a, __b)
#define __aarch64_vdup_lane_u64(__a, __b) __aarch64_vdup_lane_any (u64, , __a, __b)
#define __aarch64_vdup_laneq_f16(__a, __b) __aarch64_vdup_lane_any (f16, , __a, __b)
#define __aarch64_vdup_laneq_f32(__a, __b) __aarch64_vdup_lane_any (f32, , __a, __b)
#define __aarch64_vdup_laneq_f64(__a, __b) __aarch64_vdup_lane_any (f64, , __a, __b)
#define __aarch64_vdup_laneq_p8(__a, __b) __aarch64_vdup_lane_any (p8, , __a, __b)
#define __aarch64_vdup_laneq_p16(__a, __b) __aarch64_vdup_lane_any (p16, , __a, __b)
#define __aarch64_vdup_laneq_p64(__a, __b) __aarch64_vdup_lane_any (p64, , __a, __b)
#define __aarch64_vdup_laneq_s8(__a, __b) __aarch64_vdup_lane_any (s8, , __a, __b)
#define __aarch64_vdup_laneq_s16(__a, __b) __aarch64_vdup_lane_any (s16, , __a, __b)
#define __aarch64_vdup_laneq_s32(__a, __b) __aarch64_vdup_lane_any (s32, , __a, __b)
#define __aarch64_vdup_laneq_s64(__a, __b) __aarch64_vdup_lane_any (s64, , __a, __b)
#define __aarch64_vdup_laneq_u8(__a, __b) __aarch64_vdup_lane_any (u8, , __a, __b)
#define __aarch64_vdup_laneq_u16(__a, __b) __aarch64_vdup_lane_any (u16, , __a, __b)
#define __aarch64_vdup_laneq_u32(__a, __b) __aarch64_vdup_lane_any (u32, , __a, __b)
#define __aarch64_vdup_laneq_u64(__a, __b) __aarch64_vdup_lane_any (u64, , __a, __b)
#define __aarch64_vdupq_lane_f16(__a, __b) __aarch64_vdup_lane_any (f16, q, __a, __b)
#define __aarch64_vdupq_lane_f32(__a, __b) __aarch64_vdup_lane_any (f32, q, __a, __b)
#define __aarch64_vdupq_lane_f64(__a, __b) __aarch64_vdup_lane_any (f64, q, __a, __b)
#define __aarch64_vdupq_lane_p8(__a, __b) __aarch64_vdup_lane_any (p8, q, __a, __b)
#define __aarch64_vdupq_lane_p16(__a, __b) __aarch64_vdup_lane_any (p16, q, __a, __b)
#define __aarch64_vdupq_lane_p64(__a, __b) __aarch64_vdup_lane_any (p64, q, __a, __b)
#define __aarch64_vdupq_lane_s8(__a, __b) __aarch64_vdup_lane_any (s8, q, __a, __b)
#define __aarch64_vdupq_lane_s16(__a, __b) __aarch64_vdup_lane_any (s16, q, __a, __b)
#define __aarch64_vdupq_lane_s32(__a, __b) __aarch64_vdup_lane_any (s32, q, __a, __b)
#define __aarch64_vdupq_lane_s64(__a, __b) __aarch64_vdup_lane_any (s64, q, __a, __b)
#define __aarch64_vdupq_lane_u8(__a, __b) __aarch64_vdup_lane_any (u8, q, __a, __b)
#define __aarch64_vdupq_lane_u16(__a, __b) __aarch64_vdup_lane_any (u16, q, __a, __b)
#define __aarch64_vdupq_lane_u32(__a, __b) __aarch64_vdup_lane_any (u32, q, __a, __b)
#define __aarch64_vdupq_lane_u64(__a, __b) __aarch64_vdup_lane_any (u64, q, __a, __b)
#define __aarch64_vdupq_laneq_f16(__a, __b) __aarch64_vdup_lane_any (f16, q, __a, __b)
#define __aarch64_vdupq_laneq_f32(__a, __b) __aarch64_vdup_lane_any (f32, q, __a, __b)
#define __aarch64_vdupq_laneq_f64(__a, __b) __aarch64_vdup_lane_any (f64, q, __a, __b)
#define __aarch64_vdupq_laneq_p8(__a, __b) __aarch64_vdup_lane_any (p8, q, __a, __b)
#define __aarch64_vdupq_laneq_p16(__a, __b) __aarch64_vdup_lane_any (p16, q, __a, __b)
#define __aarch64_vdupq_laneq_p64(__a, __b) __aarch64_vdup_lane_any (p64, q, __a, __b)
#define __aarch64_vdupq_laneq_s8(__a, __b) __aarch64_vdup_lane_any (s8, q, __a, __b)
#define __aarch64_vdupq_laneq_s16(__a, __b) __aarch64_vdup_lane_any (s16, q, __a, __b)
#define __aarch64_vdupq_laneq_s32(__a, __b) __aarch64_vdup_lane_any (s32, q, __a, __b)
#define __aarch64_vdupq_laneq_s64(__a, __b) __aarch64_vdup_lane_any (s64, q, __a, __b)
#define __aarch64_vdupq_laneq_u8(__a, __b) __aarch64_vdup_lane_any (u8, q, __a, __b)
#define __aarch64_vdupq_laneq_u16(__a, __b) __aarch64_vdup_lane_any (u16, q, __a, __b)
#define __aarch64_vdupq_laneq_u32(__a, __b) __aarch64_vdup_lane_any (u32, q, __a, __b)
#define __aarch64_vdupq_laneq_u64(__a, __b) __aarch64_vdup_lane_any (u64, q, __a, __b)
#define __AARCH64_NUM_LANES(__v) (sizeof (__v) / sizeof (__v[0]))
#define __AARCH64_LANE_CHECK(__vec, __idx) __builtin_aarch64_im_lane_boundsi (sizeof(__vec), sizeof(__vec[0]), __idx)
#ifdef __AARCH64EB__
#define __aarch64_lane(__vec, __idx) (__AARCH64_NUM_LANES (__vec) - 1 - __idx)
#else
#define __aarch64_lane(__vec, __idx) __idx
#endif
#define __aarch64_vget_lane_any(__vec, __index) __extension__ ({ __AARCH64_LANE_CHECK (__vec, __index); __vec[__aarch64_lane (__vec, __index)]; })
#define __aarch64_vset_lane_any(__elem, __vec, __index) __extension__ ({ __AARCH64_LANE_CHECK (__vec, __index); __vec[__aarch64_lane (__vec, __index)] = __elem; __vec; })
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vadd_s8 (int8x8_t __a, int8x8_t __b)
{
  return __a + __b;
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vadd_s16 (int16x4_t __a, int16x4_t __b)
{
  return __a + __b;
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vadd_s32 (int32x2_t __a, int32x2_t __b)
{
  return __a + __b;
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vadd_f32 (float32x2_t __a, float32x2_t __b)
{
  return __a + __b;
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vadd_f64 (float64x1_t __a, float64x1_t __b)
{
  return __a + __b;
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vadd_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return __a + __b;
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vadd_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return __a + __b;
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vadd_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return __a + __b;
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vadd_s64 (int64x1_t __a, int64x1_t __b)
{
  return __a + __b;
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vadd_u64 (uint64x1_t __a, uint64x1_t __b)
{
  return __a + __b;
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddq_s8 (int8x16_t __a, int8x16_t __b)
{
  return __a + __b;
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddq_s16 (int16x8_t __a, int16x8_t __b)
{
  return __a + __b;
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddq_s32 (int32x4_t __a, int32x4_t __b)
{
  return __a + __b;
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddq_s64 (int64x2_t __a, int64x2_t __b)
{
  return __a + __b;
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddq_f32 (float32x4_t __a, float32x4_t __b)
{
  return __a + __b;
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddq_f64 (float64x2_t __a, float64x2_t __b)
{
  return __a + __b;
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return __a + __b;
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return __a + __b;
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return __a + __b;
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddq_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return __a + __b;
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddl_s8 (int8x8_t __a, int8x8_t __b)
{
  return __builtin_aarch64_saddlv8qi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddl_s16 (int16x4_t __a, int16x4_t __b)
{
  return __builtin_aarch64_saddlv4hi (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddl_s32 (int32x2_t __a, int32x2_t __b)
{
  return __builtin_aarch64_saddlv2si (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddl_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return __builtin_aarch64_uaddlv8qi_uuu (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddl_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return __builtin_aarch64_uaddlv4hi_uuu (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddl_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return __builtin_aarch64_uaddlv2si_uuu (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddl_high_s8 (int8x16_t __a, int8x16_t __b)
{
  return __builtin_aarch64_saddl2v16qi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddl_high_s16 (int16x8_t __a, int16x8_t __b)
{
  return __builtin_aarch64_saddl2v8hi (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddl_high_s32 (int32x4_t __a, int32x4_t __b)
{
  return __builtin_aarch64_saddl2v4si (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddl_high_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return __builtin_aarch64_uaddl2v16qi_uuu (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddl_high_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return __builtin_aarch64_uaddl2v8hi_uuu (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddl_high_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return __builtin_aarch64_uaddl2v4si_uuu (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddw_s8 (int16x8_t __a, int8x8_t __b)
{
  return __builtin_aarch64_saddwv8qi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddw_s16 (int32x4_t __a, int16x4_t __b)
{
  return __builtin_aarch64_saddwv4hi (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddw_s32 (int64x2_t __a, int32x2_t __b)
{
  return __builtin_aarch64_saddwv2si (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddw_u8 (uint16x8_t __a, uint8x8_t __b)
{
  return __builtin_aarch64_uaddwv8qi_uuu (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddw_u16 (uint32x4_t __a, uint16x4_t __b)
{
  return __builtin_aarch64_uaddwv4hi_uuu (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddw_u32 (uint64x2_t __a, uint32x2_t __b)
{
  return __builtin_aarch64_uaddwv2si_uuu (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddw_high_s8 (int16x8_t __a, int8x16_t __b)
{
  return __builtin_aarch64_saddw2v16qi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddw_high_s16 (int32x4_t __a, int16x8_t __b)
{
  return __builtin_aarch64_saddw2v8hi (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddw_high_s32 (int64x2_t __a, int32x4_t __b)
{
  return __builtin_aarch64_saddw2v4si (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddw_high_u8 (uint16x8_t __a, uint8x16_t __b)
{
  return __builtin_aarch64_uaddw2v16qi_uuu (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddw_high_u16 (uint32x4_t __a, uint16x8_t __b)
{
  return __builtin_aarch64_uaddw2v8hi_uuu (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddw_high_u32 (uint64x2_t __a, uint32x4_t __b)
{
  return __builtin_aarch64_uaddw2v4si_uuu (__a, __b);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vhadd_s8 (int8x8_t __a, int8x8_t __b)
{
  return __builtin_aarch64_shaddv8qi (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vhadd_s16 (int16x4_t __a, int16x4_t __b)
{
  return __builtin_aarch64_shaddv4hi (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vhadd_s32 (int32x2_t __a, int32x2_t __b)
{
  return __builtin_aarch64_shaddv2si (__a, __b);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vhadd_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return __builtin_aarch64_uhaddv8qi_uuu (__a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vhadd_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return __builtin_aarch64_uhaddv4hi_uuu (__a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vhadd_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return __builtin_aarch64_uhaddv2si_uuu (__a, __b);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vhaddq_s8 (int8x16_t __a, int8x16_t __b)
{
  return __builtin_aarch64_shaddv16qi (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vhaddq_s16 (int16x8_t __a, int16x8_t __b)
{
  return __builtin_aarch64_shaddv8hi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vhaddq_s32 (int32x4_t __a, int32x4_t __b)
{
  return __builtin_aarch64_shaddv4si (__a, __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vhaddq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return __builtin_aarch64_uhaddv16qi_uuu (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vhaddq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return __builtin_aarch64_uhaddv8hi_uuu (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vhaddq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return __builtin_aarch64_uhaddv4si_uuu (__a, __b);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrhadd_s8 (int8x8_t __a, int8x8_t __b)
{
  return __builtin_aarch64_srhaddv8qi (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrhadd_s16 (int16x4_t __a, int16x4_t __b)
{
  return __builtin_aarch64_srhaddv4hi (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrhadd_s32 (int32x2_t __a, int32x2_t __b)
{
  return __builtin_aarch64_srhaddv2si (__a, __b);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrhadd_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return __builtin_aarch64_urhaddv8qi_uuu (__a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrhadd_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return __builtin_aarch64_urhaddv4hi_uuu (__a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrhadd_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return __builtin_aarch64_urhaddv2si_uuu (__a, __b);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrhaddq_s8 (int8x16_t __a, int8x16_t __b)
{
  return __builtin_aarch64_srhaddv16qi (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrhaddq_s16 (int16x8_t __a, int16x8_t __b)
{
  return __builtin_aarch64_srhaddv8hi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrhaddq_s32 (int32x4_t __a, int32x4_t __b)
{
  return __builtin_aarch64_srhaddv4si (__a, __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrhaddq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return __builtin_aarch64_urhaddv16qi_uuu (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrhaddq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return __builtin_aarch64_urhaddv8hi_uuu (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrhaddq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return __builtin_aarch64_urhaddv4si_uuu (__a, __b);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddhn_s16 (int16x8_t __a, int16x8_t __b)
{
  return __builtin_aarch64_addhnv8hi (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddhn_s32 (int32x4_t __a, int32x4_t __b)
{
  return __builtin_aarch64_addhnv4si (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddhn_s64 (int64x2_t __a, int64x2_t __b)
{
  return __builtin_aarch64_addhnv2di (__a, __b);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddhn_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return __builtin_aarch64_addhnv8hi_uuu (__a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddhn_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return __builtin_aarch64_addhnv4si_uuu (__a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddhn_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return __builtin_aarch64_addhnv2di_uuu (__a, __b);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vraddhn_s16 (int16x8_t __a, int16x8_t __b)
{
  return __builtin_aarch64_raddhnv8hi (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vraddhn_s32 (int32x4_t __a, int32x4_t __b)
{
  return __builtin_aarch64_raddhnv4si (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vraddhn_s64 (int64x2_t __a, int64x2_t __b)
{
  return __builtin_aarch64_raddhnv2di (__a, __b);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vraddhn_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return __builtin_aarch64_raddhnv8hi_uuu (__a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vraddhn_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return __builtin_aarch64_raddhnv4si_uuu (__a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vraddhn_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return __builtin_aarch64_raddhnv2di_uuu (__a, __b);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddhn_high_s16 (int8x8_t __a, int16x8_t __b, int16x8_t __c)
{
  return __builtin_aarch64_addhn2v8hi (__a, __b, __c);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddhn_high_s32 (int16x4_t __a, int32x4_t __b, int32x4_t __c)
{
  return __builtin_aarch64_addhn2v4si (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddhn_high_s64 (int32x2_t __a, int64x2_t __b, int64x2_t __c)
{
  return __builtin_aarch64_addhn2v2di (__a, __b, __c);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddhn_high_u16 (uint8x8_t __a, uint16x8_t __b, uint16x8_t __c)
{
  return __builtin_aarch64_addhn2v8hi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddhn_high_u32 (uint16x4_t __a, uint32x4_t __b, uint32x4_t __c)
{
  return __builtin_aarch64_addhn2v4si_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddhn_high_u64 (uint32x2_t __a, uint64x2_t __b, uint64x2_t __c)
{
  return __builtin_aarch64_addhn2v2di_uuuu (__a, __b, __c);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vraddhn_high_s16 (int8x8_t __a, int16x8_t __b, int16x8_t __c)
{
  return __builtin_aarch64_raddhn2v8hi (__a, __b, __c);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vraddhn_high_s32 (int16x4_t __a, int32x4_t __b, int32x4_t __c)
{
  return __builtin_aarch64_raddhn2v4si (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vraddhn_high_s64 (int32x2_t __a, int64x2_t __b, int64x2_t __c)
{
  return __builtin_aarch64_raddhn2v2di (__a, __b, __c);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vraddhn_high_u16 (uint8x8_t __a, uint16x8_t __b, uint16x8_t __c)
{
  return __builtin_aarch64_raddhn2v8hi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vraddhn_high_u32 (uint16x4_t __a, uint32x4_t __b, uint32x4_t __c)
{
  return __builtin_aarch64_raddhn2v4si_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vraddhn_high_u64 (uint32x2_t __a, uint64x2_t __b, uint64x2_t __c)
{
  return __builtin_aarch64_raddhn2v2di_uuuu (__a, __b, __c);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdiv_f32 (float32x2_t __a, float32x2_t __b)
{
  return __a / __b;
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdiv_f64 (float64x1_t __a, float64x1_t __b)
{
  return __a / __b;
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdivq_f32 (float32x4_t __a, float32x4_t __b)
{
  return __a / __b;
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdivq_f64 (float64x2_t __a, float64x2_t __b)
{
  return __a / __b;
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmul_s8 (int8x8_t __a, int8x8_t __b)
{
  return __a * __b;
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmul_s16 (int16x4_t __a, int16x4_t __b)
{
  return __a * __b;
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmul_s32 (int32x2_t __a, int32x2_t __b)
{
  return __a * __b;
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmul_f32 (float32x2_t __a, float32x2_t __b)
{
  return __a * __b;
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmul_f64 (float64x1_t __a, float64x1_t __b)
{
  return __a * __b;
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmul_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return __a * __b;
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmul_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return __a * __b;
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmul_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return __a * __b;
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmul_p8 (poly8x8_t __a, poly8x8_t __b)
{
  return __builtin_aarch64_pmulv8qi_ppp (__a, __b);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulq_s8 (int8x16_t __a, int8x16_t __b)
{
  return __a * __b;
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulq_s16 (int16x8_t __a, int16x8_t __b)
{
  return __a * __b;
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulq_s32 (int32x4_t __a, int32x4_t __b)
{
  return __a * __b;
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulq_f32 (float32x4_t __a, float32x4_t __b)
{
  return __a * __b;
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulq_f64 (float64x2_t __a, float64x2_t __b)
{
  return __a * __b;
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return __a * __b;
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return __a * __b;
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return __a * __b;
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulq_p8 (poly8x16_t __a, poly8x16_t __b)
{
  return __builtin_aarch64_pmulv16qi_ppp (__a, __b);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vand_s8 (int8x8_t __a, int8x8_t __b)
{
  return __a & __b;
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vand_s16 (int16x4_t __a, int16x4_t __b)
{
  return __a & __b;
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vand_s32 (int32x2_t __a, int32x2_t __b)
{
  return __a & __b;
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vand_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return __a & __b;
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vand_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return __a & __b;
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vand_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return __a & __b;
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vand_s64 (int64x1_t __a, int64x1_t __b)
{
  return __a & __b;
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vand_u64 (uint64x1_t __a, uint64x1_t __b)
{
  return __a & __b;
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vandq_s8 (int8x16_t __a, int8x16_t __b)
{
  return __a & __b;
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vandq_s16 (int16x8_t __a, int16x8_t __b)
{
  return __a & __b;
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vandq_s32 (int32x4_t __a, int32x4_t __b)
{
  return __a & __b;
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vandq_s64 (int64x2_t __a, int64x2_t __b)
{
  return __a & __b;
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vandq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return __a & __b;
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vandq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return __a & __b;
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vandq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return __a & __b;
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vandq_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return __a & __b;
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vorr_s8 (int8x8_t __a, int8x8_t __b)
{
  return __a | __b;
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vorr_s16 (int16x4_t __a, int16x4_t __b)
{
  return __a | __b;
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vorr_s32 (int32x2_t __a, int32x2_t __b)
{
  return __a | __b;
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vorr_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return __a | __b;
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vorr_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return __a | __b;
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vorr_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return __a | __b;
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vorr_s64 (int64x1_t __a, int64x1_t __b)
{
  return __a | __b;
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vorr_u64 (uint64x1_t __a, uint64x1_t __b)
{
  return __a | __b;
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vorrq_s8 (int8x16_t __a, int8x16_t __b)
{
  return __a | __b;
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vorrq_s16 (int16x8_t __a, int16x8_t __b)
{
  return __a | __b;
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vorrq_s32 (int32x4_t __a, int32x4_t __b)
{
  return __a | __b;
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vorrq_s64 (int64x2_t __a, int64x2_t __b)
{
  return __a | __b;
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vorrq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return __a | __b;
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vorrq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return __a | __b;
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vorrq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return __a | __b;
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vorrq_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return __a | __b;
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
veor_s8 (int8x8_t __a, int8x8_t __b)
{
  return __a ^ __b;
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
veor_s16 (int16x4_t __a, int16x4_t __b)
{
  return __a ^ __b;
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
veor_s32 (int32x2_t __a, int32x2_t __b)
{
  return __a ^ __b;
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
veor_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return __a ^ __b;
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
veor_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return __a ^ __b;
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
veor_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return __a ^ __b;
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
veor_s64 (int64x1_t __a, int64x1_t __b)
{
  return __a ^ __b;
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
veor_u64 (uint64x1_t __a, uint64x1_t __b)
{
  return __a ^ __b;
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
veorq_s8 (int8x16_t __a, int8x16_t __b)
{
  return __a ^ __b;
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
veorq_s16 (int16x8_t __a, int16x8_t __b)
{
  return __a ^ __b;
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
veorq_s32 (int32x4_t __a, int32x4_t __b)
{
  return __a ^ __b;
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
veorq_s64 (int64x2_t __a, int64x2_t __b)
{
  return __a ^ __b;
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
veorq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return __a ^ __b;
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
veorq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return __a ^ __b;
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
veorq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return __a ^ __b;
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
veorq_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return __a ^ __b;
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbic_s8 (int8x8_t __a, int8x8_t __b)
{
  return __a & ~__b;
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbic_s16 (int16x4_t __a, int16x4_t __b)
{
  return __a & ~__b;
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbic_s32 (int32x2_t __a, int32x2_t __b)
{
  return __a & ~__b;
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbic_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return __a & ~__b;
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbic_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return __a & ~__b;
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbic_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return __a & ~__b;
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbic_s64 (int64x1_t __a, int64x1_t __b)
{
  return __a & ~__b;
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbic_u64 (uint64x1_t __a, uint64x1_t __b)
{
  return __a & ~__b;
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbicq_s8 (int8x16_t __a, int8x16_t __b)
{
  return __a & ~__b;
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbicq_s16 (int16x8_t __a, int16x8_t __b)
{
  return __a & ~__b;
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbicq_s32 (int32x4_t __a, int32x4_t __b)
{
  return __a & ~__b;
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbicq_s64 (int64x2_t __a, int64x2_t __b)
{
  return __a & ~__b;
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbicq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return __a & ~__b;
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbicq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return __a & ~__b;
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbicq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return __a & ~__b;
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbicq_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return __a & ~__b;
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vorn_s8 (int8x8_t __a, int8x8_t __b)
{
  return __a | ~__b;
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vorn_s16 (int16x4_t __a, int16x4_t __b)
{
  return __a | ~__b;
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vorn_s32 (int32x2_t __a, int32x2_t __b)
{
  return __a | ~__b;
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vorn_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return __a | ~__b;
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vorn_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return __a | ~__b;
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vorn_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return __a | ~__b;
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vorn_s64 (int64x1_t __a, int64x1_t __b)
{
  return __a | ~__b;
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vorn_u64 (uint64x1_t __a, uint64x1_t __b)
{
  return __a | ~__b;
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vornq_s8 (int8x16_t __a, int8x16_t __b)
{
  return __a | ~__b;
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vornq_s16 (int16x8_t __a, int16x8_t __b)
{
  return __a | ~__b;
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vornq_s32 (int32x4_t __a, int32x4_t __b)
{
  return __a | ~__b;
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vornq_s64 (int64x2_t __a, int64x2_t __b)
{
  return __a | ~__b;
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vornq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return __a | ~__b;
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vornq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return __a | ~__b;
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vornq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return __a | ~__b;
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vornq_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return __a | ~__b;
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsub_s8 (int8x8_t __a, int8x8_t __b)
{
  return __a - __b;
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsub_s16 (int16x4_t __a, int16x4_t __b)
{
  return __a - __b;
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsub_s32 (int32x2_t __a, int32x2_t __b)
{
  return __a - __b;
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsub_f32 (float32x2_t __a, float32x2_t __b)
{
  return __a - __b;
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsub_f64 (float64x1_t __a, float64x1_t __b)
{
  return __a - __b;
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsub_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return __a - __b;
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsub_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return __a - __b;
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsub_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return __a - __b;
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsub_s64 (int64x1_t __a, int64x1_t __b)
{
  return __a - __b;
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsub_u64 (uint64x1_t __a, uint64x1_t __b)
{
  return __a - __b;
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubq_s8 (int8x16_t __a, int8x16_t __b)
{
  return __a - __b;
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubq_s16 (int16x8_t __a, int16x8_t __b)
{
  return __a - __b;
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubq_s32 (int32x4_t __a, int32x4_t __b)
{
  return __a - __b;
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubq_s64 (int64x2_t __a, int64x2_t __b)
{
  return __a - __b;
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubq_f32 (float32x4_t __a, float32x4_t __b)
{
  return __a - __b;
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubq_f64 (float64x2_t __a, float64x2_t __b)
{
  return __a - __b;
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return __a - __b;
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return __a - __b;
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return __a - __b;
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubq_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return __a - __b;
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubl_s8 (int8x8_t __a, int8x8_t __b)
{
  return __builtin_aarch64_ssublv8qi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubl_s16 (int16x4_t __a, int16x4_t __b)
{
  return __builtin_aarch64_ssublv4hi (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubl_s32 (int32x2_t __a, int32x2_t __b)
{
  return __builtin_aarch64_ssublv2si (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubl_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return __builtin_aarch64_usublv8qi_uuu (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubl_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return __builtin_aarch64_usublv4hi_uuu (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubl_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return __builtin_aarch64_usublv2si_uuu (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubl_high_s8 (int8x16_t __a, int8x16_t __b)
{
  return __builtin_aarch64_ssubl2v16qi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubl_high_s16 (int16x8_t __a, int16x8_t __b)
{
  return __builtin_aarch64_ssubl2v8hi (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubl_high_s32 (int32x4_t __a, int32x4_t __b)
{
  return __builtin_aarch64_ssubl2v4si (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubl_high_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return __builtin_aarch64_usubl2v16qi_uuu (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubl_high_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return __builtin_aarch64_usubl2v8hi_uuu (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubl_high_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return __builtin_aarch64_usubl2v4si_uuu (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubw_s8 (int16x8_t __a, int8x8_t __b)
{
  return __builtin_aarch64_ssubwv8qi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubw_s16 (int32x4_t __a, int16x4_t __b)
{
  return __builtin_aarch64_ssubwv4hi (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubw_s32 (int64x2_t __a, int32x2_t __b)
{
  return __builtin_aarch64_ssubwv2si (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubw_u8 (uint16x8_t __a, uint8x8_t __b)
{
  return __builtin_aarch64_usubwv8qi_uuu (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubw_u16 (uint32x4_t __a, uint16x4_t __b)
{
  return __builtin_aarch64_usubwv4hi_uuu (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubw_u32 (uint64x2_t __a, uint32x2_t __b)
{
  return __builtin_aarch64_usubwv2si_uuu (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubw_high_s8 (int16x8_t __a, int8x16_t __b)
{
  return __builtin_aarch64_ssubw2v16qi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubw_high_s16 (int32x4_t __a, int16x8_t __b)
{
  return __builtin_aarch64_ssubw2v8hi (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubw_high_s32 (int64x2_t __a, int32x4_t __b)
{
  return __builtin_aarch64_ssubw2v4si (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubw_high_u8 (uint16x8_t __a, uint8x16_t __b)
{
  return __builtin_aarch64_usubw2v16qi_uuu (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubw_high_u16 (uint32x4_t __a, uint16x8_t __b)
{
  return __builtin_aarch64_usubw2v8hi_uuu (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubw_high_u32 (uint64x2_t __a, uint32x4_t __b)
{
  return __builtin_aarch64_usubw2v4si_uuu (__a, __b);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqadd_s8 (int8x8_t __a, int8x8_t __b)
{
  return (int8x8_t) __builtin_aarch64_sqaddv8qi (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqadd_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int16x4_t) __builtin_aarch64_sqaddv4hi (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqadd_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int32x2_t) __builtin_aarch64_sqaddv2si (__a, __b);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqadd_s64 (int64x1_t __a, int64x1_t __b)
{
  return (int64x1_t) {__builtin_aarch64_sqadddi (__a[0], __b[0])};
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqadd_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return __builtin_aarch64_uqaddv8qi_uuu (__a, __b);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vhsub_s8 (int8x8_t __a, int8x8_t __b)
{
  return __builtin_aarch64_shsubv8qi (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vhsub_s16 (int16x4_t __a, int16x4_t __b)
{
  return __builtin_aarch64_shsubv4hi (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vhsub_s32 (int32x2_t __a, int32x2_t __b)
{
  return __builtin_aarch64_shsubv2si (__a, __b);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vhsub_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return __builtin_aarch64_uhsubv8qi_uuu (__a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vhsub_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return __builtin_aarch64_uhsubv4hi_uuu (__a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vhsub_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return __builtin_aarch64_uhsubv2si_uuu (__a, __b);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vhsubq_s8 (int8x16_t __a, int8x16_t __b)
{
  return __builtin_aarch64_shsubv16qi (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vhsubq_s16 (int16x8_t __a, int16x8_t __b)
{
  return __builtin_aarch64_shsubv8hi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vhsubq_s32 (int32x4_t __a, int32x4_t __b)
{
  return __builtin_aarch64_shsubv4si (__a, __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vhsubq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return __builtin_aarch64_uhsubv16qi_uuu (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vhsubq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return __builtin_aarch64_uhsubv8hi_uuu (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vhsubq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return __builtin_aarch64_uhsubv4si_uuu (__a, __b);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubhn_s16 (int16x8_t __a, int16x8_t __b)
{
  return __builtin_aarch64_subhnv8hi (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubhn_s32 (int32x4_t __a, int32x4_t __b)
{
  return __builtin_aarch64_subhnv4si (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubhn_s64 (int64x2_t __a, int64x2_t __b)
{
  return __builtin_aarch64_subhnv2di (__a, __b);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubhn_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return __builtin_aarch64_subhnv8hi_uuu (__a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubhn_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return __builtin_aarch64_subhnv4si_uuu (__a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubhn_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return __builtin_aarch64_subhnv2di_uuu (__a, __b);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsubhn_s16 (int16x8_t __a, int16x8_t __b)
{
  return __builtin_aarch64_rsubhnv8hi (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsubhn_s32 (int32x4_t __a, int32x4_t __b)
{
  return __builtin_aarch64_rsubhnv4si (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsubhn_s64 (int64x2_t __a, int64x2_t __b)
{
  return __builtin_aarch64_rsubhnv2di (__a, __b);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsubhn_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return __builtin_aarch64_rsubhnv8hi_uuu (__a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsubhn_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return __builtin_aarch64_rsubhnv4si_uuu (__a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsubhn_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return __builtin_aarch64_rsubhnv2di_uuu (__a, __b);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsubhn_high_s16 (int8x8_t __a, int16x8_t __b, int16x8_t __c)
{
  return __builtin_aarch64_rsubhn2v8hi (__a, __b, __c);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsubhn_high_s32 (int16x4_t __a, int32x4_t __b, int32x4_t __c)
{
  return __builtin_aarch64_rsubhn2v4si (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsubhn_high_s64 (int32x2_t __a, int64x2_t __b, int64x2_t __c)
{
  return __builtin_aarch64_rsubhn2v2di (__a, __b, __c);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsubhn_high_u16 (uint8x8_t __a, uint16x8_t __b, uint16x8_t __c)
{
  return __builtin_aarch64_rsubhn2v8hi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsubhn_high_u32 (uint16x4_t __a, uint32x4_t __b, uint32x4_t __c)
{
  return __builtin_aarch64_rsubhn2v4si_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsubhn_high_u64 (uint32x2_t __a, uint64x2_t __b, uint64x2_t __c)
{
  return __builtin_aarch64_rsubhn2v2di_uuuu (__a, __b, __c);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubhn_high_s16 (int8x8_t __a, int16x8_t __b, int16x8_t __c)
{
  return __builtin_aarch64_subhn2v8hi (__a, __b, __c);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubhn_high_s32 (int16x4_t __a, int32x4_t __b, int32x4_t __c)
{
  return __builtin_aarch64_subhn2v4si (__a, __b, __c);;
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubhn_high_s64 (int32x2_t __a, int64x2_t __b, int64x2_t __c)
{
  return __builtin_aarch64_subhn2v2di (__a, __b, __c);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubhn_high_u16 (uint8x8_t __a, uint16x8_t __b, uint16x8_t __c)
{
  return __builtin_aarch64_subhn2v8hi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubhn_high_u32 (uint16x4_t __a, uint32x4_t __b, uint32x4_t __c)
{
  return __builtin_aarch64_subhn2v4si_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubhn_high_u64 (uint32x2_t __a, uint64x2_t __b, uint64x2_t __c)
{
  return __builtin_aarch64_subhn2v2di_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqadd_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return __builtin_aarch64_uqaddv4hi_uuu (__a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqadd_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return __builtin_aarch64_uqaddv2si_uuu (__a, __b);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqadd_u64 (uint64x1_t __a, uint64x1_t __b)
{
  return (uint64x1_t) {__builtin_aarch64_uqadddi_uuu (__a[0], __b[0])};
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqaddq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (int8x16_t) __builtin_aarch64_sqaddv16qi (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqaddq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (int16x8_t) __builtin_aarch64_sqaddv8hi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqaddq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (int32x4_t) __builtin_aarch64_sqaddv4si (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqaddq_s64 (int64x2_t __a, int64x2_t __b)
{
  return (int64x2_t) __builtin_aarch64_sqaddv2di (__a, __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqaddq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return __builtin_aarch64_uqaddv16qi_uuu (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqaddq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return __builtin_aarch64_uqaddv8hi_uuu (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqaddq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return __builtin_aarch64_uqaddv4si_uuu (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqaddq_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return __builtin_aarch64_uqaddv2di_uuu (__a, __b);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqsub_s8 (int8x8_t __a, int8x8_t __b)
{
  return (int8x8_t) __builtin_aarch64_sqsubv8qi (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqsub_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int16x4_t) __builtin_aarch64_sqsubv4hi (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqsub_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int32x2_t) __builtin_aarch64_sqsubv2si (__a, __b);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqsub_s64 (int64x1_t __a, int64x1_t __b)
{
  return (int64x1_t) {__builtin_aarch64_sqsubdi (__a[0], __b[0])};
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqsub_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return __builtin_aarch64_uqsubv8qi_uuu (__a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqsub_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return __builtin_aarch64_uqsubv4hi_uuu (__a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqsub_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return __builtin_aarch64_uqsubv2si_uuu (__a, __b);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqsub_u64 (uint64x1_t __a, uint64x1_t __b)
{
  return (uint64x1_t) {__builtin_aarch64_uqsubdi_uuu (__a[0], __b[0])};
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqsubq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (int8x16_t) __builtin_aarch64_sqsubv16qi (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqsubq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (int16x8_t) __builtin_aarch64_sqsubv8hi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqsubq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (int32x4_t) __builtin_aarch64_sqsubv4si (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqsubq_s64 (int64x2_t __a, int64x2_t __b)
{
  return (int64x2_t) __builtin_aarch64_sqsubv2di (__a, __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqsubq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return __builtin_aarch64_uqsubv16qi_uuu (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqsubq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return __builtin_aarch64_uqsubv8hi_uuu (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqsubq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return __builtin_aarch64_uqsubv4si_uuu (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqsubq_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return __builtin_aarch64_uqsubv2di_uuu (__a, __b);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqneg_s8 (int8x8_t __a)
{
  return (int8x8_t) __builtin_aarch64_sqnegv8qi (__a);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqneg_s16 (int16x4_t __a)
{
  return (int16x4_t) __builtin_aarch64_sqnegv4hi (__a);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqneg_s32 (int32x2_t __a)
{
  return (int32x2_t) __builtin_aarch64_sqnegv2si (__a);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqneg_s64 (int64x1_t __a)
{
  return (int64x1_t) {__builtin_aarch64_sqnegdi (__a[0])};
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqnegq_s8 (int8x16_t __a)
{
  return (int8x16_t) __builtin_aarch64_sqnegv16qi (__a);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqnegq_s16 (int16x8_t __a)
{
  return (int16x8_t) __builtin_aarch64_sqnegv8hi (__a);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqnegq_s32 (int32x4_t __a)
{
  return (int32x4_t) __builtin_aarch64_sqnegv4si (__a);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqabs_s8 (int8x8_t __a)
{
  return (int8x8_t) __builtin_aarch64_sqabsv8qi (__a);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqabs_s16 (int16x4_t __a)
{
  return (int16x4_t) __builtin_aarch64_sqabsv4hi (__a);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqabs_s32 (int32x2_t __a)
{
  return (int32x2_t) __builtin_aarch64_sqabsv2si (__a);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqabs_s64 (int64x1_t __a)
{
  return (int64x1_t) {__builtin_aarch64_sqabsdi (__a[0])};
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqabsq_s8 (int8x16_t __a)
{
  return (int8x16_t) __builtin_aarch64_sqabsv16qi (__a);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqabsq_s16 (int16x8_t __a)
{
  return (int16x8_t) __builtin_aarch64_sqabsv8hi (__a);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqabsq_s32 (int32x4_t __a)
{
  return (int32x4_t) __builtin_aarch64_sqabsv4si (__a);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmulh_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int16x4_t) __builtin_aarch64_sqdmulhv4hi (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmulh_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int32x2_t) __builtin_aarch64_sqdmulhv2si (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmulhq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (int16x8_t) __builtin_aarch64_sqdmulhv8hi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmulhq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (int32x4_t) __builtin_aarch64_sqdmulhv4si (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmulh_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int16x4_t) __builtin_aarch64_sqrdmulhv4hi (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmulh_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int32x2_t) __builtin_aarch64_sqrdmulhv2si (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmulhq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (int16x8_t) __builtin_aarch64_sqrdmulhv8hi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmulhq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (int32x4_t) __builtin_aarch64_sqrdmulhv4si (__a, __b);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcreate_s8 (uint64_t __a)
{
  return (int8x8_t) __a;
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcreate_s16 (uint64_t __a)
{
  return (int16x4_t) __a;
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcreate_s32 (uint64_t __a)
{
  return (int32x2_t) __a;
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcreate_s64 (uint64_t __a)
{
  return (int64x1_t) {__a};
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcreate_f16 (uint64_t __a)
{
  return (float16x4_t) __a;
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcreate_f32 (uint64_t __a)
{
  return (float32x2_t) __a;
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcreate_u8 (uint64_t __a)
{
  return (uint8x8_t) __a;
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcreate_u16 (uint64_t __a)
{
  return (uint16x4_t) __a;
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcreate_u32 (uint64_t __a)
{
  return (uint32x2_t) __a;
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcreate_u64 (uint64_t __a)
{
  return (uint64x1_t) {__a};
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcreate_f64 (uint64_t __a)
{
  return (float64x1_t) __a;
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcreate_p8 (uint64_t __a)
{
  return (poly8x8_t) __a;
}
__extension__ extern __inline poly16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcreate_p16 (uint64_t __a)
{
  return (poly16x4_t) __a;
}
__extension__ extern __inline poly64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcreate_p64 (uint64_t __a)
{
  return (poly64x1_t) __a;
}
__extension__ extern __inline float16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_lane_f16 (float16x4_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_lane_f32 (float32x2_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline float64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_lane_f64 (float64x1_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline poly8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_lane_p8 (poly8x8_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline poly16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_lane_p16 (poly16x4_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline poly64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_lane_p64 (poly64x1_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline int8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_lane_s8 (int8x8_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_lane_s16 (int16x4_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_lane_s32 (int32x2_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_lane_s64 (int64x1_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline uint8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_lane_u8 (uint8x8_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline uint16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_lane_u16 (uint16x4_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_lane_u32 (uint32x2_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_lane_u64 (uint64x1_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline float16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vgetq_lane_f16 (float16x8_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vgetq_lane_f32 (float32x4_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline float64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vgetq_lane_f64 (float64x2_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline poly8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vgetq_lane_p8 (poly8x16_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline poly16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vgetq_lane_p16 (poly16x8_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline poly64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vgetq_lane_p64 (poly64x2_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline int8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vgetq_lane_s8 (int8x16_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vgetq_lane_s16 (int16x8_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vgetq_lane_s32 (int32x4_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vgetq_lane_s64 (int64x2_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline uint8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vgetq_lane_u8 (uint8x16_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline uint16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vgetq_lane_u16 (uint16x8_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vgetq_lane_u32 (uint32x4_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vgetq_lane_u64 (uint64x2_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vset_lane_f16 (float16_t __elem, float16x4_t __vec, const int __index)
{
  return __aarch64_vset_lane_any (__elem, __vec, __index);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vset_lane_f32 (float32_t __elem, float32x2_t __vec, const int __index)
{
  return __aarch64_vset_lane_any (__elem, __vec, __index);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vset_lane_f64 (float64_t __elem, float64x1_t __vec, const int __index)
{
  return __aarch64_vset_lane_any (__elem, __vec, __index);
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vset_lane_p8 (poly8_t __elem, poly8x8_t __vec, const int __index)
{
  return __aarch64_vset_lane_any (__elem, __vec, __index);
}
__extension__ extern __inline poly16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vset_lane_p16 (poly16_t __elem, poly16x4_t __vec, const int __index)
{
  return __aarch64_vset_lane_any (__elem, __vec, __index);
}
__extension__ extern __inline poly64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vset_lane_p64 (poly64_t __elem, poly64x1_t __vec, const int __index)
{
  return __aarch64_vset_lane_any (__elem, __vec, __index);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vset_lane_s8 (int8_t __elem, int8x8_t __vec, const int __index)
{
  return __aarch64_vset_lane_any (__elem, __vec, __index);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vset_lane_s16 (int16_t __elem, int16x4_t __vec, const int __index)
{
  return __aarch64_vset_lane_any (__elem, __vec, __index);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vset_lane_s32 (int32_t __elem, int32x2_t __vec, const int __index)
{
  return __aarch64_vset_lane_any (__elem, __vec, __index);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vset_lane_s64 (int64_t __elem, int64x1_t __vec, const int __index)
{
  return __aarch64_vset_lane_any (__elem, __vec, __index);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vset_lane_u8 (uint8_t __elem, uint8x8_t __vec, const int __index)
{
  return __aarch64_vset_lane_any (__elem, __vec, __index);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vset_lane_u16 (uint16_t __elem, uint16x4_t __vec, const int __index)
{
  return __aarch64_vset_lane_any (__elem, __vec, __index);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vset_lane_u32 (uint32_t __elem, uint32x2_t __vec, const int __index)
{
  return __aarch64_vset_lane_any (__elem, __vec, __index);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vset_lane_u64 (uint64_t __elem, uint64x1_t __vec, const int __index)
{
  return __aarch64_vset_lane_any (__elem, __vec, __index);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsetq_lane_f16 (float16_t __elem, float16x8_t __vec, const int __index)
{
  return __aarch64_vset_lane_any (__elem, __vec, __index);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsetq_lane_f32 (float32_t __elem, float32x4_t __vec, const int __index)
{
  return __aarch64_vset_lane_any (__elem, __vec, __index);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsetq_lane_f64 (float64_t __elem, float64x2_t __vec, const int __index)
{
  return __aarch64_vset_lane_any (__elem, __vec, __index);
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsetq_lane_p8 (poly8_t __elem, poly8x16_t __vec, const int __index)
{
  return __aarch64_vset_lane_any (__elem, __vec, __index);
}
__extension__ extern __inline poly16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsetq_lane_p16 (poly16_t __elem, poly16x8_t __vec, const int __index)
{
  return __aarch64_vset_lane_any (__elem, __vec, __index);
}
__extension__ extern __inline poly64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsetq_lane_p64 (poly64_t __elem, poly64x2_t __vec, const int __index)
{
  return __aarch64_vset_lane_any (__elem, __vec, __index);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsetq_lane_s8 (int8_t __elem, int8x16_t __vec, const int __index)
{
  return __aarch64_vset_lane_any (__elem, __vec, __index);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsetq_lane_s16 (int16_t __elem, int16x8_t __vec, const int __index)
{
  return __aarch64_vset_lane_any (__elem, __vec, __index);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsetq_lane_s32 (int32_t __elem, int32x4_t __vec, const int __index)
{
  return __aarch64_vset_lane_any (__elem, __vec, __index);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsetq_lane_s64 (int64_t __elem, int64x2_t __vec, const int __index)
{
  return __aarch64_vset_lane_any (__elem, __vec, __index);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsetq_lane_u8 (uint8_t __elem, uint8x16_t __vec, const int __index)
{
  return __aarch64_vset_lane_any (__elem, __vec, __index);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsetq_lane_u16 (uint16_t __elem, uint16x8_t __vec, const int __index)
{
  return __aarch64_vset_lane_any (__elem, __vec, __index);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsetq_lane_u32 (uint32_t __elem, uint32x4_t __vec, const int __index)
{
  return __aarch64_vset_lane_any (__elem, __vec, __index);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsetq_lane_u64 (uint64_t __elem, uint64x2_t __vec, const int __index)
{
  return __aarch64_vset_lane_any (__elem, __vec, __index);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_low_f16 (float16x8_t __a)
{
  return __builtin_aarch64_get_lowv8hf (__a);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_low_f32 (float32x4_t __a)
{
  return __builtin_aarch64_get_lowv4sf (__a);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_low_f64 (float64x2_t __a)
{
  return (float64x1_t) {__builtin_aarch64_get_lowv2df (__a)};
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_low_p8 (poly8x16_t __a)
{
  return (poly8x8_t) __builtin_aarch64_get_lowv16qi ((int8x16_t) __a);
}
__extension__ extern __inline poly16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_low_p16 (poly16x8_t __a)
{
  return (poly16x4_t) __builtin_aarch64_get_lowv8hi ((int16x8_t) __a);
}
__extension__ extern __inline poly64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_low_p64 (poly64x2_t __a)
{
  return (poly64x1_t) __builtin_aarch64_get_lowv2di ((int64x2_t) __a);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_low_s8 (int8x16_t __a)
{
  return __builtin_aarch64_get_lowv16qi (__a);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_low_s16 (int16x8_t __a)
{
  return __builtin_aarch64_get_lowv8hi (__a);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_low_s32 (int32x4_t __a)
{
  return __builtin_aarch64_get_lowv4si (__a);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_low_s64 (int64x2_t __a)
{
  return (int64x1_t) {__builtin_aarch64_get_lowv2di (__a)};
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_low_u8 (uint8x16_t __a)
{
  return (uint8x8_t) __builtin_aarch64_get_lowv16qi ((int8x16_t) __a);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_low_u16 (uint16x8_t __a)
{
  return (uint16x4_t) __builtin_aarch64_get_lowv8hi ((int16x8_t) __a);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_low_u32 (uint32x4_t __a)
{
  return (uint32x2_t) __builtin_aarch64_get_lowv4si ((int32x4_t) __a);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_low_u64 (uint64x2_t __a)
{
  return (uint64x1_t) {__builtin_aarch64_get_lowv2di ((int64x2_t) __a)};
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_high_f16 (float16x8_t __a)
{
  return __builtin_aarch64_get_highv8hf (__a);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_high_f32 (float32x4_t __a)
{
  return __builtin_aarch64_get_highv4sf (__a);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_high_f64 (float64x2_t __a)
{
  return (float64x1_t) {__builtin_aarch64_get_highv2df (__a)};
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_high_p8 (poly8x16_t __a)
{
  return (poly8x8_t) __builtin_aarch64_get_highv16qi ((int8x16_t) __a);
}
__extension__ extern __inline poly16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_high_p16 (poly16x8_t __a)
{
  return (poly16x4_t) __builtin_aarch64_get_highv8hi ((int16x8_t) __a);
}
__extension__ extern __inline poly64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_high_p64 (poly64x2_t __a)
{
  return (poly64x1_t) __builtin_aarch64_get_highv2di ((int64x2_t) __a);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_high_s8 (int8x16_t __a)
{
  return __builtin_aarch64_get_highv16qi (__a);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_high_s16 (int16x8_t __a)
{
  return __builtin_aarch64_get_highv8hi (__a);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_high_s32 (int32x4_t __a)
{
  return __builtin_aarch64_get_highv4si (__a);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_high_s64 (int64x2_t __a)
{
  return (int64x1_t) {__builtin_aarch64_get_highv2di (__a)};
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_high_u8 (uint8x16_t __a)
{
  return (uint8x8_t) __builtin_aarch64_get_highv16qi ((int8x16_t) __a);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_high_u16 (uint16x8_t __a)
{
  return (uint16x4_t) __builtin_aarch64_get_highv8hi ((int16x8_t) __a);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_high_u32 (uint32x4_t __a)
{
  return (uint32x2_t) __builtin_aarch64_get_highv4si ((int32x4_t) __a);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_high_u64 (uint64x2_t __a)
{
  return (uint64x1_t) {__builtin_aarch64_get_highv2di ((int64x2_t) __a)};
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcombine_s8 (int8x8_t __a, int8x8_t __b)
{
  return __builtin_aarch64_combinev8qi (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcombine_s16 (int16x4_t __a, int16x4_t __b)
{
  return __builtin_aarch64_combinev4hi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcombine_s32 (int32x2_t __a, int32x2_t __b)
{
  return __builtin_aarch64_combinev2si (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcombine_s64 (int64x1_t __a, int64x1_t __b)
{
  return __builtin_aarch64_combinedi (__a[0], __b[0]);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcombine_f16 (float16x4_t __a, float16x4_t __b)
{
  return __builtin_aarch64_combinev4hf (__a, __b);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcombine_f32 (float32x2_t __a, float32x2_t __b)
{
  return __builtin_aarch64_combinev2sf (__a, __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcombine_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return __builtin_aarch64_combinev8qi_uuu (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcombine_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return __builtin_aarch64_combinev4hi_uuu (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcombine_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return __builtin_aarch64_combinev2si_uuu (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcombine_u64 (uint64x1_t __a, uint64x1_t __b)
{
  return __builtin_aarch64_combinedi_uuu (__a[0], __b[0]);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcombine_f64 (float64x1_t __a, float64x1_t __b)
{
  return __builtin_aarch64_combinedf (__a[0], __b[0]);
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcombine_p8 (poly8x8_t __a, poly8x8_t __b)
{
  return __builtin_aarch64_combinev8qi_ppp (__a, __b);
}
__extension__ extern __inline poly16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcombine_p16 (poly16x4_t __a, poly16x4_t __b)
{
  return __builtin_aarch64_combinev4hi_ppp (__a, __b);
}
__extension__ extern __inline poly64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcombine_p64 (poly64x1_t __a, poly64x1_t __b)
{
  return __builtin_aarch64_combinedi_ppp (__a[0], __b[0]);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaba_s8 (int8x8_t __a, int8x8_t __b, int8x8_t __c)
{
  return __builtin_aarch64_sabav8qi (__a, __b, __c);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaba_s16 (int16x4_t __a, int16x4_t __b, int16x4_t __c)
{
  return __builtin_aarch64_sabav4hi (__a, __b, __c);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaba_s32 (int32x2_t __a, int32x2_t __b, int32x2_t __c)
{
  return __builtin_aarch64_sabav2si (__a, __b, __c);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaba_u8 (uint8x8_t __a, uint8x8_t __b, uint8x8_t __c)
{
  return __builtin_aarch64_uabav8qi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaba_u16 (uint16x4_t __a, uint16x4_t __b, uint16x4_t __c)
{
  return __builtin_aarch64_uabav4hi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaba_u32 (uint32x2_t __a, uint32x2_t __b, uint32x2_t __c)
{
  return __builtin_aarch64_uabav2si_uuuu (__a, __b, __c);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabal_high_s8 (int16x8_t __a, int8x16_t __b, int8x16_t __c)
{
  return __builtin_aarch64_sabal2v16qi (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabal_high_s16 (int32x4_t __a, int16x8_t __b, int16x8_t __c)
{
  return __builtin_aarch64_sabal2v8hi (__a, __b, __c);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabal_high_s32 (int64x2_t __a, int32x4_t __b, int32x4_t __c)
{
  return __builtin_aarch64_sabal2v4si (__a, __b, __c);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabal_high_u8 (uint16x8_t __a, uint8x16_t __b, uint8x16_t __c)
{
  return __builtin_aarch64_uabal2v16qi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabal_high_u16 (uint32x4_t __a, uint16x8_t __b, uint16x8_t __c)
{
  return __builtin_aarch64_uabal2v8hi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabal_high_u32 (uint64x2_t __a, uint32x4_t __b, uint32x4_t __c)
{
  return __builtin_aarch64_uabal2v4si_uuuu (__a, __b, __c);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabal_s8 (int16x8_t __a, int8x8_t __b, int8x8_t __c)
{
  return __builtin_aarch64_sabalv8qi (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabal_s16 (int32x4_t __a, int16x4_t __b, int16x4_t __c)
{
  return __builtin_aarch64_sabalv4hi (__a, __b, __c);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabal_s32 (int64x2_t __a, int32x2_t __b, int32x2_t __c)
{
  return __builtin_aarch64_sabalv2si (__a, __b, __c);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabal_u8 (uint16x8_t __a, uint8x8_t __b, uint8x8_t __c)
{
  return __builtin_aarch64_uabalv8qi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabal_u16 (uint32x4_t __a, uint16x4_t __b, uint16x4_t __c)
{
  return __builtin_aarch64_uabalv4hi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabal_u32 (uint64x2_t __a, uint32x2_t __b, uint32x2_t __c)
{
  return __builtin_aarch64_uabalv2si_uuuu (__a, __b, __c);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabaq_s8 (int8x16_t __a, int8x16_t __b, int8x16_t __c)
{
  return __builtin_aarch64_sabav16qi (__a, __b, __c);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabaq_s16 (int16x8_t __a, int16x8_t __b, int16x8_t __c)
{
  return __builtin_aarch64_sabav8hi (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabaq_s32 (int32x4_t __a, int32x4_t __b, int32x4_t __c)
{
  return __builtin_aarch64_sabav4si (__a, __b, __c);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabaq_u8 (uint8x16_t __a, uint8x16_t __b, uint8x16_t __c)
{
  return __builtin_aarch64_uabav16qi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabaq_u16 (uint16x8_t __a, uint16x8_t __b, uint16x8_t __c)
{
  return __builtin_aarch64_uabav8hi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabaq_u32 (uint32x4_t __a, uint32x4_t __b, uint32x4_t __c)
{
  return __builtin_aarch64_uabav4si_uuuu (__a, __b, __c);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabd_s8 (int8x8_t __a, int8x8_t __b)
{
  return __builtin_aarch64_sabdv8qi (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabd_s16 (int16x4_t __a, int16x4_t __b)
{
  return __builtin_aarch64_sabdv4hi (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabd_s32 (int32x2_t __a, int32x2_t __b)
{
  return __builtin_aarch64_sabdv2si (__a, __b);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabd_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return __builtin_aarch64_uabdv8qi_uuu (__a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabd_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return __builtin_aarch64_uabdv4hi_uuu (__a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabd_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return __builtin_aarch64_uabdv2si_uuu (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabdl_high_s8 (int8x16_t __a, int8x16_t __b)
{
  return __builtin_aarch64_sabdl2v16qi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabdl_high_s16 (int16x8_t __a, int16x8_t __b)
{
  return __builtin_aarch64_sabdl2v8hi (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabdl_high_s32 (int32x4_t __a, int32x4_t __b)
{
  return __builtin_aarch64_sabdl2v4si (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabdl_high_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return __builtin_aarch64_uabdl2v16qi_uuu (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabdl_high_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return __builtin_aarch64_uabdl2v8hi_uuu (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabdl_high_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return __builtin_aarch64_uabdl2v4si_uuu (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabdl_s8 (int8x8_t __a, int8x8_t __b)
{
  return __builtin_aarch64_sabdlv8qi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabdl_s16 (int16x4_t __a, int16x4_t __b)
{
  return __builtin_aarch64_sabdlv4hi (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabdl_s32 (int32x2_t __a, int32x2_t __b)
{
  return __builtin_aarch64_sabdlv2si (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabdl_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return __builtin_aarch64_uabdlv8qi_uuu (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabdl_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return __builtin_aarch64_uabdlv4hi_uuu (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabdl_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return __builtin_aarch64_uabdlv2si_uuu (__a, __b);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabdq_s8 (int8x16_t __a, int8x16_t __b)
{
  return __builtin_aarch64_sabdv16qi (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabdq_s16 (int16x8_t __a, int16x8_t __b)
{
  return __builtin_aarch64_sabdv8hi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabdq_s32 (int32x4_t __a, int32x4_t __b)
{
  return __builtin_aarch64_sabdv4si (__a, __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabdq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return __builtin_aarch64_uabdv16qi_uuu (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabdq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return __builtin_aarch64_uabdv8hi_uuu (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabdq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return __builtin_aarch64_uabdv4si_uuu (__a, __b);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddlv_s8 (int8x8_t __a)
{
  return __builtin_aarch64_saddlvv8qi (__a);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddlv_s16 (int16x4_t __a)
{
  return __builtin_aarch64_saddlvv4hi (__a);
}
__extension__ extern __inline uint16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddlv_u8 (uint8x8_t __a)
{
  return __builtin_aarch64_uaddlvv8qi_uu (__a);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddlv_u16 (uint16x4_t __a)
{
  return __builtin_aarch64_uaddlvv4hi_uu (__a);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddlvq_s8 (int8x16_t __a)
{
  return __builtin_aarch64_saddlvv16qi (__a);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddlvq_s16 (int16x8_t __a)
{
  return __builtin_aarch64_saddlvv8hi (__a);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddlvq_s32 (int32x4_t __a)
{
  return __builtin_aarch64_saddlvv4si (__a);
}
__extension__ extern __inline uint16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddlvq_u8 (uint8x16_t __a)
{
  return __builtin_aarch64_uaddlvv16qi_uu (__a);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddlvq_u16 (uint16x8_t __a)
{
  return __builtin_aarch64_uaddlvv8hi_uu (__a);
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddlvq_u32 (uint32x4_t __a)
{
  return __builtin_aarch64_uaddlvv4si_uu (__a);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtx_f32_f64 (float64x2_t __a)
{
  return __builtin_aarch64_float_trunc_rodd_lo_v2sf (__a);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtx_high_f32_f64 (float32x2_t __a, float64x2_t __b)
{
  return __builtin_aarch64_float_trunc_rodd_hi_v4sf (__a, __b);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtxd_f32_f64 (float64_t __a)
{
  return __builtin_aarch64_float_trunc_rodd_df (__a);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmla_n_f32 (float32x2_t __a, float32x2_t __b, float32_t __c)
{
  return __builtin_aarch64_float_mla_nv2sf (__a, __b, __c);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmla_n_s16 (int16x4_t __a, int16x4_t __b, int16_t __c)
{
  return __builtin_aarch64_mla_nv4hi (__a, __b, __c);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmla_n_s32 (int32x2_t __a, int32x2_t __b, int32_t __c)
{
  return __builtin_aarch64_mla_nv2si (__a, __b, __c);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmla_n_u16 (uint16x4_t __a, uint16x4_t __b, uint16_t __c)
{
  return __builtin_aarch64_mla_nv4hi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmla_n_u32 (uint32x2_t __a, uint32x2_t __b, uint32_t __c)
{
  return __builtin_aarch64_mla_nv2si_uuuu (__a, __b, __c);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmla_s8 (int8x8_t __a, int8x8_t __b, int8x8_t __c)
{
  return __builtin_aarch64_mlav8qi (__a, __b, __c);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmla_s16 (int16x4_t __a, int16x4_t __b, int16x4_t __c)
{
  return __builtin_aarch64_mlav4hi (__a, __b, __c);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmla_s32 (int32x2_t __a, int32x2_t __b, int32x2_t __c)
{
  return __builtin_aarch64_mlav2si (__a, __b, __c);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmla_u8 (uint8x8_t __a, uint8x8_t __b, uint8x8_t __c)
{
  return __builtin_aarch64_mlav8qi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmla_u16 (uint16x4_t __a, uint16x4_t __b, uint16x4_t __c)
{
  return __builtin_aarch64_mlav4hi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmla_u32 (uint32x2_t __a, uint32x2_t __b, uint32x2_t __c)
{
  return __builtin_aarch64_mlav2si_uuuu (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_high_lane_s16(int32x4_t __a, int16x8_t __b, int16x4_t __v,
      const int __lane)
{
  return __builtin_aarch64_smlal_hi_lanev8hi (__a, __b, __v, __lane);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_high_lane_s32(int64x2_t __a, int32x4_t __b, int32x2_t __v,
      const int __lane)
{
  return __builtin_aarch64_smlal_hi_lanev4si (__a, __b, __v, __lane);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_high_lane_u16(uint32x4_t __a, uint16x8_t __b, uint16x4_t __v,
      const int __lane)
{
  return __builtin_aarch64_umlal_hi_lanev8hi_uuuus (__a, __b, __v, __lane);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_high_lane_u32(uint64x2_t __a, uint32x4_t __b, uint32x2_t __v,
      const int __lane)
{
  return __builtin_aarch64_umlal_hi_lanev4si_uuuus (__a, __b, __v, __lane);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_high_laneq_s16(int32x4_t __a, int16x8_t __b, int16x8_t __v,
       const int __lane)
{
  return __builtin_aarch64_smlal_hi_laneqv8hi (__a, __b, __v, __lane);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_high_laneq_s32(int64x2_t __a, int32x4_t __b, int32x4_t __v,
       const int __lane)
{
  return __builtin_aarch64_smlal_hi_laneqv4si (__a, __b, __v, __lane);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_high_laneq_u16(uint32x4_t __a, uint16x8_t __b, uint16x8_t __v,
       const int __lane)
{
  return __builtin_aarch64_umlal_hi_laneqv8hi_uuuus (__a, __b, __v, __lane);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_high_laneq_u32(uint64x2_t __a, uint32x4_t __b, uint32x4_t __v,
       const int __lane)
{
  return __builtin_aarch64_umlal_hi_laneqv4si_uuuus (__a, __b, __v, __lane);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_high_n_s16 (int32x4_t __a, int16x8_t __b, int16_t __c)
{
  return __builtin_aarch64_smlal_hi_nv8hi (__a, __b, __c);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_high_n_s32 (int64x2_t __a, int32x4_t __b, int32_t __c)
{
  return __builtin_aarch64_smlal_hi_nv4si (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_high_n_u16 (uint32x4_t __a, uint16x8_t __b, uint16_t __c)
{
  return __builtin_aarch64_umlal_hi_nv8hi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_high_n_u32 (uint64x2_t __a, uint32x4_t __b, uint32_t __c)
{
  return __builtin_aarch64_umlal_hi_nv4si_uuuu (__a, __b, __c);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_high_s8 (int16x8_t __a, int8x16_t __b, int8x16_t __c)
{
  return __builtin_aarch64_smlal_hiv16qi (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_high_s16 (int32x4_t __a, int16x8_t __b, int16x8_t __c)
{
  return __builtin_aarch64_smlal_hiv8hi (__a, __b, __c);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_high_s32 (int64x2_t __a, int32x4_t __b, int32x4_t __c)
{
  return __builtin_aarch64_smlal_hiv4si (__a, __b, __c);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_high_u8 (uint16x8_t __a, uint8x16_t __b, uint8x16_t __c)
{
  return __builtin_aarch64_umlal_hiv16qi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_high_u16 (uint32x4_t __a, uint16x8_t __b, uint16x8_t __c)
{
  return __builtin_aarch64_umlal_hiv8hi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_high_u32 (uint64x2_t __a, uint32x4_t __b, uint32x4_t __c)
{
  return __builtin_aarch64_umlal_hiv4si_uuuu (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_lane_s16 (int32x4_t __acc, int16x4_t __a, int16x4_t __b, const int __c)
{
  return __builtin_aarch64_vec_smlal_lane_v4hi (__acc, __a, __b, __c);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_lane_s32 (int64x2_t __acc, int32x2_t __a, int32x2_t __b, const int __c)
{
  return __builtin_aarch64_vec_smlal_lane_v2si (__acc, __a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_lane_u16 (uint32x4_t __acc, uint16x4_t __a, uint16x4_t __b, const int __c)
{
  return __builtin_aarch64_vec_umlal_lane_v4hi_uuuus (__acc, __a, __b, __c);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_lane_u32 (uint64x2_t __acc, uint32x2_t __a, uint32x2_t __b, const int __c)
{
  return __builtin_aarch64_vec_umlal_lane_v2si_uuuus (__acc, __a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_laneq_s16 (int32x4_t __acc, int16x4_t __a, int16x8_t __b, const int __c)
{
  return __builtin_aarch64_vec_smlal_laneq_v4hi (__acc, __a, __b, __c);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_laneq_s32 (int64x2_t __acc, int32x2_t __a, int32x4_t __b, const int __c)
{
  return __builtin_aarch64_vec_smlal_laneq_v2si (__acc, __a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_laneq_u16 (uint32x4_t __acc, uint16x4_t __a, uint16x8_t __b, const int __c)
{
  return __builtin_aarch64_vec_umlal_laneq_v4hi_uuuus (__acc, __a, __b, __c);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_laneq_u32 (uint64x2_t __acc, uint32x2_t __a, uint32x4_t __b, const int __c)
{
  return __builtin_aarch64_vec_umlal_laneq_v2si_uuuus (__acc, __a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_n_s16 (int32x4_t __a, int16x4_t __b, int16_t __c)
{
  return __builtin_aarch64_smlal_nv4hi (__a, __b, __c);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_n_s32 (int64x2_t __a, int32x2_t __b, int32_t __c)
{
  return __builtin_aarch64_smlal_nv2si (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_n_u16 (uint32x4_t __a, uint16x4_t __b, uint16_t __c)
{
  return __builtin_aarch64_umlal_nv4hi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_n_u32 (uint64x2_t __a, uint32x2_t __b, uint32_t __c)
{
  return __builtin_aarch64_umlal_nv2si_uuuu (__a, __b, __c);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_s8 (int16x8_t __a, int8x8_t __b, int8x8_t __c)
{
  return __builtin_aarch64_smlalv8qi (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_s16 (int32x4_t __a, int16x4_t __b, int16x4_t __c)
{
  return __builtin_aarch64_smlalv4hi (__a, __b, __c);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_s32 (int64x2_t __a, int32x2_t __b, int32x2_t __c)
{
  return __builtin_aarch64_smlalv2si (__a, __b, __c);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_u8 (uint16x8_t __a, uint8x8_t __b, uint8x8_t __c)
{
  return __builtin_aarch64_umlalv8qi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_u16 (uint32x4_t __a, uint16x4_t __b, uint16x4_t __c)
{
  return __builtin_aarch64_umlalv4hi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlal_u32 (uint64x2_t __a, uint32x2_t __b, uint32x2_t __c)
{
  return __builtin_aarch64_umlalv2si_uuuu (__a, __b, __c);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlaq_n_f32 (float32x4_t __a, float32x4_t __b, float32_t __c)
{
  return __builtin_aarch64_float_mla_nv4sf (__a, __b, __c);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlaq_n_s16 (int16x8_t __a, int16x8_t __b, int16_t __c)
{
  return __builtin_aarch64_mla_nv8hi (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlaq_n_s32 (int32x4_t __a, int32x4_t __b, int32_t __c)
{
  return __builtin_aarch64_mla_nv4si (__a, __b, __c);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlaq_n_u16 (uint16x8_t __a, uint16x8_t __b, uint16_t __c)
{
  return __builtin_aarch64_mla_nv8hi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlaq_n_u32 (uint32x4_t __a, uint32x4_t __b, uint32_t __c)
{
  return __builtin_aarch64_mla_nv4si_uuuu (__a, __b, __c);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlaq_s8 (int8x16_t __a, int8x16_t __b, int8x16_t __c)
{
  return __builtin_aarch64_mlav16qi (__a, __b, __c);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlaq_s16 (int16x8_t __a, int16x8_t __b, int16x8_t __c)
{
  return __builtin_aarch64_mlav8hi (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlaq_s32 (int32x4_t __a, int32x4_t __b, int32x4_t __c)
{
  return __builtin_aarch64_mlav4si (__a, __b, __c);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlaq_u8 (uint8x16_t __a, uint8x16_t __b, uint8x16_t __c)
{
  return __builtin_aarch64_mlav16qi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlaq_u16 (uint16x8_t __a, uint16x8_t __b, uint16x8_t __c)
{
  return __builtin_aarch64_mlav8hi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlaq_u32 (uint32x4_t __a, uint32x4_t __b, uint32x4_t __c)
{
  return __builtin_aarch64_mlav4si_uuuu (__a, __b, __c);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmls_n_f32 (float32x2_t __a, float32x2_t __b, float32_t __c)
{
  return __builtin_aarch64_float_mls_nv2sf (__a, __b, __c);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmls_n_s16 (int16x4_t __a, int16x4_t __b, int16_t __c)
{
  return __builtin_aarch64_mls_nv4hi (__a, __b, __c);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmls_n_s32 (int32x2_t __a, int32x2_t __b, int32_t __c)
{
  return __builtin_aarch64_mls_nv2si (__a, __b, __c);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmls_n_u16 (uint16x4_t __a, uint16x4_t __b, uint16_t __c)
{
  return __builtin_aarch64_mls_nv4hi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmls_n_u32 (uint32x2_t __a, uint32x2_t __b, uint32_t __c)
{
  return __builtin_aarch64_mls_nv2si_uuuu (__a, __b, __c);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmls_s8 (int8x8_t __a, int8x8_t __b, int8x8_t __c)
{
  return __builtin_aarch64_mlsv8qi (__a, __b, __c);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmls_s16 (int16x4_t __a, int16x4_t __b, int16x4_t __c)
{
  return __builtin_aarch64_mlsv4hi (__a, __b, __c);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmls_s32 (int32x2_t __a, int32x2_t __b, int32x2_t __c)
{
  return __builtin_aarch64_mlsv2si (__a, __b, __c);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmls_u8 (uint8x8_t __a, uint8x8_t __b, uint8x8_t __c)
{
  return __builtin_aarch64_mlsv8qi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmls_u16 (uint16x4_t __a, uint16x4_t __b, uint16x4_t __c)
{
  return __builtin_aarch64_mlsv4hi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmls_u32 (uint32x2_t __a, uint32x2_t __b, uint32x2_t __c)
{
  return __builtin_aarch64_mlsv2si_uuuu (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_high_lane_s16(int32x4_t __a, int16x8_t __b, int16x4_t __v,
      const int __lane)
{
  return __builtin_aarch64_smlsl_hi_lanev8hi (__a, __b, __v, __lane);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_high_lane_s32(int64x2_t __a, int32x4_t __b, int32x2_t __v,
      const int __lane)
{
  return __builtin_aarch64_smlsl_hi_lanev4si (__a, __b, __v, __lane);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_high_lane_u16(uint32x4_t __a, uint16x8_t __b, uint16x4_t __v,
      const int __lane)
{
  return __builtin_aarch64_umlsl_hi_lanev8hi_uuuus (__a, __b, __v, __lane);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_high_lane_u32(uint64x2_t __a, uint32x4_t __b, uint32x2_t __v,
      const int __lane)
{
  return __builtin_aarch64_umlsl_hi_lanev4si_uuuus (__a, __b, __v, __lane);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_high_laneq_s16(int32x4_t __a, int16x8_t __b, int16x8_t __v,
       const int __lane)
{
  return __builtin_aarch64_smlsl_hi_laneqv8hi (__a, __b, __v, __lane);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_high_laneq_s32(int64x2_t __a, int32x4_t __b, int32x4_t __v,
       const int __lane)
{
  return __builtin_aarch64_smlsl_hi_laneqv4si (__a, __b, __v, __lane);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_high_laneq_u16(uint32x4_t __a, uint16x8_t __b, uint16x8_t __v,
       const int __lane)
{
  return __builtin_aarch64_umlsl_hi_laneqv8hi_uuuus (__a, __b, __v, __lane);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_high_laneq_u32(uint64x2_t __a, uint32x4_t __b, uint32x4_t __v,
       const int __lane)
{
  return __builtin_aarch64_umlsl_hi_laneqv4si_uuuus (__a, __b, __v, __lane);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_high_n_s16 (int32x4_t __a, int16x8_t __b, int16_t __c)
{
  return __builtin_aarch64_smlsl_hi_nv8hi (__a, __b, __c);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_high_n_s32 (int64x2_t __a, int32x4_t __b, int32_t __c)
{
  return __builtin_aarch64_smlsl_hi_nv4si (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_high_n_u16 (uint32x4_t __a, uint16x8_t __b, uint16_t __c)
{
  return __builtin_aarch64_umlsl_hi_nv8hi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_high_n_u32 (uint64x2_t __a, uint32x4_t __b, uint32_t __c)
{
  return __builtin_aarch64_umlsl_hi_nv4si_uuuu (__a, __b, __c);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_high_s8 (int16x8_t __a, int8x16_t __b, int8x16_t __c)
{
  return __builtin_aarch64_smlsl_hiv16qi (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_high_s16 (int32x4_t __a, int16x8_t __b, int16x8_t __c)
{
  return __builtin_aarch64_smlsl_hiv8hi (__a, __b, __c);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_high_s32 (int64x2_t __a, int32x4_t __b, int32x4_t __c)
{
  return __builtin_aarch64_smlsl_hiv4si (__a, __b, __c);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_high_u8 (uint16x8_t __a, uint8x16_t __b, uint8x16_t __c)
{
  return __builtin_aarch64_umlsl_hiv16qi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_high_u16 (uint32x4_t __a, uint16x8_t __b, uint16x8_t __c)
{
  return __builtin_aarch64_umlsl_hiv8hi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_high_u32 (uint64x2_t __a, uint32x4_t __b, uint32x4_t __c)
{
  return __builtin_aarch64_umlsl_hiv4si_uuuu (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_lane_s16 (int32x4_t __a, int16x4_t __b, int16x4_t __v, const int __lane)
{
  return __builtin_aarch64_vec_smlsl_lane_v4hi (__a, __b, __v, __lane);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_lane_s32 (int64x2_t __a, int32x2_t __b, int32x2_t __v, const int __lane)
{
  return __builtin_aarch64_vec_smlsl_lane_v2si (__a, __b, __v, __lane);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_lane_u16 (uint32x4_t __a, uint16x4_t __b, uint16x4_t __v,
                const int __lane)
{
  return __builtin_aarch64_vec_umlsl_lane_v4hi_uuuus (__a, __b, __v, __lane);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_lane_u32 (uint64x2_t __a, uint32x2_t __b, uint32x2_t __v,
                const int __lane)
{
  return __builtin_aarch64_vec_umlsl_lane_v2si_uuuus (__a, __b, __v, __lane);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_laneq_s16 (int32x4_t __a, int16x4_t __b, int16x8_t __v, const int __lane)
{
  return __builtin_aarch64_vec_smlsl_laneq_v4hi (__a, __b, __v, __lane);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_laneq_s32 (int64x2_t __a, int32x2_t __b, int32x4_t __v, const int __lane)
{
  return __builtin_aarch64_vec_smlsl_laneq_v2si (__a, __b, __v, __lane);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_laneq_u16 (uint32x4_t __a, uint16x4_t __b, uint16x8_t __v,
                 const int __lane)
{
  return __builtin_aarch64_vec_umlsl_laneq_v4hi_uuuus (__a, __b, __v, __lane);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_laneq_u32 (uint64x2_t __a, uint32x2_t __b, uint32x4_t __v,
                 const int __lane)
{
  return __builtin_aarch64_vec_umlsl_laneq_v2si_uuuus (__a, __b, __v, __lane);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_n_s16 (int32x4_t __a, int16x4_t __b, int16_t __c)
{
  return __builtin_aarch64_smlsl_nv4hi (__a, __b, __c);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_n_s32 (int64x2_t __a, int32x2_t __b, int32_t __c)
{
  return __builtin_aarch64_smlsl_nv2si (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_n_u16 (uint32x4_t __a, uint16x4_t __b, uint16_t __c)
{
  return __builtin_aarch64_umlsl_nv4hi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_n_u32 (uint64x2_t __a, uint32x2_t __b, uint32_t __c)
{
  return __builtin_aarch64_umlsl_nv2si_uuuu (__a, __b, __c);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_s8 (int16x8_t __a, int8x8_t __b, int8x8_t __c)
{
  return __builtin_aarch64_smlslv8qi (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_s16 (int32x4_t __a, int16x4_t __b, int16x4_t __c)
{
  return __builtin_aarch64_smlslv4hi (__a, __b, __c);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_s32 (int64x2_t __a, int32x2_t __b, int32x2_t __c)
{
  return __builtin_aarch64_smlslv2si (__a, __b, __c);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_u8 (uint16x8_t __a, uint8x8_t __b, uint8x8_t __c)
{
  return __builtin_aarch64_umlslv8qi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_u16 (uint32x4_t __a, uint16x4_t __b, uint16x4_t __c)
{
  return __builtin_aarch64_umlslv4hi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsl_u32 (uint64x2_t __a, uint32x2_t __b, uint32x2_t __c)
{
  return __builtin_aarch64_umlslv2si_uuuu (__a, __b, __c);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsq_n_f32 (float32x4_t __a, float32x4_t __b, float32_t __c)
{
  return __builtin_aarch64_float_mls_nv4sf (__a, __b, __c);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsq_n_s16 (int16x8_t __a, int16x8_t __b, int16_t __c)
{
  return __builtin_aarch64_mls_nv8hi (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsq_n_s32 (int32x4_t __a, int32x4_t __b, int32_t __c)
{
  return __builtin_aarch64_mls_nv4si (__a, __b, __c);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsq_n_u16 (uint16x8_t __a, uint16x8_t __b, uint16_t __c)
{
  return __builtin_aarch64_mls_nv8hi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsq_n_u32 (uint32x4_t __a, uint32x4_t __b, uint32_t __c)
{
  return __builtin_aarch64_mls_nv4si_uuuu (__a, __b, __c);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsq_s8 (int8x16_t __a, int8x16_t __b, int8x16_t __c)
{
  return __builtin_aarch64_mlsv16qi (__a, __b, __c);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsq_s16 (int16x8_t __a, int16x8_t __b, int16x8_t __c)
{
  return __builtin_aarch64_mlsv8hi (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsq_s32 (int32x4_t __a, int32x4_t __b, int32x4_t __c)
{
  return __builtin_aarch64_mlsv4si (__a, __b, __c);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsq_u8 (uint8x16_t __a, uint8x16_t __b, uint8x16_t __c)
{
  return __builtin_aarch64_mlsv16qi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsq_u16 (uint16x8_t __a, uint16x8_t __b, uint16x8_t __c)
{
  return __builtin_aarch64_mlsv8hi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsq_u32 (uint32x4_t __a, uint32x4_t __b, uint32x4_t __c)
{
  return __builtin_aarch64_mlsv4si_uuuu (__a, __b, __c);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovl_high_s8 (int8x16_t __a)
{
  return __builtin_aarch64_vec_unpacks_hi_v16qi (__a);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovl_high_s16 (int16x8_t __a)
{
  return __builtin_aarch64_vec_unpacks_hi_v8hi (__a);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovl_high_s32 (int32x4_t __a)
{
  return __builtin_aarch64_vec_unpacks_hi_v4si (__a);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovl_high_u8 (uint8x16_t __a)
{
  return __builtin_aarch64_vec_unpacku_hi_v16qi_uu (__a);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovl_high_u16 (uint16x8_t __a)
{
  return __builtin_aarch64_vec_unpacku_hi_v8hi_uu (__a);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovl_high_u32 (uint32x4_t __a)
{
  return __builtin_aarch64_vec_unpacku_hi_v4si_uu (__a);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovl_s8 (int8x8_t __a)
{
  return __builtin_aarch64_sxtlv8hi (__a);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovl_s16 (int16x4_t __a)
{
  return __builtin_aarch64_sxtlv4si (__a);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovl_s32 (int32x2_t __a)
{
  return __builtin_aarch64_sxtlv2di (__a);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovl_u8 (uint8x8_t __a)
{
  return __builtin_aarch64_uxtlv8hi_uu (__a);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovl_u16 (uint16x4_t __a)
{
  return __builtin_aarch64_uxtlv4si_uu (__a);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovl_u32 (uint32x2_t __a)
{
  return __builtin_aarch64_uxtlv2di_uu (__a);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovn_high_s16 (int8x8_t __a, int16x8_t __b)
{
  return __builtin_aarch64_xtn2v8hi (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovn_high_s32 (int16x4_t __a, int32x4_t __b)
{
  return __builtin_aarch64_xtn2v4si (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovn_high_s64 (int32x2_t __a, int64x2_t __b)
{
  return __builtin_aarch64_xtn2v2di (__a, __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovn_high_u16 (uint8x8_t __a, uint16x8_t __b)
{
  return __builtin_aarch64_xtn2v8hi_uuu (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovn_high_u32 (uint16x4_t __a, uint32x4_t __b)
{
  return __builtin_aarch64_xtn2v4si_uuu (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovn_high_u64 (uint32x2_t __a, uint64x2_t __b)
{
  return __builtin_aarch64_xtn2v2di_uuu (__a, __b);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovn_s16 (int16x8_t __a)
{
  return __builtin_aarch64_xtnv8hi (__a);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovn_s32 (int32x4_t __a)
{
  return __builtin_aarch64_xtnv4si (__a);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovn_s64 (int64x2_t __a)
{
  return __builtin_aarch64_xtnv2di (__a);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovn_u16 (uint16x8_t __a)
{
  return __builtin_aarch64_xtnv8hi_uu (__a);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovn_u32 (uint32x4_t __a)
{
  return __builtin_aarch64_xtnv4si_uu (__a);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovn_u64 (uint64x2_t __a)
{
  return __builtin_aarch64_xtnv2di_uu (__a);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshrn_n_s16 (int16x8_t __a, const int __b)
{
  return __builtin_aarch64_shrn_nv8hi (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshrn_n_s32 (int32x4_t __a, const int __b)
{
  return __builtin_aarch64_shrn_nv4si (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshrn_n_s64 (int64x2_t __a, const int __b)
{
  return __builtin_aarch64_shrn_nv2di (__a, __b);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshrn_n_u16 (uint16x8_t __a, const int __b)
{
  return __builtin_aarch64_shrn_nv8hi_uus (__a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshrn_n_u32 (uint32x4_t __a, const int __b)
{
  return __builtin_aarch64_shrn_nv4si_uus (__a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshrn_n_u64 (uint64x2_t __a, const int __b)
{
  return __builtin_aarch64_shrn_nv2di_uus (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_high_lane_s16 (int16x8_t __a, int16x4_t __v, const int __lane)
{
  return __builtin_aarch64_smull_hi_lanev8hi (__a, __v, __lane);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_high_lane_s32 (int32x4_t __a, int32x2_t __v, const int __lane)
{
  return __builtin_aarch64_smull_hi_lanev4si (__a, __v, __lane);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_high_lane_u16 (uint16x8_t __a, uint16x4_t __v, const int __lane)
{
  return __builtin_aarch64_umull_hi_lanev8hi_uuus (__a, __v, __lane);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_high_lane_u32 (uint32x4_t __a, uint32x2_t __v, const int __lane)
{
  return __builtin_aarch64_umull_hi_lanev4si_uuus (__a, __v, __lane);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_high_laneq_s16 (int16x8_t __a, int16x8_t __v, const int __lane)
{
  return __builtin_aarch64_smull_hi_laneqv8hi (__a, __v, __lane);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_high_laneq_s32 (int32x4_t __a, int32x4_t __v, const int __lane)
{
  return __builtin_aarch64_smull_hi_laneqv4si (__a, __v, __lane);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_high_laneq_u16 (uint16x8_t __a, uint16x8_t __v, const int __lane)
{
  return __builtin_aarch64_umull_hi_laneqv8hi_uuus (__a, __v, __lane);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_high_laneq_u32 (uint32x4_t __a, uint32x4_t __v, const int __lane)
{
  return __builtin_aarch64_umull_hi_laneqv4si_uuus (__a, __v, __lane);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_high_n_s16 (int16x8_t __a, int16_t __b)
{
  return __builtin_aarch64_smull_hi_nv8hi (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_high_n_s32 (int32x4_t __a, int32_t __b)
{
  return __builtin_aarch64_smull_hi_nv4si (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_high_n_u16 (uint16x8_t __a, uint16_t __b)
{
  return __builtin_aarch64_umull_hi_nv8hi_uuu (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_high_n_u32 (uint32x4_t __a, uint32_t __b)
{
 return __builtin_aarch64_umull_hi_nv4si_uuu (__a, __b);
}
__extension__ extern __inline poly16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_high_p8 (poly8x16_t __a, poly8x16_t __b)
{
  return __builtin_aarch64_pmull_hiv16qi_ppp (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_high_s8 (int8x16_t __a, int8x16_t __b)
{
  return __builtin_aarch64_vec_widen_smult_hi_v16qi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_high_s16 (int16x8_t __a, int16x8_t __b)
{
  return __builtin_aarch64_vec_widen_smult_hi_v8hi (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_high_s32 (int32x4_t __a, int32x4_t __b)
{
  return __builtin_aarch64_vec_widen_smult_hi_v4si (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_high_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return __builtin_aarch64_vec_widen_umult_hi_v16qi_uuu (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_high_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return __builtin_aarch64_vec_widen_umult_hi_v8hi_uuu (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_high_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return __builtin_aarch64_vec_widen_umult_hi_v4si_uuu (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_lane_s16 (int16x4_t __a, int16x4_t __b, const int __c)
{
  return __builtin_aarch64_vec_smult_lane_v4hi (__a, __b, __c);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_lane_s32 (int32x2_t __a, int32x2_t __b, const int __c)
{
  return __builtin_aarch64_vec_smult_lane_v2si (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_lane_u16 (uint16x4_t __a, uint16x4_t __b, const int __c)
{
  return __builtin_aarch64_vec_umult_lane_v4hi_uuus (__a, __b, __c);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_lane_u32 (uint32x2_t __a, uint32x2_t __b, const int __c)
{
  return __builtin_aarch64_vec_umult_lane_v2si_uuus (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_laneq_s16 (int16x4_t __a, int16x8_t __b, const int __c)
{
  return __builtin_aarch64_vec_smult_laneq_v4hi (__a, __b, __c);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_laneq_s32 (int32x2_t __a, int32x4_t __b, const int __c)
{
  return __builtin_aarch64_vec_smult_laneq_v2si (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_laneq_u16 (uint16x4_t __a, uint16x8_t __b, const int __c)
{
  return __builtin_aarch64_vec_umult_laneq_v4hi_uuus (__a, __b, __c);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_laneq_u32 (uint32x2_t __a, uint32x4_t __b, const int __c)
{
  return __builtin_aarch64_vec_umult_laneq_v2si_uuus (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_n_s16 (int16x4_t __a, int16_t __b)
{
  return __builtin_aarch64_smull_nv4hi (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_n_s32 (int32x2_t __a, int32_t __b)
{
  return __builtin_aarch64_smull_nv2si (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_n_u16 (uint16x4_t __a, uint16_t __b)
{
  return __builtin_aarch64_umull_nv4hi_uuu (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_n_u32 (uint32x2_t __a, uint32_t __b)
{
  return __builtin_aarch64_umull_nv2si_uuu (__a, __b);
}
__extension__ extern __inline poly16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_p8 (poly8x8_t __a, poly8x8_t __b)
{
  return __builtin_aarch64_pmullv8qi_ppp (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_s8 (int8x8_t __a, int8x8_t __b)
{
  return __builtin_aarch64_intrinsic_vec_smult_lo_v8qi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_s16 (int16x4_t __a, int16x4_t __b)
{
  return __builtin_aarch64_intrinsic_vec_smult_lo_v4hi (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_s32 (int32x2_t __a, int32x2_t __b)
{
  return __builtin_aarch64_intrinsic_vec_smult_lo_v2si (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return __builtin_aarch64_intrinsic_vec_umult_lo_v8qi_uuu (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return __builtin_aarch64_intrinsic_vec_umult_lo_v4hi_uuu (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return __builtin_aarch64_intrinsic_vec_umult_lo_v2si_uuu (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpadal_s8 (int16x4_t __a, int8x8_t __b)
{
  return __builtin_aarch64_sadalpv8qi (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpadal_s16 (int32x2_t __a, int16x4_t __b)
{
  return __builtin_aarch64_sadalpv4hi (__a, __b);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpadal_s32 (int64x1_t __a, int32x2_t __b)
{
  return (int64x1_t) __builtin_aarch64_sadalpv2si (__a[0], __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpadal_u8 (uint16x4_t __a, uint8x8_t __b)
{
  return __builtin_aarch64_uadalpv8qi_uuu (__a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpadal_u16 (uint32x2_t __a, uint16x4_t __b)
{
  return __builtin_aarch64_uadalpv4hi_uuu (__a, __b);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpadal_u32 (uint64x1_t __a, uint32x2_t __b)
{
  return (uint64x1_t) __builtin_aarch64_uadalpv2si_uuu (__a[0], __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpadalq_s8 (int16x8_t __a, int8x16_t __b)
{
  return __builtin_aarch64_sadalpv16qi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpadalq_s16 (int32x4_t __a, int16x8_t __b)
{
  return __builtin_aarch64_sadalpv8hi (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpadalq_s32 (int64x2_t __a, int32x4_t __b)
{
  return __builtin_aarch64_sadalpv4si (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpadalq_u8 (uint16x8_t __a, uint8x16_t __b)
{
  return __builtin_aarch64_uadalpv16qi_uuu (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpadalq_u16 (uint32x4_t __a, uint16x8_t __b)
{
  return __builtin_aarch64_uadalpv8hi_uuu (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpadalq_u32 (uint64x2_t __a, uint32x4_t __b)
{
  return __builtin_aarch64_uadalpv4si_uuu (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpaddl_s8 (int8x8_t __a)
{
  return __builtin_aarch64_saddlpv8qi (__a);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpaddl_s16 (int16x4_t __a)
{
  return __builtin_aarch64_saddlpv4hi (__a);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpaddl_s32 (int32x2_t __a)
{
  return (int64x1_t) __builtin_aarch64_saddlpv2si (__a);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpaddl_u8 (uint8x8_t __a)
{
  return __builtin_aarch64_uaddlpv8qi_uu (__a);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpaddl_u16 (uint16x4_t __a)
{
  return __builtin_aarch64_uaddlpv4hi_uu (__a);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpaddl_u32 (uint32x2_t __a)
{
  return (uint64x1_t) __builtin_aarch64_uaddlpv2si_uu (__a);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpaddlq_s8 (int8x16_t __a)
{
  return __builtin_aarch64_saddlpv16qi (__a);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpaddlq_s16 (int16x8_t __a)
{
  return __builtin_aarch64_saddlpv8hi (__a);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpaddlq_s32 (int32x4_t __a)
{
  return __builtin_aarch64_saddlpv4si (__a);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpaddlq_u8 (uint8x16_t __a)
{
  return __builtin_aarch64_uaddlpv16qi_uu (__a);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpaddlq_u16 (uint16x8_t __a)
{
  return __builtin_aarch64_uaddlpv8hi_uu (__a);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpaddlq_u32 (uint32x4_t __a)
{
  return __builtin_aarch64_uaddlpv4si_uu (__a);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpaddq_s8 (int8x16_t __a, int8x16_t __b)
{
  return __builtin_aarch64_addpv16qi (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpaddq_s16 (int16x8_t __a, int16x8_t __b)
{
  return __builtin_aarch64_addpv8hi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpaddq_s32 (int32x4_t __a, int32x4_t __b)
{
  return __builtin_aarch64_addpv4si (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpaddq_s64 (int64x2_t __a, int64x2_t __b)
{
  return __builtin_aarch64_addpv2di (__a, __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpaddq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return __builtin_aarch64_addpv16qi_uuu (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpaddq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return __builtin_aarch64_addpv8hi_uuu (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpaddq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return __builtin_aarch64_addpv4si_uuu (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpaddq_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return __builtin_aarch64_addpv2di_uuu (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmulh_n_s16 (int16x4_t __a, int16_t __b)
{
  return __builtin_aarch64_sqdmulh_nv4hi (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmulh_n_s32 (int32x2_t __a, int32_t __b)
{
  return __builtin_aarch64_sqdmulh_nv2si (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmulhq_n_s16 (int16x8_t __a, int16_t __b)
{
  return __builtin_aarch64_sqdmulh_nv8hi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmulhq_n_s32 (int32x4_t __a, int32_t __b)
{
  return __builtin_aarch64_sqdmulh_nv4si (__a, __b);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqmovn_high_s16 (int8x8_t __a, int16x8_t __b)
{
  return __builtin_aarch64_sqxtn2v8hi (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqmovn_high_s32 (int16x4_t __a, int32x4_t __b)
{
  return __builtin_aarch64_sqxtn2v4si (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqmovn_high_s64 (int32x2_t __a, int64x2_t __b)
{
  return __builtin_aarch64_sqxtn2v2di (__a, __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqmovn_high_u16 (uint8x8_t __a, uint16x8_t __b)
{
  return __builtin_aarch64_uqxtn2v8hi_uuu (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqmovn_high_u32 (uint16x4_t __a, uint32x4_t __b)
{
  return __builtin_aarch64_uqxtn2v4si_uuu (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqmovn_high_u64 (uint32x2_t __a, uint64x2_t __b)
{
  return __builtin_aarch64_uqxtn2v2di_uuu (__a, __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqmovun_high_s16 (uint8x8_t __a, int16x8_t __b)
{
  return __builtin_aarch64_sqxtun2v8hi_uus (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqmovun_high_s32 (uint16x4_t __a, int32x4_t __b)
{
  return __builtin_aarch64_sqxtun2v4si_uus (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqmovun_high_s64 (uint32x2_t __a, int64x2_t __b)
{
  return __builtin_aarch64_sqxtun2v2di_uus (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmulh_n_s16 (int16x4_t __a, int16_t __b)
{
  return __builtin_aarch64_sqrdmulh_nv4hi (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmulh_n_s32 (int32x2_t __a, int32_t __b)
{
  return __builtin_aarch64_sqrdmulh_nv2si (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmulhq_n_s16 (int16x8_t __a, int16_t __b)
{
  return __builtin_aarch64_sqrdmulh_nv8hi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmulhq_n_s32 (int32x4_t __a, int32_t __b)
{
  return __builtin_aarch64_sqrdmulh_nv4si (__a, __b);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshrn_high_n_s16 (int8x8_t __a, int16x8_t __b, const int __c)
{
  return __builtin_aarch64_sqrshrn2_nv8hi (__a, __b, __c);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshrn_high_n_s32 (int16x4_t __a, int32x4_t __b, const int __c)
{
  return __builtin_aarch64_sqrshrn2_nv4si (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshrn_high_n_s64 (int32x2_t __a, int64x2_t __b, const int __c)
{
  return __builtin_aarch64_sqrshrn2_nv2di (__a, __b, __c);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshrn_high_n_u16 (uint8x8_t __a, uint16x8_t __b, const int __c)
{
  return __builtin_aarch64_uqrshrn2_nv8hi_uuus (__a, __b, __c);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshrn_high_n_u32 (uint16x4_t __a, uint32x4_t __b, const int __c)
{
  return __builtin_aarch64_uqrshrn2_nv4si_uuus (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshrn_high_n_u64 (uint32x2_t __a, uint64x2_t __b, const int __c)
{
  return __builtin_aarch64_uqrshrn2_nv2di_uuus (__a, __b, __c);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshrun_high_n_s16 (uint8x8_t __a, int16x8_t __b, const int __c)
{
  return __builtin_aarch64_sqrshrun2_nv8hi_uuss (__a, __b, __c);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshrun_high_n_s32 (uint16x4_t __a, int32x4_t __b, const int __c)
{
  return __builtin_aarch64_sqrshrun2_nv4si_uuss (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshrun_high_n_s64 (uint32x2_t __a, int64x2_t __b, const int __c)
{
  return __builtin_aarch64_sqrshrun2_nv2di_uuss (__a, __b, __c);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshrn_high_n_s16 (int8x8_t __a, int16x8_t __b, const int __c)
{
  return __builtin_aarch64_sqsshrn2_nv8hi (__a, __b, __c);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshrn_high_n_s32 (int16x4_t __a, int32x4_t __b, const int __c)
{
  return __builtin_aarch64_sqsshrn2_nv4si (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshrn_high_n_s64 (int32x2_t __a, int64x2_t __b, const int __c)
{
  return __builtin_aarch64_sqsshrn2_nv2di (__a, __b, __c);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshrn_high_n_u16 (uint8x8_t __a, uint16x8_t __b, const int __c)
{
  return __builtin_aarch64_uqushrn2_nv8hi_uuus (__a, __b, __c);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshrn_high_n_u32 (uint16x4_t __a, uint32x4_t __b, const int __c)
{
  return __builtin_aarch64_uqushrn2_nv4si_uuus (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshrn_high_n_u64 (uint32x2_t __a, uint64x2_t __b, const int __c)
{
  return __builtin_aarch64_uqushrn2_nv2di_uuus (__a, __b, __c);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshrun_high_n_s16 (uint8x8_t __a, int16x8_t __b, const int __c)
{
  return __builtin_aarch64_sqshrun2_nv8hi_uuss (__a, __b, __c);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshrun_high_n_s32 (uint16x4_t __a, int32x4_t __b, const int __c)
{
  return __builtin_aarch64_sqshrun2_nv4si_uuss (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshrun_high_n_s64 (uint32x2_t __a, int64x2_t __b, const int __c)
{
  return __builtin_aarch64_sqshrun2_nv2di_uuss (__a, __b, __c);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshrn_high_n_s16 (int8x8_t __a, int16x8_t __b, const int __c)
{
  return __builtin_aarch64_rshrn2_nv8hi (__a, __b, __c);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshrn_high_n_s32 (int16x4_t __a, int32x4_t __b, const int __c)
{
  return __builtin_aarch64_rshrn2_nv4si (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshrn_high_n_s64 (int32x2_t __a, int64x2_t __b, const int __c)
{
  return __builtin_aarch64_rshrn2_nv2di (__a, __b, __c);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshrn_high_n_u16 (uint8x8_t __a, uint16x8_t __b, const int __c)
{
  return __builtin_aarch64_rshrn2_nv8hi_uuus (__a, __b, __c);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshrn_high_n_u32 (uint16x4_t __a, uint32x4_t __b, const int __c)
{
  return __builtin_aarch64_rshrn2_nv4si_uuus (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshrn_high_n_u64 (uint32x2_t __a, uint64x2_t __b, const int __c)
{
  return __builtin_aarch64_rshrn2_nv2di_uuus (__a, __b, __c);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshrn_n_s16 (int16x8_t __a, const int __b)
{
  return __builtin_aarch64_rshrn_nv8hi (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshrn_n_s32 (int32x4_t __a, const int __b)
{
  return __builtin_aarch64_rshrn_nv4si (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshrn_n_s64 (int64x2_t __a, const int __b)
{
  return __builtin_aarch64_rshrn_nv2di (__a, __b);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshrn_n_u16 (uint16x8_t __a, const int __b)
{
  return __builtin_aarch64_rshrn_nv8hi_uus (__a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshrn_n_u32 (uint32x4_t __a, const int __b)
{
  return __builtin_aarch64_rshrn_nv4si_uus (__a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshrn_n_u64 (uint64x2_t __a, const int __b)
{
  return __builtin_aarch64_rshrn_nv2di_uus (__a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsqrte_u32 (uint32x2_t __a)
{
  return __builtin_aarch64_ursqrtev2si_uu (__a);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsqrteq_u32 (uint32x4_t __a)
{
  return __builtin_aarch64_ursqrtev4si_uu (__a);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshrn_high_n_s16 (int8x8_t __a, int16x8_t __b, const int __c)
{
  return __builtin_aarch64_ushrn2_nv8hi (__a, __b, __c);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshrn_high_n_s32 (int16x4_t __a, int32x4_t __b, const int __c)
{
  return __builtin_aarch64_ushrn2_nv4si (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshrn_high_n_s64 (int32x2_t __a, int64x2_t __b, const int __c)
{
  return __builtin_aarch64_ushrn2_nv2di (__a, __b, __c);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshrn_high_n_u16 (uint8x8_t __a, uint16x8_t __b, const int __c)
{
  return __builtin_aarch64_ushrn2_nv8hi_uuus (__a, __b, __c);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshrn_high_n_u32 (uint16x4_t __a, uint32x4_t __b, const int __c)
{
  return __builtin_aarch64_ushrn2_nv4si_uuus (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshrn_high_n_u64 (uint32x2_t __a, uint64x2_t __b, const int __c)
{
  return __builtin_aarch64_ushrn2_nv2di_uuus (__a, __b, __c);
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsli_n_p8 (poly8x8_t __a, poly8x8_t __b, const int __c)
{
  return __builtin_aarch64_ssli_nv8qi_ppps (__a, __b, __c);
}
__extension__ extern __inline poly16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsli_n_p16 (poly16x4_t __a, poly16x4_t __b, const int __c)
{
  return __builtin_aarch64_ssli_nv4hi_ppps (__a, __b, __c);
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsliq_n_p8 (poly8x16_t __a, poly8x16_t __b, const int __c)
{
  return __builtin_aarch64_ssli_nv16qi_ppps (__a, __b, __c);
}
__extension__ extern __inline poly16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsliq_n_p16 (poly16x8_t __a, poly16x8_t __b, const int __c)
{
  return __builtin_aarch64_ssli_nv8hi_ppps (__a, __b, __c);
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsri_n_p8 (poly8x8_t __a, poly8x8_t __b, const int __c)
{
  return __builtin_aarch64_ssri_nv8qi_ppps (__a, __b, __c);
}
__extension__ extern __inline poly16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsri_n_p16 (poly16x4_t __a, poly16x4_t __b, const int __c)
{
  return __builtin_aarch64_ssri_nv4hi_ppps (__a, __b, __c);
}
__extension__ extern __inline poly64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsri_n_p64 (poly64x1_t __a, poly64x1_t __b, const int __c)
{
  return (poly64x1_t) __builtin_aarch64_ssri_ndi_ppps (__a[0], __b[0], __c);
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsriq_n_p8 (poly8x16_t __a, poly8x16_t __b, const int __c)
{
  return __builtin_aarch64_ssri_nv16qi_ppps (__a, __b, __c);
}
__extension__ extern __inline poly16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsriq_n_p16 (poly16x8_t __a, poly16x8_t __b, const int __c)
{
  return __builtin_aarch64_ssri_nv8hi_ppps (__a, __b, __c);
}
__extension__ extern __inline poly64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsriq_n_p64 (poly64x2_t __a, poly64x2_t __b, const int __c)
{
  return __builtin_aarch64_ssri_nv2di_ppps (__a, __b, __c);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtst_p8 (poly8x8_t __a, poly8x8_t __b)
{
  return (uint8x8_t) ((((uint8x8_t) __a) & ((uint8x8_t) __b))
         != 0);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtst_p16 (poly16x4_t __a, poly16x4_t __b)
{
  return (uint16x4_t) ((((uint16x4_t) __a) & ((uint16x4_t) __b))
         != 0);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtst_p64 (poly64x1_t __a, poly64x1_t __b)
{
  return (uint64x1_t) ((__a & __b) != __AARCH64_INT64_C (0));
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtstq_p8 (poly8x16_t __a, poly8x16_t __b)
{
  return (uint8x16_t) ((((uint8x16_t) __a) & ((uint8x16_t) __b))
         != 0);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtstq_p16 (poly16x8_t __a, poly16x8_t __b)
{
  return (uint16x8_t) ((((uint16x8_t) __a) & ((uint16x8_t) __b))
         != 0);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtstq_p64 (poly64x2_t __a, poly64x2_t __b)
{
  return (uint64x2_t) ((((uint64x2_t) __a) & ((uint64x2_t) __b))
         != __AARCH64_INT64_C (0));
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2_lane_f16 (float16_t *__ptr, float16x4x2_t __val, const int __lane)
{
  __builtin_aarch64_st2_lanev4hf ((__builtin_aarch64_simd_hf *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2_lane_f32 (float32_t *__ptr, float32x2x2_t __val, const int __lane)
{
  __builtin_aarch64_st2_lanev2sf ((__builtin_aarch64_simd_sf *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2_lane_f64 (float64_t *__ptr, float64x1x2_t __val, const int __lane)
{
  __builtin_aarch64_st2_lanedf ((__builtin_aarch64_simd_df *) __ptr, __val,
    __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2_lane_p8 (poly8_t *__ptr, poly8x8x2_t __val, const int __lane)
{
  __builtin_aarch64_st2_lanev8qi_sps ((__builtin_aarch64_simd_qi *) __ptr,
          __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2_lane_p16 (poly16_t *__ptr, poly16x4x2_t __val, const int __lane)
{
  __builtin_aarch64_st2_lanev4hi_sps ((__builtin_aarch64_simd_hi *) __ptr,
          __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2_lane_p64 (poly64_t *__ptr, poly64x1x2_t __val, const int __lane)
{
  __builtin_aarch64_st2_lanedi_sps ((__builtin_aarch64_simd_di *) __ptr,
        __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2_lane_s8 (int8_t *__ptr, int8x8x2_t __val, const int __lane)
{
  __builtin_aarch64_st2_lanev8qi ((__builtin_aarch64_simd_qi *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2_lane_s16 (int16_t *__ptr, int16x4x2_t __val, const int __lane)
{
  __builtin_aarch64_st2_lanev4hi ((__builtin_aarch64_simd_hi *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2_lane_s32 (int32_t *__ptr, int32x2x2_t __val, const int __lane)
{
  __builtin_aarch64_st2_lanev2si ((__builtin_aarch64_simd_si *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2_lane_s64 (int64_t *__ptr, int64x1x2_t __val, const int __lane)
{
  __builtin_aarch64_st2_lanedi ((__builtin_aarch64_simd_di *) __ptr, __val,
    __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2_lane_u8 (uint8_t *__ptr, uint8x8x2_t __val, const int __lane)
{
  __builtin_aarch64_st2_lanev8qi_sus ((__builtin_aarch64_simd_qi *) __ptr,
          __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2_lane_u16 (uint16_t *__ptr, uint16x4x2_t __val, const int __lane)
{
  __builtin_aarch64_st2_lanev4hi_sus ((__builtin_aarch64_simd_hi *) __ptr,
          __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2_lane_u32 (uint32_t *__ptr, uint32x2x2_t __val, const int __lane)
{
  __builtin_aarch64_st2_lanev2si_sus ((__builtin_aarch64_simd_si *) __ptr,
          __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2_lane_u64 (uint64_t *__ptr, uint64x1x2_t __val, const int __lane)
{
  __builtin_aarch64_st2_lanedi_sus ((__builtin_aarch64_simd_di *) __ptr, __val,
        __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2q_lane_f16 (float16_t *__ptr, float16x8x2_t __val, const int __lane)
{
  __builtin_aarch64_st2_lanev8hf ((__builtin_aarch64_simd_hf *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2q_lane_f32 (float32_t *__ptr, float32x4x2_t __val, const int __lane)
{
  __builtin_aarch64_st2_lanev4sf ((__builtin_aarch64_simd_sf *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2q_lane_f64 (float64_t *__ptr, float64x2x2_t __val, const int __lane)
{
  __builtin_aarch64_st2_lanev2df ((__builtin_aarch64_simd_df *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2q_lane_p8 (poly8_t *__ptr, poly8x16x2_t __val, const int __lane)
{
  __builtin_aarch64_st2_lanev16qi_sps ((__builtin_aarch64_simd_qi *) __ptr,
           __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2q_lane_p16 (poly16_t *__ptr, poly16x8x2_t __val, const int __lane)
{
  __builtin_aarch64_st2_lanev8hi_sps ((__builtin_aarch64_simd_hi *) __ptr,
          __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2q_lane_p64 (poly64_t *__ptr, poly64x2x2_t __val, const int __lane)
{
  __builtin_aarch64_st2_lanev2di_sps ((__builtin_aarch64_simd_di *) __ptr,
          __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2q_lane_s8 (int8_t *__ptr, int8x16x2_t __val, const int __lane)
{
  __builtin_aarch64_st2_lanev16qi ((__builtin_aarch64_simd_qi *) __ptr, __val,
       __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2q_lane_s16 (int16_t *__ptr, int16x8x2_t __val, const int __lane)
{
  __builtin_aarch64_st2_lanev8hi ((__builtin_aarch64_simd_hi *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2q_lane_s32 (int32_t *__ptr, int32x4x2_t __val, const int __lane)
{
  __builtin_aarch64_st2_lanev4si ((__builtin_aarch64_simd_si *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2q_lane_s64 (int64_t *__ptr, int64x2x2_t __val, const int __lane)
{
  __builtin_aarch64_st2_lanev2di ((__builtin_aarch64_simd_di *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2q_lane_u8 (uint8_t *__ptr, uint8x16x2_t __val, const int __lane)
{
  __builtin_aarch64_st2_lanev16qi_sus ((__builtin_aarch64_simd_qi *) __ptr,
           __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2q_lane_u16 (uint16_t *__ptr, uint16x8x2_t __val, const int __lane)
{
  __builtin_aarch64_st2_lanev8hi_sus ((__builtin_aarch64_simd_hi *) __ptr,
          __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2q_lane_u32 (uint32_t *__ptr, uint32x4x2_t __val, const int __lane)
{
  __builtin_aarch64_st2_lanev4si_sus ((__builtin_aarch64_simd_si *) __ptr,
          __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2q_lane_u64 (uint64_t *__ptr, uint64x2x2_t __val, const int __lane)
{
  __builtin_aarch64_st2_lanev2di_sus ((__builtin_aarch64_simd_di *) __ptr,
          __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3_lane_f16 (float16_t *__ptr, float16x4x3_t __val, const int __lane)
{
  __builtin_aarch64_st3_lanev4hf ((__builtin_aarch64_simd_hf *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3_lane_f32 (float32_t *__ptr, float32x2x3_t __val, const int __lane)
{
  __builtin_aarch64_st3_lanev2sf ((__builtin_aarch64_simd_sf *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3_lane_f64 (float64_t *__ptr, float64x1x3_t __val, const int __lane)
{
  __builtin_aarch64_st3_lanedf ((__builtin_aarch64_simd_df *) __ptr, __val,
    __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3_lane_p8 (poly8_t *__ptr, poly8x8x3_t __val, const int __lane)
{
  __builtin_aarch64_st3_lanev8qi_sps ((__builtin_aarch64_simd_qi *) __ptr,
          __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3_lane_p16 (poly16_t *__ptr, poly16x4x3_t __val, const int __lane)
{
  __builtin_aarch64_st3_lanev4hi_sps ((__builtin_aarch64_simd_hi *) __ptr,
          __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3_lane_p64 (poly64_t *__ptr, poly64x1x3_t __val, const int __lane)
{
  __builtin_aarch64_st3_lanedi_sps ((__builtin_aarch64_simd_di *) __ptr, __val,
        __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3_lane_s8 (int8_t *__ptr, int8x8x3_t __val, const int __lane)
{
  __builtin_aarch64_st3_lanev8qi ((__builtin_aarch64_simd_qi *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3_lane_s16 (int16_t *__ptr, int16x4x3_t __val, const int __lane)
{
  __builtin_aarch64_st3_lanev4hi ((__builtin_aarch64_simd_hi *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3_lane_s32 (int32_t *__ptr, int32x2x3_t __val, const int __lane)
{
  __builtin_aarch64_st3_lanev2si ((__builtin_aarch64_simd_si *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3_lane_s64 (int64_t *__ptr, int64x1x3_t __val, const int __lane)
{
  __builtin_aarch64_st3_lanedi ((__builtin_aarch64_simd_di *) __ptr, __val,
    __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3_lane_u8 (uint8_t *__ptr, uint8x8x3_t __val, const int __lane)
{
  __builtin_aarch64_st3_lanev8qi_sus ((__builtin_aarch64_simd_qi *) __ptr,
          __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3_lane_u16 (uint16_t *__ptr, uint16x4x3_t __val, const int __lane)
{
  __builtin_aarch64_st3_lanev4hi_sus ((__builtin_aarch64_simd_hi *) __ptr,
          __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3_lane_u32 (uint32_t *__ptr, uint32x2x3_t __val, const int __lane)
{
  __builtin_aarch64_st3_lanev2si_sus ((__builtin_aarch64_simd_si *) __ptr,
          __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3_lane_u64 (uint64_t *__ptr, uint64x1x3_t __val, const int __lane)
{
  __builtin_aarch64_st3_lanedi_sus ((__builtin_aarch64_simd_di *) __ptr, __val,
        __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3q_lane_f16 (float16_t *__ptr, float16x8x3_t __val, const int __lane)
{
  __builtin_aarch64_st3_lanev8hf ((__builtin_aarch64_simd_hf *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3q_lane_f32 (float32_t *__ptr, float32x4x3_t __val, const int __lane)
{
  __builtin_aarch64_st3_lanev4sf ((__builtin_aarch64_simd_sf *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3q_lane_f64 (float64_t *__ptr, float64x2x3_t __val, const int __lane)
{
  __builtin_aarch64_st3_lanev2df ((__builtin_aarch64_simd_df *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3q_lane_p8 (poly8_t *__ptr, poly8x16x3_t __val, const int __lane)
{
  __builtin_aarch64_st3_lanev16qi_sps ((__builtin_aarch64_simd_qi *) __ptr,
           __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3q_lane_p16 (poly16_t *__ptr, poly16x8x3_t __val, const int __lane)
{
  __builtin_aarch64_st3_lanev8hi_sps ((__builtin_aarch64_simd_hi *) __ptr,
          __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3q_lane_p64 (poly64_t *__ptr, poly64x2x3_t __val, const int __lane)
{
  __builtin_aarch64_st3_lanev2di_sps ((__builtin_aarch64_simd_di *) __ptr,
          __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3q_lane_s8 (int8_t *__ptr, int8x16x3_t __val, const int __lane)
{
  __builtin_aarch64_st3_lanev16qi ((__builtin_aarch64_simd_qi *) __ptr, __val,
       __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3q_lane_s16 (int16_t *__ptr, int16x8x3_t __val, const int __lane)
{
  __builtin_aarch64_st3_lanev8hi ((__builtin_aarch64_simd_hi *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3q_lane_s32 (int32_t *__ptr, int32x4x3_t __val, const int __lane)
{
  __builtin_aarch64_st3_lanev4si ((__builtin_aarch64_simd_si *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3q_lane_s64 (int64_t *__ptr, int64x2x3_t __val, const int __lane)
{
  __builtin_aarch64_st3_lanev2di ((__builtin_aarch64_simd_di *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3q_lane_u8 (uint8_t *__ptr, uint8x16x3_t __val, const int __lane)
{
  __builtin_aarch64_st3_lanev16qi_sus ((__builtin_aarch64_simd_qi *) __ptr,
           __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3q_lane_u16 (uint16_t *__ptr, uint16x8x3_t __val, const int __lane)
{
  __builtin_aarch64_st3_lanev8hi_sus ((__builtin_aarch64_simd_hi *) __ptr,
          __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3q_lane_u32 (uint32_t *__ptr, uint32x4x3_t __val, const int __lane)
{
  __builtin_aarch64_st3_lanev4si_sus ((__builtin_aarch64_simd_si *) __ptr,
          __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3q_lane_u64 (uint64_t *__ptr, uint64x2x3_t __val, const int __lane)
{
  __builtin_aarch64_st3_lanev2di_sus ((__builtin_aarch64_simd_di *) __ptr,
          __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4_lane_f16 (float16_t *__ptr, float16x4x4_t __val, const int __lane)
{
  __builtin_aarch64_st4_lanev4hf ((__builtin_aarch64_simd_hf *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4_lane_f32 (float32_t *__ptr, float32x2x4_t __val, const int __lane)
{
  __builtin_aarch64_st4_lanev2sf ((__builtin_aarch64_simd_sf *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4_lane_f64 (float64_t *__ptr, float64x1x4_t __val, const int __lane)
{
  __builtin_aarch64_st4_lanedf ((__builtin_aarch64_simd_df *) __ptr, __val,
    __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4_lane_p8 (poly8_t *__ptr, poly8x8x4_t __val, const int __lane)
{
  __builtin_aarch64_st4_lanev8qi_sps ((__builtin_aarch64_simd_qi *) __ptr,
          __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4_lane_p16 (poly16_t *__ptr, poly16x4x4_t __val, const int __lane)
{
  __builtin_aarch64_st4_lanev4hi_sps ((__builtin_aarch64_simd_hi *) __ptr,
          __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4_lane_p64 (poly64_t *__ptr, poly64x1x4_t __val, const int __lane)
{
  __builtin_aarch64_st4_lanedi_sps ((__builtin_aarch64_simd_di *) __ptr, __val,
        __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4_lane_s8 (int8_t *__ptr, int8x8x4_t __val, const int __lane)
{
  __builtin_aarch64_st4_lanev8qi ((__builtin_aarch64_simd_qi *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4_lane_s16 (int16_t *__ptr, int16x4x4_t __val, const int __lane)
{
  __builtin_aarch64_st4_lanev4hi ((__builtin_aarch64_simd_hi *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4_lane_s32 (int32_t *__ptr, int32x2x4_t __val, const int __lane)
{
  __builtin_aarch64_st4_lanev2si ((__builtin_aarch64_simd_si *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4_lane_s64 (int64_t *__ptr, int64x1x4_t __val, const int __lane)
{
  __builtin_aarch64_st4_lanedi ((__builtin_aarch64_simd_di *) __ptr, __val,
    __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4_lane_u8 (uint8_t *__ptr, uint8x8x4_t __val, const int __lane)
{
  __builtin_aarch64_st4_lanev8qi_sus ((__builtin_aarch64_simd_qi *) __ptr,
          __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4_lane_u16 (uint16_t *__ptr, uint16x4x4_t __val, const int __lane)
{
  __builtin_aarch64_st4_lanev4hi_sus ((__builtin_aarch64_simd_hi *) __ptr,
          __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4_lane_u32 (uint32_t *__ptr, uint32x2x4_t __val, const int __lane)
{
  __builtin_aarch64_st4_lanev2si_sus ((__builtin_aarch64_simd_si *) __ptr,
          __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4_lane_u64 (uint64_t *__ptr, uint64x1x4_t __val, const int __lane)
{
  __builtin_aarch64_st4_lanedi_sus ((__builtin_aarch64_simd_di *) __ptr, __val,
        __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4q_lane_f16 (float16_t *__ptr, float16x8x4_t __val, const int __lane)
{
  __builtin_aarch64_st4_lanev8hf ((__builtin_aarch64_simd_hf *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4q_lane_f32 (float32_t *__ptr, float32x4x4_t __val, const int __lane)
{
  __builtin_aarch64_st4_lanev4sf ((__builtin_aarch64_simd_sf *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4q_lane_f64 (float64_t *__ptr, float64x2x4_t __val, const int __lane)
{
  __builtin_aarch64_st4_lanev2df ((__builtin_aarch64_simd_df *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4q_lane_p8 (poly8_t *__ptr, poly8x16x4_t __val, const int __lane)
{
  __builtin_aarch64_st4_lanev16qi_sps ((__builtin_aarch64_simd_qi *) __ptr,
           __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4q_lane_p16 (poly16_t *__ptr, poly16x8x4_t __val, const int __lane)
{
  __builtin_aarch64_st4_lanev8hi_sps ((__builtin_aarch64_simd_hi *) __ptr,
          __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4q_lane_p64 (poly64_t *__ptr, poly64x2x4_t __val, const int __lane)
{
  __builtin_aarch64_st4_lanev2di_sps ((__builtin_aarch64_simd_di *) __ptr,
          __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4q_lane_s8 (int8_t *__ptr, int8x16x4_t __val, const int __lane)
{
  __builtin_aarch64_st4_lanev16qi ((__builtin_aarch64_simd_qi *) __ptr, __val,
       __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4q_lane_s16 (int16_t *__ptr, int16x8x4_t __val, const int __lane)
{
  __builtin_aarch64_st4_lanev8hi ((__builtin_aarch64_simd_hi *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4q_lane_s32 (int32_t *__ptr, int32x4x4_t __val, const int __lane)
{
  __builtin_aarch64_st4_lanev4si ((__builtin_aarch64_simd_si *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4q_lane_s64 (int64_t *__ptr, int64x2x4_t __val, const int __lane)
{
  __builtin_aarch64_st4_lanev2di ((__builtin_aarch64_simd_di *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4q_lane_u8 (uint8_t *__ptr, uint8x16x4_t __val, const int __lane)
{
  __builtin_aarch64_st4_lanev16qi_sus ((__builtin_aarch64_simd_qi *) __ptr,
           __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4q_lane_u16 (uint16_t *__ptr, uint16x8x4_t __val, const int __lane)
{
  __builtin_aarch64_st4_lanev8hi_sus ((__builtin_aarch64_simd_hi *) __ptr,
          __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4q_lane_u32 (uint32_t *__ptr, uint32x4x4_t __val, const int __lane)
{
  __builtin_aarch64_st4_lanev4si_sus ((__builtin_aarch64_simd_si *) __ptr,
          __val, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4q_lane_u64 (uint64_t *__ptr, uint64x2x4_t __val, const int __lane)
{
  __builtin_aarch64_st4_lanev2di_sus ((__builtin_aarch64_simd_di *) __ptr,
          __val, __lane);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddlv_s32 (int32x2_t __a)
{
  return __builtin_aarch64_saddlvv2si (__a);
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddlv_u32 (uint32x2_t __a)
{
  return __builtin_aarch64_uaddlvv2si_uu (__a);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmulh_laneq_s16 (int16x4_t __a, int16x8_t __b, const int __c)
{
  return __builtin_aarch64_sqdmulh_laneqv4hi (__a, __b, __c);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmulh_laneq_s32 (int32x2_t __a, int32x4_t __b, const int __c)
{
  return __builtin_aarch64_sqdmulh_laneqv2si (__a, __b, __c);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmulhq_laneq_s16 (int16x8_t __a, int16x8_t __b, const int __c)
{
  return __builtin_aarch64_sqdmulh_laneqv8hi (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmulhq_laneq_s32 (int32x4_t __a, int32x4_t __b, const int __c)
{
  return __builtin_aarch64_sqdmulh_laneqv4si (__a, __b, __c);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmulh_laneq_s16 (int16x4_t __a, int16x8_t __b, const int __c)
{
  return __builtin_aarch64_sqrdmulh_laneqv4hi (__a, __b, __c);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmulh_laneq_s32 (int32x2_t __a, int32x4_t __b, const int __c)
{
  return __builtin_aarch64_sqrdmulh_laneqv2si (__a, __b, __c);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmulhq_laneq_s16 (int16x8_t __a, int16x8_t __b, const int __c)
{
  return __builtin_aarch64_sqrdmulh_laneqv8hi (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmulhq_laneq_s32 (int32x4_t __a, int32x4_t __b, const int __c)
{
  return __builtin_aarch64_sqrdmulh_laneqv4si (__a, __b, __c);
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbl1_p8 (poly8x16_t __tab, uint8x8_t __idx)
{
  return __builtin_aarch64_qtbl1v8qi_ppu (__tab, __idx);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbl1_s8 (int8x16_t __tab, uint8x8_t __idx)
{
  return __builtin_aarch64_qtbl1v8qi_ssu (__tab, __idx);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbl1_u8 (uint8x16_t __tab, uint8x8_t __idx)
{
  return __builtin_aarch64_qtbl1v8qi_uuu (__tab, __idx);
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbl1q_p8 (poly8x16_t __tab, uint8x16_t __idx)
{
  return __builtin_aarch64_qtbl1v16qi_ppu (__tab, __idx);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbl1q_s8 (int8x16_t __tab, uint8x16_t __idx)
{
  return __builtin_aarch64_qtbl1v16qi_ssu (__tab, __idx);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbl1q_u8 (uint8x16_t __tab, uint8x16_t __idx)
{
  return __builtin_aarch64_qtbl1v16qi_uuu (__tab, __idx);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbx1_s8 (int8x8_t __r, int8x16_t __tab, uint8x8_t __idx)
{
  return __builtin_aarch64_qtbx1v8qi_sssu (__r, __tab, __idx);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbx1_u8 (uint8x8_t __r, uint8x16_t __tab, uint8x8_t __idx)
{
  return __builtin_aarch64_qtbx1v8qi_uuuu (__r, __tab, __idx);
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbx1_p8 (poly8x8_t __r, poly8x16_t __tab, uint8x8_t __idx)
{
  return __builtin_aarch64_qtbx1v8qi_pppu (__r, __tab, __idx);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbx1q_s8 (int8x16_t __r, int8x16_t __tab, uint8x16_t __idx)
{
  return __builtin_aarch64_qtbx1v16qi_sssu (__r, __tab, __idx);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbx1q_u8 (uint8x16_t __r, uint8x16_t __tab, uint8x16_t __idx)
{
  return __builtin_aarch64_qtbx1v16qi_uuuu (__r, __tab, __idx);
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbx1q_p8 (poly8x16_t __r, poly8x16_t __tab, uint8x16_t __idx)
{
  return __builtin_aarch64_qtbx1v16qi_pppu (__r, __tab, __idx);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtbl1_s8 (int8x8_t __tab, int8x8_t __idx)
{
  int8x16_t __temp = vcombine_s8 (__tab,
      vcreate_s8 (__AARCH64_UINT64_C (0x0)));
  return __builtin_aarch64_qtbl1v8qi (__temp, __idx);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtbl1_u8 (uint8x8_t __tab, uint8x8_t __idx)
{
  uint8x16_t __temp = vcombine_u8 (__tab,
       vcreate_u8 (__AARCH64_UINT64_C (0x0)));
  return __builtin_aarch64_qtbl1v8qi_uuu (__temp, __idx);
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtbl1_p8 (poly8x8_t __tab, uint8x8_t __idx)
{
  poly8x16_t __temp = vcombine_p8 (__tab,
       vcreate_p8 (__AARCH64_UINT64_C (0x0)));
  return __builtin_aarch64_qtbl1v8qi_ppu (__temp, __idx);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtbl2_s8 (int8x8x2_t __tab, int8x8_t __idx)
{
  int8x16_t __temp = vcombine_s8 (__tab.val[0], __tab.val[1]);
  return __builtin_aarch64_qtbl1v8qi (__temp, __idx);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtbl2_u8 (uint8x8x2_t __tab, uint8x8_t __idx)
{
  uint8x16_t __temp = vcombine_u8 (__tab.val[0], __tab.val[1]);
  return __builtin_aarch64_qtbl1v8qi_uuu (__temp, __idx);
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtbl2_p8 (poly8x8x2_t __tab, uint8x8_t __idx)
{
  poly8x16_t __temp = vcombine_p8 (__tab.val[0], __tab.val[1]);
  return __builtin_aarch64_qtbl1v8qi_ppu (__temp, __idx);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtbl3_s8 (int8x8x3_t __tab, int8x8_t __idx)
{
  int8x16x2_t __temp;
  __temp.val[0] = vcombine_s8 (__tab.val[0], __tab.val[1]);
  __temp.val[1] = vcombine_s8 (__tab.val[2],
          vcreate_s8 (__AARCH64_UINT64_C (0x0)));
  return __builtin_aarch64_qtbl2v8qi (__temp, __idx);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtbl3_u8 (uint8x8x3_t __tab, uint8x8_t __idx)
{
  uint8x16x2_t __temp;
  __temp.val[0] = vcombine_u8 (__tab.val[0], __tab.val[1]);
  __temp.val[1] = vcombine_u8 (__tab.val[2],
          vcreate_u8 (__AARCH64_UINT64_C (0x0)));
  return __builtin_aarch64_qtbl2v8qi_uuu (__temp, __idx);
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtbl3_p8 (poly8x8x3_t __tab, uint8x8_t __idx)
{
  poly8x16x2_t __temp;
  __temp.val[0] = vcombine_p8 (__tab.val[0], __tab.val[1]);
  __temp.val[1] = vcombine_p8 (__tab.val[2],
          vcreate_p8 (__AARCH64_UINT64_C (0x0)));
  return __builtin_aarch64_qtbl2v8qi_ppu (__temp, __idx);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtbl4_s8 (int8x8x4_t __tab, int8x8_t __idx)
{
  int8x16x2_t __temp;
  __temp.val[0] = vcombine_s8 (__tab.val[0], __tab.val[1]);
  __temp.val[1] = vcombine_s8 (__tab.val[2], __tab.val[3]);
  return __builtin_aarch64_qtbl2v8qi (__temp, __idx);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtbl4_u8 (uint8x8x4_t __tab, uint8x8_t __idx)
{
  uint8x16x2_t __temp;
  __temp.val[0] = vcombine_u8 (__tab.val[0], __tab.val[1]);
  __temp.val[1] = vcombine_u8 (__tab.val[2], __tab.val[3]);
  return __builtin_aarch64_qtbl2v8qi_uuu (__temp, __idx);
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtbl4_p8 (poly8x8x4_t __tab, uint8x8_t __idx)
{
  poly8x16x2_t __temp;
  __temp.val[0] = vcombine_p8 (__tab.val[0], __tab.val[1]);
  __temp.val[1] = vcombine_p8 (__tab.val[2], __tab.val[3]);
  return __builtin_aarch64_qtbl2v8qi_ppu (__temp, __idx);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtbx2_s8 (int8x8_t __r, int8x8x2_t __tab, int8x8_t __idx)
{
  int8x16_t __temp = vcombine_s8 (__tab.val[0], __tab.val[1]);
  return __builtin_aarch64_qtbx1v8qi (__r, __temp, __idx);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtbx2_u8 (uint8x8_t __r, uint8x8x2_t __tab, uint8x8_t __idx)
{
  uint8x16_t __temp = vcombine_u8 (__tab.val[0], __tab.val[1]);
  return __builtin_aarch64_qtbx1v8qi_uuuu (__r, __temp, __idx);
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtbx2_p8 (poly8x8_t __r, poly8x8x2_t __tab, uint8x8_t __idx)
{
  poly8x16_t __temp = vcombine_p8 (__tab.val[0], __tab.val[1]);
  return __builtin_aarch64_qtbx1v8qi_pppu (__r, __temp, __idx);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabds_f32 (float32_t __a, float32_t __b)
{
  return __builtin_aarch64_fabdsf (__a, __b);
}
__extension__ extern __inline float64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabdd_f64 (float64_t __a, float64_t __b)
{
  return __builtin_aarch64_fabddf (__a, __b);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabd_f32 (float32x2_t __a, float32x2_t __b)
{
  return __builtin_aarch64_fabdv2sf (__a, __b);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabd_f64 (float64x1_t __a, float64x1_t __b)
{
  return (float64x1_t) {vabdd_f64 (vget_lane_f64 (__a, 0),
       vget_lane_f64 (__b, 0))};
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabdq_f32 (float32x4_t __a, float32x4_t __b)
{
  return __builtin_aarch64_fabdv4sf (__a, __b);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabdq_f64 (float64x2_t __a, float64x2_t __b)
{
  return __builtin_aarch64_fabdv2df (__a, __b);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabs_f32 (float32x2_t __a)
{
  return __builtin_aarch64_absv2sf (__a);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabs_f64 (float64x1_t __a)
{
  return (float64x1_t) {__builtin_fabs (__a[0])};
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabs_s8 (int8x8_t __a)
{
  return __builtin_aarch64_absv8qi (__a);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabs_s16 (int16x4_t __a)
{
  return __builtin_aarch64_absv4hi (__a);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabs_s32 (int32x2_t __a)
{
  return __builtin_aarch64_absv2si (__a);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabs_s64 (int64x1_t __a)
{
  return (int64x1_t) {__builtin_aarch64_absdi (__a[0])};
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabsq_f32 (float32x4_t __a)
{
  return __builtin_aarch64_absv4sf (__a);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabsq_f64 (float64x2_t __a)
{
  return __builtin_aarch64_absv2df (__a);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabsq_s8 (int8x16_t __a)
{
  return __builtin_aarch64_absv16qi (__a);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabsq_s16 (int16x8_t __a)
{
  return __builtin_aarch64_absv8hi (__a);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabsq_s32 (int32x4_t __a)
{
  return __builtin_aarch64_absv4si (__a);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabsq_s64 (int64x2_t __a)
{
  return __builtin_aarch64_absv2di (__a);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabsd_s64 (int64_t __a)
{
  return __a < 0 ? - (uint64_t) __a : __a;
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddd_s64 (int64_t __a, int64_t __b)
{
  return __a + __b;
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddd_u64 (uint64_t __a, uint64_t __b)
{
  return __a + __b;
}
__extension__ extern __inline int8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddv_s8 (int8x8_t __a)
{
  return __builtin_aarch64_reduc_plus_scal_v8qi (__a);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddv_s16 (int16x4_t __a)
{
  return __builtin_aarch64_reduc_plus_scal_v4hi (__a);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddv_s32 (int32x2_t __a)
{
  return __builtin_aarch64_reduc_plus_scal_v2si (__a);
}
__extension__ extern __inline uint8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddv_u8 (uint8x8_t __a)
{
  return __builtin_aarch64_reduc_plus_scal_v8qi_uu (__a);
}
__extension__ extern __inline uint16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddv_u16 (uint16x4_t __a)
{
  return __builtin_aarch64_reduc_plus_scal_v4hi_uu (__a);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddv_u32 (uint32x2_t __a)
{
  return __builtin_aarch64_reduc_plus_scal_v2si_uu (__a);
}
__extension__ extern __inline int8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddvq_s8 (int8x16_t __a)
{
  return __builtin_aarch64_reduc_plus_scal_v16qi (__a);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddvq_s16 (int16x8_t __a)
{
  return __builtin_aarch64_reduc_plus_scal_v8hi (__a);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddvq_s32 (int32x4_t __a)
{
  return __builtin_aarch64_reduc_plus_scal_v4si (__a);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddvq_s64 (int64x2_t __a)
{
  return __builtin_aarch64_reduc_plus_scal_v2di (__a);
}
__extension__ extern __inline uint8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddvq_u8 (uint8x16_t __a)
{
  return __builtin_aarch64_reduc_plus_scal_v16qi_uu (__a);
}
__extension__ extern __inline uint16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddvq_u16 (uint16x8_t __a)
{
  return __builtin_aarch64_reduc_plus_scal_v8hi_uu (__a);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddvq_u32 (uint32x4_t __a)
{
  return __builtin_aarch64_reduc_plus_scal_v4si_uu (__a);
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddvq_u64 (uint64x2_t __a)
{
  return __builtin_aarch64_reduc_plus_scal_v2di_uu (__a);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddv_f32 (float32x2_t __a)
{
  return __builtin_aarch64_reduc_plus_scal_v2sf (__a);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddvq_f32 (float32x4_t __a)
{
  return __builtin_aarch64_reduc_plus_scal_v4sf (__a);
}
__extension__ extern __inline float64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddvq_f64 (float64x2_t __a)
{
  return __builtin_aarch64_reduc_plus_scal_v2df (__a);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbsl_f16 (uint16x4_t __a, float16x4_t __b, float16x4_t __c)
{
  return __builtin_aarch64_simd_bslv4hf_suss (__a, __b, __c);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbsl_f32 (uint32x2_t __a, float32x2_t __b, float32x2_t __c)
{
  return __builtin_aarch64_simd_bslv2sf_suss (__a, __b, __c);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbsl_f64 (uint64x1_t __a, float64x1_t __b, float64x1_t __c)
{
  return (float64x1_t)
    { __builtin_aarch64_simd_bsldf_suss (__a[0], __b[0], __c[0]) };
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbsl_p8 (uint8x8_t __a, poly8x8_t __b, poly8x8_t __c)
{
  return __builtin_aarch64_simd_bslv8qi_pupp (__a, __b, __c);
}
__extension__ extern __inline poly16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbsl_p16 (uint16x4_t __a, poly16x4_t __b, poly16x4_t __c)
{
  return __builtin_aarch64_simd_bslv4hi_pupp (__a, __b, __c);
}
__extension__ extern __inline poly64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbsl_p64 (uint64x1_t __a, poly64x1_t __b, poly64x1_t __c)
{
  return (poly64x1_t)
      {__builtin_aarch64_simd_bsldi_pupp (__a[0], __b[0], __c[0])};
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbsl_s8 (uint8x8_t __a, int8x8_t __b, int8x8_t __c)
{
  return __builtin_aarch64_simd_bslv8qi_suss (__a, __b, __c);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbsl_s16 (uint16x4_t __a, int16x4_t __b, int16x4_t __c)
{
  return __builtin_aarch64_simd_bslv4hi_suss (__a, __b, __c);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbsl_s32 (uint32x2_t __a, int32x2_t __b, int32x2_t __c)
{
  return __builtin_aarch64_simd_bslv2si_suss (__a, __b, __c);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbsl_s64 (uint64x1_t __a, int64x1_t __b, int64x1_t __c)
{
  return (int64x1_t)
      {__builtin_aarch64_simd_bsldi_suss (__a[0], __b[0], __c[0])};
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbsl_u8 (uint8x8_t __a, uint8x8_t __b, uint8x8_t __c)
{
  return __builtin_aarch64_simd_bslv8qi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbsl_u16 (uint16x4_t __a, uint16x4_t __b, uint16x4_t __c)
{
  return __builtin_aarch64_simd_bslv4hi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbsl_u32 (uint32x2_t __a, uint32x2_t __b, uint32x2_t __c)
{
  return __builtin_aarch64_simd_bslv2si_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbsl_u64 (uint64x1_t __a, uint64x1_t __b, uint64x1_t __c)
{
  return (uint64x1_t)
      {__builtin_aarch64_simd_bsldi_uuuu (__a[0], __b[0], __c[0])};
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbslq_f16 (uint16x8_t __a, float16x8_t __b, float16x8_t __c)
{
  return __builtin_aarch64_simd_bslv8hf_suss (__a, __b, __c);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbslq_f32 (uint32x4_t __a, float32x4_t __b, float32x4_t __c)
{
  return __builtin_aarch64_simd_bslv4sf_suss (__a, __b, __c);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbslq_f64 (uint64x2_t __a, float64x2_t __b, float64x2_t __c)
{
  return __builtin_aarch64_simd_bslv2df_suss (__a, __b, __c);
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbslq_p8 (uint8x16_t __a, poly8x16_t __b, poly8x16_t __c)
{
  return __builtin_aarch64_simd_bslv16qi_pupp (__a, __b, __c);
}
__extension__ extern __inline poly16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbslq_p16 (uint16x8_t __a, poly16x8_t __b, poly16x8_t __c)
{
  return __builtin_aarch64_simd_bslv8hi_pupp (__a, __b, __c);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbslq_s8 (uint8x16_t __a, int8x16_t __b, int8x16_t __c)
{
  return __builtin_aarch64_simd_bslv16qi_suss (__a, __b, __c);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbslq_s16 (uint16x8_t __a, int16x8_t __b, int16x8_t __c)
{
  return __builtin_aarch64_simd_bslv8hi_suss (__a, __b, __c);
}
__extension__ extern __inline poly64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbslq_p64 (uint64x2_t __a, poly64x2_t __b, poly64x2_t __c)
{
  return __builtin_aarch64_simd_bslv2di_pupp (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbslq_s32 (uint32x4_t __a, int32x4_t __b, int32x4_t __c)
{
  return __builtin_aarch64_simd_bslv4si_suss (__a, __b, __c);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbslq_s64 (uint64x2_t __a, int64x2_t __b, int64x2_t __c)
{
  return __builtin_aarch64_simd_bslv2di_suss (__a, __b, __c);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbslq_u8 (uint8x16_t __a, uint8x16_t __b, uint8x16_t __c)
{
  return __builtin_aarch64_simd_bslv16qi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbslq_u16 (uint16x8_t __a, uint16x8_t __b, uint16x8_t __c)
{
  return __builtin_aarch64_simd_bslv8hi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbslq_u32 (uint32x4_t __a, uint32x4_t __b, uint32x4_t __c)
{
  return __builtin_aarch64_simd_bslv4si_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbslq_u64 (uint64x2_t __a, uint64x2_t __b, uint64x2_t __c)
{
  return __builtin_aarch64_simd_bslv2di_uuuu (__a, __b, __c);
}
#pragma GCC push_options
#pragma GCC target ("+nothing+rdma")
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlah_s16 (int16x4_t __a, int16x4_t __b, int16x4_t __c)
{
  return __builtin_aarch64_sqrdmlahv4hi (__a, __b, __c);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlah_s32 (int32x2_t __a, int32x2_t __b, int32x2_t __c)
{
  return __builtin_aarch64_sqrdmlahv2si (__a, __b, __c);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlahq_s16 (int16x8_t __a, int16x8_t __b, int16x8_t __c)
{
  return __builtin_aarch64_sqrdmlahv8hi (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlahq_s32 (int32x4_t __a, int32x4_t __b, int32x4_t __c)
{
  return __builtin_aarch64_sqrdmlahv4si (__a, __b, __c);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlsh_s16 (int16x4_t __a, int16x4_t __b, int16x4_t __c)
{
  return __builtin_aarch64_sqrdmlshv4hi (__a, __b, __c);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlsh_s32 (int32x2_t __a, int32x2_t __b, int32x2_t __c)
{
  return __builtin_aarch64_sqrdmlshv2si (__a, __b, __c);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlshq_s16 (int16x8_t __a, int16x8_t __b, int16x8_t __c)
{
  return __builtin_aarch64_sqrdmlshv8hi (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlshq_s32 (int32x4_t __a, int32x4_t __b, int32x4_t __c)
{
  return __builtin_aarch64_sqrdmlshv4si (__a, __b, __c);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlah_laneq_s16 (int16x4_t __a, int16x4_t __b, int16x8_t __c, const int __d)
{
  return __builtin_aarch64_sqrdmlah_laneqv4hi (__a, __b, __c, __d);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlah_laneq_s32 (int32x2_t __a, int32x2_t __b, int32x4_t __c, const int __d)
{
  return __builtin_aarch64_sqrdmlah_laneqv2si (__a, __b, __c, __d);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlahq_laneq_s16 (int16x8_t __a, int16x8_t __b, int16x8_t __c, const int __d)
{
  return __builtin_aarch64_sqrdmlah_laneqv8hi (__a, __b, __c, __d);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlahq_laneq_s32 (int32x4_t __a, int32x4_t __b, int32x4_t __c, const int __d)
{
  return __builtin_aarch64_sqrdmlah_laneqv4si (__a, __b, __c, __d);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlsh_laneq_s16 (int16x4_t __a, int16x4_t __b, int16x8_t __c, const int __d)
{
  return __builtin_aarch64_sqrdmlsh_laneqv4hi (__a, __b, __c, __d);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlsh_laneq_s32 (int32x2_t __a, int32x2_t __b, int32x4_t __c, const int __d)
{
  return __builtin_aarch64_sqrdmlsh_laneqv2si (__a, __b, __c, __d);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlshq_laneq_s16 (int16x8_t __a, int16x8_t __b, int16x8_t __c, const int __d)
{
  return __builtin_aarch64_sqrdmlsh_laneqv8hi (__a, __b, __c, __d);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlshq_laneq_s32 (int32x4_t __a, int32x4_t __b, int32x4_t __c, const int __d)
{
  return __builtin_aarch64_sqrdmlsh_laneqv4si (__a, __b, __c, __d);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlah_lane_s16 (int16x4_t __a, int16x4_t __b, int16x4_t __c, const int __d)
{
  return __builtin_aarch64_sqrdmlah_lanev4hi (__a, __b, __c, __d);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlah_lane_s32 (int32x2_t __a, int32x2_t __b, int32x2_t __c, const int __d)
{
  return __builtin_aarch64_sqrdmlah_lanev2si (__a, __b, __c, __d);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlahq_lane_s16 (int16x8_t __a, int16x8_t __b, int16x4_t __c, const int __d)
{
  return __builtin_aarch64_sqrdmlah_lanev8hi (__a, __b, __c, __d);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlahq_lane_s32 (int32x4_t __a, int32x4_t __b, int32x2_t __c, const int __d)
{
  return __builtin_aarch64_sqrdmlah_lanev4si (__a, __b, __c, __d);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlahh_s16 (int16_t __a, int16_t __b, int16_t __c)
{
  return (int16_t) __builtin_aarch64_sqrdmlahhi (__a, __b, __c);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlahh_lane_s16 (int16_t __a, int16_t __b, int16x4_t __c, const int __d)
{
  return __builtin_aarch64_sqrdmlah_lanehi (__a, __b, __c, __d);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlahh_laneq_s16 (int16_t __a, int16_t __b, int16x8_t __c, const int __d)
{
  return __builtin_aarch64_sqrdmlah_laneqhi (__a, __b, __c, __d);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlahs_s32 (int32_t __a, int32_t __b, int32_t __c)
{
  return (int32_t) __builtin_aarch64_sqrdmlahsi (__a, __b, __c);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlahs_lane_s32 (int32_t __a, int32_t __b, int32x2_t __c, const int __d)
{
  return __builtin_aarch64_sqrdmlah_lanesi (__a, __b, __c, __d);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlahs_laneq_s32 (int32_t __a, int32_t __b, int32x4_t __c, const int __d)
{
  return __builtin_aarch64_sqrdmlah_laneqsi (__a, __b, __c, __d);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlsh_lane_s16 (int16x4_t __a, int16x4_t __b, int16x4_t __c, const int __d)
{
  return __builtin_aarch64_sqrdmlsh_lanev4hi (__a, __b, __c, __d);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlsh_lane_s32 (int32x2_t __a, int32x2_t __b, int32x2_t __c, const int __d)
{
  return __builtin_aarch64_sqrdmlsh_lanev2si (__a, __b, __c, __d);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlshq_lane_s16 (int16x8_t __a, int16x8_t __b, int16x4_t __c, const int __d)
{
  return __builtin_aarch64_sqrdmlsh_lanev8hi (__a, __b, __c, __d);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlshq_lane_s32 (int32x4_t __a, int32x4_t __b, int32x2_t __c, const int __d)
{
  return __builtin_aarch64_sqrdmlsh_lanev4si (__a, __b, __c, __d);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlshh_s16 (int16_t __a, int16_t __b, int16_t __c)
{
  return (int16_t) __builtin_aarch64_sqrdmlshhi (__a, __b, __c);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlshh_lane_s16 (int16_t __a, int16_t __b, int16x4_t __c, const int __d)
{
  return __builtin_aarch64_sqrdmlsh_lanehi (__a, __b, __c, __d);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlshh_laneq_s16 (int16_t __a, int16_t __b, int16x8_t __c, const int __d)
{
  return __builtin_aarch64_sqrdmlsh_laneqhi (__a, __b, __c, __d);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlshs_s32 (int32_t __a, int32_t __b, int32_t __c)
{
  return (int32_t) __builtin_aarch64_sqrdmlshsi (__a, __b, __c);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlshs_lane_s32 (int32_t __a, int32_t __b, int32x2_t __c, const int __d)
{
  return __builtin_aarch64_sqrdmlsh_lanesi (__a, __b, __c, __d);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmlshs_laneq_s32 (int32_t __a, int32_t __b, int32x4_t __c, const int __d)
{
  return __builtin_aarch64_sqrdmlsh_laneqsi (__a, __b, __c, __d);
}
#pragma GCC pop_options
#pragma GCC push_options
#pragma GCC target ("+nothing+aes")
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaeseq_u8 (uint8x16_t data, uint8x16_t key)
{
  return __builtin_aarch64_crypto_aesev16qi_uuu (data, key);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaesdq_u8 (uint8x16_t data, uint8x16_t key)
{
  return __builtin_aarch64_crypto_aesdv16qi_uuu (data, key);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaesmcq_u8 (uint8x16_t data)
{
  return __builtin_aarch64_crypto_aesmcv16qi_uu (data);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaesimcq_u8 (uint8x16_t data)
{
  return __builtin_aarch64_crypto_aesimcv16qi_uu (data);
}
__extension__ extern __inline poly128_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_p64 (poly64_t __a, poly64_t __b)
{
  return
    __builtin_aarch64_crypto_pmulldi_ppp (__a, __b);
}
__extension__ extern __inline poly128_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmull_high_p64 (poly64x2_t __a, poly64x2_t __b)
{
  return __builtin_aarch64_crypto_pmullv2di_ppp (__a, __b);
}
#pragma GCC pop_options
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcage_f64 (float64x1_t __a, float64x1_t __b)
{
  return vcreate_u64 (__builtin_aarch64_facgedf_uss (__a[0], __b[0]));
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcages_f32 (float32_t __a, float32_t __b)
{
  return __builtin_aarch64_facgesf_uss (__a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcage_f32 (float32x2_t __a, float32x2_t __b)
{
  return __builtin_aarch64_facgev2sf_uss (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcageq_f32 (float32x4_t __a, float32x4_t __b)
{
  return __builtin_aarch64_facgev4sf_uss (__a, __b);
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcaged_f64 (float64_t __a, float64_t __b)
{
  return __builtin_aarch64_facgedf_uss (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcageq_f64 (float64x2_t __a, float64x2_t __b)
{
  return __builtin_aarch64_facgev2df_uss (__a, __b);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcagts_f32 (float32_t __a, float32_t __b)
{
  return __builtin_aarch64_facgtsf_uss (__a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcagt_f32 (float32x2_t __a, float32x2_t __b)
{
  return __builtin_aarch64_facgtv2sf_uss (__a, __b);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcagt_f64 (float64x1_t __a, float64x1_t __b)
{
  return vcreate_u64 (__builtin_aarch64_facgtdf_uss (__a[0], __b[0]));
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcagtq_f32 (float32x4_t __a, float32x4_t __b)
{
  return __builtin_aarch64_facgtv4sf_uss (__a, __b);
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcagtd_f64 (float64_t __a, float64_t __b)
{
  return __builtin_aarch64_facgtdf_uss (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcagtq_f64 (float64x2_t __a, float64x2_t __b)
{
  return __builtin_aarch64_facgtv2df_uss (__a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcale_f32 (float32x2_t __a, float32x2_t __b)
{
  return __builtin_aarch64_faclev2sf_uss (__a, __b);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcale_f64 (float64x1_t __a, float64x1_t __b)
{
  return vcreate_u64 (__builtin_aarch64_facledf_uss (__a[0], __b[0]));
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcaled_f64 (float64_t __a, float64_t __b)
{
  return __builtin_aarch64_facledf_uss (__a, __b);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcales_f32 (float32_t __a, float32_t __b)
{
  return __builtin_aarch64_faclesf_uss (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcaleq_f32 (float32x4_t __a, float32x4_t __b)
{
  return __builtin_aarch64_faclev4sf_uss (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcaleq_f64 (float64x2_t __a, float64x2_t __b)
{
  return __builtin_aarch64_faclev2df_uss (__a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcalt_f32 (float32x2_t __a, float32x2_t __b)
{
  return __builtin_aarch64_facltv2sf_uss (__a, __b);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcalt_f64 (float64x1_t __a, float64x1_t __b)
{
  return vcreate_u64 (__builtin_aarch64_facltdf_uss (__a[0], __b[0]));
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcaltd_f64 (float64_t __a, float64_t __b)
{
  return __builtin_aarch64_facltdf_uss (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcaltq_f32 (float32x4_t __a, float32x4_t __b)
{
  return __builtin_aarch64_facltv4sf_uss (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcaltq_f64 (float64x2_t __a, float64x2_t __b)
{
  return __builtin_aarch64_facltv2df_uss (__a, __b);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcalts_f32 (float32_t __a, float32_t __b)
{
  return __builtin_aarch64_facltsf_uss (__a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceq_f32 (float32x2_t __a, float32x2_t __b)
{
  return (uint32x2_t) (__a == __b);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceq_f64 (float64x1_t __a, float64x1_t __b)
{
  return (uint64x1_t) (__a == __b);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceq_p8 (poly8x8_t __a, poly8x8_t __b)
{
  return (uint8x8_t) (__a == __b);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceq_p64 (poly64x1_t __a, poly64x1_t __b)
{
  return (uint64x1_t) (__a == __b);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceq_s8 (int8x8_t __a, int8x8_t __b)
{
  return (uint8x8_t) (__a == __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceq_s16 (int16x4_t __a, int16x4_t __b)
{
  return (uint16x4_t) (__a == __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceq_s32 (int32x2_t __a, int32x2_t __b)
{
  return (uint32x2_t) (__a == __b);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceq_s64 (int64x1_t __a, int64x1_t __b)
{
  return (uint64x1_t) (__a == __b);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceq_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (__a == __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceq_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (__a == __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceq_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (__a == __b);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceq_u64 (uint64x1_t __a, uint64x1_t __b)
{
  return (__a == __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqq_f32 (float32x4_t __a, float32x4_t __b)
{
  return (uint32x4_t) (__a == __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqq_f64 (float64x2_t __a, float64x2_t __b)
{
  return (uint64x2_t) (__a == __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqq_p8 (poly8x16_t __a, poly8x16_t __b)
{
  return (uint8x16_t) (__a == __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (uint8x16_t) (__a == __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (uint16x8_t) (__a == __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (uint32x4_t) (__a == __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqq_s64 (int64x2_t __a, int64x2_t __b)
{
  return (uint64x2_t) (__a == __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return (__a == __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (__a == __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (__a == __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqq_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return (__a == __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqq_p64 (poly64x2_t __a, poly64x2_t __b)
{
  return (__a == __b);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqs_f32 (float32_t __a, float32_t __b)
{
  return __a == __b ? -1 : 0;
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqd_s64 (int64_t __a, int64_t __b)
{
  return __a == __b ? -1ll : 0ll;
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqd_u64 (uint64_t __a, uint64_t __b)
{
  return __a == __b ? -1ll : 0ll;
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqd_f64 (float64_t __a, float64_t __b)
{
  return __a == __b ? -1ll : 0ll;
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqz_f32 (float32x2_t __a)
{
  return (uint32x2_t) (__a == 0.0f);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqz_f64 (float64x1_t __a)
{
  return (uint64x1_t) (__a == (float64x1_t) {0.0});
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqz_p8 (poly8x8_t __a)
{
  return (uint8x8_t) (__a == 0);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqz_s8 (int8x8_t __a)
{
  return (uint8x8_t) (__a == 0);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqz_s16 (int16x4_t __a)
{
  return (uint16x4_t) (__a == 0);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqz_s32 (int32x2_t __a)
{
  return (uint32x2_t) (__a == 0);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqz_s64 (int64x1_t __a)
{
  return (uint64x1_t) (__a == __AARCH64_INT64_C (0));
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqz_u8 (uint8x8_t __a)
{
  return (__a == 0);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqz_u16 (uint16x4_t __a)
{
  return (__a == 0);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqz_u32 (uint32x2_t __a)
{
  return (__a == 0);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqz_u64 (uint64x1_t __a)
{
  return (__a == __AARCH64_UINT64_C (0));
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqz_p64 (poly64x1_t __a)
{
  return (__a == __AARCH64_UINT64_C (0));
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqzq_f32 (float32x4_t __a)
{
  return (uint32x4_t) (__a == 0.0f);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqzq_f64 (float64x2_t __a)
{
  return (uint64x2_t) (__a == 0.0f);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqzq_p8 (poly8x16_t __a)
{
  return (uint8x16_t) (__a == 0);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqzq_s8 (int8x16_t __a)
{
  return (uint8x16_t) (__a == 0);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqzq_s16 (int16x8_t __a)
{
  return (uint16x8_t) (__a == 0);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqzq_s32 (int32x4_t __a)
{
  return (uint32x4_t) (__a == 0);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqzq_s64 (int64x2_t __a)
{
  return (uint64x2_t) (__a == __AARCH64_INT64_C (0));
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqzq_u8 (uint8x16_t __a)
{
  return (__a == 0);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqzq_u16 (uint16x8_t __a)
{
  return (__a == 0);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqzq_u32 (uint32x4_t __a)
{
  return (__a == 0);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqzq_u64 (uint64x2_t __a)
{
  return (__a == __AARCH64_UINT64_C (0));
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqzq_p64 (poly64x2_t __a)
{
  return (__a == __AARCH64_UINT64_C (0));
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqzs_f32 (float32_t __a)
{
  return __a == 0.0f ? -1 : 0;
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqzd_s64 (int64_t __a)
{
  return __a == 0 ? -1ll : 0ll;
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqzd_u64 (uint64_t __a)
{
  return __a == 0 ? -1ll : 0ll;
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqzd_f64 (float64_t __a)
{
  return __a == 0.0 ? -1ll : 0ll;
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcge_f32 (float32x2_t __a, float32x2_t __b)
{
  return (uint32x2_t) (__a >= __b);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcge_f64 (float64x1_t __a, float64x1_t __b)
{
  return (uint64x1_t) (__a >= __b);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcge_s8 (int8x8_t __a, int8x8_t __b)
{
  return (uint8x8_t) (__a >= __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcge_s16 (int16x4_t __a, int16x4_t __b)
{
  return (uint16x4_t) (__a >= __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcge_s32 (int32x2_t __a, int32x2_t __b)
{
  return (uint32x2_t) (__a >= __b);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcge_s64 (int64x1_t __a, int64x1_t __b)
{
  return (uint64x1_t) (__a >= __b);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcge_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (__a >= __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcge_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (__a >= __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcge_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (__a >= __b);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcge_u64 (uint64x1_t __a, uint64x1_t __b)
{
  return (__a >= __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgeq_f32 (float32x4_t __a, float32x4_t __b)
{
  return (uint32x4_t) (__a >= __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgeq_f64 (float64x2_t __a, float64x2_t __b)
{
  return (uint64x2_t) (__a >= __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgeq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (uint8x16_t) (__a >= __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgeq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (uint16x8_t) (__a >= __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgeq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (uint32x4_t) (__a >= __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgeq_s64 (int64x2_t __a, int64x2_t __b)
{
  return (uint64x2_t) (__a >= __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgeq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return (__a >= __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgeq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (__a >= __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgeq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (__a >= __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgeq_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return (__a >= __b);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcges_f32 (float32_t __a, float32_t __b)
{
  return __a >= __b ? -1 : 0;
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcged_s64 (int64_t __a, int64_t __b)
{
  return __a >= __b ? -1ll : 0ll;
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcged_u64 (uint64_t __a, uint64_t __b)
{
  return __a >= __b ? -1ll : 0ll;
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcged_f64 (float64_t __a, float64_t __b)
{
  return __a >= __b ? -1ll : 0ll;
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgez_f32 (float32x2_t __a)
{
  return (uint32x2_t) (__a >= 0.0f);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgez_f64 (float64x1_t __a)
{
  return (uint64x1_t) (__a[0] >= (float64x1_t) {0.0});
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgez_s8 (int8x8_t __a)
{
  return (uint8x8_t) (__a >= 0);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgez_s16 (int16x4_t __a)
{
  return (uint16x4_t) (__a >= 0);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgez_s32 (int32x2_t __a)
{
  return (uint32x2_t) (__a >= 0);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgez_s64 (int64x1_t __a)
{
  return (uint64x1_t) (__a >= __AARCH64_INT64_C (0));
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgezq_f32 (float32x4_t __a)
{
  return (uint32x4_t) (__a >= 0.0f);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgezq_f64 (float64x2_t __a)
{
  return (uint64x2_t) (__a >= 0.0);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgezq_s8 (int8x16_t __a)
{
  return (uint8x16_t) (__a >= 0);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgezq_s16 (int16x8_t __a)
{
  return (uint16x8_t) (__a >= 0);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgezq_s32 (int32x4_t __a)
{
  return (uint32x4_t) (__a >= 0);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgezq_s64 (int64x2_t __a)
{
  return (uint64x2_t) (__a >= __AARCH64_INT64_C (0));
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgezs_f32 (float32_t __a)
{
  return __a >= 0.0f ? -1 : 0;
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgezd_s64 (int64_t __a)
{
  return __a >= 0 ? -1ll : 0ll;
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgezd_f64 (float64_t __a)
{
  return __a >= 0.0 ? -1ll : 0ll;
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgt_f32 (float32x2_t __a, float32x2_t __b)
{
  return (uint32x2_t) (__a > __b);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgt_f64 (float64x1_t __a, float64x1_t __b)
{
  return (uint64x1_t) (__a > __b);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgt_s8 (int8x8_t __a, int8x8_t __b)
{
  return (uint8x8_t) (__a > __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgt_s16 (int16x4_t __a, int16x4_t __b)
{
  return (uint16x4_t) (__a > __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgt_s32 (int32x2_t __a, int32x2_t __b)
{
  return (uint32x2_t) (__a > __b);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgt_s64 (int64x1_t __a, int64x1_t __b)
{
  return (uint64x1_t) (__a > __b);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgt_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (__a > __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgt_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (__a > __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgt_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (__a > __b);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgt_u64 (uint64x1_t __a, uint64x1_t __b)
{
  return (__a > __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgtq_f32 (float32x4_t __a, float32x4_t __b)
{
  return (uint32x4_t) (__a > __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgtq_f64 (float64x2_t __a, float64x2_t __b)
{
  return (uint64x2_t) (__a > __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgtq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (uint8x16_t) (__a > __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgtq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (uint16x8_t) (__a > __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgtq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (uint32x4_t) (__a > __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgtq_s64 (int64x2_t __a, int64x2_t __b)
{
  return (uint64x2_t) (__a > __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgtq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return (__a > __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgtq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (__a > __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgtq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (__a > __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgtq_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return (__a > __b);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgts_f32 (float32_t __a, float32_t __b)
{
  return __a > __b ? -1 : 0;
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgtd_s64 (int64_t __a, int64_t __b)
{
  return __a > __b ? -1ll : 0ll;
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgtd_u64 (uint64_t __a, uint64_t __b)
{
  return __a > __b ? -1ll : 0ll;
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgtd_f64 (float64_t __a, float64_t __b)
{
  return __a > __b ? -1ll : 0ll;
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgtz_f32 (float32x2_t __a)
{
  return (uint32x2_t) (__a > 0.0f);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgtz_f64 (float64x1_t __a)
{
  return (uint64x1_t) (__a > (float64x1_t) {0.0});
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgtz_s8 (int8x8_t __a)
{
  return (uint8x8_t) (__a > 0);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgtz_s16 (int16x4_t __a)
{
  return (uint16x4_t) (__a > 0);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgtz_s32 (int32x2_t __a)
{
  return (uint32x2_t) (__a > 0);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgtz_s64 (int64x1_t __a)
{
  return (uint64x1_t) (__a > __AARCH64_INT64_C (0));
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgtzq_f32 (float32x4_t __a)
{
  return (uint32x4_t) (__a > 0.0f);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgtzq_f64 (float64x2_t __a)
{
    return (uint64x2_t) (__a > 0.0);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgtzq_s8 (int8x16_t __a)
{
  return (uint8x16_t) (__a > 0);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgtzq_s16 (int16x8_t __a)
{
  return (uint16x8_t) (__a > 0);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgtzq_s32 (int32x4_t __a)
{
  return (uint32x4_t) (__a > 0);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgtzq_s64 (int64x2_t __a)
{
  return (uint64x2_t) (__a > __AARCH64_INT64_C (0));
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgtzs_f32 (float32_t __a)
{
  return __a > 0.0f ? -1 : 0;
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgtzd_s64 (int64_t __a)
{
  return __a > 0 ? -1ll : 0ll;
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgtzd_f64 (float64_t __a)
{
  return __a > 0.0 ? -1ll : 0ll;
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcle_f32 (float32x2_t __a, float32x2_t __b)
{
  return (uint32x2_t) (__a <= __b);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcle_f64 (float64x1_t __a, float64x1_t __b)
{
  return (uint64x1_t) (__a <= __b);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcle_s8 (int8x8_t __a, int8x8_t __b)
{
  return (uint8x8_t) (__a <= __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcle_s16 (int16x4_t __a, int16x4_t __b)
{
  return (uint16x4_t) (__a <= __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcle_s32 (int32x2_t __a, int32x2_t __b)
{
  return (uint32x2_t) (__a <= __b);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcle_s64 (int64x1_t __a, int64x1_t __b)
{
  return (uint64x1_t) (__a <= __b);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcle_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (__a <= __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcle_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (__a <= __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcle_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (__a <= __b);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcle_u64 (uint64x1_t __a, uint64x1_t __b)
{
  return (__a <= __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcleq_f32 (float32x4_t __a, float32x4_t __b)
{
  return (uint32x4_t) (__a <= __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcleq_f64 (float64x2_t __a, float64x2_t __b)
{
  return (uint64x2_t) (__a <= __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcleq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (uint8x16_t) (__a <= __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcleq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (uint16x8_t) (__a <= __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcleq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (uint32x4_t) (__a <= __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcleq_s64 (int64x2_t __a, int64x2_t __b)
{
  return (uint64x2_t) (__a <= __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcleq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return (__a <= __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcleq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (__a <= __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcleq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (__a <= __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcleq_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return (__a <= __b);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcles_f32 (float32_t __a, float32_t __b)
{
  return __a <= __b ? -1 : 0;
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcled_s64 (int64_t __a, int64_t __b)
{
  return __a <= __b ? -1ll : 0ll;
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcled_u64 (uint64_t __a, uint64_t __b)
{
  return __a <= __b ? -1ll : 0ll;
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcled_f64 (float64_t __a, float64_t __b)
{
  return __a <= __b ? -1ll : 0ll;
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclez_f32 (float32x2_t __a)
{
  return (uint32x2_t) (__a <= 0.0f);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclez_f64 (float64x1_t __a)
{
  return (uint64x1_t) (__a <= (float64x1_t) {0.0});
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclez_s8 (int8x8_t __a)
{
  return (uint8x8_t) (__a <= 0);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclez_s16 (int16x4_t __a)
{
  return (uint16x4_t) (__a <= 0);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclez_s32 (int32x2_t __a)
{
  return (uint32x2_t) (__a <= 0);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclez_s64 (int64x1_t __a)
{
  return (uint64x1_t) (__a <= __AARCH64_INT64_C (0));
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclezq_f32 (float32x4_t __a)
{
  return (uint32x4_t) (__a <= 0.0f);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclezq_f64 (float64x2_t __a)
{
  return (uint64x2_t) (__a <= 0.0);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclezq_s8 (int8x16_t __a)
{
  return (uint8x16_t) (__a <= 0);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclezq_s16 (int16x8_t __a)
{
  return (uint16x8_t) (__a <= 0);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclezq_s32 (int32x4_t __a)
{
  return (uint32x4_t) (__a <= 0);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclezq_s64 (int64x2_t __a)
{
  return (uint64x2_t) (__a <= __AARCH64_INT64_C (0));
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclezs_f32 (float32_t __a)
{
  return __a <= 0.0f ? -1 : 0;
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclezd_s64 (int64_t __a)
{
  return __a <= 0 ? -1ll : 0ll;
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclezd_f64 (float64_t __a)
{
  return __a <= 0.0 ? -1ll : 0ll;
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclt_f32 (float32x2_t __a, float32x2_t __b)
{
  return (uint32x2_t) (__a < __b);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclt_f64 (float64x1_t __a, float64x1_t __b)
{
  return (uint64x1_t) (__a < __b);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclt_s8 (int8x8_t __a, int8x8_t __b)
{
  return (uint8x8_t) (__a < __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclt_s16 (int16x4_t __a, int16x4_t __b)
{
  return (uint16x4_t) (__a < __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclt_s32 (int32x2_t __a, int32x2_t __b)
{
  return (uint32x2_t) (__a < __b);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclt_s64 (int64x1_t __a, int64x1_t __b)
{
  return (uint64x1_t) (__a < __b);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclt_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (__a < __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclt_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (__a < __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclt_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (__a < __b);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclt_u64 (uint64x1_t __a, uint64x1_t __b)
{
  return (__a < __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcltq_f32 (float32x4_t __a, float32x4_t __b)
{
  return (uint32x4_t) (__a < __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcltq_f64 (float64x2_t __a, float64x2_t __b)
{
  return (uint64x2_t) (__a < __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcltq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (uint8x16_t) (__a < __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcltq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (uint16x8_t) (__a < __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcltq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (uint32x4_t) (__a < __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcltq_s64 (int64x2_t __a, int64x2_t __b)
{
  return (uint64x2_t) (__a < __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcltq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return (__a < __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcltq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (__a < __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcltq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (__a < __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcltq_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return (__a < __b);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclts_f32 (float32_t __a, float32_t __b)
{
  return __a < __b ? -1 : 0;
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcltd_s64 (int64_t __a, int64_t __b)
{
  return __a < __b ? -1ll : 0ll;
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcltd_u64 (uint64_t __a, uint64_t __b)
{
  return __a < __b ? -1ll : 0ll;
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcltd_f64 (float64_t __a, float64_t __b)
{
  return __a < __b ? -1ll : 0ll;
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcltz_f32 (float32x2_t __a)
{
  return (uint32x2_t) (__a < 0.0f);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcltz_f64 (float64x1_t __a)
{
  return (uint64x1_t) (__a < (float64x1_t) {0.0});
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcltz_s8 (int8x8_t __a)
{
  return (uint8x8_t) (__a < 0);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcltz_s16 (int16x4_t __a)
{
  return (uint16x4_t) (__a < 0);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcltz_s32 (int32x2_t __a)
{
  return (uint32x2_t) (__a < 0);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcltz_s64 (int64x1_t __a)
{
  return (uint64x1_t) (__a < __AARCH64_INT64_C (0));
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcltzq_f32 (float32x4_t __a)
{
  return (uint32x4_t) (__a < 0.0f);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcltzq_f64 (float64x2_t __a)
{
  return (uint64x2_t) (__a < 0.0);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcltzq_s8 (int8x16_t __a)
{
  return (uint8x16_t) (__a < 0);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcltzq_s16 (int16x8_t __a)
{
  return (uint16x8_t) (__a < 0);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcltzq_s32 (int32x4_t __a)
{
  return (uint32x4_t) (__a < 0);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcltzq_s64 (int64x2_t __a)
{
  return (uint64x2_t) (__a < __AARCH64_INT64_C (0));
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcltzs_f32 (float32_t __a)
{
  return __a < 0.0f ? -1 : 0;
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcltzd_s64 (int64_t __a)
{
  return __a < 0 ? -1ll : 0ll;
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcltzd_f64 (float64_t __a)
{
  return __a < 0.0 ? -1ll : 0ll;
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcls_s8 (int8x8_t __a)
{
  return __builtin_aarch64_clrsbv8qi (__a);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcls_s16 (int16x4_t __a)
{
  return __builtin_aarch64_clrsbv4hi (__a);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcls_s32 (int32x2_t __a)
{
  return __builtin_aarch64_clrsbv2si (__a);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclsq_s8 (int8x16_t __a)
{
  return __builtin_aarch64_clrsbv16qi (__a);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclsq_s16 (int16x8_t __a)
{
  return __builtin_aarch64_clrsbv8hi (__a);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclsq_s32 (int32x4_t __a)
{
  return __builtin_aarch64_clrsbv4si (__a);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcls_u8 (uint8x8_t __a)
{
  return __builtin_aarch64_clrsbv8qi ((int8x8_t) __a);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcls_u16 (uint16x4_t __a)
{
  return __builtin_aarch64_clrsbv4hi ((int16x4_t) __a);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcls_u32 (uint32x2_t __a)
{
  return __builtin_aarch64_clrsbv2si ((int32x2_t) __a);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclsq_u8 (uint8x16_t __a)
{
  return __builtin_aarch64_clrsbv16qi ((int8x16_t) __a);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclsq_u16 (uint16x8_t __a)
{
  return __builtin_aarch64_clrsbv8hi ((int16x8_t) __a);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclsq_u32 (uint32x4_t __a)
{
  return __builtin_aarch64_clrsbv4si ((int32x4_t) __a);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclz_s8 (int8x8_t __a)
{
  return __builtin_aarch64_clzv8qi (__a);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclz_s16 (int16x4_t __a)
{
  return __builtin_aarch64_clzv4hi (__a);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclz_s32 (int32x2_t __a)
{
  return __builtin_aarch64_clzv2si (__a);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclz_u8 (uint8x8_t __a)
{
  return (uint8x8_t)__builtin_aarch64_clzv8qi ((int8x8_t)__a);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclz_u16 (uint16x4_t __a)
{
  return (uint16x4_t)__builtin_aarch64_clzv4hi ((int16x4_t)__a);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclz_u32 (uint32x2_t __a)
{
  return (uint32x2_t)__builtin_aarch64_clzv2si ((int32x2_t)__a);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclzq_s8 (int8x16_t __a)
{
  return __builtin_aarch64_clzv16qi (__a);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclzq_s16 (int16x8_t __a)
{
  return __builtin_aarch64_clzv8hi (__a);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclzq_s32 (int32x4_t __a)
{
  return __builtin_aarch64_clzv4si (__a);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclzq_u8 (uint8x16_t __a)
{
  return (uint8x16_t)__builtin_aarch64_clzv16qi ((int8x16_t)__a);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclzq_u16 (uint16x8_t __a)
{
  return (uint16x8_t)__builtin_aarch64_clzv8hi ((int16x8_t)__a);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclzq_u32 (uint32x4_t __a)
{
  return (uint32x4_t)__builtin_aarch64_clzv4si ((int32x4_t)__a);
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcnt_p8 (poly8x8_t __a)
{
  return (poly8x8_t) __builtin_aarch64_popcountv8qi ((int8x8_t) __a);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcnt_s8 (int8x8_t __a)
{
  return __builtin_aarch64_popcountv8qi (__a);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcnt_u8 (uint8x8_t __a)
{
  return (uint8x8_t) __builtin_aarch64_popcountv8qi ((int8x8_t) __a);
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcntq_p8 (poly8x16_t __a)
{
  return (poly8x16_t) __builtin_aarch64_popcountv16qi ((int8x16_t) __a);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcntq_s8 (int8x16_t __a)
{
  return __builtin_aarch64_popcountv16qi (__a);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcntq_u8 (uint8x16_t __a)
{
  return (uint8x16_t) __builtin_aarch64_popcountv16qi ((int8x16_t) __a);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopy_lane_f32 (float32x2_t __a, const int __lane1,
  float32x2_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
      __a, __lane1);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopy_lane_f64 (float64x1_t __a, const int __lane1,
  float64x1_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
      __a, __lane1);
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopy_lane_p8 (poly8x8_t __a, const int __lane1,
        poly8x8_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
     __a, __lane1);
}
__extension__ extern __inline poly16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopy_lane_p16 (poly16x4_t __a, const int __lane1,
  poly16x4_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
      __a, __lane1);
}
__extension__ extern __inline poly64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopy_lane_p64 (poly64x1_t __a, const int __lane1,
  poly64x1_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
      __a, __lane1);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopy_lane_s8 (int8x8_t __a, const int __lane1,
        int8x8_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
     __a, __lane1);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopy_lane_s16 (int16x4_t __a, const int __lane1,
  int16x4_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
      __a, __lane1);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopy_lane_s32 (int32x2_t __a, const int __lane1,
  int32x2_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
      __a, __lane1);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopy_lane_s64 (int64x1_t __a, const int __lane1,
  int64x1_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
      __a, __lane1);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopy_lane_u8 (uint8x8_t __a, const int __lane1,
        uint8x8_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
     __a, __lane1);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopy_lane_u16 (uint16x4_t __a, const int __lane1,
  uint16x4_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
      __a, __lane1);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopy_lane_u32 (uint32x2_t __a, const int __lane1,
  uint32x2_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
      __a, __lane1);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopy_lane_u64 (uint64x1_t __a, const int __lane1,
  uint64x1_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
      __a, __lane1);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopy_laneq_f32 (float32x2_t __a, const int __lane1,
   float32x4_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
      __a, __lane1);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopy_laneq_f64 (float64x1_t __a, const int __lane1,
   float64x2_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
      __a, __lane1);
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopy_laneq_p8 (poly8x8_t __a, const int __lane1,
  poly8x16_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
     __a, __lane1);
}
__extension__ extern __inline poly16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopy_laneq_p16 (poly16x4_t __a, const int __lane1,
   poly16x8_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
      __a, __lane1);
}
__extension__ extern __inline poly64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopy_laneq_p64 (poly64x1_t __a, const int __lane1,
   poly64x2_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
      __a, __lane1);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopy_laneq_s8 (int8x8_t __a, const int __lane1,
  int8x16_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
     __a, __lane1);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopy_laneq_s16 (int16x4_t __a, const int __lane1,
   int16x8_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
      __a, __lane1);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopy_laneq_s32 (int32x2_t __a, const int __lane1,
   int32x4_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
      __a, __lane1);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopy_laneq_s64 (int64x1_t __a, const int __lane1,
   int64x2_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
      __a, __lane1);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopy_laneq_u8 (uint8x8_t __a, const int __lane1,
  uint8x16_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
     __a, __lane1);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopy_laneq_u16 (uint16x4_t __a, const int __lane1,
   uint16x8_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
      __a, __lane1);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopy_laneq_u32 (uint32x2_t __a, const int __lane1,
   uint32x4_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
      __a, __lane1);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopy_laneq_u64 (uint64x1_t __a, const int __lane1,
   uint64x2_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
      __a, __lane1);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopyq_lane_f32 (float32x4_t __a, const int __lane1,
   float32x2_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
       __a, __lane1);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopyq_lane_f64 (float64x2_t __a, const int __lane1,
   float64x1_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
       __a, __lane1);
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopyq_lane_p8 (poly8x16_t __a, const int __lane1,
  poly8x8_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
      __a, __lane1);
}
__extension__ extern __inline poly16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopyq_lane_p16 (poly16x8_t __a, const int __lane1,
   poly16x4_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
       __a, __lane1);
}
__extension__ extern __inline poly64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopyq_lane_p64 (poly64x2_t __a, const int __lane1,
   poly64x1_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
       __a, __lane1);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopyq_lane_s8 (int8x16_t __a, const int __lane1,
  int8x8_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
      __a, __lane1);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopyq_lane_s16 (int16x8_t __a, const int __lane1,
   int16x4_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
       __a, __lane1);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopyq_lane_s32 (int32x4_t __a, const int __lane1,
   int32x2_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
       __a, __lane1);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopyq_lane_s64 (int64x2_t __a, const int __lane1,
   int64x1_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
       __a, __lane1);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopyq_lane_u8 (uint8x16_t __a, const int __lane1,
  uint8x8_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
      __a, __lane1);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopyq_lane_u16 (uint16x8_t __a, const int __lane1,
   uint16x4_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
       __a, __lane1);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopyq_lane_u32 (uint32x4_t __a, const int __lane1,
   uint32x2_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
       __a, __lane1);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopyq_lane_u64 (uint64x2_t __a, const int __lane1,
   uint64x1_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
       __a, __lane1);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopyq_laneq_f32 (float32x4_t __a, const int __lane1,
    float32x4_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
       __a, __lane1);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopyq_laneq_f64 (float64x2_t __a, const int __lane1,
    float64x2_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
       __a, __lane1);
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopyq_laneq_p8 (poly8x16_t __a, const int __lane1,
   poly8x16_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
      __a, __lane1);
}
__extension__ extern __inline poly16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopyq_laneq_p16 (poly16x8_t __a, const int __lane1,
    poly16x8_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
       __a, __lane1);
}
__extension__ extern __inline poly64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopyq_laneq_p64 (poly64x2_t __a, const int __lane1,
    poly64x2_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
       __a, __lane1);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopyq_laneq_s8 (int8x16_t __a, const int __lane1,
   int8x16_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
      __a, __lane1);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopyq_laneq_s16 (int16x8_t __a, const int __lane1,
    int16x8_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
       __a, __lane1);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopyq_laneq_s32 (int32x4_t __a, const int __lane1,
    int32x4_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
       __a, __lane1);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopyq_laneq_s64 (int64x2_t __a, const int __lane1,
    int64x2_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
       __a, __lane1);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopyq_laneq_u8 (uint8x16_t __a, const int __lane1,
   uint8x16_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
      __a, __lane1);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopyq_laneq_u16 (uint16x8_t __a, const int __lane1,
    uint16x8_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
       __a, __lane1);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopyq_laneq_u32 (uint32x4_t __a, const int __lane1,
    uint32x4_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
       __a, __lane1);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopyq_laneq_u64 (uint64x2_t __a, const int __lane1,
    uint64x2_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
       __a, __lane1);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvt_f16_f32 (float32x4_t __a)
{
  return __builtin_aarch64_float_truncate_lo_v4hf (__a);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvt_high_f16_f32 (float16x4_t __a, float32x4_t __b)
{
  return __builtin_aarch64_float_truncate_hi_v8hf (__a, __b);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvt_f32_f64 (float64x2_t __a)
{
  return __builtin_aarch64_float_truncate_lo_v2sf (__a);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvt_high_f32_f64 (float32x2_t __a, float64x2_t __b)
{
  return __builtin_aarch64_float_truncate_hi_v4sf (__a, __b);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvt_f32_f16 (float16x4_t __a)
{
  return __builtin_aarch64_float_extend_lo_v4sf (__a);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvt_f64_f32 (float32x2_t __a)
{
  return __builtin_aarch64_float_extend_lo_v2df (__a);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvt_high_f32_f16 (float16x8_t __a)
{
  return __builtin_aarch64_vec_unpacks_hi_v8hf (__a);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvt_high_f64_f32 (float32x4_t __a)
{
  return __builtin_aarch64_vec_unpacks_hi_v4sf (__a);
}
__extension__ extern __inline float64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtd_n_f64_s64 (int64_t __a, const int __b)
{
  return __builtin_aarch64_scvtfdi (__a, __b);
}
__extension__ extern __inline float64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtd_n_f64_u64 (uint64_t __a, const int __b)
{
  return __builtin_aarch64_ucvtfdi_sus (__a, __b);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvts_n_f32_s32 (int32_t __a, const int __b)
{
  return __builtin_aarch64_scvtfsi (__a, __b);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvts_n_f32_u32 (uint32_t __a, const int __b)
{
  return __builtin_aarch64_ucvtfsi_sus (__a, __b);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvt_n_f32_s32 (int32x2_t __a, const int __b)
{
  return __builtin_aarch64_scvtfv2si (__a, __b);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvt_n_f32_u32 (uint32x2_t __a, const int __b)
{
  return __builtin_aarch64_ucvtfv2si_sus (__a, __b);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvt_n_f64_s64 (int64x1_t __a, const int __b)
{
  return (float64x1_t)
    { __builtin_aarch64_scvtfdi (vget_lane_s64 (__a, 0), __b) };
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvt_n_f64_u64 (uint64x1_t __a, const int __b)
{
  return (float64x1_t)
    { __builtin_aarch64_ucvtfdi_sus (vget_lane_u64 (__a, 0), __b) };
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtq_n_f32_s32 (int32x4_t __a, const int __b)
{
  return __builtin_aarch64_scvtfv4si (__a, __b);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtq_n_f32_u32 (uint32x4_t __a, const int __b)
{
  return __builtin_aarch64_ucvtfv4si_sus (__a, __b);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtq_n_f64_s64 (int64x2_t __a, const int __b)
{
  return __builtin_aarch64_scvtfv2di (__a, __b);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtq_n_f64_u64 (uint64x2_t __a, const int __b)
{
  return __builtin_aarch64_ucvtfv2di_sus (__a, __b);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtd_n_s64_f64 (float64_t __a, const int __b)
{
  return __builtin_aarch64_fcvtzsdf (__a, __b);
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtd_n_u64_f64 (float64_t __a, const int __b)
{
  return __builtin_aarch64_fcvtzudf_uss (__a, __b);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvts_n_s32_f32 (float32_t __a, const int __b)
{
  return __builtin_aarch64_fcvtzssf (__a, __b);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvts_n_u32_f32 (float32_t __a, const int __b)
{
  return __builtin_aarch64_fcvtzusf_uss (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvt_n_s32_f32 (float32x2_t __a, const int __b)
{
  return __builtin_aarch64_fcvtzsv2sf (__a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvt_n_u32_f32 (float32x2_t __a, const int __b)
{
  return __builtin_aarch64_fcvtzuv2sf_uss (__a, __b);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvt_n_s64_f64 (float64x1_t __a, const int __b)
{
  return (int64x1_t)
    { __builtin_aarch64_fcvtzsdf (vget_lane_f64 (__a, 0), __b) };
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvt_n_u64_f64 (float64x1_t __a, const int __b)
{
  return (uint64x1_t)
    { __builtin_aarch64_fcvtzudf_uss (vget_lane_f64 (__a, 0), __b) };
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtq_n_s32_f32 (float32x4_t __a, const int __b)
{
  return __builtin_aarch64_fcvtzsv4sf (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtq_n_u32_f32 (float32x4_t __a, const int __b)
{
  return __builtin_aarch64_fcvtzuv4sf_uss (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtq_n_s64_f64 (float64x2_t __a, const int __b)
{
  return __builtin_aarch64_fcvtzsv2df (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtq_n_u64_f64 (float64x2_t __a, const int __b)
{
  return __builtin_aarch64_fcvtzuv2df_uss (__a, __b);
}
__extension__ extern __inline float64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtd_f64_s64 (int64_t __a)
{
  return (float64_t) __a;
}
__extension__ extern __inline float64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtd_f64_u64 (uint64_t __a)
{
  return (float64_t) __a;
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvts_f32_s32 (int32_t __a)
{
  return (float32_t) __a;
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvts_f32_u32 (uint32_t __a)
{
  return (float32_t) __a;
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvt_f32_s32 (int32x2_t __a)
{
  return __builtin_aarch64_floatv2siv2sf (__a);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvt_f32_u32 (uint32x2_t __a)
{
  return __builtin_aarch64_floatunsv2siv2sf ((int32x2_t) __a);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvt_f64_s64 (int64x1_t __a)
{
  return (float64x1_t) { vget_lane_s64 (__a, 0) };
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvt_f64_u64 (uint64x1_t __a)
{
  return (float64x1_t) { vget_lane_u64 (__a, 0) };
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtq_f32_s32 (int32x4_t __a)
{
  return __builtin_aarch64_floatv4siv4sf (__a);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtq_f32_u32 (uint32x4_t __a)
{
  return __builtin_aarch64_floatunsv4siv4sf ((int32x4_t) __a);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtq_f64_s64 (int64x2_t __a)
{
  return __builtin_aarch64_floatv2div2df (__a);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtq_f64_u64 (uint64x2_t __a)
{
  return __builtin_aarch64_floatunsv2div2df ((int64x2_t) __a);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtd_s64_f64 (float64_t __a)
{
  return (int64_t) __a;
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtd_u64_f64 (float64_t __a)
{
  return (uint64_t) __a;
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvts_s32_f32 (float32_t __a)
{
  return (int32_t) __a;
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvts_u32_f32 (float32_t __a)
{
  return (uint32_t) __a;
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvt_s32_f32 (float32x2_t __a)
{
  return __builtin_aarch64_lbtruncv2sfv2si (__a);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvt_u32_f32 (float32x2_t __a)
{
  return __builtin_aarch64_lbtruncuv2sfv2si_us (__a);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtq_s32_f32 (float32x4_t __a)
{
  return __builtin_aarch64_lbtruncv4sfv4si (__a);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtq_u32_f32 (float32x4_t __a)
{
  return __builtin_aarch64_lbtruncuv4sfv4si_us (__a);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvt_s64_f64 (float64x1_t __a)
{
  return (int64x1_t) {vcvtd_s64_f64 (__a[0])};
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvt_u64_f64 (float64x1_t __a)
{
  return (uint64x1_t) {vcvtd_u64_f64 (__a[0])};
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtq_s64_f64 (float64x2_t __a)
{
  return __builtin_aarch64_lbtruncv2dfv2di (__a);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtq_u64_f64 (float64x2_t __a)
{
  return __builtin_aarch64_lbtruncuv2dfv2di_us (__a);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtad_s64_f64 (float64_t __a)
{
  return __builtin_aarch64_lrounddfdi (__a);
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtad_u64_f64 (float64_t __a)
{
  return __builtin_aarch64_lroundudfdi_us (__a);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtas_s32_f32 (float32_t __a)
{
  return __builtin_aarch64_lroundsfsi (__a);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtas_u32_f32 (float32_t __a)
{
  return __builtin_aarch64_lroundusfsi_us (__a);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvta_s32_f32 (float32x2_t __a)
{
  return __builtin_aarch64_lroundv2sfv2si (__a);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvta_u32_f32 (float32x2_t __a)
{
  return __builtin_aarch64_lrounduv2sfv2si_us (__a);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtaq_s32_f32 (float32x4_t __a)
{
  return __builtin_aarch64_lroundv4sfv4si (__a);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtaq_u32_f32 (float32x4_t __a)
{
  return __builtin_aarch64_lrounduv4sfv4si_us (__a);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvta_s64_f64 (float64x1_t __a)
{
  return (int64x1_t) {vcvtad_s64_f64 (__a[0])};
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvta_u64_f64 (float64x1_t __a)
{
  return (uint64x1_t) {vcvtad_u64_f64 (__a[0])};
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtaq_s64_f64 (float64x2_t __a)
{
  return __builtin_aarch64_lroundv2dfv2di (__a);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtaq_u64_f64 (float64x2_t __a)
{
  return __builtin_aarch64_lrounduv2dfv2di_us (__a);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtmd_s64_f64 (float64_t __a)
{
  return __builtin_llfloor (__a);
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtmd_u64_f64 (float64_t __a)
{
  return __builtin_aarch64_lfloorudfdi_us (__a);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtms_s32_f32 (float32_t __a)
{
  return __builtin_ifloorf (__a);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtms_u32_f32 (float32_t __a)
{
  return __builtin_aarch64_lfloorusfsi_us (__a);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtm_s32_f32 (float32x2_t __a)
{
  return __builtin_aarch64_lfloorv2sfv2si (__a);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtm_u32_f32 (float32x2_t __a)
{
  return __builtin_aarch64_lflooruv2sfv2si_us (__a);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtmq_s32_f32 (float32x4_t __a)
{
  return __builtin_aarch64_lfloorv4sfv4si (__a);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtmq_u32_f32 (float32x4_t __a)
{
  return __builtin_aarch64_lflooruv4sfv4si_us (__a);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtm_s64_f64 (float64x1_t __a)
{
  return (int64x1_t) {vcvtmd_s64_f64 (__a[0])};
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtm_u64_f64 (float64x1_t __a)
{
  return (uint64x1_t) {vcvtmd_u64_f64 (__a[0])};
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtmq_s64_f64 (float64x2_t __a)
{
  return __builtin_aarch64_lfloorv2dfv2di (__a);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtmq_u64_f64 (float64x2_t __a)
{
  return __builtin_aarch64_lflooruv2dfv2di_us (__a);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtnd_s64_f64 (float64_t __a)
{
  return __builtin_aarch64_lfrintndfdi (__a);
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtnd_u64_f64 (float64_t __a)
{
  return __builtin_aarch64_lfrintnudfdi_us (__a);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtns_s32_f32 (float32_t __a)
{
  return __builtin_aarch64_lfrintnsfsi (__a);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtns_u32_f32 (float32_t __a)
{
  return __builtin_aarch64_lfrintnusfsi_us (__a);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtn_s32_f32 (float32x2_t __a)
{
  return __builtin_aarch64_lfrintnv2sfv2si (__a);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtn_u32_f32 (float32x2_t __a)
{
  return __builtin_aarch64_lfrintnuv2sfv2si_us (__a);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtnq_s32_f32 (float32x4_t __a)
{
  return __builtin_aarch64_lfrintnv4sfv4si (__a);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtnq_u32_f32 (float32x4_t __a)
{
  return __builtin_aarch64_lfrintnuv4sfv4si_us (__a);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtn_s64_f64 (float64x1_t __a)
{
  return (int64x1_t) {vcvtnd_s64_f64 (__a[0])};
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtn_u64_f64 (float64x1_t __a)
{
  return (uint64x1_t) {vcvtnd_u64_f64 (__a[0])};
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtnq_s64_f64 (float64x2_t __a)
{
  return __builtin_aarch64_lfrintnv2dfv2di (__a);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtnq_u64_f64 (float64x2_t __a)
{
  return __builtin_aarch64_lfrintnuv2dfv2di_us (__a);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtpd_s64_f64 (float64_t __a)
{
  return __builtin_llceil (__a);
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtpd_u64_f64 (float64_t __a)
{
  return __builtin_aarch64_lceiludfdi_us (__a);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtps_s32_f32 (float32_t __a)
{
  return __builtin_iceilf (__a);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtps_u32_f32 (float32_t __a)
{
  return __builtin_aarch64_lceilusfsi_us (__a);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtp_s32_f32 (float32x2_t __a)
{
  return __builtin_aarch64_lceilv2sfv2si (__a);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtp_u32_f32 (float32x2_t __a)
{
  return __builtin_aarch64_lceiluv2sfv2si_us (__a);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtpq_s32_f32 (float32x4_t __a)
{
  return __builtin_aarch64_lceilv4sfv4si (__a);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtpq_u32_f32 (float32x4_t __a)
{
  return __builtin_aarch64_lceiluv4sfv4si_us (__a);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtp_s64_f64 (float64x1_t __a)
{
  return (int64x1_t) {vcvtpd_s64_f64 (__a[0])};
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtp_u64_f64 (float64x1_t __a)
{
  return (uint64x1_t) {vcvtpd_u64_f64 (__a[0])};
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtpq_s64_f64 (float64x2_t __a)
{
  return __builtin_aarch64_lceilv2dfv2di (__a);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtpq_u64_f64 (float64x2_t __a)
{
  return __builtin_aarch64_lceiluv2dfv2di_us (__a);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_n_f16 (float16_t __a)
{
  return (float16x4_t) {__a, __a, __a, __a};
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_n_f32 (float32_t __a)
{
  return (float32x2_t) {__a, __a};
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_n_f64 (float64_t __a)
{
  return (float64x1_t) {__a};
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_n_p8 (poly8_t __a)
{
  return (poly8x8_t) {__a, __a, __a, __a, __a, __a, __a, __a};
}
__extension__ extern __inline poly16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_n_p16 (poly16_t __a)
{
  return (poly16x4_t) {__a, __a, __a, __a};
}
__extension__ extern __inline poly64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_n_p64 (poly64_t __a)
{
  return (poly64x1_t) {__a};
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_n_s8 (int8_t __a)
{
  return (int8x8_t) {__a, __a, __a, __a, __a, __a, __a, __a};
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_n_s16 (int16_t __a)
{
  return (int16x4_t) {__a, __a, __a, __a};
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_n_s32 (int32_t __a)
{
  return (int32x2_t) {__a, __a};
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_n_s64 (int64_t __a)
{
  return (int64x1_t) {__a};
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_n_u8 (uint8_t __a)
{
  return (uint8x8_t) {__a, __a, __a, __a, __a, __a, __a, __a};
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_n_u16 (uint16_t __a)
{
  return (uint16x4_t) {__a, __a, __a, __a};
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_n_u32 (uint32_t __a)
{
  return (uint32x2_t) {__a, __a};
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_n_u64 (uint64_t __a)
{
  return (uint64x1_t) {__a};
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_n_f16 (float16_t __a)
{
  return (float16x8_t) {__a, __a, __a, __a, __a, __a, __a, __a};
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_n_f32 (float32_t __a)
{
  return (float32x4_t) {__a, __a, __a, __a};
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_n_f64 (float64_t __a)
{
  return (float64x2_t) {__a, __a};
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_n_p8 (poly8_t __a)
{
  return (poly8x16_t) {__a, __a, __a, __a, __a, __a, __a, __a,
         __a, __a, __a, __a, __a, __a, __a, __a};
}
__extension__ extern __inline poly16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_n_p16 (poly16_t __a)
{
  return (poly16x8_t) {__a, __a, __a, __a, __a, __a, __a, __a};
}
__extension__ extern __inline poly64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_n_p64 (poly64_t __a)
{
  return (poly64x2_t) {__a, __a};
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_n_s8 (int8_t __a)
{
  return (int8x16_t) {__a, __a, __a, __a, __a, __a, __a, __a,
        __a, __a, __a, __a, __a, __a, __a, __a};
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_n_s16 (int16_t __a)
{
  return (int16x8_t) {__a, __a, __a, __a, __a, __a, __a, __a};
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_n_s32 (int32_t __a)
{
  return (int32x4_t) {__a, __a, __a, __a};
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_n_s64 (int64_t __a)
{
  return (int64x2_t) {__a, __a};
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_n_u8 (uint8_t __a)
{
  return (uint8x16_t) {__a, __a, __a, __a, __a, __a, __a, __a,
         __a, __a, __a, __a, __a, __a, __a, __a};
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_n_u16 (uint16_t __a)
{
  return (uint16x8_t) {__a, __a, __a, __a, __a, __a, __a, __a};
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_n_u32 (uint32_t __a)
{
  return (uint32x4_t) {__a, __a, __a, __a};
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_n_u64 (uint64_t __a)
{
  return (uint64x2_t) {__a, __a};
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_lane_f16 (float16x4_t __a, const int __b)
{
  return __aarch64_vdup_lane_f16 (__a, __b);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_lane_f32 (float32x2_t __a, const int __b)
{
  return __aarch64_vdup_lane_f32 (__a, __b);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_lane_f64 (float64x1_t __a, const int __b)
{
  return __aarch64_vdup_lane_f64 (__a, __b);
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_lane_p8 (poly8x8_t __a, const int __b)
{
  return __aarch64_vdup_lane_p8 (__a, __b);
}
__extension__ extern __inline poly16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_lane_p16 (poly16x4_t __a, const int __b)
{
  return __aarch64_vdup_lane_p16 (__a, __b);
}
__extension__ extern __inline poly64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_lane_p64 (poly64x1_t __a, const int __b)
{
  return __aarch64_vdup_lane_p64 (__a, __b);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_lane_s8 (int8x8_t __a, const int __b)
{
  return __aarch64_vdup_lane_s8 (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_lane_s16 (int16x4_t __a, const int __b)
{
  return __aarch64_vdup_lane_s16 (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_lane_s32 (int32x2_t __a, const int __b)
{
  return __aarch64_vdup_lane_s32 (__a, __b);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_lane_s64 (int64x1_t __a, const int __b)
{
  return __aarch64_vdup_lane_s64 (__a, __b);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_lane_u8 (uint8x8_t __a, const int __b)
{
  return __aarch64_vdup_lane_u8 (__a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_lane_u16 (uint16x4_t __a, const int __b)
{
  return __aarch64_vdup_lane_u16 (__a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_lane_u32 (uint32x2_t __a, const int __b)
{
  return __aarch64_vdup_lane_u32 (__a, __b);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_lane_u64 (uint64x1_t __a, const int __b)
{
  return __aarch64_vdup_lane_u64 (__a, __b);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_laneq_f16 (float16x8_t __a, const int __b)
{
  return __aarch64_vdup_laneq_f16 (__a, __b);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_laneq_f32 (float32x4_t __a, const int __b)
{
  return __aarch64_vdup_laneq_f32 (__a, __b);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_laneq_f64 (float64x2_t __a, const int __b)
{
  return __aarch64_vdup_laneq_f64 (__a, __b);
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_laneq_p8 (poly8x16_t __a, const int __b)
{
  return __aarch64_vdup_laneq_p8 (__a, __b);
}
__extension__ extern __inline poly16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_laneq_p16 (poly16x8_t __a, const int __b)
{
  return __aarch64_vdup_laneq_p16 (__a, __b);
}
__extension__ extern __inline poly64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_laneq_p64 (poly64x2_t __a, const int __b)
{
  return __aarch64_vdup_laneq_p64 (__a, __b);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_laneq_s8 (int8x16_t __a, const int __b)
{
  return __aarch64_vdup_laneq_s8 (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_laneq_s16 (int16x8_t __a, const int __b)
{
  return __aarch64_vdup_laneq_s16 (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_laneq_s32 (int32x4_t __a, const int __b)
{
  return __aarch64_vdup_laneq_s32 (__a, __b);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_laneq_s64 (int64x2_t __a, const int __b)
{
  return __aarch64_vdup_laneq_s64 (__a, __b);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_laneq_u8 (uint8x16_t __a, const int __b)
{
  return __aarch64_vdup_laneq_u8 (__a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_laneq_u16 (uint16x8_t __a, const int __b)
{
  return __aarch64_vdup_laneq_u16 (__a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_laneq_u32 (uint32x4_t __a, const int __b)
{
  return __aarch64_vdup_laneq_u32 (__a, __b);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_laneq_u64 (uint64x2_t __a, const int __b)
{
  return __aarch64_vdup_laneq_u64 (__a, __b);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_lane_f16 (float16x4_t __a, const int __b)
{
  return __aarch64_vdupq_lane_f16 (__a, __b);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_lane_f32 (float32x2_t __a, const int __b)
{
  return __aarch64_vdupq_lane_f32 (__a, __b);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_lane_f64 (float64x1_t __a, const int __b)
{
  return __aarch64_vdupq_lane_f64 (__a, __b);
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_lane_p8 (poly8x8_t __a, const int __b)
{
  return __aarch64_vdupq_lane_p8 (__a, __b);
}
__extension__ extern __inline poly16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_lane_p16 (poly16x4_t __a, const int __b)
{
  return __aarch64_vdupq_lane_p16 (__a, __b);
}
__extension__ extern __inline poly64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_lane_p64 (poly64x1_t __a, const int __b)
{
  return __aarch64_vdupq_lane_p64 (__a, __b);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_lane_s8 (int8x8_t __a, const int __b)
{
  return __aarch64_vdupq_lane_s8 (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_lane_s16 (int16x4_t __a, const int __b)
{
  return __aarch64_vdupq_lane_s16 (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_lane_s32 (int32x2_t __a, const int __b)
{
  return __aarch64_vdupq_lane_s32 (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_lane_s64 (int64x1_t __a, const int __b)
{
  return __aarch64_vdupq_lane_s64 (__a, __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_lane_u8 (uint8x8_t __a, const int __b)
{
  return __aarch64_vdupq_lane_u8 (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_lane_u16 (uint16x4_t __a, const int __b)
{
  return __aarch64_vdupq_lane_u16 (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_lane_u32 (uint32x2_t __a, const int __b)
{
  return __aarch64_vdupq_lane_u32 (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_lane_u64 (uint64x1_t __a, const int __b)
{
  return __aarch64_vdupq_lane_u64 (__a, __b);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_laneq_f16 (float16x8_t __a, const int __b)
{
  return __aarch64_vdupq_laneq_f16 (__a, __b);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_laneq_f32 (float32x4_t __a, const int __b)
{
  return __aarch64_vdupq_laneq_f32 (__a, __b);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_laneq_f64 (float64x2_t __a, const int __b)
{
  return __aarch64_vdupq_laneq_f64 (__a, __b);
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_laneq_p8 (poly8x16_t __a, const int __b)
{
  return __aarch64_vdupq_laneq_p8 (__a, __b);
}
__extension__ extern __inline poly16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_laneq_p16 (poly16x8_t __a, const int __b)
{
  return __aarch64_vdupq_laneq_p16 (__a, __b);
}
__extension__ extern __inline poly64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_laneq_p64 (poly64x2_t __a, const int __b)
{
  return __aarch64_vdupq_laneq_p64 (__a, __b);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_laneq_s8 (int8x16_t __a, const int __b)
{
  return __aarch64_vdupq_laneq_s8 (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_laneq_s16 (int16x8_t __a, const int __b)
{
  return __aarch64_vdupq_laneq_s16 (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_laneq_s32 (int32x4_t __a, const int __b)
{
  return __aarch64_vdupq_laneq_s32 (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_laneq_s64 (int64x2_t __a, const int __b)
{
  return __aarch64_vdupq_laneq_s64 (__a, __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_laneq_u8 (uint8x16_t __a, const int __b)
{
  return __aarch64_vdupq_laneq_u8 (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_laneq_u16 (uint16x8_t __a, const int __b)
{
  return __aarch64_vdupq_laneq_u16 (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_laneq_u32 (uint32x4_t __a, const int __b)
{
  return __aarch64_vdupq_laneq_u32 (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_laneq_u64 (uint64x2_t __a, const int __b)
{
  return __aarch64_vdupq_laneq_u64 (__a, __b);
}
__extension__ extern __inline poly8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupb_lane_p8 (poly8x8_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline int8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupb_lane_s8 (int8x8_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline uint8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupb_lane_u8 (uint8x8_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline float16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vduph_lane_f16 (float16x4_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline poly16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vduph_lane_p16 (poly16x4_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vduph_lane_s16 (int16x4_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline uint16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vduph_lane_u16 (uint16x4_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdups_lane_f32 (float32x2_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdups_lane_s32 (int32x2_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdups_lane_u32 (uint32x2_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline float64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupd_lane_f64 (float64x1_t __a, const int __b)
{
  __AARCH64_LANE_CHECK (__a, __b);
  return __a[0];
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupd_lane_s64 (int64x1_t __a, const int __b)
{
  __AARCH64_LANE_CHECK (__a, __b);
  return __a[0];
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupd_lane_u64 (uint64x1_t __a, const int __b)
{
  __AARCH64_LANE_CHECK (__a, __b);
  return __a[0];
}
__extension__ extern __inline poly8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupb_laneq_p8 (poly8x16_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline int8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupb_laneq_s8 (int8x16_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline uint8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupb_laneq_u8 (uint8x16_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline float16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vduph_laneq_f16 (float16x8_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline poly16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vduph_laneq_p16 (poly16x8_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vduph_laneq_s16 (int16x8_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline uint16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vduph_laneq_u16 (uint16x8_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdups_laneq_f32 (float32x4_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdups_laneq_s32 (int32x4_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdups_laneq_u32 (uint32x4_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline float64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupd_laneq_f64 (float64x2_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupd_laneq_s64 (int64x2_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupd_laneq_u64 (uint64x2_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vext_f16 (float16x4_t __a, float16x4_t __b, __const int __c)
{
  __AARCH64_LANE_CHECK (__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle (__b, __a,
       (uint16x4_t) {4 - __c, 5 - __c, 6 - __c, 7 - __c});
#else
  return __builtin_shuffle (__a, __b,
       (uint16x4_t) {__c, __c + 1, __c + 2, __c + 3});
#endif
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vext_f32 (float32x2_t __a, float32x2_t __b, __const int __c)
{
  __AARCH64_LANE_CHECK (__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle (__b, __a, (uint32x2_t) {2-__c, 3-__c});
#else
  return __builtin_shuffle (__a, __b, (uint32x2_t) {__c, __c+1});
#endif
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vext_f64 (float64x1_t __a, float64x1_t __b, __const int __c)
{
  __AARCH64_LANE_CHECK (__a, __c);
  return __a;
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vext_p8 (poly8x8_t __a, poly8x8_t __b, __const int __c)
{
  __AARCH64_LANE_CHECK (__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle (__b, __a, (uint8x8_t)
      {8-__c, 9-__c, 10-__c, 11-__c, 12-__c, 13-__c, 14-__c, 15-__c});
#else
  return __builtin_shuffle (__a, __b,
      (uint8x8_t) {__c, __c+1, __c+2, __c+3, __c+4, __c+5, __c+6, __c+7});
#endif
}
__extension__ extern __inline poly16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vext_p16 (poly16x4_t __a, poly16x4_t __b, __const int __c)
{
  __AARCH64_LANE_CHECK (__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle (__b, __a,
      (uint16x4_t) {4-__c, 5-__c, 6-__c, 7-__c});
#else
  return __builtin_shuffle (__a, __b, (uint16x4_t) {__c, __c+1, __c+2, __c+3});
#endif
}
__extension__ extern __inline poly64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vext_p64 (poly64x1_t __a, poly64x1_t __b, __const int __c)
{
  __AARCH64_LANE_CHECK (__a, __c);
  return __a;
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vext_s8 (int8x8_t __a, int8x8_t __b, __const int __c)
{
  __AARCH64_LANE_CHECK (__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle (__b, __a, (uint8x8_t)
      {8-__c, 9-__c, 10-__c, 11-__c, 12-__c, 13-__c, 14-__c, 15-__c});
#else
  return __builtin_shuffle (__a, __b,
      (uint8x8_t) {__c, __c+1, __c+2, __c+3, __c+4, __c+5, __c+6, __c+7});
#endif
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vext_s16 (int16x4_t __a, int16x4_t __b, __const int __c)
{
  __AARCH64_LANE_CHECK (__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle (__b, __a,
      (uint16x4_t) {4-__c, 5-__c, 6-__c, 7-__c});
#else
  return __builtin_shuffle (__a, __b, (uint16x4_t) {__c, __c+1, __c+2, __c+3});
#endif
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vext_s32 (int32x2_t __a, int32x2_t __b, __const int __c)
{
  __AARCH64_LANE_CHECK (__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle (__b, __a, (uint32x2_t) {2-__c, 3-__c});
#else
  return __builtin_shuffle (__a, __b, (uint32x2_t) {__c, __c+1});
#endif
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vext_s64 (int64x1_t __a, int64x1_t __b, __const int __c)
{
  __AARCH64_LANE_CHECK (__a, __c);
  return __a;
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vext_u8 (uint8x8_t __a, uint8x8_t __b, __const int __c)
{
  __AARCH64_LANE_CHECK (__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle (__b, __a, (uint8x8_t)
      {8-__c, 9-__c, 10-__c, 11-__c, 12-__c, 13-__c, 14-__c, 15-__c});
#else
  return __builtin_shuffle (__a, __b,
      (uint8x8_t) {__c, __c+1, __c+2, __c+3, __c+4, __c+5, __c+6, __c+7});
#endif
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vext_u16 (uint16x4_t __a, uint16x4_t __b, __const int __c)
{
  __AARCH64_LANE_CHECK (__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle (__b, __a,
      (uint16x4_t) {4-__c, 5-__c, 6-__c, 7-__c});
#else
  return __builtin_shuffle (__a, __b, (uint16x4_t) {__c, __c+1, __c+2, __c+3});
#endif
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vext_u32 (uint32x2_t __a, uint32x2_t __b, __const int __c)
{
  __AARCH64_LANE_CHECK (__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle (__b, __a, (uint32x2_t) {2-__c, 3-__c});
#else
  return __builtin_shuffle (__a, __b, (uint32x2_t) {__c, __c+1});
#endif
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vext_u64 (uint64x1_t __a, uint64x1_t __b, __const int __c)
{
  __AARCH64_LANE_CHECK (__a, __c);
  return __a;
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vextq_f16 (float16x8_t __a, float16x8_t __b, __const int __c)
{
  __AARCH64_LANE_CHECK (__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle (__b, __a,
       (uint16x8_t) {8 - __c, 9 - __c, 10 - __c, 11 - __c,
       12 - __c, 13 - __c, 14 - __c,
       15 - __c});
#else
  return __builtin_shuffle (__a, __b,
       (uint16x8_t) {__c, __c + 1, __c + 2, __c + 3,
       __c + 4, __c + 5, __c + 6, __c + 7});
#endif
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vextq_f32 (float32x4_t __a, float32x4_t __b, __const int __c)
{
  __AARCH64_LANE_CHECK (__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle (__b, __a,
      (uint32x4_t) {4-__c, 5-__c, 6-__c, 7-__c});
#else
  return __builtin_shuffle (__a, __b, (uint32x4_t) {__c, __c+1, __c+2, __c+3});
#endif
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vextq_f64 (float64x2_t __a, float64x2_t __b, __const int __c)
{
  __AARCH64_LANE_CHECK (__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle (__b, __a, (uint64x2_t) {2-__c, 3-__c});
#else
  return __builtin_shuffle (__a, __b, (uint64x2_t) {__c, __c+1});
#endif
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vextq_p8 (poly8x16_t __a, poly8x16_t __b, __const int __c)
{
  __AARCH64_LANE_CHECK (__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle (__b, __a, (uint8x16_t)
      {16-__c, 17-__c, 18-__c, 19-__c, 20-__c, 21-__c, 22-__c, 23-__c,
       24-__c, 25-__c, 26-__c, 27-__c, 28-__c, 29-__c, 30-__c, 31-__c});
#else
  return __builtin_shuffle (__a, __b, (uint8x16_t)
      {__c, __c+1, __c+2, __c+3, __c+4, __c+5, __c+6, __c+7,
       __c+8, __c+9, __c+10, __c+11, __c+12, __c+13, __c+14, __c+15});
#endif
}
__extension__ extern __inline poly16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vextq_p16 (poly16x8_t __a, poly16x8_t __b, __const int __c)
{
  __AARCH64_LANE_CHECK (__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle (__b, __a, (uint16x8_t)
      {8-__c, 9-__c, 10-__c, 11-__c, 12-__c, 13-__c, 14-__c, 15-__c});
#else
  return __builtin_shuffle (__a, __b,
      (uint16x8_t) {__c, __c+1, __c+2, __c+3, __c+4, __c+5, __c+6, __c+7});
#endif
}
__extension__ extern __inline poly64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vextq_p64 (poly64x2_t __a, poly64x2_t __b, __const int __c)
{
  __AARCH64_LANE_CHECK (__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle (__b, __a, (uint64x2_t) {2-__c, 3-__c});
#else
  return __builtin_shuffle (__a, __b, (uint64x2_t) {__c, __c+1});
#endif
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vextq_s8 (int8x16_t __a, int8x16_t __b, __const int __c)
{
  __AARCH64_LANE_CHECK (__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle (__b, __a, (uint8x16_t)
      {16-__c, 17-__c, 18-__c, 19-__c, 20-__c, 21-__c, 22-__c, 23-__c,
       24-__c, 25-__c, 26-__c, 27-__c, 28-__c, 29-__c, 30-__c, 31-__c});
#else
  return __builtin_shuffle (__a, __b, (uint8x16_t)
      {__c, __c+1, __c+2, __c+3, __c+4, __c+5, __c+6, __c+7,
       __c+8, __c+9, __c+10, __c+11, __c+12, __c+13, __c+14, __c+15});
#endif
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vextq_s16 (int16x8_t __a, int16x8_t __b, __const int __c)
{
  __AARCH64_LANE_CHECK (__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle (__b, __a, (uint16x8_t)
      {8-__c, 9-__c, 10-__c, 11-__c, 12-__c, 13-__c, 14-__c, 15-__c});
#else
  return __builtin_shuffle (__a, __b,
      (uint16x8_t) {__c, __c+1, __c+2, __c+3, __c+4, __c+5, __c+6, __c+7});
#endif
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vextq_s32 (int32x4_t __a, int32x4_t __b, __const int __c)
{
  __AARCH64_LANE_CHECK (__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle (__b, __a,
      (uint32x4_t) {4-__c, 5-__c, 6-__c, 7-__c});
#else
  return __builtin_shuffle (__a, __b, (uint32x4_t) {__c, __c+1, __c+2, __c+3});
#endif
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vextq_s64 (int64x2_t __a, int64x2_t __b, __const int __c)
{
  __AARCH64_LANE_CHECK (__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle (__b, __a, (uint64x2_t) {2-__c, 3-__c});
#else
  return __builtin_shuffle (__a, __b, (uint64x2_t) {__c, __c+1});
#endif
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vextq_u8 (uint8x16_t __a, uint8x16_t __b, __const int __c)
{
  __AARCH64_LANE_CHECK (__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle (__b, __a, (uint8x16_t)
      {16-__c, 17-__c, 18-__c, 19-__c, 20-__c, 21-__c, 22-__c, 23-__c,
       24-__c, 25-__c, 26-__c, 27-__c, 28-__c, 29-__c, 30-__c, 31-__c});
#else
  return __builtin_shuffle (__a, __b, (uint8x16_t)
      {__c, __c+1, __c+2, __c+3, __c+4, __c+5, __c+6, __c+7,
       __c+8, __c+9, __c+10, __c+11, __c+12, __c+13, __c+14, __c+15});
#endif
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vextq_u16 (uint16x8_t __a, uint16x8_t __b, __const int __c)
{
  __AARCH64_LANE_CHECK (__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle (__b, __a, (uint16x8_t)
      {8-__c, 9-__c, 10-__c, 11-__c, 12-__c, 13-__c, 14-__c, 15-__c});
#else
  return __builtin_shuffle (__a, __b,
      (uint16x8_t) {__c, __c+1, __c+2, __c+3, __c+4, __c+5, __c+6, __c+7});
#endif
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vextq_u32 (uint32x4_t __a, uint32x4_t __b, __const int __c)
{
  __AARCH64_LANE_CHECK (__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle (__b, __a,
      (uint32x4_t) {4-__c, 5-__c, 6-__c, 7-__c});
#else
  return __builtin_shuffle (__a, __b, (uint32x4_t) {__c, __c+1, __c+2, __c+3});
#endif
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vextq_u64 (uint64x2_t __a, uint64x2_t __b, __const int __c)
{
  __AARCH64_LANE_CHECK (__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle (__b, __a, (uint64x2_t) {2-__c, 3-__c});
#else
  return __builtin_shuffle (__a, __b, (uint64x2_t) {__c, __c+1});
#endif
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfma_f64 (float64x1_t __a, float64x1_t __b, float64x1_t __c)
{
  return (float64x1_t) {__builtin_fma (__b[0], __c[0], __a[0])};
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfma_f32 (float32x2_t __a, float32x2_t __b, float32x2_t __c)
{
  return __builtin_aarch64_fmav2sf (__b, __c, __a);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmaq_f32 (float32x4_t __a, float32x4_t __b, float32x4_t __c)
{
  return __builtin_aarch64_fmav4sf (__b, __c, __a);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmaq_f64 (float64x2_t __a, float64x2_t __b, float64x2_t __c)
{
  return __builtin_aarch64_fmav2df (__b, __c, __a);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfma_n_f32 (float32x2_t __a, float32x2_t __b, float32_t __c)
{
  return __builtin_aarch64_fmav2sf (__b, vdup_n_f32 (__c), __a);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfma_n_f64 (float64x1_t __a, float64x1_t __b, float64_t __c)
{
  return (float64x1_t) {__b[0] * __c + __a[0]};
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmaq_n_f32 (float32x4_t __a, float32x4_t __b, float32_t __c)
{
  return __builtin_aarch64_fmav4sf (__b, vdupq_n_f32 (__c), __a);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmaq_n_f64 (float64x2_t __a, float64x2_t __b, float64_t __c)
{
  return __builtin_aarch64_fmav2df (__b, vdupq_n_f64 (__c), __a);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfma_lane_f32 (float32x2_t __a, float32x2_t __b,
        float32x2_t __c, const int __lane)
{
  return __builtin_aarch64_fmav2sf (__b,
        __aarch64_vdup_lane_f32 (__c, __lane),
        __a);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfma_lane_f64 (float64x1_t __a, float64x1_t __b,
        float64x1_t __c, const int __lane)
{
  return (float64x1_t) {__builtin_fma (__b[0], __c[0], __a[0])};
}
__extension__ extern __inline float64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmad_lane_f64 (float64_t __a, float64_t __b,
         float64x1_t __c, const int __lane)
{
  return __builtin_fma (__b, __c[0], __a);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmas_lane_f32 (float32_t __a, float32_t __b,
         float32x2_t __c, const int __lane)
{
  return __builtin_fmaf (__b, __aarch64_vget_lane_any (__c, __lane), __a);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfma_laneq_f32 (float32x2_t __a, float32x2_t __b,
         float32x4_t __c, const int __lane)
{
  return __builtin_aarch64_fmav2sf (__b,
        __aarch64_vdup_laneq_f32 (__c, __lane),
        __a);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfma_laneq_f64 (float64x1_t __a, float64x1_t __b,
         float64x2_t __c, const int __lane)
{
  float64_t __c0 = __aarch64_vget_lane_any (__c, __lane);
  return (float64x1_t) {__builtin_fma (__b[0], __c0, __a[0])};
}
__extension__ extern __inline float64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmad_laneq_f64 (float64_t __a, float64_t __b,
          float64x2_t __c, const int __lane)
{
  return __builtin_fma (__b, __aarch64_vget_lane_any (__c, __lane), __a);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmas_laneq_f32 (float32_t __a, float32_t __b,
   float32x4_t __c, const int __lane)
{
  return __builtin_fmaf (__b, __aarch64_vget_lane_any (__c, __lane), __a);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmaq_lane_f32 (float32x4_t __a, float32x4_t __b,
         float32x2_t __c, const int __lane)
{
  return __builtin_aarch64_fmav4sf (__b,
        __aarch64_vdupq_lane_f32 (__c, __lane),
        __a);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmaq_lane_f64 (float64x2_t __a, float64x2_t __b,
         float64x1_t __c, const int __lane)
{
  return __builtin_aarch64_fmav2df (__b, vdupq_n_f64 (__c[0]), __a);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmaq_laneq_f32 (float32x4_t __a, float32x4_t __b,
          float32x4_t __c, const int __lane)
{
  return __builtin_aarch64_fmav4sf (__b,
        __aarch64_vdupq_laneq_f32 (__c, __lane),
        __a);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmaq_laneq_f64 (float64x2_t __a, float64x2_t __b,
          float64x2_t __c, const int __lane)
{
  return __builtin_aarch64_fmav2df (__b,
        __aarch64_vdupq_laneq_f64 (__c, __lane),
        __a);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfms_f64 (float64x1_t __a, float64x1_t __b, float64x1_t __c)
{
  return (float64x1_t) {__builtin_fma (-__b[0], __c[0], __a[0])};
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfms_f32 (float32x2_t __a, float32x2_t __b, float32x2_t __c)
{
  return __builtin_aarch64_fmav2sf (-__b, __c, __a);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmsq_f32 (float32x4_t __a, float32x4_t __b, float32x4_t __c)
{
  return __builtin_aarch64_fmav4sf (-__b, __c, __a);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmsq_f64 (float64x2_t __a, float64x2_t __b, float64x2_t __c)
{
  return __builtin_aarch64_fmav2df (-__b, __c, __a);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfms_n_f32 (float32x2_t __a, float32x2_t __b, float32_t __c)
{
  return __builtin_aarch64_fmav2sf (-__b, vdup_n_f32 (__c), __a);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfms_n_f64 (float64x1_t __a, float64x1_t __b, float64_t __c)
{
  return (float64x1_t) {-__b[0] * __c + __a[0]};
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmsq_n_f32 (float32x4_t __a, float32x4_t __b, float32_t __c)
{
  return __builtin_aarch64_fmav4sf (-__b, vdupq_n_f32 (__c), __a);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmsq_n_f64 (float64x2_t __a, float64x2_t __b, float64_t __c)
{
  return __builtin_aarch64_fmav2df (-__b, vdupq_n_f64 (__c), __a);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfms_lane_f32 (float32x2_t __a, float32x2_t __b,
        float32x2_t __c, const int __lane)
{
  return __builtin_aarch64_fmav2sf (-__b,
        __aarch64_vdup_lane_f32 (__c, __lane),
        __a);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfms_lane_f64 (float64x1_t __a, float64x1_t __b,
        float64x1_t __c, const int __lane)
{
  return (float64x1_t) {__builtin_fma (-__b[0], __c[0], __a[0])};
}
__extension__ extern __inline float64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmsd_lane_f64 (float64_t __a, float64_t __b,
         float64x1_t __c, const int __lane)
{
  return __builtin_fma (-__b, __c[0], __a);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmss_lane_f32 (float32_t __a, float32_t __b,
         float32x2_t __c, const int __lane)
{
  return __builtin_fmaf (-__b, __aarch64_vget_lane_any (__c, __lane), __a);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfms_laneq_f32 (float32x2_t __a, float32x2_t __b,
         float32x4_t __c, const int __lane)
{
  return __builtin_aarch64_fmav2sf (-__b,
        __aarch64_vdup_laneq_f32 (__c, __lane),
        __a);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfms_laneq_f64 (float64x1_t __a, float64x1_t __b,
         float64x2_t __c, const int __lane)
{
  float64_t __c0 = __aarch64_vget_lane_any (__c, __lane);
  return (float64x1_t) {__builtin_fma (-__b[0], __c0, __a[0])};
}
__extension__ extern __inline float64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmsd_laneq_f64 (float64_t __a, float64_t __b,
          float64x2_t __c, const int __lane)
{
  return __builtin_fma (-__b, __aarch64_vget_lane_any (__c, __lane), __a);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmss_laneq_f32 (float32_t __a, float32_t __b,
   float32x4_t __c, const int __lane)
{
  return __builtin_fmaf (-__b, __aarch64_vget_lane_any (__c, __lane), __a);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmsq_lane_f32 (float32x4_t __a, float32x4_t __b,
         float32x2_t __c, const int __lane)
{
  return __builtin_aarch64_fmav4sf (-__b,
        __aarch64_vdupq_lane_f32 (__c, __lane),
        __a);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmsq_lane_f64 (float64x2_t __a, float64x2_t __b,
         float64x1_t __c, const int __lane)
{
  return __builtin_aarch64_fmav2df (-__b, vdupq_n_f64 (__c[0]), __a);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmsq_laneq_f32 (float32x4_t __a, float32x4_t __b,
          float32x4_t __c, const int __lane)
{
  return __builtin_aarch64_fmav4sf (-__b,
        __aarch64_vdupq_laneq_f32 (__c, __lane),
        __a);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmsq_laneq_f64 (float64x2_t __a, float64x2_t __b,
          float64x2_t __c, const int __lane)
{
  return __builtin_aarch64_fmav2df (-__b,
        __aarch64_vdupq_laneq_f64 (__c, __lane),
        __a);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_f16 (const float16_t *__a)
{
  return __builtin_aarch64_ld1v4hf (__a);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_f32 (const float32_t *__a)
{
  return __builtin_aarch64_ld1v2sf ((const __builtin_aarch64_simd_sf *) __a);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_f64 (const float64_t *__a)
{
  return (float64x1_t) {*__a};
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_p8 (const poly8_t *__a)
{
  return __builtin_aarch64_ld1v8qi_ps (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline poly16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_p16 (const poly16_t *__a)
{
  return __builtin_aarch64_ld1v4hi_ps (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline poly64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_p64 (const poly64_t *__a)
{
  return (poly64x1_t) {*__a};
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_s8 (const int8_t *__a)
{
  return __builtin_aarch64_ld1v8qi ((const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_s16 (const int16_t *__a)
{
  return __builtin_aarch64_ld1v4hi ((const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_s32 (const int32_t *__a)
{
  return __builtin_aarch64_ld1v2si ((const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_s64 (const int64_t *__a)
{
  return (int64x1_t) {*__a};
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_u8 (const uint8_t *__a)
{
  return __builtin_aarch64_ld1v8qi_us (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_u16 (const uint16_t *__a)
{
  return __builtin_aarch64_ld1v4hi_us (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_u32 (const uint32_t *__a)
{
  return __builtin_aarch64_ld1v2si_us (
    (const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_u64 (const uint64_t *__a)
{
  return (uint64x1_t) {*__a};
}
__extension__ extern __inline uint8x8x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_u8_x3 (const uint8_t *__a)
{
  return __builtin_aarch64_ld1x3v8qi_us (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline int8x8x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_s8_x3 (const int8_t *__a)
{
  return __builtin_aarch64_ld1x3v8qi ((const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline uint16x4x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_u16_x3 (const uint16_t *__a)
{
  return __builtin_aarch64_ld1x3v4hi_us (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline int16x4x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_s16_x3 (const int16_t *__a)
{
  return __builtin_aarch64_ld1x3v4hi ((const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline uint32x2x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_u32_x3 (const uint32_t *__a)
{
  return __builtin_aarch64_ld1x3v2si_us (
    (const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline int32x2x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_s32_x3 (const int32_t *__a)
{
  return __builtin_aarch64_ld1x3v2si ((const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline uint64x1x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_u64_x3 (const uint64_t *__a)
{
  return __builtin_aarch64_ld1x3di_us (
    (const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline int64x1x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_s64_x3 (const int64_t *__a)
{
  return __builtin_aarch64_ld1x3di ((const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline float16x4x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_f16_x3 (const float16_t *__a)
{
  return __builtin_aarch64_ld1x3v4hf ((const __builtin_aarch64_simd_hf *) __a);
}
__extension__ extern __inline float32x2x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_f32_x3 (const float32_t *__a)
{
  return __builtin_aarch64_ld1x3v2sf ((const __builtin_aarch64_simd_sf *) __a);
}
__extension__ extern __inline float64x1x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_f64_x3 (const float64_t *__a)
{
  return __builtin_aarch64_ld1x3df ((const __builtin_aarch64_simd_df *) __a);
}
__extension__ extern __inline poly8x8x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_p8_x3 (const poly8_t *__a)
{
  return __builtin_aarch64_ld1x3v8qi_ps (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline poly16x4x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_p16_x3 (const poly16_t *__a)
{
  return __builtin_aarch64_ld1x3v4hi_ps (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline poly64x1x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_p64_x3 (const poly64_t *__a)
{
  return __builtin_aarch64_ld1x3di_ps (
    (const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline uint8x16x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_u8_x3 (const uint8_t *__a)
{
  return __builtin_aarch64_ld1x3v16qi_us (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline int8x16x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_s8_x3 (const int8_t *__a)
{
  return __builtin_aarch64_ld1x3v16qi (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline uint16x8x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_u16_x3 (const uint16_t *__a)
{
  return __builtin_aarch64_ld1x3v8hi_us (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline int16x8x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_s16_x3 (const int16_t *__a)
{
  return __builtin_aarch64_ld1x3v8hi ((const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline uint32x4x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_u32_x3 (const uint32_t *__a)
{
  return __builtin_aarch64_ld1x3v4si_us (
    (const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline int32x4x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_s32_x3 (const int32_t *__a)
{
  return __builtin_aarch64_ld1x3v4si ((const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline uint64x2x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_u64_x3 (const uint64_t *__a)
{
  return __builtin_aarch64_ld1x3v2di_us (
    (const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline int64x2x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_s64_x3 (const int64_t *__a)
{
  return __builtin_aarch64_ld1x3v2di ((const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline float16x8x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_f16_x3 (const float16_t *__a)
{
  return __builtin_aarch64_ld1x3v8hf ((const __builtin_aarch64_simd_hf *) __a);
}
__extension__ extern __inline float32x4x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_f32_x3 (const float32_t *__a)
{
  return __builtin_aarch64_ld1x3v4sf ((const __builtin_aarch64_simd_sf *) __a);
}
__extension__ extern __inline float64x2x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_f64_x3 (const float64_t *__a)
{
  return __builtin_aarch64_ld1x3v2df ((const __builtin_aarch64_simd_df *) __a);
}
__extension__ extern __inline poly8x16x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_p8_x3 (const poly8_t *__a)
{
  return __builtin_aarch64_ld1x3v16qi_ps (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline poly16x8x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_p16_x3 (const poly16_t *__a)
{
  return __builtin_aarch64_ld1x3v8hi_ps (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline poly64x2x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_p64_x3 (const poly64_t *__a)
{
  return __builtin_aarch64_ld1x3v2di_ps (
    (const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_f16 (const float16_t *__a)
{
  return __builtin_aarch64_ld1v8hf (__a);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_f32 (const float32_t *__a)
{
  return __builtin_aarch64_ld1v4sf ((const __builtin_aarch64_simd_sf *) __a);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_f64 (const float64_t *__a)
{
  return __builtin_aarch64_ld1v2df ((const __builtin_aarch64_simd_df *) __a);
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_p8 (const poly8_t *__a)
{
  return __builtin_aarch64_ld1v16qi_ps (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline poly16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_p16 (const poly16_t *__a)
{
  return __builtin_aarch64_ld1v8hi_ps (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline poly64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_p64 (const poly64_t *__a)
{
  return __builtin_aarch64_ld1v2di_ps (
    (const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_s8 (const int8_t *__a)
{
  return __builtin_aarch64_ld1v16qi ((const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_s16 (const int16_t *__a)
{
  return __builtin_aarch64_ld1v8hi ((const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_s32 (const int32_t *__a)
{
  return __builtin_aarch64_ld1v4si ((const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_s64 (const int64_t *__a)
{
  return __builtin_aarch64_ld1v2di ((const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_u8 (const uint8_t *__a)
{
  return __builtin_aarch64_ld1v16qi_us (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline uint8x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_u8_x2 (const uint8_t *__a)
{
  return __builtin_aarch64_ld1x2v8qi_us (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline int8x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_s8_x2 (const int8_t *__a)
{
  return __builtin_aarch64_ld1x2v8qi ((const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline uint16x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_u16_x2 (const uint16_t *__a)
{
  return __builtin_aarch64_ld1x2v4hi_us (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline int16x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_s16_x2 (const int16_t *__a)
{
  return __builtin_aarch64_ld1x2v4hi ((const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline uint32x2x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_u32_x2 (const uint32_t *__a)
{
  return __builtin_aarch64_ld1x2v2si_us (
    (const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline int32x2x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_s32_x2 (const int32_t *__a)
{
  return __builtin_aarch64_ld1x2v2si ((const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline uint64x1x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_u64_x2 (const uint64_t *__a)
{
  return __builtin_aarch64_ld1x2di_us (
    (const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline int64x1x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_s64_x2 (const int64_t *__a)
{
  return __builtin_aarch64_ld1x2di ((const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline float16x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_f16_x2 (const float16_t *__a)
{
  return __builtin_aarch64_ld1x2v4hf ((const __builtin_aarch64_simd_hf *) __a);
}
__extension__ extern __inline float32x2x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_f32_x2 (const float32_t *__a)
{
  return __builtin_aarch64_ld1x2v2sf ((const __builtin_aarch64_simd_sf *) __a);
}
__extension__ extern __inline float64x1x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_f64_x2 (const float64_t *__a)
{
  return __builtin_aarch64_ld1x2df ((const __builtin_aarch64_simd_df *) __a);
}
__extension__ extern __inline poly8x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_p8_x2 (const poly8_t *__a)
{
  return __builtin_aarch64_ld1x2v8qi_ps (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline poly16x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_p16_x2 (const poly16_t *__a)
{
  return __builtin_aarch64_ld1x2v4hi_ps (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline poly64x1x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_p64_x2 (const poly64_t *__a)
{
  return __builtin_aarch64_ld1x2di_ps (
    (const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline uint8x16x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_u8_x2 (const uint8_t *__a)
{
  return __builtin_aarch64_ld1x2v16qi_us (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline int8x16x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_s8_x2 (const int8_t *__a)
{
  return __builtin_aarch64_ld1x2v16qi (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline uint16x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_u16_x2 (const uint16_t *__a)
{
  return __builtin_aarch64_ld1x2v8hi_us (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline int16x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_s16_x2 (const int16_t *__a)
{
  return __builtin_aarch64_ld1x2v8hi ((const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline uint32x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_u32_x2 (const uint32_t *__a)
{
  return __builtin_aarch64_ld1x2v4si_us (
    (const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline int32x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_s32_x2 (const int32_t *__a)
{
  return __builtin_aarch64_ld1x2v4si ((const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline uint64x2x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_u64_x2 (const uint64_t *__a)
{
  return __builtin_aarch64_ld1x2v2di_us (
    (const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline int64x2x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_s64_x2 (const int64_t *__a)
{
  return __builtin_aarch64_ld1x2v2di ((const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline float16x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_f16_x2 (const float16_t *__a)
{
  return __builtin_aarch64_ld1x2v8hf ((const __builtin_aarch64_simd_hf *) __a);
}
__extension__ extern __inline float32x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_f32_x2 (const float32_t *__a)
{
  return __builtin_aarch64_ld1x2v4sf ((const __builtin_aarch64_simd_sf *) __a);
}
__extension__ extern __inline float64x2x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_f64_x2 (const float64_t *__a)
{
  return __builtin_aarch64_ld1x2v2df ((const __builtin_aarch64_simd_df *) __a);
}
__extension__ extern __inline poly8x16x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_p8_x2 (const poly8_t *__a)
{
  return __builtin_aarch64_ld1x2v16qi_ps (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline poly16x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_p16_x2 (const poly16_t *__a)
{
  return __builtin_aarch64_ld1x2v8hi_ps (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline poly64x2x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_p64_x2 (const poly64_t *__a)
{
  return __builtin_aarch64_ld1x2v2di_ps (
    (const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_u16 (const uint16_t *__a)
{
  return __builtin_aarch64_ld1v8hi_us (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_u32 (const uint32_t *__a)
{
  return __builtin_aarch64_ld1v4si_us (
    (const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_u64 (const uint64_t *__a)
{
  return __builtin_aarch64_ld1v2di_us (
    (const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline int8x8x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_s8_x4 (const int8_t *__a)
{
  return __builtin_aarch64_ld1x4v8qi ((const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline int8x16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_s8_x4 (const int8_t *__a)
{
  return __builtin_aarch64_ld1x4v16qi (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline int16x4x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_s16_x4 (const int16_t *__a)
{
  return __builtin_aarch64_ld1x4v4hi ((const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline int16x8x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_s16_x4 (const int16_t *__a)
{
  return __builtin_aarch64_ld1x4v8hi ((const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline int32x2x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_s32_x4 (const int32_t *__a)
{
  return __builtin_aarch64_ld1x4v2si ((const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline int32x4x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_s32_x4 (const int32_t *__a)
{
  return __builtin_aarch64_ld1x4v4si ((const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline uint8x8x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_u8_x4 (const uint8_t *__a)
{
  return __builtin_aarch64_ld1x4v8qi_us (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline uint8x16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_u8_x4 (const uint8_t *__a)
{
  return __builtin_aarch64_ld1x4v16qi_us (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline uint16x4x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_u16_x4 (const uint16_t *__a)
{
  return __builtin_aarch64_ld1x4v4hi_us (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline uint16x8x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_u16_x4 (const uint16_t *__a)
{
  return __builtin_aarch64_ld1x4v8hi_us (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline uint32x2x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_u32_x4 (const uint32_t *__a)
{
  return __builtin_aarch64_ld1x4v2si_us (
    (const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline uint32x4x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_u32_x4 (const uint32_t *__a)
{
  return __builtin_aarch64_ld1x4v4si_us (
    (const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline float16x4x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_f16_x4 (const float16_t *__a)
{
  return __builtin_aarch64_ld1x4v4hf ((const __builtin_aarch64_simd_hf *) __a);
}
__extension__ extern __inline float16x8x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_f16_x4 (const float16_t *__a)
{
  return __builtin_aarch64_ld1x4v8hf ((const __builtin_aarch64_simd_hf *) __a);
}
__extension__ extern __inline float32x2x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_f32_x4 (const float32_t *__a)
{
  return __builtin_aarch64_ld1x4v2sf ((const __builtin_aarch64_simd_sf *) __a);
}
__extension__ extern __inline float32x4x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_f32_x4 (const float32_t *__a)
{
  return __builtin_aarch64_ld1x4v4sf ((const __builtin_aarch64_simd_sf *) __a);
}
__extension__ extern __inline poly8x8x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_p8_x4 (const poly8_t *__a)
{
  return __builtin_aarch64_ld1x4v8qi_ps (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline poly8x16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_p8_x4 (const poly8_t *__a)
{
  return __builtin_aarch64_ld1x4v16qi_ps (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline poly16x4x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_p16_x4 (const poly16_t *__a)
{
  return __builtin_aarch64_ld1x4v4hi_ps (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline poly16x8x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_p16_x4 (const poly16_t *__a)
{
  return __builtin_aarch64_ld1x4v8hi_ps (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline int64x1x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_s64_x4 (const int64_t *__a)
{
  return __builtin_aarch64_ld1x4di ((const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline uint64x1x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_u64_x4 (const uint64_t *__a)
{
  return __builtin_aarch64_ld1x4di_us (
    (const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline poly64x1x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_p64_x4 (const poly64_t *__a)
{
  return __builtin_aarch64_ld1x4di_ps (
    (const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline int64x2x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_s64_x4 (const int64_t *__a)
{
  return __builtin_aarch64_ld1x4v2di ((const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline uint64x2x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_u64_x4 (const uint64_t *__a)
{
  return __builtin_aarch64_ld1x4v2di_us (
    (const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline poly64x2x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_p64_x4 (const poly64_t *__a)
{
  return __builtin_aarch64_ld1x4v2di_ps (
    (const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline float64x1x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_f64_x4 (const float64_t *__a)
{
  return __builtin_aarch64_ld1x4df ((const __builtin_aarch64_simd_df *) __a);
}
__extension__ extern __inline float64x2x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_f64_x4 (const float64_t *__a)
{
  return __builtin_aarch64_ld1x4v2df ((const __builtin_aarch64_simd_df *) __a);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_dup_f16 (const float16_t* __a)
{
  return vdup_n_f16 (*__a);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_dup_f32 (const float32_t* __a)
{
  return vdup_n_f32 (*__a);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_dup_f64 (const float64_t* __a)
{
  return vdup_n_f64 (*__a);
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_dup_p8 (const poly8_t* __a)
{
  return vdup_n_p8 (*__a);
}
__extension__ extern __inline poly16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_dup_p16 (const poly16_t* __a)
{
  return vdup_n_p16 (*__a);
}
__extension__ extern __inline poly64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_dup_p64 (const poly64_t* __a)
{
  return vdup_n_p64 (*__a);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_dup_s8 (const int8_t* __a)
{
  return vdup_n_s8 (*__a);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_dup_s16 (const int16_t* __a)
{
  return vdup_n_s16 (*__a);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_dup_s32 (const int32_t* __a)
{
  return vdup_n_s32 (*__a);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_dup_s64 (const int64_t* __a)
{
  return vdup_n_s64 (*__a);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_dup_u8 (const uint8_t* __a)
{
  return vdup_n_u8 (*__a);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_dup_u16 (const uint16_t* __a)
{
  return vdup_n_u16 (*__a);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_dup_u32 (const uint32_t* __a)
{
  return vdup_n_u32 (*__a);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_dup_u64 (const uint64_t* __a)
{
  return vdup_n_u64 (*__a);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_dup_f16 (const float16_t* __a)
{
  return vdupq_n_f16 (*__a);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_dup_f32 (const float32_t* __a)
{
  return vdupq_n_f32 (*__a);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_dup_f64 (const float64_t* __a)
{
  return vdupq_n_f64 (*__a);
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_dup_p8 (const poly8_t* __a)
{
  return vdupq_n_p8 (*__a);
}
__extension__ extern __inline poly16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_dup_p16 (const poly16_t* __a)
{
  return vdupq_n_p16 (*__a);
}
__extension__ extern __inline poly64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_dup_p64 (const poly64_t* __a)
{
  return vdupq_n_p64 (*__a);
}
 __extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_dup_s8 (const int8_t* __a)
{
  return vdupq_n_s8 (*__a);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_dup_s16 (const int16_t* __a)
{
  return vdupq_n_s16 (*__a);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_dup_s32 (const int32_t* __a)
{
  return vdupq_n_s32 (*__a);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_dup_s64 (const int64_t* __a)
{
  return vdupq_n_s64 (*__a);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_dup_u8 (const uint8_t* __a)
{
  return vdupq_n_u8 (*__a);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_dup_u16 (const uint16_t* __a)
{
  return vdupq_n_u16 (*__a);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_dup_u32 (const uint32_t* __a)
{
  return vdupq_n_u32 (*__a);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_dup_u64 (const uint64_t* __a)
{
  return vdupq_n_u64 (*__a);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_lane_f16 (const float16_t *__src, float16x4_t __vec, const int __lane)
{
  return __aarch64_vset_lane_any (*__src, __vec, __lane);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_lane_f32 (const float32_t *__src, float32x2_t __vec, const int __lane)
{
  return __aarch64_vset_lane_any (*__src, __vec, __lane);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_lane_f64 (const float64_t *__src, float64x1_t __vec, const int __lane)
{
  return __aarch64_vset_lane_any (*__src, __vec, __lane);
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_lane_p8 (const poly8_t *__src, poly8x8_t __vec, const int __lane)
{
  return __aarch64_vset_lane_any (*__src, __vec, __lane);
}
__extension__ extern __inline poly16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_lane_p16 (const poly16_t *__src, poly16x4_t __vec, const int __lane)
{
  return __aarch64_vset_lane_any (*__src, __vec, __lane);
}
__extension__ extern __inline poly64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_lane_p64 (const poly64_t *__src, poly64x1_t __vec, const int __lane)
{
  return __aarch64_vset_lane_any (*__src, __vec, __lane);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_lane_s8 (const int8_t *__src, int8x8_t __vec, const int __lane)
{
  return __aarch64_vset_lane_any (*__src, __vec, __lane);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_lane_s16 (const int16_t *__src, int16x4_t __vec, const int __lane)
{
  return __aarch64_vset_lane_any (*__src, __vec, __lane);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_lane_s32 (const int32_t *__src, int32x2_t __vec, const int __lane)
{
  return __aarch64_vset_lane_any (*__src, __vec, __lane);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_lane_s64 (const int64_t *__src, int64x1_t __vec, const int __lane)
{
  return __aarch64_vset_lane_any (*__src, __vec, __lane);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_lane_u8 (const uint8_t *__src, uint8x8_t __vec, const int __lane)
{
  return __aarch64_vset_lane_any (*__src, __vec, __lane);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_lane_u16 (const uint16_t *__src, uint16x4_t __vec, const int __lane)
{
  return __aarch64_vset_lane_any (*__src, __vec, __lane);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_lane_u32 (const uint32_t *__src, uint32x2_t __vec, const int __lane)
{
  return __aarch64_vset_lane_any (*__src, __vec, __lane);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_lane_u64 (const uint64_t *__src, uint64x1_t __vec, const int __lane)
{
  return __aarch64_vset_lane_any (*__src, __vec, __lane);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_lane_f16 (const float16_t *__src, float16x8_t __vec, const int __lane)
{
  return __aarch64_vset_lane_any (*__src, __vec, __lane);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_lane_f32 (const float32_t *__src, float32x4_t __vec, const int __lane)
{
  return __aarch64_vset_lane_any (*__src, __vec, __lane);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_lane_f64 (const float64_t *__src, float64x2_t __vec, const int __lane)
{
  return __aarch64_vset_lane_any (*__src, __vec, __lane);
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_lane_p8 (const poly8_t *__src, poly8x16_t __vec, const int __lane)
{
  return __aarch64_vset_lane_any (*__src, __vec, __lane);
}
__extension__ extern __inline poly16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_lane_p16 (const poly16_t *__src, poly16x8_t __vec, const int __lane)
{
  return __aarch64_vset_lane_any (*__src, __vec, __lane);
}
__extension__ extern __inline poly64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_lane_p64 (const poly64_t *__src, poly64x2_t __vec, const int __lane)
{
  return __aarch64_vset_lane_any (*__src, __vec, __lane);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_lane_s8 (const int8_t *__src, int8x16_t __vec, const int __lane)
{
  return __aarch64_vset_lane_any (*__src, __vec, __lane);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_lane_s16 (const int16_t *__src, int16x8_t __vec, const int __lane)
{
  return __aarch64_vset_lane_any (*__src, __vec, __lane);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_lane_s32 (const int32_t *__src, int32x4_t __vec, const int __lane)
{
  return __aarch64_vset_lane_any (*__src, __vec, __lane);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_lane_s64 (const int64_t *__src, int64x2_t __vec, const int __lane)
{
  return __aarch64_vset_lane_any (*__src, __vec, __lane);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_lane_u8 (const uint8_t *__src, uint8x16_t __vec, const int __lane)
{
  return __aarch64_vset_lane_any (*__src, __vec, __lane);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_lane_u16 (const uint16_t *__src, uint16x8_t __vec, const int __lane)
{
  return __aarch64_vset_lane_any (*__src, __vec, __lane);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_lane_u32 (const uint32_t *__src, uint32x4_t __vec, const int __lane)
{
  return __aarch64_vset_lane_any (*__src, __vec, __lane);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_lane_u64 (const uint64_t *__src, uint64x2_t __vec, const int __lane)
{
  return __aarch64_vset_lane_any (*__src, __vec, __lane);
}
#pragma GCC push_options
#pragma GCC target ("+nothing+rcpc3+simd")
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vldap1_lane_u64 (const uint64_t *__src, uint64x1_t __vec, const int __lane)
{
  return __builtin_aarch64_vec_ldap1_lanev1di_usus (
   (const __builtin_aarch64_simd_di *) __src, __vec, __lane);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vldap1q_lane_u64 (const uint64_t *__src, uint64x2_t __vec, const int __lane)
{
  return __builtin_aarch64_vec_ldap1_lanev2di_usus (
   (const __builtin_aarch64_simd_di *) __src, __vec, __lane);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vldap1_lane_s64 (const int64_t *__src, int64x1_t __vec, const int __lane)
{
  return __builtin_aarch64_vec_ldap1_lanev1di (
   (const __builtin_aarch64_simd_di *) __src, __vec, __lane);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vldap1q_lane_s64 (const int64_t *__src, int64x2_t __vec, const int __lane)
{
  return __builtin_aarch64_vec_ldap1_lanev2di (
   (const __builtin_aarch64_simd_di *) __src, __vec, __lane);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vldap1_lane_f64 (const float64_t *__src, float64x1_t __vec, const int __lane)
{
  return __builtin_aarch64_vec_ldap1_lanev1df (
   (const __builtin_aarch64_simd_df *) __src, __vec, __lane);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vldap1q_lane_f64 (const float64_t *__src, float64x2_t __vec, const int __lane)
{
  return __builtin_aarch64_vec_ldap1_lanev2df (
   (const __builtin_aarch64_simd_df *) __src, __vec, __lane);
}
__extension__ extern __inline poly64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vldap1_lane_p64 (const poly64_t *__src, poly64x1_t __vec, const int __lane)
{
  return __builtin_aarch64_vec_ldap1_lanev1di_psps (
   (const __builtin_aarch64_simd_di *) __src, __vec, __lane);
}
__extension__ extern __inline poly64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vldap1q_lane_p64 (const poly64_t *__src, poly64x2_t __vec, const int __lane)
{
  return __builtin_aarch64_vec_ldap1_lanev2di_psps (
   (const __builtin_aarch64_simd_di *) __src, __vec, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vstl1_lane_u64 (uint64_t *__src, uint64x1_t __vec, const int __lane)
{
  __builtin_aarch64_vec_stl1_lanev1di_sus ((__builtin_aarch64_simd_di *) __src,
        __vec, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vstl1q_lane_u64 (uint64_t *__src, uint64x2_t __vec, const int __lane)
{
  __builtin_aarch64_vec_stl1_lanev2di_sus ((__builtin_aarch64_simd_di *) __src,
        __vec, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vstl1_lane_s64 (int64_t *__src, int64x1_t __vec, const int __lane)
{
  __builtin_aarch64_vec_stl1_lanev1di ((__builtin_aarch64_simd_di *) __src,
           __vec, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vstl1q_lane_s64 (int64_t *__src, int64x2_t __vec, const int __lane)
{
  __builtin_aarch64_vec_stl1_lanev2di ((__builtin_aarch64_simd_di *) __src,
           __vec, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vstl1_lane_f64 (float64_t *__src, float64x1_t __vec, const int __lane)
{
  __builtin_aarch64_vec_stl1_lanev1df ((__builtin_aarch64_simd_df *) __src,
           __vec, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vstl1q_lane_f64 (float64_t *__src, float64x2_t __vec, const int __lane)
{
  __builtin_aarch64_vec_stl1_lanev2df ((__builtin_aarch64_simd_df *) __src,
           __vec, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vstl1_lane_p64 (poly64_t *__src, poly64x1_t __vec, const int __lane)
{
  __builtin_aarch64_vec_stl1_lanev1di_sps ((__builtin_aarch64_simd_di *) __src,
        __vec, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vstl1q_lane_p64 (poly64_t *__src, poly64x2_t __vec, const int __lane)
{
  __builtin_aarch64_vec_stl1_lanev2di_sps ((__builtin_aarch64_simd_di *) __src,
        __vec, __lane);
}
#pragma GCC pop_options
__extension__ extern __inline int64x1x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2_s64 (const int64_t * __a)
{
  return __builtin_aarch64_ld2di ((const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline uint64x1x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2_u64 (const uint64_t * __a)
{
  return __builtin_aarch64_ld2di_us ((const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline float64x1x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2_f64 (const float64_t * __a)
{
  return __builtin_aarch64_ld2df ((const __builtin_aarch64_simd_df *) __a);
}
__extension__ extern __inline int8x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2_s8 (const int8_t * __a)
{
  return __builtin_aarch64_ld2v8qi ((const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline poly8x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2_p8 (const poly8_t * __a)
{
  return __builtin_aarch64_ld2v8qi_ps (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline poly64x1x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2_p64 (const poly64_t * __a)
{
  return __builtin_aarch64_ld2di_ps ((const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline int16x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2_s16 (const int16_t * __a)
{
  return __builtin_aarch64_ld2v4hi ((const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline poly16x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2_p16 (const poly16_t * __a)
{
  return __builtin_aarch64_ld2v4hi_ps (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline int32x2x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2_s32 (const int32_t * __a)
{
  return __builtin_aarch64_ld2v2si ((const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline uint8x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2_u8 (const uint8_t * __a)
{
  return __builtin_aarch64_ld2v8qi_us (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline uint16x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2_u16 (const uint16_t * __a)
{
  return __builtin_aarch64_ld2v4hi_us (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline uint32x2x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2_u32 (const uint32_t * __a)
{
  return __builtin_aarch64_ld2v2si_us (
    (const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline float16x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2_f16 (const float16_t * __a)
{
  return __builtin_aarch64_ld2v4hf (__a);
}
__extension__ extern __inline float32x2x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2_f32 (const float32_t * __a)
{
  return __builtin_aarch64_ld2v2sf ((const __builtin_aarch64_simd_sf *) __a);
}
__extension__ extern __inline int8x16x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2q_s8 (const int8_t * __a)
{
  return __builtin_aarch64_ld2v16qi ((const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline poly8x16x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2q_p8 (const poly8_t * __a)
{
  return __builtin_aarch64_ld2v16qi_ps (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline int16x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2q_s16 (const int16_t * __a)
{
  return __builtin_aarch64_ld2v8hi ((const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline poly16x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2q_p16 (const poly16_t * __a)
{
  return __builtin_aarch64_ld2v8hi_ps (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline poly64x2x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2q_p64 (const poly64_t * __a)
{
  return __builtin_aarch64_ld2v2di_ps (
    (const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline int32x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2q_s32 (const int32_t * __a)
{
  return __builtin_aarch64_ld2v4si ((const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline int64x2x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2q_s64 (const int64_t * __a)
{
  return __builtin_aarch64_ld2v2di ((const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline uint8x16x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2q_u8 (const uint8_t * __a)
{
  return __builtin_aarch64_ld2v16qi_us (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline uint16x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2q_u16 (const uint16_t * __a)
{
  return __builtin_aarch64_ld2v8hi_us (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline uint32x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2q_u32 (const uint32_t * __a)
{
  return __builtin_aarch64_ld2v4si_us (
    (const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline uint64x2x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2q_u64 (const uint64_t * __a)
{
  return __builtin_aarch64_ld2v2di_us (
    (const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline float16x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2q_f16 (const float16_t * __a)
{
  return __builtin_aarch64_ld2v8hf (__a);
}
__extension__ extern __inline float32x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2q_f32 (const float32_t * __a)
{
  return __builtin_aarch64_ld2v4sf ((const __builtin_aarch64_simd_sf *) __a);
}
__extension__ extern __inline float64x2x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2q_f64 (const float64_t * __a)
{
  return __builtin_aarch64_ld2v2df ((const __builtin_aarch64_simd_df *) __a);
}
__extension__ extern __inline int64x1x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3_s64 (const int64_t * __a)
{
  return __builtin_aarch64_ld3di ((const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline uint64x1x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3_u64 (const uint64_t * __a)
{
  return __builtin_aarch64_ld3di_us ((const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline float64x1x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3_f64 (const float64_t * __a)
{
  return __builtin_aarch64_ld3df ((const __builtin_aarch64_simd_df *) __a);
}
__extension__ extern __inline int8x8x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3_s8 (const int8_t * __a)
{
  return __builtin_aarch64_ld3v8qi ((const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline poly8x8x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3_p8 (const poly8_t * __a)
{
  return __builtin_aarch64_ld3v8qi_ps (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline int16x4x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3_s16 (const int16_t * __a)
{
  return __builtin_aarch64_ld3v4hi ((const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline poly16x4x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3_p16 (const poly16_t * __a)
{
  return __builtin_aarch64_ld3v4hi_ps (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline int32x2x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3_s32 (const int32_t * __a)
{
  return __builtin_aarch64_ld3v2si ((const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline uint8x8x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3_u8 (const uint8_t * __a)
{
  return __builtin_aarch64_ld3v8qi_us (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline uint16x4x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3_u16 (const uint16_t * __a)
{
  return __builtin_aarch64_ld3v4hi_us (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline uint32x2x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3_u32 (const uint32_t * __a)
{
  return __builtin_aarch64_ld3v2si_us (
    (const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline float16x4x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3_f16 (const float16_t * __a)
{
  return __builtin_aarch64_ld3v4hf (__a);
}
__extension__ extern __inline float32x2x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3_f32 (const float32_t * __a)
{
  return __builtin_aarch64_ld3v2sf ((const __builtin_aarch64_simd_sf *) __a);
}
__extension__ extern __inline poly64x1x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3_p64 (const poly64_t * __a)
{
  return __builtin_aarch64_ld3di_ps ((const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline int8x16x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3q_s8 (const int8_t * __a)
{
  return __builtin_aarch64_ld3v16qi ((const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline poly8x16x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3q_p8 (const poly8_t * __a)
{
  return __builtin_aarch64_ld3v16qi_ps ((const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline int16x8x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3q_s16 (const int16_t * __a)
{
  return __builtin_aarch64_ld3v8hi ((const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline poly16x8x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3q_p16 (const poly16_t * __a)
{
  return __builtin_aarch64_ld3v8hi_ps ((const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline int32x4x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3q_s32 (const int32_t * __a)
{
  return __builtin_aarch64_ld3v4si ((const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline int64x2x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3q_s64 (const int64_t * __a)
{
  return __builtin_aarch64_ld3v2di ((const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline uint8x16x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3q_u8 (const uint8_t * __a)
{
  return __builtin_aarch64_ld3v16qi_us (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline uint16x8x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3q_u16 (const uint16_t * __a)
{
  return __builtin_aarch64_ld3v8hi_us (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline uint32x4x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3q_u32 (const uint32_t * __a)
{
  return __builtin_aarch64_ld3v4si_us (
    (const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline uint64x2x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3q_u64 (const uint64_t * __a)
{
  return __builtin_aarch64_ld3v2di_us (
    (const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline float16x8x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3q_f16 (const float16_t * __a)
{
  return __builtin_aarch64_ld3v8hf (__a);
}
__extension__ extern __inline float32x4x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3q_f32 (const float32_t * __a)
{
  return __builtin_aarch64_ld3v4sf ((const __builtin_aarch64_simd_sf *) __a);
}
__extension__ extern __inline float64x2x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3q_f64 (const float64_t * __a)
{
  return __builtin_aarch64_ld3v2df ((const __builtin_aarch64_simd_df *) __a);
}
__extension__ extern __inline poly64x2x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3q_p64 (const poly64_t * __a)
{
  return __builtin_aarch64_ld3v2di_ps (
    (const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline int64x1x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4_s64 (const int64_t * __a)
{
  return __builtin_aarch64_ld4di ((const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline uint64x1x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4_u64 (const uint64_t * __a)
{
  return __builtin_aarch64_ld4di_us ((const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline float64x1x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4_f64 (const float64_t * __a)
{
  return __builtin_aarch64_ld4df ((const __builtin_aarch64_simd_df *) __a);
}
__extension__ extern __inline int8x8x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4_s8 (const int8_t * __a)
{
  return __builtin_aarch64_ld4v8qi ((const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline poly8x8x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4_p8 (const poly8_t * __a)
{
  return __builtin_aarch64_ld4v8qi_ps (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline int16x4x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4_s16 (const int16_t * __a)
{
  return __builtin_aarch64_ld4v4hi ((const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline poly16x4x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4_p16 (const poly16_t * __a)
{
  return __builtin_aarch64_ld4v4hi_ps (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline int32x2x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4_s32 (const int32_t * __a)
{
  return __builtin_aarch64_ld4v2si ((const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline uint8x8x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4_u8 (const uint8_t * __a)
{
  return __builtin_aarch64_ld4v8qi_us (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline uint16x4x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4_u16 (const uint16_t * __a)
{
  return __builtin_aarch64_ld4v4hi_us (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline uint32x2x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4_u32 (const uint32_t * __a)
{
  return __builtin_aarch64_ld4v2si_us (
    (const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline float16x4x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4_f16 (const float16_t * __a)
{
  return __builtin_aarch64_ld4v4hf (__a);
}
__extension__ extern __inline float32x2x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4_f32 (const float32_t * __a)
{
  return __builtin_aarch64_ld4v2sf ((const __builtin_aarch64_simd_sf *) __a);
}
__extension__ extern __inline poly64x1x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4_p64 (const poly64_t * __a)
{
  return __builtin_aarch64_ld4di_ps ((const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline int8x16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4q_s8 (const int8_t * __a)
{
  return __builtin_aarch64_ld4v16qi ((const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline poly8x16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4q_p8 (const poly8_t * __a)
{
  return __builtin_aarch64_ld4v16qi_ps (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline int16x8x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4q_s16 (const int16_t * __a)
{
  return __builtin_aarch64_ld4v8hi ((const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline poly16x8x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4q_p16 (const poly16_t * __a)
{
  return __builtin_aarch64_ld4v8hi_ps (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline int32x4x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4q_s32 (const int32_t * __a)
{
  return __builtin_aarch64_ld4v4si ((const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline int64x2x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4q_s64 (const int64_t * __a)
{
  return __builtin_aarch64_ld4v2di ((const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline uint8x16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4q_u8 (const uint8_t * __a)
{
  return __builtin_aarch64_ld4v16qi_us (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline uint16x8x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4q_u16 (const uint16_t * __a)
{
  return __builtin_aarch64_ld4v8hi_us (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline uint32x4x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4q_u32 (const uint32_t * __a)
{
  return __builtin_aarch64_ld4v4si_us (
    (const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline uint64x2x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4q_u64 (const uint64_t * __a)
{
  return __builtin_aarch64_ld4v2di_us (
    (const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline float16x8x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4q_f16 (const float16_t * __a)
{
  return __builtin_aarch64_ld4v8hf (__a);
}
__extension__ extern __inline float32x4x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4q_f32 (const float32_t * __a)
{
  return __builtin_aarch64_ld4v4sf ((const __builtin_aarch64_simd_sf *) __a);
}
__extension__ extern __inline float64x2x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4q_f64 (const float64_t * __a)
{
  return __builtin_aarch64_ld4v2df ((const __builtin_aarch64_simd_df *) __a);
}
__extension__ extern __inline poly64x2x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4q_p64 (const poly64_t * __a)
{
  return __builtin_aarch64_ld4v2di_ps (
    (const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline poly128_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vldrq_p128 (const poly128_t * __ptr)
{
  return *__ptr;
}
__extension__ extern __inline int8x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2_dup_s8 (const int8_t * __a)
{
  return __builtin_aarch64_ld2rv8qi ((const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline int16x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2_dup_s16 (const int16_t * __a)
{
  return __builtin_aarch64_ld2rv4hi ((const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline int32x2x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2_dup_s32 (const int32_t * __a)
{
  return __builtin_aarch64_ld2rv2si ((const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline float16x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2_dup_f16 (const float16_t * __a)
{
  return __builtin_aarch64_ld2rv4hf ((const __builtin_aarch64_simd_hf *) __a);
}
__extension__ extern __inline float32x2x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2_dup_f32 (const float32_t * __a)
{
  return __builtin_aarch64_ld2rv2sf ((const __builtin_aarch64_simd_sf *) __a);
}
__extension__ extern __inline float64x1x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2_dup_f64 (const float64_t * __a)
{
  return __builtin_aarch64_ld2rdf ((const __builtin_aarch64_simd_df *) __a);
}
__extension__ extern __inline uint8x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2_dup_u8 (const uint8_t * __a)
{
  return __builtin_aarch64_ld2rv8qi_us (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline uint16x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2_dup_u16 (const uint16_t * __a)
{
  return __builtin_aarch64_ld2rv4hi_us (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline uint32x2x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2_dup_u32 (const uint32_t * __a)
{
  return __builtin_aarch64_ld2rv2si_us (
    (const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline poly8x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2_dup_p8 (const poly8_t * __a)
{
  return __builtin_aarch64_ld2rv8qi_ps (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline poly16x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2_dup_p16 (const poly16_t * __a)
{
  return __builtin_aarch64_ld2rv4hi_ps (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline poly64x1x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2_dup_p64 (const poly64_t * __a)
{
  return __builtin_aarch64_ld2rdi_ps ((const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline int64x1x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2_dup_s64 (const int64_t * __a)
{
  return __builtin_aarch64_ld2rdi ((const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline uint64x1x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2_dup_u64 (const uint64_t * __a)
{
  return __builtin_aarch64_ld2rdi_us ((const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline int8x16x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2q_dup_s8 (const int8_t * __a)
{
  return __builtin_aarch64_ld2rv16qi ((const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline poly8x16x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2q_dup_p8 (const poly8_t * __a)
{
  return __builtin_aarch64_ld2rv16qi_ps (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline int16x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2q_dup_s16 (const int16_t * __a)
{
  return __builtin_aarch64_ld2rv8hi ((const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline poly16x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2q_dup_p16 (const poly16_t * __a)
{
  return __builtin_aarch64_ld2rv8hi_ps (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline int32x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2q_dup_s32 (const int32_t * __a)
{
  return __builtin_aarch64_ld2rv4si ((const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline int64x2x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2q_dup_s64 (const int64_t * __a)
{
  return __builtin_aarch64_ld2rv2di ((const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline uint8x16x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2q_dup_u8 (const uint8_t * __a)
{
  return __builtin_aarch64_ld2rv16qi_us (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline uint16x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2q_dup_u16 (const uint16_t * __a)
{
  return __builtin_aarch64_ld2rv8hi_us (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline uint32x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2q_dup_u32 (const uint32_t * __a)
{
  return __builtin_aarch64_ld2rv4si_us (
    (const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline uint64x2x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2q_dup_u64 (const uint64_t * __a)
{
  return __builtin_aarch64_ld2rv2di_us (
    (const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline float16x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2q_dup_f16 (const float16_t * __a)
{
  return __builtin_aarch64_ld2rv8hf ((const __builtin_aarch64_simd_hf *) __a);
}
__extension__ extern __inline float32x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2q_dup_f32 (const float32_t * __a)
{
  return __builtin_aarch64_ld2rv4sf ((const __builtin_aarch64_simd_sf *) __a);
}
__extension__ extern __inline float64x2x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2q_dup_f64 (const float64_t * __a)
{
  return __builtin_aarch64_ld2rv2df ((const __builtin_aarch64_simd_df *) __a);
}
__extension__ extern __inline poly64x2x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2q_dup_p64 (const poly64_t * __a)
{
  return __builtin_aarch64_ld2rv2di_ps (
    (const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline int64x1x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3_dup_s64 (const int64_t * __a)
{
  return __builtin_aarch64_ld3rdi ((const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline uint64x1x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3_dup_u64 (const uint64_t * __a)
{
  return __builtin_aarch64_ld3rdi_us ((const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline float64x1x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3_dup_f64 (const float64_t * __a)
{
  return __builtin_aarch64_ld3rdf ((const __builtin_aarch64_simd_df *) __a);
}
__extension__ extern __inline int8x8x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3_dup_s8 (const int8_t * __a)
{
  return __builtin_aarch64_ld3rv8qi ((const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline poly8x8x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3_dup_p8 (const poly8_t * __a)
{
  return __builtin_aarch64_ld3rv8qi_ps (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline int16x4x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3_dup_s16 (const int16_t * __a)
{
  return __builtin_aarch64_ld3rv4hi ((const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline poly16x4x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3_dup_p16 (const poly16_t * __a)
{
  return __builtin_aarch64_ld3rv4hi_ps (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline int32x2x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3_dup_s32 (const int32_t * __a)
{
  return __builtin_aarch64_ld3rv2si ((const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline uint8x8x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3_dup_u8 (const uint8_t * __a)
{
  return __builtin_aarch64_ld3rv8qi_us (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline uint16x4x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3_dup_u16 (const uint16_t * __a)
{
  return __builtin_aarch64_ld3rv4hi_us (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline uint32x2x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3_dup_u32 (const uint32_t * __a)
{
  return __builtin_aarch64_ld3rv2si_us (
    (const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline float16x4x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3_dup_f16 (const float16_t * __a)
{
  return __builtin_aarch64_ld3rv4hf ((const __builtin_aarch64_simd_hf *) __a);
}
__extension__ extern __inline float32x2x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3_dup_f32 (const float32_t * __a)
{
  return __builtin_aarch64_ld3rv2sf ((const __builtin_aarch64_simd_sf *) __a);
}
__extension__ extern __inline poly64x1x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3_dup_p64 (const poly64_t * __a)
{
  return __builtin_aarch64_ld3rdi_ps ((const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline int8x16x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3q_dup_s8 (const int8_t * __a)
{
  return __builtin_aarch64_ld3rv16qi ((const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline poly8x16x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3q_dup_p8 (const poly8_t * __a)
{
  return __builtin_aarch64_ld3rv16qi_ps (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline int16x8x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3q_dup_s16 (const int16_t * __a)
{
  return __builtin_aarch64_ld3rv8hi ((const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline poly16x8x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3q_dup_p16 (const poly16_t * __a)
{
  return __builtin_aarch64_ld3rv8hi_ps (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline int32x4x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3q_dup_s32 (const int32_t * __a)
{
  return __builtin_aarch64_ld3rv4si ((const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline int64x2x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3q_dup_s64 (const int64_t * __a)
{
  return __builtin_aarch64_ld3rv2di ((const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline uint8x16x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3q_dup_u8 (const uint8_t * __a)
{
  return __builtin_aarch64_ld3rv16qi_us (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline uint16x8x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3q_dup_u16 (const uint16_t * __a)
{
  return __builtin_aarch64_ld3rv8hi_us (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline uint32x4x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3q_dup_u32 (const uint32_t * __a)
{
  return __builtin_aarch64_ld3rv4si_us (
    (const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline uint64x2x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3q_dup_u64 (const uint64_t * __a)
{
  return __builtin_aarch64_ld3rv2di_us (
    (const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline float16x8x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3q_dup_f16 (const float16_t * __a)
{
  return __builtin_aarch64_ld3rv8hf ((const __builtin_aarch64_simd_hf *) __a);
}
__extension__ extern __inline float32x4x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3q_dup_f32 (const float32_t * __a)
{
  return __builtin_aarch64_ld3rv4sf ((const __builtin_aarch64_simd_sf *) __a);
}
__extension__ extern __inline float64x2x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3q_dup_f64 (const float64_t * __a)
{
  return __builtin_aarch64_ld3rv2df ((const __builtin_aarch64_simd_df *) __a);
}
__extension__ extern __inline poly64x2x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3q_dup_p64 (const poly64_t * __a)
{
  return __builtin_aarch64_ld3rv2di_ps (
    (const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline int64x1x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4_dup_s64 (const int64_t * __a)
{
  return __builtin_aarch64_ld4rdi ((const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline uint64x1x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4_dup_u64 (const uint64_t * __a)
{
  return __builtin_aarch64_ld4rdi_us ((const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline float64x1x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4_dup_f64 (const float64_t * __a)
{
  return __builtin_aarch64_ld4rdf ((const __builtin_aarch64_simd_df *) __a);
}
__extension__ extern __inline int8x8x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4_dup_s8 (const int8_t * __a)
{
  return __builtin_aarch64_ld4rv8qi ((const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline poly8x8x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4_dup_p8 (const poly8_t * __a)
{
  return __builtin_aarch64_ld4rv8qi_ps (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline int16x4x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4_dup_s16 (const int16_t * __a)
{
  return __builtin_aarch64_ld4rv4hi ((const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline poly16x4x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4_dup_p16 (const poly16_t * __a)
{
  return __builtin_aarch64_ld4rv4hi_ps (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline int32x2x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4_dup_s32 (const int32_t * __a)
{
  return __builtin_aarch64_ld4rv2si ((const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline uint8x8x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4_dup_u8 (const uint8_t * __a)
{
  return __builtin_aarch64_ld4rv8qi_us (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline uint16x4x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4_dup_u16 (const uint16_t * __a)
{
  return __builtin_aarch64_ld4rv4hi_us (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline uint32x2x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4_dup_u32 (const uint32_t * __a)
{
  return __builtin_aarch64_ld4rv2si_us (
    (const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline float16x4x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4_dup_f16 (const float16_t * __a)
{
  return __builtin_aarch64_ld4rv4hf ((const __builtin_aarch64_simd_hf *) __a);
}
__extension__ extern __inline float32x2x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4_dup_f32 (const float32_t * __a)
{
  return __builtin_aarch64_ld4rv2sf ((const __builtin_aarch64_simd_sf *) __a);
}
__extension__ extern __inline poly64x1x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4_dup_p64 (const poly64_t * __a)
{
  return __builtin_aarch64_ld4rdi_ps ((const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline int8x16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4q_dup_s8 (const int8_t * __a)
{
  return __builtin_aarch64_ld4rv16qi ((const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline poly8x16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4q_dup_p8 (const poly8_t * __a)
{
  return __builtin_aarch64_ld4rv16qi_ps (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline int16x8x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4q_dup_s16 (const int16_t * __a)
{
  return __builtin_aarch64_ld4rv8hi ((const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline poly16x8x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4q_dup_p16 (const poly16_t * __a)
{
  return __builtin_aarch64_ld4rv8hi_ps (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline int32x4x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4q_dup_s32 (const int32_t * __a)
{
  return __builtin_aarch64_ld4rv4si ((const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline int64x2x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4q_dup_s64 (const int64_t * __a)
{
  return __builtin_aarch64_ld4rv2di ((const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline uint8x16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4q_dup_u8 (const uint8_t * __a)
{
  return __builtin_aarch64_ld4rv16qi_us (
    (const __builtin_aarch64_simd_qi *) __a);
}
__extension__ extern __inline uint16x8x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4q_dup_u16 (const uint16_t * __a)
{
  return __builtin_aarch64_ld4rv8hi_us (
    (const __builtin_aarch64_simd_hi *) __a);
}
__extension__ extern __inline uint32x4x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4q_dup_u32 (const uint32_t * __a)
{
  return __builtin_aarch64_ld4rv4si_us (
    (const __builtin_aarch64_simd_si *) __a);
}
__extension__ extern __inline uint64x2x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4q_dup_u64 (const uint64_t * __a)
{
  return __builtin_aarch64_ld4rv2di_us (
    (const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline float16x8x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4q_dup_f16 (const float16_t * __a)
{
  return __builtin_aarch64_ld4rv8hf ((const __builtin_aarch64_simd_hf *) __a);
}
__extension__ extern __inline float32x4x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4q_dup_f32 (const float32_t * __a)
{
  return __builtin_aarch64_ld4rv4sf ((const __builtin_aarch64_simd_sf *) __a);
}
__extension__ extern __inline float64x2x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4q_dup_f64 (const float64_t * __a)
{
  return __builtin_aarch64_ld4rv2df ((const __builtin_aarch64_simd_df *) __a);
}
__extension__ extern __inline poly64x2x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4q_dup_p64 (const poly64_t * __a)
{
  return __builtin_aarch64_ld4rv2di_ps (
    (const __builtin_aarch64_simd_di *) __a);
}
__extension__ extern __inline uint8x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld2_lane_u8 (const uint8_t * __ptr, uint8x8x2_t __b, const int __c)
{
  return __builtin_aarch64_ld2_lanev8qi_usus (
   (__builtin_aarch64_simd_qi *) __ptr, __b, __c);
}
__extension__ extern __inline uint16x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld2_lane_u16 (const uint16_t * __ptr, uint16x4x2_t __b, const int __c)
{
  return __builtin_aarch64_ld2_lanev4hi_usus (
   (__builtin_aarch64_simd_hi *) __ptr, __b, __c);
}
__extension__ extern __inline uint32x2x2_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld2_lane_u32 (const uint32_t * __ptr, uint32x2x2_t __b, const int __c)
{
  return __builtin_aarch64_ld2_lanev2si_usus (
   (__builtin_aarch64_simd_si *) __ptr, __b, __c);
}
__extension__ extern __inline uint64x1x2_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld2_lane_u64 (const uint64_t * __ptr, uint64x1x2_t __b, const int __c)
{
  return __builtin_aarch64_ld2_lanedi_usus (
   (__builtin_aarch64_simd_di *) __ptr, __b, __c);
}
__extension__ extern __inline int8x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld2_lane_s8 (const int8_t * __ptr, int8x8x2_t __b, const int __c)
{
  return __builtin_aarch64_ld2_lanev8qi (
   (__builtin_aarch64_simd_qi *) __ptr, __b, __c);
}
__extension__ extern __inline int16x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld2_lane_s16 (const int16_t * __ptr, int16x4x2_t __b, const int __c)
{
  return __builtin_aarch64_ld2_lanev4hi (
   (__builtin_aarch64_simd_hi *) __ptr, __b, __c);
}
__extension__ extern __inline int32x2x2_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld2_lane_s32 (const int32_t * __ptr, int32x2x2_t __b, const int __c)
{
  return __builtin_aarch64_ld2_lanev2si (
   (__builtin_aarch64_simd_si *) __ptr, __b, __c);
}
__extension__ extern __inline int64x1x2_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld2_lane_s64 (const int64_t * __ptr, int64x1x2_t __b, const int __c)
{
  return __builtin_aarch64_ld2_lanedi (
   (__builtin_aarch64_simd_di *) __ptr, __b, __c);
}
__extension__ extern __inline float16x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld2_lane_f16 (const float16_t * __ptr, float16x4x2_t __b, const int __c)
{
  return __builtin_aarch64_ld2_lanev4hf (
   (__builtin_aarch64_simd_hf *) __ptr, __b, __c);
}
__extension__ extern __inline float32x2x2_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld2_lane_f32 (const float32_t * __ptr, float32x2x2_t __b, const int __c)
{
  return __builtin_aarch64_ld2_lanev2sf (
   (__builtin_aarch64_simd_sf *) __ptr, __b, __c);
}
__extension__ extern __inline float64x1x2_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld2_lane_f64 (const float64_t * __ptr, float64x1x2_t __b, const int __c)
{
  return __builtin_aarch64_ld2_lanedf (
   (__builtin_aarch64_simd_df *) __ptr, __b, __c);
}
__extension__ extern __inline poly8x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld2_lane_p8 (const poly8_t * __ptr, poly8x8x2_t __b, const int __c)
{
  return __builtin_aarch64_ld2_lanev8qi_psps (
   (__builtin_aarch64_simd_qi *) __ptr, __b, __c);
}
__extension__ extern __inline poly16x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld2_lane_p16 (const poly16_t * __ptr, poly16x4x2_t __b, const int __c)
{
  return __builtin_aarch64_ld2_lanev4hi_psps (
   (__builtin_aarch64_simd_hi *) __ptr, __b, __c);
}
__extension__ extern __inline poly64x1x2_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld2_lane_p64 (const poly64_t * __ptr, poly64x1x2_t __b, const int __c)
{
  return __builtin_aarch64_ld2_lanedi_psps (
   (__builtin_aarch64_simd_di *) __ptr, __b, __c);
}
__extension__ extern __inline uint8x16x2_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld2q_lane_u8 (const uint8_t * __ptr, uint8x16x2_t __b, const int __c)
{
  return __builtin_aarch64_ld2_lanev16qi_usus (
 (__builtin_aarch64_simd_qi *) __ptr, __b, __c);
}
__extension__ extern __inline uint16x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld2q_lane_u16 (const uint16_t * __ptr, uint16x8x2_t __b, const int __c)
{
  return __builtin_aarch64_ld2_lanev8hi_usus (
 (__builtin_aarch64_simd_hi *) __ptr, __b, __c);
}
__extension__ extern __inline uint32x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld2q_lane_u32 (const uint32_t * __ptr, uint32x4x2_t __b, const int __c)
{
  return __builtin_aarch64_ld2_lanev4si_usus (
 (__builtin_aarch64_simd_si *) __ptr, __b, __c);
}
__extension__ extern __inline uint64x2x2_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld2q_lane_u64 (const uint64_t * __ptr, uint64x2x2_t __b, const int __c)
{
  return __builtin_aarch64_ld2_lanev2di_usus (
 (__builtin_aarch64_simd_di *) __ptr, __b, __c);
}
__extension__ extern __inline int8x16x2_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld2q_lane_s8 (const int8_t * __ptr, int8x16x2_t __b, const int __c)
{
  return __builtin_aarch64_ld2_lanev16qi (
 (__builtin_aarch64_simd_qi *) __ptr, __b, __c);
}
__extension__ extern __inline int16x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld2q_lane_s16 (const int16_t * __ptr, int16x8x2_t __b, const int __c)
{
  return __builtin_aarch64_ld2_lanev8hi (
 (__builtin_aarch64_simd_hi *) __ptr, __b, __c);
}
__extension__ extern __inline int32x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld2q_lane_s32 (const int32_t * __ptr, int32x4x2_t __b, const int __c)
{
  return __builtin_aarch64_ld2_lanev4si (
 (__builtin_aarch64_simd_si *) __ptr, __b, __c);
}
__extension__ extern __inline int64x2x2_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld2q_lane_s64 (const int64_t * __ptr, int64x2x2_t __b, const int __c)
{
  return __builtin_aarch64_ld2_lanev2di (
 (__builtin_aarch64_simd_di *) __ptr, __b, __c);
}
__extension__ extern __inline float16x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld2q_lane_f16 (const float16_t * __ptr, float16x8x2_t __b, const int __c)
{
  return __builtin_aarch64_ld2_lanev8hf (
 (__builtin_aarch64_simd_hf *) __ptr, __b, __c);
}
__extension__ extern __inline float32x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld2q_lane_f32 (const float32_t * __ptr, float32x4x2_t __b, const int __c)
{
  return __builtin_aarch64_ld2_lanev4sf (
 (__builtin_aarch64_simd_sf *) __ptr, __b, __c);
}
__extension__ extern __inline float64x2x2_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld2q_lane_f64 (const float64_t * __ptr, float64x2x2_t __b, const int __c)
{
  return __builtin_aarch64_ld2_lanev2df (
 (__builtin_aarch64_simd_df *) __ptr, __b, __c);
}
__extension__ extern __inline poly8x16x2_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld2q_lane_p8 (const poly8_t * __ptr, poly8x16x2_t __b, const int __c)
{
  return __builtin_aarch64_ld2_lanev16qi_psps (
 (__builtin_aarch64_simd_qi *) __ptr, __b, __c);
}
__extension__ extern __inline poly16x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld2q_lane_p16 (const poly16_t * __ptr, poly16x8x2_t __b, const int __c)
{
  return __builtin_aarch64_ld2_lanev8hi_psps (
 (__builtin_aarch64_simd_hi *) __ptr, __b, __c);
}
__extension__ extern __inline poly64x2x2_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld2q_lane_p64 (const poly64_t * __ptr, poly64x2x2_t __b, const int __c)
{
  return __builtin_aarch64_ld2_lanev2di_psps (
 (__builtin_aarch64_simd_di *) __ptr, __b, __c);
}
__extension__ extern __inline uint8x8x3_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld3_lane_u8 (const uint8_t * __ptr, uint8x8x3_t __b, const int __c)
{
  return __builtin_aarch64_ld3_lanev8qi_usus (
   (__builtin_aarch64_simd_qi *) __ptr, __b, __c);
}
__extension__ extern __inline uint16x4x3_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld3_lane_u16 (const uint16_t * __ptr, uint16x4x3_t __b, const int __c)
{
  return __builtin_aarch64_ld3_lanev4hi_usus (
   (__builtin_aarch64_simd_hi *) __ptr, __b, __c);
}
__extension__ extern __inline uint32x2x3_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld3_lane_u32 (const uint32_t * __ptr, uint32x2x3_t __b, const int __c)
{
  return __builtin_aarch64_ld3_lanev2si_usus (
   (__builtin_aarch64_simd_si *) __ptr, __b, __c);
}
__extension__ extern __inline uint64x1x3_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld3_lane_u64 (const uint64_t * __ptr, uint64x1x3_t __b, const int __c)
{
  return __builtin_aarch64_ld3_lanedi_usus (
   (__builtin_aarch64_simd_di *) __ptr, __b, __c);
}
__extension__ extern __inline int8x8x3_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld3_lane_s8 (const int8_t * __ptr, int8x8x3_t __b, const int __c)
{
  return __builtin_aarch64_ld3_lanev8qi (
   (__builtin_aarch64_simd_qi *) __ptr, __b, __c);
}
__extension__ extern __inline int16x4x3_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld3_lane_s16 (const int16_t * __ptr, int16x4x3_t __b, const int __c)
{
  return __builtin_aarch64_ld3_lanev4hi (
   (__builtin_aarch64_simd_hi *) __ptr, __b, __c);
}
__extension__ extern __inline int32x2x3_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld3_lane_s32 (const int32_t * __ptr, int32x2x3_t __b, const int __c)
{
  return __builtin_aarch64_ld3_lanev2si (
   (__builtin_aarch64_simd_si *) __ptr, __b, __c);
}
__extension__ extern __inline int64x1x3_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld3_lane_s64 (const int64_t * __ptr, int64x1x3_t __b, const int __c)
{
  return __builtin_aarch64_ld3_lanedi (
   (__builtin_aarch64_simd_di *) __ptr, __b, __c);
}
__extension__ extern __inline float16x4x3_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld3_lane_f16 (const float16_t * __ptr, float16x4x3_t __b, const int __c)
{
  return __builtin_aarch64_ld3_lanev4hf (
   (__builtin_aarch64_simd_hf *) __ptr, __b, __c);
}
__extension__ extern __inline float32x2x3_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld3_lane_f32 (const float32_t * __ptr, float32x2x3_t __b, const int __c)
{
  return __builtin_aarch64_ld3_lanev2sf (
   (__builtin_aarch64_simd_sf *) __ptr, __b, __c);
}
__extension__ extern __inline float64x1x3_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld3_lane_f64 (const float64_t * __ptr, float64x1x3_t __b, const int __c)
{
  return __builtin_aarch64_ld3_lanedf (
   (__builtin_aarch64_simd_df *) __ptr, __b, __c);
}
__extension__ extern __inline poly8x8x3_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld3_lane_p8 (const poly8_t * __ptr, poly8x8x3_t __b, const int __c)
{
  return __builtin_aarch64_ld3_lanev8qi_psps (
   (__builtin_aarch64_simd_qi *) __ptr, __b, __c);
}
__extension__ extern __inline poly16x4x3_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld3_lane_p16 (const poly16_t * __ptr, poly16x4x3_t __b, const int __c)
{
  return __builtin_aarch64_ld3_lanev4hi_psps (
   (__builtin_aarch64_simd_hi *) __ptr, __b, __c);
}
__extension__ extern __inline poly64x1x3_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld3_lane_p64 (const poly64_t * __ptr, poly64x1x3_t __b, const int __c)
{
  return __builtin_aarch64_ld3_lanedi_psps (
   (__builtin_aarch64_simd_di *) __ptr, __b, __c);
}
__extension__ extern __inline uint8x16x3_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld3q_lane_u8 (const uint8_t * __ptr, uint8x16x3_t __b, const int __c)
{
  return __builtin_aarch64_ld3_lanev16qi_usus (
 (__builtin_aarch64_simd_qi *) __ptr, __b, __c);
}
__extension__ extern __inline uint16x8x3_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld3q_lane_u16 (const uint16_t * __ptr, uint16x8x3_t __b, const int __c)
{
  return __builtin_aarch64_ld3_lanev8hi_usus (
 (__builtin_aarch64_simd_hi *) __ptr, __b, __c);
}
__extension__ extern __inline uint32x4x3_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld3q_lane_u32 (const uint32_t * __ptr, uint32x4x3_t __b, const int __c)
{
  return __builtin_aarch64_ld3_lanev4si_usus (
 (__builtin_aarch64_simd_si *) __ptr, __b, __c);
}
__extension__ extern __inline uint64x2x3_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld3q_lane_u64 (const uint64_t * __ptr, uint64x2x3_t __b, const int __c)
{
  return __builtin_aarch64_ld3_lanev2di_usus (
 (__builtin_aarch64_simd_di *) __ptr, __b, __c);
}
__extension__ extern __inline int8x16x3_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld3q_lane_s8 (const int8_t * __ptr, int8x16x3_t __b, const int __c)
{
  return __builtin_aarch64_ld3_lanev16qi (
 (__builtin_aarch64_simd_qi *) __ptr, __b, __c);
}
__extension__ extern __inline int16x8x3_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld3q_lane_s16 (const int16_t * __ptr, int16x8x3_t __b, const int __c)
{
  return __builtin_aarch64_ld3_lanev8hi (
 (__builtin_aarch64_simd_hi *) __ptr, __b, __c);
}
__extension__ extern __inline int32x4x3_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld3q_lane_s32 (const int32_t * __ptr, int32x4x3_t __b, const int __c)
{
  return __builtin_aarch64_ld3_lanev4si (
 (__builtin_aarch64_simd_si *) __ptr, __b, __c);
}
__extension__ extern __inline int64x2x3_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld3q_lane_s64 (const int64_t * __ptr, int64x2x3_t __b, const int __c)
{
  return __builtin_aarch64_ld3_lanev2di (
 (__builtin_aarch64_simd_di *) __ptr, __b, __c);
}
__extension__ extern __inline float16x8x3_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld3q_lane_f16 (const float16_t * __ptr, float16x8x3_t __b, const int __c)
{
  return __builtin_aarch64_ld3_lanev8hf (
 (__builtin_aarch64_simd_hf *) __ptr, __b, __c);
}
__extension__ extern __inline float32x4x3_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld3q_lane_f32 (const float32_t * __ptr, float32x4x3_t __b, const int __c)
{
  return __builtin_aarch64_ld3_lanev4sf (
 (__builtin_aarch64_simd_sf *) __ptr, __b, __c);
}
__extension__ extern __inline float64x2x3_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld3q_lane_f64 (const float64_t * __ptr, float64x2x3_t __b, const int __c)
{
  return __builtin_aarch64_ld3_lanev2df (
 (__builtin_aarch64_simd_df *) __ptr, __b, __c);
}
__extension__ extern __inline poly8x16x3_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld3q_lane_p8 (const poly8_t * __ptr, poly8x16x3_t __b, const int __c)
{
  return __builtin_aarch64_ld3_lanev16qi_psps (
 (__builtin_aarch64_simd_qi *) __ptr, __b, __c);
}
__extension__ extern __inline poly16x8x3_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld3q_lane_p16 (const poly16_t * __ptr, poly16x8x3_t __b, const int __c)
{
  return __builtin_aarch64_ld3_lanev8hi_psps (
 (__builtin_aarch64_simd_hi *) __ptr, __b, __c);
}
__extension__ extern __inline poly64x2x3_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld3q_lane_p64 (const poly64_t * __ptr, poly64x2x3_t __b, const int __c)
{
  return __builtin_aarch64_ld3_lanev2di_psps (
 (__builtin_aarch64_simd_di *) __ptr, __b, __c);
}
__extension__ extern __inline uint8x8x4_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld4_lane_u8 (const uint8_t * __ptr, uint8x8x4_t __b, const int __c)
{
  return __builtin_aarch64_ld4_lanev8qi_usus (
   (__builtin_aarch64_simd_qi *) __ptr, __b, __c);
}
__extension__ extern __inline uint16x4x4_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld4_lane_u16 (const uint16_t * __ptr, uint16x4x4_t __b, const int __c)
{
  return __builtin_aarch64_ld4_lanev4hi_usus (
   (__builtin_aarch64_simd_hi *) __ptr, __b, __c);
}
__extension__ extern __inline uint32x2x4_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld4_lane_u32 (const uint32_t * __ptr, uint32x2x4_t __b, const int __c)
{
  return __builtin_aarch64_ld4_lanev2si_usus (
   (__builtin_aarch64_simd_si *) __ptr, __b, __c);
}
__extension__ extern __inline uint64x1x4_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld4_lane_u64 (const uint64_t * __ptr, uint64x1x4_t __b, const int __c)
{
  return __builtin_aarch64_ld4_lanedi_usus (
   (__builtin_aarch64_simd_di *) __ptr, __b, __c);
}
__extension__ extern __inline int8x8x4_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld4_lane_s8 (const int8_t * __ptr, int8x8x4_t __b, const int __c)
{
  return __builtin_aarch64_ld4_lanev8qi (
   (__builtin_aarch64_simd_qi *) __ptr, __b, __c);
}
__extension__ extern __inline int16x4x4_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld4_lane_s16 (const int16_t * __ptr, int16x4x4_t __b, const int __c)
{
  return __builtin_aarch64_ld4_lanev4hi (
   (__builtin_aarch64_simd_hi *) __ptr, __b, __c);
}
__extension__ extern __inline int32x2x4_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld4_lane_s32 (const int32_t * __ptr, int32x2x4_t __b, const int __c)
{
  return __builtin_aarch64_ld4_lanev2si (
   (__builtin_aarch64_simd_si *) __ptr, __b, __c);
}
__extension__ extern __inline int64x1x4_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld4_lane_s64 (const int64_t * __ptr, int64x1x4_t __b, const int __c)
{
  return __builtin_aarch64_ld4_lanedi (
   (__builtin_aarch64_simd_di *) __ptr, __b, __c);
}
__extension__ extern __inline float16x4x4_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld4_lane_f16 (const float16_t * __ptr, float16x4x4_t __b, const int __c)
{
  return __builtin_aarch64_ld4_lanev4hf (
   (__builtin_aarch64_simd_hf *) __ptr, __b, __c);
}
__extension__ extern __inline float32x2x4_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld4_lane_f32 (const float32_t * __ptr, float32x2x4_t __b, const int __c)
{
  return __builtin_aarch64_ld4_lanev2sf (
   (__builtin_aarch64_simd_sf *) __ptr, __b, __c);
}
__extension__ extern __inline float64x1x4_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld4_lane_f64 (const float64_t * __ptr, float64x1x4_t __b, const int __c)
{
  return __builtin_aarch64_ld4_lanedf (
   (__builtin_aarch64_simd_df *) __ptr, __b, __c);
}
__extension__ extern __inline poly8x8x4_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld4_lane_p8 (const poly8_t * __ptr, poly8x8x4_t __b, const int __c)
{
  return __builtin_aarch64_ld4_lanev8qi_psps (
   (__builtin_aarch64_simd_qi *) __ptr, __b, __c);
}
__extension__ extern __inline poly16x4x4_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld4_lane_p16 (const poly16_t * __ptr, poly16x4x4_t __b, const int __c)
{
  return __builtin_aarch64_ld4_lanev4hi_psps (
   (__builtin_aarch64_simd_hi *) __ptr, __b, __c);
}
__extension__ extern __inline poly64x1x4_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld4_lane_p64 (const poly64_t * __ptr, poly64x1x4_t __b, const int __c)
{
  return __builtin_aarch64_ld4_lanedi_psps (
   (__builtin_aarch64_simd_di *) __ptr, __b, __c);
}
__extension__ extern __inline uint8x16x4_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld4q_lane_u8 (const uint8_t * __ptr, uint8x16x4_t __b, const int __c)
{
  return __builtin_aarch64_ld4_lanev16qi_usus (
 (__builtin_aarch64_simd_qi *) __ptr, __b, __c);
}
__extension__ extern __inline uint16x8x4_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld4q_lane_u16 (const uint16_t * __ptr, uint16x8x4_t __b, const int __c)
{
  return __builtin_aarch64_ld4_lanev8hi_usus (
 (__builtin_aarch64_simd_hi *) __ptr, __b, __c);
}
__extension__ extern __inline uint32x4x4_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld4q_lane_u32 (const uint32_t * __ptr, uint32x4x4_t __b, const int __c)
{
  return __builtin_aarch64_ld4_lanev4si_usus (
 (__builtin_aarch64_simd_si *) __ptr, __b, __c);
}
__extension__ extern __inline uint64x2x4_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld4q_lane_u64 (const uint64_t * __ptr, uint64x2x4_t __b, const int __c)
{
  return __builtin_aarch64_ld4_lanev2di_usus (
 (__builtin_aarch64_simd_di *) __ptr, __b, __c);
}
__extension__ extern __inline int8x16x4_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld4q_lane_s8 (const int8_t * __ptr, int8x16x4_t __b, const int __c)
{
  return __builtin_aarch64_ld4_lanev16qi (
 (__builtin_aarch64_simd_qi *) __ptr, __b, __c);
}
__extension__ extern __inline int16x8x4_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld4q_lane_s16 (const int16_t * __ptr, int16x8x4_t __b, const int __c)
{
  return __builtin_aarch64_ld4_lanev8hi (
 (__builtin_aarch64_simd_hi *) __ptr, __b, __c);
}
__extension__ extern __inline int32x4x4_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld4q_lane_s32 (const int32_t * __ptr, int32x4x4_t __b, const int __c)
{
  return __builtin_aarch64_ld4_lanev4si (
 (__builtin_aarch64_simd_si *) __ptr, __b, __c);
}
__extension__ extern __inline int64x2x4_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld4q_lane_s64 (const int64_t * __ptr, int64x2x4_t __b, const int __c)
{
  return __builtin_aarch64_ld4_lanev2di (
 (__builtin_aarch64_simd_di *) __ptr, __b, __c);
}
__extension__ extern __inline float16x8x4_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld4q_lane_f16 (const float16_t * __ptr, float16x8x4_t __b, const int __c)
{
  return __builtin_aarch64_ld4_lanev8hf (
 (__builtin_aarch64_simd_hf *) __ptr, __b, __c);
}
__extension__ extern __inline float32x4x4_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld4q_lane_f32 (const float32_t * __ptr, float32x4x4_t __b, const int __c)
{
  return __builtin_aarch64_ld4_lanev4sf (
 (__builtin_aarch64_simd_sf *) __ptr, __b, __c);
}
__extension__ extern __inline float64x2x4_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld4q_lane_f64 (const float64_t * __ptr, float64x2x4_t __b, const int __c)
{
  return __builtin_aarch64_ld4_lanev2df (
 (__builtin_aarch64_simd_df *) __ptr, __b, __c);
}
__extension__ extern __inline poly8x16x4_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld4q_lane_p8 (const poly8_t * __ptr, poly8x16x4_t __b, const int __c)
{
  return __builtin_aarch64_ld4_lanev16qi_psps (
 (__builtin_aarch64_simd_qi *) __ptr, __b, __c);
}
__extension__ extern __inline poly16x8x4_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld4q_lane_p16 (const poly16_t * __ptr, poly16x8x4_t __b, const int __c)
{
  return __builtin_aarch64_ld4_lanev8hi_psps (
 (__builtin_aarch64_simd_hi *) __ptr, __b, __c);
}
__extension__ extern __inline poly64x2x4_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld4q_lane_p64 (const poly64_t * __ptr, poly64x2x4_t __b, const int __c)
{
  return __builtin_aarch64_ld4_lanev2di_psps (
 (__builtin_aarch64_simd_di *) __ptr, __b, __c);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmax_f32 (float32x2_t __a, float32x2_t __b)
{
  return __builtin_aarch64_fmax_nanv2sf (__a, __b);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmax_f64 (float64x1_t __a, float64x1_t __b)
{
    return (float64x1_t)
      { __builtin_aarch64_fmax_nandf (vget_lane_f64 (__a, 0),
          vget_lane_f64 (__b, 0)) };
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmax_s8 (int8x8_t __a, int8x8_t __b)
{
  return __builtin_aarch64_smaxv8qi (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmax_s16 (int16x4_t __a, int16x4_t __b)
{
  return __builtin_aarch64_smaxv4hi (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmax_s32 (int32x2_t __a, int32x2_t __b)
{
  return __builtin_aarch64_smaxv2si (__a, __b);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmax_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint8x8_t) __builtin_aarch64_umaxv8qi ((int8x8_t) __a,
       (int8x8_t) __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmax_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint16x4_t) __builtin_aarch64_umaxv4hi ((int16x4_t) __a,
        (int16x4_t) __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmax_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint32x2_t) __builtin_aarch64_umaxv2si ((int32x2_t) __a,
        (int32x2_t) __b);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxq_f32 (float32x4_t __a, float32x4_t __b)
{
  return __builtin_aarch64_fmax_nanv4sf (__a, __b);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxq_f64 (float64x2_t __a, float64x2_t __b)
{
  return __builtin_aarch64_fmax_nanv2df (__a, __b);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxq_s8 (int8x16_t __a, int8x16_t __b)
{
  return __builtin_aarch64_smaxv16qi (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxq_s16 (int16x8_t __a, int16x8_t __b)
{
  return __builtin_aarch64_smaxv8hi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxq_s32 (int32x4_t __a, int32x4_t __b)
{
  return __builtin_aarch64_smaxv4si (__a, __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return (uint8x16_t) __builtin_aarch64_umaxv16qi ((int8x16_t) __a,
         (int8x16_t) __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (uint16x8_t) __builtin_aarch64_umaxv8hi ((int16x8_t) __a,
        (int16x8_t) __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (uint32x4_t) __builtin_aarch64_umaxv4si ((int32x4_t) __a,
        (int32x4_t) __b);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulx_f32 (float32x2_t __a, float32x2_t __b)
{
  return __builtin_aarch64_fmulxv2sf (__a, __b);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulxq_f32 (float32x4_t __a, float32x4_t __b)
{
  return __builtin_aarch64_fmulxv4sf (__a, __b);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulx_f64 (float64x1_t __a, float64x1_t __b)
{
  return (float64x1_t) {__builtin_aarch64_fmulxdf (__a[0], __b[0])};
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulxq_f64 (float64x2_t __a, float64x2_t __b)
{
  return __builtin_aarch64_fmulxv2df (__a, __b);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulxs_f32 (float32_t __a, float32_t __b)
{
  return __builtin_aarch64_fmulxsf (__a, __b);
}
__extension__ extern __inline float64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulxd_f64 (float64_t __a, float64_t __b)
{
  return __builtin_aarch64_fmulxdf (__a, __b);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulx_lane_f32 (float32x2_t __a, float32x2_t __v, const int __lane)
{
  return vmulx_f32 (__a, __aarch64_vdup_lane_f32 (__v, __lane));
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulx_lane_f64 (float64x1_t __a, float64x1_t __v, const int __lane)
{
  return vmulx_f64 (__a, __aarch64_vdup_lane_f64 (__v, __lane));
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulxq_lane_f32 (float32x4_t __a, float32x2_t __v, const int __lane)
{
  return vmulxq_f32 (__a, __aarch64_vdupq_lane_f32 (__v, __lane));
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulxq_lane_f64 (float64x2_t __a, float64x1_t __v, const int __lane)
{
  return vmulxq_f64 (__a, __aarch64_vdupq_lane_f64 (__v, __lane));
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulx_laneq_f32 (float32x2_t __a, float32x4_t __v, const int __lane)
{
  return vmulx_f32 (__a, __aarch64_vdup_laneq_f32 (__v, __lane));
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulx_laneq_f64 (float64x1_t __a, float64x2_t __v, const int __lane)
{
  return vmulx_f64 (__a, __aarch64_vdup_laneq_f64 (__v, __lane));
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulxq_laneq_f32 (float32x4_t __a, float32x4_t __v, const int __lane)
{
  return vmulxq_f32 (__a, __aarch64_vdupq_laneq_f32 (__v, __lane));
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulxq_laneq_f64 (float64x2_t __a, float64x2_t __v, const int __lane)
{
  return vmulxq_f64 (__a, __aarch64_vdupq_laneq_f64 (__v, __lane));
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulxs_lane_f32 (float32_t __a, float32x2_t __v, const int __lane)
{
  return vmulxs_f32 (__a, __aarch64_vget_lane_any (__v, __lane));
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulxs_laneq_f32 (float32_t __a, float32x4_t __v, const int __lane)
{
  return vmulxs_f32 (__a, __aarch64_vget_lane_any (__v, __lane));
}
__extension__ extern __inline float64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulxd_lane_f64 (float64_t __a, float64x1_t __v, const int __lane)
{
  return vmulxd_f64 (__a, __aarch64_vget_lane_any (__v, __lane));
}
__extension__ extern __inline float64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulxd_laneq_f64 (float64_t __a, float64x2_t __v, const int __lane)
{
  return vmulxd_f64 (__a, __aarch64_vget_lane_any (__v, __lane));
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmax_s8 (int8x8_t __a, int8x8_t __b)
{
  return __builtin_aarch64_smaxpv8qi (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmax_s16 (int16x4_t __a, int16x4_t __b)
{
  return __builtin_aarch64_smaxpv4hi (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmax_s32 (int32x2_t __a, int32x2_t __b)
{
  return __builtin_aarch64_smaxpv2si (__a, __b);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmax_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint8x8_t) __builtin_aarch64_umaxpv8qi ((int8x8_t) __a,
        (int8x8_t) __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmax_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint16x4_t) __builtin_aarch64_umaxpv4hi ((int16x4_t) __a,
         (int16x4_t) __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmax_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint32x2_t) __builtin_aarch64_umaxpv2si ((int32x2_t) __a,
         (int32x2_t) __b);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmaxq_s8 (int8x16_t __a, int8x16_t __b)
{
  return __builtin_aarch64_smaxpv16qi (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmaxq_s16 (int16x8_t __a, int16x8_t __b)
{
  return __builtin_aarch64_smaxpv8hi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmaxq_s32 (int32x4_t __a, int32x4_t __b)
{
  return __builtin_aarch64_smaxpv4si (__a, __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmaxq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return (uint8x16_t) __builtin_aarch64_umaxpv16qi ((int8x16_t) __a,
          (int8x16_t) __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmaxq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (uint16x8_t) __builtin_aarch64_umaxpv8hi ((int16x8_t) __a,
         (int16x8_t) __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmaxq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (uint32x4_t) __builtin_aarch64_umaxpv4si ((int32x4_t) __a,
         (int32x4_t) __b);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmax_f32 (float32x2_t __a, float32x2_t __b)
{
  return __builtin_aarch64_smax_nanpv2sf (__a, __b);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmaxq_f32 (float32x4_t __a, float32x4_t __b)
{
  return __builtin_aarch64_smax_nanpv4sf (__a, __b);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmaxq_f64 (float64x2_t __a, float64x2_t __b)
{
  return __builtin_aarch64_smax_nanpv2df (__a, __b);
}
__extension__ extern __inline float64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmaxqd_f64 (float64x2_t __a)
{
  return __builtin_aarch64_reduc_smax_nan_scal_v2df (__a);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmaxs_f32 (float32x2_t __a)
{
  return __builtin_aarch64_reduc_smax_nan_scal_v2sf (__a);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmaxnm_f32 (float32x2_t __a, float32x2_t __b)
{
  return __builtin_aarch64_smaxpv2sf (__a, __b);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmaxnmq_f32 (float32x4_t __a, float32x4_t __b)
{
  return __builtin_aarch64_smaxpv4sf (__a, __b);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmaxnmq_f64 (float64x2_t __a, float64x2_t __b)
{
  return __builtin_aarch64_smaxpv2df (__a, __b);
}
__extension__ extern __inline float64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmaxnmqd_f64 (float64x2_t __a)
{
  return __builtin_aarch64_reduc_smax_scal_v2df (__a);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmaxnms_f32 (float32x2_t __a)
{
  return __builtin_aarch64_reduc_smax_scal_v2sf (__a);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmin_s8 (int8x8_t __a, int8x8_t __b)
{
  return __builtin_aarch64_sminpv8qi (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmin_s16 (int16x4_t __a, int16x4_t __b)
{
  return __builtin_aarch64_sminpv4hi (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmin_s32 (int32x2_t __a, int32x2_t __b)
{
  return __builtin_aarch64_sminpv2si (__a, __b);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmin_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint8x8_t) __builtin_aarch64_uminpv8qi ((int8x8_t) __a,
        (int8x8_t) __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmin_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint16x4_t) __builtin_aarch64_uminpv4hi ((int16x4_t) __a,
         (int16x4_t) __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmin_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint32x2_t) __builtin_aarch64_uminpv2si ((int32x2_t) __a,
         (int32x2_t) __b);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpminq_s8 (int8x16_t __a, int8x16_t __b)
{
  return __builtin_aarch64_sminpv16qi (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpminq_s16 (int16x8_t __a, int16x8_t __b)
{
  return __builtin_aarch64_sminpv8hi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpminq_s32 (int32x4_t __a, int32x4_t __b)
{
  return __builtin_aarch64_sminpv4si (__a, __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpminq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return (uint8x16_t) __builtin_aarch64_uminpv16qi ((int8x16_t) __a,
          (int8x16_t) __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpminq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (uint16x8_t) __builtin_aarch64_uminpv8hi ((int16x8_t) __a,
         (int16x8_t) __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpminq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (uint32x4_t) __builtin_aarch64_uminpv4si ((int32x4_t) __a,
         (int32x4_t) __b);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmin_f32 (float32x2_t __a, float32x2_t __b)
{
  return __builtin_aarch64_smin_nanpv2sf (__a, __b);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpminq_f32 (float32x4_t __a, float32x4_t __b)
{
  return __builtin_aarch64_smin_nanpv4sf (__a, __b);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpminq_f64 (float64x2_t __a, float64x2_t __b)
{
  return __builtin_aarch64_smin_nanpv2df (__a, __b);
}
__extension__ extern __inline float64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpminqd_f64 (float64x2_t __a)
{
  return __builtin_aarch64_reduc_smin_nan_scal_v2df (__a);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmins_f32 (float32x2_t __a)
{
  return __builtin_aarch64_reduc_smin_nan_scal_v2sf (__a);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpminnm_f32 (float32x2_t __a, float32x2_t __b)
{
  return __builtin_aarch64_sminpv2sf (__a, __b);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpminnmq_f32 (float32x4_t __a, float32x4_t __b)
{
  return __builtin_aarch64_sminpv4sf (__a, __b);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpminnmq_f64 (float64x2_t __a, float64x2_t __b)
{
  return __builtin_aarch64_sminpv2df (__a, __b);
}
__extension__ extern __inline float64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpminnmqd_f64 (float64x2_t __a)
{
  return __builtin_aarch64_reduc_smin_scal_v2df (__a);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpminnms_f32 (float32x2_t __a)
{
  return __builtin_aarch64_reduc_smin_scal_v2sf (__a);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxnm_f32 (float32x2_t __a, float32x2_t __b)
{
  return __builtin_aarch64_fmaxv2sf (__a, __b);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxnm_f64 (float64x1_t __a, float64x1_t __b)
{
  return (float64x1_t)
    { __builtin_aarch64_fmaxdf (vget_lane_f64 (__a, 0),
    vget_lane_f64 (__b, 0)) };
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxnmq_f32 (float32x4_t __a, float32x4_t __b)
{
  return __builtin_aarch64_fmaxv4sf (__a, __b);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxnmq_f64 (float64x2_t __a, float64x2_t __b)
{
  return __builtin_aarch64_fmaxv2df (__a, __b);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxv_f32 (float32x2_t __a)
{
  return __builtin_aarch64_reduc_smax_nan_scal_v2sf (__a);
}
__extension__ extern __inline int8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxv_s8 (int8x8_t __a)
{
  return __builtin_aarch64_reduc_smax_scal_v8qi (__a);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxv_s16 (int16x4_t __a)
{
  return __builtin_aarch64_reduc_smax_scal_v4hi (__a);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxv_s32 (int32x2_t __a)
{
  return __builtin_aarch64_reduc_smax_scal_v2si (__a);
}
__extension__ extern __inline uint8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxv_u8 (uint8x8_t __a)
{
  return __builtin_aarch64_reduc_umax_scal_v8qi_uu (__a);
}
__extension__ extern __inline uint16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxv_u16 (uint16x4_t __a)
{
  return __builtin_aarch64_reduc_umax_scal_v4hi_uu (__a);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxv_u32 (uint32x2_t __a)
{
  return __builtin_aarch64_reduc_umax_scal_v2si_uu (__a);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxvq_f32 (float32x4_t __a)
{
  return __builtin_aarch64_reduc_smax_nan_scal_v4sf (__a);
}
__extension__ extern __inline float64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxvq_f64 (float64x2_t __a)
{
  return __builtin_aarch64_reduc_smax_nan_scal_v2df (__a);
}
__extension__ extern __inline int8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxvq_s8 (int8x16_t __a)
{
  return __builtin_aarch64_reduc_smax_scal_v16qi (__a);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxvq_s16 (int16x8_t __a)
{
  return __builtin_aarch64_reduc_smax_scal_v8hi (__a);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxvq_s32 (int32x4_t __a)
{
  return __builtin_aarch64_reduc_smax_scal_v4si (__a);
}
__extension__ extern __inline uint8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxvq_u8 (uint8x16_t __a)
{
  return __builtin_aarch64_reduc_umax_scal_v16qi_uu (__a);
}
__extension__ extern __inline uint16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxvq_u16 (uint16x8_t __a)
{
  return __builtin_aarch64_reduc_umax_scal_v8hi_uu (__a);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxvq_u32 (uint32x4_t __a)
{
  return __builtin_aarch64_reduc_umax_scal_v4si_uu (__a);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxnmv_f32 (float32x2_t __a)
{
  return __builtin_aarch64_reduc_smax_scal_v2sf (__a);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxnmvq_f32 (float32x4_t __a)
{
  return __builtin_aarch64_reduc_smax_scal_v4sf (__a);
}
__extension__ extern __inline float64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxnmvq_f64 (float64x2_t __a)
{
  return __builtin_aarch64_reduc_smax_scal_v2df (__a);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmin_f32 (float32x2_t __a, float32x2_t __b)
{
  return __builtin_aarch64_fmin_nanv2sf (__a, __b);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmin_f64 (float64x1_t __a, float64x1_t __b)
{
    return (float64x1_t)
   { __builtin_aarch64_fmin_nandf (vget_lane_f64 (__a, 0),
       vget_lane_f64 (__b, 0)) };
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmin_s8 (int8x8_t __a, int8x8_t __b)
{
  return __builtin_aarch64_sminv8qi (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmin_s16 (int16x4_t __a, int16x4_t __b)
{
  return __builtin_aarch64_sminv4hi (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmin_s32 (int32x2_t __a, int32x2_t __b)
{
  return __builtin_aarch64_sminv2si (__a, __b);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmin_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint8x8_t) __builtin_aarch64_uminv8qi ((int8x8_t) __a,
       (int8x8_t) __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmin_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint16x4_t) __builtin_aarch64_uminv4hi ((int16x4_t) __a,
        (int16x4_t) __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmin_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint32x2_t) __builtin_aarch64_uminv2si ((int32x2_t) __a,
        (int32x2_t) __b);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminq_f32 (float32x4_t __a, float32x4_t __b)
{
  return __builtin_aarch64_fmin_nanv4sf (__a, __b);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminq_f64 (float64x2_t __a, float64x2_t __b)
{
  return __builtin_aarch64_fmin_nanv2df (__a, __b);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminq_s8 (int8x16_t __a, int8x16_t __b)
{
  return __builtin_aarch64_sminv16qi (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminq_s16 (int16x8_t __a, int16x8_t __b)
{
  return __builtin_aarch64_sminv8hi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminq_s32 (int32x4_t __a, int32x4_t __b)
{
  return __builtin_aarch64_sminv4si (__a, __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return (uint8x16_t) __builtin_aarch64_uminv16qi ((int8x16_t) __a,
         (int8x16_t) __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (uint16x8_t) __builtin_aarch64_uminv8hi ((int16x8_t) __a,
        (int16x8_t) __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (uint32x4_t) __builtin_aarch64_uminv4si ((int32x4_t) __a,
        (int32x4_t) __b);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminnm_f32 (float32x2_t __a, float32x2_t __b)
{
  return __builtin_aarch64_fminv2sf (__a, __b);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminnm_f64 (float64x1_t __a, float64x1_t __b)
{
  return (float64x1_t)
    { __builtin_aarch64_fmindf (vget_lane_f64 (__a, 0),
    vget_lane_f64 (__b, 0)) };
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminnmq_f32 (float32x4_t __a, float32x4_t __b)
{
  return __builtin_aarch64_fminv4sf (__a, __b);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminnmq_f64 (float64x2_t __a, float64x2_t __b)
{
  return __builtin_aarch64_fminv2df (__a, __b);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminv_f32 (float32x2_t __a)
{
  return __builtin_aarch64_reduc_smin_nan_scal_v2sf (__a);
}
__extension__ extern __inline int8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminv_s8 (int8x8_t __a)
{
  return __builtin_aarch64_reduc_smin_scal_v8qi (__a);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminv_s16 (int16x4_t __a)
{
  return __builtin_aarch64_reduc_smin_scal_v4hi (__a);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminv_s32 (int32x2_t __a)
{
  return __builtin_aarch64_reduc_smin_scal_v2si (__a);
}
__extension__ extern __inline uint8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminv_u8 (uint8x8_t __a)
{
  return __builtin_aarch64_reduc_umin_scal_v8qi_uu (__a);
}
__extension__ extern __inline uint16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminv_u16 (uint16x4_t __a)
{
  return __builtin_aarch64_reduc_umin_scal_v4hi_uu (__a);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminv_u32 (uint32x2_t __a)
{
  return __builtin_aarch64_reduc_umin_scal_v2si_uu (__a);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminvq_f32 (float32x4_t __a)
{
  return __builtin_aarch64_reduc_smin_nan_scal_v4sf (__a);
}
__extension__ extern __inline float64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminvq_f64 (float64x2_t __a)
{
  return __builtin_aarch64_reduc_smin_nan_scal_v2df (__a);
}
__extension__ extern __inline int8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminvq_s8 (int8x16_t __a)
{
  return __builtin_aarch64_reduc_smin_scal_v16qi (__a);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminvq_s16 (int16x8_t __a)
{
  return __builtin_aarch64_reduc_smin_scal_v8hi (__a);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminvq_s32 (int32x4_t __a)
{
  return __builtin_aarch64_reduc_smin_scal_v4si (__a);
}
__extension__ extern __inline uint8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminvq_u8 (uint8x16_t __a)
{
  return __builtin_aarch64_reduc_umin_scal_v16qi_uu (__a);
}
__extension__ extern __inline uint16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminvq_u16 (uint16x8_t __a)
{
  return __builtin_aarch64_reduc_umin_scal_v8hi_uu (__a);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminvq_u32 (uint32x4_t __a)
{
  return __builtin_aarch64_reduc_umin_scal_v4si_uu (__a);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminnmv_f32 (float32x2_t __a)
{
  return __builtin_aarch64_reduc_smin_scal_v2sf (__a);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminnmvq_f32 (float32x4_t __a)
{
  return __builtin_aarch64_reduc_smin_scal_v4sf (__a);
}
__extension__ extern __inline float64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminnmvq_f64 (float64x2_t __a)
{
  return __builtin_aarch64_reduc_smin_scal_v2df (__a);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmla_f32 (float32x2_t __a, float32x2_t __b, float32x2_t __c)
{
  return __builtin_aarch64_float_mlav2sf (__a, __b, __c);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmla_f64 (float64x1_t __a, float64x1_t __b, float64x1_t __c)
{
  return (float64x1_t) {__builtin_aarch64_float_mladf (__a[0], __b[0], __c[0])};
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlaq_f32 (float32x4_t __a, float32x4_t __b, float32x4_t __c)
{
  return __builtin_aarch64_float_mlav4sf (__a, __b, __c);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlaq_f64 (float64x2_t __a, float64x2_t __b, float64x2_t __c)
{
  return __builtin_aarch64_float_mlav2df (__a, __b, __c);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmla_lane_f32 (float32x2_t __a, float32x2_t __b,
        float32x2_t __c, const int __lane)
{
  return __builtin_aarch64_float_mla_lanev2sf (__a, __b, __c, __lane);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmla_lane_s16 (int16x4_t __a, int16x4_t __b,
  int16x4_t __c, const int __lane)
{
  return (__a + (__b * __aarch64_vget_lane_any (__c, __lane)));
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmla_lane_s32 (int32x2_t __a, int32x2_t __b,
  int32x2_t __c, const int __lane)
{
  return (__a + (__b * __aarch64_vget_lane_any (__c, __lane)));
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmla_lane_u16 (uint16x4_t __a, uint16x4_t __b,
  uint16x4_t __c, const int __lane)
{
  return (__a + (__b * __aarch64_vget_lane_any (__c, __lane)));
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmla_lane_u32 (uint32x2_t __a, uint32x2_t __b,
        uint32x2_t __c, const int __lane)
{
  return (__a + (__b * __aarch64_vget_lane_any (__c, __lane)));
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmla_laneq_f32 (float32x2_t __a, float32x2_t __b,
         float32x4_t __c, const int __lane)
{
  return __builtin_aarch64_float_mla_laneqv2sf (__a, __b, __c, __lane);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmla_laneq_s16 (int16x4_t __a, int16x4_t __b,
  int16x8_t __c, const int __lane)
{
  return (__a + (__b * __aarch64_vget_lane_any (__c, __lane)));
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmla_laneq_s32 (int32x2_t __a, int32x2_t __b,
  int32x4_t __c, const int __lane)
{
  return (__a + (__b * __aarch64_vget_lane_any (__c, __lane)));
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmla_laneq_u16 (uint16x4_t __a, uint16x4_t __b,
  uint16x8_t __c, const int __lane)
{
  return (__a + (__b * __aarch64_vget_lane_any (__c, __lane)));
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmla_laneq_u32 (uint32x2_t __a, uint32x2_t __b,
  uint32x4_t __c, const int __lane)
{
  return (__a + (__b * __aarch64_vget_lane_any (__c, __lane)));
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlaq_lane_f32 (float32x4_t __a, float32x4_t __b,
  float32x2_t __c, const int __lane)
{
  return __builtin_aarch64_float_mla_lanev4sf (__a, __b, __c, __lane);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlaq_lane_s16 (int16x8_t __a, int16x8_t __b,
  int16x4_t __c, const int __lane)
{
  return (__a + (__b * __aarch64_vget_lane_any (__c, __lane)));
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlaq_lane_s32 (int32x4_t __a, int32x4_t __b,
  int32x2_t __c, const int __lane)
{
  return (__a + (__b * __aarch64_vget_lane_any (__c, __lane)));
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlaq_lane_u16 (uint16x8_t __a, uint16x8_t __b,
  uint16x4_t __c, const int __lane)
{
  return (__a + (__b * __aarch64_vget_lane_any (__c, __lane)));
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlaq_lane_u32 (uint32x4_t __a, uint32x4_t __b,
  uint32x2_t __c, const int __lane)
{
  return (__a + (__b * __aarch64_vget_lane_any (__c, __lane)));
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlaq_laneq_f32 (float32x4_t __a, float32x4_t __b,
   float32x4_t __c, const int __lane)
{
  return __builtin_aarch64_float_mla_laneqv4sf (__a, __b, __c, __lane);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlaq_laneq_s16 (int16x8_t __a, int16x8_t __b,
  int16x8_t __c, const int __lane)
{
  return (__a + (__b * __aarch64_vget_lane_any (__c, __lane)));
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlaq_laneq_s32 (int32x4_t __a, int32x4_t __b,
  int32x4_t __c, const int __lane)
{
  return (__a + (__b * __aarch64_vget_lane_any (__c, __lane)));
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlaq_laneq_u16 (uint16x8_t __a, uint16x8_t __b,
  uint16x8_t __c, const int __lane)
{
  return (__a + (__b * __aarch64_vget_lane_any (__c, __lane)));
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlaq_laneq_u32 (uint32x4_t __a, uint32x4_t __b,
  uint32x4_t __c, const int __lane)
{
  return (__a + (__b * __aarch64_vget_lane_any (__c, __lane)));
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmls_f32 (float32x2_t __a, float32x2_t __b, float32x2_t __c)
{
  return __builtin_aarch64_float_mlsv2sf (__a, __b, __c);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmls_f64 (float64x1_t __a, float64x1_t __b, float64x1_t __c)
{
  return (float64x1_t) {__builtin_aarch64_float_mlsdf (__a[0], __b[0], __c[0])};
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsq_f32 (float32x4_t __a, float32x4_t __b, float32x4_t __c)
{
  return __builtin_aarch64_float_mlsv4sf (__a, __b, __c);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsq_f64 (float64x2_t __a, float64x2_t __b, float64x2_t __c)
{
  return __builtin_aarch64_float_mlsv2df (__a, __b, __c);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmls_lane_f32 (float32x2_t __a, float32x2_t __b,
        float32x2_t __c, const int __lane)
{
  return __builtin_aarch64_float_mls_lanev2sf (__a, __b, __c, __lane);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmls_lane_s16 (int16x4_t __a, int16x4_t __b,
  int16x4_t __c, const int __lane)
{
  return (__a - (__b * __aarch64_vget_lane_any (__c, __lane)));
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmls_lane_s32 (int32x2_t __a, int32x2_t __b,
  int32x2_t __c, const int __lane)
{
  return (__a - (__b * __aarch64_vget_lane_any (__c, __lane)));
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmls_lane_u16 (uint16x4_t __a, uint16x4_t __b,
  uint16x4_t __c, const int __lane)
{
  return (__a - (__b * __aarch64_vget_lane_any (__c, __lane)));
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmls_lane_u32 (uint32x2_t __a, uint32x2_t __b,
        uint32x2_t __c, const int __lane)
{
  return (__a - (__b * __aarch64_vget_lane_any (__c, __lane)));
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmls_laneq_f32 (float32x2_t __a, float32x2_t __b,
        float32x4_t __c, const int __lane)
{
  return __builtin_aarch64_float_mls_laneqv2sf (__a, __b, __c, __lane);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmls_laneq_s16 (int16x4_t __a, int16x4_t __b,
  int16x8_t __c, const int __lane)
{
  return (__a - (__b * __aarch64_vget_lane_any (__c, __lane)));
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmls_laneq_s32 (int32x2_t __a, int32x2_t __b,
  int32x4_t __c, const int __lane)
{
  return (__a - (__b * __aarch64_vget_lane_any (__c, __lane)));
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmls_laneq_u16 (uint16x4_t __a, uint16x4_t __b,
  uint16x8_t __c, const int __lane)
{
  return (__a - (__b * __aarch64_vget_lane_any (__c, __lane)));
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmls_laneq_u32 (uint32x2_t __a, uint32x2_t __b,
  uint32x4_t __c, const int __lane)
{
  return (__a - (__b * __aarch64_vget_lane_any (__c, __lane)));
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsq_lane_f32 (float32x4_t __a, float32x4_t __b,
  float32x2_t __c, const int __lane)
{
  return __builtin_aarch64_float_mls_lanev4sf (__a, __b, __c, __lane);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsq_lane_s16 (int16x8_t __a, int16x8_t __b,
  int16x4_t __c, const int __lane)
{
  return (__a - (__b * __aarch64_vget_lane_any (__c, __lane)));
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsq_lane_s32 (int32x4_t __a, int32x4_t __b,
  int32x2_t __c, const int __lane)
{
  return (__a - (__b * __aarch64_vget_lane_any (__c, __lane)));
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsq_lane_u16 (uint16x8_t __a, uint16x8_t __b,
  uint16x4_t __c, const int __lane)
{
  return (__a - (__b * __aarch64_vget_lane_any (__c, __lane)));
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsq_lane_u32 (uint32x4_t __a, uint32x4_t __b,
  uint32x2_t __c, const int __lane)
{
  return (__a - (__b * __aarch64_vget_lane_any (__c, __lane)));
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsq_laneq_f32 (float32x4_t __a, float32x4_t __b,
  float32x4_t __c, const int __lane)
{
  return __builtin_aarch64_float_mls_laneqv4sf (__a, __b, __c, __lane);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsq_laneq_s16 (int16x8_t __a, int16x8_t __b,
  int16x8_t __c, const int __lane)
{
  return (__a - (__b * __aarch64_vget_lane_any (__c, __lane)));
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsq_laneq_s32 (int32x4_t __a, int32x4_t __b,
  int32x4_t __c, const int __lane)
{
  return (__a - (__b * __aarch64_vget_lane_any (__c, __lane)));
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsq_laneq_u16 (uint16x8_t __a, uint16x8_t __b,
  uint16x8_t __c, const int __lane)
{
  return (__a - (__b * __aarch64_vget_lane_any (__c, __lane)));
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmlsq_laneq_u32 (uint32x4_t __a, uint32x4_t __b,
  uint32x4_t __c, const int __lane)
{
  return (__a - (__b * __aarch64_vget_lane_any (__c, __lane)));
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmov_n_f16 (float16_t __a)
{
  return vdup_n_f16 (__a);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmov_n_f32 (float32_t __a)
{
  return vdup_n_f32 (__a);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmov_n_f64 (float64_t __a)
{
  return (float64x1_t) {__a};
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmov_n_p8 (poly8_t __a)
{
  return vdup_n_p8 (__a);
}
__extension__ extern __inline poly16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmov_n_p16 (poly16_t __a)
{
  return vdup_n_p16 (__a);
}
__extension__ extern __inline poly64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmov_n_p64 (poly64_t __a)
{
  return vdup_n_p64 (__a);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmov_n_s8 (int8_t __a)
{
  return vdup_n_s8 (__a);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmov_n_s16 (int16_t __a)
{
  return vdup_n_s16 (__a);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmov_n_s32 (int32_t __a)
{
  return vdup_n_s32 (__a);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmov_n_s64 (int64_t __a)
{
  return (int64x1_t) {__a};
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmov_n_u8 (uint8_t __a)
{
  return vdup_n_u8 (__a);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmov_n_u16 (uint16_t __a)
{
    return vdup_n_u16 (__a);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmov_n_u32 (uint32_t __a)
{
   return vdup_n_u32 (__a);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmov_n_u64 (uint64_t __a)
{
  return (uint64x1_t) {__a};
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovq_n_f16 (float16_t __a)
{
  return vdupq_n_f16 (__a);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovq_n_f32 (float32_t __a)
{
  return vdupq_n_f32 (__a);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovq_n_f64 (float64_t __a)
{
  return vdupq_n_f64 (__a);
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovq_n_p8 (poly8_t __a)
{
  return vdupq_n_p8 (__a);
}
__extension__ extern __inline poly16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovq_n_p16 (poly16_t __a)
{
  return vdupq_n_p16 (__a);
}
__extension__ extern __inline poly64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovq_n_p64 (poly64_t __a)
{
  return vdupq_n_p64 (__a);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovq_n_s8 (int8_t __a)
{
  return vdupq_n_s8 (__a);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovq_n_s16 (int16_t __a)
{
  return vdupq_n_s16 (__a);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovq_n_s32 (int32_t __a)
{
  return vdupq_n_s32 (__a);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovq_n_s64 (int64_t __a)
{
  return vdupq_n_s64 (__a);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovq_n_u8 (uint8_t __a)
{
  return vdupq_n_u8 (__a);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovq_n_u16 (uint16_t __a)
{
  return vdupq_n_u16 (__a);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovq_n_u32 (uint32_t __a)
{
  return vdupq_n_u32 (__a);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmovq_n_u64 (uint64_t __a)
{
  return vdupq_n_u64 (__a);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmul_lane_f32 (float32x2_t __a, float32x2_t __b, const int __lane)
{
  return __a * __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmul_lane_f64 (float64x1_t __a, float64x1_t __b, const int __lane)
{
  return __a * __b;
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmul_lane_s16 (int16x4_t __a, int16x4_t __b, const int __lane)
{
  return __a * __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmul_lane_s32 (int32x2_t __a, int32x2_t __b, const int __lane)
{
  return __a * __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmul_lane_u16 (uint16x4_t __a, uint16x4_t __b, const int __lane)
{
  return __a * __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmul_lane_u32 (uint32x2_t __a, uint32x2_t __b, const int __lane)
{
  return __a * __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline float64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmuld_lane_f64 (float64_t __a, float64x1_t __b, const int __lane)
{
  return __a * __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline float64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmuld_laneq_f64 (float64_t __a, float64x2_t __b, const int __lane)
{
  return __a * __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmuls_lane_f32 (float32_t __a, float32x2_t __b, const int __lane)
{
  return __a * __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmuls_laneq_f32 (float32_t __a, float32x4_t __b, const int __lane)
{
  return __a * __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmul_laneq_f32 (float32x2_t __a, float32x4_t __b, const int __lane)
{
  return __a * __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmul_laneq_f64 (float64x1_t __a, float64x2_t __b, const int __lane)
{
  return __a * __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmul_laneq_s16 (int16x4_t __a, int16x8_t __b, const int __lane)
{
  return __a * __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmul_laneq_s32 (int32x2_t __a, int32x4_t __b, const int __lane)
{
  return __a * __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmul_laneq_u16 (uint16x4_t __a, uint16x8_t __b, const int __lane)
{
  return __a * __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmul_laneq_u32 (uint32x2_t __a, uint32x4_t __b, const int __lane)
{
  return __a * __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmul_n_f64 (float64x1_t __a, float64_t __b)
{
  return (float64x1_t) { vget_lane_f64 (__a, 0) * __b };
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulq_lane_f32 (float32x4_t __a, float32x2_t __b, const int __lane)
{
  return __a * __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulq_lane_f64 (float64x2_t __a, float64x1_t __b, const int __lane)
{
  __AARCH64_LANE_CHECK (__a, __lane);
  return __a * __b[0];
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulq_lane_s16 (int16x8_t __a, int16x4_t __b, const int __lane)
{
  return __a * __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulq_lane_s32 (int32x4_t __a, int32x2_t __b, const int __lane)
{
  return __a * __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulq_lane_u16 (uint16x8_t __a, uint16x4_t __b, const int __lane)
{
  return __a * __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulq_lane_u32 (uint32x4_t __a, uint32x2_t __b, const int __lane)
{
  return __a * __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulq_laneq_f32 (float32x4_t __a, float32x4_t __b, const int __lane)
{
  return __a * __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulq_laneq_f64 (float64x2_t __a, float64x2_t __b, const int __lane)
{
  return __a * __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulq_laneq_s16 (int16x8_t __a, int16x8_t __b, const int __lane)
{
  return __a * __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulq_laneq_s32 (int32x4_t __a, int32x4_t __b, const int __lane)
{
  return __a * __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulq_laneq_u16 (uint16x8_t __a, uint16x8_t __b, const int __lane)
{
  return __a * __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulq_laneq_u32 (uint32x4_t __a, uint32x4_t __b, const int __lane)
{
  return __a * __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmul_n_f32 (float32x2_t __a, float32_t __b)
{
  return __a * __b;
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulq_n_f32 (float32x4_t __a, float32_t __b)
{
  return __a * __b;
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulq_n_f64 (float64x2_t __a, float64_t __b)
{
  return __a * __b;
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmul_n_s16 (int16x4_t __a, int16_t __b)
{
  return __a * __b;
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulq_n_s16 (int16x8_t __a, int16_t __b)
{
  return __a * __b;
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmul_n_s32 (int32x2_t __a, int32_t __b)
{
  return __a * __b;
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulq_n_s32 (int32x4_t __a, int32_t __b)
{
  return __a * __b;
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmul_n_u16 (uint16x4_t __a, uint16_t __b)
{
  return __a * __b;
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulq_n_u16 (uint16x8_t __a, uint16_t __b)
{
  return __a * __b;
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmul_n_u32 (uint32x2_t __a, uint32_t __b)
{
  return __a * __b;
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulq_n_u32 (uint32x4_t __a, uint32_t __b)
{
  return __a * __b;
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmvn_p8 (poly8x8_t __a)
{
  return (poly8x8_t) ~((int8x8_t) __a);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmvn_s8 (int8x8_t __a)
{
  return ~__a;
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmvn_s16 (int16x4_t __a)
{
  return ~__a;
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmvn_s32 (int32x2_t __a)
{
  return ~__a;
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmvn_u8 (uint8x8_t __a)
{
  return ~__a;
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmvn_u16 (uint16x4_t __a)
{
  return ~__a;
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmvn_u32 (uint32x2_t __a)
{
  return ~__a;
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmvnq_p8 (poly8x16_t __a)
{
  return (poly8x16_t) ~((int8x16_t) __a);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmvnq_s8 (int8x16_t __a)
{
  return ~__a;
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmvnq_s16 (int16x8_t __a)
{
  return ~__a;
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmvnq_s32 (int32x4_t __a)
{
  return ~__a;
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmvnq_u8 (uint8x16_t __a)
{
  return ~__a;
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmvnq_u16 (uint16x8_t __a)
{
  return ~__a;
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmvnq_u32 (uint32x4_t __a)
{
  return ~__a;
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vneg_f32 (float32x2_t __a)
{
  return -__a;
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vneg_f64 (float64x1_t __a)
{
  return -__a;
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vneg_s8 (int8x8_t __a)
{
  return -__a;
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vneg_s16 (int16x4_t __a)
{
  return -__a;
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vneg_s32 (int32x2_t __a)
{
  return -__a;
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vneg_s64 (int64x1_t __a)
{
  return -__a;
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vnegd_s64 (int64_t __a)
{
  return - (uint64_t) __a;
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vnegq_f32 (float32x4_t __a)
{
  return -__a;
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vnegq_f64 (float64x2_t __a)
{
  return -__a;
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vnegq_s8 (int8x16_t __a)
{
  return -__a;
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vnegq_s16 (int16x8_t __a)
{
  return -__a;
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vnegq_s32 (int32x4_t __a)
{
  return -__a;
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vnegq_s64 (int64x2_t __a)
{
  return -__a;
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpadd_f32 (float32x2_t __a, float32x2_t __b)
{
  return __builtin_aarch64_faddpv2sf (__a, __b);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpaddq_f32 (float32x4_t __a, float32x4_t __b)
{
  return __builtin_aarch64_faddpv4sf (__a, __b);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpaddq_f64 (float64x2_t __a, float64x2_t __b)
{
  return __builtin_aarch64_faddpv2df (__a, __b);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpadd_s8 (int8x8_t __a, int8x8_t __b)
{
  return __builtin_aarch64_addpv8qi (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpadd_s16 (int16x4_t __a, int16x4_t __b)
{
  return __builtin_aarch64_addpv4hi (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpadd_s32 (int32x2_t __a, int32x2_t __b)
{
  return __builtin_aarch64_addpv2si (__a, __b);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpadd_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return __builtin_aarch64_addpv8qi_uuu (__a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpadd_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return __builtin_aarch64_addpv4hi_uuu (__a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpadd_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return __builtin_aarch64_addpv2si_uuu (__a, __b);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpadds_f32 (float32x2_t __a)
{
  return __builtin_aarch64_reduc_plus_scal_v2sf (__a);
}
__extension__ extern __inline float64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpaddd_f64 (float64x2_t __a)
{
  return __builtin_aarch64_reduc_plus_scal_v2df (__a);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpaddd_s64 (int64x2_t __a)
{
  return __builtin_aarch64_reduc_plus_scal_v2di (__a);
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpaddd_u64 (uint64x2_t __a)
{
  return __builtin_aarch64_reduc_plus_scal_v2di_uu (__a);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqabsq_s64 (int64x2_t __a)
{
  return (int64x2_t) __builtin_aarch64_sqabsv2di (__a);
}
__extension__ extern __inline int8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqabsb_s8 (int8_t __a)
{
  return (int8_t) __builtin_aarch64_sqabsqi (__a);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqabsh_s16 (int16_t __a)
{
  return (int16_t) __builtin_aarch64_sqabshi (__a);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqabss_s32 (int32_t __a)
{
  return (int32_t) __builtin_aarch64_sqabssi (__a);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqabsd_s64 (int64_t __a)
{
  return __builtin_aarch64_sqabsdi (__a);
}
__extension__ extern __inline int8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqaddb_s8 (int8_t __a, int8_t __b)
{
  return (int8_t) __builtin_aarch64_sqaddqi (__a, __b);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqaddh_s16 (int16_t __a, int16_t __b)
{
  return (int16_t) __builtin_aarch64_sqaddhi (__a, __b);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqadds_s32 (int32_t __a, int32_t __b)
{
  return (int32_t) __builtin_aarch64_sqaddsi (__a, __b);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqaddd_s64 (int64_t __a, int64_t __b)
{
  return __builtin_aarch64_sqadddi (__a, __b);
}
__extension__ extern __inline uint8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqaddb_u8 (uint8_t __a, uint8_t __b)
{
  return (uint8_t) __builtin_aarch64_uqaddqi_uuu (__a, __b);
}
__extension__ extern __inline uint16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqaddh_u16 (uint16_t __a, uint16_t __b)
{
  return (uint16_t) __builtin_aarch64_uqaddhi_uuu (__a, __b);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqadds_u32 (uint32_t __a, uint32_t __b)
{
  return (uint32_t) __builtin_aarch64_uqaddsi_uuu (__a, __b);
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqaddd_u64 (uint64_t __a, uint64_t __b)
{
  return __builtin_aarch64_uqadddi_uuu (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlal_s16 (int32x4_t __a, int16x4_t __b, int16x4_t __c)
{
  return __builtin_aarch64_sqdmlalv4hi (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlal_high_s16 (int32x4_t __a, int16x8_t __b, int16x8_t __c)
{
  return __builtin_aarch64_sqdmlal2v8hi (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlal_high_lane_s16 (int32x4_t __a, int16x8_t __b, int16x4_t __c,
         int const __d)
{
  return __builtin_aarch64_sqdmlal2_lanev8hi (__a, __b, __c, __d);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlal_high_laneq_s16 (int32x4_t __a, int16x8_t __b, int16x8_t __c,
   int const __d)
{
  return __builtin_aarch64_sqdmlal2_laneqv8hi (__a, __b, __c, __d);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlal_high_n_s16 (int32x4_t __a, int16x8_t __b, int16_t __c)
{
  return __builtin_aarch64_sqdmlal2_nv8hi (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlal_lane_s16 (int32x4_t __a, int16x4_t __b, int16x4_t __c, int const __d)
{
  return __builtin_aarch64_sqdmlal_lanev4hi (__a, __b, __c, __d);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlal_laneq_s16 (int32x4_t __a, int16x4_t __b, int16x8_t __c, int const __d)
{
  return __builtin_aarch64_sqdmlal_laneqv4hi (__a, __b, __c, __d);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlal_n_s16 (int32x4_t __a, int16x4_t __b, int16_t __c)
{
  return __builtin_aarch64_sqdmlal_nv4hi (__a, __b, __c);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlal_s32 (int64x2_t __a, int32x2_t __b, int32x2_t __c)
{
  return __builtin_aarch64_sqdmlalv2si (__a, __b, __c);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlal_high_s32 (int64x2_t __a, int32x4_t __b, int32x4_t __c)
{
  return __builtin_aarch64_sqdmlal2v4si (__a, __b, __c);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlal_high_lane_s32 (int64x2_t __a, int32x4_t __b, int32x2_t __c,
         int const __d)
{
  return __builtin_aarch64_sqdmlal2_lanev4si (__a, __b, __c, __d);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlal_high_laneq_s32 (int64x2_t __a, int32x4_t __b, int32x4_t __c,
   int const __d)
{
  return __builtin_aarch64_sqdmlal2_laneqv4si (__a, __b, __c, __d);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlal_high_n_s32 (int64x2_t __a, int32x4_t __b, int32_t __c)
{
  return __builtin_aarch64_sqdmlal2_nv4si (__a, __b, __c);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlal_lane_s32 (int64x2_t __a, int32x2_t __b, int32x2_t __c, int const __d)
{
  return __builtin_aarch64_sqdmlal_lanev2si (__a, __b, __c, __d);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlal_laneq_s32 (int64x2_t __a, int32x2_t __b, int32x4_t __c, int const __d)
{
  return __builtin_aarch64_sqdmlal_laneqv2si (__a, __b, __c, __d);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlal_n_s32 (int64x2_t __a, int32x2_t __b, int32_t __c)
{
  return __builtin_aarch64_sqdmlal_nv2si (__a, __b, __c);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlalh_s16 (int32_t __a, int16_t __b, int16_t __c)
{
  return __builtin_aarch64_sqdmlalhi (__a, __b, __c);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlalh_lane_s16 (int32_t __a, int16_t __b, int16x4_t __c, const int __d)
{
  return __builtin_aarch64_sqdmlal_lanehi (__a, __b, __c, __d);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlalh_laneq_s16 (int32_t __a, int16_t __b, int16x8_t __c, const int __d)
{
  return __builtin_aarch64_sqdmlal_laneqhi (__a, __b, __c, __d);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlals_s32 (int64_t __a, int32_t __b, int32_t __c)
{
  return __builtin_aarch64_sqdmlalsi (__a, __b, __c);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlals_lane_s32 (int64_t __a, int32_t __b, int32x2_t __c, const int __d)
{
  return __builtin_aarch64_sqdmlal_lanesi (__a, __b, __c, __d);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlals_laneq_s32 (int64_t __a, int32_t __b, int32x4_t __c, const int __d)
{
  return __builtin_aarch64_sqdmlal_laneqsi (__a, __b, __c, __d);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlsl_s16 (int32x4_t __a, int16x4_t __b, int16x4_t __c)
{
  return __builtin_aarch64_sqdmlslv4hi (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlsl_high_s16 (int32x4_t __a, int16x8_t __b, int16x8_t __c)
{
  return __builtin_aarch64_sqdmlsl2v8hi (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlsl_high_lane_s16 (int32x4_t __a, int16x8_t __b, int16x4_t __c,
         int const __d)
{
  return __builtin_aarch64_sqdmlsl2_lanev8hi (__a, __b, __c, __d);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlsl_high_laneq_s16 (int32x4_t __a, int16x8_t __b, int16x8_t __c,
   int const __d)
{
  return __builtin_aarch64_sqdmlsl2_laneqv8hi (__a, __b, __c, __d);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlsl_high_n_s16 (int32x4_t __a, int16x8_t __b, int16_t __c)
{
  return __builtin_aarch64_sqdmlsl2_nv8hi (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlsl_lane_s16 (int32x4_t __a, int16x4_t __b, int16x4_t __c, int const __d)
{
  return __builtin_aarch64_sqdmlsl_lanev4hi (__a, __b, __c, __d);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlsl_laneq_s16 (int32x4_t __a, int16x4_t __b, int16x8_t __c, int const __d)
{
  return __builtin_aarch64_sqdmlsl_laneqv4hi (__a, __b, __c, __d);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlsl_n_s16 (int32x4_t __a, int16x4_t __b, int16_t __c)
{
  return __builtin_aarch64_sqdmlsl_nv4hi (__a, __b, __c);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlsl_s32 (int64x2_t __a, int32x2_t __b, int32x2_t __c)
{
  return __builtin_aarch64_sqdmlslv2si (__a, __b, __c);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlsl_high_s32 (int64x2_t __a, int32x4_t __b, int32x4_t __c)
{
  return __builtin_aarch64_sqdmlsl2v4si (__a, __b, __c);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlsl_high_lane_s32 (int64x2_t __a, int32x4_t __b, int32x2_t __c,
         int const __d)
{
  return __builtin_aarch64_sqdmlsl2_lanev4si (__a, __b, __c, __d);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlsl_high_laneq_s32 (int64x2_t __a, int32x4_t __b, int32x4_t __c,
   int const __d)
{
  return __builtin_aarch64_sqdmlsl2_laneqv4si (__a, __b, __c, __d);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlsl_high_n_s32 (int64x2_t __a, int32x4_t __b, int32_t __c)
{
  return __builtin_aarch64_sqdmlsl2_nv4si (__a, __b, __c);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlsl_lane_s32 (int64x2_t __a, int32x2_t __b, int32x2_t __c, int const __d)
{
  return __builtin_aarch64_sqdmlsl_lanev2si (__a, __b, __c, __d);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlsl_laneq_s32 (int64x2_t __a, int32x2_t __b, int32x4_t __c, int const __d)
{
  return __builtin_aarch64_sqdmlsl_laneqv2si (__a, __b, __c, __d);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlsl_n_s32 (int64x2_t __a, int32x2_t __b, int32_t __c)
{
  return __builtin_aarch64_sqdmlsl_nv2si (__a, __b, __c);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlslh_s16 (int32_t __a, int16_t __b, int16_t __c)
{
  return __builtin_aarch64_sqdmlslhi (__a, __b, __c);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlslh_lane_s16 (int32_t __a, int16_t __b, int16x4_t __c, const int __d)
{
  return __builtin_aarch64_sqdmlsl_lanehi (__a, __b, __c, __d);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlslh_laneq_s16 (int32_t __a, int16_t __b, int16x8_t __c, const int __d)
{
  return __builtin_aarch64_sqdmlsl_laneqhi (__a, __b, __c, __d);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlsls_s32 (int64_t __a, int32_t __b, int32_t __c)
{
  return __builtin_aarch64_sqdmlslsi (__a, __b, __c);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlsls_lane_s32 (int64_t __a, int32_t __b, int32x2_t __c, const int __d)
{
  return __builtin_aarch64_sqdmlsl_lanesi (__a, __b, __c, __d);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmlsls_laneq_s32 (int64_t __a, int32_t __b, int32x4_t __c, const int __d)
{
  return __builtin_aarch64_sqdmlsl_laneqsi (__a, __b, __c, __d);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmulh_lane_s16 (int16x4_t __a, int16x4_t __b, const int __c)
{
  return __builtin_aarch64_sqdmulh_lanev4hi (__a, __b, __c);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmulh_lane_s32 (int32x2_t __a, int32x2_t __b, const int __c)
{
  return __builtin_aarch64_sqdmulh_lanev2si (__a, __b, __c);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmulhq_lane_s16 (int16x8_t __a, int16x4_t __b, const int __c)
{
  return __builtin_aarch64_sqdmulh_lanev8hi (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmulhq_lane_s32 (int32x4_t __a, int32x2_t __b, const int __c)
{
  return __builtin_aarch64_sqdmulh_lanev4si (__a, __b, __c);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmulhh_s16 (int16_t __a, int16_t __b)
{
  return (int16_t) __builtin_aarch64_sqdmulhhi (__a, __b);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmulhh_lane_s16 (int16_t __a, int16x4_t __b, const int __c)
{
  return __builtin_aarch64_sqdmulh_lanehi (__a, __b, __c);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmulhh_laneq_s16 (int16_t __a, int16x8_t __b, const int __c)
{
  return __builtin_aarch64_sqdmulh_laneqhi (__a, __b, __c);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmulhs_s32 (int32_t __a, int32_t __b)
{
  return (int32_t) __builtin_aarch64_sqdmulhsi (__a, __b);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmulhs_lane_s32 (int32_t __a, int32x2_t __b, const int __c)
{
  return __builtin_aarch64_sqdmulh_lanesi (__a, __b, __c);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmulhs_laneq_s32 (int32_t __a, int32x4_t __b, const int __c)
{
  return __builtin_aarch64_sqdmulh_laneqsi (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmull_s16 (int16x4_t __a, int16x4_t __b)
{
  return __builtin_aarch64_sqdmullv4hi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmull_high_s16 (int16x8_t __a, int16x8_t __b)
{
  return __builtin_aarch64_sqdmull2v8hi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmull_high_lane_s16 (int16x8_t __a, int16x4_t __b, int const __c)
{
  return __builtin_aarch64_sqdmull2_lanev8hi (__a, __b,__c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmull_high_laneq_s16 (int16x8_t __a, int16x8_t __b, int const __c)
{
  return __builtin_aarch64_sqdmull2_laneqv8hi (__a, __b,__c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmull_high_n_s16 (int16x8_t __a, int16_t __b)
{
  return __builtin_aarch64_sqdmull2_nv8hi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmull_lane_s16 (int16x4_t __a, int16x4_t __b, int const __c)
{
  return __builtin_aarch64_sqdmull_lanev4hi (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmull_laneq_s16 (int16x4_t __a, int16x8_t __b, int const __c)
{
  return __builtin_aarch64_sqdmull_laneqv4hi (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmull_n_s16 (int16x4_t __a, int16_t __b)
{
  return __builtin_aarch64_sqdmull_nv4hi (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmull_s32 (int32x2_t __a, int32x2_t __b)
{
  return __builtin_aarch64_sqdmullv2si (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmull_high_s32 (int32x4_t __a, int32x4_t __b)
{
  return __builtin_aarch64_sqdmull2v4si (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmull_high_lane_s32 (int32x4_t __a, int32x2_t __b, int const __c)
{
  return __builtin_aarch64_sqdmull2_lanev4si (__a, __b, __c);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmull_high_laneq_s32 (int32x4_t __a, int32x4_t __b, int const __c)
{
  return __builtin_aarch64_sqdmull2_laneqv4si (__a, __b, __c);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmull_high_n_s32 (int32x4_t __a, int32_t __b)
{
  return __builtin_aarch64_sqdmull2_nv4si (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmull_lane_s32 (int32x2_t __a, int32x2_t __b, int const __c)
{
  return __builtin_aarch64_sqdmull_lanev2si (__a, __b, __c);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmull_laneq_s32 (int32x2_t __a, int32x4_t __b, int const __c)
{
  return __builtin_aarch64_sqdmull_laneqv2si (__a, __b, __c);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmull_n_s32 (int32x2_t __a, int32_t __b)
{
  return __builtin_aarch64_sqdmull_nv2si (__a, __b);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmullh_s16 (int16_t __a, int16_t __b)
{
  return (int32_t) __builtin_aarch64_sqdmullhi (__a, __b);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmullh_lane_s16 (int16_t __a, int16x4_t __b, const int __c)
{
  return __builtin_aarch64_sqdmull_lanehi (__a, __b, __c);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmullh_laneq_s16 (int16_t __a, int16x8_t __b, const int __c)
{
  return __builtin_aarch64_sqdmull_laneqhi (__a, __b, __c);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmulls_s32 (int32_t __a, int32_t __b)
{
  return __builtin_aarch64_sqdmullsi (__a, __b);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmulls_lane_s32 (int32_t __a, int32x2_t __b, const int __c)
{
  return __builtin_aarch64_sqdmull_lanesi (__a, __b, __c);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqdmulls_laneq_s32 (int32_t __a, int32x4_t __b, const int __c)
{
  return __builtin_aarch64_sqdmull_laneqsi (__a, __b, __c);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqmovn_s16 (int16x8_t __a)
{
  return (int8x8_t) __builtin_aarch64_sqmovnv8hi (__a);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqmovn_s32 (int32x4_t __a)
{
  return (int16x4_t) __builtin_aarch64_sqmovnv4si (__a);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqmovn_s64 (int64x2_t __a)
{
  return (int32x2_t) __builtin_aarch64_sqmovnv2di (__a);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqmovn_u16 (uint16x8_t __a)
{
  return (uint8x8_t) __builtin_aarch64_uqmovnv8hi ((int16x8_t) __a);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqmovn_u32 (uint32x4_t __a)
{
  return (uint16x4_t) __builtin_aarch64_uqmovnv4si ((int32x4_t) __a);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqmovn_u64 (uint64x2_t __a)
{
  return (uint32x2_t) __builtin_aarch64_uqmovnv2di ((int64x2_t) __a);
}
__extension__ extern __inline int8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqmovnh_s16 (int16_t __a)
{
  return (int8_t) __builtin_aarch64_sqmovnhi (__a);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqmovns_s32 (int32_t __a)
{
  return (int16_t) __builtin_aarch64_sqmovnsi (__a);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqmovnd_s64 (int64_t __a)
{
  return (int32_t) __builtin_aarch64_sqmovndi (__a);
}
__extension__ extern __inline uint8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqmovnh_u16 (uint16_t __a)
{
  return (uint8_t) __builtin_aarch64_uqmovnhi (__a);
}
__extension__ extern __inline uint16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqmovns_u32 (uint32_t __a)
{
  return (uint16_t) __builtin_aarch64_uqmovnsi (__a);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqmovnd_u64 (uint64_t __a)
{
  return (uint32_t) __builtin_aarch64_uqmovndi (__a);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqmovun_s16 (int16x8_t __a)
{
  return __builtin_aarch64_sqmovunv8hi_us (__a);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqmovun_s32 (int32x4_t __a)
{
  return __builtin_aarch64_sqmovunv4si_us (__a);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqmovun_s64 (int64x2_t __a)
{
  return __builtin_aarch64_sqmovunv2di_us (__a);
}
__extension__ extern __inline uint8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqmovunh_s16 (int16_t __a)
{
  return __builtin_aarch64_sqmovunhi_us (__a);
}
__extension__ extern __inline uint16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqmovuns_s32 (int32_t __a)
{
  return __builtin_aarch64_sqmovunsi_us (__a);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqmovund_s64 (int64_t __a)
{
  return __builtin_aarch64_sqmovundi_us (__a);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqnegq_s64 (int64x2_t __a)
{
  return (int64x2_t) __builtin_aarch64_sqnegv2di (__a);
}
__extension__ extern __inline int8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqnegb_s8 (int8_t __a)
{
  return (int8_t) __builtin_aarch64_sqnegqi (__a);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqnegh_s16 (int16_t __a)
{
  return (int16_t) __builtin_aarch64_sqneghi (__a);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqnegs_s32 (int32_t __a)
{
  return (int32_t) __builtin_aarch64_sqnegsi (__a);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqnegd_s64 (int64_t __a)
{
  return __builtin_aarch64_sqnegdi (__a);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmulh_lane_s16 (int16x4_t __a, int16x4_t __b, const int __c)
{
  return __builtin_aarch64_sqrdmulh_lanev4hi (__a, __b, __c);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmulh_lane_s32 (int32x2_t __a, int32x2_t __b, const int __c)
{
  return __builtin_aarch64_sqrdmulh_lanev2si (__a, __b, __c);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmulhq_lane_s16 (int16x8_t __a, int16x4_t __b, const int __c)
{
  return __builtin_aarch64_sqrdmulh_lanev8hi (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmulhq_lane_s32 (int32x4_t __a, int32x2_t __b, const int __c)
{
  return __builtin_aarch64_sqrdmulh_lanev4si (__a, __b, __c);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmulhh_s16 (int16_t __a, int16_t __b)
{
  return (int16_t) __builtin_aarch64_sqrdmulhhi (__a, __b);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmulhh_lane_s16 (int16_t __a, int16x4_t __b, const int __c)
{
  return __builtin_aarch64_sqrdmulh_lanehi (__a, __b, __c);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmulhh_laneq_s16 (int16_t __a, int16x8_t __b, const int __c)
{
  return __builtin_aarch64_sqrdmulh_laneqhi (__a, __b, __c);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmulhs_s32 (int32_t __a, int32_t __b)
{
  return (int32_t) __builtin_aarch64_sqrdmulhsi (__a, __b);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmulhs_lane_s32 (int32_t __a, int32x2_t __b, const int __c)
{
  return __builtin_aarch64_sqrdmulh_lanesi (__a, __b, __c);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrdmulhs_laneq_s32 (int32_t __a, int32x4_t __b, const int __c)
{
  return __builtin_aarch64_sqrdmulh_laneqsi (__a, __b, __c);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshl_s8 (int8x8_t __a, int8x8_t __b)
{
  return __builtin_aarch64_sqrshlv8qi (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshl_s16 (int16x4_t __a, int16x4_t __b)
{
  return __builtin_aarch64_sqrshlv4hi (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshl_s32 (int32x2_t __a, int32x2_t __b)
{
  return __builtin_aarch64_sqrshlv2si (__a, __b);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshl_s64 (int64x1_t __a, int64x1_t __b)
{
  return (int64x1_t) {__builtin_aarch64_sqrshldi (__a[0], __b[0])};
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshl_u8 (uint8x8_t __a, int8x8_t __b)
{
  return __builtin_aarch64_uqrshlv8qi_uus ( __a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshl_u16 (uint16x4_t __a, int16x4_t __b)
{
  return __builtin_aarch64_uqrshlv4hi_uus ( __a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshl_u32 (uint32x2_t __a, int32x2_t __b)
{
  return __builtin_aarch64_uqrshlv2si_uus ( __a, __b);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshl_u64 (uint64x1_t __a, int64x1_t __b)
{
  return (uint64x1_t) {__builtin_aarch64_uqrshldi_uus (__a[0], __b[0])};
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshlq_s8 (int8x16_t __a, int8x16_t __b)
{
  return __builtin_aarch64_sqrshlv16qi (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshlq_s16 (int16x8_t __a, int16x8_t __b)
{
  return __builtin_aarch64_sqrshlv8hi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshlq_s32 (int32x4_t __a, int32x4_t __b)
{
  return __builtin_aarch64_sqrshlv4si (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshlq_s64 (int64x2_t __a, int64x2_t __b)
{
  return __builtin_aarch64_sqrshlv2di (__a, __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshlq_u8 (uint8x16_t __a, int8x16_t __b)
{
  return __builtin_aarch64_uqrshlv16qi_uus ( __a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshlq_u16 (uint16x8_t __a, int16x8_t __b)
{
  return __builtin_aarch64_uqrshlv8hi_uus ( __a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshlq_u32 (uint32x4_t __a, int32x4_t __b)
{
  return __builtin_aarch64_uqrshlv4si_uus ( __a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshlq_u64 (uint64x2_t __a, int64x2_t __b)
{
  return __builtin_aarch64_uqrshlv2di_uus ( __a, __b);
}
__extension__ extern __inline int8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshlb_s8 (int8_t __a, int8_t __b)
{
  return __builtin_aarch64_sqrshlqi (__a, __b);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshlh_s16 (int16_t __a, int16_t __b)
{
  return __builtin_aarch64_sqrshlhi (__a, __b);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshls_s32 (int32_t __a, int32_t __b)
{
  return __builtin_aarch64_sqrshlsi (__a, __b);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshld_s64 (int64_t __a, int64_t __b)
{
  return __builtin_aarch64_sqrshldi (__a, __b);
}
__extension__ extern __inline uint8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshlb_u8 (uint8_t __a, int8_t __b)
{
  return __builtin_aarch64_uqrshlqi_uus (__a, __b);
}
__extension__ extern __inline uint16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshlh_u16 (uint16_t __a, int16_t __b)
{
  return __builtin_aarch64_uqrshlhi_uus (__a, __b);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshls_u32 (uint32_t __a, int32_t __b)
{
  return __builtin_aarch64_uqrshlsi_uus (__a, __b);
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshld_u64 (uint64_t __a, int64_t __b)
{
  return __builtin_aarch64_uqrshldi_uus (__a, __b);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshrn_n_s16 (int16x8_t __a, const int __b)
{
  return (int8x8_t) __builtin_aarch64_sqrshrn_nv8hi (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshrn_n_s32 (int32x4_t __a, const int __b)
{
  return (int16x4_t) __builtin_aarch64_sqrshrn_nv4si (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshrn_n_s64 (int64x2_t __a, const int __b)
{
  return (int32x2_t) __builtin_aarch64_sqrshrn_nv2di (__a, __b);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshrn_n_u16 (uint16x8_t __a, const int __b)
{
  return __builtin_aarch64_uqrshrn_nv8hi_uus ( __a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshrn_n_u32 (uint32x4_t __a, const int __b)
{
  return __builtin_aarch64_uqrshrn_nv4si_uus ( __a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshrn_n_u64 (uint64x2_t __a, const int __b)
{
  return __builtin_aarch64_uqrshrn_nv2di_uus ( __a, __b);
}
__extension__ extern __inline int8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshrnh_n_s16 (int16_t __a, const int __b)
{
  return (int8_t) __builtin_aarch64_sqrshrn_nhi (__a, __b);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshrns_n_s32 (int32_t __a, const int __b)
{
  return (int16_t) __builtin_aarch64_sqrshrn_nsi (__a, __b);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshrnd_n_s64 (int64_t __a, const int __b)
{
  return (int32_t) __builtin_aarch64_sqrshrn_ndi (__a, __b);
}
__extension__ extern __inline uint8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshrnh_n_u16 (uint16_t __a, const int __b)
{
  return __builtin_aarch64_uqrshrn_nhi_uus (__a, __b);
}
__extension__ extern __inline uint16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshrns_n_u32 (uint32_t __a, const int __b)
{
  return __builtin_aarch64_uqrshrn_nsi_uus (__a, __b);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshrnd_n_u64 (uint64_t __a, const int __b)
{
  return __builtin_aarch64_uqrshrn_ndi_uus (__a, __b);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshrun_n_s16 (int16x8_t __a, const int __b)
{
  return (uint8x8_t) __builtin_aarch64_sqrshrun_nv8hi (__a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshrun_n_s32 (int32x4_t __a, const int __b)
{
  return (uint16x4_t) __builtin_aarch64_sqrshrun_nv4si (__a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshrun_n_s64 (int64x2_t __a, const int __b)
{
  return (uint32x2_t) __builtin_aarch64_sqrshrun_nv2di (__a, __b);
}
__extension__ extern __inline int8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshrunh_n_s16 (int16_t __a, const int __b)
{
  return (int8_t) __builtin_aarch64_sqrshrun_nhi (__a, __b);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshruns_n_s32 (int32_t __a, const int __b)
{
  return (int16_t) __builtin_aarch64_sqrshrun_nsi (__a, __b);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqrshrund_n_s64 (int64_t __a, const int __b)
{
  return (int32_t) __builtin_aarch64_sqrshrun_ndi (__a, __b);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshl_s8 (int8x8_t __a, int8x8_t __b)
{
  return __builtin_aarch64_sqshlv8qi (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshl_s16 (int16x4_t __a, int16x4_t __b)
{
  return __builtin_aarch64_sqshlv4hi (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshl_s32 (int32x2_t __a, int32x2_t __b)
{
  return __builtin_aarch64_sqshlv2si (__a, __b);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshl_s64 (int64x1_t __a, int64x1_t __b)
{
  return (int64x1_t) {__builtin_aarch64_sqshldi (__a[0], __b[0])};
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshl_u8 (uint8x8_t __a, int8x8_t __b)
{
  return __builtin_aarch64_uqshlv8qi_uus ( __a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshl_u16 (uint16x4_t __a, int16x4_t __b)
{
  return __builtin_aarch64_uqshlv4hi_uus ( __a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshl_u32 (uint32x2_t __a, int32x2_t __b)
{
  return __builtin_aarch64_uqshlv2si_uus ( __a, __b);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshl_u64 (uint64x1_t __a, int64x1_t __b)
{
  return (uint64x1_t) {__builtin_aarch64_uqshldi_uus (__a[0], __b[0])};
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshlq_s8 (int8x16_t __a, int8x16_t __b)
{
  return __builtin_aarch64_sqshlv16qi (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshlq_s16 (int16x8_t __a, int16x8_t __b)
{
  return __builtin_aarch64_sqshlv8hi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshlq_s32 (int32x4_t __a, int32x4_t __b)
{
  return __builtin_aarch64_sqshlv4si (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshlq_s64 (int64x2_t __a, int64x2_t __b)
{
  return __builtin_aarch64_sqshlv2di (__a, __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshlq_u8 (uint8x16_t __a, int8x16_t __b)
{
  return __builtin_aarch64_uqshlv16qi_uus ( __a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshlq_u16 (uint16x8_t __a, int16x8_t __b)
{
  return __builtin_aarch64_uqshlv8hi_uus ( __a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshlq_u32 (uint32x4_t __a, int32x4_t __b)
{
  return __builtin_aarch64_uqshlv4si_uus ( __a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshlq_u64 (uint64x2_t __a, int64x2_t __b)
{
  return __builtin_aarch64_uqshlv2di_uus ( __a, __b);
}
__extension__ extern __inline int8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshlb_s8 (int8_t __a, int8_t __b)
{
  return __builtin_aarch64_sqshlqi (__a, __b);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshlh_s16 (int16_t __a, int16_t __b)
{
  return __builtin_aarch64_sqshlhi (__a, __b);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshls_s32 (int32_t __a, int32_t __b)
{
  return __builtin_aarch64_sqshlsi (__a, __b);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshld_s64 (int64_t __a, int64_t __b)
{
  return __builtin_aarch64_sqshldi (__a, __b);
}
__extension__ extern __inline uint8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshlb_u8 (uint8_t __a, int8_t __b)
{
  return __builtin_aarch64_uqshlqi_uus (__a, __b);
}
__extension__ extern __inline uint16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshlh_u16 (uint16_t __a, int16_t __b)
{
  return __builtin_aarch64_uqshlhi_uus (__a, __b);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshls_u32 (uint32_t __a, int32_t __b)
{
  return __builtin_aarch64_uqshlsi_uus (__a, __b);
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshld_u64 (uint64_t __a, int64_t __b)
{
  return __builtin_aarch64_uqshldi_uus (__a, __b);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshl_n_s8 (int8x8_t __a, const int __b)
{
  return (int8x8_t) __builtin_aarch64_sqshl_nv8qi (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshl_n_s16 (int16x4_t __a, const int __b)
{
  return (int16x4_t) __builtin_aarch64_sqshl_nv4hi (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshl_n_s32 (int32x2_t __a, const int __b)
{
  return (int32x2_t) __builtin_aarch64_sqshl_nv2si (__a, __b);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshl_n_s64 (int64x1_t __a, const int __b)
{
  return (int64x1_t) {__builtin_aarch64_sqshl_ndi (__a[0], __b)};
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshl_n_u8 (uint8x8_t __a, const int __b)
{
  return __builtin_aarch64_uqshl_nv8qi_uus (__a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshl_n_u16 (uint16x4_t __a, const int __b)
{
  return __builtin_aarch64_uqshl_nv4hi_uus (__a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshl_n_u32 (uint32x2_t __a, const int __b)
{
  return __builtin_aarch64_uqshl_nv2si_uus (__a, __b);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshl_n_u64 (uint64x1_t __a, const int __b)
{
  return (uint64x1_t) {__builtin_aarch64_uqshl_ndi_uus (__a[0], __b)};
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshlq_n_s8 (int8x16_t __a, const int __b)
{
  return (int8x16_t) __builtin_aarch64_sqshl_nv16qi (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshlq_n_s16 (int16x8_t __a, const int __b)
{
  return (int16x8_t) __builtin_aarch64_sqshl_nv8hi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshlq_n_s32 (int32x4_t __a, const int __b)
{
  return (int32x4_t) __builtin_aarch64_sqshl_nv4si (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshlq_n_s64 (int64x2_t __a, const int __b)
{
  return (int64x2_t) __builtin_aarch64_sqshl_nv2di (__a, __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshlq_n_u8 (uint8x16_t __a, const int __b)
{
  return __builtin_aarch64_uqshl_nv16qi_uus (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshlq_n_u16 (uint16x8_t __a, const int __b)
{
  return __builtin_aarch64_uqshl_nv8hi_uus (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshlq_n_u32 (uint32x4_t __a, const int __b)
{
  return __builtin_aarch64_uqshl_nv4si_uus (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshlq_n_u64 (uint64x2_t __a, const int __b)
{
  return __builtin_aarch64_uqshl_nv2di_uus (__a, __b);
}
__extension__ extern __inline int8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshlb_n_s8 (int8_t __a, const int __b)
{
  return (int8_t) __builtin_aarch64_sqshl_nqi (__a, __b);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshlh_n_s16 (int16_t __a, const int __b)
{
  return (int16_t) __builtin_aarch64_sqshl_nhi (__a, __b);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshls_n_s32 (int32_t __a, const int __b)
{
  return (int32_t) __builtin_aarch64_sqshl_nsi (__a, __b);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshld_n_s64 (int64_t __a, const int __b)
{
  return __builtin_aarch64_sqshl_ndi (__a, __b);
}
__extension__ extern __inline uint8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshlb_n_u8 (uint8_t __a, const int __b)
{
  return __builtin_aarch64_uqshl_nqi_uus (__a, __b);
}
__extension__ extern __inline uint16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshlh_n_u16 (uint16_t __a, const int __b)
{
  return __builtin_aarch64_uqshl_nhi_uus (__a, __b);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshls_n_u32 (uint32_t __a, const int __b)
{
  return __builtin_aarch64_uqshl_nsi_uus (__a, __b);
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshld_n_u64 (uint64_t __a, const int __b)
{
  return __builtin_aarch64_uqshl_ndi_uus (__a, __b);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshlu_n_s8 (int8x8_t __a, const int __b)
{
  return __builtin_aarch64_sqshlu_nv8qi_uss (__a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshlu_n_s16 (int16x4_t __a, const int __b)
{
  return __builtin_aarch64_sqshlu_nv4hi_uss (__a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshlu_n_s32 (int32x2_t __a, const int __b)
{
  return __builtin_aarch64_sqshlu_nv2si_uss (__a, __b);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshlu_n_s64 (int64x1_t __a, const int __b)
{
  return (uint64x1_t) {__builtin_aarch64_sqshlu_ndi_uss (__a[0], __b)};
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshluq_n_s8 (int8x16_t __a, const int __b)
{
  return __builtin_aarch64_sqshlu_nv16qi_uss (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshluq_n_s16 (int16x8_t __a, const int __b)
{
  return __builtin_aarch64_sqshlu_nv8hi_uss (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshluq_n_s32 (int32x4_t __a, const int __b)
{
  return __builtin_aarch64_sqshlu_nv4si_uss (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshluq_n_s64 (int64x2_t __a, const int __b)
{
  return __builtin_aarch64_sqshlu_nv2di_uss (__a, __b);
}
__extension__ extern __inline int8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshlub_n_s8 (int8_t __a, const int __b)
{
  return (int8_t) __builtin_aarch64_sqshlu_nqi_uss (__a, __b);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshluh_n_s16 (int16_t __a, const int __b)
{
  return (int16_t) __builtin_aarch64_sqshlu_nhi_uss (__a, __b);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshlus_n_s32 (int32_t __a, const int __b)
{
  return (int32_t) __builtin_aarch64_sqshlu_nsi_uss (__a, __b);
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshlud_n_s64 (int64_t __a, const int __b)
{
  return __builtin_aarch64_sqshlu_ndi_uss (__a, __b);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshrn_n_s16 (int16x8_t __a, const int __b)
{
  return (int8x8_t) __builtin_aarch64_sqshrn_nv8hi (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshrn_n_s32 (int32x4_t __a, const int __b)
{
  return (int16x4_t) __builtin_aarch64_sqshrn_nv4si (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshrn_n_s64 (int64x2_t __a, const int __b)
{
  return (int32x2_t) __builtin_aarch64_sqshrn_nv2di (__a, __b);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshrn_n_u16 (uint16x8_t __a, const int __b)
{
  return __builtin_aarch64_uqshrn_nv8hi_uus ( __a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshrn_n_u32 (uint32x4_t __a, const int __b)
{
  return __builtin_aarch64_uqshrn_nv4si_uus ( __a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshrn_n_u64 (uint64x2_t __a, const int __b)
{
  return __builtin_aarch64_uqshrn_nv2di_uus ( __a, __b);
}
__extension__ extern __inline int8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshrnh_n_s16 (int16_t __a, const int __b)
{
  return (int8_t) __builtin_aarch64_sqshrn_nhi (__a, __b);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshrns_n_s32 (int32_t __a, const int __b)
{
  return (int16_t) __builtin_aarch64_sqshrn_nsi (__a, __b);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshrnd_n_s64 (int64_t __a, const int __b)
{
  return (int32_t) __builtin_aarch64_sqshrn_ndi (__a, __b);
}
__extension__ extern __inline uint8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshrnh_n_u16 (uint16_t __a, const int __b)
{
  return __builtin_aarch64_uqshrn_nhi_uus (__a, __b);
}
__extension__ extern __inline uint16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshrns_n_u32 (uint32_t __a, const int __b)
{
  return __builtin_aarch64_uqshrn_nsi_uus (__a, __b);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshrnd_n_u64 (uint64_t __a, const int __b)
{
  return __builtin_aarch64_uqshrn_ndi_uus (__a, __b);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshrun_n_s16 (int16x8_t __a, const int __b)
{
  return (uint8x8_t) __builtin_aarch64_sqshrun_nv8hi (__a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshrun_n_s32 (int32x4_t __a, const int __b)
{
  return (uint16x4_t) __builtin_aarch64_sqshrun_nv4si (__a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshrun_n_s64 (int64x2_t __a, const int __b)
{
  return (uint32x2_t) __builtin_aarch64_sqshrun_nv2di (__a, __b);
}
__extension__ extern __inline int8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshrunh_n_s16 (int16_t __a, const int __b)
{
  return (int8_t) __builtin_aarch64_sqshrun_nhi (__a, __b);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshruns_n_s32 (int32_t __a, const int __b)
{
  return (int16_t) __builtin_aarch64_sqshrun_nsi (__a, __b);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqshrund_n_s64 (int64_t __a, const int __b)
{
  return (int32_t) __builtin_aarch64_sqshrun_ndi (__a, __b);
}
__extension__ extern __inline int8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqsubb_s8 (int8_t __a, int8_t __b)
{
  return (int8_t) __builtin_aarch64_sqsubqi (__a, __b);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqsubh_s16 (int16_t __a, int16_t __b)
{
  return (int16_t) __builtin_aarch64_sqsubhi (__a, __b);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqsubs_s32 (int32_t __a, int32_t __b)
{
  return (int32_t) __builtin_aarch64_sqsubsi (__a, __b);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqsubd_s64 (int64_t __a, int64_t __b)
{
  return __builtin_aarch64_sqsubdi (__a, __b);
}
__extension__ extern __inline uint8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqsubb_u8 (uint8_t __a, uint8_t __b)
{
  return (uint8_t) __builtin_aarch64_uqsubqi_uuu (__a, __b);
}
__extension__ extern __inline uint16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqsubh_u16 (uint16_t __a, uint16_t __b)
{
  return (uint16_t) __builtin_aarch64_uqsubhi_uuu (__a, __b);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqsubs_u32 (uint32_t __a, uint32_t __b)
{
  return (uint32_t) __builtin_aarch64_uqsubsi_uuu (__a, __b);
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqsubd_u64 (uint64_t __a, uint64_t __b)
{
  return __builtin_aarch64_uqsubdi_uuu (__a, __b);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbl2_s8 (int8x16x2_t __tab, uint8x8_t __idx)
{
  return __builtin_aarch64_qtbl2v8qi_ssu (__tab, __idx);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbl2_u8 (uint8x16x2_t __tab, uint8x8_t __idx)
{
  return __builtin_aarch64_qtbl2v8qi_uuu (__tab, __idx);
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbl2_p8 (poly8x16x2_t __tab, uint8x8_t __idx)
{
  return __builtin_aarch64_qtbl2v8qi_ppu (__tab, __idx);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbl2q_s8 (int8x16x2_t __tab, uint8x16_t __idx)
{
  return __builtin_aarch64_qtbl2v16qi_ssu (__tab, __idx);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbl2q_u8 (uint8x16x2_t __tab, uint8x16_t __idx)
{
  return __builtin_aarch64_qtbl2v16qi_uuu (__tab, __idx);
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbl2q_p8 (poly8x16x2_t __tab, uint8x16_t __idx)
{
  return __builtin_aarch64_qtbl2v16qi_ppu (__tab, __idx);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbl3_s8 (int8x16x3_t __tab, uint8x8_t __idx)
{
  return __builtin_aarch64_qtbl3v8qi_ssu (__tab, __idx);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbl3_u8 (uint8x16x3_t __tab, uint8x8_t __idx)
{
  return __builtin_aarch64_qtbl3v8qi_uuu (__tab, __idx);
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbl3_p8 (poly8x16x3_t __tab, uint8x8_t __idx)
{
  return __builtin_aarch64_qtbl3v8qi_ppu (__tab, __idx);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbl3q_s8 (int8x16x3_t __tab, uint8x16_t __idx)
{
  return __builtin_aarch64_qtbl3v16qi_ssu (__tab, __idx);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbl3q_u8 (uint8x16x3_t __tab, uint8x16_t __idx)
{
  return __builtin_aarch64_qtbl3v16qi_uuu (__tab, __idx);
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbl3q_p8 (poly8x16x3_t __tab, uint8x16_t __idx)
{
  return __builtin_aarch64_qtbl3v16qi_ppu (__tab, __idx);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbl4_s8 (int8x16x4_t __tab, uint8x8_t __idx)
{
  return __builtin_aarch64_qtbl4v8qi_ssu (__tab, __idx);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbl4_u8 (uint8x16x4_t __tab, uint8x8_t __idx)
{
  return __builtin_aarch64_qtbl4v8qi_uuu (__tab, __idx);
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbl4_p8 (poly8x16x4_t __tab, uint8x8_t __idx)
{
  return __builtin_aarch64_qtbl4v8qi_ppu (__tab, __idx);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbl4q_s8 (int8x16x4_t __tab, uint8x16_t __idx)
{
  return __builtin_aarch64_qtbl4v16qi_ssu (__tab, __idx);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbl4q_u8 (uint8x16x4_t __tab, uint8x16_t __idx)
{
  return __builtin_aarch64_qtbl4v16qi_uuu (__tab, __idx);
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbl4q_p8 (poly8x16x4_t __tab, uint8x16_t __idx)
{
  return __builtin_aarch64_qtbl4v16qi_ppu (__tab, __idx);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbx2_s8 (int8x8_t __r, int8x16x2_t __tab, uint8x8_t __idx)
{
  return __builtin_aarch64_qtbx2v8qi_sssu (__r, __tab, __idx);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbx2_u8 (uint8x8_t __r, uint8x16x2_t __tab, uint8x8_t __idx)
{
  return __builtin_aarch64_qtbx2v8qi_uuuu (__r, __tab, __idx);
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbx2_p8 (poly8x8_t __r, poly8x16x2_t __tab, uint8x8_t __idx)
{
  return __builtin_aarch64_qtbx2v8qi_pppu (__r, __tab, __idx);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbx2q_s8 (int8x16_t __r, int8x16x2_t __tab, uint8x16_t __idx)
{
  return __builtin_aarch64_qtbx2v16qi_sssu (__r, __tab, __idx);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbx2q_u8 (uint8x16_t __r, uint8x16x2_t __tab, uint8x16_t __idx)
{
  return __builtin_aarch64_qtbx2v16qi_uuuu (__r, __tab, __idx);
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbx2q_p8 (poly8x16_t __r, poly8x16x2_t __tab, uint8x16_t __idx)
{
  return __builtin_aarch64_qtbx2v16qi_pppu (__r, __tab, __idx);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbx3_s8 (int8x8_t __r, int8x16x3_t __tab, uint8x8_t __idx)
{
  return __builtin_aarch64_qtbx3v8qi_sssu (__r, __tab, __idx);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbx3_u8 (uint8x8_t __r, uint8x16x3_t __tab, uint8x8_t __idx)
{
  return __builtin_aarch64_qtbx3v8qi_uuuu (__r, __tab, __idx);
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbx3_p8 (poly8x8_t __r, poly8x16x3_t __tab, uint8x8_t __idx)
{
  return __builtin_aarch64_qtbx3v8qi_pppu (__r, __tab, __idx);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbx3q_s8 (int8x16_t __r, int8x16x3_t __tab, uint8x16_t __idx)
{
  return __builtin_aarch64_qtbx3v16qi_sssu (__r, __tab, __idx);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbx3q_u8 (uint8x16_t __r, uint8x16x3_t __tab, uint8x16_t __idx)
{
  return __builtin_aarch64_qtbx3v16qi_uuuu (__r, __tab, __idx);
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbx3q_p8 (poly8x16_t __r, poly8x16x3_t __tab, uint8x16_t __idx)
{
  return __builtin_aarch64_qtbx3v16qi_pppu (__r, __tab, __idx);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbx4_s8 (int8x8_t __r, int8x16x4_t __tab, uint8x8_t __idx)
{
  return __builtin_aarch64_qtbx4v8qi_sssu (__r, __tab, __idx);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbx4_u8 (uint8x8_t __r, uint8x16x4_t __tab, uint8x8_t __idx)
{
  return __builtin_aarch64_qtbx4v8qi_uuuu (__r, __tab, __idx);
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbx4_p8 (poly8x8_t __r, poly8x16x4_t __tab, uint8x8_t __idx)
{
  return __builtin_aarch64_qtbx4v8qi_pppu (__r, __tab, __idx);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbx4q_s8 (int8x16_t __r, int8x16x4_t __tab, uint8x16_t __idx)
{
  return __builtin_aarch64_qtbx4v16qi_sssu (__r, __tab, __idx);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbx4q_u8 (uint8x16_t __r, uint8x16x4_t __tab, uint8x16_t __idx)
{
  return __builtin_aarch64_qtbx4v16qi_uuuu (__r, __tab, __idx);
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vqtbx4q_p8 (poly8x16_t __r, poly8x16x4_t __tab, uint8x16_t __idx)
{
  return __builtin_aarch64_qtbx4v16qi_pppu (__r, __tab, __idx);
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrbit_p8 (poly8x8_t __a)
{
  return (poly8x8_t) __builtin_aarch64_rbitv8qi ((int8x8_t) __a);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrbit_s8 (int8x8_t __a)
{
  return __builtin_aarch64_rbitv8qi (__a);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrbit_u8 (uint8x8_t __a)
{
  return (uint8x8_t) __builtin_aarch64_rbitv8qi ((int8x8_t) __a);
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrbitq_p8 (poly8x16_t __a)
{
  return (poly8x16_t) __builtin_aarch64_rbitv16qi ((int8x16_t)__a);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrbitq_s8 (int8x16_t __a)
{
  return __builtin_aarch64_rbitv16qi (__a);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrbitq_u8 (uint8x16_t __a)
{
  return (uint8x16_t) __builtin_aarch64_rbitv16qi ((int8x16_t) __a);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrecpe_u32 (uint32x2_t __a)
{
  return (uint32x2_t) __builtin_aarch64_urecpev2si ((int32x2_t) __a);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrecpeq_u32 (uint32x4_t __a)
{
  return (uint32x4_t) __builtin_aarch64_urecpev4si ((int32x4_t) __a);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrecpes_f32 (float32_t __a)
{
  return __builtin_aarch64_frecpesf (__a);
}
__extension__ extern __inline float64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrecped_f64 (float64_t __a)
{
  return __builtin_aarch64_frecpedf (__a);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrecpe_f32 (float32x2_t __a)
{
  return __builtin_aarch64_frecpev2sf (__a);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrecpe_f64 (float64x1_t __a)
{
  return (float64x1_t) { vrecped_f64 (vget_lane_f64 (__a, 0)) };
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrecpeq_f32 (float32x4_t __a)
{
  return __builtin_aarch64_frecpev4sf (__a);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrecpeq_f64 (float64x2_t __a)
{
  return __builtin_aarch64_frecpev2df (__a);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrecpss_f32 (float32_t __a, float32_t __b)
{
  return __builtin_aarch64_frecpssf (__a, __b);
}
__extension__ extern __inline float64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrecpsd_f64 (float64_t __a, float64_t __b)
{
  return __builtin_aarch64_frecpsdf (__a, __b);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrecps_f32 (float32x2_t __a, float32x2_t __b)
{
  return __builtin_aarch64_frecpsv2sf (__a, __b);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrecps_f64 (float64x1_t __a, float64x1_t __b)
{
  return (float64x1_t) { vrecpsd_f64 (vget_lane_f64 (__a, 0),
          vget_lane_f64 (__b, 0)) };
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrecpsq_f32 (float32x4_t __a, float32x4_t __b)
{
  return __builtin_aarch64_frecpsv4sf (__a, __b);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrecpsq_f64 (float64x2_t __a, float64x2_t __b)
{
  return __builtin_aarch64_frecpsv2df (__a, __b);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrecpxs_f32 (float32_t __a)
{
  return __builtin_aarch64_frecpxsf (__a);
}
__extension__ extern __inline float64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrecpxd_f64 (float64_t __a)
{
  return __builtin_aarch64_frecpxdf (__a);
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev16_p8 (poly8x8_t __a)
{
  return __builtin_shuffle (__a, (uint8x8_t) { 1, 0, 3, 2, 5, 4, 7, 6 });
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev16_s8 (int8x8_t __a)
{
  return __builtin_shuffle (__a, (uint8x8_t) { 1, 0, 3, 2, 5, 4, 7, 6 });
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev16_u8 (uint8x8_t __a)
{
  return __builtin_shuffle (__a, (uint8x8_t) { 1, 0, 3, 2, 5, 4, 7, 6 });
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev16q_p8 (poly8x16_t __a)
{
  return __builtin_shuffle (__a,
      (uint8x16_t) { 1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14 });
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev16q_s8 (int8x16_t __a)
{
  return __builtin_shuffle (__a,
      (uint8x16_t) { 1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14 });
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev16q_u8 (uint8x16_t __a)
{
  return __builtin_shuffle (__a,
      (uint8x16_t) { 1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14 });
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev32_p8 (poly8x8_t __a)
{
  return __builtin_shuffle (__a, (uint8x8_t) { 3, 2, 1, 0, 7, 6, 5, 4 });
}
__extension__ extern __inline poly16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev32_p16 (poly16x4_t __a)
{
  return __builtin_shuffle (__a, (uint16x4_t) { 1, 0, 3, 2 });
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev32_s8 (int8x8_t __a)
{
  return __builtin_shuffle (__a, (uint8x8_t) { 3, 2, 1, 0, 7, 6, 5, 4 });
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev32_s16 (int16x4_t __a)
{
  return __builtin_shuffle (__a, (uint16x4_t) { 1, 0, 3, 2 });
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev32_u8 (uint8x8_t __a)
{
  return __builtin_shuffle (__a, (uint8x8_t) { 3, 2, 1, 0, 7, 6, 5, 4 });
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev32_u16 (uint16x4_t __a)
{
  return __builtin_shuffle (__a, (uint16x4_t) { 1, 0, 3, 2 });
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev32q_p8 (poly8x16_t __a)
{
  return __builtin_shuffle (__a,
      (uint8x16_t) { 3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12 });
}
__extension__ extern __inline poly16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev32q_p16 (poly16x8_t __a)
{
  return __builtin_shuffle (__a, (uint16x8_t) { 1, 0, 3, 2, 5, 4, 7, 6 });
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev32q_s8 (int8x16_t __a)
{
  return __builtin_shuffle (__a,
      (uint8x16_t) { 3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12 });
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev32q_s16 (int16x8_t __a)
{
  return __builtin_shuffle (__a, (uint16x8_t) { 1, 0, 3, 2, 5, 4, 7, 6 });
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev32q_u8 (uint8x16_t __a)
{
  return __builtin_shuffle (__a,
      (uint8x16_t) { 3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12 });
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev32q_u16 (uint16x8_t __a)
{
  return __builtin_shuffle (__a, (uint16x8_t) { 1, 0, 3, 2, 5, 4, 7, 6 });
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev64_f16 (float16x4_t __a)
{
  return __builtin_shuffle (__a, (uint16x4_t) { 3, 2, 1, 0 });
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev64_f32 (float32x2_t __a)
{
  return __builtin_shuffle (__a, (uint32x2_t) { 1, 0 });
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev64_p8 (poly8x8_t __a)
{
  return __builtin_shuffle (__a, (uint8x8_t) { 7, 6, 5, 4, 3, 2, 1, 0 });
}
__extension__ extern __inline poly16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev64_p16 (poly16x4_t __a)
{
  return __builtin_shuffle (__a, (uint16x4_t) { 3, 2, 1, 0 });
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev64_s8 (int8x8_t __a)
{
  return __builtin_shuffle (__a, (uint8x8_t) { 7, 6, 5, 4, 3, 2, 1, 0 });
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev64_s16 (int16x4_t __a)
{
  return __builtin_shuffle (__a, (uint16x4_t) { 3, 2, 1, 0 });
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev64_s32 (int32x2_t __a)
{
  return __builtin_shuffle (__a, (uint32x2_t) { 1, 0 });
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev64_u8 (uint8x8_t __a)
{
  return __builtin_shuffle (__a, (uint8x8_t) { 7, 6, 5, 4, 3, 2, 1, 0 });
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev64_u16 (uint16x4_t __a)
{
  return __builtin_shuffle (__a, (uint16x4_t) { 3, 2, 1, 0 });
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev64_u32 (uint32x2_t __a)
{
  return __builtin_shuffle (__a, (uint32x2_t) { 1, 0 });
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev64q_f16 (float16x8_t __a)
{
  return __builtin_shuffle (__a, (uint16x8_t) { 3, 2, 1, 0, 7, 6, 5, 4 });
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev64q_f32 (float32x4_t __a)
{
  return __builtin_shuffle (__a, (uint32x4_t) { 1, 0, 3, 2 });
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev64q_p8 (poly8x16_t __a)
{
  return __builtin_shuffle (__a,
      (uint8x16_t) { 7, 6, 5, 4, 3, 2, 1, 0, 15, 14, 13, 12, 11, 10, 9, 8 });
}
__extension__ extern __inline poly16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev64q_p16 (poly16x8_t __a)
{
  return __builtin_shuffle (__a, (uint16x8_t) { 3, 2, 1, 0, 7, 6, 5, 4 });
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev64q_s8 (int8x16_t __a)
{
  return __builtin_shuffle (__a,
      (uint8x16_t) { 7, 6, 5, 4, 3, 2, 1, 0, 15, 14, 13, 12, 11, 10, 9, 8 });
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev64q_s16 (int16x8_t __a)
{
  return __builtin_shuffle (__a, (uint16x8_t) { 3, 2, 1, 0, 7, 6, 5, 4 });
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev64q_s32 (int32x4_t __a)
{
  return __builtin_shuffle (__a, (uint32x4_t) { 1, 0, 3, 2 });
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev64q_u8 (uint8x16_t __a)
{
  return __builtin_shuffle (__a,
      (uint8x16_t) { 7, 6, 5, 4, 3, 2, 1, 0, 15, 14, 13, 12, 11, 10, 9, 8 });
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev64q_u16 (uint16x8_t __a)
{
  return __builtin_shuffle (__a, (uint16x8_t) { 3, 2, 1, 0, 7, 6, 5, 4 });
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrev64q_u32 (uint32x4_t __a)
{
  return __builtin_shuffle (__a, (uint32x4_t) { 1, 0, 3, 2 });
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrnd_f32 (float32x2_t __a)
{
  return __builtin_aarch64_btruncv2sf (__a);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrnd_f64 (float64x1_t __a)
{
  return vset_lane_f64 (__builtin_trunc (vget_lane_f64 (__a, 0)), __a, 0);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndq_f32 (float32x4_t __a)
{
  return __builtin_aarch64_btruncv4sf (__a);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndq_f64 (float64x2_t __a)
{
  return __builtin_aarch64_btruncv2df (__a);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrnda_f32 (float32x2_t __a)
{
  return __builtin_aarch64_roundv2sf (__a);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrnda_f64 (float64x1_t __a)
{
  return vset_lane_f64 (__builtin_round (vget_lane_f64 (__a, 0)), __a, 0);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndaq_f32 (float32x4_t __a)
{
  return __builtin_aarch64_roundv4sf (__a);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndaq_f64 (float64x2_t __a)
{
  return __builtin_aarch64_roundv2df (__a);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndi_f32 (float32x2_t __a)
{
  return __builtin_aarch64_nearbyintv2sf (__a);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndi_f64 (float64x1_t __a)
{
  return vset_lane_f64 (__builtin_nearbyint (vget_lane_f64 (__a, 0)), __a, 0);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndiq_f32 (float32x4_t __a)
{
  return __builtin_aarch64_nearbyintv4sf (__a);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndiq_f64 (float64x2_t __a)
{
  return __builtin_aarch64_nearbyintv2df (__a);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndm_f32 (float32x2_t __a)
{
  return __builtin_aarch64_floorv2sf (__a);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndm_f64 (float64x1_t __a)
{
  return vset_lane_f64 (__builtin_floor (vget_lane_f64 (__a, 0)), __a, 0);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndmq_f32 (float32x4_t __a)
{
  return __builtin_aarch64_floorv4sf (__a);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndmq_f64 (float64x2_t __a)
{
  return __builtin_aarch64_floorv2df (__a);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndns_f32 (float32_t __a)
{
  return __builtin_aarch64_roundevensf (__a);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndn_f32 (float32x2_t __a)
{
  return __builtin_aarch64_roundevenv2sf (__a);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndn_f64 (float64x1_t __a)
{
  return (float64x1_t) {__builtin_aarch64_roundevendf (__a[0])};
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndnq_f32 (float32x4_t __a)
{
  return __builtin_aarch64_roundevenv4sf (__a);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndnq_f64 (float64x2_t __a)
{
  return __builtin_aarch64_roundevenv2df (__a);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndp_f32 (float32x2_t __a)
{
  return __builtin_aarch64_ceilv2sf (__a);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndp_f64 (float64x1_t __a)
{
  return vset_lane_f64 (__builtin_ceil (vget_lane_f64 (__a, 0)), __a, 0);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndpq_f32 (float32x4_t __a)
{
  return __builtin_aarch64_ceilv4sf (__a);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndpq_f64 (float64x2_t __a)
{
  return __builtin_aarch64_ceilv2df (__a);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndx_f32 (float32x2_t __a)
{
  return __builtin_aarch64_rintv2sf (__a);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndx_f64 (float64x1_t __a)
{
  return vset_lane_f64 (__builtin_rint (vget_lane_f64 (__a, 0)), __a, 0);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndxq_f32 (float32x4_t __a)
{
  return __builtin_aarch64_rintv4sf (__a);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndxq_f64 (float64x2_t __a)
{
  return __builtin_aarch64_rintv2df (__a);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshl_s8 (int8x8_t __a, int8x8_t __b)
{
  return (int8x8_t) __builtin_aarch64_srshlv8qi (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshl_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int16x4_t) __builtin_aarch64_srshlv4hi (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshl_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int32x2_t) __builtin_aarch64_srshlv2si (__a, __b);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshl_s64 (int64x1_t __a, int64x1_t __b)
{
  return (int64x1_t) {__builtin_aarch64_srshldi (__a[0], __b[0])};
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshl_u8 (uint8x8_t __a, int8x8_t __b)
{
  return __builtin_aarch64_urshlv8qi_uus (__a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshl_u16 (uint16x4_t __a, int16x4_t __b)
{
  return __builtin_aarch64_urshlv4hi_uus (__a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshl_u32 (uint32x2_t __a, int32x2_t __b)
{
  return __builtin_aarch64_urshlv2si_uus (__a, __b);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshl_u64 (uint64x1_t __a, int64x1_t __b)
{
  return (uint64x1_t) {__builtin_aarch64_urshldi_uus (__a[0], __b[0])};
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshlq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (int8x16_t) __builtin_aarch64_srshlv16qi (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshlq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (int16x8_t) __builtin_aarch64_srshlv8hi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshlq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (int32x4_t) __builtin_aarch64_srshlv4si (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshlq_s64 (int64x2_t __a, int64x2_t __b)
{
  return (int64x2_t) __builtin_aarch64_srshlv2di (__a, __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshlq_u8 (uint8x16_t __a, int8x16_t __b)
{
  return __builtin_aarch64_urshlv16qi_uus (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshlq_u16 (uint16x8_t __a, int16x8_t __b)
{
  return __builtin_aarch64_urshlv8hi_uus (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshlq_u32 (uint32x4_t __a, int32x4_t __b)
{
  return __builtin_aarch64_urshlv4si_uus (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshlq_u64 (uint64x2_t __a, int64x2_t __b)
{
  return __builtin_aarch64_urshlv2di_uus (__a, __b);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshld_s64 (int64_t __a, int64_t __b)
{
  return __builtin_aarch64_srshldi (__a, __b);
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshld_u64 (uint64_t __a, int64_t __b)
{
  return __builtin_aarch64_urshldi_uus (__a, __b);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshr_n_s8 (int8x8_t __a, const int __b)
{
  return (int8x8_t) __builtin_aarch64_srshr_nv8qi (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshr_n_s16 (int16x4_t __a, const int __b)
{
  return (int16x4_t) __builtin_aarch64_srshr_nv4hi (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshr_n_s32 (int32x2_t __a, const int __b)
{
  return (int32x2_t) __builtin_aarch64_srshr_nv2si (__a, __b);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshr_n_s64 (int64x1_t __a, const int __b)
{
  return (int64x1_t) {__builtin_aarch64_srshr_ndi (__a[0], __b)};
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshr_n_u8 (uint8x8_t __a, const int __b)
{
  return __builtin_aarch64_urshr_nv8qi_uus (__a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshr_n_u16 (uint16x4_t __a, const int __b)
{
  return __builtin_aarch64_urshr_nv4hi_uus (__a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshr_n_u32 (uint32x2_t __a, const int __b)
{
  return __builtin_aarch64_urshr_nv2si_uus (__a, __b);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshr_n_u64 (uint64x1_t __a, const int __b)
{
  return (uint64x1_t) {__builtin_aarch64_urshr_ndi_uus (__a[0], __b)};
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshrq_n_s8 (int8x16_t __a, const int __b)
{
  return (int8x16_t) __builtin_aarch64_srshr_nv16qi (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshrq_n_s16 (int16x8_t __a, const int __b)
{
  return (int16x8_t) __builtin_aarch64_srshr_nv8hi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshrq_n_s32 (int32x4_t __a, const int __b)
{
  return (int32x4_t) __builtin_aarch64_srshr_nv4si (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshrq_n_s64 (int64x2_t __a, const int __b)
{
  return (int64x2_t) __builtin_aarch64_srshr_nv2di (__a, __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshrq_n_u8 (uint8x16_t __a, const int __b)
{
  return __builtin_aarch64_urshr_nv16qi_uus (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshrq_n_u16 (uint16x8_t __a, const int __b)
{
  return __builtin_aarch64_urshr_nv8hi_uus (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshrq_n_u32 (uint32x4_t __a, const int __b)
{
  return __builtin_aarch64_urshr_nv4si_uus (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshrq_n_u64 (uint64x2_t __a, const int __b)
{
  return __builtin_aarch64_urshr_nv2di_uus (__a, __b);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshrd_n_s64 (int64_t __a, const int __b)
{
  return __builtin_aarch64_srshr_ndi (__a, __b);
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrshrd_n_u64 (uint64_t __a, const int __b)
{
  return __builtin_aarch64_urshr_ndi_uus (__a, __b);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsqrtes_f32 (float32_t __a)
{
  return __builtin_aarch64_rsqrtesf (__a);
}
__extension__ extern __inline float64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsqrted_f64 (float64_t __a)
{
  return __builtin_aarch64_rsqrtedf (__a);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsqrte_f32 (float32x2_t __a)
{
  return __builtin_aarch64_rsqrtev2sf (__a);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsqrte_f64 (float64x1_t __a)
{
  return (float64x1_t) {vrsqrted_f64 (vget_lane_f64 (__a, 0))};
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsqrteq_f32 (float32x4_t __a)
{
  return __builtin_aarch64_rsqrtev4sf (__a);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsqrteq_f64 (float64x2_t __a)
{
  return __builtin_aarch64_rsqrtev2df (__a);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsqrtss_f32 (float32_t __a, float32_t __b)
{
  return __builtin_aarch64_rsqrtssf (__a, __b);
}
__extension__ extern __inline float64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsqrtsd_f64 (float64_t __a, float64_t __b)
{
  return __builtin_aarch64_rsqrtsdf (__a, __b);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsqrts_f32 (float32x2_t __a, float32x2_t __b)
{
  return __builtin_aarch64_rsqrtsv2sf (__a, __b);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsqrts_f64 (float64x1_t __a, float64x1_t __b)
{
  return (float64x1_t) {vrsqrtsd_f64 (vget_lane_f64 (__a, 0),
          vget_lane_f64 (__b, 0))};
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsqrtsq_f32 (float32x4_t __a, float32x4_t __b)
{
  return __builtin_aarch64_rsqrtsv4sf (__a, __b);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsqrtsq_f64 (float64x2_t __a, float64x2_t __b)
{
  return __builtin_aarch64_rsqrtsv2df (__a, __b);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsra_n_s8 (int8x8_t __a, int8x8_t __b, const int __c)
{
  return (int8x8_t) __builtin_aarch64_srsra_nv8qi (__a, __b, __c);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsra_n_s16 (int16x4_t __a, int16x4_t __b, const int __c)
{
  return (int16x4_t) __builtin_aarch64_srsra_nv4hi (__a, __b, __c);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsra_n_s32 (int32x2_t __a, int32x2_t __b, const int __c)
{
  return (int32x2_t) __builtin_aarch64_srsra_nv2si (__a, __b, __c);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsra_n_s64 (int64x1_t __a, int64x1_t __b, const int __c)
{
  return (int64x1_t) {__builtin_aarch64_srsra_ndi (__a[0], __b[0], __c)};
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsra_n_u8 (uint8x8_t __a, uint8x8_t __b, const int __c)
{
  return __builtin_aarch64_ursra_nv8qi_uuus (__a, __b, __c);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsra_n_u16 (uint16x4_t __a, uint16x4_t __b, const int __c)
{
  return __builtin_aarch64_ursra_nv4hi_uuus (__a, __b, __c);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsra_n_u32 (uint32x2_t __a, uint32x2_t __b, const int __c)
{
  return __builtin_aarch64_ursra_nv2si_uuus (__a, __b, __c);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsra_n_u64 (uint64x1_t __a, uint64x1_t __b, const int __c)
{
  return (uint64x1_t) {__builtin_aarch64_ursra_ndi_uuus (__a[0], __b[0], __c)};
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsraq_n_s8 (int8x16_t __a, int8x16_t __b, const int __c)
{
  return (int8x16_t) __builtin_aarch64_srsra_nv16qi (__a, __b, __c);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsraq_n_s16 (int16x8_t __a, int16x8_t __b, const int __c)
{
  return (int16x8_t) __builtin_aarch64_srsra_nv8hi (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsraq_n_s32 (int32x4_t __a, int32x4_t __b, const int __c)
{
  return (int32x4_t) __builtin_aarch64_srsra_nv4si (__a, __b, __c);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsraq_n_s64 (int64x2_t __a, int64x2_t __b, const int __c)
{
  return (int64x2_t) __builtin_aarch64_srsra_nv2di (__a, __b, __c);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsraq_n_u8 (uint8x16_t __a, uint8x16_t __b, const int __c)
{
  return __builtin_aarch64_ursra_nv16qi_uuus (__a, __b, __c);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsraq_n_u16 (uint16x8_t __a, uint16x8_t __b, const int __c)
{
  return __builtin_aarch64_ursra_nv8hi_uuus (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsraq_n_u32 (uint32x4_t __a, uint32x4_t __b, const int __c)
{
  return __builtin_aarch64_ursra_nv4si_uuus (__a, __b, __c);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsraq_n_u64 (uint64x2_t __a, uint64x2_t __b, const int __c)
{
  return __builtin_aarch64_ursra_nv2di_uuus (__a, __b, __c);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsrad_n_s64 (int64_t __a, int64_t __b, const int __c)
{
  return __builtin_aarch64_srsra_ndi (__a, __b, __c);
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsrad_n_u64 (uint64_t __a, uint64_t __b, const int __c)
{
  return __builtin_aarch64_ursra_ndi_uuus (__a, __b, __c);
}
#pragma GCC push_options
#pragma GCC target ("+nothing+sha2")
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsha1cq_u32 (uint32x4_t __hash_abcd, uint32_t __hash_e, uint32x4_t __wk)
{
  return __builtin_aarch64_crypto_sha1cv4si_uuuu (__hash_abcd, __hash_e, __wk);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsha1mq_u32 (uint32x4_t __hash_abcd, uint32_t __hash_e, uint32x4_t __wk)
{
  return __builtin_aarch64_crypto_sha1mv4si_uuuu (__hash_abcd, __hash_e, __wk);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsha1pq_u32 (uint32x4_t __hash_abcd, uint32_t __hash_e, uint32x4_t __wk)
{
  return __builtin_aarch64_crypto_sha1pv4si_uuuu (__hash_abcd, __hash_e, __wk);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsha1h_u32 (uint32_t __hash_e)
{
  return __builtin_aarch64_crypto_sha1hsi_uu (__hash_e);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsha1su0q_u32 (uint32x4_t __w0_3, uint32x4_t __w4_7, uint32x4_t __w8_11)
{
  return __builtin_aarch64_crypto_sha1su0v4si_uuuu (__w0_3, __w4_7, __w8_11);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsha1su1q_u32 (uint32x4_t __tw0_3, uint32x4_t __w12_15)
{
  return __builtin_aarch64_crypto_sha1su1v4si_uuu (__tw0_3, __w12_15);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsha256hq_u32 (uint32x4_t __hash_abcd, uint32x4_t __hash_efgh, uint32x4_t __wk)
{
  return __builtin_aarch64_crypto_sha256hv4si_uuuu (__hash_abcd, __hash_efgh,
           __wk);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsha256h2q_u32 (uint32x4_t __hash_efgh, uint32x4_t __hash_abcd, uint32x4_t __wk)
{
  return __builtin_aarch64_crypto_sha256h2v4si_uuuu (__hash_efgh, __hash_abcd,
            __wk);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsha256su0q_u32 (uint32x4_t __w0_3, uint32x4_t __w4_7)
{
  return __builtin_aarch64_crypto_sha256su0v4si_uuu (__w0_3, __w4_7);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsha256su1q_u32 (uint32x4_t __tw0_3, uint32x4_t __w8_11, uint32x4_t __w12_15)
{
  return __builtin_aarch64_crypto_sha256su1v4si_uuuu (__tw0_3, __w8_11,
             __w12_15);
}
#pragma GCC pop_options
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshl_n_s8 (int8x8_t __a, const int __b)
{
  return (int8x8_t) __builtin_aarch64_ashlv8qi (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshl_n_s16 (int16x4_t __a, const int __b)
{
  return (int16x4_t) __builtin_aarch64_ashlv4hi (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshl_n_s32 (int32x2_t __a, const int __b)
{
  return (int32x2_t) __builtin_aarch64_ashlv2si (__a, __b);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshl_n_s64 (int64x1_t __a, const int __b)
{
  return (int64x1_t) {__builtin_aarch64_ashldi (__a[0], __b)};
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshl_n_u8 (uint8x8_t __a, const int __b)
{
  return (uint8x8_t) __builtin_aarch64_ashlv8qi ((int8x8_t) __a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshl_n_u16 (uint16x4_t __a, const int __b)
{
  return (uint16x4_t) __builtin_aarch64_ashlv4hi ((int16x4_t) __a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshl_n_u32 (uint32x2_t __a, const int __b)
{
  return (uint32x2_t) __builtin_aarch64_ashlv2si ((int32x2_t) __a, __b);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshl_n_u64 (uint64x1_t __a, const int __b)
{
  return (uint64x1_t) {__builtin_aarch64_ashldi ((int64_t) __a[0], __b)};
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshlq_n_s8 (int8x16_t __a, const int __b)
{
  return (int8x16_t) __builtin_aarch64_ashlv16qi (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshlq_n_s16 (int16x8_t __a, const int __b)
{
  return (int16x8_t) __builtin_aarch64_ashlv8hi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshlq_n_s32 (int32x4_t __a, const int __b)
{
  return (int32x4_t) __builtin_aarch64_ashlv4si (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshlq_n_s64 (int64x2_t __a, const int __b)
{
  return (int64x2_t) __builtin_aarch64_ashlv2di (__a, __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshlq_n_u8 (uint8x16_t __a, const int __b)
{
  return (uint8x16_t) __builtin_aarch64_ashlv16qi ((int8x16_t) __a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshlq_n_u16 (uint16x8_t __a, const int __b)
{
  return (uint16x8_t) __builtin_aarch64_ashlv8hi ((int16x8_t) __a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshlq_n_u32 (uint32x4_t __a, const int __b)
{
  return (uint32x4_t) __builtin_aarch64_ashlv4si ((int32x4_t) __a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshlq_n_u64 (uint64x2_t __a, const int __b)
{
  return (uint64x2_t) __builtin_aarch64_ashlv2di ((int64x2_t) __a, __b);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshld_n_s64 (int64_t __a, const int __b)
{
  return __builtin_aarch64_ashldi (__a, __b);
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshld_n_u64 (uint64_t __a, const int __b)
{
  return (uint64_t) __builtin_aarch64_ashldi (__a, __b);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshl_s8 (int8x8_t __a, int8x8_t __b)
{
  return __builtin_aarch64_sshlv8qi (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshl_s16 (int16x4_t __a, int16x4_t __b)
{
  return __builtin_aarch64_sshlv4hi (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshl_s32 (int32x2_t __a, int32x2_t __b)
{
  return __builtin_aarch64_sshlv2si (__a, __b);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshl_s64 (int64x1_t __a, int64x1_t __b)
{
  return (int64x1_t) {__builtin_aarch64_sshldi (__a[0], __b[0])};
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshl_u8 (uint8x8_t __a, int8x8_t __b)
{
  return __builtin_aarch64_ushlv8qi_uus (__a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshl_u16 (uint16x4_t __a, int16x4_t __b)
{
  return __builtin_aarch64_ushlv4hi_uus (__a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshl_u32 (uint32x2_t __a, int32x2_t __b)
{
  return __builtin_aarch64_ushlv2si_uus (__a, __b);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshl_u64 (uint64x1_t __a, int64x1_t __b)
{
  return (uint64x1_t) {__builtin_aarch64_ushldi_uus (__a[0], __b[0])};
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshlq_s8 (int8x16_t __a, int8x16_t __b)
{
  return __builtin_aarch64_sshlv16qi (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshlq_s16 (int16x8_t __a, int16x8_t __b)
{
  return __builtin_aarch64_sshlv8hi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshlq_s32 (int32x4_t __a, int32x4_t __b)
{
  return __builtin_aarch64_sshlv4si (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshlq_s64 (int64x2_t __a, int64x2_t __b)
{
  return __builtin_aarch64_sshlv2di (__a, __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshlq_u8 (uint8x16_t __a, int8x16_t __b)
{
  return __builtin_aarch64_ushlv16qi_uus (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshlq_u16 (uint16x8_t __a, int16x8_t __b)
{
  return __builtin_aarch64_ushlv8hi_uus (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshlq_u32 (uint32x4_t __a, int32x4_t __b)
{
  return __builtin_aarch64_ushlv4si_uus (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshlq_u64 (uint64x2_t __a, int64x2_t __b)
{
  return __builtin_aarch64_ushlv2di_uus (__a, __b);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshld_s64 (int64_t __a, int64_t __b)
{
  return __builtin_aarch64_sshldi (__a, __b);
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshld_u64 (uint64_t __a, int64_t __b)
{
  return __builtin_aarch64_ushldi_uus (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshll_high_n_s8 (int8x16_t __a, const int __b)
{
  return __builtin_aarch64_sshll2_nv16qi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshll_high_n_s16 (int16x8_t __a, const int __b)
{
  return __builtin_aarch64_sshll2_nv8hi (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshll_high_n_s32 (int32x4_t __a, const int __b)
{
  return __builtin_aarch64_sshll2_nv4si (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshll_high_n_u8 (uint8x16_t __a, const int __b)
{
  return (uint16x8_t) __builtin_aarch64_ushll2_nv16qi ((int8x16_t) __a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshll_high_n_u16 (uint16x8_t __a, const int __b)
{
  return (uint32x4_t) __builtin_aarch64_ushll2_nv8hi ((int16x8_t) __a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshll_high_n_u32 (uint32x4_t __a, const int __b)
{
  return (uint64x2_t) __builtin_aarch64_ushll2_nv4si ((int32x4_t) __a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshll_n_s8 (int8x8_t __a, const int __b)
{
  return __builtin_aarch64_sshll_nv8qi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshll_n_s16 (int16x4_t __a, const int __b)
{
  return __builtin_aarch64_sshll_nv4hi (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshll_n_s32 (int32x2_t __a, const int __b)
{
  return __builtin_aarch64_sshll_nv2si (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshll_n_u8 (uint8x8_t __a, const int __b)
{
  return __builtin_aarch64_ushll_nv8qi_uus (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshll_n_u16 (uint16x4_t __a, const int __b)
{
  return __builtin_aarch64_ushll_nv4hi_uus (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshll_n_u32 (uint32x2_t __a, const int __b)
{
  return __builtin_aarch64_ushll_nv2si_uus (__a, __b);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshr_n_s8 (int8x8_t __a, const int __b)
{
  return (int8x8_t) __builtin_aarch64_ashrv8qi (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshr_n_s16 (int16x4_t __a, const int __b)
{
  return (int16x4_t) __builtin_aarch64_ashrv4hi (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshr_n_s32 (int32x2_t __a, const int __b)
{
  return (int32x2_t) __builtin_aarch64_ashrv2si (__a, __b);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshr_n_s64 (int64x1_t __a, const int __b)
{
  return (int64x1_t) {__builtin_aarch64_ashr_simddi (__a[0], __b)};
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshr_n_u8 (uint8x8_t __a, const int __b)
{
  return __builtin_aarch64_lshrv8qi_uus (__a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshr_n_u16 (uint16x4_t __a, const int __b)
{
  return __builtin_aarch64_lshrv4hi_uus (__a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshr_n_u32 (uint32x2_t __a, const int __b)
{
  return __builtin_aarch64_lshrv2si_uus (__a, __b);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshr_n_u64 (uint64x1_t __a, const int __b)
{
  return (uint64x1_t) {__builtin_aarch64_lshr_simddi_uus ( __a[0], __b)};
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshrq_n_s8 (int8x16_t __a, const int __b)
{
  return (int8x16_t) __builtin_aarch64_ashrv16qi (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshrq_n_s16 (int16x8_t __a, const int __b)
{
  return (int16x8_t) __builtin_aarch64_ashrv8hi (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshrq_n_s32 (int32x4_t __a, const int __b)
{
  return (int32x4_t) __builtin_aarch64_ashrv4si (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshrq_n_s64 (int64x2_t __a, const int __b)
{
  return (int64x2_t) __builtin_aarch64_ashrv2di (__a, __b);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshrq_n_u8 (uint8x16_t __a, const int __b)
{
  return __builtin_aarch64_lshrv16qi_uus (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshrq_n_u16 (uint16x8_t __a, const int __b)
{
  return __builtin_aarch64_lshrv8hi_uus (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshrq_n_u32 (uint32x4_t __a, const int __b)
{
  return __builtin_aarch64_lshrv4si_uus (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshrq_n_u64 (uint64x2_t __a, const int __b)
{
  return __builtin_aarch64_lshrv2di_uus (__a, __b);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshrd_n_s64 (int64_t __a, const int __b)
{
  return __builtin_aarch64_ashr_simddi (__a, __b);
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vshrd_n_u64 (uint64_t __a, const int __b)
{
  return __builtin_aarch64_lshr_simddi_uus (__a, __b);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsli_n_s8 (int8x8_t __a, int8x8_t __b, const int __c)
{
  return (int8x8_t) __builtin_aarch64_ssli_nv8qi (__a, __b, __c);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsli_n_s16 (int16x4_t __a, int16x4_t __b, const int __c)
{
  return (int16x4_t) __builtin_aarch64_ssli_nv4hi (__a, __b, __c);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsli_n_s32 (int32x2_t __a, int32x2_t __b, const int __c)
{
  return (int32x2_t) __builtin_aarch64_ssli_nv2si (__a, __b, __c);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsli_n_s64 (int64x1_t __a, int64x1_t __b, const int __c)
{
  return (int64x1_t) {__builtin_aarch64_ssli_ndi (__a[0], __b[0], __c)};
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsli_n_u8 (uint8x8_t __a, uint8x8_t __b, const int __c)
{
  return __builtin_aarch64_usli_nv8qi_uuus (__a, __b, __c);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsli_n_u16 (uint16x4_t __a, uint16x4_t __b, const int __c)
{
  return __builtin_aarch64_usli_nv4hi_uuus (__a, __b, __c);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsli_n_u32 (uint32x2_t __a, uint32x2_t __b, const int __c)
{
  return __builtin_aarch64_usli_nv2si_uuus (__a, __b, __c);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsli_n_u64 (uint64x1_t __a, uint64x1_t __b, const int __c)
{
  return (uint64x1_t) {__builtin_aarch64_usli_ndi_uuus (__a[0], __b[0], __c)};
}
__extension__ extern __inline poly64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsli_n_p64 (poly64x1_t __a, poly64x1_t __b, const int __c)
{
  return (poly64x1_t) {__builtin_aarch64_ssli_ndi_ppps (__a[0], __b[0], __c)};
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsliq_n_s8 (int8x16_t __a, int8x16_t __b, const int __c)
{
  return (int8x16_t) __builtin_aarch64_ssli_nv16qi (__a, __b, __c);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsliq_n_s16 (int16x8_t __a, int16x8_t __b, const int __c)
{
  return (int16x8_t) __builtin_aarch64_ssli_nv8hi (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsliq_n_s32 (int32x4_t __a, int32x4_t __b, const int __c)
{
  return (int32x4_t) __builtin_aarch64_ssli_nv4si (__a, __b, __c);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsliq_n_s64 (int64x2_t __a, int64x2_t __b, const int __c)
{
  return (int64x2_t) __builtin_aarch64_ssli_nv2di (__a, __b, __c);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsliq_n_u8 (uint8x16_t __a, uint8x16_t __b, const int __c)
{
  return __builtin_aarch64_usli_nv16qi_uuus (__a, __b, __c);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsliq_n_u16 (uint16x8_t __a, uint16x8_t __b, const int __c)
{
  return __builtin_aarch64_usli_nv8hi_uuus (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsliq_n_u32 (uint32x4_t __a, uint32x4_t __b, const int __c)
{
  return __builtin_aarch64_usli_nv4si_uuus (__a, __b, __c);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsliq_n_u64 (uint64x2_t __a, uint64x2_t __b, const int __c)
{
  return __builtin_aarch64_usli_nv2di_uuus (__a, __b, __c);
}
__extension__ extern __inline poly64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsliq_n_p64 (poly64x2_t __a, poly64x2_t __b, const int __c)
{
  return __builtin_aarch64_ssli_nv2di_ppps (__a, __b, __c);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vslid_n_s64 (int64_t __a, int64_t __b, const int __c)
{
  return __builtin_aarch64_ssli_ndi (__a, __b, __c);
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vslid_n_u64 (uint64_t __a, uint64_t __b, const int __c)
{
  return __builtin_aarch64_usli_ndi_uuus (__a, __b, __c);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsqadd_u8 (uint8x8_t __a, int8x8_t __b)
{
  return __builtin_aarch64_usqaddv8qi_uus (__a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsqadd_u16 (uint16x4_t __a, int16x4_t __b)
{
  return __builtin_aarch64_usqaddv4hi_uus (__a, __b);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsqadd_u32 (uint32x2_t __a, int32x2_t __b)
{
  return __builtin_aarch64_usqaddv2si_uus (__a, __b);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsqadd_u64 (uint64x1_t __a, int64x1_t __b)
{
  return (uint64x1_t) {__builtin_aarch64_usqadddi_uus (__a[0], __b[0])};
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsqaddq_u8 (uint8x16_t __a, int8x16_t __b)
{
  return __builtin_aarch64_usqaddv16qi_uus (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsqaddq_u16 (uint16x8_t __a, int16x8_t __b)
{
  return __builtin_aarch64_usqaddv8hi_uus (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsqaddq_u32 (uint32x4_t __a, int32x4_t __b)
{
  return __builtin_aarch64_usqaddv4si_uus (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsqaddq_u64 (uint64x2_t __a, int64x2_t __b)
{
  return __builtin_aarch64_usqaddv2di_uus (__a, __b);
}
__extension__ extern __inline uint8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsqaddb_u8 (uint8_t __a, int8_t __b)
{
  return __builtin_aarch64_usqaddqi_uus (__a, __b);
}
__extension__ extern __inline uint16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsqaddh_u16 (uint16_t __a, int16_t __b)
{
  return __builtin_aarch64_usqaddhi_uus (__a, __b);
}
__extension__ extern __inline uint32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsqadds_u32 (uint32_t __a, int32_t __b)
{
  return __builtin_aarch64_usqaddsi_uus (__a, __b);
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsqaddd_u64 (uint64_t __a, int64_t __b)
{
  return __builtin_aarch64_usqadddi_uus (__a, __b);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsqrt_f32 (float32x2_t __a)
{
  return __builtin_aarch64_sqrtv2sf (__a);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsqrtq_f32 (float32x4_t __a)
{
  return __builtin_aarch64_sqrtv4sf (__a);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsqrt_f64 (float64x1_t __a)
{
  return (float64x1_t) { __builtin_aarch64_sqrtdf (__a[0]) };
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsqrtq_f64 (float64x2_t __a)
{
  return __builtin_aarch64_sqrtv2df (__a);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsra_n_s8 (int8x8_t __a, int8x8_t __b, const int __c)
{
  return (int8x8_t) __builtin_aarch64_ssra_nv8qi (__a, __b, __c);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsra_n_s16 (int16x4_t __a, int16x4_t __b, const int __c)
{
  return (int16x4_t) __builtin_aarch64_ssra_nv4hi (__a, __b, __c);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsra_n_s32 (int32x2_t __a, int32x2_t __b, const int __c)
{
  return (int32x2_t) __builtin_aarch64_ssra_nv2si (__a, __b, __c);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsra_n_s64 (int64x1_t __a, int64x1_t __b, const int __c)
{
  return (int64x1_t) {__builtin_aarch64_ssra_ndi (__a[0], __b[0], __c)};
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsra_n_u8 (uint8x8_t __a, uint8x8_t __b, const int __c)
{
  return __builtin_aarch64_usra_nv8qi_uuus (__a, __b, __c);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsra_n_u16 (uint16x4_t __a, uint16x4_t __b, const int __c)
{
  return __builtin_aarch64_usra_nv4hi_uuus (__a, __b, __c);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsra_n_u32 (uint32x2_t __a, uint32x2_t __b, const int __c)
{
  return __builtin_aarch64_usra_nv2si_uuus (__a, __b, __c);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsra_n_u64 (uint64x1_t __a, uint64x1_t __b, const int __c)
{
  return (uint64x1_t) {__builtin_aarch64_usra_ndi_uuus (__a[0], __b[0], __c)};
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsraq_n_s8 (int8x16_t __a, int8x16_t __b, const int __c)
{
  return (int8x16_t) __builtin_aarch64_ssra_nv16qi (__a, __b, __c);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsraq_n_s16 (int16x8_t __a, int16x8_t __b, const int __c)
{
  return (int16x8_t) __builtin_aarch64_ssra_nv8hi (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsraq_n_s32 (int32x4_t __a, int32x4_t __b, const int __c)
{
  return (int32x4_t) __builtin_aarch64_ssra_nv4si (__a, __b, __c);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsraq_n_s64 (int64x2_t __a, int64x2_t __b, const int __c)
{
  return (int64x2_t) __builtin_aarch64_ssra_nv2di (__a, __b, __c);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsraq_n_u8 (uint8x16_t __a, uint8x16_t __b, const int __c)
{
  return __builtin_aarch64_usra_nv16qi_uuus (__a, __b, __c);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsraq_n_u16 (uint16x8_t __a, uint16x8_t __b, const int __c)
{
  return __builtin_aarch64_usra_nv8hi_uuus (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsraq_n_u32 (uint32x4_t __a, uint32x4_t __b, const int __c)
{
  return __builtin_aarch64_usra_nv4si_uuus (__a, __b, __c);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsraq_n_u64 (uint64x2_t __a, uint64x2_t __b, const int __c)
{
  return __builtin_aarch64_usra_nv2di_uuus (__a, __b, __c);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsrad_n_s64 (int64_t __a, int64_t __b, const int __c)
{
  return __builtin_aarch64_ssra_ndi (__a, __b, __c);
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsrad_n_u64 (uint64_t __a, uint64_t __b, const int __c)
{
  return __builtin_aarch64_usra_ndi_uuus (__a, __b, __c);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsri_n_s8 (int8x8_t __a, int8x8_t __b, const int __c)
{
  return (int8x8_t) __builtin_aarch64_ssri_nv8qi (__a, __b, __c);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsri_n_s16 (int16x4_t __a, int16x4_t __b, const int __c)
{
  return (int16x4_t) __builtin_aarch64_ssri_nv4hi (__a, __b, __c);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsri_n_s32 (int32x2_t __a, int32x2_t __b, const int __c)
{
  return (int32x2_t) __builtin_aarch64_ssri_nv2si (__a, __b, __c);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsri_n_s64 (int64x1_t __a, int64x1_t __b, const int __c)
{
  return (int64x1_t) {__builtin_aarch64_ssri_ndi (__a[0], __b[0], __c)};
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsri_n_u8 (uint8x8_t __a, uint8x8_t __b, const int __c)
{
  return __builtin_aarch64_usri_nv8qi_uuus (__a, __b, __c);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsri_n_u16 (uint16x4_t __a, uint16x4_t __b, const int __c)
{
  return __builtin_aarch64_usri_nv4hi_uuus (__a, __b, __c);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsri_n_u32 (uint32x2_t __a, uint32x2_t __b, const int __c)
{
  return __builtin_aarch64_usri_nv2si_uuus (__a, __b, __c);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsri_n_u64 (uint64x1_t __a, uint64x1_t __b, const int __c)
{
  return (uint64x1_t) {__builtin_aarch64_usri_ndi_uuus (__a[0], __b[0], __c)};
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsriq_n_s8 (int8x16_t __a, int8x16_t __b, const int __c)
{
  return (int8x16_t) __builtin_aarch64_ssri_nv16qi (__a, __b, __c);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsriq_n_s16 (int16x8_t __a, int16x8_t __b, const int __c)
{
  return (int16x8_t) __builtin_aarch64_ssri_nv8hi (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsriq_n_s32 (int32x4_t __a, int32x4_t __b, const int __c)
{
  return (int32x4_t) __builtin_aarch64_ssri_nv4si (__a, __b, __c);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsriq_n_s64 (int64x2_t __a, int64x2_t __b, const int __c)
{
  return (int64x2_t) __builtin_aarch64_ssri_nv2di (__a, __b, __c);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsriq_n_u8 (uint8x16_t __a, uint8x16_t __b, const int __c)
{
  return __builtin_aarch64_usri_nv16qi_uuus (__a, __b, __c);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsriq_n_u16 (uint16x8_t __a, uint16x8_t __b, const int __c)
{
  return __builtin_aarch64_usri_nv8hi_uuus (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsriq_n_u32 (uint32x4_t __a, uint32x4_t __b, const int __c)
{
  return __builtin_aarch64_usri_nv4si_uuus (__a, __b, __c);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsriq_n_u64 (uint64x2_t __a, uint64x2_t __b, const int __c)
{
  return __builtin_aarch64_usri_nv2di_uuus (__a, __b, __c);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsrid_n_s64 (int64_t __a, int64_t __b, const int __c)
{
  return __builtin_aarch64_ssri_ndi (__a, __b, __c);
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsrid_n_u64 (uint64_t __a, uint64_t __b, const int __c)
{
  return __builtin_aarch64_usri_ndi_uuus (__a, __b, __c);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_f16 (float16_t *__a, float16x4_t __b)
{
  __builtin_aarch64_st1v4hf (__a, __b);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_f32 (float32_t *__a, float32x2_t __b)
{
  __builtin_aarch64_st1v2sf ((__builtin_aarch64_simd_sf *) __a, __b);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_f64 (float64_t *__a, float64x1_t __b)
{
  *__a = __b[0];
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_p8 (poly8_t *__a, poly8x8_t __b)
{
  __builtin_aarch64_st1v8qi_sp ((__builtin_aarch64_simd_qi *) __a, __b);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_p16 (poly16_t *__a, poly16x4_t __b)
{
  __builtin_aarch64_st1v4hi_sp ((__builtin_aarch64_simd_hi *) __a, __b);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_p64 (poly64_t *__a, poly64x1_t __b)
{
  *__a = __b[0];
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_s8 (int8_t *__a, int8x8_t __b)
{
  __builtin_aarch64_st1v8qi ((__builtin_aarch64_simd_qi *) __a, __b);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_s16 (int16_t *__a, int16x4_t __b)
{
  __builtin_aarch64_st1v4hi ((__builtin_aarch64_simd_hi *) __a, __b);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_s32 (int32_t *__a, int32x2_t __b)
{
  __builtin_aarch64_st1v2si ((__builtin_aarch64_simd_si *) __a, __b);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_s64 (int64_t *__a, int64x1_t __b)
{
  *__a = __b[0];
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_u8 (uint8_t *__a, uint8x8_t __b)
{
  __builtin_aarch64_st1v8qi_su ((__builtin_aarch64_simd_qi *) __a, __b);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_u16 (uint16_t *__a, uint16x4_t __b)
{
  __builtin_aarch64_st1v4hi_su ((__builtin_aarch64_simd_hi *) __a, __b);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_u32 (uint32_t *__a, uint32x2_t __b)
{
  __builtin_aarch64_st1v2si_su ((__builtin_aarch64_simd_si *) __a, __b);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_u64 (uint64_t *__a, uint64x1_t __b)
{
  *__a = __b[0];
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_f16 (float16_t *__a, float16x8_t __b)
{
  __builtin_aarch64_st1v8hf (__a, __b);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_f32 (float32_t *__a, float32x4_t __b)
{
  __builtin_aarch64_st1v4sf ((__builtin_aarch64_simd_sf *) __a, __b);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_f64 (float64_t *__a, float64x2_t __b)
{
  __builtin_aarch64_st1v2df ((__builtin_aarch64_simd_df *) __a, __b);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_p8 (poly8_t *__a, poly8x16_t __b)
{
  __builtin_aarch64_st1v16qi_sp ((__builtin_aarch64_simd_qi *) __a, __b);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_p16 (poly16_t *__a, poly16x8_t __b)
{
  __builtin_aarch64_st1v8hi_sp ((__builtin_aarch64_simd_hi *) __a, __b);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_p64 (poly64_t *__a, poly64x2_t __b)
{
  __builtin_aarch64_st1v2di_sp ((__builtin_aarch64_simd_di *) __a, __b);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_s8 (int8_t *__a, int8x16_t __b)
{
  __builtin_aarch64_st1v16qi ((__builtin_aarch64_simd_qi *) __a, __b);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_s16 (int16_t *__a, int16x8_t __b)
{
  __builtin_aarch64_st1v8hi ((__builtin_aarch64_simd_hi *) __a, __b);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_s32 (int32_t *__a, int32x4_t __b)
{
  __builtin_aarch64_st1v4si ((__builtin_aarch64_simd_si *) __a, __b);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_s64 (int64_t *__a, int64x2_t __b)
{
  __builtin_aarch64_st1v2di ((__builtin_aarch64_simd_di *) __a, __b);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_u8 (uint8_t *__a, uint8x16_t __b)
{
  __builtin_aarch64_st1v16qi_su ((__builtin_aarch64_simd_qi *) __a, __b);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_u16 (uint16_t *__a, uint16x8_t __b)
{
  __builtin_aarch64_st1v8hi_su ((__builtin_aarch64_simd_hi *) __a, __b);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_u32 (uint32_t *__a, uint32x4_t __b)
{
  __builtin_aarch64_st1v4si_su ((__builtin_aarch64_simd_si *) __a, __b);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_u64 (uint64_t *__a, uint64x2_t __b)
{
  __builtin_aarch64_st1v2di_su ((__builtin_aarch64_simd_di *) __a, __b);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_lane_f16 (float16_t *__a, float16x4_t __b, const int __lane)
{
  *__a = __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_lane_f32 (float32_t *__a, float32x2_t __b, const int __lane)
{
  *__a = __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_lane_f64 (float64_t *__a, float64x1_t __b, const int __lane)
{
  *__a = __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_lane_p8 (poly8_t *__a, poly8x8_t __b, const int __lane)
{
  *__a = __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_lane_p16 (poly16_t *__a, poly16x4_t __b, const int __lane)
{
  *__a = __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_lane_p64 (poly64_t *__a, poly64x1_t __b, const int __lane)
{
  *__a = __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_lane_s8 (int8_t *__a, int8x8_t __b, const int __lane)
{
  *__a = __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_lane_s16 (int16_t *__a, int16x4_t __b, const int __lane)
{
  *__a = __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_lane_s32 (int32_t *__a, int32x2_t __b, const int __lane)
{
  *__a = __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_lane_s64 (int64_t *__a, int64x1_t __b, const int __lane)
{
  *__a = __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_lane_u8 (uint8_t *__a, uint8x8_t __b, const int __lane)
{
  *__a = __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_lane_u16 (uint16_t *__a, uint16x4_t __b, const int __lane)
{
  *__a = __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_lane_u32 (uint32_t *__a, uint32x2_t __b, const int __lane)
{
  *__a = __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_lane_u64 (uint64_t *__a, uint64x1_t __b, const int __lane)
{
  *__a = __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_lane_f16 (float16_t *__a, float16x8_t __b, const int __lane)
{
  *__a = __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_lane_f32 (float32_t *__a, float32x4_t __b, const int __lane)
{
  *__a = __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_lane_f64 (float64_t *__a, float64x2_t __b, const int __lane)
{
  *__a = __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_lane_p8 (poly8_t *__a, poly8x16_t __b, const int __lane)
{
  *__a = __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_lane_p16 (poly16_t *__a, poly16x8_t __b, const int __lane)
{
  *__a = __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_lane_p64 (poly64_t *__a, poly64x2_t __b, const int __lane)
{
  *__a = __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_lane_s8 (int8_t *__a, int8x16_t __b, const int __lane)
{
  *__a = __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_lane_s16 (int16_t *__a, int16x8_t __b, const int __lane)
{
  *__a = __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_lane_s32 (int32_t *__a, int32x4_t __b, const int __lane)
{
  *__a = __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_lane_s64 (int64_t *__a, int64x2_t __b, const int __lane)
{
  *__a = __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_lane_u8 (uint8_t *__a, uint8x16_t __b, const int __lane)
{
  *__a = __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_lane_u16 (uint16_t *__a, uint16x8_t __b, const int __lane)
{
  *__a = __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_lane_u32 (uint32_t *__a, uint32x4_t __b, const int __lane)
{
  *__a = __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_lane_u64 (uint64_t *__a, uint64x2_t __b, const int __lane)
{
  *__a = __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_s64_x2 (int64_t * __a, int64x1x2_t __val)
{
  __builtin_aarch64_st1x2di ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_u64_x2 (uint64_t * __a, uint64x1x2_t __val)
{
  __builtin_aarch64_st1x2di_su ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_f64_x2 (float64_t * __a, float64x1x2_t __val)
{
  __builtin_aarch64_st1x2df ((__builtin_aarch64_simd_df *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_s8_x2 (int8_t * __a, int8x8x2_t __val)
{
  __builtin_aarch64_st1x2v8qi ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_p8_x2 (poly8_t * __a, poly8x8x2_t __val)
{
  __builtin_aarch64_st1x2v8qi_sp ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_s16_x2 (int16_t * __a, int16x4x2_t __val)
{
  __builtin_aarch64_st1x2v4hi ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_p16_x2 (poly16_t * __a, poly16x4x2_t __val)
{
  __builtin_aarch64_st1x2v4hi_sp ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_s32_x2 (int32_t * __a, int32x2x2_t __val)
{
  __builtin_aarch64_st1x2v2si ((__builtin_aarch64_simd_si *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_u8_x2 (uint8_t * __a, uint8x8x2_t __val)
{
  __builtin_aarch64_st1x2v8qi_su ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_u16_x2 (uint16_t * __a, uint16x4x2_t __val)
{
  __builtin_aarch64_st1x2v4hi_su ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_u32_x2 (uint32_t * __a, uint32x2x2_t __val)
{
  __builtin_aarch64_st1x2v2si_su ((__builtin_aarch64_simd_si *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_f16_x2 (float16_t * __a, float16x4x2_t __val)
{
  __builtin_aarch64_st1x2v4hf (__a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_f32_x2 (float32_t * __a, float32x2x2_t __val)
{
  __builtin_aarch64_st1x2v2sf ((__builtin_aarch64_simd_sf *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_p64_x2 (poly64_t * __a, poly64x1x2_t __val)
{
  __builtin_aarch64_st1x2di_sp ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_s8_x2 (int8_t * __a, int8x16x2_t __val)
{
  __builtin_aarch64_st1x2v16qi ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_p8_x2 (poly8_t * __a, poly8x16x2_t __val)
{
  __builtin_aarch64_st1x2v16qi_sp ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_s16_x2 (int16_t * __a, int16x8x2_t __val)
{
  __builtin_aarch64_st1x2v8hi ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_p16_x2 (poly16_t * __a, poly16x8x2_t __val)
{
  __builtin_aarch64_st1x2v8hi_sp ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_s32_x2 (int32_t * __a, int32x4x2_t __val)
{
  __builtin_aarch64_st1x2v4si ((__builtin_aarch64_simd_si *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_s64_x2 (int64_t * __a, int64x2x2_t __val)
{
  __builtin_aarch64_st1x2v2di ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_u8_x2 (uint8_t * __a, uint8x16x2_t __val)
{
  __builtin_aarch64_st1x2v16qi_su ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_u16_x2 (uint16_t * __a, uint16x8x2_t __val)
{
  __builtin_aarch64_st1x2v8hi_su ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_u32_x2 (uint32_t * __a, uint32x4x2_t __val)
{
  __builtin_aarch64_st1x2v4si_su ((__builtin_aarch64_simd_si *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_u64_x2 (uint64_t * __a, uint64x2x2_t __val)
{
  __builtin_aarch64_st1x2v2di_su ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_f16_x2 (float16_t * __a, float16x8x2_t __val)
{
  __builtin_aarch64_st1x2v8hf (__a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_f32_x2 (float32_t * __a, float32x4x2_t __val)
{
  __builtin_aarch64_st1x2v4sf ((__builtin_aarch64_simd_sf *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_f64_x2 (float64_t * __a, float64x2x2_t __val)
{
  __builtin_aarch64_st1x2v2df ((__builtin_aarch64_simd_df *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_p64_x2 (poly64_t * __a, poly64x2x2_t __val)
{
  __builtin_aarch64_st1x2v2di_sp ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_s64_x3 (int64_t * __a, int64x1x3_t __val)
{
  __builtin_aarch64_st1x3di ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_u64_x3 (uint64_t * __a, uint64x1x3_t __val)
{
  __builtin_aarch64_st1x3di_su ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_f64_x3 (float64_t * __a, float64x1x3_t __val)
{
  __builtin_aarch64_st1x3df ((__builtin_aarch64_simd_df *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_s8_x3 (int8_t * __a, int8x8x3_t __val)
{
  __builtin_aarch64_st1x3v8qi ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_p8_x3 (poly8_t * __a, poly8x8x3_t __val)
{
  __builtin_aarch64_st1x3v8qi_sp ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_s16_x3 (int16_t * __a, int16x4x3_t __val)
{
  __builtin_aarch64_st1x3v4hi ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_p16_x3 (poly16_t * __a, poly16x4x3_t __val)
{
  __builtin_aarch64_st1x3v4hi_sp ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_s32_x3 (int32_t * __a, int32x2x3_t __val)
{
  __builtin_aarch64_st1x3v2si ((__builtin_aarch64_simd_si *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_u8_x3 (uint8_t * __a, uint8x8x3_t __val)
{
  __builtin_aarch64_st1x3v8qi_su ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_u16_x3 (uint16_t * __a, uint16x4x3_t __val)
{
  __builtin_aarch64_st1x3v4hi_su ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_u32_x3 (uint32_t * __a, uint32x2x3_t __val)
{
  __builtin_aarch64_st1x3v2si_su ((__builtin_aarch64_simd_si *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_f16_x3 (float16_t * __a, float16x4x3_t __val)
{
  __builtin_aarch64_st1x3v4hf ((__builtin_aarch64_simd_hf *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_f32_x3 (float32_t * __a, float32x2x3_t __val)
{
  __builtin_aarch64_st1x3v2sf ((__builtin_aarch64_simd_sf *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_p64_x3 (poly64_t * __a, poly64x1x3_t __val)
{
  __builtin_aarch64_st1x3di_sp ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_s8_x3 (int8_t * __a, int8x16x3_t __val)
{
  __builtin_aarch64_st1x3v16qi ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_p8_x3 (poly8_t * __a, poly8x16x3_t __val)
{
  __builtin_aarch64_st1x3v16qi_sp ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_s16_x3 (int16_t * __a, int16x8x3_t __val)
{
  __builtin_aarch64_st1x3v8hi ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_p16_x3 (poly16_t * __a, poly16x8x3_t __val)
{
  __builtin_aarch64_st1x3v8hi_sp ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_s32_x3 (int32_t * __a, int32x4x3_t __val)
{
  __builtin_aarch64_st1x3v4si ((__builtin_aarch64_simd_si *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_s64_x3 (int64_t * __a, int64x2x3_t __val)
{
  __builtin_aarch64_st1x3v2di ((__builtin_aarch64_simd_di *) __a,
          (int64x2x3_t) __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_u8_x3 (uint8_t * __a, uint8x16x3_t __val)
{
  __builtin_aarch64_st1x3v16qi_su ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_u16_x3 (uint16_t * __a, uint16x8x3_t __val)
{
  __builtin_aarch64_st1x3v8hi_su ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_u32_x3 (uint32_t * __a, uint32x4x3_t __val)
{
  __builtin_aarch64_st1x3v4si_su ((__builtin_aarch64_simd_si *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_u64_x3 (uint64_t * __a, uint64x2x3_t __val)
{
  __builtin_aarch64_st1x3v2di_su ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_f16_x3 (float16_t * __a, float16x8x3_t __val)
{
  __builtin_aarch64_st1x3v8hf ((__builtin_aarch64_simd_hf *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_f32_x3 (float32_t * __a, float32x4x3_t __val)
{
  __builtin_aarch64_st1x3v4sf ((__builtin_aarch64_simd_sf *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_f64_x3 (float64_t * __a, float64x2x3_t __val)
{
  __builtin_aarch64_st1x3v2df ((__builtin_aarch64_simd_df *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_p64_x3 (poly64_t * __a, poly64x2x3_t __val)
{
  __builtin_aarch64_st1x3v2di_sp ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_s8_x4 (int8_t * __a, int8x8x4_t __val)
{
  __builtin_aarch64_st1x4v8qi ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_s8_x4 (int8_t * __a, int8x16x4_t __val)
{
  __builtin_aarch64_st1x4v16qi ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_s16_x4 (int16_t * __a, int16x4x4_t __val)
{
  __builtin_aarch64_st1x4v4hi ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_s16_x4 (int16_t * __a, int16x8x4_t __val)
{
  __builtin_aarch64_st1x4v8hi ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_s32_x4 (int32_t * __a, int32x2x4_t __val)
{
  __builtin_aarch64_st1x4v2si ((__builtin_aarch64_simd_si *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_s32_x4 (int32_t * __a, int32x4x4_t __val)
{
  __builtin_aarch64_st1x4v4si ((__builtin_aarch64_simd_si *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_u8_x4 (uint8_t * __a, uint8x8x4_t __val)
{
  __builtin_aarch64_st1x4v8qi_su ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_u8_x4 (uint8_t * __a, uint8x16x4_t __val)
{
  __builtin_aarch64_st1x4v16qi_su ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_u16_x4 (uint16_t * __a, uint16x4x4_t __val)
{
  __builtin_aarch64_st1x4v4hi_su ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_u16_x4 (uint16_t * __a, uint16x8x4_t __val)
{
  __builtin_aarch64_st1x4v8hi_su ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_u32_x4 (uint32_t * __a, uint32x2x4_t __val)
{
  __builtin_aarch64_st1x4v2si_su ((__builtin_aarch64_simd_si *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_u32_x4 (uint32_t * __a, uint32x4x4_t __val)
{
  __builtin_aarch64_st1x4v4si_su ((__builtin_aarch64_simd_si *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_f16_x4 (float16_t * __a, float16x4x4_t __val)
{
  __builtin_aarch64_st1x4v4hf ((__builtin_aarch64_simd_hf *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_f16_x4 (float16_t * __a, float16x8x4_t __val)
{
  __builtin_aarch64_st1x4v8hf ((__builtin_aarch64_simd_hf *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_f32_x4 (float32_t * __a, float32x2x4_t __val)
{
  __builtin_aarch64_st1x4v2sf ((__builtin_aarch64_simd_sf *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_f32_x4 (float32_t * __a, float32x4x4_t __val)
{
  __builtin_aarch64_st1x4v4sf ((__builtin_aarch64_simd_sf *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_p8_x4 (poly8_t * __a, poly8x8x4_t __val)
{
  __builtin_aarch64_st1x4v8qi_sp ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_p8_x4 (poly8_t * __a, poly8x16x4_t __val)
{
  __builtin_aarch64_st1x4v16qi_sp ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_p16_x4 (poly16_t * __a, poly16x4x4_t __val)
{
  __builtin_aarch64_st1x4v4hi_sp ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_p16_x4 (poly16_t * __a, poly16x8x4_t __val)
{
  __builtin_aarch64_st1x4v8hi_sp ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_s64_x4 (int64_t * __a, int64x1x4_t __val)
{
  __builtin_aarch64_st1x4di ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_u64_x4 (uint64_t * __a, uint64x1x4_t __val)
{
  __builtin_aarch64_st1x4di_su ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_p64_x4 (poly64_t * __a, poly64x1x4_t __val)
{
  __builtin_aarch64_st1x4di_sp ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_s64_x4 (int64_t * __a, int64x2x4_t __val)
{
  __builtin_aarch64_st1x4v2di ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_u64_x4 (uint64_t * __a, uint64x2x4_t __val)
{
  __builtin_aarch64_st1x4v2di_su ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_p64_x4 (poly64_t * __a, poly64x2x4_t __val)
{
  __builtin_aarch64_st1x4v2di_sp ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_f64_x4 (float64_t * __a, float64x1x4_t __val)
{
  __builtin_aarch64_st1x4df ((__builtin_aarch64_simd_df *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_f64_x4 (float64_t * __a, float64x2x4_t __val)
{
  __builtin_aarch64_st1x4v2df ((__builtin_aarch64_simd_df *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2_s64 (int64_t * __a, int64x1x2_t __val)
{
  __builtin_aarch64_st2di ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2_u64 (uint64_t * __a, uint64x1x2_t __val)
{
  __builtin_aarch64_st2di_su ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2_f64 (float64_t * __a, float64x1x2_t __val)
{
  __builtin_aarch64_st2df ((__builtin_aarch64_simd_df *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2_s8 (int8_t * __a, int8x8x2_t __val)
{
  __builtin_aarch64_st2v8qi ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2_p8 (poly8_t * __a, poly8x8x2_t __val)
{
  __builtin_aarch64_st2v8qi_sp ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2_s16 (int16_t * __a, int16x4x2_t __val)
{
  __builtin_aarch64_st2v4hi ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2_p16 (poly16_t * __a, poly16x4x2_t __val)
{
  __builtin_aarch64_st2v4hi_sp ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2_s32 (int32_t * __a, int32x2x2_t __val)
{
  __builtin_aarch64_st2v2si ((__builtin_aarch64_simd_si *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2_u8 (uint8_t * __a, uint8x8x2_t __val)
{
  __builtin_aarch64_st2v8qi_su ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2_u16 (uint16_t * __a, uint16x4x2_t __val)
{
  __builtin_aarch64_st2v4hi_su ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2_u32 (uint32_t * __a, uint32x2x2_t __val)
{
  __builtin_aarch64_st2v2si_su ((__builtin_aarch64_simd_si *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2_f16 (float16_t * __a, float16x4x2_t __val)
{
  __builtin_aarch64_st2v4hf ((__builtin_aarch64_simd_hf *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2_f32 (float32_t * __a, float32x2x2_t __val)
{
  __builtin_aarch64_st2v2sf ((__builtin_aarch64_simd_sf *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2_p64 (poly64_t * __a, poly64x1x2_t __val)
{
  __builtin_aarch64_st2di_sp ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2q_s8 (int8_t * __a, int8x16x2_t __val)
{
  __builtin_aarch64_st2v16qi ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2q_p8 (poly8_t * __a, poly8x16x2_t __val)
{
  __builtin_aarch64_st2v16qi_sp ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2q_s16 (int16_t * __a, int16x8x2_t __val)
{
  __builtin_aarch64_st2v8hi ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2q_p16 (poly16_t * __a, poly16x8x2_t __val)
{
  __builtin_aarch64_st2v8hi_sp ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2q_s32 (int32_t * __a, int32x4x2_t __val)
{
  __builtin_aarch64_st2v4si ((__builtin_aarch64_simd_si *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2q_s64 (int64_t * __a, int64x2x2_t __val)
{
  __builtin_aarch64_st2v2di ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2q_u8 (uint8_t * __a, uint8x16x2_t __val)
{
  __builtin_aarch64_st2v16qi_su ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2q_u16 (uint16_t * __a, uint16x8x2_t __val)
{
  __builtin_aarch64_st2v8hi_su ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2q_u32 (uint32_t * __a, uint32x4x2_t __val)
{
  __builtin_aarch64_st2v4si_su ((__builtin_aarch64_simd_si *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2q_u64 (uint64_t * __a, uint64x2x2_t __val)
{
  __builtin_aarch64_st2v2di_su ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2q_f16 (float16_t * __a, float16x8x2_t __val)
{
  __builtin_aarch64_st2v8hf ((__builtin_aarch64_simd_hf *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2q_f32 (float32_t * __a, float32x4x2_t __val)
{
  __builtin_aarch64_st2v4sf ((__builtin_aarch64_simd_sf *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2q_f64 (float64_t * __a, float64x2x2_t __val)
{
  __builtin_aarch64_st2v2df ((__builtin_aarch64_simd_df *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2q_p64 (poly64_t * __a, poly64x2x2_t __val)
{
  __builtin_aarch64_st2v2di_sp ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3_s64 (int64_t * __a, int64x1x3_t __val)
{
  __builtin_aarch64_st3di ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3_u64 (uint64_t * __a, uint64x1x3_t __val)
{
  __builtin_aarch64_st3di_su ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3_f64 (float64_t * __a, float64x1x3_t __val)
{
  __builtin_aarch64_st3df ((__builtin_aarch64_simd_df *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3_s8 (int8_t * __a, int8x8x3_t __val)
{
  __builtin_aarch64_st3v8qi ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3_p8 (poly8_t * __a, poly8x8x3_t __val)
{
  __builtin_aarch64_st3v8qi_sp ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3_s16 (int16_t * __a, int16x4x3_t __val)
{
  __builtin_aarch64_st3v4hi ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3_p16 (poly16_t * __a, poly16x4x3_t __val)
{
  __builtin_aarch64_st3v4hi_sp ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3_s32 (int32_t * __a, int32x2x3_t __val)
{
  __builtin_aarch64_st3v2si ((__builtin_aarch64_simd_si *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3_u8 (uint8_t * __a, uint8x8x3_t __val)
{
  __builtin_aarch64_st3v8qi_su ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3_u16 (uint16_t * __a, uint16x4x3_t __val)
{
  __builtin_aarch64_st3v4hi_su ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3_u32 (uint32_t * __a, uint32x2x3_t __val)
{
  __builtin_aarch64_st3v2si_su ((__builtin_aarch64_simd_si *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3_f16 (float16_t * __a, float16x4x3_t __val)
{
  __builtin_aarch64_st3v4hf ((__builtin_aarch64_simd_hf *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3_f32 (float32_t * __a, float32x2x3_t __val)
{
  __builtin_aarch64_st3v2sf ((__builtin_aarch64_simd_sf *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3_p64 (poly64_t * __a, poly64x1x3_t __val)
{
  __builtin_aarch64_st3di_sp ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3q_s8 (int8_t * __a, int8x16x3_t __val)
{
  __builtin_aarch64_st3v16qi ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3q_p8 (poly8_t * __a, poly8x16x3_t __val)
{
  __builtin_aarch64_st3v16qi_sp ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3q_s16 (int16_t * __a, int16x8x3_t __val)
{
  __builtin_aarch64_st3v8hi ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3q_p16 (poly16_t * __a, poly16x8x3_t __val)
{
  __builtin_aarch64_st3v8hi_sp ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3q_s32 (int32_t * __a, int32x4x3_t __val)
{
  __builtin_aarch64_st3v4si ((__builtin_aarch64_simd_si *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3q_s64 (int64_t * __a, int64x2x3_t __val)
{
  __builtin_aarch64_st3v2di ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3q_u8 (uint8_t * __a, uint8x16x3_t __val)
{
  __builtin_aarch64_st3v16qi_su ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3q_u16 (uint16_t * __a, uint16x8x3_t __val)
{
  __builtin_aarch64_st3v8hi_su ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3q_u32 (uint32_t * __a, uint32x4x3_t __val)
{
  __builtin_aarch64_st3v4si_su ((__builtin_aarch64_simd_si *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3q_u64 (uint64_t * __a, uint64x2x3_t __val)
{
  __builtin_aarch64_st3v2di_su ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3q_f16 (float16_t * __a, float16x8x3_t __val)
{
  __builtin_aarch64_st3v8hf ((__builtin_aarch64_simd_hf *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3q_f32 (float32_t * __a, float32x4x3_t __val)
{
  __builtin_aarch64_st3v4sf ((__builtin_aarch64_simd_sf *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3q_f64 (float64_t * __a, float64x2x3_t __val)
{
  __builtin_aarch64_st3v2df ((__builtin_aarch64_simd_df *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3q_p64 (poly64_t * __a, poly64x2x3_t __val)
{
  __builtin_aarch64_st3v2di_sp ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4_s64 (int64_t * __a, int64x1x4_t __val)
{
  __builtin_aarch64_st4di ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4_u64 (uint64_t * __a, uint64x1x4_t __val)
{
  __builtin_aarch64_st4di_su ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4_f64 (float64_t * __a, float64x1x4_t __val)
{
  __builtin_aarch64_st4df ((__builtin_aarch64_simd_df *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4_s8 (int8_t * __a, int8x8x4_t __val)
{
  __builtin_aarch64_st4v8qi ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4_p8 (poly8_t * __a, poly8x8x4_t __val)
{
  __builtin_aarch64_st4v8qi_sp ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4_s16 (int16_t * __a, int16x4x4_t __val)
{
  __builtin_aarch64_st4v4hi ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4_p16 (poly16_t * __a, poly16x4x4_t __val)
{
  __builtin_aarch64_st4v4hi_sp ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4_s32 (int32_t * __a, int32x2x4_t __val)
{
  __builtin_aarch64_st4v2si ((__builtin_aarch64_simd_si *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4_u8 (uint8_t * __a, uint8x8x4_t __val)
{
  __builtin_aarch64_st4v8qi_su ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4_u16 (uint16_t * __a, uint16x4x4_t __val)
{
  __builtin_aarch64_st4v4hi_su ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4_u32 (uint32_t * __a, uint32x2x4_t __val)
{
  __builtin_aarch64_st4v2si_su ((__builtin_aarch64_simd_si *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4_f16 (float16_t * __a, float16x4x4_t __val)
{
  __builtin_aarch64_st4v4hf ((__builtin_aarch64_simd_hf *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4_f32 (float32_t * __a, float32x2x4_t __val)
{
  __builtin_aarch64_st4v2sf ((__builtin_aarch64_simd_sf *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4_p64 (poly64_t * __a, poly64x1x4_t __val)
{
  __builtin_aarch64_st4di_sp ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4q_s8 (int8_t * __a, int8x16x4_t __val)
{
  __builtin_aarch64_st4v16qi ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4q_p8 (poly8_t * __a, poly8x16x4_t __val)
{
  __builtin_aarch64_st4v16qi_sp ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4q_s16 (int16_t * __a, int16x8x4_t __val)
{
  __builtin_aarch64_st4v8hi ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4q_p16 (poly16_t * __a, poly16x8x4_t __val)
{
  __builtin_aarch64_st4v8hi_sp ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4q_s32 (int32_t * __a, int32x4x4_t __val)
{
  __builtin_aarch64_st4v4si ((__builtin_aarch64_simd_si *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4q_s64 (int64_t * __a, int64x2x4_t __val)
{
  __builtin_aarch64_st4v2di ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4q_u8 (uint8_t * __a, uint8x16x4_t __val)
{
  __builtin_aarch64_st4v16qi_su ((__builtin_aarch64_simd_qi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4q_u16 (uint16_t * __a, uint16x8x4_t __val)
{
  __builtin_aarch64_st4v8hi_su ((__builtin_aarch64_simd_hi *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4q_u32 (uint32_t * __a, uint32x4x4_t __val)
{
  __builtin_aarch64_st4v4si_su ((__builtin_aarch64_simd_si *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4q_u64 (uint64_t * __a, uint64x2x4_t __val)
{
  __builtin_aarch64_st4v2di_su ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4q_f16 (float16_t * __a, float16x8x4_t __val)
{
  __builtin_aarch64_st4v8hf ((__builtin_aarch64_simd_hf *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4q_f32 (float32_t * __a, float32x4x4_t __val)
{
  __builtin_aarch64_st4v4sf ((__builtin_aarch64_simd_sf *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4q_f64 (float64_t * __a, float64x2x4_t __val)
{
  __builtin_aarch64_st4v2df ((__builtin_aarch64_simd_df *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4q_p64 (poly64_t * __a, poly64x2x4_t __val)
{
  __builtin_aarch64_st4v2di_sp ((__builtin_aarch64_simd_di *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vstrq_p128 (poly128_t * __ptr, poly128_t __val)
{
  *__ptr = __val;
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubd_s64 (int64_t __a, int64_t __b)
{
  return __a - __b;
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubd_u64 (uint64_t __a, uint64_t __b)
{
  return __a - __b;
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtbx1_s8 (int8x8_t __r, int8x8_t __tab, int8x8_t __idx)
{
  uint8x8_t __mask = vclt_u8 (vreinterpret_u8_s8 (__idx),
         vmov_n_u8 (8));
  int8x8_t __tbl = vtbl1_s8 (__tab, __idx);
  return vbsl_s8 (__mask, __tbl, __r);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtbx1_u8 (uint8x8_t __r, uint8x8_t __tab, uint8x8_t __idx)
{
  uint8x8_t __mask = vclt_u8 (__idx, vmov_n_u8 (8));
  uint8x8_t __tbl = vtbl1_u8 (__tab, __idx);
  return vbsl_u8 (__mask, __tbl, __r);
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtbx1_p8 (poly8x8_t __r, poly8x8_t __tab, uint8x8_t __idx)
{
  uint8x8_t __mask = vclt_u8 (__idx, vmov_n_u8 (8));
  poly8x8_t __tbl = vtbl1_p8 (__tab, __idx);
  return vbsl_p8 (__mask, __tbl, __r);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtbx3_s8 (int8x8_t __r, int8x8x3_t __tab, int8x8_t __idx)
{
  uint8x8_t __mask = vclt_u8 (vreinterpret_u8_s8 (__idx),
         vmov_n_u8 (24));
  int8x8_t __tbl = vtbl3_s8 (__tab, __idx);
  return vbsl_s8 (__mask, __tbl, __r);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtbx3_u8 (uint8x8_t __r, uint8x8x3_t __tab, uint8x8_t __idx)
{
  uint8x8_t __mask = vclt_u8 (__idx, vmov_n_u8 (24));
  uint8x8_t __tbl = vtbl3_u8 (__tab, __idx);
  return vbsl_u8 (__mask, __tbl, __r);
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtbx3_p8 (poly8x8_t __r, poly8x8x3_t __tab, uint8x8_t __idx)
{
  uint8x8_t __mask = vclt_u8 (__idx, vmov_n_u8 (24));
  poly8x8_t __tbl = vtbl3_p8 (__tab, __idx);
  return vbsl_p8 (__mask, __tbl, __r);
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtbx4_s8 (int8x8_t __r, int8x8x4_t __tab, int8x8_t __idx)
{
  int8x16x2_t __temp;
  __temp.val[0] = vcombine_s8 (__tab.val[0], __tab.val[1]);
  __temp.val[1] = vcombine_s8 (__tab.val[2], __tab.val[3]);
  return __builtin_aarch64_qtbx2v8qi (__r, __temp, __idx);
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtbx4_u8 (uint8x8_t __r, uint8x8x4_t __tab, uint8x8_t __idx)
{
  uint8x16x2_t __temp;
  __temp.val[0] = vcombine_u8 (__tab.val[0], __tab.val[1]);
  __temp.val[1] = vcombine_u8 (__tab.val[2], __tab.val[3]);
  return __builtin_aarch64_qtbx2v8qi_uuuu (__r, __temp, __idx);
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtbx4_p8 (poly8x8_t __r, poly8x8x4_t __tab, uint8x8_t __idx)
{
  poly8x16x2_t __temp;
  __temp.val[0] = vcombine_p8 (__tab.val[0], __tab.val[1]);
  __temp.val[1] = vcombine_p8 (__tab.val[2], __tab.val[3]);
  return __builtin_aarch64_qtbx2v8qi_pppu (__r, __temp, __idx);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn1_f16 (float16x4_t __a, float16x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x4_t) {5, 1, 7, 3});
#else
  return __builtin_shuffle (__a, __b, (uint16x4_t) {0, 4, 2, 6});
#endif
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn1_f32 (float32x2_t __a, float32x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x2_t) {3, 1});
#else
  return __builtin_shuffle (__a, __b, (uint32x2_t) {0, 2});
#endif
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn1_p8 (poly8x8_t __a, poly8x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint8x8_t) {9, 1, 11, 3, 13, 5, 15, 7});
#else
  return __builtin_shuffle (__a, __b, (uint8x8_t) {0, 8, 2, 10, 4, 12, 6, 14});
#endif
}
__extension__ extern __inline poly16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn1_p16 (poly16x4_t __a, poly16x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x4_t) {5, 1, 7, 3});
#else
  return __builtin_shuffle (__a, __b, (uint16x4_t) {0, 4, 2, 6});
#endif
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn1_s8 (int8x8_t __a, int8x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint8x8_t) {9, 1, 11, 3, 13, 5, 15, 7});
#else
  return __builtin_shuffle (__a, __b, (uint8x8_t) {0, 8, 2, 10, 4, 12, 6, 14});
#endif
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn1_s16 (int16x4_t __a, int16x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x4_t) {5, 1, 7, 3});
#else
  return __builtin_shuffle (__a, __b, (uint16x4_t) {0, 4, 2, 6});
#endif
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn1_s32 (int32x2_t __a, int32x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x2_t) {3, 1});
#else
  return __builtin_shuffle (__a, __b, (uint32x2_t) {0, 2});
#endif
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn1_u8 (uint8x8_t __a, uint8x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint8x8_t) {9, 1, 11, 3, 13, 5, 15, 7});
#else
  return __builtin_shuffle (__a, __b, (uint8x8_t) {0, 8, 2, 10, 4, 12, 6, 14});
#endif
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn1_u16 (uint16x4_t __a, uint16x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x4_t) {5, 1, 7, 3});
#else
  return __builtin_shuffle (__a, __b, (uint16x4_t) {0, 4, 2, 6});
#endif
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn1_u32 (uint32x2_t __a, uint32x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x2_t) {3, 1});
#else
  return __builtin_shuffle (__a, __b, (uint32x2_t) {0, 2});
#endif
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn1q_f16 (float16x8_t __a, float16x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x8_t) {9, 1, 11, 3, 13, 5, 15, 7});
#else
  return __builtin_shuffle (__a, __b, (uint16x8_t) {0, 8, 2, 10, 4, 12, 6, 14});
#endif
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn1q_f32 (float32x4_t __a, float32x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x4_t) {5, 1, 7, 3});
#else
  return __builtin_shuffle (__a, __b, (uint32x4_t) {0, 4, 2, 6});
#endif
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn1q_f64 (float64x2_t __a, float64x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint64x2_t) {3, 1});
#else
  return __builtin_shuffle (__a, __b, (uint64x2_t) {0, 2});
#endif
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn1q_p8 (poly8x16_t __a, poly8x16_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b,
      (uint8x16_t) {17, 1, 19, 3, 21, 5, 23, 7, 25, 9, 27, 11, 29, 13, 31, 15});
#else
  return __builtin_shuffle (__a, __b,
      (uint8x16_t) {0, 16, 2, 18, 4, 20, 6, 22, 8, 24, 10, 26, 12, 28, 14, 30});
#endif
}
__extension__ extern __inline poly16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn1q_p16 (poly16x8_t __a, poly16x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x8_t) {9, 1, 11, 3, 13, 5, 15, 7});
#else
  return __builtin_shuffle (__a, __b, (uint16x8_t) {0, 8, 2, 10, 4, 12, 6, 14});
#endif
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn1q_s8 (int8x16_t __a, int8x16_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b,
      (uint8x16_t) {17, 1, 19, 3, 21, 5, 23, 7, 25, 9, 27, 11, 29, 13, 31, 15});
#else
  return __builtin_shuffle (__a, __b,
      (uint8x16_t) {0, 16, 2, 18, 4, 20, 6, 22, 8, 24, 10, 26, 12, 28, 14, 30});
#endif
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn1q_s16 (int16x8_t __a, int16x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x8_t) {9, 1, 11, 3, 13, 5, 15, 7});
#else
  return __builtin_shuffle (__a, __b, (uint16x8_t) {0, 8, 2, 10, 4, 12, 6, 14});
#endif
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn1q_s32 (int32x4_t __a, int32x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x4_t) {5, 1, 7, 3});
#else
  return __builtin_shuffle (__a, __b, (uint32x4_t) {0, 4, 2, 6});
#endif
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn1q_s64 (int64x2_t __a, int64x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint64x2_t) {3, 1});
#else
  return __builtin_shuffle (__a, __b, (uint64x2_t) {0, 2});
#endif
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn1q_u8 (uint8x16_t __a, uint8x16_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b,
      (uint8x16_t) {17, 1, 19, 3, 21, 5, 23, 7, 25, 9, 27, 11, 29, 13, 31, 15});
#else
  return __builtin_shuffle (__a, __b,
      (uint8x16_t) {0, 16, 2, 18, 4, 20, 6, 22, 8, 24, 10, 26, 12, 28, 14, 30});
#endif
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn1q_u16 (uint16x8_t __a, uint16x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x8_t) {9, 1, 11, 3, 13, 5, 15, 7});
#else
  return __builtin_shuffle (__a, __b, (uint16x8_t) {0, 8, 2, 10, 4, 12, 6, 14});
#endif
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn1q_u32 (uint32x4_t __a, uint32x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x4_t) {5, 1, 7, 3});
#else
  return __builtin_shuffle (__a, __b, (uint32x4_t) {0, 4, 2, 6});
#endif
}
__extension__ extern __inline poly64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn1q_p64 (poly64x2_t __a, poly64x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (poly64x2_t) {3, 1});
#else
  return __builtin_shuffle (__a, __b, (poly64x2_t) {0, 2});
#endif
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn1q_u64 (uint64x2_t __a, uint64x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint64x2_t) {3, 1});
#else
  return __builtin_shuffle (__a, __b, (uint64x2_t) {0, 2});
#endif
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn2_f16 (float16x4_t __a, float16x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x4_t) {4, 0, 6, 2});
#else
  return __builtin_shuffle (__a, __b, (uint16x4_t) {1, 5, 3, 7});
#endif
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn2_f32 (float32x2_t __a, float32x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x2_t) {2, 0});
#else
  return __builtin_shuffle (__a, __b, (uint32x2_t) {1, 3});
#endif
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn2_p8 (poly8x8_t __a, poly8x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint8x8_t) {8, 0, 10, 2, 12, 4, 14, 6});
#else
  return __builtin_shuffle (__a, __b, (uint8x8_t) {1, 9, 3, 11, 5, 13, 7, 15});
#endif
}
__extension__ extern __inline poly16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn2_p16 (poly16x4_t __a, poly16x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x4_t) {4, 0, 6, 2});
#else
  return __builtin_shuffle (__a, __b, (uint16x4_t) {1, 5, 3, 7});
#endif
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn2_s8 (int8x8_t __a, int8x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint8x8_t) {8, 0, 10, 2, 12, 4, 14, 6});
#else
  return __builtin_shuffle (__a, __b, (uint8x8_t) {1, 9, 3, 11, 5, 13, 7, 15});
#endif
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn2_s16 (int16x4_t __a, int16x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x4_t) {4, 0, 6, 2});
#else
  return __builtin_shuffle (__a, __b, (uint16x4_t) {1, 5, 3, 7});
#endif
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn2_s32 (int32x2_t __a, int32x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x2_t) {2, 0});
#else
  return __builtin_shuffle (__a, __b, (uint32x2_t) {1, 3});
#endif
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn2_u8 (uint8x8_t __a, uint8x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint8x8_t) {8, 0, 10, 2, 12, 4, 14, 6});
#else
  return __builtin_shuffle (__a, __b, (uint8x8_t) {1, 9, 3, 11, 5, 13, 7, 15});
#endif
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn2_u16 (uint16x4_t __a, uint16x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x4_t) {4, 0, 6, 2});
#else
  return __builtin_shuffle (__a, __b, (uint16x4_t) {1, 5, 3, 7});
#endif
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn2_u32 (uint32x2_t __a, uint32x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x2_t) {2, 0});
#else
  return __builtin_shuffle (__a, __b, (uint32x2_t) {1, 3});
#endif
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn2q_f16 (float16x8_t __a, float16x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x8_t) {8, 0, 10, 2, 12, 4, 14, 6});
#else
  return __builtin_shuffle (__a, __b, (uint16x8_t) {1, 9, 3, 11, 5, 13, 7, 15});
#endif
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn2q_f32 (float32x4_t __a, float32x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x4_t) {4, 0, 6, 2});
#else
  return __builtin_shuffle (__a, __b, (uint32x4_t) {1, 5, 3, 7});
#endif
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn2q_f64 (float64x2_t __a, float64x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint64x2_t) {2, 0});
#else
  return __builtin_shuffle (__a, __b, (uint64x2_t) {1, 3});
#endif
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn2q_p8 (poly8x16_t __a, poly8x16_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b,
      (uint8x16_t) {16, 0, 18, 2, 20, 4, 22, 6, 24, 8, 26, 10, 28, 12, 30, 14});
#else
  return __builtin_shuffle (__a, __b,
      (uint8x16_t) {1, 17, 3, 19, 5, 21, 7, 23, 9, 25, 11, 27, 13, 29, 15, 31});
#endif
}
__extension__ extern __inline poly16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn2q_p16 (poly16x8_t __a, poly16x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x8_t) {8, 0, 10, 2, 12, 4, 14, 6});
#else
  return __builtin_shuffle (__a, __b, (uint16x8_t) {1, 9, 3, 11, 5, 13, 7, 15});
#endif
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn2q_s8 (int8x16_t __a, int8x16_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b,
      (uint8x16_t) {16, 0, 18, 2, 20, 4, 22, 6, 24, 8, 26, 10, 28, 12, 30, 14});
#else
  return __builtin_shuffle (__a, __b,
      (uint8x16_t) {1, 17, 3, 19, 5, 21, 7, 23, 9, 25, 11, 27, 13, 29, 15, 31});
#endif
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn2q_s16 (int16x8_t __a, int16x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x8_t) {8, 0, 10, 2, 12, 4, 14, 6});
#else
  return __builtin_shuffle (__a, __b, (uint16x8_t) {1, 9, 3, 11, 5, 13, 7, 15});
#endif
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn2q_s32 (int32x4_t __a, int32x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x4_t) {4, 0, 6, 2});
#else
  return __builtin_shuffle (__a, __b, (uint32x4_t) {1, 5, 3, 7});
#endif
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn2q_s64 (int64x2_t __a, int64x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint64x2_t) {2, 0});
#else
  return __builtin_shuffle (__a, __b, (uint64x2_t) {1, 3});
#endif
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn2q_u8 (uint8x16_t __a, uint8x16_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b,
      (uint8x16_t) {16, 0, 18, 2, 20, 4, 22, 6, 24, 8, 26, 10, 28, 12, 30, 14});
#else
  return __builtin_shuffle (__a, __b,
      (uint8x16_t) {1, 17, 3, 19, 5, 21, 7, 23, 9, 25, 11, 27, 13, 29, 15, 31});
#endif
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn2q_u16 (uint16x8_t __a, uint16x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x8_t) {8, 0, 10, 2, 12, 4, 14, 6});
#else
  return __builtin_shuffle (__a, __b, (uint16x8_t) {1, 9, 3, 11, 5, 13, 7, 15});
#endif
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn2q_u32 (uint32x4_t __a, uint32x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x4_t) {4, 0, 6, 2});
#else
  return __builtin_shuffle (__a, __b, (uint32x4_t) {1, 5, 3, 7});
#endif
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn2q_u64 (uint64x2_t __a, uint64x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint64x2_t) {2, 0});
#else
  return __builtin_shuffle (__a, __b, (uint64x2_t) {1, 3});
#endif
}
__extension__ extern __inline poly64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn2q_p64 (poly64x2_t __a, poly64x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (poly64x2_t) {2, 0});
#else
  return __builtin_shuffle (__a, __b, (poly64x2_t) {1, 3});
#endif
}
__extension__ extern __inline float16x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn_f16 (float16x4_t __a, float16x4_t __b)
{
  return (float16x4x2_t) {vtrn1_f16 (__a, __b), vtrn2_f16 (__a, __b)};
}
__extension__ extern __inline float32x2x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn_f32 (float32x2_t __a, float32x2_t __b)
{
  return (float32x2x2_t) {vtrn1_f32 (__a, __b), vtrn2_f32 (__a, __b)};
}
__extension__ extern __inline poly8x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn_p8 (poly8x8_t __a, poly8x8_t __b)
{
  return (poly8x8x2_t) {vtrn1_p8 (__a, __b), vtrn2_p8 (__a, __b)};
}
__extension__ extern __inline poly16x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn_p16 (poly16x4_t __a, poly16x4_t __b)
{
  return (poly16x4x2_t) {vtrn1_p16 (__a, __b), vtrn2_p16 (__a, __b)};
}
__extension__ extern __inline int8x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn_s8 (int8x8_t __a, int8x8_t __b)
{
  return (int8x8x2_t) {vtrn1_s8 (__a, __b), vtrn2_s8 (__a, __b)};
}
__extension__ extern __inline int16x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int16x4x2_t) {vtrn1_s16 (__a, __b), vtrn2_s16 (__a, __b)};
}
__extension__ extern __inline int32x2x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int32x2x2_t) {vtrn1_s32 (__a, __b), vtrn2_s32 (__a, __b)};
}
__extension__ extern __inline uint8x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint8x8x2_t) {vtrn1_u8 (__a, __b), vtrn2_u8 (__a, __b)};
}
__extension__ extern __inline uint16x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint16x4x2_t) {vtrn1_u16 (__a, __b), vtrn2_u16 (__a, __b)};
}
__extension__ extern __inline uint32x2x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrn_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint32x2x2_t) {vtrn1_u32 (__a, __b), vtrn2_u32 (__a, __b)};
}
__extension__ extern __inline float16x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrnq_f16 (float16x8_t __a, float16x8_t __b)
{
  return (float16x8x2_t) {vtrn1q_f16 (__a, __b), vtrn2q_f16 (__a, __b)};
}
__extension__ extern __inline float32x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrnq_f32 (float32x4_t __a, float32x4_t __b)
{
  return (float32x4x2_t) {vtrn1q_f32 (__a, __b), vtrn2q_f32 (__a, __b)};
}
__extension__ extern __inline poly8x16x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrnq_p8 (poly8x16_t __a, poly8x16_t __b)
{
  return (poly8x16x2_t) {vtrn1q_p8 (__a, __b), vtrn2q_p8 (__a, __b)};
}
__extension__ extern __inline poly16x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrnq_p16 (poly16x8_t __a, poly16x8_t __b)
{
  return (poly16x8x2_t) {vtrn1q_p16 (__a, __b), vtrn2q_p16 (__a, __b)};
}
__extension__ extern __inline int8x16x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrnq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (int8x16x2_t) {vtrn1q_s8 (__a, __b), vtrn2q_s8 (__a, __b)};
}
__extension__ extern __inline int16x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrnq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (int16x8x2_t) {vtrn1q_s16 (__a, __b), vtrn2q_s16 (__a, __b)};
}
__extension__ extern __inline int32x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrnq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (int32x4x2_t) {vtrn1q_s32 (__a, __b), vtrn2q_s32 (__a, __b)};
}
__extension__ extern __inline uint8x16x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrnq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return (uint8x16x2_t) {vtrn1q_u8 (__a, __b), vtrn2q_u8 (__a, __b)};
}
__extension__ extern __inline uint16x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrnq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (uint16x8x2_t) {vtrn1q_u16 (__a, __b), vtrn2q_u16 (__a, __b)};
}
__extension__ extern __inline uint32x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtrnq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (uint32x4x2_t) {vtrn1q_u32 (__a, __b), vtrn2q_u32 (__a, __b)};
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtst_s8 (int8x8_t __a, int8x8_t __b)
{
  return (uint8x8_t) ((__a & __b) != 0);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtst_s16 (int16x4_t __a, int16x4_t __b)
{
  return (uint16x4_t) ((__a & __b) != 0);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtst_s32 (int32x2_t __a, int32x2_t __b)
{
  return (uint32x2_t) ((__a & __b) != 0);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtst_s64 (int64x1_t __a, int64x1_t __b)
{
  return (uint64x1_t) ((__a & __b) != __AARCH64_INT64_C (0));
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtst_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return ((__a & __b) != 0);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtst_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return ((__a & __b) != 0);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtst_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return ((__a & __b) != 0);
}
__extension__ extern __inline uint64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtst_u64 (uint64x1_t __a, uint64x1_t __b)
{
  return ((__a & __b) != __AARCH64_UINT64_C (0));
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtstq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (uint8x16_t) ((__a & __b) != 0);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtstq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (uint16x8_t) ((__a & __b) != 0);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtstq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (uint32x4_t) ((__a & __b) != 0);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtstq_s64 (int64x2_t __a, int64x2_t __b)
{
  return (uint64x2_t) ((__a & __b) != __AARCH64_INT64_C (0));
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtstq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return ((__a & __b) != 0);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtstq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return ((__a & __b) != 0);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtstq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return ((__a & __b) != 0);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtstq_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return ((__a & __b) != __AARCH64_UINT64_C (0));
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtstd_s64 (int64_t __a, int64_t __b)
{
  return (__a & __b) ? -1ll : 0ll;
}
__extension__ extern __inline uint64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vtstd_u64 (uint64_t __a, uint64_t __b)
{
  return (__a & __b) ? -1ll : 0ll;
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuqadd_s8 (int8x8_t __a, uint8x8_t __b)
{
  return __builtin_aarch64_suqaddv8qi_ssu (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuqadd_s16 (int16x4_t __a, uint16x4_t __b)
{
  return __builtin_aarch64_suqaddv4hi_ssu (__a, __b);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuqadd_s32 (int32x2_t __a, uint32x2_t __b)
{
  return __builtin_aarch64_suqaddv2si_ssu (__a, __b);
}
__extension__ extern __inline int64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuqadd_s64 (int64x1_t __a, uint64x1_t __b)
{
  return (int64x1_t) {__builtin_aarch64_suqadddi_ssu (__a[0], __b[0])};
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuqaddq_s8 (int8x16_t __a, uint8x16_t __b)
{
  return __builtin_aarch64_suqaddv16qi_ssu (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuqaddq_s16 (int16x8_t __a, uint16x8_t __b)
{
  return __builtin_aarch64_suqaddv8hi_ssu (__a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuqaddq_s32 (int32x4_t __a, uint32x4_t __b)
{
  return __builtin_aarch64_suqaddv4si_ssu (__a, __b);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuqaddq_s64 (int64x2_t __a, uint64x2_t __b)
{
  return __builtin_aarch64_suqaddv2di_ssu (__a, __b);
}
__extension__ extern __inline int8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuqaddb_s8 (int8_t __a, uint8_t __b)
{
  return __builtin_aarch64_suqaddqi_ssu (__a, __b);
}
__extension__ extern __inline int16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuqaddh_s16 (int16_t __a, uint16_t __b)
{
  return __builtin_aarch64_suqaddhi_ssu (__a, __b);
}
__extension__ extern __inline int32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuqadds_s32 (int32_t __a, uint32_t __b)
{
  return __builtin_aarch64_suqaddsi_ssu (__a, __b);
}
__extension__ extern __inline int64_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuqaddd_s64 (int64_t __a, uint64_t __b)
{
  return __builtin_aarch64_suqadddi_ssu (__a, __b);
}
#define __DEFINTERLEAVE(op, rettype, intype, funcsuffix, Q) __extension__ extern __inline rettype __attribute__ ((__always_inline__, __gnu_inline__, __artificial__)) v ## op ## Q ## _ ## funcsuffix (intype a, intype b) { return (rettype) {v ## op ## 1 ## Q ## _ ## funcsuffix (a, b), v ## op ## 2 ## Q ## _ ## funcsuffix (a, b)}; }
#define __INTERLEAVE_LIST(op) __DEFINTERLEAVE (op, float16x4x2_t, float16x4_t, f16,) __DEFINTERLEAVE (op, float32x2x2_t, float32x2_t, f32,) __DEFINTERLEAVE (op, poly8x8x2_t, poly8x8_t, p8,) __DEFINTERLEAVE (op, poly16x4x2_t, poly16x4_t, p16,) __DEFINTERLEAVE (op, int8x8x2_t, int8x8_t, s8,) __DEFINTERLEAVE (op, int16x4x2_t, int16x4_t, s16,) __DEFINTERLEAVE (op, int32x2x2_t, int32x2_t, s32,) __DEFINTERLEAVE (op, uint8x8x2_t, uint8x8_t, u8,) __DEFINTERLEAVE (op, uint16x4x2_t, uint16x4_t, u16,) __DEFINTERLEAVE (op, uint32x2x2_t, uint32x2_t, u32,) __DEFINTERLEAVE (op, float16x8x2_t, float16x8_t, f16, q) __DEFINTERLEAVE (op, float32x4x2_t, float32x4_t, f32, q) __DEFINTERLEAVE (op, poly8x16x2_t, poly8x16_t, p8, q) __DEFINTERLEAVE (op, poly16x8x2_t, poly16x8_t, p16, q) __DEFINTERLEAVE (op, int8x16x2_t, int8x16_t, s8, q) __DEFINTERLEAVE (op, int16x8x2_t, int16x8_t, s16, q) __DEFINTERLEAVE (op, int32x4x2_t, int32x4_t, s32, q) __DEFINTERLEAVE (op, uint8x16x2_t, uint8x16_t, u8, q) __DEFINTERLEAVE (op, uint16x8x2_t, uint16x8_t, u16, q) __DEFINTERLEAVE (op, uint32x4x2_t, uint32x4_t, u32, q)
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp1_f16 (float16x4_t __a, float16x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x4_t) {5, 7, 1, 3});
#else
  return __builtin_shuffle (__a, __b, (uint16x4_t) {0, 2, 4, 6});
#endif
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp1_f32 (float32x2_t __a, float32x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x2_t) {3, 1});
#else
  return __builtin_shuffle (__a, __b, (uint32x2_t) {0, 2});
#endif
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp1_p8 (poly8x8_t __a, poly8x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint8x8_t) {9, 11, 13, 15, 1, 3, 5, 7});
#else
  return __builtin_shuffle (__a, __b, (uint8x8_t) {0, 2, 4, 6, 8, 10, 12, 14});
#endif
}
__extension__ extern __inline poly16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp1_p16 (poly16x4_t __a, poly16x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x4_t) {5, 7, 1, 3});
#else
  return __builtin_shuffle (__a, __b, (uint16x4_t) {0, 2, 4, 6});
#endif
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp1_s8 (int8x8_t __a, int8x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint8x8_t) {9, 11, 13, 15, 1, 3, 5, 7});
#else
  return __builtin_shuffle (__a, __b, (uint8x8_t) {0, 2, 4, 6, 8, 10, 12, 14});
#endif
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp1_s16 (int16x4_t __a, int16x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x4_t) {5, 7, 1, 3});
#else
  return __builtin_shuffle (__a, __b, (uint16x4_t) {0, 2, 4, 6});
#endif
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp1_s32 (int32x2_t __a, int32x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x2_t) {3, 1});
#else
  return __builtin_shuffle (__a, __b, (uint32x2_t) {0, 2});
#endif
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp1_u8 (uint8x8_t __a, uint8x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint8x8_t) {9, 11, 13, 15, 1, 3, 5, 7});
#else
  return __builtin_shuffle (__a, __b, (uint8x8_t) {0, 2, 4, 6, 8, 10, 12, 14});
#endif
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp1_u16 (uint16x4_t __a, uint16x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x4_t) {5, 7, 1, 3});
#else
  return __builtin_shuffle (__a, __b, (uint16x4_t) {0, 2, 4, 6});
#endif
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp1_u32 (uint32x2_t __a, uint32x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x2_t) {3, 1});
#else
  return __builtin_shuffle (__a, __b, (uint32x2_t) {0, 2});
#endif
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp1q_f16 (float16x8_t __a, float16x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x8_t) {9, 11, 13, 15, 1, 3, 5, 7});
#else
  return __builtin_shuffle (__a, __b, (uint16x8_t) {0, 2, 4, 6, 8, 10, 12, 14});
#endif
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp1q_f32 (float32x4_t __a, float32x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x4_t) {5, 7, 1, 3});
#else
  return __builtin_shuffle (__a, __b, (uint32x4_t) {0, 2, 4, 6});
#endif
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp1q_f64 (float64x2_t __a, float64x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint64x2_t) {3, 1});
#else
  return __builtin_shuffle (__a, __b, (uint64x2_t) {0, 2});
#endif
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp1q_p8 (poly8x16_t __a, poly8x16_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint8x16_t)
      {17, 19, 21, 23, 25, 27, 29, 31, 1, 3, 5, 7, 9, 11, 13, 15});
#else
  return __builtin_shuffle (__a, __b, (uint8x16_t)
      {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30});
#endif
}
__extension__ extern __inline poly16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp1q_p16 (poly16x8_t __a, poly16x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x8_t) {9, 11, 13, 15, 1, 3, 5, 7});
#else
  return __builtin_shuffle (__a, __b, (uint16x8_t) {0, 2, 4, 6, 8, 10, 12, 14});
#endif
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp1q_s8 (int8x16_t __a, int8x16_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b,
      (uint8x16_t) {17, 19, 21, 23, 25, 27, 29, 31, 1, 3, 5, 7, 9, 11, 13, 15});
#else
  return __builtin_shuffle (__a, __b,
      (uint8x16_t) {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30});
#endif
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp1q_s16 (int16x8_t __a, int16x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x8_t) {9, 11, 13, 15, 1, 3, 5, 7});
#else
  return __builtin_shuffle (__a, __b, (uint16x8_t) {0, 2, 4, 6, 8, 10, 12, 14});
#endif
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp1q_s32 (int32x4_t __a, int32x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x4_t) {5, 7, 1, 3});
#else
  return __builtin_shuffle (__a, __b, (uint32x4_t) {0, 2, 4, 6});
#endif
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp1q_s64 (int64x2_t __a, int64x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint64x2_t) {3, 1});
#else
  return __builtin_shuffle (__a, __b, (uint64x2_t) {0, 2});
#endif
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp1q_u8 (uint8x16_t __a, uint8x16_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b,
      (uint8x16_t) {17, 19, 21, 23, 25, 27, 29, 31, 1, 3, 5, 7, 9, 11, 13, 15});
#else
  return __builtin_shuffle (__a, __b,
      (uint8x16_t) {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30});
#endif
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp1q_u16 (uint16x8_t __a, uint16x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x8_t) {9, 11, 13, 15, 1, 3, 5, 7});
#else
  return __builtin_shuffle (__a, __b, (uint16x8_t) {0, 2, 4, 6, 8, 10, 12, 14});
#endif
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp1q_u32 (uint32x4_t __a, uint32x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x4_t) {5, 7, 1, 3});
#else
  return __builtin_shuffle (__a, __b, (uint32x4_t) {0, 2, 4, 6});
#endif
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp1q_u64 (uint64x2_t __a, uint64x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint64x2_t) {3, 1});
#else
  return __builtin_shuffle (__a, __b, (uint64x2_t) {0, 2});
#endif
}
__extension__ extern __inline poly64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp1q_p64 (poly64x2_t __a, poly64x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (poly64x2_t) {3, 1});
#else
  return __builtin_shuffle (__a, __b, (poly64x2_t) {0, 2});
#endif
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp2_f16 (float16x4_t __a, float16x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x4_t) {4, 6, 0, 2});
#else
  return __builtin_shuffle (__a, __b, (uint16x4_t) {1, 3, 5, 7});
#endif
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp2_f32 (float32x2_t __a, float32x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x2_t) {2, 0});
#else
  return __builtin_shuffle (__a, __b, (uint32x2_t) {1, 3});
#endif
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp2_p8 (poly8x8_t __a, poly8x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint8x8_t) {8, 10, 12, 14, 0, 2, 4, 6});
#else
  return __builtin_shuffle (__a, __b, (uint8x8_t) {1, 3, 5, 7, 9, 11, 13, 15});
#endif
}
__extension__ extern __inline poly16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp2_p16 (poly16x4_t __a, poly16x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x4_t) {4, 6, 0, 2});
#else
  return __builtin_shuffle (__a, __b, (uint16x4_t) {1, 3, 5, 7});
#endif
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp2_s8 (int8x8_t __a, int8x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint8x8_t) {8, 10, 12, 14, 0, 2, 4, 6});
#else
  return __builtin_shuffle (__a, __b, (uint8x8_t) {1, 3, 5, 7, 9, 11, 13, 15});
#endif
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp2_s16 (int16x4_t __a, int16x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x4_t) {4, 6, 0, 2});
#else
  return __builtin_shuffle (__a, __b, (uint16x4_t) {1, 3, 5, 7});
#endif
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp2_s32 (int32x2_t __a, int32x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x2_t) {2, 0});
#else
  return __builtin_shuffle (__a, __b, (uint32x2_t) {1, 3});
#endif
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp2_u8 (uint8x8_t __a, uint8x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint8x8_t) {8, 10, 12, 14, 0, 2, 4, 6});
#else
  return __builtin_shuffle (__a, __b, (uint8x8_t) {1, 3, 5, 7, 9, 11, 13, 15});
#endif
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp2_u16 (uint16x4_t __a, uint16x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x4_t) {4, 6, 0, 2});
#else
  return __builtin_shuffle (__a, __b, (uint16x4_t) {1, 3, 5, 7});
#endif
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp2_u32 (uint32x2_t __a, uint32x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x2_t) {2, 0});
#else
  return __builtin_shuffle (__a, __b, (uint32x2_t) {1, 3});
#endif
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp2q_f16 (float16x8_t __a, float16x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x8_t) {8, 10, 12, 14, 0, 2, 4, 6});
#else
  return __builtin_shuffle (__a, __b, (uint16x8_t) {1, 3, 5, 7, 9, 11, 13, 15});
#endif
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp2q_f32 (float32x4_t __a, float32x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x4_t) {4, 6, 0, 2});
#else
  return __builtin_shuffle (__a, __b, (uint32x4_t) {1, 3, 5, 7});
#endif
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp2q_f64 (float64x2_t __a, float64x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint64x2_t) {2, 0});
#else
  return __builtin_shuffle (__a, __b, (uint64x2_t) {1, 3});
#endif
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp2q_p8 (poly8x16_t __a, poly8x16_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b,
      (uint8x16_t) {16, 18, 20, 22, 24, 26, 28, 30, 0, 2, 4, 6, 8, 10, 12, 14});
#else
  return __builtin_shuffle (__a, __b,
      (uint8x16_t) {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31});
#endif
}
__extension__ extern __inline poly16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp2q_p16 (poly16x8_t __a, poly16x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x8_t) {8, 10, 12, 14, 0, 2, 4, 6});
#else
  return __builtin_shuffle (__a, __b, (uint16x8_t) {1, 3, 5, 7, 9, 11, 13, 15});
#endif
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp2q_s8 (int8x16_t __a, int8x16_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b,
      (uint8x16_t) {16, 18, 20, 22, 24, 26, 28, 30, 0, 2, 4, 6, 8, 10, 12, 14});
#else
  return __builtin_shuffle (__a, __b,
      (uint8x16_t) {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31});
#endif
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp2q_s16 (int16x8_t __a, int16x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x8_t) {8, 10, 12, 14, 0, 2, 4, 6});
#else
  return __builtin_shuffle (__a, __b, (uint16x8_t) {1, 3, 5, 7, 9, 11, 13, 15});
#endif
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp2q_s32 (int32x4_t __a, int32x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x4_t) {4, 6, 0, 2});
#else
  return __builtin_shuffle (__a, __b, (uint32x4_t) {1, 3, 5, 7});
#endif
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp2q_s64 (int64x2_t __a, int64x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint64x2_t) {2, 0});
#else
  return __builtin_shuffle (__a, __b, (uint64x2_t) {1, 3});
#endif
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp2q_u8 (uint8x16_t __a, uint8x16_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint8x16_t)
      {16, 18, 20, 22, 24, 26, 28, 30, 0, 2, 4, 6, 8, 10, 12, 14});
#else
  return __builtin_shuffle (__a, __b, (uint8x16_t)
      {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31});
#endif
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp2q_u16 (uint16x8_t __a, uint16x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x8_t) {8, 10, 12, 14, 0, 2, 4, 6});
#else
  return __builtin_shuffle (__a, __b, (uint16x8_t) {1, 3, 5, 7, 9, 11, 13, 15});
#endif
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp2q_u32 (uint32x4_t __a, uint32x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x4_t) {4, 6, 0, 2});
#else
  return __builtin_shuffle (__a, __b, (uint32x4_t) {1, 3, 5, 7});
#endif
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp2q_u64 (uint64x2_t __a, uint64x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint64x2_t) {2, 0});
#else
  return __builtin_shuffle (__a, __b, (uint64x2_t) {1, 3});
#endif
}
__extension__ extern __inline poly64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vuzp2q_p64 (poly64x2_t __a, poly64x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (poly64x2_t) {2, 0});
#else
  return __builtin_shuffle (__a, __b, (poly64x2_t) {1, 3});
#endif
}
__INTERLEAVE_LIST (uzp)
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip1_f16 (float16x4_t __a, float16x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x4_t) {6, 2, 7, 3});
#else
  return __builtin_shuffle (__a, __b, (uint16x4_t) {0, 4, 1, 5});
#endif
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip1_f32 (float32x2_t __a, float32x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x2_t) {3, 1});
#else
  return __builtin_shuffle (__a, __b, (uint32x2_t) {0, 2});
#endif
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip1_p8 (poly8x8_t __a, poly8x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint8x8_t) {12, 4, 13, 5, 14, 6, 15, 7});
#else
  return __builtin_shuffle (__a, __b, (uint8x8_t) {0, 8, 1, 9, 2, 10, 3, 11});
#endif
}
__extension__ extern __inline poly16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip1_p16 (poly16x4_t __a, poly16x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x4_t) {6, 2, 7, 3});
#else
  return __builtin_shuffle (__a, __b, (uint16x4_t) {0, 4, 1, 5});
#endif
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip1_s8 (int8x8_t __a, int8x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint8x8_t) {12, 4, 13, 5, 14, 6, 15, 7});
#else
  return __builtin_shuffle (__a, __b, (uint8x8_t) {0, 8, 1, 9, 2, 10, 3, 11});
#endif
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip1_s16 (int16x4_t __a, int16x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x4_t) {6, 2, 7, 3});
#else
  return __builtin_shuffle (__a, __b, (uint16x4_t) {0, 4, 1, 5});
#endif
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip1_s32 (int32x2_t __a, int32x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x2_t) {3, 1});
#else
  return __builtin_shuffle (__a, __b, (uint32x2_t) {0, 2});
#endif
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip1_u8 (uint8x8_t __a, uint8x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint8x8_t) {12, 4, 13, 5, 14, 6, 15, 7});
#else
  return __builtin_shuffle (__a, __b, (uint8x8_t) {0, 8, 1, 9, 2, 10, 3, 11});
#endif
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip1_u16 (uint16x4_t __a, uint16x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x4_t) {6, 2, 7, 3});
#else
  return __builtin_shuffle (__a, __b, (uint16x4_t) {0, 4, 1, 5});
#endif
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip1_u32 (uint32x2_t __a, uint32x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x2_t) {3, 1});
#else
  return __builtin_shuffle (__a, __b, (uint32x2_t) {0, 2});
#endif
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip1q_f16 (float16x8_t __a, float16x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b,
       (uint16x8_t) {12, 4, 13, 5, 14, 6, 15, 7});
#else
  return __builtin_shuffle (__a, __b,
       (uint16x8_t) {0, 8, 1, 9, 2, 10, 3, 11});
#endif
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip1q_f32 (float32x4_t __a, float32x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x4_t) {6, 2, 7, 3});
#else
  return __builtin_shuffle (__a, __b, (uint32x4_t) {0, 4, 1, 5});
#endif
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip1q_f64 (float64x2_t __a, float64x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint64x2_t) {3, 1});
#else
  return __builtin_shuffle (__a, __b, (uint64x2_t) {0, 2});
#endif
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip1q_p8 (poly8x16_t __a, poly8x16_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint8x16_t)
      {24, 8, 25, 9, 26, 10, 27, 11, 28, 12, 29, 13, 30, 14, 31, 15});
#else
  return __builtin_shuffle (__a, __b, (uint8x16_t)
      {0, 16, 1, 17, 2, 18, 3, 19, 4, 20, 5, 21, 6, 22, 7, 23});
#endif
}
__extension__ extern __inline poly16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip1q_p16 (poly16x8_t __a, poly16x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x8_t)
      {12, 4, 13, 5, 14, 6, 15, 7});
#else
  return __builtin_shuffle (__a, __b, (uint16x8_t) {0, 8, 1, 9, 2, 10, 3, 11});
#endif
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip1q_s8 (int8x16_t __a, int8x16_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint8x16_t)
      {24, 8, 25, 9, 26, 10, 27, 11, 28, 12, 29, 13, 30, 14, 31, 15});
#else
  return __builtin_shuffle (__a, __b, (uint8x16_t)
      {0, 16, 1, 17, 2, 18, 3, 19, 4, 20, 5, 21, 6, 22, 7, 23});
#endif
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip1q_s16 (int16x8_t __a, int16x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x8_t)
      {12, 4, 13, 5, 14, 6, 15, 7});
#else
  return __builtin_shuffle (__a, __b, (uint16x8_t) {0, 8, 1, 9, 2, 10, 3, 11});
#endif
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip1q_s32 (int32x4_t __a, int32x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x4_t) {6, 2, 7, 3});
#else
  return __builtin_shuffle (__a, __b, (uint32x4_t) {0, 4, 1, 5});
#endif
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip1q_s64 (int64x2_t __a, int64x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint64x2_t) {3, 1});
#else
  return __builtin_shuffle (__a, __b, (uint64x2_t) {0, 2});
#endif
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip1q_u8 (uint8x16_t __a, uint8x16_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint8x16_t)
      {24, 8, 25, 9, 26, 10, 27, 11, 28, 12, 29, 13, 30, 14, 31, 15});
#else
  return __builtin_shuffle (__a, __b, (uint8x16_t)
      {0, 16, 1, 17, 2, 18, 3, 19, 4, 20, 5, 21, 6, 22, 7, 23});
#endif
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip1q_u16 (uint16x8_t __a, uint16x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x8_t)
      {12, 4, 13, 5, 14, 6, 15, 7});
#else
  return __builtin_shuffle (__a, __b, (uint16x8_t) {0, 8, 1, 9, 2, 10, 3, 11});
#endif
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip1q_u32 (uint32x4_t __a, uint32x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x4_t) {6, 2, 7, 3});
#else
  return __builtin_shuffle (__a, __b, (uint32x4_t) {0, 4, 1, 5});
#endif
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip1q_u64 (uint64x2_t __a, uint64x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint64x2_t) {3, 1});
#else
  return __builtin_shuffle (__a, __b, (uint64x2_t) {0, 2});
#endif
}
__extension__ extern __inline poly64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip1q_p64 (poly64x2_t __a, poly64x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (poly64x2_t) {3, 1});
#else
  return __builtin_shuffle (__a, __b, (poly64x2_t) {0, 2});
#endif
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip2_f16 (float16x4_t __a, float16x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x4_t) {4, 0, 5, 1});
#else
  return __builtin_shuffle (__a, __b, (uint16x4_t) {2, 6, 3, 7});
#endif
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip2_f32 (float32x2_t __a, float32x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x2_t) {2, 0});
#else
  return __builtin_shuffle (__a, __b, (uint32x2_t) {1, 3});
#endif
}
__extension__ extern __inline poly8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip2_p8 (poly8x8_t __a, poly8x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint8x8_t) {8, 0, 9, 1, 10, 2, 11, 3});
#else
  return __builtin_shuffle (__a, __b, (uint8x8_t) {4, 12, 5, 13, 6, 14, 7, 15});
#endif
}
__extension__ extern __inline poly16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip2_p16 (poly16x4_t __a, poly16x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x4_t) {4, 0, 5, 1});
#else
  return __builtin_shuffle (__a, __b, (uint16x4_t) {2, 6, 3, 7});
#endif
}
__extension__ extern __inline int8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip2_s8 (int8x8_t __a, int8x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint8x8_t) {8, 0, 9, 1, 10, 2, 11, 3});
#else
  return __builtin_shuffle (__a, __b, (uint8x8_t) {4, 12, 5, 13, 6, 14, 7, 15});
#endif
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip2_s16 (int16x4_t __a, int16x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x4_t) {4, 0, 5, 1});
#else
  return __builtin_shuffle (__a, __b, (uint16x4_t) {2, 6, 3, 7});
#endif
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip2_s32 (int32x2_t __a, int32x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x2_t) {2, 0});
#else
  return __builtin_shuffle (__a, __b, (uint32x2_t) {1, 3});
#endif
}
__extension__ extern __inline uint8x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip2_u8 (uint8x8_t __a, uint8x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint8x8_t) {8, 0, 9, 1, 10, 2, 11, 3});
#else
  return __builtin_shuffle (__a, __b, (uint8x8_t) {4, 12, 5, 13, 6, 14, 7, 15});
#endif
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip2_u16 (uint16x4_t __a, uint16x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x4_t) {4, 0, 5, 1});
#else
  return __builtin_shuffle (__a, __b, (uint16x4_t) {2, 6, 3, 7});
#endif
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip2_u32 (uint32x2_t __a, uint32x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x2_t) {2, 0});
#else
  return __builtin_shuffle (__a, __b, (uint32x2_t) {1, 3});
#endif
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip2q_f16 (float16x8_t __a, float16x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b,
       (uint16x8_t) {8, 0, 9, 1, 10, 2, 11, 3});
#else
  return __builtin_shuffle (__a, __b,
       (uint16x8_t) {4, 12, 5, 13, 6, 14, 7, 15});
#endif
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip2q_f32 (float32x4_t __a, float32x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x4_t) {4, 0, 5, 1});
#else
  return __builtin_shuffle (__a, __b, (uint32x4_t) {2, 6, 3, 7});
#endif
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip2q_f64 (float64x2_t __a, float64x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint64x2_t) {2, 0});
#else
  return __builtin_shuffle (__a, __b, (uint64x2_t) {1, 3});
#endif
}
__extension__ extern __inline poly8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip2q_p8 (poly8x16_t __a, poly8x16_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint8x16_t)
      {16, 0, 17, 1, 18, 2, 19, 3, 20, 4, 21, 5, 22, 6, 23, 7});
#else
  return __builtin_shuffle (__a, __b, (uint8x16_t)
      {8, 24, 9, 25, 10, 26, 11, 27, 12, 28, 13, 29, 14, 30, 15, 31});
#endif
}
__extension__ extern __inline poly16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip2q_p16 (poly16x8_t __a, poly16x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x8_t) {8, 0, 9, 1, 10, 2, 11, 3});
#else
  return __builtin_shuffle (__a, __b, (uint16x8_t)
      {4, 12, 5, 13, 6, 14, 7, 15});
#endif
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip2q_s8 (int8x16_t __a, int8x16_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint8x16_t)
      {16, 0, 17, 1, 18, 2, 19, 3, 20, 4, 21, 5, 22, 6, 23, 7});
#else
  return __builtin_shuffle (__a, __b, (uint8x16_t)
      {8, 24, 9, 25, 10, 26, 11, 27, 12, 28, 13, 29, 14, 30, 15, 31});
#endif
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip2q_s16 (int16x8_t __a, int16x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x8_t) {8, 0, 9, 1, 10, 2, 11, 3});
#else
  return __builtin_shuffle (__a, __b, (uint16x8_t)
      {4, 12, 5, 13, 6, 14, 7, 15});
#endif
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip2q_s32 (int32x4_t __a, int32x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x4_t) {4, 0, 5, 1});
#else
  return __builtin_shuffle (__a, __b, (uint32x4_t) {2, 6, 3, 7});
#endif
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip2q_s64 (int64x2_t __a, int64x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint64x2_t) {2, 0});
#else
  return __builtin_shuffle (__a, __b, (uint64x2_t) {1, 3});
#endif
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip2q_u8 (uint8x16_t __a, uint8x16_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint8x16_t)
      {16, 0, 17, 1, 18, 2, 19, 3, 20, 4, 21, 5, 22, 6, 23, 7});
#else
  return __builtin_shuffle (__a, __b, (uint8x16_t)
      {8, 24, 9, 25, 10, 26, 11, 27, 12, 28, 13, 29, 14, 30, 15, 31});
#endif
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip2q_u16 (uint16x8_t __a, uint16x8_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint16x8_t) {8, 0, 9, 1, 10, 2, 11, 3});
#else
  return __builtin_shuffle (__a, __b, (uint16x8_t)
      {4, 12, 5, 13, 6, 14, 7, 15});
#endif
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip2q_u32 (uint32x4_t __a, uint32x4_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint32x4_t) {4, 0, 5, 1});
#else
  return __builtin_shuffle (__a, __b, (uint32x4_t) {2, 6, 3, 7});
#endif
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip2q_u64 (uint64x2_t __a, uint64x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (uint64x2_t) {2, 0});
#else
  return __builtin_shuffle (__a, __b, (uint64x2_t) {1, 3});
#endif
}
__extension__ extern __inline poly64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vzip2q_p64 (poly64x2_t __a, poly64x2_t __b)
{
#ifdef __AARCH64EB__
  return __builtin_shuffle (__a, __b, (poly64x2_t) {2, 0});
#else
  return __builtin_shuffle (__a, __b, (poly64x2_t) {1, 3});
#endif
}
__INTERLEAVE_LIST (zip)
#undef __INTERLEAVE_LIST
#undef __DEFINTERLEAVE
#pragma GCC pop_options
#include "third_party/aarch64/arm_fp16.internal.h"
#pragma GCC push_options
#pragma GCC target ("+nothing+simd+fp16")
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabs_f16 (float16x4_t __a)
{
  return __builtin_aarch64_absv4hf (__a);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabsq_f16 (float16x8_t __a)
{
  return __builtin_aarch64_absv8hf (__a);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqz_f16 (float16x4_t __a)
{
  return __builtin_aarch64_cmeqv4hf_uss (__a, vdup_n_f16 (0.0f));
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqzq_f16 (float16x8_t __a)
{
  return __builtin_aarch64_cmeqv8hf_uss (__a, vdupq_n_f16 (0.0f));
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgez_f16 (float16x4_t __a)
{
  return __builtin_aarch64_cmgev4hf_uss (__a, vdup_n_f16 (0.0f));
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgezq_f16 (float16x8_t __a)
{
  return __builtin_aarch64_cmgev8hf_uss (__a, vdupq_n_f16 (0.0f));
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgtz_f16 (float16x4_t __a)
{
  return __builtin_aarch64_cmgtv4hf_uss (__a, vdup_n_f16 (0.0f));
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgtzq_f16 (float16x8_t __a)
{
  return __builtin_aarch64_cmgtv8hf_uss (__a, vdupq_n_f16 (0.0f));
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclez_f16 (float16x4_t __a)
{
  return __builtin_aarch64_cmlev4hf_uss (__a, vdup_n_f16 (0.0f));
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclezq_f16 (float16x8_t __a)
{
  return __builtin_aarch64_cmlev8hf_uss (__a, vdupq_n_f16 (0.0f));
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcltz_f16 (float16x4_t __a)
{
  return __builtin_aarch64_cmltv4hf_uss (__a, vdup_n_f16 (0.0f));
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcltzq_f16 (float16x8_t __a)
{
  return __builtin_aarch64_cmltv8hf_uss (__a, vdupq_n_f16 (0.0f));
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvt_f16_s16 (int16x4_t __a)
{
  return __builtin_aarch64_floatv4hiv4hf (__a);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtq_f16_s16 (int16x8_t __a)
{
  return __builtin_aarch64_floatv8hiv8hf (__a);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvt_f16_u16 (uint16x4_t __a)
{
  return __builtin_aarch64_floatunsv4hiv4hf ((int16x4_t) __a);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtq_f16_u16 (uint16x8_t __a)
{
  return __builtin_aarch64_floatunsv8hiv8hf ((int16x8_t) __a);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvt_s16_f16 (float16x4_t __a)
{
  return __builtin_aarch64_lbtruncv4hfv4hi (__a);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtq_s16_f16 (float16x8_t __a)
{
  return __builtin_aarch64_lbtruncv8hfv8hi (__a);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvt_u16_f16 (float16x4_t __a)
{
  return __builtin_aarch64_lbtruncuv4hfv4hi_us (__a);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtq_u16_f16 (float16x8_t __a)
{
  return __builtin_aarch64_lbtruncuv8hfv8hi_us (__a);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvta_s16_f16 (float16x4_t __a)
{
  return __builtin_aarch64_lroundv4hfv4hi (__a);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtaq_s16_f16 (float16x8_t __a)
{
  return __builtin_aarch64_lroundv8hfv8hi (__a);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvta_u16_f16 (float16x4_t __a)
{
  return __builtin_aarch64_lrounduv4hfv4hi_us (__a);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtaq_u16_f16 (float16x8_t __a)
{
  return __builtin_aarch64_lrounduv8hfv8hi_us (__a);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtm_s16_f16 (float16x4_t __a)
{
  return __builtin_aarch64_lfloorv4hfv4hi (__a);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtmq_s16_f16 (float16x8_t __a)
{
  return __builtin_aarch64_lfloorv8hfv8hi (__a);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtm_u16_f16 (float16x4_t __a)
{
  return __builtin_aarch64_lflooruv4hfv4hi_us (__a);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtmq_u16_f16 (float16x8_t __a)
{
  return __builtin_aarch64_lflooruv8hfv8hi_us (__a);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtn_s16_f16 (float16x4_t __a)
{
  return __builtin_aarch64_lfrintnv4hfv4hi (__a);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtnq_s16_f16 (float16x8_t __a)
{
  return __builtin_aarch64_lfrintnv8hfv8hi (__a);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtn_u16_f16 (float16x4_t __a)
{
  return __builtin_aarch64_lfrintnuv4hfv4hi_us (__a);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtnq_u16_f16 (float16x8_t __a)
{
  return __builtin_aarch64_lfrintnuv8hfv8hi_us (__a);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtp_s16_f16 (float16x4_t __a)
{
  return __builtin_aarch64_lceilv4hfv4hi (__a);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtpq_s16_f16 (float16x8_t __a)
{
  return __builtin_aarch64_lceilv8hfv8hi (__a);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtp_u16_f16 (float16x4_t __a)
{
  return __builtin_aarch64_lceiluv4hfv4hi_us (__a);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtpq_u16_f16 (float16x8_t __a)
{
  return __builtin_aarch64_lceiluv8hfv8hi_us (__a);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vneg_f16 (float16x4_t __a)
{
  return -__a;
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vnegq_f16 (float16x8_t __a)
{
  return -__a;
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrecpe_f16 (float16x4_t __a)
{
  return __builtin_aarch64_frecpev4hf (__a);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrecpeq_f16 (float16x8_t __a)
{
  return __builtin_aarch64_frecpev8hf (__a);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrnd_f16 (float16x4_t __a)
{
  return __builtin_aarch64_btruncv4hf (__a);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndq_f16 (float16x8_t __a)
{
  return __builtin_aarch64_btruncv8hf (__a);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrnda_f16 (float16x4_t __a)
{
  return __builtin_aarch64_roundv4hf (__a);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndaq_f16 (float16x8_t __a)
{
  return __builtin_aarch64_roundv8hf (__a);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndi_f16 (float16x4_t __a)
{
  return __builtin_aarch64_nearbyintv4hf (__a);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndiq_f16 (float16x8_t __a)
{
  return __builtin_aarch64_nearbyintv8hf (__a);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndm_f16 (float16x4_t __a)
{
  return __builtin_aarch64_floorv4hf (__a);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndmq_f16 (float16x8_t __a)
{
  return __builtin_aarch64_floorv8hf (__a);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndn_f16 (float16x4_t __a)
{
  return __builtin_aarch64_roundevenv4hf (__a);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndnq_f16 (float16x8_t __a)
{
  return __builtin_aarch64_roundevenv8hf (__a);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndp_f16 (float16x4_t __a)
{
  return __builtin_aarch64_ceilv4hf (__a);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndpq_f16 (float16x8_t __a)
{
  return __builtin_aarch64_ceilv8hf (__a);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndx_f16 (float16x4_t __a)
{
  return __builtin_aarch64_rintv4hf (__a);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrndxq_f16 (float16x8_t __a)
{
  return __builtin_aarch64_rintv8hf (__a);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsqrte_f16 (float16x4_t __a)
{
  return __builtin_aarch64_rsqrtev4hf (__a);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsqrteq_f16 (float16x8_t __a)
{
  return __builtin_aarch64_rsqrtev8hf (__a);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsqrt_f16 (float16x4_t __a)
{
  return __builtin_aarch64_sqrtv4hf (__a);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsqrtq_f16 (float16x8_t __a)
{
  return __builtin_aarch64_sqrtv8hf (__a);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vadd_f16 (float16x4_t __a, float16x4_t __b)
{
  return __a + __b;
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vaddq_f16 (float16x8_t __a, float16x8_t __b)
{
  return __a + __b;
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabd_f16 (float16x4_t __a, float16x4_t __b)
{
  return __builtin_aarch64_fabdv4hf (__a, __b);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vabdq_f16 (float16x8_t __a, float16x8_t __b)
{
  return __builtin_aarch64_fabdv8hf (__a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcage_f16 (float16x4_t __a, float16x4_t __b)
{
  return __builtin_aarch64_facgev4hf_uss (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcageq_f16 (float16x8_t __a, float16x8_t __b)
{
  return __builtin_aarch64_facgev8hf_uss (__a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcagt_f16 (float16x4_t __a, float16x4_t __b)
{
  return __builtin_aarch64_facgtv4hf_uss (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcagtq_f16 (float16x8_t __a, float16x8_t __b)
{
  return __builtin_aarch64_facgtv8hf_uss (__a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcale_f16 (float16x4_t __a, float16x4_t __b)
{
  return __builtin_aarch64_faclev4hf_uss (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcaleq_f16 (float16x8_t __a, float16x8_t __b)
{
  return __builtin_aarch64_faclev8hf_uss (__a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcalt_f16 (float16x4_t __a, float16x4_t __b)
{
  return __builtin_aarch64_facltv4hf_uss (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcaltq_f16 (float16x8_t __a, float16x8_t __b)
{
  return __builtin_aarch64_facltv8hf_uss (__a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceq_f16 (float16x4_t __a, float16x4_t __b)
{
  return __builtin_aarch64_cmeqv4hf_uss (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vceqq_f16 (float16x8_t __a, float16x8_t __b)
{
  return __builtin_aarch64_cmeqv8hf_uss (__a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcge_f16 (float16x4_t __a, float16x4_t __b)
{
  return __builtin_aarch64_cmgev4hf_uss (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgeq_f16 (float16x8_t __a, float16x8_t __b)
{
  return __builtin_aarch64_cmgev8hf_uss (__a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgt_f16 (float16x4_t __a, float16x4_t __b)
{
  return __builtin_aarch64_cmgtv4hf_uss (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcgtq_f16 (float16x8_t __a, float16x8_t __b)
{
  return __builtin_aarch64_cmgtv8hf_uss (__a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcle_f16 (float16x4_t __a, float16x4_t __b)
{
  return __builtin_aarch64_cmlev4hf_uss (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcleq_f16 (float16x8_t __a, float16x8_t __b)
{
  return __builtin_aarch64_cmlev8hf_uss (__a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vclt_f16 (float16x4_t __a, float16x4_t __b)
{
  return __builtin_aarch64_cmltv4hf_uss (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcltq_f16 (float16x8_t __a, float16x8_t __b)
{
  return __builtin_aarch64_cmltv8hf_uss (__a, __b);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvt_n_f16_s16 (int16x4_t __a, const int __b)
{
  return __builtin_aarch64_scvtfv4hi (__a, __b);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtq_n_f16_s16 (int16x8_t __a, const int __b)
{
  return __builtin_aarch64_scvtfv8hi (__a, __b);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvt_n_f16_u16 (uint16x4_t __a, const int __b)
{
  return __builtin_aarch64_ucvtfv4hi_sus (__a, __b);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtq_n_f16_u16 (uint16x8_t __a, const int __b)
{
  return __builtin_aarch64_ucvtfv8hi_sus (__a, __b);
}
__extension__ extern __inline int16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvt_n_s16_f16 (float16x4_t __a, const int __b)
{
  return __builtin_aarch64_fcvtzsv4hf (__a, __b);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtq_n_s16_f16 (float16x8_t __a, const int __b)
{
  return __builtin_aarch64_fcvtzsv8hf (__a, __b);
}
__extension__ extern __inline uint16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvt_n_u16_f16 (float16x4_t __a, const int __b)
{
  return __builtin_aarch64_fcvtzuv4hf_uss (__a, __b);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtq_n_u16_f16 (float16x8_t __a, const int __b)
{
  return __builtin_aarch64_fcvtzuv8hf_uss (__a, __b);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdiv_f16 (float16x4_t __a, float16x4_t __b)
{
  return __a / __b;
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdivq_f16 (float16x8_t __a, float16x8_t __b)
{
  return __a / __b;
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmax_f16 (float16x4_t __a, float16x4_t __b)
{
  return __builtin_aarch64_fmax_nanv4hf (__a, __b);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxq_f16 (float16x8_t __a, float16x8_t __b)
{
  return __builtin_aarch64_fmax_nanv8hf (__a, __b);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxnm_f16 (float16x4_t __a, float16x4_t __b)
{
  return __builtin_aarch64_fmaxv4hf (__a, __b);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxnmq_f16 (float16x8_t __a, float16x8_t __b)
{
  return __builtin_aarch64_fmaxv8hf (__a, __b);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmin_f16 (float16x4_t __a, float16x4_t __b)
{
  return __builtin_aarch64_fmin_nanv4hf (__a, __b);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminq_f16 (float16x8_t __a, float16x8_t __b)
{
  return __builtin_aarch64_fmin_nanv8hf (__a, __b);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminnm_f16 (float16x4_t __a, float16x4_t __b)
{
  return __builtin_aarch64_fminv4hf (__a, __b);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminnmq_f16 (float16x8_t __a, float16x8_t __b)
{
  return __builtin_aarch64_fminv8hf (__a, __b);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmul_f16 (float16x4_t __a, float16x4_t __b)
{
  return __a * __b;
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulq_f16 (float16x8_t __a, float16x8_t __b)
{
  return __a * __b;
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulx_f16 (float16x4_t __a, float16x4_t __b)
{
  return __builtin_aarch64_fmulxv4hf (__a, __b);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulxq_f16 (float16x8_t __a, float16x8_t __b)
{
  return __builtin_aarch64_fmulxv8hf (__a, __b);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpadd_f16 (float16x4_t __a, float16x4_t __b)
{
  return __builtin_aarch64_faddpv4hf (__a, __b);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpaddq_f16 (float16x8_t __a, float16x8_t __b)
{
  return __builtin_aarch64_faddpv8hf (__a, __b);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmax_f16 (float16x4_t __a, float16x4_t __b)
{
  return __builtin_aarch64_smax_nanpv4hf (__a, __b);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmaxq_f16 (float16x8_t __a, float16x8_t __b)
{
  return __builtin_aarch64_smax_nanpv8hf (__a, __b);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmaxnm_f16 (float16x4_t __a, float16x4_t __b)
{
  return __builtin_aarch64_smaxpv4hf (__a, __b);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmaxnmq_f16 (float16x8_t __a, float16x8_t __b)
{
  return __builtin_aarch64_smaxpv8hf (__a, __b);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpmin_f16 (float16x4_t __a, float16x4_t __b)
{
  return __builtin_aarch64_smin_nanpv4hf (__a, __b);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpminq_f16 (float16x8_t __a, float16x8_t __b)
{
  return __builtin_aarch64_smin_nanpv8hf (__a, __b);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpminnm_f16 (float16x4_t __a, float16x4_t __b)
{
  return __builtin_aarch64_sminpv4hf (__a, __b);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vpminnmq_f16 (float16x8_t __a, float16x8_t __b)
{
  return __builtin_aarch64_sminpv8hf (__a, __b);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrecps_f16 (float16x4_t __a, float16x4_t __b)
{
  return __builtin_aarch64_frecpsv4hf (__a, __b);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrecpsq_f16 (float16x8_t __a, float16x8_t __b)
{
  return __builtin_aarch64_frecpsv8hf (__a, __b);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsqrts_f16 (float16x4_t __a, float16x4_t __b)
{
  return __builtin_aarch64_rsqrtsv4hf (__a, __b);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrsqrtsq_f16 (float16x8_t __a, float16x8_t __b)
{
  return __builtin_aarch64_rsqrtsv8hf (__a, __b);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsub_f16 (float16x4_t __a, float16x4_t __b)
{
  return __a - __b;
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsubq_f16 (float16x8_t __a, float16x8_t __b)
{
  return __a - __b;
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfma_f16 (float16x4_t __a, float16x4_t __b, float16x4_t __c)
{
  return __builtin_aarch64_fmav4hf (__b, __c, __a);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmaq_f16 (float16x8_t __a, float16x8_t __b, float16x8_t __c)
{
  return __builtin_aarch64_fmav8hf (__b, __c, __a);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfms_f16 (float16x4_t __a, float16x4_t __b, float16x4_t __c)
{
  return __builtin_aarch64_fnmav4hf (__b, __c, __a);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmsq_f16 (float16x8_t __a, float16x8_t __b, float16x8_t __c)
{
  return __builtin_aarch64_fnmav8hf (__b, __c, __a);
}
__extension__ extern __inline float16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmah_lane_f16 (float16_t __a, float16_t __b,
  float16x4_t __c, const int __lane)
{
  return vfmah_f16 (__a, __b, __aarch64_vget_lane_any (__c, __lane));
}
__extension__ extern __inline float16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmah_laneq_f16 (float16_t __a, float16_t __b,
   float16x8_t __c, const int __lane)
{
  return vfmah_f16 (__a, __b, __aarch64_vget_lane_any (__c, __lane));
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfma_lane_f16 (float16x4_t __a, float16x4_t __b,
        float16x4_t __c, const int __lane)
{
  return vfma_f16 (__a, __b, __aarch64_vdup_lane_f16 (__c, __lane));
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmaq_lane_f16 (float16x8_t __a, float16x8_t __b,
  float16x4_t __c, const int __lane)
{
  return vfmaq_f16 (__a, __b, __aarch64_vdupq_lane_f16 (__c, __lane));
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfma_laneq_f16 (float16x4_t __a, float16x4_t __b,
  float16x8_t __c, const int __lane)
{
  return vfma_f16 (__a, __b, __aarch64_vdup_laneq_f16 (__c, __lane));
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmaq_laneq_f16 (float16x8_t __a, float16x8_t __b,
   float16x8_t __c, const int __lane)
{
  return vfmaq_f16 (__a, __b, __aarch64_vdupq_laneq_f16 (__c, __lane));
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfma_n_f16 (float16x4_t __a, float16x4_t __b, float16_t __c)
{
  return vfma_f16 (__a, __b, vdup_n_f16 (__c));
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmaq_n_f16 (float16x8_t __a, float16x8_t __b, float16_t __c)
{
  return vfmaq_f16 (__a, __b, vdupq_n_f16 (__c));
}
__extension__ extern __inline float16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmsh_lane_f16 (float16_t __a, float16_t __b,
  float16x4_t __c, const int __lane)
{
  return vfmsh_f16 (__a, __b, __aarch64_vget_lane_any (__c, __lane));
}
__extension__ extern __inline float16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmsh_laneq_f16 (float16_t __a, float16_t __b,
   float16x8_t __c, const int __lane)
{
  return vfmsh_f16 (__a, __b, __aarch64_vget_lane_any (__c, __lane));
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfms_lane_f16 (float16x4_t __a, float16x4_t __b,
        float16x4_t __c, const int __lane)
{
  return vfms_f16 (__a, __b, __aarch64_vdup_lane_f16 (__c, __lane));
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmsq_lane_f16 (float16x8_t __a, float16x8_t __b,
  float16x4_t __c, const int __lane)
{
  return vfmsq_f16 (__a, __b, __aarch64_vdupq_lane_f16 (__c, __lane));
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfms_laneq_f16 (float16x4_t __a, float16x4_t __b,
  float16x8_t __c, const int __lane)
{
  return vfms_f16 (__a, __b, __aarch64_vdup_laneq_f16 (__c, __lane));
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmsq_laneq_f16 (float16x8_t __a, float16x8_t __b,
   float16x8_t __c, const int __lane)
{
  return vfmsq_f16 (__a, __b, __aarch64_vdupq_laneq_f16 (__c, __lane));
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfms_n_f16 (float16x4_t __a, float16x4_t __b, float16_t __c)
{
  return vfms_f16 (__a, __b, vdup_n_f16 (__c));
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmsq_n_f16 (float16x8_t __a, float16x8_t __b, float16_t __c)
{
  return vfmsq_f16 (__a, __b, vdupq_n_f16 (__c));
}
__extension__ extern __inline float16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulh_lane_f16 (float16_t __a, float16x4_t __b, const int __lane)
{
  return __a * __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmul_lane_f16 (float16x4_t __a, float16x4_t __b, const int __lane)
{
  return vmul_f16 (__a, vdup_n_f16 (__aarch64_vget_lane_any (__b, __lane)));
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulq_lane_f16 (float16x8_t __a, float16x4_t __b, const int __lane)
{
  return vmulq_f16 (__a, vdupq_n_f16 (__aarch64_vget_lane_any (__b, __lane)));
}
__extension__ extern __inline float16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulh_laneq_f16 (float16_t __a, float16x8_t __b, const int __lane)
{
  return __a * __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmul_laneq_f16 (float16x4_t __a, float16x8_t __b, const int __lane)
{
  return vmul_f16 (__a, vdup_n_f16 (__aarch64_vget_lane_any (__b, __lane)));
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulq_laneq_f16 (float16x8_t __a, float16x8_t __b, const int __lane)
{
  return vmulq_f16 (__a, vdupq_n_f16 (__aarch64_vget_lane_any (__b, __lane)));
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmul_n_f16 (float16x4_t __a, float16_t __b)
{
  return vmul_lane_f16 (__a, vdup_n_f16 (__b), 0);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulq_n_f16 (float16x8_t __a, float16_t __b)
{
  return vmulq_laneq_f16 (__a, vdupq_n_f16 (__b), 0);
}
__extension__ extern __inline float16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulxh_lane_f16 (float16_t __a, float16x4_t __b, const int __lane)
{
  return vmulxh_f16 (__a, __aarch64_vget_lane_any (__b, __lane));
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulx_lane_f16 (float16x4_t __a, float16x4_t __b, const int __lane)
{
  return vmulx_f16 (__a, __aarch64_vdup_lane_f16 (__b, __lane));
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulxq_lane_f16 (float16x8_t __a, float16x4_t __b, const int __lane)
{
  return vmulxq_f16 (__a, __aarch64_vdupq_lane_f16 (__b, __lane));
}
__extension__ extern __inline float16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulxh_laneq_f16 (float16_t __a, float16x8_t __b, const int __lane)
{
  return vmulxh_f16 (__a, __aarch64_vget_lane_any (__b, __lane));
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulx_laneq_f16 (float16x4_t __a, float16x8_t __b, const int __lane)
{
  return vmulx_f16 (__a, __aarch64_vdup_laneq_f16 (__b, __lane));
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulxq_laneq_f16 (float16x8_t __a, float16x8_t __b, const int __lane)
{
  return vmulxq_f16 (__a, __aarch64_vdupq_laneq_f16 (__b, __lane));
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulx_n_f16 (float16x4_t __a, float16_t __b)
{
  return vmulx_f16 (__a, vdup_n_f16 (__b));
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmulxq_n_f16 (float16x8_t __a, float16_t __b)
{
  return vmulxq_f16 (__a, vdupq_n_f16 (__b));
}
__extension__ extern __inline float16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxv_f16 (float16x4_t __a)
{
  return __builtin_aarch64_reduc_smax_nan_scal_v4hf (__a);
}
__extension__ extern __inline float16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxvq_f16 (float16x8_t __a)
{
  return __builtin_aarch64_reduc_smax_nan_scal_v8hf (__a);
}
__extension__ extern __inline float16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminv_f16 (float16x4_t __a)
{
  return __builtin_aarch64_reduc_smin_nan_scal_v4hf (__a);
}
__extension__ extern __inline float16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminvq_f16 (float16x8_t __a)
{
  return __builtin_aarch64_reduc_smin_nan_scal_v8hf (__a);
}
__extension__ extern __inline float16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxnmv_f16 (float16x4_t __a)
{
  return __builtin_aarch64_reduc_smax_scal_v4hf (__a);
}
__extension__ extern __inline float16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmaxnmvq_f16 (float16x8_t __a)
{
  return __builtin_aarch64_reduc_smax_scal_v8hf (__a);
}
__extension__ extern __inline float16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminnmv_f16 (float16x4_t __a)
{
  return __builtin_aarch64_reduc_smin_scal_v4hf (__a);
}
__extension__ extern __inline float16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vminnmvq_f16 (float16x8_t __a)
{
  return __builtin_aarch64_reduc_smin_scal_v8hf (__a);
}
#pragma GCC pop_options
#pragma GCC push_options
#pragma GCC target ("+nothing+dotprod")
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdot_u32 (uint32x2_t __r, uint8x8_t __a, uint8x8_t __b)
{
  return __builtin_aarch64_udot_prodv8qi_uuuu (__a, __b, __r);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdotq_u32 (uint32x4_t __r, uint8x16_t __a, uint8x16_t __b)
{
  return __builtin_aarch64_udot_prodv16qi_uuuu (__a, __b, __r);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdot_s32 (int32x2_t __r, int8x8_t __a, int8x8_t __b)
{
  return __builtin_aarch64_sdot_prodv8qi (__a, __b, __r);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdotq_s32 (int32x4_t __r, int8x16_t __a, int8x16_t __b)
{
  return __builtin_aarch64_sdot_prodv16qi (__a, __b, __r);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdot_lane_u32 (uint32x2_t __r, uint8x8_t __a, uint8x8_t __b, const int __index)
{
  return __builtin_aarch64_udot_lanev8qi_uuuus (__r, __a, __b, __index);
}
__extension__ extern __inline uint32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdot_laneq_u32 (uint32x2_t __r, uint8x8_t __a, uint8x16_t __b,
  const int __index)
{
  return __builtin_aarch64_udot_laneqv8qi_uuuus (__r, __a, __b, __index);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdotq_lane_u32 (uint32x4_t __r, uint8x16_t __a, uint8x8_t __b,
  const int __index)
{
  return __builtin_aarch64_udot_lanev16qi_uuuus (__r, __a, __b, __index);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdotq_laneq_u32 (uint32x4_t __r, uint8x16_t __a, uint8x16_t __b,
   const int __index)
{
  return __builtin_aarch64_udot_laneqv16qi_uuuus (__r, __a, __b, __index);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdot_lane_s32 (int32x2_t __r, int8x8_t __a, int8x8_t __b, const int __index)
{
  return __builtin_aarch64_sdot_lanev8qi (__r, __a, __b, __index);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdot_laneq_s32 (int32x2_t __r, int8x8_t __a, int8x16_t __b, const int __index)
{
  return __builtin_aarch64_sdot_laneqv8qi (__r, __a, __b, __index);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdotq_lane_s32 (int32x4_t __r, int8x16_t __a, int8x8_t __b, const int __index)
{
  return __builtin_aarch64_sdot_lanev16qi (__r, __a, __b, __index);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdotq_laneq_s32 (int32x4_t __r, int8x16_t __a, int8x16_t __b, const int __index)
{
  return __builtin_aarch64_sdot_laneqv16qi (__r, __a, __b, __index);
}
#pragma GCC pop_options
#pragma GCC push_options
#pragma GCC target ("+nothing+sm4")
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsm3ss1q_u32 (uint32x4_t __a, uint32x4_t __b, uint32x4_t __c)
{
  return __builtin_aarch64_sm3ss1qv4si_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsm3tt1aq_u32 (uint32x4_t __a, uint32x4_t __b, uint32x4_t __c, const int __imm2)
{
  return __builtin_aarch64_sm3tt1aqv4si_uuuus (__a, __b, __c, __imm2);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsm3tt1bq_u32 (uint32x4_t __a, uint32x4_t __b, uint32x4_t __c, const int __imm2)
{
  return __builtin_aarch64_sm3tt1bqv4si_uuuus (__a, __b, __c, __imm2);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsm3tt2aq_u32 (uint32x4_t __a, uint32x4_t __b, uint32x4_t __c, const int __imm2)
{
  return __builtin_aarch64_sm3tt2aqv4si_uuuus (__a, __b, __c, __imm2);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsm3tt2bq_u32 (uint32x4_t __a, uint32x4_t __b, uint32x4_t __c, const int __imm2)
{
  return __builtin_aarch64_sm3tt2bqv4si_uuuus (__a, __b, __c, __imm2);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsm3partw1q_u32 (uint32x4_t __a, uint32x4_t __b, uint32x4_t __c)
{
  return __builtin_aarch64_sm3partw1qv4si_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsm3partw2q_u32 (uint32x4_t __a, uint32x4_t __b, uint32x4_t __c)
{
  return __builtin_aarch64_sm3partw2qv4si_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsm4eq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return __builtin_aarch64_sm4eqv4si_uuu (__a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsm4ekeyq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return __builtin_aarch64_sm4ekeyqv4si_uuu (__a, __b);
}
#pragma GCC pop_options
#pragma GCC push_options
#pragma GCC target ("+nothing+sha3")
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsha512hq_u64 (uint64x2_t __a, uint64x2_t __b, uint64x2_t __c)
{
  return __builtin_aarch64_crypto_sha512hqv2di_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsha512h2q_u64 (uint64x2_t __a, uint64x2_t __b, uint64x2_t __c)
{
  return __builtin_aarch64_crypto_sha512h2qv2di_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsha512su0q_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return __builtin_aarch64_crypto_sha512su0qv2di_uuu (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsha512su1q_u64 (uint64x2_t __a, uint64x2_t __b, uint64x2_t __c)
{
  return __builtin_aarch64_crypto_sha512su1qv2di_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
veor3q_u8 (uint8x16_t __a, uint8x16_t __b, uint8x16_t __c)
{
  return __builtin_aarch64_eor3qv16qi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
veor3q_u16 (uint16x8_t __a, uint16x8_t __b, uint16x8_t __c)
{
  return __builtin_aarch64_eor3qv8hi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
veor3q_u32 (uint32x4_t __a, uint32x4_t __b, uint32x4_t __c)
{
  return __builtin_aarch64_eor3qv4si_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
veor3q_u64 (uint64x2_t __a, uint64x2_t __b, uint64x2_t __c)
{
  return __builtin_aarch64_eor3qv2di_uuuu (__a, __b, __c);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
veor3q_s8 (int8x16_t __a, int8x16_t __b, int8x16_t __c)
{
  return __builtin_aarch64_eor3qv16qi (__a, __b, __c);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
veor3q_s16 (int16x8_t __a, int16x8_t __b, int16x8_t __c)
{
  return __builtin_aarch64_eor3qv8hi (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
veor3q_s32 (int32x4_t __a, int32x4_t __b, int32x4_t __c)
{
  return __builtin_aarch64_eor3qv4si (__a, __b, __c);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
veor3q_s64 (int64x2_t __a, int64x2_t __b, int64x2_t __c)
{
  return __builtin_aarch64_eor3qv2di (__a, __b, __c);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrax1q_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return __builtin_aarch64_rax1qv2di_uuu (__a, __b);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vxarq_u64 (uint64x2_t __a, uint64x2_t __b, const int imm6)
{
  return __builtin_aarch64_xarqv2di_uuus (__a, __b,imm6);
}
__extension__ extern __inline uint8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbcaxq_u8 (uint8x16_t __a, uint8x16_t __b, uint8x16_t __c)
{
  return __builtin_aarch64_bcaxqv16qi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbcaxq_u16 (uint16x8_t __a, uint16x8_t __b, uint16x8_t __c)
{
  return __builtin_aarch64_bcaxqv8hi_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbcaxq_u32 (uint32x4_t __a, uint32x4_t __b, uint32x4_t __c)
{
  return __builtin_aarch64_bcaxqv4si_uuuu (__a, __b, __c);
}
__extension__ extern __inline uint64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbcaxq_u64 (uint64x2_t __a, uint64x2_t __b, uint64x2_t __c)
{
  return __builtin_aarch64_bcaxqv2di_uuuu (__a, __b, __c);
}
__extension__ extern __inline int8x16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbcaxq_s8 (int8x16_t __a, int8x16_t __b, int8x16_t __c)
{
  return __builtin_aarch64_bcaxqv16qi (__a, __b, __c);
}
__extension__ extern __inline int16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbcaxq_s16 (int16x8_t __a, int16x8_t __b, int16x8_t __c)
{
  return __builtin_aarch64_bcaxqv8hi (__a, __b, __c);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbcaxq_s32 (int32x4_t __a, int32x4_t __b, int32x4_t __c)
{
  return __builtin_aarch64_bcaxqv4si (__a, __b, __c);
}
__extension__ extern __inline int64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbcaxq_s64 (int64x2_t __a, int64x2_t __b, int64x2_t __c)
{
  return __builtin_aarch64_bcaxqv2di (__a, __b, __c);
}
#pragma GCC pop_options
#pragma GCC push_options
#pragma GCC target ("arch=armv8.3-a")
#pragma GCC push_options
#pragma GCC target ("+fp16")
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcadd_rot90_f16 (float16x4_t __a, float16x4_t __b)
{
  return __builtin_aarch64_fcadd90v4hf (__a, __b);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcaddq_rot90_f16 (float16x8_t __a, float16x8_t __b)
{
  return __builtin_aarch64_fcadd90v8hf (__a, __b);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcadd_rot270_f16 (float16x4_t __a, float16x4_t __b)
{
  return __builtin_aarch64_fcadd270v4hf (__a, __b);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcaddq_rot270_f16 (float16x8_t __a, float16x8_t __b)
{
  return __builtin_aarch64_fcadd270v8hf (__a, __b);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmla_f16 (float16x4_t __r, float16x4_t __a, float16x4_t __b)
{
  return __builtin_aarch64_fcmla0v4hf (__r, __a, __b);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmlaq_f16 (float16x8_t __r, float16x8_t __a, float16x8_t __b)
{
  return __builtin_aarch64_fcmla0v8hf (__r, __a, __b);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmla_lane_f16 (float16x4_t __r, float16x4_t __a, float16x4_t __b,
  const int __index)
{
  return __builtin_aarch64_fcmla_lane0v4hf (__r, __a, __b, __index);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmla_laneq_f16 (float16x4_t __r, float16x4_t __a, float16x8_t __b,
   const int __index)
{
  return __builtin_aarch64_fcmla_laneq0v4hf (__r, __a, __b, __index);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmlaq_lane_f16 (float16x8_t __r, float16x8_t __a, float16x4_t __b,
   const int __index)
{
  return __builtin_aarch64_fcmlaq_lane0v8hf (__r, __a, __b, __index);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmlaq_rot90_lane_f16 (float16x8_t __r, float16x8_t __a, float16x4_t __b,
         const int __index)
{
  return __builtin_aarch64_fcmlaq_lane90v8hf (__r, __a, __b, __index);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmla_rot90_laneq_f16 (float16x4_t __r, float16x4_t __a, float16x8_t __b,
         const int __index)
{
  return __builtin_aarch64_fcmla_laneq90v4hf (__r, __a, __b, __index);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmla_rot90_lane_f16 (float16x4_t __r, float16x4_t __a, float16x4_t __b,
        const int __index)
{
  return __builtin_aarch64_fcmla_lane90v4hf (__r, __a, __b, __index);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmlaq_rot90_f16 (float16x8_t __r, float16x8_t __a, float16x8_t __b)
{
  return __builtin_aarch64_fcmla90v8hf (__r, __a, __b);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmla_rot90_f16 (float16x4_t __r, float16x4_t __a, float16x4_t __b)
{
  return __builtin_aarch64_fcmla90v4hf (__r, __a, __b);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmlaq_laneq_f16 (float16x8_t __r, float16x8_t __a, float16x8_t __b,
    const int __index)
{
  return __builtin_aarch64_fcmla_lane0v8hf (__r, __a, __b, __index);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmla_rot180_laneq_f16 (float16x4_t __r, float16x4_t __a, float16x8_t __b,
   const int __index)
{
  return __builtin_aarch64_fcmla_laneq180v4hf (__r, __a, __b, __index);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmla_rot180_lane_f16 (float16x4_t __r, float16x4_t __a, float16x4_t __b,
         const int __index)
{
  return __builtin_aarch64_fcmla_lane180v4hf (__r, __a, __b, __index);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmlaq_rot180_f16 (float16x8_t __r, float16x8_t __a, float16x8_t __b)
{
  return __builtin_aarch64_fcmla180v8hf (__r, __a, __b);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmla_rot180_f16 (float16x4_t __r, float16x4_t __a, float16x4_t __b)
{
  return __builtin_aarch64_fcmla180v4hf (__r, __a, __b);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmlaq_rot90_laneq_f16 (float16x8_t __r, float16x8_t __a, float16x8_t __b,
   const int __index)
{
  return __builtin_aarch64_fcmla_lane90v8hf (__r, __a, __b, __index);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmlaq_rot270_laneq_f16 (float16x8_t __r, float16x8_t __a, float16x8_t __b,
    const int __index)
{
  return __builtin_aarch64_fcmla_lane270v8hf (__r, __a, __b, __index);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmlaq_rot270_lane_f16 (float16x8_t __r, float16x8_t __a, float16x4_t __b,
   const int __index)
{
  return __builtin_aarch64_fcmlaq_lane270v8hf (__r, __a, __b, __index);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmla_rot270_laneq_f16 (float16x4_t __r, float16x4_t __a, float16x8_t __b,
   const int __index)
{
  return __builtin_aarch64_fcmla_laneq270v4hf (__r, __a, __b, __index);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmlaq_rot270_f16 (float16x8_t __r, float16x8_t __a, float16x8_t __b)
{
  return __builtin_aarch64_fcmla270v8hf (__r, __a, __b);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmla_rot270_f16 (float16x4_t __r, float16x4_t __a, float16x4_t __b)
{
  return __builtin_aarch64_fcmla270v4hf (__r, __a, __b);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmlaq_rot180_laneq_f16 (float16x8_t __r, float16x8_t __a, float16x8_t __b,
    const int __index)
{
  return __builtin_aarch64_fcmla_lane180v8hf (__r, __a, __b, __index);
}
__extension__ extern __inline float16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmlaq_rot180_lane_f16 (float16x8_t __r, float16x8_t __a, float16x4_t __b,
   const int __index)
{
  return __builtin_aarch64_fcmlaq_lane180v8hf (__r, __a, __b, __index);
}
__extension__ extern __inline float16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmla_rot270_lane_f16 (float16x4_t __r, float16x4_t __a, float16x4_t __b,
         const int __index)
{
  return __builtin_aarch64_fcmla_lane270v4hf (__r, __a, __b, __index);
}
#pragma GCC pop_options
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcadd_rot90_f32 (float32x2_t __a, float32x2_t __b)
{
  return __builtin_aarch64_fcadd90v2sf (__a, __b);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcaddq_rot90_f32 (float32x4_t __a, float32x4_t __b)
{
  return __builtin_aarch64_fcadd90v4sf (__a, __b);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcaddq_rot90_f64 (float64x2_t __a, float64x2_t __b)
{
  return __builtin_aarch64_fcadd90v2df (__a, __b);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcadd_rot270_f32 (float32x2_t __a, float32x2_t __b)
{
  return __builtin_aarch64_fcadd270v2sf (__a, __b);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcaddq_rot270_f32 (float32x4_t __a, float32x4_t __b)
{
  return __builtin_aarch64_fcadd270v4sf (__a, __b);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcaddq_rot270_f64 (float64x2_t __a, float64x2_t __b)
{
  return __builtin_aarch64_fcadd270v2df (__a, __b);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmla_f32 (float32x2_t __r, float32x2_t __a, float32x2_t __b)
{
  return __builtin_aarch64_fcmla0v2sf (__r, __a, __b);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmlaq_f32 (float32x4_t __r, float32x4_t __a, float32x4_t __b)
{
  return __builtin_aarch64_fcmla0v4sf (__r, __a, __b);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmlaq_f64 (float64x2_t __r, float64x2_t __a, float64x2_t __b)
{
  return __builtin_aarch64_fcmla0v2df (__r, __a, __b);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmla_lane_f32 (float32x2_t __r, float32x2_t __a, float32x2_t __b,
  const int __index)
{
  return __builtin_aarch64_fcmla_lane0v2sf (__r, __a, __b, __index);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmla_laneq_f32 (float32x2_t __r, float32x2_t __a, float32x4_t __b,
   const int __index)
{
  return __builtin_aarch64_fcmla_laneq0v2sf (__r, __a, __b, __index);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmlaq_lane_f32 (float32x4_t __r, float32x4_t __a, float32x2_t __b,
   const int __index)
{
  return __builtin_aarch64_fcmlaq_lane0v4sf (__r, __a, __b, __index);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmlaq_laneq_f32 (float32x4_t __r, float32x4_t __a, float32x4_t __b,
    const int __index)
{
  return __builtin_aarch64_fcmla_lane0v4sf (__r, __a, __b, __index);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmla_rot90_f32 (float32x2_t __r, float32x2_t __a, float32x2_t __b)
{
  return __builtin_aarch64_fcmla90v2sf (__r, __a, __b);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmlaq_rot90_f32 (float32x4_t __r, float32x4_t __a, float32x4_t __b)
{
  return __builtin_aarch64_fcmla90v4sf (__r, __a, __b);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmlaq_rot90_f64 (float64x2_t __r, float64x2_t __a, float64x2_t __b)
{
  return __builtin_aarch64_fcmla90v2df (__r, __a, __b);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmla_rot90_lane_f32 (float32x2_t __r, float32x2_t __a, float32x2_t __b,
        const int __index)
{
  return __builtin_aarch64_fcmla_lane90v2sf (__r, __a, __b, __index);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmla_rot90_laneq_f32 (float32x2_t __r, float32x2_t __a, float32x4_t __b,
         const int __index)
{
  return __builtin_aarch64_fcmla_laneq90v2sf (__r, __a, __b, __index);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmlaq_rot90_lane_f32 (float32x4_t __r, float32x4_t __a, float32x2_t __b,
         const int __index)
{
  return __builtin_aarch64_fcmlaq_lane90v4sf (__r, __a, __b, __index);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmlaq_rot90_laneq_f32 (float32x4_t __r, float32x4_t __a, float32x4_t __b,
   const int __index)
{
  return __builtin_aarch64_fcmla_lane90v4sf (__r, __a, __b, __index);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmla_rot180_f32 (float32x2_t __r, float32x2_t __a, float32x2_t __b)
{
  return __builtin_aarch64_fcmla180v2sf (__r, __a, __b);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmlaq_rot180_f32 (float32x4_t __r, float32x4_t __a, float32x4_t __b)
{
  return __builtin_aarch64_fcmla180v4sf (__r, __a, __b);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmlaq_rot180_f64 (float64x2_t __r, float64x2_t __a, float64x2_t __b)
{
  return __builtin_aarch64_fcmla180v2df (__r, __a, __b);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmla_rot180_lane_f32 (float32x2_t __r, float32x2_t __a, float32x2_t __b,
         const int __index)
{
  return __builtin_aarch64_fcmla_lane180v2sf (__r, __a, __b, __index);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmla_rot180_laneq_f32 (float32x2_t __r, float32x2_t __a, float32x4_t __b,
   const int __index)
{
  return __builtin_aarch64_fcmla_laneq180v2sf (__r, __a, __b, __index);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmlaq_rot180_lane_f32 (float32x4_t __r, float32x4_t __a, float32x2_t __b,
   const int __index)
{
  return __builtin_aarch64_fcmlaq_lane180v4sf (__r, __a, __b, __index);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmlaq_rot180_laneq_f32 (float32x4_t __r, float32x4_t __a, float32x4_t __b,
    const int __index)
{
  return __builtin_aarch64_fcmla_lane180v4sf (__r, __a, __b, __index);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmla_rot270_f32 (float32x2_t __r, float32x2_t __a, float32x2_t __b)
{
  return __builtin_aarch64_fcmla270v2sf (__r, __a, __b);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmlaq_rot270_f32 (float32x4_t __r, float32x4_t __a, float32x4_t __b)
{
  return __builtin_aarch64_fcmla270v4sf (__r, __a, __b);
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmlaq_rot270_f64 (float64x2_t __r, float64x2_t __a, float64x2_t __b)
{
  return __builtin_aarch64_fcmla270v2df (__r, __a, __b);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmla_rot270_lane_f32 (float32x2_t __r, float32x2_t __a, float32x2_t __b,
         const int __index)
{
  return __builtin_aarch64_fcmla_lane270v2sf (__r, __a, __b, __index);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmla_rot270_laneq_f32 (float32x2_t __r, float32x2_t __a, float32x4_t __b,
   const int __index)
{
  return __builtin_aarch64_fcmla_laneq270v2sf (__r, __a, __b, __index);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmlaq_rot270_lane_f32 (float32x4_t __r, float32x4_t __a, float32x2_t __b,
   const int __index)
{
  return __builtin_aarch64_fcmlaq_lane270v4sf (__r, __a, __b, __index);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcmlaq_rot270_laneq_f32 (float32x4_t __r, float32x4_t __a, float32x4_t __b,
    const int __index)
{
  return __builtin_aarch64_fcmla_lane270v4sf (__r, __a, __b, __index);
}
#pragma GCC pop_options
#pragma GCC push_options
#pragma GCC target ("+nothing+fp16fml")
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmlal_low_f16 (float32x2_t __r, float16x4_t __a, float16x4_t __b)
{
  return __builtin_aarch64_fmlal_lowv2sf (__r, __a, __b);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmlsl_low_f16 (float32x2_t __r, float16x4_t __a, float16x4_t __b)
{
  return __builtin_aarch64_fmlsl_lowv2sf (__r, __a, __b);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmlalq_low_f16 (float32x4_t __r, float16x8_t __a, float16x8_t __b)
{
  return __builtin_aarch64_fmlalq_lowv4sf (__r, __a, __b);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmlslq_low_f16 (float32x4_t __r, float16x8_t __a, float16x8_t __b)
{
  return __builtin_aarch64_fmlslq_lowv4sf (__r, __a, __b);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmlal_high_f16 (float32x2_t __r, float16x4_t __a, float16x4_t __b)
{
  return __builtin_aarch64_fmlal_highv2sf (__r, __a, __b);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmlsl_high_f16 (float32x2_t __r, float16x4_t __a, float16x4_t __b)
{
  return __builtin_aarch64_fmlsl_highv2sf (__r, __a, __b);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmlalq_high_f16 (float32x4_t __r, float16x8_t __a, float16x8_t __b)
{
  return __builtin_aarch64_fmlalq_highv4sf (__r, __a, __b);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmlslq_high_f16 (float32x4_t __r, float16x8_t __a, float16x8_t __b)
{
  return __builtin_aarch64_fmlslq_highv4sf (__r, __a, __b);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmlal_lane_low_f16 (float32x2_t __r, float16x4_t __a, float16x4_t __b,
       const int __lane)
{
  return __builtin_aarch64_fmlal_lane_lowv2sf (__r, __a, __b, __lane);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmlsl_lane_low_f16 (float32x2_t __r, float16x4_t __a, float16x4_t __b,
       const int __lane)
{
  return __builtin_aarch64_fmlsl_lane_lowv2sf (__r, __a, __b, __lane);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmlal_laneq_low_f16 (float32x2_t __r, float16x4_t __a, float16x8_t __b,
        const int __lane)
{
  return __builtin_aarch64_fmlal_laneq_lowv2sf (__r, __a, __b, __lane);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmlsl_laneq_low_f16 (float32x2_t __r, float16x4_t __a, float16x8_t __b,
        const int __lane)
{
  return __builtin_aarch64_fmlsl_laneq_lowv2sf (__r, __a, __b, __lane);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmlalq_lane_low_f16 (float32x4_t __r, float16x8_t __a, float16x4_t __b,
        const int __lane)
{
  return __builtin_aarch64_fmlalq_lane_lowv4sf (__r, __a, __b, __lane);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmlslq_lane_low_f16 (float32x4_t __r, float16x8_t __a, float16x4_t __b,
        const int __lane)
{
  return __builtin_aarch64_fmlslq_lane_lowv4sf (__r, __a, __b, __lane);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmlalq_laneq_low_f16 (float32x4_t __r, float16x8_t __a, float16x8_t __b,
         const int __lane)
{
  return __builtin_aarch64_fmlalq_laneq_lowv4sf (__r, __a, __b, __lane);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmlslq_laneq_low_f16 (float32x4_t __r, float16x8_t __a, float16x8_t __b,
        const int __lane)
{
  return __builtin_aarch64_fmlslq_laneq_lowv4sf (__r, __a, __b, __lane);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmlal_lane_high_f16 (float32x2_t __r, float16x4_t __a, float16x4_t __b,
       const int __lane)
{
  return __builtin_aarch64_fmlal_lane_highv2sf (__r, __a, __b, __lane);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmlsl_lane_high_f16 (float32x2_t __r, float16x4_t __a, float16x4_t __b,
       const int __lane)
{
  return __builtin_aarch64_fmlsl_lane_highv2sf (__r, __a, __b, __lane);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmlal_laneq_high_f16 (float32x2_t __r, float16x4_t __a, float16x8_t __b,
        const int __lane)
{
  return __builtin_aarch64_fmlal_laneq_highv2sf (__r, __a, __b, __lane);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmlsl_laneq_high_f16 (float32x2_t __r, float16x4_t __a, float16x8_t __b,
        const int __lane)
{
  return __builtin_aarch64_fmlsl_laneq_highv2sf (__r, __a, __b, __lane);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmlalq_lane_high_f16 (float32x4_t __r, float16x8_t __a, float16x4_t __b,
        const int __lane)
{
  return __builtin_aarch64_fmlalq_lane_highv4sf (__r, __a, __b, __lane);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmlslq_lane_high_f16 (float32x4_t __r, float16x8_t __a, float16x4_t __b,
        const int __lane)
{
  return __builtin_aarch64_fmlslq_lane_highv4sf (__r, __a, __b, __lane);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmlalq_laneq_high_f16 (float32x4_t __r, float16x8_t __a, float16x8_t __b,
         const int __lane)
{
  return __builtin_aarch64_fmlalq_laneq_highv4sf (__r, __a, __b, __lane);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vfmlslq_laneq_high_f16 (float32x4_t __r, float16x8_t __a, float16x8_t __b,
        const int __lane)
{
  return __builtin_aarch64_fmlslq_laneq_highv4sf (__r, __a, __b, __lane);
}
#pragma GCC pop_options
#pragma GCC push_options
#pragma GCC target ("arch=armv8.5-a")
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrnd32z_f32 (float32x2_t __a)
{
  return __builtin_aarch64_frint32zv2sf (__a);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrnd32zq_f32 (float32x4_t __a)
{
  return __builtin_aarch64_frint32zv4sf (__a);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrnd32z_f64 (float64x1_t __a)
{
  return (float64x1_t)
    {__builtin_aarch64_frint32zdf (vget_lane_f64 (__a, 0))};
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrnd32zq_f64 (float64x2_t __a)
{
  return __builtin_aarch64_frint32zv2df (__a);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrnd32x_f32 (float32x2_t __a)
{
  return __builtin_aarch64_frint32xv2sf (__a);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrnd32xq_f32 (float32x4_t __a)
{
  return __builtin_aarch64_frint32xv4sf (__a);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrnd32x_f64 (float64x1_t __a)
{
  return (float64x1_t) {__builtin_aarch64_frint32xdf (vget_lane_f64 (__a, 0))};
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrnd32xq_f64 (float64x2_t __a)
{
  return __builtin_aarch64_frint32xv2df (__a);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrnd64z_f32 (float32x2_t __a)
{
  return __builtin_aarch64_frint64zv2sf (__a);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrnd64zq_f32 (float32x4_t __a)
{
  return __builtin_aarch64_frint64zv4sf (__a);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrnd64z_f64 (float64x1_t __a)
{
  return (float64x1_t) {__builtin_aarch64_frint64zdf (vget_lane_f64 (__a, 0))};
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrnd64zq_f64 (float64x2_t __a)
{
  return __builtin_aarch64_frint64zv2df (__a);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrnd64x_f32 (float32x2_t __a)
{
  return __builtin_aarch64_frint64xv2sf (__a);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrnd64xq_f32 (float32x4_t __a)
{
  return __builtin_aarch64_frint64xv4sf (__a);
}
__extension__ extern __inline float64x1_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrnd64x_f64 (float64x1_t __a)
{
  return (float64x1_t) {__builtin_aarch64_frint64xdf (vget_lane_f64 (__a, 0))};
}
__extension__ extern __inline float64x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vrnd64xq_f64 (float64x2_t __a)
{
  return __builtin_aarch64_frint64xv2df (__a);
}
#pragma GCC pop_options
#include "third_party/aarch64/arm_bf16.internal.h"
#pragma GCC push_options
#pragma GCC target ("+nothing+bf16")
__extension__ extern __inline bfloat16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vset_lane_bf16 (bfloat16_t __elem, bfloat16x4_t __vec, const int __index)
{
  return __aarch64_vset_lane_any (__elem, __vec, __index);
}
__extension__ extern __inline bfloat16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsetq_lane_bf16 (bfloat16_t __elem, bfloat16x8_t __vec, const int __index)
{
  return __aarch64_vset_lane_any (__elem, __vec, __index);
}
__extension__ extern __inline bfloat16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_lane_bf16 (bfloat16x4_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline bfloat16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vgetq_lane_bf16 (bfloat16x8_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline bfloat16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcreate_bf16 (uint64_t __a)
{
  return (bfloat16x4_t) __a;
}
__extension__ extern __inline bfloat16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcombine_bf16 (bfloat16x4_t __a, bfloat16x4_t __b)
{
  return __builtin_aarch64_combinev4bf (__a, __b);
}
__extension__ extern __inline bfloat16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_n_bf16 (bfloat16_t __a)
{
  return (bfloat16x4_t) {__a, __a, __a, __a};
}
__extension__ extern __inline bfloat16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_n_bf16 (bfloat16_t __a)
{
  return (bfloat16x8_t) {__a, __a, __a, __a, __a, __a, __a, __a};
}
__extension__ extern __inline bfloat16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_lane_bf16 (bfloat16x4_t __a, const int __b)
{
  return vdup_n_bf16 (__aarch64_vget_lane_any (__a, __b));
}
__extension__ extern __inline bfloat16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdup_laneq_bf16 (bfloat16x8_t __a, const int __b)
{
  return vdup_n_bf16 (__aarch64_vget_lane_any (__a, __b));
}
__extension__ extern __inline bfloat16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_lane_bf16 (bfloat16x4_t __a, const int __b)
{
  return vdupq_n_bf16 (__aarch64_vget_lane_any (__a, __b));
}
__extension__ extern __inline bfloat16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vdupq_laneq_bf16 (bfloat16x8_t __a, const int __b)
{
  return vdupq_n_bf16 (__aarch64_vget_lane_any (__a, __b));
}
__extension__ extern __inline bfloat16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vduph_lane_bf16 (bfloat16x4_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline bfloat16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vduph_laneq_bf16 (bfloat16x8_t __a, const int __b)
{
  return __aarch64_vget_lane_any (__a, __b);
}
__extension__ extern __inline bfloat16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_bf16 (const bfloat16_t *__a)
{
  return (bfloat16x4_t) __builtin_aarch64_ld1v4bf (__a);
}
__extension__ extern __inline bfloat16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_bf16 (const bfloat16_t *__a)
{
  return __builtin_aarch64_ld1v8bf (__a);
}
__extension__ extern __inline bfloat16x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_bf16_x2 (const bfloat16_t *__a)
{
  return __builtin_aarch64_ld1x2v4bf ((const __builtin_aarch64_simd_bf *) __a);
}
__extension__ extern __inline bfloat16x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_bf16_x2 (const bfloat16_t *__a)
{
  return __builtin_aarch64_ld1x2v8bf (
    (const __builtin_aarch64_simd_bf *) __a);
}
__extension__ extern __inline bfloat16x4x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_bf16_x3 (const bfloat16_t *__a)
{
  return __builtin_aarch64_ld1x3v4bf ((const __builtin_aarch64_simd_bf *) __a);
}
__extension__ extern __inline bfloat16x8x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_bf16_x3 (const bfloat16_t *__a)
{
  return __builtin_aarch64_ld1x3v8bf ((const __builtin_aarch64_simd_bf *) __a);
}
__extension__ extern __inline bfloat16x4x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_bf16_x4 (const bfloat16_t *__a)
{
  return __builtin_aarch64_ld1x4v4bf ((const __builtin_aarch64_simd_bf *) __a);
}
__extension__ extern __inline bfloat16x8x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_bf16_x4 (const bfloat16_t *__a)
{
  return __builtin_aarch64_ld1x4v8bf ((const __builtin_aarch64_simd_bf *) __a);
}
__extension__ extern __inline bfloat16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_lane_bf16 (const bfloat16_t *__src, bfloat16x4_t __vec, const int __lane)
{
  return __aarch64_vset_lane_any (*__src, __vec, __lane);
}
__extension__ extern __inline bfloat16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_lane_bf16 (const bfloat16_t *__src, bfloat16x8_t __vec, const int __lane)
{
  return __aarch64_vset_lane_any (*__src, __vec, __lane);
}
__extension__ extern __inline bfloat16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1_dup_bf16 (const bfloat16_t* __a)
{
  return vdup_n_bf16 (*__a);
}
__extension__ extern __inline bfloat16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld1q_dup_bf16 (const bfloat16_t* __a)
{
  return vdupq_n_bf16 (*__a);
}
__extension__ extern __inline bfloat16x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2_bf16 (const bfloat16_t * __a)
{
  return __builtin_aarch64_ld2v4bf (__a);
}
__extension__ extern __inline bfloat16x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2q_bf16 (const bfloat16_t * __a)
{
  return __builtin_aarch64_ld2v8bf ((const __builtin_aarch64_simd_bf *) __a);
}
__extension__ extern __inline bfloat16x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2_dup_bf16 (const bfloat16_t * __a)
{
  return __builtin_aarch64_ld2rv4bf ((const __builtin_aarch64_simd_bf *) __a);
}
__extension__ extern __inline bfloat16x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld2q_dup_bf16 (const bfloat16_t * __a)
{
  return __builtin_aarch64_ld2rv8bf ((const __builtin_aarch64_simd_bf *) __a);
}
__extension__ extern __inline bfloat16x4x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3_bf16 (const bfloat16_t * __a)
{
  return __builtin_aarch64_ld3v4bf ((const __builtin_aarch64_simd_bf *) __a);
}
__extension__ extern __inline bfloat16x8x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3q_bf16 (const bfloat16_t * __a)
{
  return __builtin_aarch64_ld3v8bf ((const __builtin_aarch64_simd_bf *) __a);
}
__extension__ extern __inline bfloat16x4x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3_dup_bf16 (const bfloat16_t * __a)
{
  return __builtin_aarch64_ld3rv4bf ((const __builtin_aarch64_simd_bf *) __a);
}
__extension__ extern __inline bfloat16x8x3_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld3q_dup_bf16 (const bfloat16_t * __a)
{
  return __builtin_aarch64_ld3rv8bf ((const __builtin_aarch64_simd_bf *) __a);
}
__extension__ extern __inline bfloat16x4x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4_bf16 (const bfloat16_t * __a)
{
  return __builtin_aarch64_ld4v4bf ((const __builtin_aarch64_simd_bf *) __a);
}
__extension__ extern __inline bfloat16x8x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4q_bf16 (const bfloat16_t * __a)
{
  return __builtin_aarch64_ld4v8bf ((const __builtin_aarch64_simd_bf *) __a);
}
__extension__ extern __inline bfloat16x4x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4_dup_bf16 (const bfloat16_t * __a)
{
  return __builtin_aarch64_ld4rv4bf ((const __builtin_aarch64_simd_bf *) __a);
}
__extension__ extern __inline bfloat16x8x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vld4q_dup_bf16 (const bfloat16_t * __a)
{
  return __builtin_aarch64_ld4rv8bf ((const __builtin_aarch64_simd_bf *) __a);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_bf16 (bfloat16_t *__a, bfloat16x4_t __b)
{
  __builtin_aarch64_st1v4bf (__a, __b);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_bf16_x2 (bfloat16_t * __a, bfloat16x4x2_t __val)
{
  __builtin_aarch64_st1x2v4bf (__a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_bf16_x2 (bfloat16_t * __a, bfloat16x8x2_t __val)
{
  __builtin_aarch64_st1x2v8bf (__a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_bf16_x3 (bfloat16_t * __a, bfloat16x4x3_t __val)
{
  __builtin_aarch64_st1x3v4bf ((__builtin_aarch64_simd_bf *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_bf16_x3 (bfloat16_t * __a, bfloat16x8x3_t __val)
{
  __builtin_aarch64_st1x3v8bf ((__builtin_aarch64_simd_bf *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_bf16_x4 (bfloat16_t * __a, bfloat16x4x4_t __val)
{
  __builtin_aarch64_st1x4v4bf ((__builtin_aarch64_simd_bf *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_bf16_x4 (bfloat16_t * __a, bfloat16x8x4_t __val)
{
  __builtin_aarch64_st1x4v8bf ((__builtin_aarch64_simd_bf *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_bf16 (bfloat16_t *__a, bfloat16x8_t __b)
{
  __builtin_aarch64_st1v8bf (__a, __b);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1_lane_bf16 (bfloat16_t *__a, bfloat16x4_t __b, const int __lane)
{
  *__a = __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst1q_lane_bf16 (bfloat16_t *__a, bfloat16x8_t __b, const int __lane)
{
  *__a = __aarch64_vget_lane_any (__b, __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2_bf16 (bfloat16_t * __a, bfloat16x4x2_t __val)
{
  __builtin_aarch64_st2v4bf (__a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2q_bf16 (bfloat16_t * __a, bfloat16x8x2_t __val)
{
  __builtin_aarch64_st2v8bf (__a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3_bf16 (bfloat16_t * __a, bfloat16x4x3_t __val)
{
  __builtin_aarch64_st3v4bf ((__builtin_aarch64_simd_bf *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3q_bf16 (bfloat16_t * __a, bfloat16x8x3_t __val)
{
  __builtin_aarch64_st3v8bf ((__builtin_aarch64_simd_bf *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4_bf16 (bfloat16_t * __a, bfloat16x4x4_t __val)
{
  __builtin_aarch64_st4v4bf ((__builtin_aarch64_simd_bf *) __a, __val);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4q_bf16 (bfloat16_t * __a, bfloat16x8x4_t __val)
{
  __builtin_aarch64_st4v8bf ((__builtin_aarch64_simd_bf *) __a, __val);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbfdot_f32 (float32x2_t __r, bfloat16x4_t __a, bfloat16x4_t __b)
{
  return __builtin_aarch64_bfdotv2sf (__r, __a, __b);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbfdotq_f32 (float32x4_t __r, bfloat16x8_t __a, bfloat16x8_t __b)
{
  return __builtin_aarch64_bfdotv4sf (__r, __a, __b);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbfdot_lane_f32 (float32x2_t __r, bfloat16x4_t __a, bfloat16x4_t __b,
   const int __index)
{
  return __builtin_aarch64_bfdot_lanev2sf (__r, __a, __b, __index);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbfdotq_lane_f32 (float32x4_t __r, bfloat16x8_t __a, bfloat16x4_t __b,
    const int __index)
{
  return __builtin_aarch64_bfdot_lanev4sf (__r, __a, __b, __index);
}
__extension__ extern __inline float32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbfdot_laneq_f32 (float32x2_t __r, bfloat16x4_t __a, bfloat16x8_t __b,
    const int __index)
{
  return __builtin_aarch64_bfdot_laneqv2sf (__r, __a, __b, __index);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbfdotq_laneq_f32 (float32x4_t __r, bfloat16x8_t __a, bfloat16x8_t __b,
     const int __index)
{
  return __builtin_aarch64_bfdot_laneqv4sf (__r, __a, __b, __index);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbfmmlaq_f32 (float32x4_t __r, bfloat16x8_t __a, bfloat16x8_t __b)
{
  return __builtin_aarch64_bfmmlaqv4sf (__r, __a, __b);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbfmlalbq_f32 (float32x4_t __r, bfloat16x8_t __a, bfloat16x8_t __b)
{
  return __builtin_aarch64_bfmlalbv4sf (__r, __a, __b);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbfmlaltq_f32 (float32x4_t __r, bfloat16x8_t __a, bfloat16x8_t __b)
{
  return __builtin_aarch64_bfmlaltv4sf (__r, __a, __b);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbfmlalbq_lane_f32 (float32x4_t __r, bfloat16x8_t __a, bfloat16x4_t __b,
      const int __index)
{
  return __builtin_aarch64_bfmlalb_lanev4sf (__r, __a, __b, __index);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbfmlaltq_lane_f32 (float32x4_t __r, bfloat16x8_t __a, bfloat16x4_t __b,
      const int __index)
{
  return __builtin_aarch64_bfmlalt_lanev4sf (__r, __a, __b, __index);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbfmlalbq_laneq_f32 (float32x4_t __r, bfloat16x8_t __a, bfloat16x8_t __b,
       const int __index)
{
  return __builtin_aarch64_bfmlalb_lane_qv4sf (__r, __a, __b, __index);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vbfmlaltq_laneq_f32 (float32x4_t __r, bfloat16x8_t __a, bfloat16x8_t __b,
       const int __index)
{
  return __builtin_aarch64_bfmlalt_lane_qv4sf (__r, __a, __b, __index);
}
__extension__ extern __inline bfloat16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_low_bf16 (bfloat16x8_t __a)
{
  return __builtin_aarch64_vget_lo_halfv8bf (__a);
}
__extension__ extern __inline bfloat16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vget_high_bf16 (bfloat16x8_t __a)
{
  return __builtin_aarch64_vget_hi_halfv8bf (__a);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvt_f32_bf16 (bfloat16x4_t __a)
{
  return __builtin_aarch64_vbfcvtv4bf (__a);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtq_low_f32_bf16 (bfloat16x8_t __a)
{
  return __builtin_aarch64_vbfcvtv8bf (__a);
}
__extension__ extern __inline float32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtq_high_f32_bf16 (bfloat16x8_t __a)
{
  return __builtin_aarch64_vbfcvt_highv8bf (__a);
}
__extension__ extern __inline bfloat16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvt_bf16_f32 (float32x4_t __a)
{
  return __builtin_aarch64_bfcvtnv4bf (__a);
}
__extension__ extern __inline bfloat16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtq_low_bf16_f32 (float32x4_t __a)
{
  return __builtin_aarch64_bfcvtn_qv8bf (__a);
}
__extension__ extern __inline bfloat16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtq_high_bf16_f32 (bfloat16x8_t __inactive, float32x4_t __a)
{
  return __builtin_aarch64_bfcvtn2v8bf (__inactive, __a);
}
__extension__ extern __inline bfloat16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopy_lane_bf16 (bfloat16x4_t __a, const int __lane1,
   bfloat16x4_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
      __a, __lane1);
}
__extension__ extern __inline bfloat16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopyq_lane_bf16 (bfloat16x8_t __a, const int __lane1,
    bfloat16x4_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
      __a, __lane1);
}
__extension__ extern __inline bfloat16x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopy_laneq_bf16 (bfloat16x4_t __a, const int __lane1,
    bfloat16x8_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
      __a, __lane1);
}
__extension__ extern __inline bfloat16x8_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcopyq_laneq_bf16 (bfloat16x8_t __a, const int __lane1,
     bfloat16x8_t __b, const int __lane2)
{
  return __aarch64_vset_lane_any (__aarch64_vget_lane_any (__b, __lane2),
      __a, __lane1);
}
__extension__ extern __inline bfloat16x4x2_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld2_lane_bf16 (const bfloat16_t * __ptr, bfloat16x4x2_t __b, const int __c)
{
  return __builtin_aarch64_ld2_lanev4bf (
   (__builtin_aarch64_simd_bf *) __ptr, __b, __c);
}
__extension__ extern __inline bfloat16x8x2_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld2q_lane_bf16 (const bfloat16_t * __ptr, bfloat16x8x2_t __b, const int __c)
{
  return __builtin_aarch64_ld2_lanev8bf (
   (__builtin_aarch64_simd_bf *) __ptr, __b, __c);
}
__extension__ extern __inline bfloat16x4x3_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld3_lane_bf16 (const bfloat16_t * __ptr, bfloat16x4x3_t __b, const int __c)
{
  return __builtin_aarch64_ld3_lanev4bf (
   (__builtin_aarch64_simd_bf *) __ptr, __b, __c);
}
__extension__ extern __inline bfloat16x8x3_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld3q_lane_bf16 (const bfloat16_t * __ptr, bfloat16x8x3_t __b, const int __c)
{
  return __builtin_aarch64_ld3_lanev8bf (
   (__builtin_aarch64_simd_bf *) __ptr, __b, __c);
}
__extension__ extern __inline bfloat16x4x4_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld4_lane_bf16 (const bfloat16_t * __ptr, bfloat16x4x4_t __b, const int __c)
{
  return __builtin_aarch64_ld4_lanev4bf (
   (__builtin_aarch64_simd_bf *) __ptr, __b, __c);
}
__extension__ extern __inline bfloat16x8x4_t
__attribute__ ((__always_inline__, __gnu_inline__,__artificial__))
vld4q_lane_bf16 (const bfloat16_t * __ptr, bfloat16x8x4_t __b, const int __c)
{
  return __builtin_aarch64_ld4_lanev8bf (
   (__builtin_aarch64_simd_bf *) __ptr, __b, __c);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2_lane_bf16 (bfloat16_t *__ptr, bfloat16x4x2_t __val, const int __lane)
{
  __builtin_aarch64_st2_lanev4bf ((__builtin_aarch64_simd_bf *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst2q_lane_bf16 (bfloat16_t *__ptr, bfloat16x8x2_t __val, const int __lane)
{
  __builtin_aarch64_st2_lanev8bf ((__builtin_aarch64_simd_bf *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3_lane_bf16 (bfloat16_t *__ptr, bfloat16x4x3_t __val, const int __lane)
{
  __builtin_aarch64_st3_lanev4bf ((__builtin_aarch64_simd_bf *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst3q_lane_bf16 (bfloat16_t *__ptr, bfloat16x8x3_t __val, const int __lane)
{
  __builtin_aarch64_st3_lanev8bf ((__builtin_aarch64_simd_bf *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4_lane_bf16 (bfloat16_t *__ptr, bfloat16x4x4_t __val, const int __lane)
{
  __builtin_aarch64_st4_lanev4bf ((__builtin_aarch64_simd_bf *) __ptr, __val,
      __lane);
}
__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vst4q_lane_bf16 (bfloat16_t *__ptr, bfloat16x8x4_t __val, const int __lane)
{
  __builtin_aarch64_st4_lanev8bf ((__builtin_aarch64_simd_bf *) __ptr, __val,
      __lane);
}
#pragma GCC pop_options
#pragma GCC push_options
#pragma GCC target ("+nothing+i8mm")
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vusdot_s32 (int32x2_t __r, uint8x8_t __a, int8x8_t __b)
{
  return __builtin_aarch64_usdot_prodv8qi_suss (__a, __b, __r);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vusdotq_s32 (int32x4_t __r, uint8x16_t __a, int8x16_t __b)
{
  return __builtin_aarch64_usdot_prodv16qi_suss (__a, __b, __r);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vusdot_lane_s32 (int32x2_t __r, uint8x8_t __a, int8x8_t __b, const int __index)
{
  return __builtin_aarch64_usdot_lanev8qi_ssuss (__r, __a, __b, __index);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vusdot_laneq_s32 (int32x2_t __r, uint8x8_t __a, int8x16_t __b,
    const int __index)
{
  return __builtin_aarch64_usdot_laneqv8qi_ssuss (__r, __a, __b, __index);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vusdotq_lane_s32 (int32x4_t __r, uint8x16_t __a, int8x8_t __b,
    const int __index)
{
  return __builtin_aarch64_usdot_lanev16qi_ssuss (__r, __a, __b, __index);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vusdotq_laneq_s32 (int32x4_t __r, uint8x16_t __a, int8x16_t __b,
     const int __index)
{
  return __builtin_aarch64_usdot_laneqv16qi_ssuss (__r, __a, __b, __index);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsudot_lane_s32 (int32x2_t __r, int8x8_t __a, uint8x8_t __b, const int __index)
{
  return __builtin_aarch64_sudot_lanev8qi_sssus (__r, __a, __b, __index);
}
__extension__ extern __inline int32x2_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsudot_laneq_s32 (int32x2_t __r, int8x8_t __a, uint8x16_t __b,
    const int __index)
{
  return __builtin_aarch64_sudot_laneqv8qi_sssus (__r, __a, __b, __index);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsudotq_lane_s32 (int32x4_t __r, int8x16_t __a, uint8x8_t __b,
    const int __index)
{
  return __builtin_aarch64_sudot_lanev16qi_sssus (__r, __a, __b, __index);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vsudotq_laneq_s32 (int32x4_t __r, int8x16_t __a, uint8x16_t __b,
     const int __index)
{
  return __builtin_aarch64_sudot_laneqv16qi_sssus (__r, __a, __b, __index);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmmlaq_s32 (int32x4_t __r, int8x16_t __a, int8x16_t __b)
{
  return __builtin_aarch64_simd_smmlav16qi (__r, __a, __b);
}
__extension__ extern __inline uint32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vmmlaq_u32 (uint32x4_t __r, uint8x16_t __a, uint8x16_t __b)
{
  return __builtin_aarch64_simd_ummlav16qi_uuuu (__r, __a, __b);
}
__extension__ extern __inline int32x4_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vusmmlaq_s32 (int32x4_t __r, uint8x16_t __a, int8x16_t __b)
{
  return __builtin_aarch64_simd_usmmlav16qi_ssus (__r, __a, __b);
}
#pragma GCC pop_options
__extension__ extern __inline poly8x8_t
__attribute ((__always_inline__, __gnu_inline__, __artificial__))
vadd_p8 (poly8x8_t __a, poly8x8_t __b)
{
  return __a ^ __b;
}
__extension__ extern __inline poly16x4_t
__attribute ((__always_inline__, __gnu_inline__, __artificial__))
vadd_p16 (poly16x4_t __a, poly16x4_t __b)
{
  return __a ^ __b;
}
__extension__ extern __inline poly64x1_t
__attribute ((__always_inline__, __gnu_inline__, __artificial__))
vadd_p64 (poly64x1_t __a, poly64x1_t __b)
{
  return __a ^ __b;
}
__extension__ extern __inline poly8x16_t
__attribute ((__always_inline__, __gnu_inline__, __artificial__))
vaddq_p8 (poly8x16_t __a, poly8x16_t __b)
{
  return __a ^ __b;
}
__extension__ extern __inline poly16x8_t
__attribute ((__always_inline__, __gnu_inline__, __artificial__))
vaddq_p16 (poly16x8_t __a, poly16x8_t __b)
{
  return __a ^__b;
}
__extension__ extern __inline poly64x2_t
__attribute ((__always_inline__, __gnu_inline__, __artificial__))
vaddq_p64 (poly64x2_t __a, poly64x2_t __b)
{
  return __a ^ __b;
}
__extension__ extern __inline poly128_t
__attribute ((__always_inline__, __gnu_inline__, __artificial__))
vaddq_p128 (poly128_t __a, poly128_t __b)
{
  return __a ^ __b;
}
#undef __aarch64_vget_lane_any
#undef __aarch64_vdup_lane_any
#undef __aarch64_vdup_lane_f16
#undef __aarch64_vdup_lane_f32
#undef __aarch64_vdup_lane_f64
#undef __aarch64_vdup_lane_p8
#undef __aarch64_vdup_lane_p16
#undef __aarch64_vdup_lane_s8
#undef __aarch64_vdup_lane_s16
#undef __aarch64_vdup_lane_s32
#undef __aarch64_vdup_lane_s64
#undef __aarch64_vdup_lane_u8
#undef __aarch64_vdup_lane_u16
#undef __aarch64_vdup_lane_u32
#undef __aarch64_vdup_lane_u64
#undef __aarch64_vdup_laneq_f16
#undef __aarch64_vdup_laneq_f32
#undef __aarch64_vdup_laneq_f64
#undef __aarch64_vdup_laneq_p8
#undef __aarch64_vdup_laneq_p16
#undef __aarch64_vdup_laneq_s8
#undef __aarch64_vdup_laneq_s16
#undef __aarch64_vdup_laneq_s32
#undef __aarch64_vdup_laneq_s64
#undef __aarch64_vdup_laneq_u8
#undef __aarch64_vdup_laneq_u16
#undef __aarch64_vdup_laneq_u32
#undef __aarch64_vdup_laneq_u64
#undef __aarch64_vdupq_lane_f16
#undef __aarch64_vdupq_lane_f32
#undef __aarch64_vdupq_lane_f64
#undef __aarch64_vdupq_lane_p8
#undef __aarch64_vdupq_lane_p16
#undef __aarch64_vdupq_lane_s8
#undef __aarch64_vdupq_lane_s16
#undef __aarch64_vdupq_lane_s32
#undef __aarch64_vdupq_lane_s64
#undef __aarch64_vdupq_lane_u8
#undef __aarch64_vdupq_lane_u16
#undef __aarch64_vdupq_lane_u32
#undef __aarch64_vdupq_lane_u64
#undef __aarch64_vdupq_laneq_f16
#undef __aarch64_vdupq_laneq_f32
#undef __aarch64_vdupq_laneq_f64
#undef __aarch64_vdupq_laneq_p8
#undef __aarch64_vdupq_laneq_p16
#undef __aarch64_vdupq_laneq_s8
#undef __aarch64_vdupq_laneq_s16
#undef __aarch64_vdupq_laneq_s32
#undef __aarch64_vdupq_laneq_s64
#undef __aarch64_vdupq_laneq_u8
#undef __aarch64_vdupq_laneq_u16
#undef __aarch64_vdupq_laneq_u32
#undef __aarch64_vdupq_laneq_u64
#endif
#endif
