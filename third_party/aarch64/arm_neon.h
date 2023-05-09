#ifndef _AARCH64_NEON_H_
#define _AARCH64_NEON_H_
#ifdef __aarch64__
#include "libc/inttypes.h"
#include "libc/limits.h"
#include "libc/literal.h"

#pragma GCC push_options
#pragma GCC diagnostic ignored "-Wmissing-braces"

#pragma GCC push_options
#pragma GCC target("+nothing+simd")

#define __AARCH64_UINT64_C(__C) ((uint64_t)__C)
#define __AARCH64_INT64_C(__C)  ((int64_t)__C)

#define FUNK(T)                   \
  __extension__ extern __inline T \
      __attribute__((__always_inline__, __gnu_inline__, __artificial__))

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

typedef struct int8x8x2_t {
  int8x8_t val[2];
} int8x8x2_t;

typedef struct int8x16x2_t {
  int8x16_t val[2];
} int8x16x2_t;

typedef struct int16x4x2_t {
  int16x4_t val[2];
} int16x4x2_t;

typedef struct int16x8x2_t {
  int16x8_t val[2];
} int16x8x2_t;

typedef struct int32x2x2_t {
  int32x2_t val[2];
} int32x2x2_t;

typedef struct int32x4x2_t {
  int32x4_t val[2];
} int32x4x2_t;

typedef struct int64x1x2_t {
  int64x1_t val[2];
} int64x1x2_t;

typedef struct int64x2x2_t {
  int64x2_t val[2];
} int64x2x2_t;

typedef struct uint8x8x2_t {
  uint8x8_t val[2];
} uint8x8x2_t;

typedef struct uint8x16x2_t {
  uint8x16_t val[2];
} uint8x16x2_t;

typedef struct uint16x4x2_t {
  uint16x4_t val[2];
} uint16x4x2_t;

typedef struct uint16x8x2_t {
  uint16x8_t val[2];
} uint16x8x2_t;

typedef struct uint32x2x2_t {
  uint32x2_t val[2];
} uint32x2x2_t;

typedef struct uint32x4x2_t {
  uint32x4_t val[2];
} uint32x4x2_t;

typedef struct uint64x1x2_t {
  uint64x1_t val[2];
} uint64x1x2_t;

typedef struct uint64x2x2_t {
  uint64x2_t val[2];
} uint64x2x2_t;

typedef struct float16x4x2_t {
  float16x4_t val[2];
} float16x4x2_t;

typedef struct float16x8x2_t {
  float16x8_t val[2];
} float16x8x2_t;

typedef struct float32x2x2_t {
  float32x2_t val[2];
} float32x2x2_t;

typedef struct float32x4x2_t {
  float32x4_t val[2];
} float32x4x2_t;

typedef struct float64x2x2_t {
  float64x2_t val[2];
} float64x2x2_t;

typedef struct float64x1x2_t {
  float64x1_t val[2];
} float64x1x2_t;

typedef struct poly8x8x2_t {
  poly8x8_t val[2];
} poly8x8x2_t;

typedef struct poly8x16x2_t {
  poly8x16_t val[2];
} poly8x16x2_t;

typedef struct poly16x4x2_t {
  poly16x4_t val[2];
} poly16x4x2_t;

typedef struct poly16x8x2_t {
  poly16x8_t val[2];
} poly16x8x2_t;

typedef struct poly64x1x2_t {
  poly64x1_t val[2];
} poly64x1x2_t;

typedef struct poly64x1x3_t {
  poly64x1_t val[3];
} poly64x1x3_t;

typedef struct poly64x1x4_t {
  poly64x1_t val[4];
} poly64x1x4_t;

typedef struct poly64x2x2_t {
  poly64x2_t val[2];
} poly64x2x2_t;

typedef struct poly64x2x3_t {
  poly64x2_t val[3];
} poly64x2x3_t;

typedef struct poly64x2x4_t {
  poly64x2_t val[4];
} poly64x2x4_t;

typedef struct int8x8x3_t {
  int8x8_t val[3];
} int8x8x3_t;

typedef struct int8x16x3_t {
  int8x16_t val[3];
} int8x16x3_t;

typedef struct int16x4x3_t {
  int16x4_t val[3];
} int16x4x3_t;

typedef struct int16x8x3_t {
  int16x8_t val[3];
} int16x8x3_t;

typedef struct int32x2x3_t {
  int32x2_t val[3];
} int32x2x3_t;

typedef struct int32x4x3_t {
  int32x4_t val[3];
} int32x4x3_t;

typedef struct int64x1x3_t {
  int64x1_t val[3];
} int64x1x3_t;

typedef struct int64x2x3_t {
  int64x2_t val[3];
} int64x2x3_t;

typedef struct uint8x8x3_t {
  uint8x8_t val[3];
} uint8x8x3_t;

typedef struct uint8x16x3_t {
  uint8x16_t val[3];
} uint8x16x3_t;

typedef struct uint16x4x3_t {
  uint16x4_t val[3];
} uint16x4x3_t;

typedef struct uint16x8x3_t {
  uint16x8_t val[3];
} uint16x8x3_t;

typedef struct uint32x2x3_t {
  uint32x2_t val[3];
} uint32x2x3_t;

typedef struct uint32x4x3_t {
  uint32x4_t val[3];
} uint32x4x3_t;

typedef struct uint64x1x3_t {
  uint64x1_t val[3];
} uint64x1x3_t;

typedef struct uint64x2x3_t {
  uint64x2_t val[3];
} uint64x2x3_t;

typedef struct float16x4x3_t {
  float16x4_t val[3];
} float16x4x3_t;

typedef struct float16x8x3_t {
  float16x8_t val[3];
} float16x8x3_t;

typedef struct float32x2x3_t {
  float32x2_t val[3];
} float32x2x3_t;

typedef struct float32x4x3_t {
  float32x4_t val[3];
} float32x4x3_t;

typedef struct float64x2x3_t {
  float64x2_t val[3];
} float64x2x3_t;

typedef struct float64x1x3_t {
  float64x1_t val[3];
} float64x1x3_t;

typedef struct poly8x8x3_t {
  poly8x8_t val[3];
} poly8x8x3_t;

typedef struct poly8x16x3_t {
  poly8x16_t val[3];
} poly8x16x3_t;

typedef struct poly16x4x3_t {
  poly16x4_t val[3];
} poly16x4x3_t;

typedef struct poly16x8x3_t {
  poly16x8_t val[3];
} poly16x8x3_t;

typedef struct int8x8x4_t {
  int8x8_t val[4];
} int8x8x4_t;

typedef struct int8x16x4_t {
  int8x16_t val[4];
} int8x16x4_t;

typedef struct int16x4x4_t {
  int16x4_t val[4];
} int16x4x4_t;

typedef struct int16x8x4_t {
  int16x8_t val[4];
} int16x8x4_t;

typedef struct int32x2x4_t {
  int32x2_t val[4];
} int32x2x4_t;

typedef struct int32x4x4_t {
  int32x4_t val[4];
} int32x4x4_t;

typedef struct int64x1x4_t {
  int64x1_t val[4];
} int64x1x4_t;

typedef struct int64x2x4_t {
  int64x2_t val[4];
} int64x2x4_t;

typedef struct uint8x8x4_t {
  uint8x8_t val[4];
} uint8x8x4_t;

typedef struct uint8x16x4_t {
  uint8x16_t val[4];
} uint8x16x4_t;

typedef struct uint16x4x4_t {
  uint16x4_t val[4];
} uint16x4x4_t;

typedef struct uint16x8x4_t {
  uint16x8_t val[4];
} uint16x8x4_t;

typedef struct uint32x2x4_t {
  uint32x2_t val[4];
} uint32x2x4_t;

typedef struct uint32x4x4_t {
  uint32x4_t val[4];
} uint32x4x4_t;

typedef struct uint64x1x4_t {
  uint64x1_t val[4];
} uint64x1x4_t;

typedef struct uint64x2x4_t {
  uint64x2_t val[4];
} uint64x2x4_t;

typedef struct float16x4x4_t {
  float16x4_t val[4];
} float16x4x4_t;

typedef struct float16x8x4_t {
  float16x8_t val[4];
} float16x8x4_t;

typedef struct float32x2x4_t {
  float32x2_t val[4];
} float32x2x4_t;

typedef struct float32x4x4_t {
  float32x4_t val[4];
} float32x4x4_t;

typedef struct float64x2x4_t {
  float64x2_t val[4];
} float64x2x4_t;

typedef struct float64x1x4_t {
  float64x1_t val[4];
} float64x1x4_t;

typedef struct poly8x8x4_t {
  poly8x8_t val[4];
} poly8x8x4_t;

typedef struct poly8x16x4_t {
  poly8x16_t val[4];
} poly8x16x4_t;

typedef struct poly16x4x4_t {
  poly16x4_t val[4];
} poly16x4x4_t;

typedef struct poly16x8x4_t {
  poly16x8_t val[4];
} poly16x8x4_t;

#define __aarch64_vdup_lane_any(__size, __q, __a, __b) \
  vdup##__q##_n_##__size(__aarch64_vget_lane_any(__a, __b))

#define __aarch64_vdup_lane_f16(__a, __b) \
  __aarch64_vdup_lane_any(f16, , __a, __b)
#define __aarch64_vdup_lane_f32(__a, __b) \
  __aarch64_vdup_lane_any(f32, , __a, __b)
#define __aarch64_vdup_lane_f64(__a, __b) \
  __aarch64_vdup_lane_any(f64, , __a, __b)
#define __aarch64_vdup_lane_p8(__a, __b) __aarch64_vdup_lane_any(p8, , __a, __b)
#define __aarch64_vdup_lane_p16(__a, __b) \
  __aarch64_vdup_lane_any(p16, , __a, __b)
#define __aarch64_vdup_lane_p64(__a, __b) \
  __aarch64_vdup_lane_any(p64, , __a, __b)
#define __aarch64_vdup_lane_s8(__a, __b) __aarch64_vdup_lane_any(s8, , __a, __b)
#define __aarch64_vdup_lane_s16(__a, __b) \
  __aarch64_vdup_lane_any(s16, , __a, __b)
#define __aarch64_vdup_lane_s32(__a, __b) \
  __aarch64_vdup_lane_any(s32, , __a, __b)
#define __aarch64_vdup_lane_s64(__a, __b) \
  __aarch64_vdup_lane_any(s64, , __a, __b)
#define __aarch64_vdup_lane_u8(__a, __b) __aarch64_vdup_lane_any(u8, , __a, __b)
#define __aarch64_vdup_lane_u16(__a, __b) \
  __aarch64_vdup_lane_any(u16, , __a, __b)
#define __aarch64_vdup_lane_u32(__a, __b) \
  __aarch64_vdup_lane_any(u32, , __a, __b)
#define __aarch64_vdup_lane_u64(__a, __b) \
  __aarch64_vdup_lane_any(u64, , __a, __b)

#define __aarch64_vdup_laneq_f16(__a, __b) \
  __aarch64_vdup_lane_any(f16, , __a, __b)
#define __aarch64_vdup_laneq_f32(__a, __b) \
  __aarch64_vdup_lane_any(f32, , __a, __b)
#define __aarch64_vdup_laneq_f64(__a, __b) \
  __aarch64_vdup_lane_any(f64, , __a, __b)
#define __aarch64_vdup_laneq_p8(__a, __b) \
  __aarch64_vdup_lane_any(p8, , __a, __b)
#define __aarch64_vdup_laneq_p16(__a, __b) \
  __aarch64_vdup_lane_any(p16, , __a, __b)
#define __aarch64_vdup_laneq_p64(__a, __b) \
  __aarch64_vdup_lane_any(p64, , __a, __b)
#define __aarch64_vdup_laneq_s8(__a, __b) \
  __aarch64_vdup_lane_any(s8, , __a, __b)
#define __aarch64_vdup_laneq_s16(__a, __b) \
  __aarch64_vdup_lane_any(s16, , __a, __b)
#define __aarch64_vdup_laneq_s32(__a, __b) \
  __aarch64_vdup_lane_any(s32, , __a, __b)
#define __aarch64_vdup_laneq_s64(__a, __b) \
  __aarch64_vdup_lane_any(s64, , __a, __b)
#define __aarch64_vdup_laneq_u8(__a, __b) \
  __aarch64_vdup_lane_any(u8, , __a, __b)
#define __aarch64_vdup_laneq_u16(__a, __b) \
  __aarch64_vdup_lane_any(u16, , __a, __b)
#define __aarch64_vdup_laneq_u32(__a, __b) \
  __aarch64_vdup_lane_any(u32, , __a, __b)
#define __aarch64_vdup_laneq_u64(__a, __b) \
  __aarch64_vdup_lane_any(u64, , __a, __b)

#define __aarch64_vdupq_lane_f16(__a, __b) \
  __aarch64_vdup_lane_any(f16, q, __a, __b)
#define __aarch64_vdupq_lane_f32(__a, __b) \
  __aarch64_vdup_lane_any(f32, q, __a, __b)
#define __aarch64_vdupq_lane_f64(__a, __b) \
  __aarch64_vdup_lane_any(f64, q, __a, __b)
#define __aarch64_vdupq_lane_p8(__a, __b) \
  __aarch64_vdup_lane_any(p8, q, __a, __b)
#define __aarch64_vdupq_lane_p16(__a, __b) \
  __aarch64_vdup_lane_any(p16, q, __a, __b)
#define __aarch64_vdupq_lane_p64(__a, __b) \
  __aarch64_vdup_lane_any(p64, q, __a, __b)
#define __aarch64_vdupq_lane_s8(__a, __b) \
  __aarch64_vdup_lane_any(s8, q, __a, __b)
#define __aarch64_vdupq_lane_s16(__a, __b) \
  __aarch64_vdup_lane_any(s16, q, __a, __b)
#define __aarch64_vdupq_lane_s32(__a, __b) \
  __aarch64_vdup_lane_any(s32, q, __a, __b)
#define __aarch64_vdupq_lane_s64(__a, __b) \
  __aarch64_vdup_lane_any(s64, q, __a, __b)
#define __aarch64_vdupq_lane_u8(__a, __b) \
  __aarch64_vdup_lane_any(u8, q, __a, __b)
#define __aarch64_vdupq_lane_u16(__a, __b) \
  __aarch64_vdup_lane_any(u16, q, __a, __b)
#define __aarch64_vdupq_lane_u32(__a, __b) \
  __aarch64_vdup_lane_any(u32, q, __a, __b)
#define __aarch64_vdupq_lane_u64(__a, __b) \
  __aarch64_vdup_lane_any(u64, q, __a, __b)

#define __aarch64_vdupq_laneq_f16(__a, __b) \
  __aarch64_vdup_lane_any(f16, q, __a, __b)
#define __aarch64_vdupq_laneq_f32(__a, __b) \
  __aarch64_vdup_lane_any(f32, q, __a, __b)
#define __aarch64_vdupq_laneq_f64(__a, __b) \
  __aarch64_vdup_lane_any(f64, q, __a, __b)
#define __aarch64_vdupq_laneq_p8(__a, __b) \
  __aarch64_vdup_lane_any(p8, q, __a, __b)
#define __aarch64_vdupq_laneq_p16(__a, __b) \
  __aarch64_vdup_lane_any(p16, q, __a, __b)
#define __aarch64_vdupq_laneq_p64(__a, __b) \
  __aarch64_vdup_lane_any(p64, q, __a, __b)
#define __aarch64_vdupq_laneq_s8(__a, __b) \
  __aarch64_vdup_lane_any(s8, q, __a, __b)
#define __aarch64_vdupq_laneq_s16(__a, __b) \
  __aarch64_vdup_lane_any(s16, q, __a, __b)
#define __aarch64_vdupq_laneq_s32(__a, __b) \
  __aarch64_vdup_lane_any(s32, q, __a, __b)
#define __aarch64_vdupq_laneq_s64(__a, __b) \
  __aarch64_vdup_lane_any(s64, q, __a, __b)
#define __aarch64_vdupq_laneq_u8(__a, __b) \
  __aarch64_vdup_lane_any(u8, q, __a, __b)
#define __aarch64_vdupq_laneq_u16(__a, __b) \
  __aarch64_vdup_lane_any(u16, q, __a, __b)
#define __aarch64_vdupq_laneq_u32(__a, __b) \
  __aarch64_vdup_lane_any(u32, q, __a, __b)
#define __aarch64_vdupq_laneq_u64(__a, __b) \
  __aarch64_vdup_lane_any(u64, q, __a, __b)

#define __AARCH64_NUM_LANES(__v) (sizeof(__v) / sizeof(__v[0]))
#define __AARCH64_LANE_CHECK(__vec, __idx) \
  __builtin_aarch64_im_lane_boundsi(sizeof(__vec), sizeof(__vec[0]), __idx)

#ifdef __AARCH64EB__
#define __aarch64_lane(__vec, __idx) (__AARCH64_NUM_LANES(__vec) - 1 - __idx)
#else
#define __aarch64_lane(__vec, __idx) __idx
#endif

#define __aarch64_vget_lane_any(__vec, __index) \
  __extension__({                               \
    __AARCH64_LANE_CHECK(__vec, __index);       \
    __vec[__aarch64_lane(__vec, __index)];      \
  })

#define __aarch64_vset_lane_any(__elem, __vec, __index) \
  __extension__({                                       \
    __AARCH64_LANE_CHECK(__vec, __index);               \
    __vec[__aarch64_lane(__vec, __index)] = __elem;     \
    __vec;                                              \
  })

FUNK(int8x8_t)
vadd_s8(int8x8_t __a, int8x8_t __b) {
  return __a + __b;
}

FUNK(int16x4_t)
vadd_s16(int16x4_t __a, int16x4_t __b) {
  return __a + __b;
}

FUNK(int32x2_t)
vadd_s32(int32x2_t __a, int32x2_t __b) {
  return __a + __b;
}

FUNK(float32x2_t)
vadd_f32(float32x2_t __a, float32x2_t __b) {
  return __a + __b;
}

FUNK(float64x1_t)
vadd_f64(float64x1_t __a, float64x1_t __b) {
  return __a + __b;
}

FUNK(uint8x8_t)
vadd_u8(uint8x8_t __a, uint8x8_t __b) {
  return __a + __b;
}

FUNK(uint16x4_t)
vadd_u16(uint16x4_t __a, uint16x4_t __b) {
  return __a + __b;
}

FUNK(uint32x2_t)
vadd_u32(uint32x2_t __a, uint32x2_t __b) {
  return __a + __b;
}

FUNK(int64x1_t)
vadd_s64(int64x1_t __a, int64x1_t __b) {
  return __a + __b;
}

FUNK(uint64x1_t)
vadd_u64(uint64x1_t __a, uint64x1_t __b) {
  return __a + __b;
}

FUNK(int8x16_t)
vaddq_s8(int8x16_t __a, int8x16_t __b) {
  return __a + __b;
}

FUNK(int16x8_t)
vaddq_s16(int16x8_t __a, int16x8_t __b) {
  return __a + __b;
}

FUNK(int32x4_t)
vaddq_s32(int32x4_t __a, int32x4_t __b) {
  return __a + __b;
}

FUNK(int64x2_t)
vaddq_s64(int64x2_t __a, int64x2_t __b) {
  return __a + __b;
}

FUNK(float32x4_t)
vaddq_f32(float32x4_t __a, float32x4_t __b) {
  return __a + __b;
}

FUNK(float64x2_t)
vaddq_f64(float64x2_t __a, float64x2_t __b) {
  return __a + __b;
}

FUNK(uint8x16_t)
vaddq_u8(uint8x16_t __a, uint8x16_t __b) {
  return __a + __b;
}

FUNK(uint16x8_t)
vaddq_u16(uint16x8_t __a, uint16x8_t __b) {
  return __a + __b;
}

FUNK(uint32x4_t)
vaddq_u32(uint32x4_t __a, uint32x4_t __b) {
  return __a + __b;
}

FUNK(uint64x2_t)
vaddq_u64(uint64x2_t __a, uint64x2_t __b) {
  return __a + __b;
}

FUNK(int16x8_t)
vaddl_s8(int8x8_t __a, int8x8_t __b) {
  return (int16x8_t)__builtin_aarch64_saddlv8qi(__a, __b);
}

FUNK(int32x4_t)
vaddl_s16(int16x4_t __a, int16x4_t __b) {
  return (int32x4_t)__builtin_aarch64_saddlv4hi(__a, __b);
}

FUNK(int64x2_t)
vaddl_s32(int32x2_t __a, int32x2_t __b) {
  return (int64x2_t)__builtin_aarch64_saddlv2si(__a, __b);
}

FUNK(uint16x8_t)
vaddl_u8(uint8x8_t __a, uint8x8_t __b) {
  return (uint16x8_t)__builtin_aarch64_uaddlv8qi((int8x8_t)__a, (int8x8_t)__b);
}

FUNK(uint32x4_t)
vaddl_u16(uint16x4_t __a, uint16x4_t __b) {
  return (uint32x4_t)__builtin_aarch64_uaddlv4hi((int16x4_t)__a,
                                                 (int16x4_t)__b);
}

FUNK(uint64x2_t)
vaddl_u32(uint32x2_t __a, uint32x2_t __b) {
  return (uint64x2_t)__builtin_aarch64_uaddlv2si((int32x2_t)__a,
                                                 (int32x2_t)__b);
}

FUNK(int16x8_t)
vaddl_high_s8(int8x16_t __a, int8x16_t __b) {
  return (int16x8_t)__builtin_aarch64_saddl2v16qi(__a, __b);
}

FUNK(int32x4_t)
vaddl_high_s16(int16x8_t __a, int16x8_t __b) {
  return (int32x4_t)__builtin_aarch64_saddl2v8hi(__a, __b);
}

FUNK(int64x2_t)
vaddl_high_s32(int32x4_t __a, int32x4_t __b) {
  return (int64x2_t)__builtin_aarch64_saddl2v4si(__a, __b);
}

FUNK(uint16x8_t)
vaddl_high_u8(uint8x16_t __a, uint8x16_t __b) {
  return (uint16x8_t)__builtin_aarch64_uaddl2v16qi((int8x16_t)__a,
                                                   (int8x16_t)__b);
}

FUNK(uint32x4_t)
vaddl_high_u16(uint16x8_t __a, uint16x8_t __b) {
  return (uint32x4_t)__builtin_aarch64_uaddl2v8hi((int16x8_t)__a,
                                                  (int16x8_t)__b);
}

FUNK(uint64x2_t)
vaddl_high_u32(uint32x4_t __a, uint32x4_t __b) {
  return (uint64x2_t)__builtin_aarch64_uaddl2v4si((int32x4_t)__a,
                                                  (int32x4_t)__b);
}

FUNK(int16x8_t)
vaddw_s8(int16x8_t __a, int8x8_t __b) {
  return (int16x8_t)__builtin_aarch64_saddwv8qi(__a, __b);
}

FUNK(int32x4_t)
vaddw_s16(int32x4_t __a, int16x4_t __b) {
  return (int32x4_t)__builtin_aarch64_saddwv4hi(__a, __b);
}

FUNK(int64x2_t)
vaddw_s32(int64x2_t __a, int32x2_t __b) {
  return (int64x2_t)__builtin_aarch64_saddwv2si(__a, __b);
}

FUNK(uint16x8_t)
vaddw_u8(uint16x8_t __a, uint8x8_t __b) {
  return (uint16x8_t)__builtin_aarch64_uaddwv8qi((int16x8_t)__a, (int8x8_t)__b);
}

FUNK(uint32x4_t)
vaddw_u16(uint32x4_t __a, uint16x4_t __b) {
  return (uint32x4_t)__builtin_aarch64_uaddwv4hi((int32x4_t)__a,
                                                 (int16x4_t)__b);
}

FUNK(uint64x2_t)
vaddw_u32(uint64x2_t __a, uint32x2_t __b) {
  return (uint64x2_t)__builtin_aarch64_uaddwv2si((int64x2_t)__a,
                                                 (int32x2_t)__b);
}

FUNK(int16x8_t)
vaddw_high_s8(int16x8_t __a, int8x16_t __b) {
  return (int16x8_t)__builtin_aarch64_saddw2v16qi(__a, __b);
}

FUNK(int32x4_t)
vaddw_high_s16(int32x4_t __a, int16x8_t __b) {
  return (int32x4_t)__builtin_aarch64_saddw2v8hi(__a, __b);
}

FUNK(int64x2_t)
vaddw_high_s32(int64x2_t __a, int32x4_t __b) {
  return (int64x2_t)__builtin_aarch64_saddw2v4si(__a, __b);
}

FUNK(uint16x8_t)
vaddw_high_u8(uint16x8_t __a, uint8x16_t __b) {
  return (uint16x8_t)__builtin_aarch64_uaddw2v16qi((int16x8_t)__a,
                                                   (int8x16_t)__b);
}

FUNK(uint32x4_t)
vaddw_high_u16(uint32x4_t __a, uint16x8_t __b) {
  return (uint32x4_t)__builtin_aarch64_uaddw2v8hi((int32x4_t)__a,
                                                  (int16x8_t)__b);
}

FUNK(uint64x2_t)
vaddw_high_u32(uint64x2_t __a, uint32x4_t __b) {
  return (uint64x2_t)__builtin_aarch64_uaddw2v4si((int64x2_t)__a,
                                                  (int32x4_t)__b);
}

FUNK(int8x8_t)
vhadd_s8(int8x8_t __a, int8x8_t __b) {
  return (int8x8_t)__builtin_aarch64_shaddv8qi(__a, __b);
}

FUNK(int16x4_t)
vhadd_s16(int16x4_t __a, int16x4_t __b) {
  return (int16x4_t)__builtin_aarch64_shaddv4hi(__a, __b);
}

FUNK(int32x2_t)
vhadd_s32(int32x2_t __a, int32x2_t __b) {
  return (int32x2_t)__builtin_aarch64_shaddv2si(__a, __b);
}

FUNK(uint8x8_t)
vhadd_u8(uint8x8_t __a, uint8x8_t __b) {
  return (uint8x8_t)__builtin_aarch64_uhaddv8qi((int8x8_t)__a, (int8x8_t)__b);
}

FUNK(uint16x4_t)
vhadd_u16(uint16x4_t __a, uint16x4_t __b) {
  return (uint16x4_t)__builtin_aarch64_uhaddv4hi((int16x4_t)__a,
                                                 (int16x4_t)__b);
}

FUNK(uint32x2_t)
vhadd_u32(uint32x2_t __a, uint32x2_t __b) {
  return (uint32x2_t)__builtin_aarch64_uhaddv2si((int32x2_t)__a,
                                                 (int32x2_t)__b);
}

FUNK(int8x16_t)
vhaddq_s8(int8x16_t __a, int8x16_t __b) {
  return (int8x16_t)__builtin_aarch64_shaddv16qi(__a, __b);
}

FUNK(int16x8_t)
vhaddq_s16(int16x8_t __a, int16x8_t __b) {
  return (int16x8_t)__builtin_aarch64_shaddv8hi(__a, __b);
}

FUNK(int32x4_t)
vhaddq_s32(int32x4_t __a, int32x4_t __b) {
  return (int32x4_t)__builtin_aarch64_shaddv4si(__a, __b);
}

FUNK(uint8x16_t)
vhaddq_u8(uint8x16_t __a, uint8x16_t __b) {
  return (uint8x16_t)__builtin_aarch64_uhaddv16qi((int8x16_t)__a,
                                                  (int8x16_t)__b);
}

FUNK(uint16x8_t)
vhaddq_u16(uint16x8_t __a, uint16x8_t __b) {
  return (uint16x8_t)__builtin_aarch64_uhaddv8hi((int16x8_t)__a,
                                                 (int16x8_t)__b);
}

FUNK(uint32x4_t)
vhaddq_u32(uint32x4_t __a, uint32x4_t __b) {
  return (uint32x4_t)__builtin_aarch64_uhaddv4si((int32x4_t)__a,
                                                 (int32x4_t)__b);
}

FUNK(int8x8_t)
vrhadd_s8(int8x8_t __a, int8x8_t __b) {
  return (int8x8_t)__builtin_aarch64_srhaddv8qi(__a, __b);
}

FUNK(int16x4_t)
vrhadd_s16(int16x4_t __a, int16x4_t __b) {
  return (int16x4_t)__builtin_aarch64_srhaddv4hi(__a, __b);
}

FUNK(int32x2_t)
vrhadd_s32(int32x2_t __a, int32x2_t __b) {
  return (int32x2_t)__builtin_aarch64_srhaddv2si(__a, __b);
}

FUNK(uint8x8_t)
vrhadd_u8(uint8x8_t __a, uint8x8_t __b) {
  return (uint8x8_t)__builtin_aarch64_urhaddv8qi((int8x8_t)__a, (int8x8_t)__b);
}

FUNK(uint16x4_t)
vrhadd_u16(uint16x4_t __a, uint16x4_t __b) {
  return (uint16x4_t)__builtin_aarch64_urhaddv4hi((int16x4_t)__a,
                                                  (int16x4_t)__b);
}

FUNK(uint32x2_t)
vrhadd_u32(uint32x2_t __a, uint32x2_t __b) {
  return (uint32x2_t)__builtin_aarch64_urhaddv2si((int32x2_t)__a,
                                                  (int32x2_t)__b);
}

FUNK(int8x16_t)
vrhaddq_s8(int8x16_t __a, int8x16_t __b) {
  return (int8x16_t)__builtin_aarch64_srhaddv16qi(__a, __b);
}

FUNK(int16x8_t)
vrhaddq_s16(int16x8_t __a, int16x8_t __b) {
  return (int16x8_t)__builtin_aarch64_srhaddv8hi(__a, __b);
}

FUNK(int32x4_t)
vrhaddq_s32(int32x4_t __a, int32x4_t __b) {
  return (int32x4_t)__builtin_aarch64_srhaddv4si(__a, __b);
}

FUNK(uint8x16_t)
vrhaddq_u8(uint8x16_t __a, uint8x16_t __b) {
  return (uint8x16_t)__builtin_aarch64_urhaddv16qi((int8x16_t)__a,
                                                   (int8x16_t)__b);
}

FUNK(uint16x8_t)
vrhaddq_u16(uint16x8_t __a, uint16x8_t __b) {
  return (uint16x8_t)__builtin_aarch64_urhaddv8hi((int16x8_t)__a,
                                                  (int16x8_t)__b);
}

FUNK(uint32x4_t)
vrhaddq_u32(uint32x4_t __a, uint32x4_t __b) {
  return (uint32x4_t)__builtin_aarch64_urhaddv4si((int32x4_t)__a,
                                                  (int32x4_t)__b);
}

FUNK(int8x8_t)
vaddhn_s16(int16x8_t __a, int16x8_t __b) {
  return (int8x8_t)__builtin_aarch64_addhnv8hi(__a, __b);
}

FUNK(int16x4_t)
vaddhn_s32(int32x4_t __a, int32x4_t __b) {
  return (int16x4_t)__builtin_aarch64_addhnv4si(__a, __b);
}

FUNK(int32x2_t)
vaddhn_s64(int64x2_t __a, int64x2_t __b) {
  return (int32x2_t)__builtin_aarch64_addhnv2di(__a, __b);
}

FUNK(uint8x8_t)
vaddhn_u16(uint16x8_t __a, uint16x8_t __b) {
  return (uint8x8_t)__builtin_aarch64_addhnv8hi((int16x8_t)__a, (int16x8_t)__b);
}

FUNK(uint16x4_t)
vaddhn_u32(uint32x4_t __a, uint32x4_t __b) {
  return (uint16x4_t)__builtin_aarch64_addhnv4si((int32x4_t)__a,
                                                 (int32x4_t)__b);
}

FUNK(uint32x2_t)
vaddhn_u64(uint64x2_t __a, uint64x2_t __b) {
  return (uint32x2_t)__builtin_aarch64_addhnv2di((int64x2_t)__a,
                                                 (int64x2_t)__b);
}

FUNK(int8x8_t)
vraddhn_s16(int16x8_t __a, int16x8_t __b) {
  return (int8x8_t)__builtin_aarch64_raddhnv8hi(__a, __b);
}

FUNK(int16x4_t)
vraddhn_s32(int32x4_t __a, int32x4_t __b) {
  return (int16x4_t)__builtin_aarch64_raddhnv4si(__a, __b);
}

FUNK(int32x2_t)
vraddhn_s64(int64x2_t __a, int64x2_t __b) {
  return (int32x2_t)__builtin_aarch64_raddhnv2di(__a, __b);
}

FUNK(uint8x8_t)
vraddhn_u16(uint16x8_t __a, uint16x8_t __b) {
  return (uint8x8_t)__builtin_aarch64_raddhnv8hi((int16x8_t)__a,
                                                 (int16x8_t)__b);
}

FUNK(uint16x4_t)
vraddhn_u32(uint32x4_t __a, uint32x4_t __b) {
  return (uint16x4_t)__builtin_aarch64_raddhnv4si((int32x4_t)__a,
                                                  (int32x4_t)__b);
}

FUNK(uint32x2_t)
vraddhn_u64(uint64x2_t __a, uint64x2_t __b) {
  return (uint32x2_t)__builtin_aarch64_raddhnv2di((int64x2_t)__a,
                                                  (int64x2_t)__b);
}

FUNK(int8x16_t)
vaddhn_high_s16(int8x8_t __a, int16x8_t __b, int16x8_t __c) {
  return (int8x16_t)__builtin_aarch64_addhn2v8hi(__a, __b, __c);
}

FUNK(int16x8_t)
vaddhn_high_s32(int16x4_t __a, int32x4_t __b, int32x4_t __c) {
  return (int16x8_t)__builtin_aarch64_addhn2v4si(__a, __b, __c);
}

FUNK(int32x4_t)
vaddhn_high_s64(int32x2_t __a, int64x2_t __b, int64x2_t __c) {
  return (int32x4_t)__builtin_aarch64_addhn2v2di(__a, __b, __c);
}

FUNK(uint8x16_t)
vaddhn_high_u16(uint8x8_t __a, uint16x8_t __b, uint16x8_t __c) {
  return (uint8x16_t)__builtin_aarch64_addhn2v8hi((int8x8_t)__a, (int16x8_t)__b,
                                                  (int16x8_t)__c);
}

FUNK(uint16x8_t)
vaddhn_high_u32(uint16x4_t __a, uint32x4_t __b, uint32x4_t __c) {
  return (uint16x8_t)__builtin_aarch64_addhn2v4si(
      (int16x4_t)__a, (int32x4_t)__b, (int32x4_t)__c);
}

FUNK(uint32x4_t)
vaddhn_high_u64(uint32x2_t __a, uint64x2_t __b, uint64x2_t __c) {
  return (uint32x4_t)__builtin_aarch64_addhn2v2di(
      (int32x2_t)__a, (int64x2_t)__b, (int64x2_t)__c);
}

FUNK(int8x16_t)
vraddhn_high_s16(int8x8_t __a, int16x8_t __b, int16x8_t __c) {
  return (int8x16_t)__builtin_aarch64_raddhn2v8hi(__a, __b, __c);
}

FUNK(int16x8_t)
vraddhn_high_s32(int16x4_t __a, int32x4_t __b, int32x4_t __c) {
  return (int16x8_t)__builtin_aarch64_raddhn2v4si(__a, __b, __c);
}

FUNK(int32x4_t)
vraddhn_high_s64(int32x2_t __a, int64x2_t __b, int64x2_t __c) {
  return (int32x4_t)__builtin_aarch64_raddhn2v2di(__a, __b, __c);
}

FUNK(uint8x16_t)
vraddhn_high_u16(uint8x8_t __a, uint16x8_t __b, uint16x8_t __c) {
  return (uint8x16_t)__builtin_aarch64_raddhn2v8hi(
      (int8x8_t)__a, (int16x8_t)__b, (int16x8_t)__c);
}

FUNK(uint16x8_t)
vraddhn_high_u32(uint16x4_t __a, uint32x4_t __b, uint32x4_t __c) {
  return (uint16x8_t)__builtin_aarch64_raddhn2v4si(
      (int16x4_t)__a, (int32x4_t)__b, (int32x4_t)__c);
}

FUNK(uint32x4_t)
vraddhn_high_u64(uint32x2_t __a, uint64x2_t __b, uint64x2_t __c) {
  return (uint32x4_t)__builtin_aarch64_raddhn2v2di(
      (int32x2_t)__a, (int64x2_t)__b, (int64x2_t)__c);
}

FUNK(float32x2_t)
vdiv_f32(float32x2_t __a, float32x2_t __b) {
  return __a / __b;
}

FUNK(float64x1_t)
vdiv_f64(float64x1_t __a, float64x1_t __b) {
  return __a / __b;
}

FUNK(float32x4_t)
vdivq_f32(float32x4_t __a, float32x4_t __b) {
  return __a / __b;
}

FUNK(float64x2_t)
vdivq_f64(float64x2_t __a, float64x2_t __b) {
  return __a / __b;
}

FUNK(int8x8_t)
vmul_s8(int8x8_t __a, int8x8_t __b) {
  return __a * __b;
}

FUNK(int16x4_t)
vmul_s16(int16x4_t __a, int16x4_t __b) {
  return __a * __b;
}

FUNK(int32x2_t)
vmul_s32(int32x2_t __a, int32x2_t __b) {
  return __a * __b;
}

FUNK(float32x2_t)
vmul_f32(float32x2_t __a, float32x2_t __b) {
  return __a * __b;
}

FUNK(float64x1_t)
vmul_f64(float64x1_t __a, float64x1_t __b) {
  return __a * __b;
}

FUNK(uint8x8_t)
vmul_u8(uint8x8_t __a, uint8x8_t __b) {
  return __a * __b;
}

FUNK(uint16x4_t)
vmul_u16(uint16x4_t __a, uint16x4_t __b) {
  return __a * __b;
}

FUNK(uint32x2_t)
vmul_u32(uint32x2_t __a, uint32x2_t __b) {
  return __a * __b;
}

FUNK(poly8x8_t)
vmul_p8(poly8x8_t __a, poly8x8_t __b) {
  return (poly8x8_t)__builtin_aarch64_pmulv8qi((int8x8_t)__a, (int8x8_t)__b);
}

FUNK(int8x16_t)
vmulq_s8(int8x16_t __a, int8x16_t __b) {
  return __a * __b;
}

FUNK(int16x8_t)
vmulq_s16(int16x8_t __a, int16x8_t __b) {
  return __a * __b;
}

FUNK(int32x4_t)
vmulq_s32(int32x4_t __a, int32x4_t __b) {
  return __a * __b;
}

FUNK(float32x4_t)
vmulq_f32(float32x4_t __a, float32x4_t __b) {
  return __a * __b;
}

FUNK(float64x2_t)
vmulq_f64(float64x2_t __a, float64x2_t __b) {
  return __a * __b;
}

FUNK(uint8x16_t)
vmulq_u8(uint8x16_t __a, uint8x16_t __b) {
  return __a * __b;
}

FUNK(uint16x8_t)
vmulq_u16(uint16x8_t __a, uint16x8_t __b) {
  return __a * __b;
}

FUNK(uint32x4_t)
vmulq_u32(uint32x4_t __a, uint32x4_t __b) {
  return __a * __b;
}

FUNK(poly8x16_t)
vmulq_p8(poly8x16_t __a, poly8x16_t __b) {
  return (poly8x16_t)__builtin_aarch64_pmulv16qi((int8x16_t)__a,
                                                 (int8x16_t)__b);
}

FUNK(int8x8_t)
vand_s8(int8x8_t __a, int8x8_t __b) {
  return __a & __b;
}

FUNK(int16x4_t)
vand_s16(int16x4_t __a, int16x4_t __b) {
  return __a & __b;
}

FUNK(int32x2_t)
vand_s32(int32x2_t __a, int32x2_t __b) {
  return __a & __b;
}

FUNK(uint8x8_t)
vand_u8(uint8x8_t __a, uint8x8_t __b) {
  return __a & __b;
}

FUNK(uint16x4_t)
vand_u16(uint16x4_t __a, uint16x4_t __b) {
  return __a & __b;
}

FUNK(uint32x2_t)
vand_u32(uint32x2_t __a, uint32x2_t __b) {
  return __a & __b;
}

FUNK(int64x1_t)
vand_s64(int64x1_t __a, int64x1_t __b) {
  return __a & __b;
}

FUNK(uint64x1_t)
vand_u64(uint64x1_t __a, uint64x1_t __b) {
  return __a & __b;
}

FUNK(int8x16_t)
vandq_s8(int8x16_t __a, int8x16_t __b) {
  return __a & __b;
}

FUNK(int16x8_t)
vandq_s16(int16x8_t __a, int16x8_t __b) {
  return __a & __b;
}

FUNK(int32x4_t)
vandq_s32(int32x4_t __a, int32x4_t __b) {
  return __a & __b;
}

FUNK(int64x2_t)
vandq_s64(int64x2_t __a, int64x2_t __b) {
  return __a & __b;
}

FUNK(uint8x16_t)
vandq_u8(uint8x16_t __a, uint8x16_t __b) {
  return __a & __b;
}

FUNK(uint16x8_t)
vandq_u16(uint16x8_t __a, uint16x8_t __b) {
  return __a & __b;
}

FUNK(uint32x4_t)
vandq_u32(uint32x4_t __a, uint32x4_t __b) {
  return __a & __b;
}

FUNK(uint64x2_t)
vandq_u64(uint64x2_t __a, uint64x2_t __b) {
  return __a & __b;
}

FUNK(int8x8_t)
vorr_s8(int8x8_t __a, int8x8_t __b) {
  return __a | __b;
}

FUNK(int16x4_t)
vorr_s16(int16x4_t __a, int16x4_t __b) {
  return __a | __b;
}

FUNK(int32x2_t)
vorr_s32(int32x2_t __a, int32x2_t __b) {
  return __a | __b;
}

FUNK(uint8x8_t)
vorr_u8(uint8x8_t __a, uint8x8_t __b) {
  return __a | __b;
}

FUNK(uint16x4_t)
vorr_u16(uint16x4_t __a, uint16x4_t __b) {
  return __a | __b;
}

FUNK(uint32x2_t)
vorr_u32(uint32x2_t __a, uint32x2_t __b) {
  return __a | __b;
}

FUNK(int64x1_t)
vorr_s64(int64x1_t __a, int64x1_t __b) {
  return __a | __b;
}

FUNK(uint64x1_t)
vorr_u64(uint64x1_t __a, uint64x1_t __b) {
  return __a | __b;
}

FUNK(int8x16_t)
vorrq_s8(int8x16_t __a, int8x16_t __b) {
  return __a | __b;
}

FUNK(int16x8_t)
vorrq_s16(int16x8_t __a, int16x8_t __b) {
  return __a | __b;
}

FUNK(int32x4_t)
vorrq_s32(int32x4_t __a, int32x4_t __b) {
  return __a | __b;
}

FUNK(int64x2_t)
vorrq_s64(int64x2_t __a, int64x2_t __b) {
  return __a | __b;
}

FUNK(uint8x16_t)
vorrq_u8(uint8x16_t __a, uint8x16_t __b) {
  return __a | __b;
}

FUNK(uint16x8_t)
vorrq_u16(uint16x8_t __a, uint16x8_t __b) {
  return __a | __b;
}

FUNK(uint32x4_t)
vorrq_u32(uint32x4_t __a, uint32x4_t __b) {
  return __a | __b;
}

FUNK(uint64x2_t)
vorrq_u64(uint64x2_t __a, uint64x2_t __b) {
  return __a | __b;
}

FUNK(int8x8_t)
veor_s8(int8x8_t __a, int8x8_t __b) {
  return __a ^ __b;
}

FUNK(int16x4_t)
veor_s16(int16x4_t __a, int16x4_t __b) {
  return __a ^ __b;
}

FUNK(int32x2_t)
veor_s32(int32x2_t __a, int32x2_t __b) {
  return __a ^ __b;
}

FUNK(uint8x8_t)
veor_u8(uint8x8_t __a, uint8x8_t __b) {
  return __a ^ __b;
}

FUNK(uint16x4_t)
veor_u16(uint16x4_t __a, uint16x4_t __b) {
  return __a ^ __b;
}

FUNK(uint32x2_t)
veor_u32(uint32x2_t __a, uint32x2_t __b) {
  return __a ^ __b;
}

FUNK(int64x1_t)
veor_s64(int64x1_t __a, int64x1_t __b) {
  return __a ^ __b;
}

FUNK(uint64x1_t)
veor_u64(uint64x1_t __a, uint64x1_t __b) {
  return __a ^ __b;
}

FUNK(int8x16_t)
veorq_s8(int8x16_t __a, int8x16_t __b) {
  return __a ^ __b;
}

FUNK(int16x8_t)
veorq_s16(int16x8_t __a, int16x8_t __b) {
  return __a ^ __b;
}

FUNK(int32x4_t)
veorq_s32(int32x4_t __a, int32x4_t __b) {
  return __a ^ __b;
}

FUNK(int64x2_t)
veorq_s64(int64x2_t __a, int64x2_t __b) {
  return __a ^ __b;
}

FUNK(uint8x16_t)
veorq_u8(uint8x16_t __a, uint8x16_t __b) {
  return __a ^ __b;
}

FUNK(uint16x8_t)
veorq_u16(uint16x8_t __a, uint16x8_t __b) {
  return __a ^ __b;
}

FUNK(uint32x4_t)
veorq_u32(uint32x4_t __a, uint32x4_t __b) {
  return __a ^ __b;
}

FUNK(uint64x2_t)
veorq_u64(uint64x2_t __a, uint64x2_t __b) {
  return __a ^ __b;
}

FUNK(int8x8_t)
vbic_s8(int8x8_t __a, int8x8_t __b) {
  return __a & ~__b;
}

FUNK(int16x4_t)
vbic_s16(int16x4_t __a, int16x4_t __b) {
  return __a & ~__b;
}

FUNK(int32x2_t)
vbic_s32(int32x2_t __a, int32x2_t __b) {
  return __a & ~__b;
}

FUNK(uint8x8_t)
vbic_u8(uint8x8_t __a, uint8x8_t __b) {
  return __a & ~__b;
}

FUNK(uint16x4_t)
vbic_u16(uint16x4_t __a, uint16x4_t __b) {
  return __a & ~__b;
}

FUNK(uint32x2_t)
vbic_u32(uint32x2_t __a, uint32x2_t __b) {
  return __a & ~__b;
}

FUNK(int64x1_t)
vbic_s64(int64x1_t __a, int64x1_t __b) {
  return __a & ~__b;
}

FUNK(uint64x1_t)
vbic_u64(uint64x1_t __a, uint64x1_t __b) {
  return __a & ~__b;
}

FUNK(int8x16_t)
vbicq_s8(int8x16_t __a, int8x16_t __b) {
  return __a & ~__b;
}

FUNK(int16x8_t)
vbicq_s16(int16x8_t __a, int16x8_t __b) {
  return __a & ~__b;
}

FUNK(int32x4_t)
vbicq_s32(int32x4_t __a, int32x4_t __b) {
  return __a & ~__b;
}

FUNK(int64x2_t)
vbicq_s64(int64x2_t __a, int64x2_t __b) {
  return __a & ~__b;
}

FUNK(uint8x16_t)
vbicq_u8(uint8x16_t __a, uint8x16_t __b) {
  return __a & ~__b;
}

FUNK(uint16x8_t)
vbicq_u16(uint16x8_t __a, uint16x8_t __b) {
  return __a & ~__b;
}

FUNK(uint32x4_t)
vbicq_u32(uint32x4_t __a, uint32x4_t __b) {
  return __a & ~__b;
}

FUNK(uint64x2_t)
vbicq_u64(uint64x2_t __a, uint64x2_t __b) {
  return __a & ~__b;
}

FUNK(int8x8_t)
vorn_s8(int8x8_t __a, int8x8_t __b) {
  return __a | ~__b;
}

FUNK(int16x4_t)
vorn_s16(int16x4_t __a, int16x4_t __b) {
  return __a | ~__b;
}

FUNK(int32x2_t)
vorn_s32(int32x2_t __a, int32x2_t __b) {
  return __a | ~__b;
}

FUNK(uint8x8_t)
vorn_u8(uint8x8_t __a, uint8x8_t __b) {
  return __a | ~__b;
}

FUNK(uint16x4_t)
vorn_u16(uint16x4_t __a, uint16x4_t __b) {
  return __a | ~__b;
}

FUNK(uint32x2_t)
vorn_u32(uint32x2_t __a, uint32x2_t __b) {
  return __a | ~__b;
}

FUNK(int64x1_t)
vorn_s64(int64x1_t __a, int64x1_t __b) {
  return __a | ~__b;
}

FUNK(uint64x1_t)
vorn_u64(uint64x1_t __a, uint64x1_t __b) {
  return __a | ~__b;
}

FUNK(int8x16_t)
vornq_s8(int8x16_t __a, int8x16_t __b) {
  return __a | ~__b;
}

FUNK(int16x8_t)
vornq_s16(int16x8_t __a, int16x8_t __b) {
  return __a | ~__b;
}

FUNK(int32x4_t)
vornq_s32(int32x4_t __a, int32x4_t __b) {
  return __a | ~__b;
}

FUNK(int64x2_t)
vornq_s64(int64x2_t __a, int64x2_t __b) {
  return __a | ~__b;
}

FUNK(uint8x16_t)
vornq_u8(uint8x16_t __a, uint8x16_t __b) {
  return __a | ~__b;
}

FUNK(uint16x8_t)
vornq_u16(uint16x8_t __a, uint16x8_t __b) {
  return __a | ~__b;
}

FUNK(uint32x4_t)
vornq_u32(uint32x4_t __a, uint32x4_t __b) {
  return __a | ~__b;
}

FUNK(uint64x2_t)
vornq_u64(uint64x2_t __a, uint64x2_t __b) {
  return __a | ~__b;
}

FUNK(int8x8_t)
vsub_s8(int8x8_t __a, int8x8_t __b) {
  return __a - __b;
}

FUNK(int16x4_t)
vsub_s16(int16x4_t __a, int16x4_t __b) {
  return __a - __b;
}

FUNK(int32x2_t)
vsub_s32(int32x2_t __a, int32x2_t __b) {
  return __a - __b;
}

FUNK(float32x2_t)
vsub_f32(float32x2_t __a, float32x2_t __b) {
  return __a - __b;
}

FUNK(float64x1_t)
vsub_f64(float64x1_t __a, float64x1_t __b) {
  return __a - __b;
}

FUNK(uint8x8_t)
vsub_u8(uint8x8_t __a, uint8x8_t __b) {
  return __a - __b;
}

FUNK(uint16x4_t)
vsub_u16(uint16x4_t __a, uint16x4_t __b) {
  return __a - __b;
}

FUNK(uint32x2_t)
vsub_u32(uint32x2_t __a, uint32x2_t __b) {
  return __a - __b;
}

FUNK(int64x1_t)
vsub_s64(int64x1_t __a, int64x1_t __b) {
  return __a - __b;
}

FUNK(uint64x1_t)
vsub_u64(uint64x1_t __a, uint64x1_t __b) {
  return __a - __b;
}

FUNK(int8x16_t)
vsubq_s8(int8x16_t __a, int8x16_t __b) {
  return __a - __b;
}

FUNK(int16x8_t)
vsubq_s16(int16x8_t __a, int16x8_t __b) {
  return __a - __b;
}

FUNK(int32x4_t)
vsubq_s32(int32x4_t __a, int32x4_t __b) {
  return __a - __b;
}

FUNK(int64x2_t)
vsubq_s64(int64x2_t __a, int64x2_t __b) {
  return __a - __b;
}

FUNK(float32x4_t)
vsubq_f32(float32x4_t __a, float32x4_t __b) {
  return __a - __b;
}

FUNK(float64x2_t)
vsubq_f64(float64x2_t __a, float64x2_t __b) {
  return __a - __b;
}

FUNK(uint8x16_t)
vsubq_u8(uint8x16_t __a, uint8x16_t __b) {
  return __a - __b;
}

FUNK(uint16x8_t)
vsubq_u16(uint16x8_t __a, uint16x8_t __b) {
  return __a - __b;
}

FUNK(uint32x4_t)
vsubq_u32(uint32x4_t __a, uint32x4_t __b) {
  return __a - __b;
}

FUNK(uint64x2_t)
vsubq_u64(uint64x2_t __a, uint64x2_t __b) {
  return __a - __b;
}

FUNK(int16x8_t)
vsubl_s8(int8x8_t __a, int8x8_t __b) {
  return (int16x8_t)__builtin_aarch64_ssublv8qi(__a, __b);
}

FUNK(int32x4_t)
vsubl_s16(int16x4_t __a, int16x4_t __b) {
  return (int32x4_t)__builtin_aarch64_ssublv4hi(__a, __b);
}

FUNK(int64x2_t)
vsubl_s32(int32x2_t __a, int32x2_t __b) {
  return (int64x2_t)__builtin_aarch64_ssublv2si(__a, __b);
}

FUNK(uint16x8_t)
vsubl_u8(uint8x8_t __a, uint8x8_t __b) {
  return (uint16x8_t)__builtin_aarch64_usublv8qi((int8x8_t)__a, (int8x8_t)__b);
}

FUNK(uint32x4_t)
vsubl_u16(uint16x4_t __a, uint16x4_t __b) {
  return (uint32x4_t)__builtin_aarch64_usublv4hi((int16x4_t)__a,
                                                 (int16x4_t)__b);
}

FUNK(uint64x2_t)
vsubl_u32(uint32x2_t __a, uint32x2_t __b) {
  return (uint64x2_t)__builtin_aarch64_usublv2si((int32x2_t)__a,
                                                 (int32x2_t)__b);
}

FUNK(int16x8_t)
vsubl_high_s8(int8x16_t __a, int8x16_t __b) {
  return (int16x8_t)__builtin_aarch64_ssubl2v16qi(__a, __b);
}

FUNK(int32x4_t)
vsubl_high_s16(int16x8_t __a, int16x8_t __b) {
  return (int32x4_t)__builtin_aarch64_ssubl2v8hi(__a, __b);
}

FUNK(int64x2_t)
vsubl_high_s32(int32x4_t __a, int32x4_t __b) {
  return (int64x2_t)__builtin_aarch64_ssubl2v4si(__a, __b);
}

FUNK(uint16x8_t)
vsubl_high_u8(uint8x16_t __a, uint8x16_t __b) {
  return (uint16x8_t)__builtin_aarch64_usubl2v16qi((int8x16_t)__a,
                                                   (int8x16_t)__b);
}

FUNK(uint32x4_t)
vsubl_high_u16(uint16x8_t __a, uint16x8_t __b) {
  return (uint32x4_t)__builtin_aarch64_usubl2v8hi((int16x8_t)__a,
                                                  (int16x8_t)__b);
}

FUNK(uint64x2_t)
vsubl_high_u32(uint32x4_t __a, uint32x4_t __b) {
  return (uint64x2_t)__builtin_aarch64_usubl2v4si((int32x4_t)__a,
                                                  (int32x4_t)__b);
}

FUNK(int16x8_t)
vsubw_s8(int16x8_t __a, int8x8_t __b) {
  return (int16x8_t)__builtin_aarch64_ssubwv8qi(__a, __b);
}

FUNK(int32x4_t)
vsubw_s16(int32x4_t __a, int16x4_t __b) {
  return (int32x4_t)__builtin_aarch64_ssubwv4hi(__a, __b);
}

FUNK(int64x2_t)
vsubw_s32(int64x2_t __a, int32x2_t __b) {
  return (int64x2_t)__builtin_aarch64_ssubwv2si(__a, __b);
}

FUNK(uint16x8_t)
vsubw_u8(uint16x8_t __a, uint8x8_t __b) {
  return (uint16x8_t)__builtin_aarch64_usubwv8qi((int16x8_t)__a, (int8x8_t)__b);
}

FUNK(uint32x4_t)
vsubw_u16(uint32x4_t __a, uint16x4_t __b) {
  return (uint32x4_t)__builtin_aarch64_usubwv4hi((int32x4_t)__a,
                                                 (int16x4_t)__b);
}

FUNK(uint64x2_t)
vsubw_u32(uint64x2_t __a, uint32x2_t __b) {
  return (uint64x2_t)__builtin_aarch64_usubwv2si((int64x2_t)__a,
                                                 (int32x2_t)__b);
}

FUNK(int16x8_t)
vsubw_high_s8(int16x8_t __a, int8x16_t __b) {
  return (int16x8_t)__builtin_aarch64_ssubw2v16qi(__a, __b);
}

FUNK(int32x4_t)
vsubw_high_s16(int32x4_t __a, int16x8_t __b) {
  return (int32x4_t)__builtin_aarch64_ssubw2v8hi(__a, __b);
}

FUNK(int64x2_t)
vsubw_high_s32(int64x2_t __a, int32x4_t __b) {
  return (int64x2_t)__builtin_aarch64_ssubw2v4si(__a, __b);
}

FUNK(uint16x8_t)
vsubw_high_u8(uint16x8_t __a, uint8x16_t __b) {
  return (uint16x8_t)__builtin_aarch64_usubw2v16qi((int16x8_t)__a,
                                                   (int8x16_t)__b);
}

FUNK(uint32x4_t)
vsubw_high_u16(uint32x4_t __a, uint16x8_t __b) {
  return (uint32x4_t)__builtin_aarch64_usubw2v8hi((int32x4_t)__a,
                                                  (int16x8_t)__b);
}

FUNK(uint64x2_t)
vsubw_high_u32(uint64x2_t __a, uint32x4_t __b) {
  return (uint64x2_t)__builtin_aarch64_usubw2v4si((int64x2_t)__a,
                                                  (int32x4_t)__b);
}

FUNK(int8x8_t)
vqadd_s8(int8x8_t __a, int8x8_t __b) {
  return (int8x8_t)__builtin_aarch64_sqaddv8qi(__a, __b);
}

FUNK(int16x4_t)
vqadd_s16(int16x4_t __a, int16x4_t __b) {
  return (int16x4_t)__builtin_aarch64_sqaddv4hi(__a, __b);
}

FUNK(int32x2_t)
vqadd_s32(int32x2_t __a, int32x2_t __b) {
  return (int32x2_t)__builtin_aarch64_sqaddv2si(__a, __b);
}

FUNK(int64x1_t)
vqadd_s64(int64x1_t __a, int64x1_t __b) {
  return (int64x1_t){__builtin_aarch64_sqadddi(__a[0], __b[0])};
}

FUNK(uint8x8_t)
vqadd_u8(uint8x8_t __a, uint8x8_t __b) {
  return __builtin_aarch64_uqaddv8qi_uuu(__a, __b);
}

FUNK(int8x8_t)
vhsub_s8(int8x8_t __a, int8x8_t __b) {
  return (int8x8_t)__builtin_aarch64_shsubv8qi(__a, __b);
}

FUNK(int16x4_t)
vhsub_s16(int16x4_t __a, int16x4_t __b) {
  return (int16x4_t)__builtin_aarch64_shsubv4hi(__a, __b);
}

FUNK(int32x2_t)
vhsub_s32(int32x2_t __a, int32x2_t __b) {
  return (int32x2_t)__builtin_aarch64_shsubv2si(__a, __b);
}

FUNK(uint8x8_t)
vhsub_u8(uint8x8_t __a, uint8x8_t __b) {
  return (uint8x8_t)__builtin_aarch64_uhsubv8qi((int8x8_t)__a, (int8x8_t)__b);
}

FUNK(uint16x4_t)
vhsub_u16(uint16x4_t __a, uint16x4_t __b) {
  return (uint16x4_t)__builtin_aarch64_uhsubv4hi((int16x4_t)__a,
                                                 (int16x4_t)__b);
}

FUNK(uint32x2_t)
vhsub_u32(uint32x2_t __a, uint32x2_t __b) {
  return (uint32x2_t)__builtin_aarch64_uhsubv2si((int32x2_t)__a,
                                                 (int32x2_t)__b);
}

FUNK(int8x16_t)
vhsubq_s8(int8x16_t __a, int8x16_t __b) {
  return (int8x16_t)__builtin_aarch64_shsubv16qi(__a, __b);
}

FUNK(int16x8_t)
vhsubq_s16(int16x8_t __a, int16x8_t __b) {
  return (int16x8_t)__builtin_aarch64_shsubv8hi(__a, __b);
}

FUNK(int32x4_t)
vhsubq_s32(int32x4_t __a, int32x4_t __b) {
  return (int32x4_t)__builtin_aarch64_shsubv4si(__a, __b);
}

FUNK(uint8x16_t)
vhsubq_u8(uint8x16_t __a, uint8x16_t __b) {
  return (uint8x16_t)__builtin_aarch64_uhsubv16qi((int8x16_t)__a,
                                                  (int8x16_t)__b);
}

FUNK(uint16x8_t)
vhsubq_u16(uint16x8_t __a, uint16x8_t __b) {
  return (uint16x8_t)__builtin_aarch64_uhsubv8hi((int16x8_t)__a,
                                                 (int16x8_t)__b);
}

FUNK(uint32x4_t)
vhsubq_u32(uint32x4_t __a, uint32x4_t __b) {
  return (uint32x4_t)__builtin_aarch64_uhsubv4si((int32x4_t)__a,
                                                 (int32x4_t)__b);
}

FUNK(int8x8_t)
vsubhn_s16(int16x8_t __a, int16x8_t __b) {
  return (int8x8_t)__builtin_aarch64_subhnv8hi(__a, __b);
}

FUNK(int16x4_t)
vsubhn_s32(int32x4_t __a, int32x4_t __b) {
  return (int16x4_t)__builtin_aarch64_subhnv4si(__a, __b);
}

FUNK(int32x2_t)
vsubhn_s64(int64x2_t __a, int64x2_t __b) {
  return (int32x2_t)__builtin_aarch64_subhnv2di(__a, __b);
}

FUNK(uint8x8_t)
vsubhn_u16(uint16x8_t __a, uint16x8_t __b) {
  return (uint8x8_t)__builtin_aarch64_subhnv8hi((int16x8_t)__a, (int16x8_t)__b);
}

FUNK(uint16x4_t)
vsubhn_u32(uint32x4_t __a, uint32x4_t __b) {
  return (uint16x4_t)__builtin_aarch64_subhnv4si((int32x4_t)__a,
                                                 (int32x4_t)__b);
}

FUNK(uint32x2_t)
vsubhn_u64(uint64x2_t __a, uint64x2_t __b) {
  return (uint32x2_t)__builtin_aarch64_subhnv2di((int64x2_t)__a,
                                                 (int64x2_t)__b);
}

FUNK(int8x8_t)
vrsubhn_s16(int16x8_t __a, int16x8_t __b) {
  return (int8x8_t)__builtin_aarch64_rsubhnv8hi(__a, __b);
}

FUNK(int16x4_t)
vrsubhn_s32(int32x4_t __a, int32x4_t __b) {
  return (int16x4_t)__builtin_aarch64_rsubhnv4si(__a, __b);
}

FUNK(int32x2_t)
vrsubhn_s64(int64x2_t __a, int64x2_t __b) {
  return (int32x2_t)__builtin_aarch64_rsubhnv2di(__a, __b);
}

FUNK(uint8x8_t)
vrsubhn_u16(uint16x8_t __a, uint16x8_t __b) {
  return (uint8x8_t)__builtin_aarch64_rsubhnv8hi((int16x8_t)__a,
                                                 (int16x8_t)__b);
}

FUNK(uint16x4_t)
vrsubhn_u32(uint32x4_t __a, uint32x4_t __b) {
  return (uint16x4_t)__builtin_aarch64_rsubhnv4si((int32x4_t)__a,
                                                  (int32x4_t)__b);
}

FUNK(uint32x2_t)
vrsubhn_u64(uint64x2_t __a, uint64x2_t __b) {
  return (uint32x2_t)__builtin_aarch64_rsubhnv2di((int64x2_t)__a,
                                                  (int64x2_t)__b);
}

FUNK(int8x16_t)
vrsubhn_high_s16(int8x8_t __a, int16x8_t __b, int16x8_t __c) {
  return (int8x16_t)__builtin_aarch64_rsubhn2v8hi(__a, __b, __c);
}

FUNK(int16x8_t)
vrsubhn_high_s32(int16x4_t __a, int32x4_t __b, int32x4_t __c) {
  return (int16x8_t)__builtin_aarch64_rsubhn2v4si(__a, __b, __c);
}

FUNK(int32x4_t)
vrsubhn_high_s64(int32x2_t __a, int64x2_t __b, int64x2_t __c) {
  return (int32x4_t)__builtin_aarch64_rsubhn2v2di(__a, __b, __c);
}

FUNK(uint8x16_t)
vrsubhn_high_u16(uint8x8_t __a, uint16x8_t __b, uint16x8_t __c) {
  return (uint8x16_t)__builtin_aarch64_rsubhn2v8hi(
      (int8x8_t)__a, (int16x8_t)__b, (int16x8_t)__c);
}

FUNK(uint16x8_t)
vrsubhn_high_u32(uint16x4_t __a, uint32x4_t __b, uint32x4_t __c) {
  return (uint16x8_t)__builtin_aarch64_rsubhn2v4si(
      (int16x4_t)__a, (int32x4_t)__b, (int32x4_t)__c);
}

FUNK(uint32x4_t)
vrsubhn_high_u64(uint32x2_t __a, uint64x2_t __b, uint64x2_t __c) {
  return (uint32x4_t)__builtin_aarch64_rsubhn2v2di(
      (int32x2_t)__a, (int64x2_t)__b, (int64x2_t)__c);
}

FUNK(int8x16_t)
vsubhn_high_s16(int8x8_t __a, int16x8_t __b, int16x8_t __c) {
  return (int8x16_t)__builtin_aarch64_subhn2v8hi(__a, __b, __c);
}

FUNK(int16x8_t)
vsubhn_high_s32(int16x4_t __a, int32x4_t __b, int32x4_t __c) {
  return (int16x8_t)__builtin_aarch64_subhn2v4si(__a, __b, __c);
  ;
}

FUNK(int32x4_t)
vsubhn_high_s64(int32x2_t __a, int64x2_t __b, int64x2_t __c) {
  return (int32x4_t)__builtin_aarch64_subhn2v2di(__a, __b, __c);
}

FUNK(uint8x16_t)
vsubhn_high_u16(uint8x8_t __a, uint16x8_t __b, uint16x8_t __c) {
  return (uint8x16_t)__builtin_aarch64_subhn2v8hi((int8x8_t)__a, (int16x8_t)__b,
                                                  (int16x8_t)__c);
}

FUNK(uint16x8_t)
vsubhn_high_u32(uint16x4_t __a, uint32x4_t __b, uint32x4_t __c) {
  return (uint16x8_t)__builtin_aarch64_subhn2v4si(
      (int16x4_t)__a, (int32x4_t)__b, (int32x4_t)__c);
}

FUNK(uint32x4_t)
vsubhn_high_u64(uint32x2_t __a, uint64x2_t __b, uint64x2_t __c) {
  return (uint32x4_t)__builtin_aarch64_subhn2v2di(
      (int32x2_t)__a, (int64x2_t)__b, (int64x2_t)__c);
}

FUNK(uint16x4_t)
vqadd_u16(uint16x4_t __a, uint16x4_t __b) {
  return __builtin_aarch64_uqaddv4hi_uuu(__a, __b);
}

FUNK(uint32x2_t)
vqadd_u32(uint32x2_t __a, uint32x2_t __b) {
  return __builtin_aarch64_uqaddv2si_uuu(__a, __b);
}

FUNK(uint64x1_t)
vqadd_u64(uint64x1_t __a, uint64x1_t __b) {
  return (uint64x1_t){__builtin_aarch64_uqadddi_uuu(__a[0], __b[0])};
}

FUNK(int8x16_t)
vqaddq_s8(int8x16_t __a, int8x16_t __b) {
  return (int8x16_t)__builtin_aarch64_sqaddv16qi(__a, __b);
}

FUNK(int16x8_t)
vqaddq_s16(int16x8_t __a, int16x8_t __b) {
  return (int16x8_t)__builtin_aarch64_sqaddv8hi(__a, __b);
}

FUNK(int32x4_t)
vqaddq_s32(int32x4_t __a, int32x4_t __b) {
  return (int32x4_t)__builtin_aarch64_sqaddv4si(__a, __b);
}

FUNK(int64x2_t)
vqaddq_s64(int64x2_t __a, int64x2_t __b) {
  return (int64x2_t)__builtin_aarch64_sqaddv2di(__a, __b);
}

FUNK(uint8x16_t)
vqaddq_u8(uint8x16_t __a, uint8x16_t __b) {
  return __builtin_aarch64_uqaddv16qi_uuu(__a, __b);
}

FUNK(uint16x8_t)
vqaddq_u16(uint16x8_t __a, uint16x8_t __b) {
  return __builtin_aarch64_uqaddv8hi_uuu(__a, __b);
}

FUNK(uint32x4_t)
vqaddq_u32(uint32x4_t __a, uint32x4_t __b) {
  return __builtin_aarch64_uqaddv4si_uuu(__a, __b);
}

FUNK(uint64x2_t)
vqaddq_u64(uint64x2_t __a, uint64x2_t __b) {
  return __builtin_aarch64_uqaddv2di_uuu(__a, __b);
}

FUNK(int8x8_t)
vqsub_s8(int8x8_t __a, int8x8_t __b) {
  return (int8x8_t)__builtin_aarch64_sqsubv8qi(__a, __b);
}

FUNK(int16x4_t)
vqsub_s16(int16x4_t __a, int16x4_t __b) {
  return (int16x4_t)__builtin_aarch64_sqsubv4hi(__a, __b);
}

FUNK(int32x2_t)
vqsub_s32(int32x2_t __a, int32x2_t __b) {
  return (int32x2_t)__builtin_aarch64_sqsubv2si(__a, __b);
}

FUNK(int64x1_t)
vqsub_s64(int64x1_t __a, int64x1_t __b) {
  return (int64x1_t){__builtin_aarch64_sqsubdi(__a[0], __b[0])};
}

FUNK(uint8x8_t)
vqsub_u8(uint8x8_t __a, uint8x8_t __b) {
  return __builtin_aarch64_uqsubv8qi_uuu(__a, __b);
}

FUNK(uint16x4_t)
vqsub_u16(uint16x4_t __a, uint16x4_t __b) {
  return __builtin_aarch64_uqsubv4hi_uuu(__a, __b);
}

FUNK(uint32x2_t)
vqsub_u32(uint32x2_t __a, uint32x2_t __b) {
  return __builtin_aarch64_uqsubv2si_uuu(__a, __b);
}

FUNK(uint64x1_t)
vqsub_u64(uint64x1_t __a, uint64x1_t __b) {
  return (uint64x1_t){__builtin_aarch64_uqsubdi_uuu(__a[0], __b[0])};
}

FUNK(int8x16_t)
vqsubq_s8(int8x16_t __a, int8x16_t __b) {
  return (int8x16_t)__builtin_aarch64_sqsubv16qi(__a, __b);
}

FUNK(int16x8_t)
vqsubq_s16(int16x8_t __a, int16x8_t __b) {
  return (int16x8_t)__builtin_aarch64_sqsubv8hi(__a, __b);
}

FUNK(int32x4_t)
vqsubq_s32(int32x4_t __a, int32x4_t __b) {
  return (int32x4_t)__builtin_aarch64_sqsubv4si(__a, __b);
}

FUNK(int64x2_t)
vqsubq_s64(int64x2_t __a, int64x2_t __b) {
  return (int64x2_t)__builtin_aarch64_sqsubv2di(__a, __b);
}

FUNK(uint8x16_t)
vqsubq_u8(uint8x16_t __a, uint8x16_t __b) {
  return __builtin_aarch64_uqsubv16qi_uuu(__a, __b);
}

FUNK(uint16x8_t)
vqsubq_u16(uint16x8_t __a, uint16x8_t __b) {
  return __builtin_aarch64_uqsubv8hi_uuu(__a, __b);
}

FUNK(uint32x4_t)
vqsubq_u32(uint32x4_t __a, uint32x4_t __b) {
  return __builtin_aarch64_uqsubv4si_uuu(__a, __b);
}

FUNK(uint64x2_t)
vqsubq_u64(uint64x2_t __a, uint64x2_t __b) {
  return __builtin_aarch64_uqsubv2di_uuu(__a, __b);
}

FUNK(int8x8_t)
vqneg_s8(int8x8_t __a) {
  return (int8x8_t)__builtin_aarch64_sqnegv8qi(__a);
}

FUNK(int16x4_t)
vqneg_s16(int16x4_t __a) {
  return (int16x4_t)__builtin_aarch64_sqnegv4hi(__a);
}

FUNK(int32x2_t)
vqneg_s32(int32x2_t __a) {
  return (int32x2_t)__builtin_aarch64_sqnegv2si(__a);
}

FUNK(int64x1_t)
vqneg_s64(int64x1_t __a) {
  return (int64x1_t){__builtin_aarch64_sqnegdi(__a[0])};
}

FUNK(int8x16_t)
vqnegq_s8(int8x16_t __a) {
  return (int8x16_t)__builtin_aarch64_sqnegv16qi(__a);
}

FUNK(int16x8_t)
vqnegq_s16(int16x8_t __a) {
  return (int16x8_t)__builtin_aarch64_sqnegv8hi(__a);
}

FUNK(int32x4_t)
vqnegq_s32(int32x4_t __a) {
  return (int32x4_t)__builtin_aarch64_sqnegv4si(__a);
}

FUNK(int8x8_t)
vqabs_s8(int8x8_t __a) {
  return (int8x8_t)__builtin_aarch64_sqabsv8qi(__a);
}

FUNK(int16x4_t)
vqabs_s16(int16x4_t __a) {
  return (int16x4_t)__builtin_aarch64_sqabsv4hi(__a);
}

FUNK(int32x2_t)
vqabs_s32(int32x2_t __a) {
  return (int32x2_t)__builtin_aarch64_sqabsv2si(__a);
}

FUNK(int64x1_t)
vqabs_s64(int64x1_t __a) {
  return (int64x1_t){__builtin_aarch64_sqabsdi(__a[0])};
}

FUNK(int8x16_t)
vqabsq_s8(int8x16_t __a) {
  return (int8x16_t)__builtin_aarch64_sqabsv16qi(__a);
}

FUNK(int16x8_t)
vqabsq_s16(int16x8_t __a) {
  return (int16x8_t)__builtin_aarch64_sqabsv8hi(__a);
}

FUNK(int32x4_t)
vqabsq_s32(int32x4_t __a) {
  return (int32x4_t)__builtin_aarch64_sqabsv4si(__a);
}

FUNK(int16x4_t)
vqdmulh_s16(int16x4_t __a, int16x4_t __b) {
  return (int16x4_t)__builtin_aarch64_sqdmulhv4hi(__a, __b);
}

FUNK(int32x2_t)
vqdmulh_s32(int32x2_t __a, int32x2_t __b) {
  return (int32x2_t)__builtin_aarch64_sqdmulhv2si(__a, __b);
}

FUNK(int16x8_t)
vqdmulhq_s16(int16x8_t __a, int16x8_t __b) {
  return (int16x8_t)__builtin_aarch64_sqdmulhv8hi(__a, __b);
}

FUNK(int32x4_t)
vqdmulhq_s32(int32x4_t __a, int32x4_t __b) {
  return (int32x4_t)__builtin_aarch64_sqdmulhv4si(__a, __b);
}

FUNK(int16x4_t)
vqrdmulh_s16(int16x4_t __a, int16x4_t __b) {
  return (int16x4_t)__builtin_aarch64_sqrdmulhv4hi(__a, __b);
}

FUNK(int32x2_t)
vqrdmulh_s32(int32x2_t __a, int32x2_t __b) {
  return (int32x2_t)__builtin_aarch64_sqrdmulhv2si(__a, __b);
}

FUNK(int16x8_t)
vqrdmulhq_s16(int16x8_t __a, int16x8_t __b) {
  return (int16x8_t)__builtin_aarch64_sqrdmulhv8hi(__a, __b);
}

FUNK(int32x4_t)
vqrdmulhq_s32(int32x4_t __a, int32x4_t __b) {
  return (int32x4_t)__builtin_aarch64_sqrdmulhv4si(__a, __b);
}

FUNK(int8x8_t)
vcreate_s8(uint64_t __a) {
  return (int8x8_t)__a;
}

FUNK(int16x4_t)
vcreate_s16(uint64_t __a) {
  return (int16x4_t)__a;
}

FUNK(int32x2_t)
vcreate_s32(uint64_t __a) {
  return (int32x2_t)__a;
}

FUNK(int64x1_t)
vcreate_s64(uint64_t __a) {
  return (int64x1_t){__a};
}

FUNK(float16x4_t)
vcreate_f16(uint64_t __a) {
  return (float16x4_t)__a;
}

FUNK(float32x2_t)
vcreate_f32(uint64_t __a) {
  return (float32x2_t)__a;
}

FUNK(uint8x8_t)
vcreate_u8(uint64_t __a) {
  return (uint8x8_t)__a;
}

FUNK(uint16x4_t)
vcreate_u16(uint64_t __a) {
  return (uint16x4_t)__a;
}

FUNK(uint32x2_t)
vcreate_u32(uint64_t __a) {
  return (uint32x2_t)__a;
}

FUNK(uint64x1_t)
vcreate_u64(uint64_t __a) {
  return (uint64x1_t){__a};
}

FUNK(float64x1_t)
vcreate_f64(uint64_t __a) {
  return (float64x1_t)__a;
}

FUNK(poly8x8_t)
vcreate_p8(uint64_t __a) {
  return (poly8x8_t)__a;
}

FUNK(poly16x4_t)
vcreate_p16(uint64_t __a) {
  return (poly16x4_t)__a;
}

FUNK(poly64x1_t)
vcreate_p64(uint64_t __a) {
  return (poly64x1_t)__a;
}

FUNK(float16_t)
vget_lane_f16(float16x4_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(float32_t)
vget_lane_f32(float32x2_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(float64_t)
vget_lane_f64(float64x1_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(poly8_t)
vget_lane_p8(poly8x8_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(poly16_t)
vget_lane_p16(poly16x4_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(poly64_t)
vget_lane_p64(poly64x1_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(int8_t)
vget_lane_s8(int8x8_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(int16_t)
vget_lane_s16(int16x4_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(int32_t)
vget_lane_s32(int32x2_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(int64_t)
vget_lane_s64(int64x1_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(uint8_t)
vget_lane_u8(uint8x8_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(uint16_t)
vget_lane_u16(uint16x4_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(uint32_t)
vget_lane_u32(uint32x2_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(uint64_t)
vget_lane_u64(uint64x1_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(float16_t)
vgetq_lane_f16(float16x8_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(float32_t)
vgetq_lane_f32(float32x4_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(float64_t)
vgetq_lane_f64(float64x2_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(poly8_t)
vgetq_lane_p8(poly8x16_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(poly16_t)
vgetq_lane_p16(poly16x8_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(poly64_t)
vgetq_lane_p64(poly64x2_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(int8_t)
vgetq_lane_s8(int8x16_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(int16_t)
vgetq_lane_s16(int16x8_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(int32_t)
vgetq_lane_s32(int32x4_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(int64_t)
vgetq_lane_s64(int64x2_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(uint8_t)
vgetq_lane_u8(uint8x16_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(uint16_t)
vgetq_lane_u16(uint16x8_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(uint32_t)
vgetq_lane_u32(uint32x4_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(uint64_t)
vgetq_lane_u64(uint64x2_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(poly8x8_t)
vreinterpret_p8_f16(float16x4_t __a) {
  return (poly8x8_t)__a;
}

FUNK(poly8x8_t)
vreinterpret_p8_f64(float64x1_t __a) {
  return (poly8x8_t)__a;
}

FUNK(poly8x8_t)
vreinterpret_p8_s8(int8x8_t __a) {
  return (poly8x8_t)__a;
}

FUNK(poly8x8_t)
vreinterpret_p8_s16(int16x4_t __a) {
  return (poly8x8_t)__a;
}

FUNK(poly8x8_t)
vreinterpret_p8_s32(int32x2_t __a) {
  return (poly8x8_t)__a;
}

FUNK(poly8x8_t)
vreinterpret_p8_s64(int64x1_t __a) {
  return (poly8x8_t)__a;
}

FUNK(poly8x8_t)
vreinterpret_p8_f32(float32x2_t __a) {
  return (poly8x8_t)__a;
}

FUNK(poly8x8_t)
vreinterpret_p8_u8(uint8x8_t __a) {
  return (poly8x8_t)__a;
}

FUNK(poly8x8_t)
vreinterpret_p8_u16(uint16x4_t __a) {
  return (poly8x8_t)__a;
}

FUNK(poly8x8_t)
vreinterpret_p8_u32(uint32x2_t __a) {
  return (poly8x8_t)__a;
}

FUNK(poly8x8_t)
vreinterpret_p8_u64(uint64x1_t __a) {
  return (poly8x8_t)__a;
}

FUNK(poly8x8_t)
vreinterpret_p8_p16(poly16x4_t __a) {
  return (poly8x8_t)__a;
}

FUNK(poly8x8_t)
vreinterpret_p8_p64(poly64x1_t __a) {
  return (poly8x8_t)__a;
}

FUNK(poly8x16_t)
vreinterpretq_p8_f64(float64x2_t __a) {
  return (poly8x16_t)__a;
}

FUNK(poly8x16_t)
vreinterpretq_p8_s8(int8x16_t __a) {
  return (poly8x16_t)__a;
}

FUNK(poly8x16_t)
vreinterpretq_p8_s16(int16x8_t __a) {
  return (poly8x16_t)__a;
}

FUNK(poly8x16_t)
vreinterpretq_p8_s32(int32x4_t __a) {
  return (poly8x16_t)__a;
}

FUNK(poly8x16_t)
vreinterpretq_p8_s64(int64x2_t __a) {
  return (poly8x16_t)__a;
}

FUNK(poly8x16_t)
vreinterpretq_p8_f16(float16x8_t __a) {
  return (poly8x16_t)__a;
}

FUNK(poly8x16_t)
vreinterpretq_p8_f32(float32x4_t __a) {
  return (poly8x16_t)__a;
}

FUNK(poly8x16_t)
vreinterpretq_p8_u8(uint8x16_t __a) {
  return (poly8x16_t)__a;
}

FUNK(poly8x16_t)
vreinterpretq_p8_u16(uint16x8_t __a) {
  return (poly8x16_t)__a;
}

FUNK(poly8x16_t)
vreinterpretq_p8_u32(uint32x4_t __a) {
  return (poly8x16_t)__a;
}

FUNK(poly8x16_t)
vreinterpretq_p8_u64(uint64x2_t __a) {
  return (poly8x16_t)__a;
}

FUNK(poly8x16_t)
vreinterpretq_p8_p16(poly16x8_t __a) {
  return (poly8x16_t)__a;
}

FUNK(poly8x16_t)
vreinterpretq_p8_p64(poly64x2_t __a) {
  return (poly8x16_t)__a;
}

FUNK(poly8x16_t)
vreinterpretq_p8_p128(poly128_t __a) {
  return (poly8x16_t)__a;
}

FUNK(poly16x4_t)
vreinterpret_p16_f16(float16x4_t __a) {
  return (poly16x4_t)__a;
}

FUNK(poly16x4_t)
vreinterpret_p16_f64(float64x1_t __a) {
  return (poly16x4_t)__a;
}

FUNK(poly16x4_t)
vreinterpret_p16_s8(int8x8_t __a) {
  return (poly16x4_t)__a;
}

FUNK(poly16x4_t)
vreinterpret_p16_s16(int16x4_t __a) {
  return (poly16x4_t)__a;
}

FUNK(poly16x4_t)
vreinterpret_p16_s32(int32x2_t __a) {
  return (poly16x4_t)__a;
}

FUNK(poly16x4_t)
vreinterpret_p16_s64(int64x1_t __a) {
  return (poly16x4_t)__a;
}

FUNK(poly16x4_t)
vreinterpret_p16_f32(float32x2_t __a) {
  return (poly16x4_t)__a;
}

FUNK(poly16x4_t)
vreinterpret_p16_u8(uint8x8_t __a) {
  return (poly16x4_t)__a;
}

FUNK(poly16x4_t)
vreinterpret_p16_u16(uint16x4_t __a) {
  return (poly16x4_t)__a;
}

FUNK(poly16x4_t)
vreinterpret_p16_u32(uint32x2_t __a) {
  return (poly16x4_t)__a;
}

FUNK(poly16x4_t)
vreinterpret_p16_u64(uint64x1_t __a) {
  return (poly16x4_t)__a;
}

FUNK(poly16x4_t)
vreinterpret_p16_p8(poly8x8_t __a) {
  return (poly16x4_t)__a;
}

FUNK(poly16x4_t)
vreinterpret_p16_p64(poly64x1_t __a) {
  return (poly16x4_t)__a;
}

FUNK(poly16x8_t)
vreinterpretq_p16_f64(float64x2_t __a) {
  return (poly16x8_t)__a;
}

FUNK(poly16x8_t)
vreinterpretq_p16_s8(int8x16_t __a) {
  return (poly16x8_t)__a;
}

FUNK(poly16x8_t)
vreinterpretq_p16_s16(int16x8_t __a) {
  return (poly16x8_t)__a;
}

FUNK(poly16x8_t)
vreinterpretq_p16_s32(int32x4_t __a) {
  return (poly16x8_t)__a;
}

FUNK(poly16x8_t)
vreinterpretq_p16_s64(int64x2_t __a) {
  return (poly16x8_t)__a;
}

FUNK(poly16x8_t)
vreinterpretq_p16_f16(float16x8_t __a) {
  return (poly16x8_t)__a;
}

FUNK(poly16x8_t)
vreinterpretq_p16_f32(float32x4_t __a) {
  return (poly16x8_t)__a;
}

FUNK(poly16x8_t)
vreinterpretq_p16_u8(uint8x16_t __a) {
  return (poly16x8_t)__a;
}

FUNK(poly16x8_t)
vreinterpretq_p16_u16(uint16x8_t __a) {
  return (poly16x8_t)__a;
}

FUNK(poly16x8_t)
vreinterpretq_p16_u32(uint32x4_t __a) {
  return (poly16x8_t)__a;
}

FUNK(poly16x8_t)
vreinterpretq_p16_u64(uint64x2_t __a) {
  return (poly16x8_t)__a;
}

FUNK(poly16x8_t)
vreinterpretq_p16_p8(poly8x16_t __a) {
  return (poly16x8_t)__a;
}

FUNK(poly16x8_t)
vreinterpretq_p16_p64(poly64x2_t __a) {
  return (poly16x8_t)__a;
}

FUNK(poly16x8_t)
vreinterpretq_p16_p128(poly128_t __a) {
  return (poly16x8_t)__a;
}

FUNK(poly64x1_t)
vreinterpret_p64_f16(float16x4_t __a) {
  return (poly64x1_t)__a;
}

FUNK(poly64x1_t)
vreinterpret_p64_f64(float64x1_t __a) {
  return (poly64x1_t)__a;
}

FUNK(poly64x1_t)
vreinterpret_p64_s8(int8x8_t __a) {
  return (poly64x1_t)__a;
}

FUNK(poly64x1_t)
vreinterpret_p64_s16(int16x4_t __a) {
  return (poly64x1_t)__a;
}

FUNK(poly64x1_t)
vreinterpret_p64_s32(int32x2_t __a) {
  return (poly64x1_t)__a;
}

FUNK(poly64x1_t)
vreinterpret_p64_s64(int64x1_t __a) {
  return (poly64x1_t)__a;
}

FUNK(poly64x1_t)
vreinterpret_p64_f32(float32x2_t __a) {
  return (poly64x1_t)__a;
}

FUNK(poly64x1_t)
vreinterpret_p64_u8(uint8x8_t __a) {
  return (poly64x1_t)__a;
}

FUNK(poly64x1_t)
vreinterpret_p64_u16(uint16x4_t __a) {
  return (poly64x1_t)__a;
}

FUNK(poly64x1_t)
vreinterpret_p64_u32(uint32x2_t __a) {
  return (poly64x1_t)__a;
}

FUNK(poly64x1_t)
vreinterpret_p64_u64(uint64x1_t __a) {
  return (poly64x1_t)__a;
}

FUNK(poly64x1_t)
vreinterpret_p64_p8(poly8x8_t __a) {
  return (poly64x1_t)__a;
}

FUNK(poly64x1_t)
vreinterpret_p64_p16(poly16x4_t __a) {
  return (poly64x1_t)__a;
}

FUNK(poly64x2_t)
vreinterpretq_p64_f64(float64x2_t __a) {
  return (poly64x2_t)__a;
}

FUNK(poly64x2_t)
vreinterpretq_p64_s8(int8x16_t __a) {
  return (poly64x2_t)__a;
}

FUNK(poly64x2_t)
vreinterpretq_p64_s16(int16x8_t __a) {
  return (poly64x2_t)__a;
}

FUNK(poly64x2_t)
vreinterpretq_p64_s32(int32x4_t __a) {
  return (poly64x2_t)__a;
}

FUNK(poly64x2_t)
vreinterpretq_p64_s64(int64x2_t __a) {
  return (poly64x2_t)__a;
}

FUNK(poly64x2_t)
vreinterpretq_p64_f16(float16x8_t __a) {
  return (poly64x2_t)__a;
}

FUNK(poly64x2_t)
vreinterpretq_p64_f32(float32x4_t __a) {
  return (poly64x2_t)__a;
}

FUNK(poly64x2_t)
vreinterpretq_p64_p128(poly128_t __a) {
  return (poly64x2_t)__a;
}

FUNK(poly64x2_t)
vreinterpretq_p64_u8(uint8x16_t __a) {
  return (poly64x2_t)__a;
}

FUNK(poly64x2_t)
vreinterpretq_p64_u16(uint16x8_t __a) {
  return (poly64x2_t)__a;
}

FUNK(poly64x2_t)
vreinterpretq_p64_p16(poly16x8_t __a) {
  return (poly64x2_t)__a;
}

FUNK(poly64x2_t)
vreinterpretq_p64_u32(uint32x4_t __a) {
  return (poly64x2_t)__a;
}

FUNK(poly64x2_t)
vreinterpretq_p64_u64(uint64x2_t __a) {
  return (poly64x2_t)__a;
}

FUNK(poly64x2_t)
vreinterpretq_p64_p8(poly8x16_t __a) {
  return (poly64x2_t)__a;
}

FUNK(poly128_t)
vreinterpretq_p128_p8(poly8x16_t __a) {
  return (poly128_t)__a;
}

FUNK(poly128_t)
vreinterpretq_p128_p16(poly16x8_t __a) {
  return (poly128_t)__a;
}

FUNK(poly128_t)
vreinterpretq_p128_f16(float16x8_t __a) {
  return (poly128_t)__a;
}

FUNK(poly128_t)
vreinterpretq_p128_f32(float32x4_t __a) {
  return (poly128_t)__a;
}

FUNK(poly128_t)
vreinterpretq_p128_p64(poly64x2_t __a) {
  return (poly128_t)__a;
}

FUNK(poly128_t)
vreinterpretq_p128_s64(int64x2_t __a) {
  return (poly128_t)__a;
}

FUNK(poly128_t)
vreinterpretq_p128_u64(uint64x2_t __a) {
  return (poly128_t)__a;
}

FUNK(poly128_t)
vreinterpretq_p128_s8(int8x16_t __a) {
  return (poly128_t)__a;
}

FUNK(poly128_t)
vreinterpretq_p128_s16(int16x8_t __a) {
  return (poly128_t)__a;
}

FUNK(poly128_t)
vreinterpretq_p128_s32(int32x4_t __a) {
  return (poly128_t)__a;
}

FUNK(poly128_t)
vreinterpretq_p128_u8(uint8x16_t __a) {
  return (poly128_t)__a;
}

FUNK(poly128_t)
vreinterpretq_p128_u16(uint16x8_t __a) {
  return (poly128_t)__a;
}

FUNK(poly128_t)
vreinterpretq_p128_u32(uint32x4_t __a) {
  return (poly128_t)__a;
}

FUNK(float16x4_t)
vreinterpret_f16_f64(float64x1_t __a) {
  return (float16x4_t)__a;
}

FUNK(float16x4_t)
vreinterpret_f16_s8(int8x8_t __a) {
  return (float16x4_t)__a;
}

FUNK(float16x4_t)
vreinterpret_f16_s16(int16x4_t __a) {
  return (float16x4_t)__a;
}

FUNK(float16x4_t)
vreinterpret_f16_s32(int32x2_t __a) {
  return (float16x4_t)__a;
}

FUNK(float16x4_t)
vreinterpret_f16_s64(int64x1_t __a) {
  return (float16x4_t)__a;
}

FUNK(float16x4_t)
vreinterpret_f16_f32(float32x2_t __a) {
  return (float16x4_t)__a;
}

FUNK(float16x4_t)
vreinterpret_f16_u8(uint8x8_t __a) {
  return (float16x4_t)__a;
}

FUNK(float16x4_t)
vreinterpret_f16_u16(uint16x4_t __a) {
  return (float16x4_t)__a;
}

FUNK(float16x4_t)
vreinterpret_f16_u32(uint32x2_t __a) {
  return (float16x4_t)__a;
}

FUNK(float16x4_t)
vreinterpret_f16_u64(uint64x1_t __a) {
  return (float16x4_t)__a;
}

FUNK(float16x4_t)
vreinterpret_f16_p8(poly8x8_t __a) {
  return (float16x4_t)__a;
}

FUNK(float16x4_t)
vreinterpret_f16_p16(poly16x4_t __a) {
  return (float16x4_t)__a;
}

FUNK(float16x4_t)
vreinterpret_f16_p64(poly64x1_t __a) {
  return (float16x4_t)__a;
}

FUNK(float16x8_t)
vreinterpretq_f16_f64(float64x2_t __a) {
  return (float16x8_t)__a;
}

FUNK(float16x8_t)
vreinterpretq_f16_s8(int8x16_t __a) {
  return (float16x8_t)__a;
}

FUNK(float16x8_t)
vreinterpretq_f16_s16(int16x8_t __a) {
  return (float16x8_t)__a;
}

FUNK(float16x8_t)
vreinterpretq_f16_s32(int32x4_t __a) {
  return (float16x8_t)__a;
}

FUNK(float16x8_t)
vreinterpretq_f16_s64(int64x2_t __a) {
  return (float16x8_t)__a;
}

FUNK(float16x8_t)
vreinterpretq_f16_f32(float32x4_t __a) {
  return (float16x8_t)__a;
}

FUNK(float16x8_t)
vreinterpretq_f16_u8(uint8x16_t __a) {
  return (float16x8_t)__a;
}

FUNK(float16x8_t)
vreinterpretq_f16_u16(uint16x8_t __a) {
  return (float16x8_t)__a;
}

FUNK(float16x8_t)
vreinterpretq_f16_u32(uint32x4_t __a) {
  return (float16x8_t)__a;
}

FUNK(float16x8_t)
vreinterpretq_f16_u64(uint64x2_t __a) {
  return (float16x8_t)__a;
}

FUNK(float16x8_t)
vreinterpretq_f16_p8(poly8x16_t __a) {
  return (float16x8_t)__a;
}

FUNK(float16x8_t)
vreinterpretq_f16_p128(poly128_t __a) {
  return (float16x8_t)__a;
}

FUNK(float16x8_t)
vreinterpretq_f16_p16(poly16x8_t __a) {
  return (float16x8_t)__a;
}

FUNK(float16x8_t)
vreinterpretq_f16_p64(poly64x2_t __a) {
  return (float16x8_t)__a;
}

FUNK(float32x2_t)
vreinterpret_f32_f16(float16x4_t __a) {
  return (float32x2_t)__a;
}

FUNK(float32x2_t)
vreinterpret_f32_f64(float64x1_t __a) {
  return (float32x2_t)__a;
}

FUNK(float32x2_t)
vreinterpret_f32_s8(int8x8_t __a) {
  return (float32x2_t)__a;
}

FUNK(float32x2_t)
vreinterpret_f32_s16(int16x4_t __a) {
  return (float32x2_t)__a;
}

FUNK(float32x2_t)
vreinterpret_f32_s32(int32x2_t __a) {
  return (float32x2_t)__a;
}

FUNK(float32x2_t)
vreinterpret_f32_s64(int64x1_t __a) {
  return (float32x2_t)__a;
}

FUNK(float32x2_t)
vreinterpret_f32_u8(uint8x8_t __a) {
  return (float32x2_t)__a;
}

FUNK(float32x2_t)
vreinterpret_f32_u16(uint16x4_t __a) {
  return (float32x2_t)__a;
}

FUNK(float32x2_t)
vreinterpret_f32_u32(uint32x2_t __a) {
  return (float32x2_t)__a;
}

FUNK(float32x2_t)
vreinterpret_f32_u64(uint64x1_t __a) {
  return (float32x2_t)__a;
}

FUNK(float32x2_t)
vreinterpret_f32_p8(poly8x8_t __a) {
  return (float32x2_t)__a;
}

FUNK(float32x2_t)
vreinterpret_f32_p16(poly16x4_t __a) {
  return (float32x2_t)__a;
}

FUNK(float32x2_t)
vreinterpret_f32_p64(poly64x1_t __a) {
  return (float32x2_t)__a;
}

FUNK(float32x4_t)
vreinterpretq_f32_f16(float16x8_t __a) {
  return (float32x4_t)__a;
}

FUNK(float32x4_t)
vreinterpretq_f32_f64(float64x2_t __a) {
  return (float32x4_t)__a;
}

FUNK(float32x4_t)
vreinterpretq_f32_s8(int8x16_t __a) {
  return (float32x4_t)__a;
}

FUNK(float32x4_t)
vreinterpretq_f32_s16(int16x8_t __a) {
  return (float32x4_t)__a;
}

FUNK(float32x4_t)
vreinterpretq_f32_s32(int32x4_t __a) {
  return (float32x4_t)__a;
}

FUNK(float32x4_t)
vreinterpretq_f32_s64(int64x2_t __a) {
  return (float32x4_t)__a;
}

FUNK(float32x4_t)
vreinterpretq_f32_u8(uint8x16_t __a) {
  return (float32x4_t)__a;
}

FUNK(float32x4_t)
vreinterpretq_f32_u16(uint16x8_t __a) {
  return (float32x4_t)__a;
}

FUNK(float32x4_t)
vreinterpretq_f32_u32(uint32x4_t __a) {
  return (float32x4_t)__a;
}

FUNK(float32x4_t)
vreinterpretq_f32_u64(uint64x2_t __a) {
  return (float32x4_t)__a;
}

FUNK(float32x4_t)
vreinterpretq_f32_p8(poly8x16_t __a) {
  return (float32x4_t)__a;
}

FUNK(float32x4_t)
vreinterpretq_f32_p16(poly16x8_t __a) {
  return (float32x4_t)__a;
}

FUNK(float32x4_t)
vreinterpretq_f32_p64(poly64x2_t __a) {
  return (float32x4_t)__a;
}

FUNK(float32x4_t)
vreinterpretq_f32_p128(poly128_t __a) {
  return (float32x4_t)__a;
}

FUNK(float64x1_t)
vreinterpret_f64_f16(float16x4_t __a) {
  return (float64x1_t)__a;
}

FUNK(float64x1_t)
vreinterpret_f64_f32(float32x2_t __a) {
  return (float64x1_t)__a;
}

FUNK(float64x1_t)
vreinterpret_f64_p8(poly8x8_t __a) {
  return (float64x1_t)__a;
}

FUNK(float64x1_t)
vreinterpret_f64_p16(poly16x4_t __a) {
  return (float64x1_t)__a;
}

FUNK(float64x1_t)
vreinterpret_f64_p64(poly64x1_t __a) {
  return (float64x1_t)__a;
}

FUNK(float64x1_t)
vreinterpret_f64_s8(int8x8_t __a) {
  return (float64x1_t)__a;
}

FUNK(float64x1_t)
vreinterpret_f64_s16(int16x4_t __a) {
  return (float64x1_t)__a;
}

FUNK(float64x1_t)
vreinterpret_f64_s32(int32x2_t __a) {
  return (float64x1_t)__a;
}

FUNK(float64x1_t)
vreinterpret_f64_s64(int64x1_t __a) {
  return (float64x1_t)__a;
}

FUNK(float64x1_t)
vreinterpret_f64_u8(uint8x8_t __a) {
  return (float64x1_t)__a;
}

FUNK(float64x1_t)
vreinterpret_f64_u16(uint16x4_t __a) {
  return (float64x1_t)__a;
}

FUNK(float64x1_t)
vreinterpret_f64_u32(uint32x2_t __a) {
  return (float64x1_t)__a;
}

FUNK(float64x1_t)
vreinterpret_f64_u64(uint64x1_t __a) {
  return (float64x1_t)__a;
}

FUNK(float64x2_t)
vreinterpretq_f64_f16(float16x8_t __a) {
  return (float64x2_t)__a;
}

FUNK(float64x2_t)
vreinterpretq_f64_f32(float32x4_t __a) {
  return (float64x2_t)__a;
}

FUNK(float64x2_t)
vreinterpretq_f64_p8(poly8x16_t __a) {
  return (float64x2_t)__a;
}

FUNK(float64x2_t)
vreinterpretq_f64_p16(poly16x8_t __a) {
  return (float64x2_t)__a;
}

FUNK(float64x2_t)
vreinterpretq_f64_p64(poly64x2_t __a) {
  return (float64x2_t)__a;
}

FUNK(float64x2_t)
vreinterpretq_f64_s8(int8x16_t __a) {
  return (float64x2_t)__a;
}

FUNK(float64x2_t)
vreinterpretq_f64_s16(int16x8_t __a) {
  return (float64x2_t)__a;
}

FUNK(float64x2_t)
vreinterpretq_f64_s32(int32x4_t __a) {
  return (float64x2_t)__a;
}

FUNK(float64x2_t)
vreinterpretq_f64_s64(int64x2_t __a) {
  return (float64x2_t)__a;
}

FUNK(float64x2_t)
vreinterpretq_f64_u8(uint8x16_t __a) {
  return (float64x2_t)__a;
}

FUNK(float64x2_t)
vreinterpretq_f64_u16(uint16x8_t __a) {
  return (float64x2_t)__a;
}

FUNK(float64x2_t)
vreinterpretq_f64_u32(uint32x4_t __a) {
  return (float64x2_t)__a;
}

FUNK(float64x2_t)
vreinterpretq_f64_u64(uint64x2_t __a) {
  return (float64x2_t)__a;
}

FUNK(int64x1_t)
vreinterpret_s64_f16(float16x4_t __a) {
  return (int64x1_t)__a;
}

FUNK(int64x1_t)
vreinterpret_s64_f64(float64x1_t __a) {
  return (int64x1_t)__a;
}

FUNK(int64x1_t)
vreinterpret_s64_s8(int8x8_t __a) {
  return (int64x1_t)__a;
}

FUNK(int64x1_t)
vreinterpret_s64_s16(int16x4_t __a) {
  return (int64x1_t)__a;
}

FUNK(int64x1_t)
vreinterpret_s64_s32(int32x2_t __a) {
  return (int64x1_t)__a;
}

FUNK(int64x1_t)
vreinterpret_s64_f32(float32x2_t __a) {
  return (int64x1_t)__a;
}

FUNK(int64x1_t)
vreinterpret_s64_u8(uint8x8_t __a) {
  return (int64x1_t)__a;
}

FUNK(int64x1_t)
vreinterpret_s64_u16(uint16x4_t __a) {
  return (int64x1_t)__a;
}

FUNK(int64x1_t)
vreinterpret_s64_u32(uint32x2_t __a) {
  return (int64x1_t)__a;
}

FUNK(int64x1_t)
vreinterpret_s64_u64(uint64x1_t __a) {
  return (int64x1_t)__a;
}

FUNK(int64x1_t)
vreinterpret_s64_p8(poly8x8_t __a) {
  return (int64x1_t)__a;
}

FUNK(int64x1_t)
vreinterpret_s64_p16(poly16x4_t __a) {
  return (int64x1_t)__a;
}

FUNK(int64x1_t)
vreinterpret_s64_p64(poly64x1_t __a) {
  return (int64x1_t)__a;
}

FUNK(int64x2_t)
vreinterpretq_s64_f64(float64x2_t __a) {
  return (int64x2_t)__a;
}

FUNK(int64x2_t)
vreinterpretq_s64_s8(int8x16_t __a) {
  return (int64x2_t)__a;
}

FUNK(int64x2_t)
vreinterpretq_s64_s16(int16x8_t __a) {
  return (int64x2_t)__a;
}

FUNK(int64x2_t)
vreinterpretq_s64_s32(int32x4_t __a) {
  return (int64x2_t)__a;
}

FUNK(int64x2_t)
vreinterpretq_s64_f16(float16x8_t __a) {
  return (int64x2_t)__a;
}

FUNK(int64x2_t)
vreinterpretq_s64_f32(float32x4_t __a) {
  return (int64x2_t)__a;
}

FUNK(int64x2_t)
vreinterpretq_s64_u8(uint8x16_t __a) {
  return (int64x2_t)__a;
}

FUNK(int64x2_t)
vreinterpretq_s64_u16(uint16x8_t __a) {
  return (int64x2_t)__a;
}

FUNK(int64x2_t)
vreinterpretq_s64_u32(uint32x4_t __a) {
  return (int64x2_t)__a;
}

FUNK(int64x2_t)
vreinterpretq_s64_u64(uint64x2_t __a) {
  return (int64x2_t)__a;
}

FUNK(int64x2_t)
vreinterpretq_s64_p8(poly8x16_t __a) {
  return (int64x2_t)__a;
}

FUNK(int64x2_t)
vreinterpretq_s64_p16(poly16x8_t __a) {
  return (int64x2_t)__a;
}

FUNK(int64x2_t)
vreinterpretq_s64_p64(poly64x2_t __a) {
  return (int64x2_t)__a;
}

FUNK(int64x2_t)
vreinterpretq_s64_p128(poly128_t __a) {
  return (int64x2_t)__a;
}

FUNK(uint64x1_t)
vreinterpret_u64_f16(float16x4_t __a) {
  return (uint64x1_t)__a;
}

FUNK(uint64x1_t)
vreinterpret_u64_f64(float64x1_t __a) {
  return (uint64x1_t)__a;
}

FUNK(uint64x1_t)
vreinterpret_u64_s8(int8x8_t __a) {
  return (uint64x1_t)__a;
}

FUNK(uint64x1_t)
vreinterpret_u64_s16(int16x4_t __a) {
  return (uint64x1_t)__a;
}

FUNK(uint64x1_t)
vreinterpret_u64_s32(int32x2_t __a) {
  return (uint64x1_t)__a;
}

FUNK(uint64x1_t)
vreinterpret_u64_s64(int64x1_t __a) {
  return (uint64x1_t)__a;
}

FUNK(uint64x1_t)
vreinterpret_u64_f32(float32x2_t __a) {
  return (uint64x1_t)__a;
}

FUNK(uint64x1_t)
vreinterpret_u64_u8(uint8x8_t __a) {
  return (uint64x1_t)__a;
}

FUNK(uint64x1_t)
vreinterpret_u64_u16(uint16x4_t __a) {
  return (uint64x1_t)__a;
}

FUNK(uint64x1_t)
vreinterpret_u64_u32(uint32x2_t __a) {
  return (uint64x1_t)__a;
}

FUNK(uint64x1_t)
vreinterpret_u64_p8(poly8x8_t __a) {
  return (uint64x1_t)__a;
}

FUNK(uint64x1_t)
vreinterpret_u64_p16(poly16x4_t __a) {
  return (uint64x1_t)__a;
}

FUNK(uint64x1_t)
vreinterpret_u64_p64(poly64x1_t __a) {
  return (uint64x1_t)__a;
}

FUNK(uint64x2_t)
vreinterpretq_u64_f64(float64x2_t __a) {
  return (uint64x2_t)__a;
}

FUNK(uint64x2_t)
vreinterpretq_u64_s8(int8x16_t __a) {
  return (uint64x2_t)__a;
}

FUNK(uint64x2_t)
vreinterpretq_u64_s16(int16x8_t __a) {
  return (uint64x2_t)__a;
}

FUNK(uint64x2_t)
vreinterpretq_u64_s32(int32x4_t __a) {
  return (uint64x2_t)__a;
}

FUNK(uint64x2_t)
vreinterpretq_u64_s64(int64x2_t __a) {
  return (uint64x2_t)__a;
}

FUNK(uint64x2_t)
vreinterpretq_u64_f16(float16x8_t __a) {
  return (uint64x2_t)__a;
}

FUNK(uint64x2_t)
vreinterpretq_u64_f32(float32x4_t __a) {
  return (uint64x2_t)__a;
}

FUNK(uint64x2_t)
vreinterpretq_u64_u8(uint8x16_t __a) {
  return (uint64x2_t)__a;
}

FUNK(uint64x2_t)
vreinterpretq_u64_u16(uint16x8_t __a) {
  return (uint64x2_t)__a;
}

FUNK(uint64x2_t)
vreinterpretq_u64_u32(uint32x4_t __a) {
  return (uint64x2_t)__a;
}

FUNK(uint64x2_t)
vreinterpretq_u64_p8(poly8x16_t __a) {
  return (uint64x2_t)__a;
}

FUNK(uint64x2_t)
vreinterpretq_u64_p16(poly16x8_t __a) {
  return (uint64x2_t)__a;
}

FUNK(uint64x2_t)
vreinterpretq_u64_p64(poly64x2_t __a) {
  return (uint64x2_t)__a;
}

FUNK(uint64x2_t)
vreinterpretq_u64_p128(poly128_t __a) {
  return (uint64x2_t)__a;
}

FUNK(int8x8_t)
vreinterpret_s8_f16(float16x4_t __a) {
  return (int8x8_t)__a;
}

FUNK(int8x8_t)
vreinterpret_s8_f64(float64x1_t __a) {
  return (int8x8_t)__a;
}

FUNK(int8x8_t)
vreinterpret_s8_s16(int16x4_t __a) {
  return (int8x8_t)__a;
}

FUNK(int8x8_t)
vreinterpret_s8_s32(int32x2_t __a) {
  return (int8x8_t)__a;
}

FUNK(int8x8_t)
vreinterpret_s8_s64(int64x1_t __a) {
  return (int8x8_t)__a;
}

FUNK(int8x8_t)
vreinterpret_s8_f32(float32x2_t __a) {
  return (int8x8_t)__a;
}

FUNK(int8x8_t)
vreinterpret_s8_u8(uint8x8_t __a) {
  return (int8x8_t)__a;
}

FUNK(int8x8_t)
vreinterpret_s8_u16(uint16x4_t __a) {
  return (int8x8_t)__a;
}

FUNK(int8x8_t)
vreinterpret_s8_u32(uint32x2_t __a) {
  return (int8x8_t)__a;
}

FUNK(int8x8_t)
vreinterpret_s8_u64(uint64x1_t __a) {
  return (int8x8_t)__a;
}

FUNK(int8x8_t)
vreinterpret_s8_p8(poly8x8_t __a) {
  return (int8x8_t)__a;
}

FUNK(int8x8_t)
vreinterpret_s8_p16(poly16x4_t __a) {
  return (int8x8_t)__a;
}

FUNK(int8x8_t)
vreinterpret_s8_p64(poly64x1_t __a) {
  return (int8x8_t)__a;
}

FUNK(int8x16_t)
vreinterpretq_s8_f64(float64x2_t __a) {
  return (int8x16_t)__a;
}

FUNK(int8x16_t)
vreinterpretq_s8_s16(int16x8_t __a) {
  return (int8x16_t)__a;
}

FUNK(int8x16_t)
vreinterpretq_s8_s32(int32x4_t __a) {
  return (int8x16_t)__a;
}

FUNK(int8x16_t)
vreinterpretq_s8_s64(int64x2_t __a) {
  return (int8x16_t)__a;
}

FUNK(int8x16_t)
vreinterpretq_s8_f16(float16x8_t __a) {
  return (int8x16_t)__a;
}

FUNK(int8x16_t)
vreinterpretq_s8_f32(float32x4_t __a) {
  return (int8x16_t)__a;
}

FUNK(int8x16_t)
vreinterpretq_s8_u8(uint8x16_t __a) {
  return (int8x16_t)__a;
}

FUNK(int8x16_t)
vreinterpretq_s8_u16(uint16x8_t __a) {
  return (int8x16_t)__a;
}

FUNK(int8x16_t)
vreinterpretq_s8_u32(uint32x4_t __a) {
  return (int8x16_t)__a;
}

FUNK(int8x16_t)
vreinterpretq_s8_u64(uint64x2_t __a) {
  return (int8x16_t)__a;
}

FUNK(int8x16_t)
vreinterpretq_s8_p8(poly8x16_t __a) {
  return (int8x16_t)__a;
}

FUNK(int8x16_t)
vreinterpretq_s8_p16(poly16x8_t __a) {
  return (int8x16_t)__a;
}

FUNK(int8x16_t)
vreinterpretq_s8_p64(poly64x2_t __a) {
  return (int8x16_t)__a;
}

FUNK(int8x16_t)
vreinterpretq_s8_p128(poly128_t __a) {
  return (int8x16_t)__a;
}

FUNK(int16x4_t)
vreinterpret_s16_f16(float16x4_t __a) {
  return (int16x4_t)__a;
}

FUNK(int16x4_t)
vreinterpret_s16_f64(float64x1_t __a) {
  return (int16x4_t)__a;
}

FUNK(int16x4_t)
vreinterpret_s16_s8(int8x8_t __a) {
  return (int16x4_t)__a;
}

FUNK(int16x4_t)
vreinterpret_s16_s32(int32x2_t __a) {
  return (int16x4_t)__a;
}

FUNK(int16x4_t)
vreinterpret_s16_s64(int64x1_t __a) {
  return (int16x4_t)__a;
}

FUNK(int16x4_t)
vreinterpret_s16_f32(float32x2_t __a) {
  return (int16x4_t)__a;
}

FUNK(int16x4_t)
vreinterpret_s16_u8(uint8x8_t __a) {
  return (int16x4_t)__a;
}

FUNK(int16x4_t)
vreinterpret_s16_u16(uint16x4_t __a) {
  return (int16x4_t)__a;
}

FUNK(int16x4_t)
vreinterpret_s16_u32(uint32x2_t __a) {
  return (int16x4_t)__a;
}

FUNK(int16x4_t)
vreinterpret_s16_u64(uint64x1_t __a) {
  return (int16x4_t)__a;
}

FUNK(int16x4_t)
vreinterpret_s16_p8(poly8x8_t __a) {
  return (int16x4_t)__a;
}

FUNK(int16x4_t)
vreinterpret_s16_p16(poly16x4_t __a) {
  return (int16x4_t)__a;
}

FUNK(int16x4_t)
vreinterpret_s16_p64(poly64x1_t __a) {
  return (int16x4_t)__a;
}

FUNK(int16x8_t)
vreinterpretq_s16_f64(float64x2_t __a) {
  return (int16x8_t)__a;
}

FUNK(int16x8_t)
vreinterpretq_s16_s8(int8x16_t __a) {
  return (int16x8_t)__a;
}

FUNK(int16x8_t)
vreinterpretq_s16_s32(int32x4_t __a) {
  return (int16x8_t)__a;
}

FUNK(int16x8_t)
vreinterpretq_s16_s64(int64x2_t __a) {
  return (int16x8_t)__a;
}

FUNK(int16x8_t)
vreinterpretq_s16_f16(float16x8_t __a) {
  return (int16x8_t)__a;
}

FUNK(int16x8_t)
vreinterpretq_s16_f32(float32x4_t __a) {
  return (int16x8_t)__a;
}

FUNK(int16x8_t)
vreinterpretq_s16_u8(uint8x16_t __a) {
  return (int16x8_t)__a;
}

FUNK(int16x8_t)
vreinterpretq_s16_u16(uint16x8_t __a) {
  return (int16x8_t)__a;
}

FUNK(int16x8_t)
vreinterpretq_s16_u32(uint32x4_t __a) {
  return (int16x8_t)__a;
}

FUNK(int16x8_t)
vreinterpretq_s16_u64(uint64x2_t __a) {
  return (int16x8_t)__a;
}

FUNK(int16x8_t)
vreinterpretq_s16_p8(poly8x16_t __a) {
  return (int16x8_t)__a;
}

FUNK(int16x8_t)
vreinterpretq_s16_p16(poly16x8_t __a) {
  return (int16x8_t)__a;
}

FUNK(int16x8_t)
vreinterpretq_s16_p64(poly64x2_t __a) {
  return (int16x8_t)__a;
}

FUNK(int16x8_t)
vreinterpretq_s16_p128(poly128_t __a) {
  return (int16x8_t)__a;
}

FUNK(int32x2_t)
vreinterpret_s32_f16(float16x4_t __a) {
  return (int32x2_t)__a;
}

FUNK(int32x2_t)
vreinterpret_s32_f64(float64x1_t __a) {
  return (int32x2_t)__a;
}

FUNK(int32x2_t)
vreinterpret_s32_s8(int8x8_t __a) {
  return (int32x2_t)__a;
}

FUNK(int32x2_t)
vreinterpret_s32_s16(int16x4_t __a) {
  return (int32x2_t)__a;
}

FUNK(int32x2_t)
vreinterpret_s32_s64(int64x1_t __a) {
  return (int32x2_t)__a;
}

FUNK(int32x2_t)
vreinterpret_s32_f32(float32x2_t __a) {
  return (int32x2_t)__a;
}

FUNK(int32x2_t)
vreinterpret_s32_u8(uint8x8_t __a) {
  return (int32x2_t)__a;
}

FUNK(int32x2_t)
vreinterpret_s32_u16(uint16x4_t __a) {
  return (int32x2_t)__a;
}

FUNK(int32x2_t)
vreinterpret_s32_u32(uint32x2_t __a) {
  return (int32x2_t)__a;
}

FUNK(int32x2_t)
vreinterpret_s32_u64(uint64x1_t __a) {
  return (int32x2_t)__a;
}

FUNK(int32x2_t)
vreinterpret_s32_p8(poly8x8_t __a) {
  return (int32x2_t)__a;
}

FUNK(int32x2_t)
vreinterpret_s32_p16(poly16x4_t __a) {
  return (int32x2_t)__a;
}

FUNK(int32x2_t)
vreinterpret_s32_p64(poly64x1_t __a) {
  return (int32x2_t)__a;
}

FUNK(int32x4_t)
vreinterpretq_s32_f64(float64x2_t __a) {
  return (int32x4_t)__a;
}

FUNK(int32x4_t)
vreinterpretq_s32_s8(int8x16_t __a) {
  return (int32x4_t)__a;
}

FUNK(int32x4_t)
vreinterpretq_s32_s16(int16x8_t __a) {
  return (int32x4_t)__a;
}

FUNK(int32x4_t)
vreinterpretq_s32_s64(int64x2_t __a) {
  return (int32x4_t)__a;
}

FUNK(int32x4_t)
vreinterpretq_s32_f16(float16x8_t __a) {
  return (int32x4_t)__a;
}

FUNK(int32x4_t)
vreinterpretq_s32_f32(float32x4_t __a) {
  return (int32x4_t)__a;
}

FUNK(int32x4_t)
vreinterpretq_s32_u8(uint8x16_t __a) {
  return (int32x4_t)__a;
}

FUNK(int32x4_t)
vreinterpretq_s32_u16(uint16x8_t __a) {
  return (int32x4_t)__a;
}

FUNK(int32x4_t)
vreinterpretq_s32_u32(uint32x4_t __a) {
  return (int32x4_t)__a;
}

FUNK(int32x4_t)
vreinterpretq_s32_u64(uint64x2_t __a) {
  return (int32x4_t)__a;
}

FUNK(int32x4_t)
vreinterpretq_s32_p8(poly8x16_t __a) {
  return (int32x4_t)__a;
}

FUNK(int32x4_t)
vreinterpretq_s32_p16(poly16x8_t __a) {
  return (int32x4_t)__a;
}

FUNK(int32x4_t)
vreinterpretq_s32_p64(poly64x2_t __a) {
  return (int32x4_t)__a;
}

FUNK(int32x4_t)
vreinterpretq_s32_p128(poly128_t __a) {
  return (int32x4_t)__a;
}

FUNK(uint8x8_t)
vreinterpret_u8_f16(float16x4_t __a) {
  return (uint8x8_t)__a;
}

FUNK(uint8x8_t)
vreinterpret_u8_f64(float64x1_t __a) {
  return (uint8x8_t)__a;
}

FUNK(uint8x8_t)
vreinterpret_u8_s8(int8x8_t __a) {
  return (uint8x8_t)__a;
}

FUNK(uint8x8_t)
vreinterpret_u8_s16(int16x4_t __a) {
  return (uint8x8_t)__a;
}

FUNK(uint8x8_t)
vreinterpret_u8_s32(int32x2_t __a) {
  return (uint8x8_t)__a;
}

FUNK(uint8x8_t)
vreinterpret_u8_s64(int64x1_t __a) {
  return (uint8x8_t)__a;
}

FUNK(uint8x8_t)
vreinterpret_u8_f32(float32x2_t __a) {
  return (uint8x8_t)__a;
}

FUNK(uint8x8_t)
vreinterpret_u8_u16(uint16x4_t __a) {
  return (uint8x8_t)__a;
}

FUNK(uint8x8_t)
vreinterpret_u8_u32(uint32x2_t __a) {
  return (uint8x8_t)__a;
}

FUNK(uint8x8_t)
vreinterpret_u8_u64(uint64x1_t __a) {
  return (uint8x8_t)__a;
}

FUNK(uint8x8_t)
vreinterpret_u8_p8(poly8x8_t __a) {
  return (uint8x8_t)__a;
}

FUNK(uint8x8_t)
vreinterpret_u8_p16(poly16x4_t __a) {
  return (uint8x8_t)__a;
}

FUNK(uint8x8_t)
vreinterpret_u8_p64(poly64x1_t __a) {
  return (uint8x8_t)__a;
}

FUNK(uint8x16_t)
vreinterpretq_u8_f64(float64x2_t __a) {
  return (uint8x16_t)__a;
}

FUNK(uint8x16_t)
vreinterpretq_u8_s8(int8x16_t __a) {
  return (uint8x16_t)__a;
}

FUNK(uint8x16_t)
vreinterpretq_u8_s16(int16x8_t __a) {
  return (uint8x16_t)__a;
}

FUNK(uint8x16_t)
vreinterpretq_u8_s32(int32x4_t __a) {
  return (uint8x16_t)__a;
}

FUNK(uint8x16_t)
vreinterpretq_u8_s64(int64x2_t __a) {
  return (uint8x16_t)__a;
}

FUNK(uint8x16_t)
vreinterpretq_u8_f16(float16x8_t __a) {
  return (uint8x16_t)__a;
}

FUNK(uint8x16_t)
vreinterpretq_u8_f32(float32x4_t __a) {
  return (uint8x16_t)__a;
}

FUNK(uint8x16_t)
vreinterpretq_u8_u16(uint16x8_t __a) {
  return (uint8x16_t)__a;
}

FUNK(uint8x16_t)
vreinterpretq_u8_u32(uint32x4_t __a) {
  return (uint8x16_t)__a;
}

FUNK(uint8x16_t)
vreinterpretq_u8_u64(uint64x2_t __a) {
  return (uint8x16_t)__a;
}

FUNK(uint8x16_t)
vreinterpretq_u8_p8(poly8x16_t __a) {
  return (uint8x16_t)__a;
}

FUNK(uint8x16_t)
vreinterpretq_u8_p16(poly16x8_t __a) {
  return (uint8x16_t)__a;
}

FUNK(uint8x16_t)
vreinterpretq_u8_p64(poly64x2_t __a) {
  return (uint8x16_t)__a;
}

FUNK(uint8x16_t)
vreinterpretq_u8_p128(poly128_t __a) {
  return (uint8x16_t)__a;
}

FUNK(uint16x4_t)
vreinterpret_u16_f16(float16x4_t __a) {
  return (uint16x4_t)__a;
}

FUNK(uint16x4_t)
vreinterpret_u16_f64(float64x1_t __a) {
  return (uint16x4_t)__a;
}

FUNK(uint16x4_t)
vreinterpret_u16_s8(int8x8_t __a) {
  return (uint16x4_t)__a;
}

FUNK(uint16x4_t)
vreinterpret_u16_s16(int16x4_t __a) {
  return (uint16x4_t)__a;
}

FUNK(uint16x4_t)
vreinterpret_u16_s32(int32x2_t __a) {
  return (uint16x4_t)__a;
}

FUNK(uint16x4_t)
vreinterpret_u16_s64(int64x1_t __a) {
  return (uint16x4_t)__a;
}

FUNK(uint16x4_t)
vreinterpret_u16_f32(float32x2_t __a) {
  return (uint16x4_t)__a;
}

FUNK(uint16x4_t)
vreinterpret_u16_u8(uint8x8_t __a) {
  return (uint16x4_t)__a;
}

FUNK(uint16x4_t)
vreinterpret_u16_u32(uint32x2_t __a) {
  return (uint16x4_t)__a;
}

FUNK(uint16x4_t)
vreinterpret_u16_u64(uint64x1_t __a) {
  return (uint16x4_t)__a;
}

FUNK(uint16x4_t)
vreinterpret_u16_p8(poly8x8_t __a) {
  return (uint16x4_t)__a;
}

FUNK(uint16x4_t)
vreinterpret_u16_p16(poly16x4_t __a) {
  return (uint16x4_t)__a;
}

FUNK(uint16x4_t)
vreinterpret_u16_p64(poly64x1_t __a) {
  return (uint16x4_t)__a;
}

FUNK(uint16x8_t)
vreinterpretq_u16_f64(float64x2_t __a) {
  return (uint16x8_t)__a;
}

FUNK(uint16x8_t)
vreinterpretq_u16_s8(int8x16_t __a) {
  return (uint16x8_t)__a;
}

FUNK(uint16x8_t)
vreinterpretq_u16_s16(int16x8_t __a) {
  return (uint16x8_t)__a;
}

FUNK(uint16x8_t)
vreinterpretq_u16_s32(int32x4_t __a) {
  return (uint16x8_t)__a;
}

FUNK(uint16x8_t)
vreinterpretq_u16_s64(int64x2_t __a) {
  return (uint16x8_t)__a;
}

FUNK(uint16x8_t)
vreinterpretq_u16_f16(float16x8_t __a) {
  return (uint16x8_t)__a;
}

FUNK(uint16x8_t)
vreinterpretq_u16_f32(float32x4_t __a) {
  return (uint16x8_t)__a;
}

FUNK(uint16x8_t)
vreinterpretq_u16_u8(uint8x16_t __a) {
  return (uint16x8_t)__a;
}

FUNK(uint16x8_t)
vreinterpretq_u16_u32(uint32x4_t __a) {
  return (uint16x8_t)__a;
}

FUNK(uint16x8_t)
vreinterpretq_u16_u64(uint64x2_t __a) {
  return (uint16x8_t)__a;
}

FUNK(uint16x8_t)
vreinterpretq_u16_p8(poly8x16_t __a) {
  return (uint16x8_t)__a;
}

FUNK(uint16x8_t)
vreinterpretq_u16_p16(poly16x8_t __a) {
  return (uint16x8_t)__a;
}

FUNK(uint16x8_t)
vreinterpretq_u16_p64(poly64x2_t __a) {
  return (uint16x8_t)__a;
}

FUNK(uint16x8_t)
vreinterpretq_u16_p128(poly128_t __a) {
  return (uint16x8_t)__a;
}

FUNK(uint32x2_t)
vreinterpret_u32_f16(float16x4_t __a) {
  return (uint32x2_t)__a;
}

FUNK(uint32x2_t)
vreinterpret_u32_f64(float64x1_t __a) {
  return (uint32x2_t)__a;
}

FUNK(uint32x2_t)
vreinterpret_u32_s8(int8x8_t __a) {
  return (uint32x2_t)__a;
}

FUNK(uint32x2_t)
vreinterpret_u32_s16(int16x4_t __a) {
  return (uint32x2_t)__a;
}

FUNK(uint32x2_t)
vreinterpret_u32_s32(int32x2_t __a) {
  return (uint32x2_t)__a;
}

FUNK(uint32x2_t)
vreinterpret_u32_s64(int64x1_t __a) {
  return (uint32x2_t)__a;
}

FUNK(uint32x2_t)
vreinterpret_u32_f32(float32x2_t __a) {
  return (uint32x2_t)__a;
}

FUNK(uint32x2_t)
vreinterpret_u32_u8(uint8x8_t __a) {
  return (uint32x2_t)__a;
}

FUNK(uint32x2_t)
vreinterpret_u32_u16(uint16x4_t __a) {
  return (uint32x2_t)__a;
}

FUNK(uint32x2_t)
vreinterpret_u32_u64(uint64x1_t __a) {
  return (uint32x2_t)__a;
}

FUNK(uint32x2_t)
vreinterpret_u32_p8(poly8x8_t __a) {
  return (uint32x2_t)__a;
}

FUNK(uint32x2_t)
vreinterpret_u32_p16(poly16x4_t __a) {
  return (uint32x2_t)__a;
}

FUNK(uint32x2_t)
vreinterpret_u32_p64(poly64x1_t __a) {
  return (uint32x2_t)__a;
}

FUNK(uint32x4_t)
vreinterpretq_u32_f64(float64x2_t __a) {
  return (uint32x4_t)__a;
}

FUNK(uint32x4_t)
vreinterpretq_u32_s8(int8x16_t __a) {
  return (uint32x4_t)__a;
}

FUNK(uint32x4_t)
vreinterpretq_u32_s16(int16x8_t __a) {
  return (uint32x4_t)__a;
}

FUNK(uint32x4_t)
vreinterpretq_u32_s32(int32x4_t __a) {
  return (uint32x4_t)__a;
}

FUNK(uint32x4_t)
vreinterpretq_u32_s64(int64x2_t __a) {
  return (uint32x4_t)__a;
}

FUNK(uint32x4_t)
vreinterpretq_u32_f16(float16x8_t __a) {
  return (uint32x4_t)__a;
}

FUNK(uint32x4_t)
vreinterpretq_u32_f32(float32x4_t __a) {
  return (uint32x4_t)__a;
}

FUNK(uint32x4_t)
vreinterpretq_u32_u8(uint8x16_t __a) {
  return (uint32x4_t)__a;
}

FUNK(uint32x4_t)
vreinterpretq_u32_u16(uint16x8_t __a) {
  return (uint32x4_t)__a;
}

FUNK(uint32x4_t)
vreinterpretq_u32_u64(uint64x2_t __a) {
  return (uint32x4_t)__a;
}

FUNK(uint32x4_t)
vreinterpretq_u32_p8(poly8x16_t __a) {
  return (uint32x4_t)__a;
}

FUNK(uint32x4_t)
vreinterpretq_u32_p16(poly16x8_t __a) {
  return (uint32x4_t)__a;
}

FUNK(uint32x4_t)
vreinterpretq_u32_p64(poly64x2_t __a) {
  return (uint32x4_t)__a;
}

FUNK(uint32x4_t)
vreinterpretq_u32_p128(poly128_t __a) {
  return (uint32x4_t)__a;
}

FUNK(float16x4_t)
vset_lane_f16(float16_t __elem, float16x4_t __vec, const int __index) {
  return __aarch64_vset_lane_any(__elem, __vec, __index);
}

FUNK(float32x2_t)
vset_lane_f32(float32_t __elem, float32x2_t __vec, const int __index) {
  return __aarch64_vset_lane_any(__elem, __vec, __index);
}

FUNK(float64x1_t)
vset_lane_f64(float64_t __elem, float64x1_t __vec, const int __index) {
  return __aarch64_vset_lane_any(__elem, __vec, __index);
}

FUNK(poly8x8_t)
vset_lane_p8(poly8_t __elem, poly8x8_t __vec, const int __index) {
  return __aarch64_vset_lane_any(__elem, __vec, __index);
}

FUNK(poly16x4_t)
vset_lane_p16(poly16_t __elem, poly16x4_t __vec, const int __index) {
  return __aarch64_vset_lane_any(__elem, __vec, __index);
}

FUNK(poly64x1_t)
vset_lane_p64(poly64_t __elem, poly64x1_t __vec, const int __index) {
  return __aarch64_vset_lane_any(__elem, __vec, __index);
}

FUNK(int8x8_t)
vset_lane_s8(int8_t __elem, int8x8_t __vec, const int __index) {
  return __aarch64_vset_lane_any(__elem, __vec, __index);
}

FUNK(int16x4_t)
vset_lane_s16(int16_t __elem, int16x4_t __vec, const int __index) {
  return __aarch64_vset_lane_any(__elem, __vec, __index);
}

FUNK(int32x2_t)
vset_lane_s32(int32_t __elem, int32x2_t __vec, const int __index) {
  return __aarch64_vset_lane_any(__elem, __vec, __index);
}

FUNK(int64x1_t)
vset_lane_s64(int64_t __elem, int64x1_t __vec, const int __index) {
  return __aarch64_vset_lane_any(__elem, __vec, __index);
}

FUNK(uint8x8_t)
vset_lane_u8(uint8_t __elem, uint8x8_t __vec, const int __index) {
  return __aarch64_vset_lane_any(__elem, __vec, __index);
}

FUNK(uint16x4_t)
vset_lane_u16(uint16_t __elem, uint16x4_t __vec, const int __index) {
  return __aarch64_vset_lane_any(__elem, __vec, __index);
}

FUNK(uint32x2_t)
vset_lane_u32(uint32_t __elem, uint32x2_t __vec, const int __index) {
  return __aarch64_vset_lane_any(__elem, __vec, __index);
}

FUNK(uint64x1_t)
vset_lane_u64(uint64_t __elem, uint64x1_t __vec, const int __index) {
  return __aarch64_vset_lane_any(__elem, __vec, __index);
}

FUNK(float16x8_t)
vsetq_lane_f16(float16_t __elem, float16x8_t __vec, const int __index) {
  return __aarch64_vset_lane_any(__elem, __vec, __index);
}

FUNK(float32x4_t)
vsetq_lane_f32(float32_t __elem, float32x4_t __vec, const int __index) {
  return __aarch64_vset_lane_any(__elem, __vec, __index);
}

FUNK(float64x2_t)
vsetq_lane_f64(float64_t __elem, float64x2_t __vec, const int __index) {
  return __aarch64_vset_lane_any(__elem, __vec, __index);
}

FUNK(poly8x16_t)
vsetq_lane_p8(poly8_t __elem, poly8x16_t __vec, const int __index) {
  return __aarch64_vset_lane_any(__elem, __vec, __index);
}

FUNK(poly16x8_t)
vsetq_lane_p16(poly16_t __elem, poly16x8_t __vec, const int __index) {
  return __aarch64_vset_lane_any(__elem, __vec, __index);
}

FUNK(poly64x2_t)
vsetq_lane_p64(poly64_t __elem, poly64x2_t __vec, const int __index) {
  return __aarch64_vset_lane_any(__elem, __vec, __index);
}

FUNK(int8x16_t)
vsetq_lane_s8(int8_t __elem, int8x16_t __vec, const int __index) {
  return __aarch64_vset_lane_any(__elem, __vec, __index);
}

FUNK(int16x8_t)
vsetq_lane_s16(int16_t __elem, int16x8_t __vec, const int __index) {
  return __aarch64_vset_lane_any(__elem, __vec, __index);
}

FUNK(int32x4_t)
vsetq_lane_s32(int32_t __elem, int32x4_t __vec, const int __index) {
  return __aarch64_vset_lane_any(__elem, __vec, __index);
}

FUNK(int64x2_t)
vsetq_lane_s64(int64_t __elem, int64x2_t __vec, const int __index) {
  return __aarch64_vset_lane_any(__elem, __vec, __index);
}

FUNK(uint8x16_t)
vsetq_lane_u8(uint8_t __elem, uint8x16_t __vec, const int __index) {
  return __aarch64_vset_lane_any(__elem, __vec, __index);
}

FUNK(uint16x8_t)
vsetq_lane_u16(uint16_t __elem, uint16x8_t __vec, const int __index) {
  return __aarch64_vset_lane_any(__elem, __vec, __index);
}

FUNK(uint32x4_t)
vsetq_lane_u32(uint32_t __elem, uint32x4_t __vec, const int __index) {
  return __aarch64_vset_lane_any(__elem, __vec, __index);
}

FUNK(uint64x2_t)
vsetq_lane_u64(uint64_t __elem, uint64x2_t __vec, const int __index) {
  return __aarch64_vset_lane_any(__elem, __vec, __index);
}

#define __GET_LOW(__TYPE)                              \
  uint64x2_t tmp = vreinterpretq_u64_##__TYPE(__a);    \
  uint64x1_t lo = vcreate_u64(vgetq_lane_u64(tmp, 0)); \
  return vreinterpret_##__TYPE##_u64(lo);

FUNK(float16x4_t)
vget_low_f16(float16x8_t __a) {
  __GET_LOW(f16);
}

FUNK(float32x2_t)
vget_low_f32(float32x4_t __a) {
  __GET_LOW(f32);
}

FUNK(float64x1_t)
vget_low_f64(float64x2_t __a) {
  return (float64x1_t){vgetq_lane_f64(__a, 0)};
}

FUNK(poly8x8_t)
vget_low_p8(poly8x16_t __a) {
  __GET_LOW(p8);
}

FUNK(poly16x4_t)
vget_low_p16(poly16x8_t __a) {
  __GET_LOW(p16);
}

FUNK(poly64x1_t)
vget_low_p64(poly64x2_t __a) {
  __GET_LOW(p64);
}

FUNK(int8x8_t)
vget_low_s8(int8x16_t __a) {
  __GET_LOW(s8);
}

FUNK(int16x4_t)
vget_low_s16(int16x8_t __a) {
  __GET_LOW(s16);
}

FUNK(int32x2_t)
vget_low_s32(int32x4_t __a) {
  __GET_LOW(s32);
}

FUNK(int64x1_t)
vget_low_s64(int64x2_t __a) {
  __GET_LOW(s64);
}

FUNK(uint8x8_t)
vget_low_u8(uint8x16_t __a) {
  __GET_LOW(u8);
}

FUNK(uint16x4_t)
vget_low_u16(uint16x8_t __a) {
  __GET_LOW(u16);
}

FUNK(uint32x2_t)
vget_low_u32(uint32x4_t __a) {
  __GET_LOW(u32);
}

FUNK(uint64x1_t)
vget_low_u64(uint64x2_t __a) {
  return vcreate_u64(vgetq_lane_u64(__a, 0));
}

#undef __GET_LOW

#define __GET_HIGH(__TYPE)                             \
  uint64x2_t tmp = vreinterpretq_u64_##__TYPE(__a);    \
  uint64x1_t hi = vcreate_u64(vgetq_lane_u64(tmp, 1)); \
  return vreinterpret_##__TYPE##_u64(hi);

FUNK(float16x4_t)
vget_high_f16(float16x8_t __a) {
  __GET_HIGH(f16);
}

FUNK(float32x2_t)
vget_high_f32(float32x4_t __a) {
  __GET_HIGH(f32);
}

FUNK(float64x1_t)
vget_high_f64(float64x2_t __a) {
  __GET_HIGH(f64);
}

FUNK(poly8x8_t)
vget_high_p8(poly8x16_t __a) {
  __GET_HIGH(p8);
}

FUNK(poly16x4_t)
vget_high_p16(poly16x8_t __a) {
  __GET_HIGH(p16);
}

FUNK(poly64x1_t)
vget_high_p64(poly64x2_t __a) {
  __GET_HIGH(p64);
}

FUNK(int8x8_t)
vget_high_s8(int8x16_t __a) {
  __GET_HIGH(s8);
}

FUNK(int16x4_t)
vget_high_s16(int16x8_t __a) {
  __GET_HIGH(s16);
}

FUNK(int32x2_t)
vget_high_s32(int32x4_t __a) {
  __GET_HIGH(s32);
}

FUNK(int64x1_t)
vget_high_s64(int64x2_t __a) {
  __GET_HIGH(s64);
}

FUNK(uint8x8_t)
vget_high_u8(uint8x16_t __a) {
  __GET_HIGH(u8);
}

FUNK(uint16x4_t)
vget_high_u16(uint16x8_t __a) {
  __GET_HIGH(u16);
}

FUNK(uint32x2_t)
vget_high_u32(uint32x4_t __a) {
  __GET_HIGH(u32);
}

#undef __GET_HIGH

FUNK(uint64x1_t)
vget_high_u64(uint64x2_t __a) {
  return vcreate_u64(vgetq_lane_u64(__a, 1));
}

FUNK(int8x16_t)
vcombine_s8(int8x8_t __a, int8x8_t __b) {
  return (int8x16_t)__builtin_aarch64_combinev8qi(__a, __b);
}

FUNK(int16x8_t)
vcombine_s16(int16x4_t __a, int16x4_t __b) {
  return (int16x8_t)__builtin_aarch64_combinev4hi(__a, __b);
}

FUNK(int32x4_t)
vcombine_s32(int32x2_t __a, int32x2_t __b) {
  return (int32x4_t)__builtin_aarch64_combinev2si(__a, __b);
}

FUNK(int64x2_t)
vcombine_s64(int64x1_t __a, int64x1_t __b) {
  return __builtin_aarch64_combinedi(__a[0], __b[0]);
}

FUNK(float16x8_t)
vcombine_f16(float16x4_t __a, float16x4_t __b) {
  return __builtin_aarch64_combinev4hf(__a, __b);
}

FUNK(float32x4_t)
vcombine_f32(float32x2_t __a, float32x2_t __b) {
  return (float32x4_t)__builtin_aarch64_combinev2sf(__a, __b);
}

FUNK(uint8x16_t)
vcombine_u8(uint8x8_t __a, uint8x8_t __b) {
  return (uint8x16_t)__builtin_aarch64_combinev8qi((int8x8_t)__a,
                                                   (int8x8_t)__b);
}

FUNK(uint16x8_t)
vcombine_u16(uint16x4_t __a, uint16x4_t __b) {
  return (uint16x8_t)__builtin_aarch64_combinev4hi((int16x4_t)__a,
                                                   (int16x4_t)__b);
}

FUNK(uint32x4_t)
vcombine_u32(uint32x2_t __a, uint32x2_t __b) {
  return (uint32x4_t)__builtin_aarch64_combinev2si((int32x2_t)__a,
                                                   (int32x2_t)__b);
}

FUNK(uint64x2_t)
vcombine_u64(uint64x1_t __a, uint64x1_t __b) {
  return (uint64x2_t)__builtin_aarch64_combinedi(__a[0], __b[0]);
}

FUNK(float64x2_t)
vcombine_f64(float64x1_t __a, float64x1_t __b) {
  return __builtin_aarch64_combinedf(__a[0], __b[0]);
}

FUNK(poly8x16_t)
vcombine_p8(poly8x8_t __a, poly8x8_t __b) {
  return (poly8x16_t)__builtin_aarch64_combinev8qi((int8x8_t)__a,
                                                   (int8x8_t)__b);
}

FUNK(poly16x8_t)
vcombine_p16(poly16x4_t __a, poly16x4_t __b) {
  return (poly16x8_t)__builtin_aarch64_combinev4hi((int16x4_t)__a,
                                                   (int16x4_t)__b);
}

FUNK(poly64x2_t)
vcombine_p64(poly64x1_t __a, poly64x1_t __b) {
  return (poly64x2_t)__builtin_aarch64_combinedi_ppp(__a[0], __b[0]);
}

FUNK(int8x8_t)
vaba_s8(int8x8_t a, int8x8_t b, int8x8_t c) {
  int8x8_t result;
  __asm__("saba %0.8b,%2.8b,%3.8b"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int16x4_t)
vaba_s16(int16x4_t a, int16x4_t b, int16x4_t c) {
  int16x4_t result;
  __asm__("saba %0.4h,%2.4h,%3.4h"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int32x2_t)
vaba_s32(int32x2_t a, int32x2_t b, int32x2_t c) {
  int32x2_t result;
  __asm__("saba %0.2s,%2.2s,%3.2s"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint8x8_t)
vaba_u8(uint8x8_t a, uint8x8_t b, uint8x8_t c) {
  uint8x8_t result;
  __asm__("uaba %0.8b,%2.8b,%3.8b"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint16x4_t)
vaba_u16(uint16x4_t a, uint16x4_t b, uint16x4_t c) {
  uint16x4_t result;
  __asm__("uaba %0.4h,%2.4h,%3.4h"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint32x2_t)
vaba_u32(uint32x2_t a, uint32x2_t b, uint32x2_t c) {
  uint32x2_t result;
  __asm__("uaba %0.2s,%2.2s,%3.2s"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int16x8_t)
vabal_high_s8(int16x8_t a, int8x16_t b, int8x16_t c) {
  int16x8_t result;
  __asm__("sabal2 %0.8h,%2.16b,%3.16b"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int32x4_t)
vabal_high_s16(int32x4_t a, int16x8_t b, int16x8_t c) {
  int32x4_t result;
  __asm__("sabal2 %0.4s,%2.8h,%3.8h"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int64x2_t)
vabal_high_s32(int64x2_t a, int32x4_t b, int32x4_t c) {
  int64x2_t result;
  __asm__("sabal2 %0.2d,%2.4s,%3.4s"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint16x8_t)
vabal_high_u8(uint16x8_t a, uint8x16_t b, uint8x16_t c) {
  uint16x8_t result;
  __asm__("uabal2 %0.8h,%2.16b,%3.16b"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint32x4_t)
vabal_high_u16(uint32x4_t a, uint16x8_t b, uint16x8_t c) {
  uint32x4_t result;
  __asm__("uabal2 %0.4s,%2.8h,%3.8h"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint64x2_t)
vabal_high_u32(uint64x2_t a, uint32x4_t b, uint32x4_t c) {
  uint64x2_t result;
  __asm__("uabal2 %0.2d,%2.4s,%3.4s"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int16x8_t)
vabal_s8(int16x8_t a, int8x8_t b, int8x8_t c) {
  int16x8_t result;
  __asm__("sabal %0.8h,%2.8b,%3.8b"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int32x4_t)
vabal_s16(int32x4_t a, int16x4_t b, int16x4_t c) {
  int32x4_t result;
  __asm__("sabal %0.4s,%2.4h,%3.4h"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int64x2_t)
vabal_s32(int64x2_t a, int32x2_t b, int32x2_t c) {
  int64x2_t result;
  __asm__("sabal %0.2d,%2.2s,%3.2s"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint16x8_t)
vabal_u8(uint16x8_t a, uint8x8_t b, uint8x8_t c) {
  uint16x8_t result;
  __asm__("uabal %0.8h,%2.8b,%3.8b"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint32x4_t)
vabal_u16(uint32x4_t a, uint16x4_t b, uint16x4_t c) {
  uint32x4_t result;
  __asm__("uabal %0.4s,%2.4h,%3.4h"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint64x2_t)
vabal_u32(uint64x2_t a, uint32x2_t b, uint32x2_t c) {
  uint64x2_t result;
  __asm__("uabal %0.2d,%2.2s,%3.2s"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int8x16_t)
vabaq_s8(int8x16_t a, int8x16_t b, int8x16_t c) {
  int8x16_t result;
  __asm__("saba %0.16b,%2.16b,%3.16b"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int16x8_t)
vabaq_s16(int16x8_t a, int16x8_t b, int16x8_t c) {
  int16x8_t result;
  __asm__("saba %0.8h,%2.8h,%3.8h"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int32x4_t)
vabaq_s32(int32x4_t a, int32x4_t b, int32x4_t c) {
  int32x4_t result;
  __asm__("saba %0.4s,%2.4s,%3.4s"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint8x16_t)
vabaq_u8(uint8x16_t a, uint8x16_t b, uint8x16_t c) {
  uint8x16_t result;
  __asm__("uaba %0.16b,%2.16b,%3.16b"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint16x8_t)
vabaq_u16(uint16x8_t a, uint16x8_t b, uint16x8_t c) {
  uint16x8_t result;
  __asm__("uaba %0.8h,%2.8h,%3.8h"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint32x4_t)
vabaq_u32(uint32x4_t a, uint32x4_t b, uint32x4_t c) {
  uint32x4_t result;
  __asm__("uaba %0.4s,%2.4s,%3.4s"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int8x8_t)
vabd_s8(int8x8_t a, int8x8_t b) {
  int8x8_t result;
  __asm__("sabd %0.8b, %1.8b, %2.8b"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int16x4_t)
vabd_s16(int16x4_t a, int16x4_t b) {
  int16x4_t result;
  __asm__("sabd %0.4h, %1.4h, %2.4h"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int32x2_t)
vabd_s32(int32x2_t a, int32x2_t b) {
  int32x2_t result;
  __asm__("sabd %0.2s, %1.2s, %2.2s"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(uint8x8_t)
vabd_u8(uint8x8_t a, uint8x8_t b) {
  uint8x8_t result;
  __asm__("uabd %0.8b, %1.8b, %2.8b"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(uint16x4_t)
vabd_u16(uint16x4_t a, uint16x4_t b) {
  uint16x4_t result;
  __asm__("uabd %0.4h, %1.4h, %2.4h"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(uint32x2_t)
vabd_u32(uint32x2_t a, uint32x2_t b) {
  uint32x2_t result;
  __asm__("uabd %0.2s, %1.2s, %2.2s"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int16x8_t)
vabdl_high_s8(int8x16_t a, int8x16_t b) {
  int16x8_t result;
  __asm__("sabdl2 %0.8h,%1.16b,%2.16b"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int32x4_t)
vabdl_high_s16(int16x8_t a, int16x8_t b) {
  int32x4_t result;
  __asm__("sabdl2 %0.4s,%1.8h,%2.8h"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int64x2_t)
vabdl_high_s32(int32x4_t a, int32x4_t b) {
  int64x2_t result;
  __asm__("sabdl2 %0.2d,%1.4s,%2.4s"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(uint16x8_t)
vabdl_high_u8(uint8x16_t a, uint8x16_t b) {
  uint16x8_t result;
  __asm__("uabdl2 %0.8h,%1.16b,%2.16b"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(uint32x4_t)
vabdl_high_u16(uint16x8_t a, uint16x8_t b) {
  uint32x4_t result;
  __asm__("uabdl2 %0.4s,%1.8h,%2.8h"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(uint64x2_t)
vabdl_high_u32(uint32x4_t a, uint32x4_t b) {
  uint64x2_t result;
  __asm__("uabdl2 %0.2d,%1.4s,%2.4s"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int16x8_t)
vabdl_s8(int8x8_t a, int8x8_t b) {
  int16x8_t result;
  __asm__("sabdl %0.8h, %1.8b, %2.8b"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int32x4_t)
vabdl_s16(int16x4_t a, int16x4_t b) {
  int32x4_t result;
  __asm__("sabdl %0.4s, %1.4h, %2.4h"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int64x2_t)
vabdl_s32(int32x2_t a, int32x2_t b) {
  int64x2_t result;
  __asm__("sabdl %0.2d, %1.2s, %2.2s"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(uint16x8_t)
vabdl_u8(uint8x8_t a, uint8x8_t b) {
  uint16x8_t result;
  __asm__("uabdl %0.8h, %1.8b, %2.8b"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(uint32x4_t)
vabdl_u16(uint16x4_t a, uint16x4_t b) {
  uint32x4_t result;
  __asm__("uabdl %0.4s, %1.4h, %2.4h"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(uint64x2_t)
vabdl_u32(uint32x2_t a, uint32x2_t b) {
  uint64x2_t result;
  __asm__("uabdl %0.2d, %1.2s, %2.2s"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int8x16_t)
vabdq_s8(int8x16_t a, int8x16_t b) {
  int8x16_t result;
  __asm__("sabd %0.16b, %1.16b, %2.16b"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int16x8_t)
vabdq_s16(int16x8_t a, int16x8_t b) {
  int16x8_t result;
  __asm__("sabd %0.8h, %1.8h, %2.8h"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int32x4_t)
vabdq_s32(int32x4_t a, int32x4_t b) {
  int32x4_t result;
  __asm__("sabd %0.4s, %1.4s, %2.4s"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(uint8x16_t)
vabdq_u8(uint8x16_t a, uint8x16_t b) {
  uint8x16_t result;
  __asm__("uabd %0.16b, %1.16b, %2.16b"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(uint16x8_t)
vabdq_u16(uint16x8_t a, uint16x8_t b) {
  uint16x8_t result;
  __asm__("uabd %0.8h, %1.8h, %2.8h"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(uint32x4_t)
vabdq_u32(uint32x4_t a, uint32x4_t b) {
  uint32x4_t result;
  __asm__("uabd %0.4s, %1.4s, %2.4s"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int16_t)
vaddlv_s8(int8x8_t a) {
  int16_t result;
  __asm__("saddlv %h0,%1.8b" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(int32_t)
vaddlv_s16(int16x4_t a) {
  int32_t result;
  __asm__("saddlv %s0,%1.4h" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(uint16_t)
vaddlv_u8(uint8x8_t a) {
  uint16_t result;
  __asm__("uaddlv %h0,%1.8b" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(uint32_t)
vaddlv_u16(uint16x4_t a) {
  uint32_t result;
  __asm__("uaddlv %s0,%1.4h" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(int16_t)
vaddlvq_s8(int8x16_t a) {
  int16_t result;
  __asm__("saddlv %h0,%1.16b" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(int32_t)
vaddlvq_s16(int16x8_t a) {
  int32_t result;
  __asm__("saddlv %s0,%1.8h" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(int64_t)
vaddlvq_s32(int32x4_t a) {
  int64_t result;
  __asm__("saddlv %d0,%1.4s" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(uint16_t)
vaddlvq_u8(uint8x16_t a) {
  uint16_t result;
  __asm__("uaddlv %h0,%1.16b" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(uint32_t)
vaddlvq_u16(uint16x8_t a) {
  uint32_t result;
  __asm__("uaddlv %s0,%1.8h" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(uint64_t)
vaddlvq_u32(uint32x4_t a) {
  uint64_t result;
  __asm__("uaddlv %d0,%1.4s" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(float32x2_t)
vcvtx_f32_f64(float64x2_t a) {
  float32x2_t result;
  __asm__("fcvtxn %0.2s,%1.2d" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(float32x4_t)
vcvtx_high_f32_f64(float32x2_t a, float64x2_t b) {
  float32x4_t result;
  __asm__("fcvtxn2 %0.4s,%1.2d"
          : "=w"(result)
          : "w"(b), "0"(a)
          : /* No clobbers */);
  return result;
}

FUNK(float32_t)
vcvtxd_f32_f64(float64_t a) {
  float32_t result;
  __asm__("fcvtxn %s0,%d1" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(float32x2_t)
vmla_n_f32(float32x2_t a, float32x2_t b, float32_t c) {
  float32x2_t result;
  float32x2_t t1;
  __asm__("fmul %1.2s, %3.2s, %4.s[0]; fadd %0.2s, %0.2s, %1.2s"
          : "=w"(result), "=w"(t1)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int16x4_t)
vmla_n_s16(int16x4_t a, int16x4_t b, int16_t c) {
  int16x4_t result;
  __asm__("mla %0.4h,%2.4h,%3.h[0]"
          : "=w"(result)
          : "0"(a), "w"(b), "x"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int32x2_t)
vmla_n_s32(int32x2_t a, int32x2_t b, int32_t c) {
  int32x2_t result;
  __asm__("mla %0.2s,%2.2s,%3.s[0]"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint16x4_t)
vmla_n_u16(uint16x4_t a, uint16x4_t b, uint16_t c) {
  uint16x4_t result;
  __asm__("mla %0.4h,%2.4h,%3.h[0]"
          : "=w"(result)
          : "0"(a), "w"(b), "x"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint32x2_t)
vmla_n_u32(uint32x2_t a, uint32x2_t b, uint32_t c) {
  uint32x2_t result;
  __asm__("mla %0.2s,%2.2s,%3.s[0]"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int8x8_t)
vmla_s8(int8x8_t a, int8x8_t b, int8x8_t c) {
  int8x8_t result;
  __asm__("mla %0.8b, %2.8b, %3.8b"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int16x4_t)
vmla_s16(int16x4_t a, int16x4_t b, int16x4_t c) {
  int16x4_t result;
  __asm__("mla %0.4h, %2.4h, %3.4h"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int32x2_t)
vmla_s32(int32x2_t a, int32x2_t b, int32x2_t c) {
  int32x2_t result;
  __asm__("mla %0.2s, %2.2s, %3.2s"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint8x8_t)
vmla_u8(uint8x8_t a, uint8x8_t b, uint8x8_t c) {
  uint8x8_t result;
  __asm__("mla %0.8b, %2.8b, %3.8b"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint16x4_t)
vmla_u16(uint16x4_t a, uint16x4_t b, uint16x4_t c) {
  uint16x4_t result;
  __asm__("mla %0.4h, %2.4h, %3.4h"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint32x2_t)
vmla_u32(uint32x2_t a, uint32x2_t b, uint32x2_t c) {
  uint32x2_t result;
  __asm__("mla %0.2s, %2.2s, %3.2s"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

#define vmlal_high_lane_s16(a, b, c, d)         \
  __extension__({                               \
    int16x4_t c_ = (c);                         \
    int16x8_t b_ = (b);                         \
    int32x4_t a_ = (a);                         \
    int32x4_t result;                           \
    __asm__("smlal2 %0.4s, %2.8h, %3.h[%4]"     \
            : "=w"(result)                      \
            : "0"(a_), "w"(b_), "x"(c_), "i"(d) \
            : /* No clobbers */);               \
    result;                                     \
  })

#define vmlal_high_lane_s32(a, b, c, d)         \
  __extension__({                               \
    int32x2_t c_ = (c);                         \
    int32x4_t b_ = (b);                         \
    int64x2_t a_ = (a);                         \
    int64x2_t result;                           \
    __asm__("smlal2 %0.2d, %2.4s, %3.s[%4]"     \
            : "=w"(result)                      \
            : "0"(a_), "w"(b_), "w"(c_), "i"(d) \
            : /* No clobbers */);               \
    result;                                     \
  })

#define vmlal_high_lane_u16(a, b, c, d)         \
  __extension__({                               \
    uint16x4_t c_ = (c);                        \
    uint16x8_t b_ = (b);                        \
    uint32x4_t a_ = (a);                        \
    uint32x4_t result;                          \
    __asm__("umlal2 %0.4s, %2.8h, %3.h[%4]"     \
            : "=w"(result)                      \
            : "0"(a_), "w"(b_), "x"(c_), "i"(d) \
            : /* No clobbers */);               \
    result;                                     \
  })

#define vmlal_high_lane_u32(a, b, c, d)         \
  __extension__({                               \
    uint32x2_t c_ = (c);                        \
    uint32x4_t b_ = (b);                        \
    uint64x2_t a_ = (a);                        \
    uint64x2_t result;                          \
    __asm__("umlal2 %0.2d, %2.4s, %3.s[%4]"     \
            : "=w"(result)                      \
            : "0"(a_), "w"(b_), "w"(c_), "i"(d) \
            : /* No clobbers */);               \
    result;                                     \
  })

#define vmlal_high_laneq_s16(a, b, c, d)        \
  __extension__({                               \
    int16x8_t c_ = (c);                         \
    int16x8_t b_ = (b);                         \
    int32x4_t a_ = (a);                         \
    int32x4_t result;                           \
    __asm__("smlal2 %0.4s, %2.8h, %3.h[%4]"     \
            : "=w"(result)                      \
            : "0"(a_), "w"(b_), "x"(c_), "i"(d) \
            : /* No clobbers */);               \
    result;                                     \
  })

#define vmlal_high_laneq_s32(a, b, c, d)        \
  __extension__({                               \
    int32x4_t c_ = (c);                         \
    int32x4_t b_ = (b);                         \
    int64x2_t a_ = (a);                         \
    int64x2_t result;                           \
    __asm__("smlal2 %0.2d, %2.4s, %3.s[%4]"     \
            : "=w"(result)                      \
            : "0"(a_), "w"(b_), "w"(c_), "i"(d) \
            : /* No clobbers */);               \
    result;                                     \
  })

#define vmlal_high_laneq_u16(a, b, c, d)        \
  __extension__({                               \
    uint16x8_t c_ = (c);                        \
    uint16x8_t b_ = (b);                        \
    uint32x4_t a_ = (a);                        \
    uint32x4_t result;                          \
    __asm__("umlal2 %0.4s, %2.8h, %3.h[%4]"     \
            : "=w"(result)                      \
            : "0"(a_), "w"(b_), "x"(c_), "i"(d) \
            : /* No clobbers */);               \
    result;                                     \
  })

#define vmlal_high_laneq_u32(a, b, c, d)        \
  __extension__({                               \
    uint32x4_t c_ = (c);                        \
    uint32x4_t b_ = (b);                        \
    uint64x2_t a_ = (a);                        \
    uint64x2_t result;                          \
    __asm__("umlal2 %0.2d, %2.4s, %3.s[%4]"     \
            : "=w"(result)                      \
            : "0"(a_), "w"(b_), "w"(c_), "i"(d) \
            : /* No clobbers */);               \
    result;                                     \
  })

FUNK(int32x4_t)
vmlal_high_n_s16(int32x4_t a, int16x8_t b, int16_t c) {
  int32x4_t result;
  __asm__("smlal2 %0.4s,%2.8h,%3.h[0]"
          : "=w"(result)
          : "0"(a), "w"(b), "x"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int64x2_t)
vmlal_high_n_s32(int64x2_t a, int32x4_t b, int32_t c) {
  int64x2_t result;
  __asm__("smlal2 %0.2d,%2.4s,%3.s[0]"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint32x4_t)
vmlal_high_n_u16(uint32x4_t a, uint16x8_t b, uint16_t c) {
  uint32x4_t result;
  __asm__("umlal2 %0.4s,%2.8h,%3.h[0]"
          : "=w"(result)
          : "0"(a), "w"(b), "x"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint64x2_t)
vmlal_high_n_u32(uint64x2_t a, uint32x4_t b, uint32_t c) {
  uint64x2_t result;
  __asm__("umlal2 %0.2d,%2.4s,%3.s[0]"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int16x8_t)
vmlal_high_s8(int16x8_t a, int8x16_t b, int8x16_t c) {
  int16x8_t result;
  __asm__("smlal2 %0.8h,%2.16b,%3.16b"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int32x4_t)
vmlal_high_s16(int32x4_t a, int16x8_t b, int16x8_t c) {
  int32x4_t result;
  __asm__("smlal2 %0.4s,%2.8h,%3.8h"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int64x2_t)
vmlal_high_s32(int64x2_t a, int32x4_t b, int32x4_t c) {
  int64x2_t result;
  __asm__("smlal2 %0.2d,%2.4s,%3.4s"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint16x8_t)
vmlal_high_u8(uint16x8_t a, uint8x16_t b, uint8x16_t c) {
  uint16x8_t result;
  __asm__("umlal2 %0.8h,%2.16b,%3.16b"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint32x4_t)
vmlal_high_u16(uint32x4_t a, uint16x8_t b, uint16x8_t c) {
  uint32x4_t result;
  __asm__("umlal2 %0.4s,%2.8h,%3.8h"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint64x2_t)
vmlal_high_u32(uint64x2_t a, uint32x4_t b, uint32x4_t c) {
  uint64x2_t result;
  __asm__("umlal2 %0.2d,%2.4s,%3.4s"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

#define vmlal_lane_s16(a, b, c, d)              \
  __extension__({                               \
    int16x4_t c_ = (c);                         \
    int16x4_t b_ = (b);                         \
    int32x4_t a_ = (a);                         \
    int32x4_t result;                           \
    __asm__("smlal %0.4s,%2.4h,%3.h[%4]"        \
            : "=w"(result)                      \
            : "0"(a_), "w"(b_), "x"(c_), "i"(d) \
            : /* No clobbers */);               \
    result;                                     \
  })

#define vmlal_lane_s32(a, b, c, d)              \
  __extension__({                               \
    int32x2_t c_ = (c);                         \
    int32x2_t b_ = (b);                         \
    int64x2_t a_ = (a);                         \
    int64x2_t result;                           \
    __asm__("smlal %0.2d,%2.2s,%3.s[%4]"        \
            : "=w"(result)                      \
            : "0"(a_), "w"(b_), "w"(c_), "i"(d) \
            : /* No clobbers */);               \
    result;                                     \
  })

#define vmlal_lane_u16(a, b, c, d)              \
  __extension__({                               \
    uint16x4_t c_ = (c);                        \
    uint16x4_t b_ = (b);                        \
    uint32x4_t a_ = (a);                        \
    uint32x4_t result;                          \
    __asm__("umlal %0.4s,%2.4h,%3.h[%4]"        \
            : "=w"(result)                      \
            : "0"(a_), "w"(b_), "x"(c_), "i"(d) \
            : /* No clobbers */);               \
    result;                                     \
  })

#define vmlal_lane_u32(a, b, c, d)              \
  __extension__({                               \
    uint32x2_t c_ = (c);                        \
    uint32x2_t b_ = (b);                        \
    uint64x2_t a_ = (a);                        \
    uint64x2_t result;                          \
    __asm__("umlal %0.2d, %2.2s, %3.s[%4]"      \
            : "=w"(result)                      \
            : "0"(a_), "w"(b_), "w"(c_), "i"(d) \
            : /* No clobbers */);               \
    result;                                     \
  })

#define vmlal_laneq_s16(a, b, c, d)             \
  __extension__({                               \
    int16x8_t c_ = (c);                         \
    int16x4_t b_ = (b);                         \
    int32x4_t a_ = (a);                         \
    int32x4_t result;                           \
    __asm__("smlal %0.4s, %2.4h, %3.h[%4]"      \
            : "=w"(result)                      \
            : "0"(a_), "w"(b_), "x"(c_), "i"(d) \
            : /* No clobbers */);               \
    result;                                     \
  })

#define vmlal_laneq_s32(a, b, c, d)             \
  __extension__({                               \
    int32x4_t c_ = (c);                         \
    int32x2_t b_ = (b);                         \
    int64x2_t a_ = (a);                         \
    int64x2_t result;                           \
    __asm__("smlal %0.2d, %2.2s, %3.s[%4]"      \
            : "=w"(result)                      \
            : "0"(a_), "w"(b_), "w"(c_), "i"(d) \
            : /* No clobbers */);               \
    result;                                     \
  })

#define vmlal_laneq_u16(a, b, c, d)             \
  __extension__({                               \
    uint16x8_t c_ = (c);                        \
    uint16x4_t b_ = (b);                        \
    uint32x4_t a_ = (a);                        \
    uint32x4_t result;                          \
    __asm__("umlal %0.4s, %2.4h, %3.h[%4]"      \
            : "=w"(result)                      \
            : "0"(a_), "w"(b_), "x"(c_), "i"(d) \
            : /* No clobbers */);               \
    result;                                     \
  })

#define vmlal_laneq_u32(a, b, c, d)             \
  __extension__({                               \
    uint32x4_t c_ = (c);                        \
    uint32x2_t b_ = (b);                        \
    uint64x2_t a_ = (a);                        \
    uint64x2_t result;                          \
    __asm__("umlal %0.2d, %2.2s, %3.s[%4]"      \
            : "=w"(result)                      \
            : "0"(a_), "w"(b_), "w"(c_), "i"(d) \
            : /* No clobbers */);               \
    result;                                     \
  })

FUNK(int32x4_t)
vmlal_n_s16(int32x4_t a, int16x4_t b, int16_t c) {
  int32x4_t result;
  __asm__("smlal %0.4s,%2.4h,%3.h[0]"
          : "=w"(result)
          : "0"(a), "w"(b), "x"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int64x2_t)
vmlal_n_s32(int64x2_t a, int32x2_t b, int32_t c) {
  int64x2_t result;
  __asm__("smlal %0.2d,%2.2s,%3.s[0]"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint32x4_t)
vmlal_n_u16(uint32x4_t a, uint16x4_t b, uint16_t c) {
  uint32x4_t result;
  __asm__("umlal %0.4s,%2.4h,%3.h[0]"
          : "=w"(result)
          : "0"(a), "w"(b), "x"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint64x2_t)
vmlal_n_u32(uint64x2_t a, uint32x2_t b, uint32_t c) {
  uint64x2_t result;
  __asm__("umlal %0.2d,%2.2s,%3.s[0]"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int16x8_t)
vmlal_s8(int16x8_t a, int8x8_t b, int8x8_t c) {
  int16x8_t result;
  __asm__("smlal %0.8h,%2.8b,%3.8b"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int32x4_t)
vmlal_s16(int32x4_t a, int16x4_t b, int16x4_t c) {
  int32x4_t result;
  __asm__("smlal %0.4s,%2.4h,%3.4h"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int64x2_t)
vmlal_s32(int64x2_t a, int32x2_t b, int32x2_t c) {
  int64x2_t result;
  __asm__("smlal %0.2d,%2.2s,%3.2s"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint16x8_t)
vmlal_u8(uint16x8_t a, uint8x8_t b, uint8x8_t c) {
  uint16x8_t result;
  __asm__("umlal %0.8h,%2.8b,%3.8b"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint32x4_t)
vmlal_u16(uint32x4_t a, uint16x4_t b, uint16x4_t c) {
  uint32x4_t result;
  __asm__("umlal %0.4s,%2.4h,%3.4h"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint64x2_t)
vmlal_u32(uint64x2_t a, uint32x2_t b, uint32x2_t c) {
  uint64x2_t result;
  __asm__("umlal %0.2d,%2.2s,%3.2s"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(float32x4_t)
vmlaq_n_f32(float32x4_t a, float32x4_t b, float32_t c) {
  float32x4_t result;
  float32x4_t t1;
  __asm__("fmul %1.4s, %3.4s, %4.s[0]; fadd %0.4s, %0.4s, %1.4s"
          : "=w"(result), "=w"(t1)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int16x8_t)
vmlaq_n_s16(int16x8_t a, int16x8_t b, int16_t c) {
  int16x8_t result;
  __asm__("mla %0.8h,%2.8h,%3.h[0]"
          : "=w"(result)
          : "0"(a), "w"(b), "x"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int32x4_t)
vmlaq_n_s32(int32x4_t a, int32x4_t b, int32_t c) {
  int32x4_t result;
  __asm__("mla %0.4s,%2.4s,%3.s[0]"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint16x8_t)
vmlaq_n_u16(uint16x8_t a, uint16x8_t b, uint16_t c) {
  uint16x8_t result;
  __asm__("mla %0.8h,%2.8h,%3.h[0]"
          : "=w"(result)
          : "0"(a), "w"(b), "x"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint32x4_t)
vmlaq_n_u32(uint32x4_t a, uint32x4_t b, uint32_t c) {
  uint32x4_t result;
  __asm__("mla %0.4s,%2.4s,%3.s[0]"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int8x16_t)
vmlaq_s8(int8x16_t a, int8x16_t b, int8x16_t c) {
  int8x16_t result;
  __asm__("mla %0.16b, %2.16b, %3.16b"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int16x8_t)
vmlaq_s16(int16x8_t a, int16x8_t b, int16x8_t c) {
  int16x8_t result;
  __asm__("mla %0.8h, %2.8h, %3.8h"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int32x4_t)
vmlaq_s32(int32x4_t a, int32x4_t b, int32x4_t c) {
  int32x4_t result;
  __asm__("mla %0.4s, %2.4s, %3.4s"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint8x16_t)
vmlaq_u8(uint8x16_t a, uint8x16_t b, uint8x16_t c) {
  uint8x16_t result;
  __asm__("mla %0.16b, %2.16b, %3.16b"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint16x8_t)
vmlaq_u16(uint16x8_t a, uint16x8_t b, uint16x8_t c) {
  uint16x8_t result;
  __asm__("mla %0.8h, %2.8h, %3.8h"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint32x4_t)
vmlaq_u32(uint32x4_t a, uint32x4_t b, uint32x4_t c) {
  uint32x4_t result;
  __asm__("mla %0.4s, %2.4s, %3.4s"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(float32x2_t)
vmls_n_f32(float32x2_t a, float32x2_t b, float32_t c) {
  float32x2_t result;
  float32x2_t t1;
  __asm__("fmul %1.2s, %3.2s, %4.s[0]; fsub %0.2s, %0.2s, %1.2s"
          : "=w"(result), "=w"(t1)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int16x4_t)
vmls_n_s16(int16x4_t a, int16x4_t b, int16_t c) {
  int16x4_t result;
  __asm__("mls %0.4h, %2.4h, %3.h[0]"
          : "=w"(result)
          : "0"(a), "w"(b), "x"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int32x2_t)
vmls_n_s32(int32x2_t a, int32x2_t b, int32_t c) {
  int32x2_t result;
  __asm__("mls %0.2s, %2.2s, %3.s[0]"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint16x4_t)
vmls_n_u16(uint16x4_t a, uint16x4_t b, uint16_t c) {
  uint16x4_t result;
  __asm__("mls %0.4h, %2.4h, %3.h[0]"
          : "=w"(result)
          : "0"(a), "w"(b), "x"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint32x2_t)
vmls_n_u32(uint32x2_t a, uint32x2_t b, uint32_t c) {
  uint32x2_t result;
  __asm__("mls %0.2s, %2.2s, %3.s[0]"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int8x8_t)
vmls_s8(int8x8_t a, int8x8_t b, int8x8_t c) {
  int8x8_t result;
  __asm__("mls %0.8b,%2.8b,%3.8b"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int16x4_t)
vmls_s16(int16x4_t a, int16x4_t b, int16x4_t c) {
  int16x4_t result;
  __asm__("mls %0.4h,%2.4h,%3.4h"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int32x2_t)
vmls_s32(int32x2_t a, int32x2_t b, int32x2_t c) {
  int32x2_t result;
  __asm__("mls %0.2s,%2.2s,%3.2s"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint8x8_t)
vmls_u8(uint8x8_t a, uint8x8_t b, uint8x8_t c) {
  uint8x8_t result;
  __asm__("mls %0.8b,%2.8b,%3.8b"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint16x4_t)
vmls_u16(uint16x4_t a, uint16x4_t b, uint16x4_t c) {
  uint16x4_t result;
  __asm__("mls %0.4h,%2.4h,%3.4h"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint32x2_t)
vmls_u32(uint32x2_t a, uint32x2_t b, uint32x2_t c) {
  uint32x2_t result;
  __asm__("mls %0.2s,%2.2s,%3.2s"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

#define vmlsl_high_lane_s16(a, b, c, d)         \
  __extension__({                               \
    int16x4_t c_ = (c);                         \
    int16x8_t b_ = (b);                         \
    int32x4_t a_ = (a);                         \
    int32x4_t result;                           \
    __asm__("smlsl2 %0.4s, %2.8h, %3.h[%4]"     \
            : "=w"(result)                      \
            : "0"(a_), "w"(b_), "x"(c_), "i"(d) \
            : /* No clobbers */);               \
    result;                                     \
  })

#define vmlsl_high_lane_s32(a, b, c, d)         \
  __extension__({                               \
    int32x2_t c_ = (c);                         \
    int32x4_t b_ = (b);                         \
    int64x2_t a_ = (a);                         \
    int64x2_t result;                           \
    __asm__("smlsl2 %0.2d, %2.4s, %3.s[%4]"     \
            : "=w"(result)                      \
            : "0"(a_), "w"(b_), "w"(c_), "i"(d) \
            : /* No clobbers */);               \
    result;                                     \
  })

#define vmlsl_high_lane_u16(a, b, c, d)         \
  __extension__({                               \
    uint16x4_t c_ = (c);                        \
    uint16x8_t b_ = (b);                        \
    uint32x4_t a_ = (a);                        \
    uint32x4_t result;                          \
    __asm__("umlsl2 %0.4s, %2.8h, %3.h[%4]"     \
            : "=w"(result)                      \
            : "0"(a_), "w"(b_), "x"(c_), "i"(d) \
            : /* No clobbers */);               \
    result;                                     \
  })

#define vmlsl_high_lane_u32(a, b, c, d)         \
  __extension__({                               \
    uint32x2_t c_ = (c);                        \
    uint32x4_t b_ = (b);                        \
    uint64x2_t a_ = (a);                        \
    uint64x2_t result;                          \
    __asm__("umlsl2 %0.2d, %2.4s, %3.s[%4]"     \
            : "=w"(result)                      \
            : "0"(a_), "w"(b_), "w"(c_), "i"(d) \
            : /* No clobbers */);               \
    result;                                     \
  })

#define vmlsl_high_laneq_s16(a, b, c, d)        \
  __extension__({                               \
    int16x8_t c_ = (c);                         \
    int16x8_t b_ = (b);                         \
    int32x4_t a_ = (a);                         \
    int32x4_t result;                           \
    __asm__("smlsl2 %0.4s, %2.8h, %3.h[%4]"     \
            : "=w"(result)                      \
            : "0"(a_), "w"(b_), "x"(c_), "i"(d) \
            : /* No clobbers */);               \
    result;                                     \
  })

#define vmlsl_high_laneq_s32(a, b, c, d)        \
  __extension__({                               \
    int32x4_t c_ = (c);                         \
    int32x4_t b_ = (b);                         \
    int64x2_t a_ = (a);                         \
    int64x2_t result;                           \
    __asm__("smlsl2 %0.2d, %2.4s, %3.s[%4]"     \
            : "=w"(result)                      \
            : "0"(a_), "w"(b_), "w"(c_), "i"(d) \
            : /* No clobbers */);               \
    result;                                     \
  })

#define vmlsl_high_laneq_u16(a, b, c, d)        \
  __extension__({                               \
    uint16x8_t c_ = (c);                        \
    uint16x8_t b_ = (b);                        \
    uint32x4_t a_ = (a);                        \
    uint32x4_t result;                          \
    __asm__("umlsl2 %0.4s, %2.8h, %3.h[%4]"     \
            : "=w"(result)                      \
            : "0"(a_), "w"(b_), "x"(c_), "i"(d) \
            : /* No clobbers */);               \
    result;                                     \
  })

#define vmlsl_high_laneq_u32(a, b, c, d)        \
  __extension__({                               \
    uint32x4_t c_ = (c);                        \
    uint32x4_t b_ = (b);                        \
    uint64x2_t a_ = (a);                        \
    uint64x2_t result;                          \
    __asm__("umlsl2 %0.2d, %2.4s, %3.s[%4]"     \
            : "=w"(result)                      \
            : "0"(a_), "w"(b_), "w"(c_), "i"(d) \
            : /* No clobbers */);               \
    result;                                     \
  })

FUNK(int32x4_t)
vmlsl_high_n_s16(int32x4_t a, int16x8_t b, int16_t c) {
  int32x4_t result;
  __asm__("smlsl2 %0.4s, %2.8h, %3.h[0]"
          : "=w"(result)
          : "0"(a), "w"(b), "x"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int64x2_t)
vmlsl_high_n_s32(int64x2_t a, int32x4_t b, int32_t c) {
  int64x2_t result;
  __asm__("smlsl2 %0.2d, %2.4s, %3.s[0]"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint32x4_t)
vmlsl_high_n_u16(uint32x4_t a, uint16x8_t b, uint16_t c) {
  uint32x4_t result;
  __asm__("umlsl2 %0.4s, %2.8h, %3.h[0]"
          : "=w"(result)
          : "0"(a), "w"(b), "x"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint64x2_t)
vmlsl_high_n_u32(uint64x2_t a, uint32x4_t b, uint32_t c) {
  uint64x2_t result;
  __asm__("umlsl2 %0.2d, %2.4s, %3.s[0]"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int16x8_t)
vmlsl_high_s8(int16x8_t a, int8x16_t b, int8x16_t c) {
  int16x8_t result;
  __asm__("smlsl2 %0.8h,%2.16b,%3.16b"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int32x4_t)
vmlsl_high_s16(int32x4_t a, int16x8_t b, int16x8_t c) {
  int32x4_t result;
  __asm__("smlsl2 %0.4s,%2.8h,%3.8h"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int64x2_t)
vmlsl_high_s32(int64x2_t a, int32x4_t b, int32x4_t c) {
  int64x2_t result;
  __asm__("smlsl2 %0.2d,%2.4s,%3.4s"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint16x8_t)
vmlsl_high_u8(uint16x8_t a, uint8x16_t b, uint8x16_t c) {
  uint16x8_t result;
  __asm__("umlsl2 %0.8h,%2.16b,%3.16b"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint32x4_t)
vmlsl_high_u16(uint32x4_t a, uint16x8_t b, uint16x8_t c) {
  uint32x4_t result;
  __asm__("umlsl2 %0.4s,%2.8h,%3.8h"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint64x2_t)
vmlsl_high_u32(uint64x2_t a, uint32x4_t b, uint32x4_t c) {
  uint64x2_t result;
  __asm__("umlsl2 %0.2d,%2.4s,%3.4s"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

#define vmlsl_lane_s16(a, b, c, d)              \
  __extension__({                               \
    int16x4_t c_ = (c);                         \
    int16x4_t b_ = (b);                         \
    int32x4_t a_ = (a);                         \
    int32x4_t result;                           \
    __asm__("smlsl %0.4s, %2.4h, %3.h[%4]"      \
            : "=w"(result)                      \
            : "0"(a_), "w"(b_), "x"(c_), "i"(d) \
            : /* No clobbers */);               \
    result;                                     \
  })

#define vmlsl_lane_s32(a, b, c, d)              \
  __extension__({                               \
    int32x2_t c_ = (c);                         \
    int32x2_t b_ = (b);                         \
    int64x2_t a_ = (a);                         \
    int64x2_t result;                           \
    __asm__("smlsl %0.2d, %2.2s, %3.s[%4]"      \
            : "=w"(result)                      \
            : "0"(a_), "w"(b_), "w"(c_), "i"(d) \
            : /* No clobbers */);               \
    result;                                     \
  })

#define vmlsl_lane_u16(a, b, c, d)              \
  __extension__({                               \
    uint16x4_t c_ = (c);                        \
    uint16x4_t b_ = (b);                        \
    uint32x4_t a_ = (a);                        \
    uint32x4_t result;                          \
    __asm__("umlsl %0.4s, %2.4h, %3.h[%4]"      \
            : "=w"(result)                      \
            : "0"(a_), "w"(b_), "x"(c_), "i"(d) \
            : /* No clobbers */);               \
    result;                                     \
  })

#define vmlsl_lane_u32(a, b, c, d)              \
  __extension__({                               \
    uint32x2_t c_ = (c);                        \
    uint32x2_t b_ = (b);                        \
    uint64x2_t a_ = (a);                        \
    uint64x2_t result;                          \
    __asm__("umlsl %0.2d, %2.2s, %3.s[%4]"      \
            : "=w"(result)                      \
            : "0"(a_), "w"(b_), "w"(c_), "i"(d) \
            : /* No clobbers */);               \
    result;                                     \
  })

#define vmlsl_laneq_s16(a, b, c, d)             \
  __extension__({                               \
    int16x8_t c_ = (c);                         \
    int16x4_t b_ = (b);                         \
    int32x4_t a_ = (a);                         \
    int32x4_t result;                           \
    __asm__("smlsl %0.4s, %2.4h, %3.h[%4]"      \
            : "=w"(result)                      \
            : "0"(a_), "w"(b_), "x"(c_), "i"(d) \
            : /* No clobbers */);               \
    result;                                     \
  })

#define vmlsl_laneq_s32(a, b, c, d)             \
  __extension__({                               \
    int32x4_t c_ = (c);                         \
    int32x2_t b_ = (b);                         \
    int64x2_t a_ = (a);                         \
    int64x2_t result;                           \
    __asm__("smlsl %0.2d, %2.2s, %3.s[%4]"      \
            : "=w"(result)                      \
            : "0"(a_), "w"(b_), "w"(c_), "i"(d) \
            : /* No clobbers */);               \
    result;                                     \
  })

#define vmlsl_laneq_u16(a, b, c, d)             \
  __extension__({                               \
    uint16x8_t c_ = (c);                        \
    uint16x4_t b_ = (b);                        \
    uint32x4_t a_ = (a);                        \
    uint32x4_t result;                          \
    __asm__("umlsl %0.4s, %2.4h, %3.h[%4]"      \
            : "=w"(result)                      \
            : "0"(a_), "w"(b_), "x"(c_), "i"(d) \
            : /* No clobbers */);               \
    result;                                     \
  })

#define vmlsl_laneq_u32(a, b, c, d)             \
  __extension__({                               \
    uint32x4_t c_ = (c);                        \
    uint32x2_t b_ = (b);                        \
    uint64x2_t a_ = (a);                        \
    uint64x2_t result;                          \
    __asm__("umlsl %0.2d, %2.2s, %3.s[%4]"      \
            : "=w"(result)                      \
            : "0"(a_), "w"(b_), "w"(c_), "i"(d) \
            : /* No clobbers */);               \
    result;                                     \
  })

FUNK(int32x4_t)
vmlsl_n_s16(int32x4_t a, int16x4_t b, int16_t c) {
  int32x4_t result;
  __asm__("smlsl %0.4s, %2.4h, %3.h[0]"
          : "=w"(result)
          : "0"(a), "w"(b), "x"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int64x2_t)
vmlsl_n_s32(int64x2_t a, int32x2_t b, int32_t c) {
  int64x2_t result;
  __asm__("smlsl %0.2d, %2.2s, %3.s[0]"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint32x4_t)
vmlsl_n_u16(uint32x4_t a, uint16x4_t b, uint16_t c) {
  uint32x4_t result;
  __asm__("umlsl %0.4s, %2.4h, %3.h[0]"
          : "=w"(result)
          : "0"(a), "w"(b), "x"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint64x2_t)
vmlsl_n_u32(uint64x2_t a, uint32x2_t b, uint32_t c) {
  uint64x2_t result;
  __asm__("umlsl %0.2d, %2.2s, %3.s[0]"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int16x8_t)
vmlsl_s8(int16x8_t a, int8x8_t b, int8x8_t c) {
  int16x8_t result;
  __asm__("smlsl %0.8h, %2.8b, %3.8b"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int32x4_t)
vmlsl_s16(int32x4_t a, int16x4_t b, int16x4_t c) {
  int32x4_t result;
  __asm__("smlsl %0.4s, %2.4h, %3.4h"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int64x2_t)
vmlsl_s32(int64x2_t a, int32x2_t b, int32x2_t c) {
  int64x2_t result;
  __asm__("smlsl %0.2d, %2.2s, %3.2s"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint16x8_t)
vmlsl_u8(uint16x8_t a, uint8x8_t b, uint8x8_t c) {
  uint16x8_t result;
  __asm__("umlsl %0.8h, %2.8b, %3.8b"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint32x4_t)
vmlsl_u16(uint32x4_t a, uint16x4_t b, uint16x4_t c) {
  uint32x4_t result;
  __asm__("umlsl %0.4s, %2.4h, %3.4h"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint64x2_t)
vmlsl_u32(uint64x2_t a, uint32x2_t b, uint32x2_t c) {
  uint64x2_t result;
  __asm__("umlsl %0.2d, %2.2s, %3.2s"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(float32x4_t)
vmlsq_n_f32(float32x4_t a, float32x4_t b, float32_t c) {
  float32x4_t result;
  float32x4_t t1;
  __asm__("fmul %1.4s, %3.4s, %4.s[0]; fsub %0.4s, %0.4s, %1.4s"
          : "=w"(result), "=w"(t1)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int16x8_t)
vmlsq_n_s16(int16x8_t a, int16x8_t b, int16_t c) {
  int16x8_t result;
  __asm__("mls %0.8h, %2.8h, %3.h[0]"
          : "=w"(result)
          : "0"(a), "w"(b), "x"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int32x4_t)
vmlsq_n_s32(int32x4_t a, int32x4_t b, int32_t c) {
  int32x4_t result;
  __asm__("mls %0.4s, %2.4s, %3.s[0]"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint16x8_t)
vmlsq_n_u16(uint16x8_t a, uint16x8_t b, uint16_t c) {
  uint16x8_t result;
  __asm__("mls %0.8h, %2.8h, %3.h[0]"
          : "=w"(result)
          : "0"(a), "w"(b), "x"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint32x4_t)
vmlsq_n_u32(uint32x4_t a, uint32x4_t b, uint32_t c) {
  uint32x4_t result;
  __asm__("mls %0.4s, %2.4s, %3.s[0]"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int8x16_t)
vmlsq_s8(int8x16_t a, int8x16_t b, int8x16_t c) {
  int8x16_t result;
  __asm__("mls %0.16b,%2.16b,%3.16b"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int16x8_t)
vmlsq_s16(int16x8_t a, int16x8_t b, int16x8_t c) {
  int16x8_t result;
  __asm__("mls %0.8h,%2.8h,%3.8h"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int32x4_t)
vmlsq_s32(int32x4_t a, int32x4_t b, int32x4_t c) {
  int32x4_t result;
  __asm__("mls %0.4s,%2.4s,%3.4s"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint8x16_t)
vmlsq_u8(uint8x16_t a, uint8x16_t b, uint8x16_t c) {
  uint8x16_t result;
  __asm__("mls %0.16b,%2.16b,%3.16b"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint16x8_t)
vmlsq_u16(uint16x8_t a, uint16x8_t b, uint16x8_t c) {
  uint16x8_t result;
  __asm__("mls %0.8h,%2.8h,%3.8h"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(uint32x4_t)
vmlsq_u32(uint32x4_t a, uint32x4_t b, uint32x4_t c) {
  uint32x4_t result;
  __asm__("mls %0.4s,%2.4s,%3.4s"
          : "=w"(result)
          : "0"(a), "w"(b), "w"(c)
          : /* No clobbers */);
  return result;
}

FUNK(int16x8_t)
vmovl_high_s8(int8x16_t a) {
  int16x8_t result;
  __asm__("sshll2 %0.8h,%1.16b,#0" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(int32x4_t)
vmovl_high_s16(int16x8_t a) {
  int32x4_t result;
  __asm__("sshll2 %0.4s,%1.8h,#0" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(int64x2_t)
vmovl_high_s32(int32x4_t a) {
  int64x2_t result;
  __asm__("sshll2 %0.2d,%1.4s,#0" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(uint16x8_t)
vmovl_high_u8(uint8x16_t a) {
  uint16x8_t result;
  __asm__("ushll2 %0.8h,%1.16b,#0" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(uint32x4_t)
vmovl_high_u16(uint16x8_t a) {
  uint32x4_t result;
  __asm__("ushll2 %0.4s,%1.8h,#0" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(uint64x2_t)
vmovl_high_u32(uint32x4_t a) {
  uint64x2_t result;
  __asm__("ushll2 %0.2d,%1.4s,#0" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(int16x8_t)
vmovl_s8(int8x8_t a) {
  int16x8_t result;
  __asm__("sshll %0.8h,%1.8b,#0" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(int32x4_t)
vmovl_s16(int16x4_t a) {
  int32x4_t result;
  __asm__("sshll %0.4s,%1.4h,#0" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(int64x2_t)
vmovl_s32(int32x2_t a) {
  int64x2_t result;
  __asm__("sshll %0.2d,%1.2s,#0" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(uint16x8_t)
vmovl_u8(uint8x8_t a) {
  uint16x8_t result;
  __asm__("ushll %0.8h,%1.8b,#0" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(uint32x4_t)
vmovl_u16(uint16x4_t a) {
  uint32x4_t result;
  __asm__("ushll %0.4s,%1.4h,#0" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(uint64x2_t)
vmovl_u32(uint32x2_t a) {
  uint64x2_t result;
  __asm__("ushll %0.2d,%1.2s,#0" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(int8x16_t)
vmovn_high_s16(int8x8_t a, int16x8_t b) {
  int8x16_t result = vcombine_s8(a, vcreate_s8(__AARCH64_UINT64_C(0x0)));
  __asm__("xtn2 %0.16b,%1.8h" : "+w"(result) : "w"(b) : /* No clobbers */);
  return result;
}

FUNK(int16x8_t)
vmovn_high_s32(int16x4_t a, int32x4_t b) {
  int16x8_t result = vcombine_s16(a, vcreate_s16(__AARCH64_UINT64_C(0x0)));
  __asm__("xtn2 %0.8h,%1.4s" : "+w"(result) : "w"(b) : /* No clobbers */);
  return result;
}

FUNK(int32x4_t)
vmovn_high_s64(int32x2_t a, int64x2_t b) {
  int32x4_t result = vcombine_s32(a, vcreate_s32(__AARCH64_UINT64_C(0x0)));
  __asm__("xtn2 %0.4s,%1.2d" : "+w"(result) : "w"(b) : /* No clobbers */);
  return result;
}

FUNK(uint8x16_t)
vmovn_high_u16(uint8x8_t a, uint16x8_t b) {
  uint8x16_t result = vcombine_u8(a, vcreate_u8(__AARCH64_UINT64_C(0x0)));
  __asm__("xtn2 %0.16b,%1.8h" : "+w"(result) : "w"(b) : /* No clobbers */);
  return result;
}

FUNK(uint16x8_t)
vmovn_high_u32(uint16x4_t a, uint32x4_t b) {
  uint16x8_t result = vcombine_u16(a, vcreate_u16(__AARCH64_UINT64_C(0x0)));
  __asm__("xtn2 %0.8h,%1.4s" : "+w"(result) : "w"(b) : /* No clobbers */);
  return result;
}

FUNK(uint32x4_t)
vmovn_high_u64(uint32x2_t a, uint64x2_t b) {
  uint32x4_t result = vcombine_u32(a, vcreate_u32(__AARCH64_UINT64_C(0x0)));
  __asm__("xtn2 %0.4s,%1.2d" : "+w"(result) : "w"(b) : /* No clobbers */);
  return result;
}

FUNK(int8x8_t)
vmovn_s16(int16x8_t a) {
  int8x8_t result;
  __asm__("xtn %0.8b,%1.8h" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(int16x4_t)
vmovn_s32(int32x4_t a) {
  int16x4_t result;
  __asm__("xtn %0.4h,%1.4s" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(int32x2_t)
vmovn_s64(int64x2_t a) {
  int32x2_t result;
  __asm__("xtn %0.2s,%1.2d" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(uint8x8_t)
vmovn_u16(uint16x8_t a) {
  uint8x8_t result;
  __asm__("xtn %0.8b,%1.8h" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(uint16x4_t)
vmovn_u32(uint32x4_t a) {
  uint16x4_t result;
  __asm__("xtn %0.4h,%1.4s" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(uint32x2_t)
vmovn_u64(uint64x2_t a) {
  uint32x2_t result;
  __asm__("xtn %0.2s,%1.2d" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

#define vmull_high_lane_s16(a, b, c)        \
  __extension__({                           \
    int16x4_t b_ = (b);                     \
    int16x8_t a_ = (a);                     \
    int32x4_t result;                       \
    __asm__("smull2 %0.4s, %1.8h, %2.h[%3]" \
            : "=w"(result)                  \
            : "w"(a_), "x"(b_), "i"(c)      \
            : /* No clobbers */);           \
    result;                                 \
  })

#define vmull_high_lane_s32(a, b, c)        \
  __extension__({                           \
    int32x2_t b_ = (b);                     \
    int32x4_t a_ = (a);                     \
    int64x2_t result;                       \
    __asm__("smull2 %0.2d, %1.4s, %2.s[%3]" \
            : "=w"(result)                  \
            : "w"(a_), "w"(b_), "i"(c)      \
            : /* No clobbers */);           \
    result;                                 \
  })

#define vmull_high_lane_u16(a, b, c)        \
  __extension__({                           \
    uint16x4_t b_ = (b);                    \
    uint16x8_t a_ = (a);                    \
    uint32x4_t result;                      \
    __asm__("umull2 %0.4s, %1.8h, %2.h[%3]" \
            : "=w"(result)                  \
            : "w"(a_), "x"(b_), "i"(c)      \
            : /* No clobbers */);           \
    result;                                 \
  })

#define vmull_high_lane_u32(a, b, c)        \
  __extension__({                           \
    uint32x2_t b_ = (b);                    \
    uint32x4_t a_ = (a);                    \
    uint64x2_t result;                      \
    __asm__("umull2 %0.2d, %1.4s, %2.s[%3]" \
            : "=w"(result)                  \
            : "w"(a_), "w"(b_), "i"(c)      \
            : /* No clobbers */);           \
    result;                                 \
  })

#define vmull_high_laneq_s16(a, b, c)       \
  __extension__({                           \
    int16x8_t b_ = (b);                     \
    int16x8_t a_ = (a);                     \
    int32x4_t result;                       \
    __asm__("smull2 %0.4s, %1.8h, %2.h[%3]" \
            : "=w"(result)                  \
            : "w"(a_), "x"(b_), "i"(c)      \
            : /* No clobbers */);           \
    result;                                 \
  })

#define vmull_high_laneq_s32(a, b, c)       \
  __extension__({                           \
    int32x4_t b_ = (b);                     \
    int32x4_t a_ = (a);                     \
    int64x2_t result;                       \
    __asm__("smull2 %0.2d, %1.4s, %2.s[%3]" \
            : "=w"(result)                  \
            : "w"(a_), "w"(b_), "i"(c)      \
            : /* No clobbers */);           \
    result;                                 \
  })

#define vmull_high_laneq_u16(a, b, c)       \
  __extension__({                           \
    uint16x8_t b_ = (b);                    \
    uint16x8_t a_ = (a);                    \
    uint32x4_t result;                      \
    __asm__("umull2 %0.4s, %1.8h, %2.h[%3]" \
            : "=w"(result)                  \
            : "w"(a_), "x"(b_), "i"(c)      \
            : /* No clobbers */);           \
    result;                                 \
  })

#define vmull_high_laneq_u32(a, b, c)       \
  __extension__({                           \
    uint32x4_t b_ = (b);                    \
    uint32x4_t a_ = (a);                    \
    uint64x2_t result;                      \
    __asm__("umull2 %0.2d, %1.4s, %2.s[%3]" \
            : "=w"(result)                  \
            : "w"(a_), "w"(b_), "i"(c)      \
            : /* No clobbers */);           \
    result;                                 \
  })

FUNK(int32x4_t)
vmull_high_n_s16(int16x8_t a, int16_t b) {
  int32x4_t result;
  __asm__("smull2 %0.4s,%1.8h,%2.h[0]"
          : "=w"(result)
          : "w"(a), "x"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int64x2_t)
vmull_high_n_s32(int32x4_t a, int32_t b) {
  int64x2_t result;
  __asm__("smull2 %0.2d,%1.4s,%2.s[0]"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(uint32x4_t)
vmull_high_n_u16(uint16x8_t a, uint16_t b) {
  uint32x4_t result;
  __asm__("umull2 %0.4s,%1.8h,%2.h[0]"
          : "=w"(result)
          : "w"(a), "x"(b)
          : /* No clobbers */);
  return result;
}

FUNK(uint64x2_t)
vmull_high_n_u32(uint32x4_t a, uint32_t b) {
  uint64x2_t result;
  __asm__("umull2 %0.2d,%1.4s,%2.s[0]"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(poly16x8_t)
vmull_high_p8(poly8x16_t a, poly8x16_t b) {
  poly16x8_t result;
  __asm__("pmull2 %0.8h,%1.16b,%2.16b"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int16x8_t)
vmull_high_s8(int8x16_t a, int8x16_t b) {
  int16x8_t result;
  __asm__("smull2 %0.8h,%1.16b,%2.16b"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int32x4_t)
vmull_high_s16(int16x8_t a, int16x8_t b) {
  int32x4_t result;
  __asm__("smull2 %0.4s,%1.8h,%2.8h"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int64x2_t)
vmull_high_s32(int32x4_t a, int32x4_t b) {
  int64x2_t result;
  __asm__("smull2 %0.2d,%1.4s,%2.4s"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(uint16x8_t)
vmull_high_u8(uint8x16_t a, uint8x16_t b) {
  uint16x8_t result;
  __asm__("umull2 %0.8h,%1.16b,%2.16b"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(uint32x4_t)
vmull_high_u16(uint16x8_t a, uint16x8_t b) {
  uint32x4_t result;
  __asm__("umull2 %0.4s,%1.8h,%2.8h"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(uint64x2_t)
vmull_high_u32(uint32x4_t a, uint32x4_t b) {
  uint64x2_t result;
  __asm__("umull2 %0.2d,%1.4s,%2.4s"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

#define vmull_lane_s16(a, b, c)          \
  __extension__({                        \
    int16x4_t b_ = (b);                  \
    int16x4_t a_ = (a);                  \
    int32x4_t result;                    \
    __asm__("smull %0.4s,%1.4h,%2.h[%3]" \
            : "=w"(result)               \
            : "w"(a_), "x"(b_), "i"(c)   \
            : /* No clobbers */);        \
    result;                              \
  })

#define vmull_lane_s32(a, b, c)          \
  __extension__({                        \
    int32x2_t b_ = (b);                  \
    int32x2_t a_ = (a);                  \
    int64x2_t result;                    \
    __asm__("smull %0.2d,%1.2s,%2.s[%3]" \
            : "=w"(result)               \
            : "w"(a_), "w"(b_), "i"(c)   \
            : /* No clobbers */);        \
    result;                              \
  })

#define vmull_lane_u16(a, b, c)          \
  __extension__({                        \
    uint16x4_t b_ = (b);                 \
    uint16x4_t a_ = (a);                 \
    uint32x4_t result;                   \
    __asm__("umull %0.4s,%1.4h,%2.h[%3]" \
            : "=w"(result)               \
            : "w"(a_), "x"(b_), "i"(c)   \
            : /* No clobbers */);        \
    result;                              \
  })

#define vmull_lane_u32(a, b, c)            \
  __extension__({                          \
    uint32x2_t b_ = (b);                   \
    uint32x2_t a_ = (a);                   \
    uint64x2_t result;                     \
    __asm__("umull %0.2d, %1.2s, %2.s[%3]" \
            : "=w"(result)                 \
            : "w"(a_), "w"(b_), "i"(c)     \
            : /* No clobbers */);          \
    result;                                \
  })

#define vmull_laneq_s16(a, b, c)           \
  __extension__({                          \
    int16x8_t b_ = (b);                    \
    int16x4_t a_ = (a);                    \
    int32x4_t result;                      \
    __asm__("smull %0.4s, %1.4h, %2.h[%3]" \
            : "=w"(result)                 \
            : "w"(a_), "x"(b_), "i"(c)     \
            : /* No clobbers */);          \
    result;                                \
  })

#define vmull_laneq_s32(a, b, c)           \
  __extension__({                          \
    int32x4_t b_ = (b);                    \
    int32x2_t a_ = (a);                    \
    int64x2_t result;                      \
    __asm__("smull %0.2d, %1.2s, %2.s[%3]" \
            : "=w"(result)                 \
            : "w"(a_), "w"(b_), "i"(c)     \
            : /* No clobbers */);          \
    result;                                \
  })

#define vmull_laneq_u16(a, b, c)           \
  __extension__({                          \
    uint16x8_t b_ = (b);                   \
    uint16x4_t a_ = (a);                   \
    uint32x4_t result;                     \
    __asm__("umull %0.4s, %1.4h, %2.h[%3]" \
            : "=w"(result)                 \
            : "w"(a_), "x"(b_), "i"(c)     \
            : /* No clobbers */);          \
    result;                                \
  })

#define vmull_laneq_u32(a, b, c)           \
  __extension__({                          \
    uint32x4_t b_ = (b);                   \
    uint32x2_t a_ = (a);                   \
    uint64x2_t result;                     \
    __asm__("umull %0.2d, %1.2s, %2.s[%3]" \
            : "=w"(result)                 \
            : "w"(a_), "w"(b_), "i"(c)     \
            : /* No clobbers */);          \
    result;                                \
  })

FUNK(int32x4_t)
vmull_n_s16(int16x4_t a, int16_t b) {
  int32x4_t result;
  __asm__("smull %0.4s,%1.4h,%2.h[0]"
          : "=w"(result)
          : "w"(a), "x"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int64x2_t)
vmull_n_s32(int32x2_t a, int32_t b) {
  int64x2_t result;
  __asm__("smull %0.2d,%1.2s,%2.s[0]"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(uint32x4_t)
vmull_n_u16(uint16x4_t a, uint16_t b) {
  uint32x4_t result;
  __asm__("umull %0.4s,%1.4h,%2.h[0]"
          : "=w"(result)
          : "w"(a), "x"(b)
          : /* No clobbers */);
  return result;
}

FUNK(uint64x2_t)
vmull_n_u32(uint32x2_t a, uint32_t b) {
  uint64x2_t result;
  __asm__("umull %0.2d,%1.2s,%2.s[0]"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(poly16x8_t)
vmull_p8(poly8x8_t a, poly8x8_t b) {
  poly16x8_t result;
  __asm__("pmull %0.8h, %1.8b, %2.8b"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int16x8_t)
vmull_s8(int8x8_t a, int8x8_t b) {
  int16x8_t result;
  __asm__("smull %0.8h, %1.8b, %2.8b"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int32x4_t)
vmull_s16(int16x4_t a, int16x4_t b) {
  int32x4_t result;
  __asm__("smull %0.4s, %1.4h, %2.4h"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int64x2_t)
vmull_s32(int32x2_t a, int32x2_t b) {
  int64x2_t result;
  __asm__("smull %0.2d, %1.2s, %2.2s"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(uint16x8_t)
vmull_u8(uint8x8_t a, uint8x8_t b) {
  uint16x8_t result;
  __asm__("umull %0.8h, %1.8b, %2.8b"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(uint32x4_t)
vmull_u16(uint16x4_t a, uint16x4_t b) {
  uint32x4_t result;
  __asm__("umull %0.4s, %1.4h, %2.4h"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(uint64x2_t)
vmull_u32(uint32x2_t a, uint32x2_t b) {
  uint64x2_t result;
  __asm__("umull %0.2d, %1.2s, %2.2s"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int16x4_t)
vpadal_s8(int16x4_t a, int8x8_t b) {
  int16x4_t result;
  __asm__("sadalp %0.4h,%2.8b"
          : "=w"(result)
          : "0"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int32x2_t)
vpadal_s16(int32x2_t a, int16x4_t b) {
  int32x2_t result;
  __asm__("sadalp %0.2s,%2.4h"
          : "=w"(result)
          : "0"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int64x1_t)
vpadal_s32(int64x1_t a, int32x2_t b) {
  int64x1_t result;
  __asm__("sadalp %0.1d,%2.2s"
          : "=w"(result)
          : "0"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(uint16x4_t)
vpadal_u8(uint16x4_t a, uint8x8_t b) {
  uint16x4_t result;
  __asm__("uadalp %0.4h,%2.8b"
          : "=w"(result)
          : "0"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(uint32x2_t)
vpadal_u16(uint32x2_t a, uint16x4_t b) {
  uint32x2_t result;
  __asm__("uadalp %0.2s,%2.4h"
          : "=w"(result)
          : "0"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(uint64x1_t)
vpadal_u32(uint64x1_t a, uint32x2_t b) {
  uint64x1_t result;
  __asm__("uadalp %0.1d,%2.2s"
          : "=w"(result)
          : "0"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int16x8_t)
vpadalq_s8(int16x8_t a, int8x16_t b) {
  int16x8_t result;
  __asm__("sadalp %0.8h,%2.16b"
          : "=w"(result)
          : "0"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int32x4_t)
vpadalq_s16(int32x4_t a, int16x8_t b) {
  int32x4_t result;
  __asm__("sadalp %0.4s,%2.8h"
          : "=w"(result)
          : "0"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int64x2_t)
vpadalq_s32(int64x2_t a, int32x4_t b) {
  int64x2_t result;
  __asm__("sadalp %0.2d,%2.4s"
          : "=w"(result)
          : "0"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(uint16x8_t)
vpadalq_u8(uint16x8_t a, uint8x16_t b) {
  uint16x8_t result;
  __asm__("uadalp %0.8h,%2.16b"
          : "=w"(result)
          : "0"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(uint32x4_t)
vpadalq_u16(uint32x4_t a, uint16x8_t b) {
  uint32x4_t result;
  __asm__("uadalp %0.4s,%2.8h"
          : "=w"(result)
          : "0"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(uint64x2_t)
vpadalq_u32(uint64x2_t a, uint32x4_t b) {
  uint64x2_t result;
  __asm__("uadalp %0.2d,%2.4s"
          : "=w"(result)
          : "0"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int16x4_t)
vpaddl_s8(int8x8_t a) {
  int16x4_t result;
  __asm__("saddlp %0.4h,%1.8b" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(int32x2_t)
vpaddl_s16(int16x4_t a) {
  int32x2_t result;
  __asm__("saddlp %0.2s,%1.4h" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(int64x1_t)
vpaddl_s32(int32x2_t a) {
  int64x1_t result;
  __asm__("saddlp %0.1d,%1.2s" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(uint16x4_t)
vpaddl_u8(uint8x8_t a) {
  uint16x4_t result;
  __asm__("uaddlp %0.4h,%1.8b" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(uint32x2_t)
vpaddl_u16(uint16x4_t a) {
  uint32x2_t result;
  __asm__("uaddlp %0.2s,%1.4h" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(uint64x1_t)
vpaddl_u32(uint32x2_t a) {
  uint64x1_t result;
  __asm__("uaddlp %0.1d,%1.2s" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(int16x8_t)
vpaddlq_s8(int8x16_t a) {
  int16x8_t result;
  __asm__("saddlp %0.8h,%1.16b" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(int32x4_t)
vpaddlq_s16(int16x8_t a) {
  int32x4_t result;
  __asm__("saddlp %0.4s,%1.8h" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(int64x2_t)
vpaddlq_s32(int32x4_t a) {
  int64x2_t result;
  __asm__("saddlp %0.2d,%1.4s" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(uint16x8_t)
vpaddlq_u8(uint8x16_t a) {
  uint16x8_t result;
  __asm__("uaddlp %0.8h,%1.16b" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(uint32x4_t)
vpaddlq_u16(uint16x8_t a) {
  uint32x4_t result;
  __asm__("uaddlp %0.4s,%1.8h" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(uint64x2_t)
vpaddlq_u32(uint32x4_t a) {
  uint64x2_t result;
  __asm__("uaddlp %0.2d,%1.4s" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

FUNK(int8x16_t)
vpaddq_s8(int8x16_t a, int8x16_t b) {
  int8x16_t result;
  __asm__("addp %0.16b,%1.16b,%2.16b"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int16x8_t)
vpaddq_s16(int16x8_t a, int16x8_t b) {
  int16x8_t result;
  __asm__("addp %0.8h,%1.8h,%2.8h"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int32x4_t)
vpaddq_s32(int32x4_t a, int32x4_t b) {
  int32x4_t result;
  __asm__("addp %0.4s,%1.4s,%2.4s"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int64x2_t)
vpaddq_s64(int64x2_t a, int64x2_t b) {
  int64x2_t result;
  __asm__("addp %0.2d,%1.2d,%2.2d"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(uint8x16_t)
vpaddq_u8(uint8x16_t a, uint8x16_t b) {
  uint8x16_t result;
  __asm__("addp %0.16b,%1.16b,%2.16b"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(uint16x8_t)
vpaddq_u16(uint16x8_t a, uint16x8_t b) {
  uint16x8_t result;
  __asm__("addp %0.8h,%1.8h,%2.8h"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(uint32x4_t)
vpaddq_u32(uint32x4_t a, uint32x4_t b) {
  uint32x4_t result;
  __asm__("addp %0.4s,%1.4s,%2.4s"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(uint64x2_t)
vpaddq_u64(uint64x2_t a, uint64x2_t b) {
  uint64x2_t result;
  __asm__("addp %0.2d,%1.2d,%2.2d"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int16x4_t)
vqdmulh_n_s16(int16x4_t a, int16_t b) {
  int16x4_t result;
  __asm__("sqdmulh %0.4h,%1.4h,%2.h[0]"
          : "=w"(result)
          : "w"(a), "x"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int32x2_t)
vqdmulh_n_s32(int32x2_t a, int32_t b) {
  int32x2_t result;
  __asm__("sqdmulh %0.2s,%1.2s,%2.s[0]"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int16x8_t)
vqdmulhq_n_s16(int16x8_t a, int16_t b) {
  int16x8_t result;
  __asm__("sqdmulh %0.8h,%1.8h,%2.h[0]"
          : "=w"(result)
          : "w"(a), "x"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int32x4_t)
vqdmulhq_n_s32(int32x4_t a, int32_t b) {
  int32x4_t result;
  __asm__("sqdmulh %0.4s,%1.4s,%2.s[0]"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int8x16_t)
vqmovn_high_s16(int8x8_t a, int16x8_t b) {
  int8x16_t result = vcombine_s8(a, vcreate_s8(__AARCH64_UINT64_C(0x0)));
  __asm__("sqxtn2 %0.16b, %1.8h" : "+w"(result) : "w"(b) : /* No clobbers */);
  return result;
}

FUNK(int16x8_t)
vqmovn_high_s32(int16x4_t a, int32x4_t b) {
  int16x8_t result = vcombine_s16(a, vcreate_s16(__AARCH64_UINT64_C(0x0)));
  __asm__("sqxtn2 %0.8h, %1.4s" : "+w"(result) : "w"(b) : /* No clobbers */);
  return result;
}

FUNK(int32x4_t)
vqmovn_high_s64(int32x2_t a, int64x2_t b) {
  int32x4_t result = vcombine_s32(a, vcreate_s32(__AARCH64_UINT64_C(0x0)));
  __asm__("sqxtn2 %0.4s, %1.2d" : "+w"(result) : "w"(b) : /* No clobbers */);
  return result;
}

FUNK(uint8x16_t)
vqmovn_high_u16(uint8x8_t a, uint16x8_t b) {
  uint8x16_t result = vcombine_u8(a, vcreate_u8(__AARCH64_UINT64_C(0x0)));
  __asm__("uqxtn2 %0.16b, %1.8h" : "+w"(result) : "w"(b) : /* No clobbers */);
  return result;
}

FUNK(uint16x8_t)
vqmovn_high_u32(uint16x4_t a, uint32x4_t b) {
  uint16x8_t result = vcombine_u16(a, vcreate_u16(__AARCH64_UINT64_C(0x0)));
  __asm__("uqxtn2 %0.8h, %1.4s" : "+w"(result) : "w"(b) : /* No clobbers */);
  return result;
}

FUNK(uint32x4_t)
vqmovn_high_u64(uint32x2_t a, uint64x2_t b) {
  uint32x4_t result = vcombine_u32(a, vcreate_u32(__AARCH64_UINT64_C(0x0)));
  __asm__("uqxtn2 %0.4s, %1.2d" : "+w"(result) : "w"(b) : /* No clobbers */);
  return result;
}

FUNK(uint8x16_t)
vqmovun_high_s16(uint8x8_t a, int16x8_t b) {
  uint8x16_t result = vcombine_u8(a, vcreate_u8(__AARCH64_UINT64_C(0x0)));
  __asm__("sqxtun2 %0.16b, %1.8h" : "+w"(result) : "w"(b) : /* No clobbers */);
  return result;
}

FUNK(uint16x8_t)
vqmovun_high_s32(uint16x4_t a, int32x4_t b) {
  uint16x8_t result = vcombine_u16(a, vcreate_u16(__AARCH64_UINT64_C(0x0)));
  __asm__("sqxtun2 %0.8h, %1.4s" : "+w"(result) : "w"(b) : /* No clobbers */);
  return result;
}

FUNK(uint32x4_t)
vqmovun_high_s64(uint32x2_t a, int64x2_t b) {
  uint32x4_t result = vcombine_u32(a, vcreate_u32(__AARCH64_UINT64_C(0x0)));
  __asm__("sqxtun2 %0.4s, %1.2d" : "+w"(result) : "w"(b) : /* No clobbers */);
  return result;
}

FUNK(int16x4_t)
vqrdmulh_n_s16(int16x4_t a, int16_t b) {
  int16x4_t result;
  __asm__("sqrdmulh %0.4h,%1.4h,%2.h[0]"
          : "=w"(result)
          : "w"(a), "x"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int32x2_t)
vqrdmulh_n_s32(int32x2_t a, int32_t b) {
  int32x2_t result;
  __asm__("sqrdmulh %0.2s,%1.2s,%2.s[0]"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int16x8_t)
vqrdmulhq_n_s16(int16x8_t a, int16_t b) {
  int16x8_t result;
  __asm__("sqrdmulh %0.8h,%1.8h,%2.h[0]"
          : "=w"(result)
          : "w"(a), "x"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int32x4_t)
vqrdmulhq_n_s32(int32x4_t a, int32_t b) {
  int32x4_t result;
  __asm__("sqrdmulh %0.4s,%1.4s,%2.s[0]"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

#define vqrshrn_high_n_s16(a, b, c)                                          \
  __extension__({                                                            \
    int16x8_t b_ = (b);                                                      \
    int8x8_t a_ = (a);                                                       \
    int8x16_t result = vcombine_s8(a_, vcreate_s8(__AARCH64_UINT64_C(0x0))); \
    __asm__("sqrshrn2 %0.16b, %1.8h, #%2"                                    \
            : "+w"(result)                                                   \
            : "w"(b_), "i"(c)                                                \
            : /* No clobbers */);                                            \
    result;                                                                  \
  })

#define vqrshrn_high_n_s32(a, b, c)                                            \
  __extension__({                                                              \
    int32x4_t b_ = (b);                                                        \
    int16x4_t a_ = (a);                                                        \
    int16x8_t result = vcombine_s16(a_, vcreate_s16(__AARCH64_UINT64_C(0x0))); \
    __asm__("sqrshrn2 %0.8h, %1.4s, #%2"                                       \
            : "+w"(result)                                                     \
            : "w"(b_), "i"(c)                                                  \
            : /* No clobbers */);                                              \
    result;                                                                    \
  })

#define vqrshrn_high_n_s64(a, b, c)                                            \
  __extension__({                                                              \
    int64x2_t b_ = (b);                                                        \
    int32x2_t a_ = (a);                                                        \
    int32x4_t result = vcombine_s32(a_, vcreate_s32(__AARCH64_UINT64_C(0x0))); \
    __asm__("sqrshrn2 %0.4s, %1.2d, #%2"                                       \
            : "+w"(result)                                                     \
            : "w"(b_), "i"(c)                                                  \
            : /* No clobbers */);                                              \
    result;                                                                    \
  })

#define vqrshrn_high_n_u16(a, b, c)                                           \
  __extension__({                                                             \
    uint16x8_t b_ = (b);                                                      \
    uint8x8_t a_ = (a);                                                       \
    uint8x16_t result = vcombine_u8(a_, vcreate_u8(__AARCH64_UINT64_C(0x0))); \
    __asm__("uqrshrn2 %0.16b, %1.8h, #%2"                                     \
            : "+w"(result)                                                    \
            : "w"(b_), "i"(c)                                                 \
            : /* No clobbers */);                                             \
    result;                                                                   \
  })

#define vqrshrn_high_n_u32(a, b, c)                             \
  __extension__({                                               \
    uint32x4_t b_ = (b);                                        \
    uint16x4_t a_ = (a);                                        \
    uint16x8_t result =                                         \
        vcombine_u16(a_, vcreate_u16(__AARCH64_UINT64_C(0x0))); \
    __asm__("uqrshrn2 %0.8h, %1.4s, #%2"                        \
            : "+w"(result)                                      \
            : "w"(b_), "i"(c)                                   \
            : /* No clobbers */);                               \
    result;                                                     \
  })

#define vqrshrn_high_n_u64(a, b, c)                             \
  __extension__({                                               \
    uint64x2_t b_ = (b);                                        \
    uint32x2_t a_ = (a);                                        \
    uint32x4_t result =                                         \
        vcombine_u32(a_, vcreate_u32(__AARCH64_UINT64_C(0x0))); \
    __asm__("uqrshrn2 %0.4s, %1.2d, #%2"                        \
            : "+w"(result)                                      \
            : "w"(b_), "i"(c)                                   \
            : /* No clobbers */);                               \
    result;                                                     \
  })

#define vqrshrun_high_n_s16(a, b, c)                                          \
  __extension__({                                                             \
    int16x8_t b_ = (b);                                                       \
    uint8x8_t a_ = (a);                                                       \
    uint8x16_t result = vcombine_u8(a_, vcreate_u8(__AARCH64_UINT64_C(0x0))); \
    __asm__("sqrshrun2 %0.16b, %1.8h, #%2"                                    \
            : "+w"(result)                                                    \
            : "w"(b_), "i"(c)                                                 \
            : /* No clobbers */);                                             \
    result;                                                                   \
  })

#define vqrshrun_high_n_s32(a, b, c)                            \
  __extension__({                                               \
    int32x4_t b_ = (b);                                         \
    uint16x4_t a_ = (a);                                        \
    uint16x8_t result =                                         \
        vcombine_u16(a_, vcreate_u16(__AARCH64_UINT64_C(0x0))); \
    __asm__("sqrshrun2 %0.8h, %1.4s, #%2"                       \
            : "+w"(result)                                      \
            : "w"(b_), "i"(c)                                   \
            : /* No clobbers */);                               \
    result;                                                     \
  })

#define vqrshrun_high_n_s64(a, b, c)                            \
  __extension__({                                               \
    int64x2_t b_ = (b);                                         \
    uint32x2_t a_ = (a);                                        \
    uint32x4_t result =                                         \
        vcombine_u32(a_, vcreate_u32(__AARCH64_UINT64_C(0x0))); \
    __asm__("sqrshrun2 %0.4s, %1.2d, #%2"                       \
            : "+w"(result)                                      \
            : "w"(b_), "i"(c)                                   \
            : /* No clobbers */);                               \
    result;                                                     \
  })

#define vqshrn_high_n_s16(a, b, c)                                           \
  __extension__({                                                            \
    int16x8_t b_ = (b);                                                      \
    int8x8_t a_ = (a);                                                       \
    int8x16_t result = vcombine_s8(a_, vcreate_s8(__AARCH64_UINT64_C(0x0))); \
    __asm__("sqshrn2 %0.16b, %1.8h, #%2"                                     \
            : "+w"(result)                                                   \
            : "w"(b_), "i"(c)                                                \
            : /* No clobbers */);                                            \
    result;                                                                  \
  })

#define vqshrn_high_n_s32(a, b, c)                                             \
  __extension__({                                                              \
    int32x4_t b_ = (b);                                                        \
    int16x4_t a_ = (a);                                                        \
    int16x8_t result = vcombine_s16(a_, vcreate_s16(__AARCH64_UINT64_C(0x0))); \
    __asm__("sqshrn2 %0.8h, %1.4s, #%2"                                        \
            : "+w"(result)                                                     \
            : "w"(b_), "i"(c)                                                  \
            : /* No clobbers */);                                              \
    result;                                                                    \
  })

#define vqshrn_high_n_s64(a, b, c)                                             \
  __extension__({                                                              \
    int64x2_t b_ = (b);                                                        \
    int32x2_t a_ = (a);                                                        \
    int32x4_t result = vcombine_s32(a_, vcreate_s32(__AARCH64_UINT64_C(0x0))); \
    __asm__("sqshrn2 %0.4s, %1.2d, #%2"                                        \
            : "+w"(result)                                                     \
            : "w"(b_), "i"(c)                                                  \
            : /* No clobbers */);                                              \
    result;                                                                    \
  })

#define vqshrn_high_n_u16(a, b, c)                                            \
  __extension__({                                                             \
    uint16x8_t b_ = (b);                                                      \
    uint8x8_t a_ = (a);                                                       \
    uint8x16_t result = vcombine_u8(a_, vcreate_u8(__AARCH64_UINT64_C(0x0))); \
    __asm__("uqshrn2 %0.16b, %1.8h, #%2"                                      \
            : "+w"(result)                                                    \
            : "w"(b_), "i"(c)                                                 \
            : /* No clobbers */);                                             \
    result;                                                                   \
  })

#define vqshrn_high_n_u32(a, b, c)                              \
  __extension__({                                               \
    uint32x4_t b_ = (b);                                        \
    uint16x4_t a_ = (a);                                        \
    uint16x8_t result =                                         \
        vcombine_u16(a_, vcreate_u16(__AARCH64_UINT64_C(0x0))); \
    __asm__("uqshrn2 %0.8h, %1.4s, #%2"                         \
            : "+w"(result)                                      \
            : "w"(b_), "i"(c)                                   \
            : /* No clobbers */);                               \
    result;                                                     \
  })

#define vqshrn_high_n_u64(a, b, c)                              \
  __extension__({                                               \
    uint64x2_t b_ = (b);                                        \
    uint32x2_t a_ = (a);                                        \
    uint32x4_t result =                                         \
        vcombine_u32(a_, vcreate_u32(__AARCH64_UINT64_C(0x0))); \
    __asm__("uqshrn2 %0.4s, %1.2d, #%2"                         \
            : "+w"(result)                                      \
            : "w"(b_), "i"(c)                                   \
            : /* No clobbers */);                               \
    result;                                                     \
  })

#define vqshrun_high_n_s16(a, b, c)                                           \
  __extension__({                                                             \
    int16x8_t b_ = (b);                                                       \
    uint8x8_t a_ = (a);                                                       \
    uint8x16_t result = vcombine_u8(a_, vcreate_u8(__AARCH64_UINT64_C(0x0))); \
    __asm__("sqshrun2 %0.16b, %1.8h, #%2"                                     \
            : "+w"(result)                                                    \
            : "w"(b_), "i"(c)                                                 \
            : /* No clobbers */);                                             \
    result;                                                                   \
  })

#define vqshrun_high_n_s32(a, b, c)                             \
  __extension__({                                               \
    int32x4_t b_ = (b);                                         \
    uint16x4_t a_ = (a);                                        \
    uint16x8_t result =                                         \
        vcombine_u16(a_, vcreate_u16(__AARCH64_UINT64_C(0x0))); \
    __asm__("sqshrun2 %0.8h, %1.4s, #%2"                        \
            : "+w"(result)                                      \
            : "w"(b_), "i"(c)                                   \
            : /* No clobbers */);                               \
    result;                                                     \
  })

#define vqshrun_high_n_s64(a, b, c)                             \
  __extension__({                                               \
    int64x2_t b_ = (b);                                         \
    uint32x2_t a_ = (a);                                        \
    uint32x4_t result =                                         \
        vcombine_u32(a_, vcreate_u32(__AARCH64_UINT64_C(0x0))); \
    __asm__("sqshrun2 %0.4s, %1.2d, #%2"                        \
            : "+w"(result)                                      \
            : "w"(b_), "i"(c)                                   \
            : /* No clobbers */);                               \
    result;                                                     \
  })

#define vrshrn_high_n_s16(a, b, c)                                           \
  __extension__({                                                            \
    int16x8_t b_ = (b);                                                      \
    int8x8_t a_ = (a);                                                       \
    int8x16_t result = vcombine_s8(a_, vcreate_s8(__AARCH64_UINT64_C(0x0))); \
    __asm__("rshrn2 %0.16b,%1.8h,#%2"                                        \
            : "+w"(result)                                                   \
            : "w"(b_), "i"(c)                                                \
            : /* No clobbers */);                                            \
    result;                                                                  \
  })

#define vrshrn_high_n_s32(a, b, c)                                             \
  __extension__({                                                              \
    int32x4_t b_ = (b);                                                        \
    int16x4_t a_ = (a);                                                        \
    int16x8_t result = vcombine_s16(a_, vcreate_s16(__AARCH64_UINT64_C(0x0))); \
    __asm__("rshrn2 %0.8h,%1.4s,#%2"                                           \
            : "+w"(result)                                                     \
            : "w"(b_), "i"(c)                                                  \
            : /* No clobbers */);                                              \
    result;                                                                    \
  })

#define vrshrn_high_n_s64(a, b, c)                                             \
  __extension__({                                                              \
    int64x2_t b_ = (b);                                                        \
    int32x2_t a_ = (a);                                                        \
    int32x4_t result = vcombine_s32(a_, vcreate_s32(__AARCH64_UINT64_C(0x0))); \
    __asm__("rshrn2 %0.4s,%1.2d,#%2"                                           \
            : "+w"(result)                                                     \
            : "w"(b_), "i"(c)                                                  \
            : /* No clobbers */);                                              \
    result;                                                                    \
  })

#define vrshrn_high_n_u16(a, b, c)                                            \
  __extension__({                                                             \
    uint16x8_t b_ = (b);                                                      \
    uint8x8_t a_ = (a);                                                       \
    uint8x16_t result = vcombine_u8(a_, vcreate_u8(__AARCH64_UINT64_C(0x0))); \
    __asm__("rshrn2 %0.16b,%1.8h,#%2"                                         \
            : "+w"(result)                                                    \
            : "w"(b_), "i"(c)                                                 \
            : /* No clobbers */);                                             \
    result;                                                                   \
  })

#define vrshrn_high_n_u32(a, b, c)                              \
  __extension__({                                               \
    uint32x4_t b_ = (b);                                        \
    uint16x4_t a_ = (a);                                        \
    uint16x8_t result =                                         \
        vcombine_u16(a_, vcreate_u16(__AARCH64_UINT64_C(0x0))); \
    __asm__("rshrn2 %0.8h,%1.4s,#%2"                            \
            : "+w"(result)                                      \
            : "w"(b_), "i"(c)                                   \
            : /* No clobbers */);                               \
    result;                                                     \
  })

#define vrshrn_high_n_u64(a, b, c)                              \
  __extension__({                                               \
    uint64x2_t b_ = (b);                                        \
    uint32x2_t a_ = (a);                                        \
    uint32x4_t result =                                         \
        vcombine_u32(a_, vcreate_u32(__AARCH64_UINT64_C(0x0))); \
    __asm__("rshrn2 %0.4s,%1.2d,#%2"                            \
            : "+w"(result)                                      \
            : "w"(b_), "i"(c)                                   \
            : /* No clobbers */);                               \
    result;                                                     \
  })

#define vrshrn_n_s16(a, b)         \
  __extension__({                  \
    int16x8_t a_ = (a);            \
    int8x8_t result;               \
    __asm__("rshrn %0.8b,%1.8h,%2" \
            : "=w"(result)         \
            : "w"(a_), "i"(b)      \
            : /* No clobbers */);  \
    result;                        \
  })

#define vrshrn_n_s32(a, b)         \
  __extension__({                  \
    int32x4_t a_ = (a);            \
    int16x4_t result;              \
    __asm__("rshrn %0.4h,%1.4s,%2" \
            : "=w"(result)         \
            : "w"(a_), "i"(b)      \
            : /* No clobbers */);  \
    result;                        \
  })

#define vrshrn_n_s64(a, b)         \
  __extension__({                  \
    int64x2_t a_ = (a);            \
    int32x2_t result;              \
    __asm__("rshrn %0.2s,%1.2d,%2" \
            : "=w"(result)         \
            : "w"(a_), "i"(b)      \
            : /* No clobbers */);  \
    result;                        \
  })

#define vrshrn_n_u16(a, b)         \
  __extension__({                  \
    uint16x8_t a_ = (a);           \
    uint8x8_t result;              \
    __asm__("rshrn %0.8b,%1.8h,%2" \
            : "=w"(result)         \
            : "w"(a_), "i"(b)      \
            : /* No clobbers */);  \
    result;                        \
  })

#define vrshrn_n_u32(a, b)         \
  __extension__({                  \
    uint32x4_t a_ = (a);           \
    uint16x4_t result;             \
    __asm__("rshrn %0.4h,%1.4s,%2" \
            : "=w"(result)         \
            : "w"(a_), "i"(b)      \
            : /* No clobbers */);  \
    result;                        \
  })

#define vrshrn_n_u64(a, b)         \
  __extension__({                  \
    uint64x2_t a_ = (a);           \
    uint32x2_t result;             \
    __asm__("rshrn %0.2s,%1.2d,%2" \
            : "=w"(result)         \
            : "w"(a_), "i"(b)      \
            : /* No clobbers */);  \
    result;                        \
  })

__extension__ extern __inline uint32x2_t
    __attribute__((__always_inline__, __gnu_inline__, __artificial__))
    vrsqrte_u32(uint32x2_t a) {
  uint32x2_t result;
  __asm__("ursqrte %0.2s,%1.2s" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

__extension__ extern __inline uint32x4_t
    __attribute__((__always_inline__, __gnu_inline__, __artificial__))
    vrsqrteq_u32(uint32x4_t a) {
  uint32x4_t result;
  __asm__("ursqrte %0.4s,%1.4s" : "=w"(result) : "w"(a) : /* No clobbers */);
  return result;
}

#define vshrn_high_n_s16(a, b, c)                                            \
  __extension__({                                                            \
    int16x8_t b_ = (b);                                                      \
    int8x8_t a_ = (a);                                                       \
    int8x16_t result = vcombine_s8(a_, vcreate_s8(__AARCH64_UINT64_C(0x0))); \
    __asm__("shrn2 %0.16b,%1.8h,#%2"                                         \
            : "+w"(result)                                                   \
            : "w"(b_), "i"(c)                                                \
            : /* No clobbers */);                                            \
    result;                                                                  \
  })

#define vshrn_high_n_s32(a, b, c)                                              \
  __extension__({                                                              \
    int32x4_t b_ = (b);                                                        \
    int16x4_t a_ = (a);                                                        \
    int16x8_t result = vcombine_s16(a_, vcreate_s16(__AARCH64_UINT64_C(0x0))); \
    __asm__("shrn2 %0.8h,%1.4s,#%2"                                            \
            : "+w"(result)                                                     \
            : "w"(b_), "i"(c)                                                  \
            : /* No clobbers */);                                              \
    result;                                                                    \
  })

#define vshrn_high_n_s64(a, b, c)                                              \
  __extension__({                                                              \
    int64x2_t b_ = (b);                                                        \
    int32x2_t a_ = (a);                                                        \
    int32x4_t result = vcombine_s32(a_, vcreate_s32(__AARCH64_UINT64_C(0x0))); \
    __asm__("shrn2 %0.4s,%1.2d,#%2"                                            \
            : "+w"(result)                                                     \
            : "w"(b_), "i"(c)                                                  \
            : /* No clobbers */);                                              \
    result;                                                                    \
  })

#define vshrn_high_n_u16(a, b, c)                                             \
  __extension__({                                                             \
    uint16x8_t b_ = (b);                                                      \
    uint8x8_t a_ = (a);                                                       \
    uint8x16_t result = vcombine_u8(a_, vcreate_u8(__AARCH64_UINT64_C(0x0))); \
    __asm__("shrn2 %0.16b,%1.8h,#%2"                                          \
            : "+w"(result)                                                    \
            : "w"(b_), "i"(c)                                                 \
            : /* No clobbers */);                                             \
    result;                                                                   \
  })

#define vshrn_high_n_u32(a, b, c)                               \
  __extension__({                                               \
    uint32x4_t b_ = (b);                                        \
    uint16x4_t a_ = (a);                                        \
    uint16x8_t result =                                         \
        vcombine_u16(a_, vcreate_u16(__AARCH64_UINT64_C(0x0))); \
    __asm__("shrn2 %0.8h,%1.4s,#%2"                             \
            : "+w"(result)                                      \
            : "w"(b_), "i"(c)                                   \
            : /* No clobbers */);                               \
    result;                                                     \
  })

#define vshrn_high_n_u64(a, b, c)                               \
  __extension__({                                               \
    uint64x2_t b_ = (b);                                        \
    uint32x2_t a_ = (a);                                        \
    uint32x4_t result =                                         \
        vcombine_u32(a_, vcreate_u32(__AARCH64_UINT64_C(0x0))); \
    __asm__("shrn2 %0.4s,%1.2d,#%2"                             \
            : "+w"(result)                                      \
            : "w"(b_), "i"(c)                                   \
            : /* No clobbers */);                               \
    result;                                                     \
  })

#define vshrn_n_s16(a, b)         \
  __extension__({                 \
    int16x8_t a_ = (a);           \
    int8x8_t result;              \
    __asm__("shrn %0.8b,%1.8h,%2" \
            : "=w"(result)        \
            : "w"(a_), "i"(b)     \
            : /* No clobbers */); \
    result;                       \
  })

#define vshrn_n_s32(a, b)         \
  __extension__({                 \
    int32x4_t a_ = (a);           \
    int16x4_t result;             \
    __asm__("shrn %0.4h,%1.4s,%2" \
            : "=w"(result)        \
            : "w"(a_), "i"(b)     \
            : /* No clobbers */); \
    result;                       \
  })

#define vshrn_n_s64(a, b)         \
  __extension__({                 \
    int64x2_t a_ = (a);           \
    int32x2_t result;             \
    __asm__("shrn %0.2s,%1.2d,%2" \
            : "=w"(result)        \
            : "w"(a_), "i"(b)     \
            : /* No clobbers */); \
    result;                       \
  })

#define vshrn_n_u16(a, b)         \
  __extension__({                 \
    uint16x8_t a_ = (a);          \
    uint8x8_t result;             \
    __asm__("shrn %0.8b,%1.8h,%2" \
            : "=w"(result)        \
            : "w"(a_), "i"(b)     \
            : /* No clobbers */); \
    result;                       \
  })

#define vshrn_n_u32(a, b)         \
  __extension__({                 \
    uint32x4_t a_ = (a);          \
    uint16x4_t result;            \
    __asm__("shrn %0.4h,%1.4s,%2" \
            : "=w"(result)        \
            : "w"(a_), "i"(b)     \
            : /* No clobbers */); \
    result;                       \
  })

#define vshrn_n_u64(a, b)         \
  __extension__({                 \
    uint64x2_t a_ = (a);          \
    uint32x2_t result;            \
    __asm__("shrn %0.2s,%1.2d,%2" \
            : "=w"(result)        \
            : "w"(a_), "i"(b)     \
            : /* No clobbers */); \
    result;                       \
  })

#define vsli_n_p8(a, b, c)             \
  __extension__({                      \
    poly8x8_t b_ = (b);                \
    poly8x8_t a_ = (a);                \
    poly8x8_t result;                  \
    __asm__("sli %0.8b,%2.8b,%3"       \
            : "=w"(result)             \
            : "0"(a_), "w"(b_), "i"(c) \
            : /* No clobbers */);      \
    result;                            \
  })

#define vsli_n_p16(a, b, c)            \
  __extension__({                      \
    poly16x4_t b_ = (b);               \
    poly16x4_t a_ = (a);               \
    poly16x4_t result;                 \
    __asm__("sli %0.4h,%2.4h,%3"       \
            : "=w"(result)             \
            : "0"(a_), "w"(b_), "i"(c) \
            : /* No clobbers */);      \
    result;                            \
  })

#define vsliq_n_p8(a, b, c)            \
  __extension__({                      \
    poly8x16_t b_ = (b);               \
    poly8x16_t a_ = (a);               \
    poly8x16_t result;                 \
    __asm__("sli %0.16b,%2.16b,%3"     \
            : "=w"(result)             \
            : "0"(a_), "w"(b_), "i"(c) \
            : /* No clobbers */);      \
    result;                            \
  })

#define vsliq_n_p16(a, b, c)           \
  __extension__({                      \
    poly16x8_t b_ = (b);               \
    poly16x8_t a_ = (a);               \
    poly16x8_t result;                 \
    __asm__("sli %0.8h,%2.8h,%3"       \
            : "=w"(result)             \
            : "0"(a_), "w"(b_), "i"(c) \
            : /* No clobbers */);      \
    result;                            \
  })

#define vsri_n_p8(a, b, c)             \
  __extension__({                      \
    poly8x8_t b_ = (b);                \
    poly8x8_t a_ = (a);                \
    poly8x8_t result;                  \
    __asm__("sri %0.8b,%2.8b,%3"       \
            : "=w"(result)             \
            : "0"(a_), "w"(b_), "i"(c) \
            : /* No clobbers */);      \
    result;                            \
  })

#define vsri_n_p16(a, b, c)            \
  __extension__({                      \
    poly16x4_t b_ = (b);               \
    poly16x4_t a_ = (a);               \
    poly16x4_t result;                 \
    __asm__("sri %0.4h,%2.4h,%3"       \
            : "=w"(result)             \
            : "0"(a_), "w"(b_), "i"(c) \
            : /* No clobbers */);      \
    result;                            \
  })

#define vsri_n_p64(a, b, c)            \
  __extension__({                      \
    poly64x1_t b_ = (b);               \
    poly64x1_t a_ = (a);               \
    poly64x1_t result;                 \
    __asm__("sri %d0,%d2,%3"           \
            : "=w"(result)             \
            : "0"(a_), "w"(b_), "i"(c) \
            : /* No clobbers.  */);    \
    result;                            \
  })

#define vsriq_n_p8(a, b, c)            \
  __extension__({                      \
    poly8x16_t b_ = (b);               \
    poly8x16_t a_ = (a);               \
    poly8x16_t result;                 \
    __asm__("sri %0.16b,%2.16b,%3"     \
            : "=w"(result)             \
            : "0"(a_), "w"(b_), "i"(c) \
            : /* No clobbers */);      \
    result;                            \
  })

#define vsriq_n_p16(a, b, c)           \
  __extension__({                      \
    poly16x8_t b_ = (b);               \
    poly16x8_t a_ = (a);               \
    poly16x8_t result;                 \
    __asm__("sri %0.8h,%2.8h,%3"       \
            : "=w"(result)             \
            : "0"(a_), "w"(b_), "i"(c) \
            : /* No clobbers */);      \
    result;                            \
  })

#define vsriq_n_p64(a, b, c)           \
  __extension__({                      \
    poly64x2_t b_ = (b);               \
    poly64x2_t a_ = (a);               \
    poly64x2_t result;                 \
    __asm__("sri %0.2d,%2.2d,%3"       \
            : "=w"(result)             \
            : "0"(a_), "w"(b_), "i"(c) \
            : /* No clobbers.  */);    \
    result;                            \
  })

__extension__ extern __inline uint8x8_t
    __attribute__((__always_inline__, __gnu_inline__, __artificial__))
    vtst_p8(poly8x8_t a, poly8x8_t b) {
  return (uint8x8_t)((((uint8x8_t)a) & ((uint8x8_t)b)) != 0);
}

__extension__ extern __inline uint16x4_t
    __attribute__((__always_inline__, __gnu_inline__, __artificial__))
    vtst_p16(poly16x4_t a, poly16x4_t b) {
  return (uint16x4_t)((((uint16x4_t)a) & ((uint16x4_t)b)) != 0);
}

__extension__ extern __inline uint64x1_t
    __attribute__((__always_inline__, __gnu_inline__, __artificial__))
    vtst_p64(poly64x1_t a, poly64x1_t b) {
  return (uint64x1_t)((a & b) != __AARCH64_INT64_C(0));
}

__extension__ extern __inline uint8x16_t
    __attribute__((__always_inline__, __gnu_inline__, __artificial__))
    vtstq_p8(poly8x16_t a, poly8x16_t b) {
  return (uint8x16_t)((((uint8x16_t)a) & ((uint8x16_t)b)) != 0);
}

__extension__ extern __inline uint16x8_t
    __attribute__((__always_inline__, __gnu_inline__, __artificial__))
    vtstq_p16(poly16x8_t a, poly16x8_t b) {
  return (uint16x8_t)((((uint16x8_t)a) & ((uint16x8_t)b)) != 0);
}

__extension__ extern __inline uint64x2_t
    __attribute__((__always_inline__, __gnu_inline__, __artificial__))
    vtstq_p64(poly64x2_t a, poly64x2_t b) {
  return (uint64x2_t)((((uint64x2_t)a) & ((uint64x2_t)b)) !=
                      __AARCH64_INT64_C(0));
}

#define __STRUCTN(t, sz, nelem)        \
  typedef struct t##sz##x##nelem##_t { \
    t##sz##_t val[nelem];              \
  } t##sz##x##nelem##_t;

__STRUCTN(int, 8, 2)
__STRUCTN(int, 16, 2)
__STRUCTN(uint, 8, 2)
__STRUCTN(uint, 16, 2)
__STRUCTN(float, 16, 2)
__STRUCTN(poly, 8, 2)
__STRUCTN(poly, 16, 2)
__STRUCTN(int, 8, 3)
__STRUCTN(int, 16, 3)
__STRUCTN(int, 32, 3)
__STRUCTN(int, 64, 3)
__STRUCTN(uint, 8, 3)
__STRUCTN(uint, 16, 3)
__STRUCTN(uint, 32, 3)
__STRUCTN(uint, 64, 3)
__STRUCTN(float, 16, 3)
__STRUCTN(float, 32, 3)
__STRUCTN(float, 64, 3)
__STRUCTN(poly, 8, 3)
__STRUCTN(poly, 16, 3)
__STRUCTN(int, 8, 4)
__STRUCTN(int, 64, 4)
__STRUCTN(uint, 8, 4)
__STRUCTN(uint, 64, 4)
__STRUCTN(poly, 8, 4)
__STRUCTN(float, 64, 4)
#undef __STRUCTN

#define __ST2_LANE_FUNC(intype, largetype, ptrtype, mode, qmode, ptr_mode,    \
                        funcsuffix, signedtype)                               \
  __extension__ extern __inline void                                          \
      __attribute__((__always_inline__, __gnu_inline__, __artificial__))      \
      vst2_lane_##funcsuffix(ptrtype *__ptr, intype __b, const int __c) {     \
    __builtin_aarch64_simd_oi __o;                                            \
    largetype __temp;                                                         \
    __temp.val[0] = vcombine_##funcsuffix(                                    \
        __b.val[0], vcreate_##funcsuffix(__AARCH64_UINT64_C(0)));             \
    __temp.val[1] = vcombine_##funcsuffix(                                    \
        __b.val[1], vcreate_##funcsuffix(__AARCH64_UINT64_C(0)));             \
    __o = __builtin_aarch64_set_qregoi##qmode(__o, (signedtype)__temp.val[0], \
                                              0);                             \
    __o = __builtin_aarch64_set_qregoi##qmode(__o, (signedtype)__temp.val[1], \
                                              1);                             \
    __builtin_aarch64_st2_lane##mode(                                         \
        (__builtin_aarch64_simd_##ptr_mode *)__ptr, __o, __c);                \
  }

__ST2_LANE_FUNC(float16x4x2_t, float16x8x2_t, float16_t, v4hf, v8hf, hf, f16,
                float16x8_t)
__ST2_LANE_FUNC(float32x2x2_t, float32x4x2_t, float32_t, v2sf, v4sf, sf, f32,
                float32x4_t)
__ST2_LANE_FUNC(float64x1x2_t, float64x2x2_t, float64_t, df, v2df, df, f64,
                float64x2_t)
__ST2_LANE_FUNC(poly8x8x2_t, poly8x16x2_t, poly8_t, v8qi, v16qi, qi, p8,
                int8x16_t)
__ST2_LANE_FUNC(poly16x4x2_t, poly16x8x2_t, poly16_t, v4hi, v8hi, hi, p16,
                int16x8_t)
__ST2_LANE_FUNC(poly64x1x2_t, poly64x2x2_t, poly64_t, di, v2di_ssps, di, p64,
                poly64x2_t)
__ST2_LANE_FUNC(int8x8x2_t, int8x16x2_t, int8_t, v8qi, v16qi, qi, s8, int8x16_t)
__ST2_LANE_FUNC(int16x4x2_t, int16x8x2_t, int16_t, v4hi, v8hi, hi, s16,
                int16x8_t)
__ST2_LANE_FUNC(int32x2x2_t, int32x4x2_t, int32_t, v2si, v4si, si, s32,
                int32x4_t)
__ST2_LANE_FUNC(int64x1x2_t, int64x2x2_t, int64_t, di, v2di, di, s64, int64x2_t)
__ST2_LANE_FUNC(uint8x8x2_t, uint8x16x2_t, uint8_t, v8qi, v16qi, qi, u8,
                int8x16_t)
__ST2_LANE_FUNC(uint16x4x2_t, uint16x8x2_t, uint16_t, v4hi, v8hi, hi, u16,
                int16x8_t)
__ST2_LANE_FUNC(uint32x2x2_t, uint32x4x2_t, uint32_t, v2si, v4si, si, u32,
                int32x4_t)
__ST2_LANE_FUNC(uint64x1x2_t, uint64x2x2_t, uint64_t, di, v2di, di, u64,
                int64x2_t)

#undef __ST2_LANE_FUNC
#define __ST2_LANE_FUNC(intype, ptrtype, mode, ptr_mode, funcsuffix)       \
  __extension__ extern __inline void                                       \
      __attribute__((__always_inline__, __gnu_inline__, __artificial__))   \
      vst2q_lane_##funcsuffix(ptrtype *__ptr, intype __b, const int __c) { \
    union {                                                                \
      intype __i;                                                          \
      __builtin_aarch64_simd_oi __o;                                       \
    } __temp = {__b};                                                      \
    __builtin_aarch64_st2_lane##mode(                                      \
        (__builtin_aarch64_simd_##ptr_mode *)__ptr, __temp.__o, __c);      \
  }

__ST2_LANE_FUNC(float16x8x2_t, float16_t, v8hf, hf, f16)
__ST2_LANE_FUNC(float32x4x2_t, float32_t, v4sf, sf, f32)
__ST2_LANE_FUNC(float64x2x2_t, float64_t, v2df, df, f64)
__ST2_LANE_FUNC(poly8x16x2_t, poly8_t, v16qi, qi, p8)
__ST2_LANE_FUNC(poly16x8x2_t, poly16_t, v8hi, hi, p16)
__ST2_LANE_FUNC(poly64x2x2_t, poly64_t, v2di, di, p64)
__ST2_LANE_FUNC(int8x16x2_t, int8_t, v16qi, qi, s8)
__ST2_LANE_FUNC(int16x8x2_t, int16_t, v8hi, hi, s16)
__ST2_LANE_FUNC(int32x4x2_t, int32_t, v4si, si, s32)
__ST2_LANE_FUNC(int64x2x2_t, int64_t, v2di, di, s64)
__ST2_LANE_FUNC(uint8x16x2_t, uint8_t, v16qi, qi, u8)
__ST2_LANE_FUNC(uint16x8x2_t, uint16_t, v8hi, hi, u16)
__ST2_LANE_FUNC(uint32x4x2_t, uint32_t, v4si, si, u32)
__ST2_LANE_FUNC(uint64x2x2_t, uint64_t, v2di, di, u64)

#define __ST3_LANE_FUNC(intype, largetype, ptrtype, mode, qmode, ptr_mode,    \
                        funcsuffix, signedtype)                               \
  __extension__ extern __inline void                                          \
      __attribute__((__always_inline__, __gnu_inline__, __artificial__))      \
      vst3_lane_##funcsuffix(ptrtype *__ptr, intype __b, const int __c) {     \
    __builtin_aarch64_simd_ci __o;                                            \
    largetype __temp;                                                         \
    __temp.val[0] = vcombine_##funcsuffix(                                    \
        __b.val[0], vcreate_##funcsuffix(__AARCH64_UINT64_C(0)));             \
    __temp.val[1] = vcombine_##funcsuffix(                                    \
        __b.val[1], vcreate_##funcsuffix(__AARCH64_UINT64_C(0)));             \
    __temp.val[2] = vcombine_##funcsuffix(                                    \
        __b.val[2], vcreate_##funcsuffix(__AARCH64_UINT64_C(0)));             \
    __o = __builtin_aarch64_set_qregci##qmode(__o, (signedtype)__temp.val[0], \
                                              0);                             \
    __o = __builtin_aarch64_set_qregci##qmode(__o, (signedtype)__temp.val[1], \
                                              1);                             \
    __o = __builtin_aarch64_set_qregci##qmode(__o, (signedtype)__temp.val[2], \
                                              2);                             \
    __builtin_aarch64_st3_lane##mode(                                         \
        (__builtin_aarch64_simd_##ptr_mode *)__ptr, __o, __c);                \
  }

__ST3_LANE_FUNC(float16x4x3_t, float16x8x3_t, float16_t, v4hf, v8hf, hf, f16,
                float16x8_t)
__ST3_LANE_FUNC(float32x2x3_t, float32x4x3_t, float32_t, v2sf, v4sf, sf, f32,
                float32x4_t)
__ST3_LANE_FUNC(float64x1x3_t, float64x2x3_t, float64_t, df, v2df, df, f64,
                float64x2_t)
__ST3_LANE_FUNC(poly8x8x3_t, poly8x16x3_t, poly8_t, v8qi, v16qi, qi, p8,
                int8x16_t)
__ST3_LANE_FUNC(poly16x4x3_t, poly16x8x3_t, poly16_t, v4hi, v8hi, hi, p16,
                int16x8_t)
__ST3_LANE_FUNC(poly64x1x3_t, poly64x2x3_t, poly64_t, di, v2di_ssps, di, p64,
                poly64x2_t)
__ST3_LANE_FUNC(int8x8x3_t, int8x16x3_t, int8_t, v8qi, v16qi, qi, s8, int8x16_t)
__ST3_LANE_FUNC(int16x4x3_t, int16x8x3_t, int16_t, v4hi, v8hi, hi, s16,
                int16x8_t)
__ST3_LANE_FUNC(int32x2x3_t, int32x4x3_t, int32_t, v2si, v4si, si, s32,
                int32x4_t)
__ST3_LANE_FUNC(int64x1x3_t, int64x2x3_t, int64_t, di, v2di, di, s64, int64x2_t)
__ST3_LANE_FUNC(uint8x8x3_t, uint8x16x3_t, uint8_t, v8qi, v16qi, qi, u8,
                int8x16_t)
__ST3_LANE_FUNC(uint16x4x3_t, uint16x8x3_t, uint16_t, v4hi, v8hi, hi, u16,
                int16x8_t)
__ST3_LANE_FUNC(uint32x2x3_t, uint32x4x3_t, uint32_t, v2si, v4si, si, u32,
                int32x4_t)
__ST3_LANE_FUNC(uint64x1x3_t, uint64x2x3_t, uint64_t, di, v2di, di, u64,
                int64x2_t)

#undef __ST3_LANE_FUNC
#define __ST3_LANE_FUNC(intype, ptrtype, mode, ptr_mode, funcsuffix)       \
  __extension__ extern __inline void                                       \
      __attribute__((__always_inline__, __gnu_inline__, __artificial__))   \
      vst3q_lane_##funcsuffix(ptrtype *__ptr, intype __b, const int __c) { \
    union {                                                                \
      intype __i;                                                          \
      __builtin_aarch64_simd_ci __o;                                       \
    } __temp = {__b};                                                      \
    __builtin_aarch64_st3_lane##mode(                                      \
        (__builtin_aarch64_simd_##ptr_mode *)__ptr, __temp.__o, __c);      \
  }

__ST3_LANE_FUNC(float16x8x3_t, float16_t, v8hf, hf, f16)
__ST3_LANE_FUNC(float32x4x3_t, float32_t, v4sf, sf, f32)
__ST3_LANE_FUNC(float64x2x3_t, float64_t, v2df, df, f64)
__ST3_LANE_FUNC(poly8x16x3_t, poly8_t, v16qi, qi, p8)
__ST3_LANE_FUNC(poly16x8x3_t, poly16_t, v8hi, hi, p16)
__ST3_LANE_FUNC(poly64x2x3_t, poly64_t, v2di, di, p64)
__ST3_LANE_FUNC(int8x16x3_t, int8_t, v16qi, qi, s8)
__ST3_LANE_FUNC(int16x8x3_t, int16_t, v8hi, hi, s16)
__ST3_LANE_FUNC(int32x4x3_t, int32_t, v4si, si, s32)
__ST3_LANE_FUNC(int64x2x3_t, int64_t, v2di, di, s64)
__ST3_LANE_FUNC(uint8x16x3_t, uint8_t, v16qi, qi, u8)
__ST3_LANE_FUNC(uint16x8x3_t, uint16_t, v8hi, hi, u16)
__ST3_LANE_FUNC(uint32x4x3_t, uint32_t, v4si, si, u32)
__ST3_LANE_FUNC(uint64x2x3_t, uint64_t, v2di, di, u64)

#define __ST4_LANE_FUNC(intype, largetype, ptrtype, mode, qmode, ptr_mode,    \
                        funcsuffix, signedtype)                               \
  __extension__ extern __inline void                                          \
      __attribute__((__always_inline__, __gnu_inline__, __artificial__))      \
      vst4_lane_##funcsuffix(ptrtype *__ptr, intype __b, const int __c) {     \
    __builtin_aarch64_simd_xi __o;                                            \
    largetype __temp;                                                         \
    __temp.val[0] = vcombine_##funcsuffix(                                    \
        __b.val[0], vcreate_##funcsuffix(__AARCH64_UINT64_C(0)));             \
    __temp.val[1] = vcombine_##funcsuffix(                                    \
        __b.val[1], vcreate_##funcsuffix(__AARCH64_UINT64_C(0)));             \
    __temp.val[2] = vcombine_##funcsuffix(                                    \
        __b.val[2], vcreate_##funcsuffix(__AARCH64_UINT64_C(0)));             \
    __temp.val[3] = vcombine_##funcsuffix(                                    \
        __b.val[3], vcreate_##funcsuffix(__AARCH64_UINT64_C(0)));             \
    __o = __builtin_aarch64_set_qregxi##qmode(__o, (signedtype)__temp.val[0], \
                                              0);                             \
    __o = __builtin_aarch64_set_qregxi##qmode(__o, (signedtype)__temp.val[1], \
                                              1);                             \
    __o = __builtin_aarch64_set_qregxi##qmode(__o, (signedtype)__temp.val[2], \
                                              2);                             \
    __o = __builtin_aarch64_set_qregxi##qmode(__o, (signedtype)__temp.val[3], \
                                              3);                             \
    __builtin_aarch64_st4_lane##mode(                                         \
        (__builtin_aarch64_simd_##ptr_mode *)__ptr, __o, __c);                \
  }

__ST4_LANE_FUNC(float16x4x4_t, float16x8x4_t, float16_t, v4hf, v8hf, hf, f16,
                float16x8_t)
__ST4_LANE_FUNC(float32x2x4_t, float32x4x4_t, float32_t, v2sf, v4sf, sf, f32,
                float32x4_t)
__ST4_LANE_FUNC(float64x1x4_t, float64x2x4_t, float64_t, df, v2df, df, f64,
                float64x2_t)
__ST4_LANE_FUNC(poly8x8x4_t, poly8x16x4_t, poly8_t, v8qi, v16qi, qi, p8,
                int8x16_t)
__ST4_LANE_FUNC(poly16x4x4_t, poly16x8x4_t, poly16_t, v4hi, v8hi, hi, p16,
                int16x8_t)
__ST4_LANE_FUNC(poly64x1x4_t, poly64x2x4_t, poly64_t, di, v2di_ssps, di, p64,
                poly64x2_t)
__ST4_LANE_FUNC(int8x8x4_t, int8x16x4_t, int8_t, v8qi, v16qi, qi, s8, int8x16_t)
__ST4_LANE_FUNC(int16x4x4_t, int16x8x4_t, int16_t, v4hi, v8hi, hi, s16,
                int16x8_t)
__ST4_LANE_FUNC(int32x2x4_t, int32x4x4_t, int32_t, v2si, v4si, si, s32,
                int32x4_t)
__ST4_LANE_FUNC(int64x1x4_t, int64x2x4_t, int64_t, di, v2di, di, s64, int64x2_t)
__ST4_LANE_FUNC(uint8x8x4_t, uint8x16x4_t, uint8_t, v8qi, v16qi, qi, u8,
                int8x16_t)
__ST4_LANE_FUNC(uint16x4x4_t, uint16x8x4_t, uint16_t, v4hi, v8hi, hi, u16,
                int16x8_t)
__ST4_LANE_FUNC(uint32x2x4_t, uint32x4x4_t, uint32_t, v2si, v4si, si, u32,
                int32x4_t)
__ST4_LANE_FUNC(uint64x1x4_t, uint64x2x4_t, uint64_t, di, v2di, di, u64,
                int64x2_t)

#undef __ST4_LANE_FUNC
#define __ST4_LANE_FUNC(intype, ptrtype, mode, ptr_mode, funcsuffix)       \
  __extension__ extern __inline void                                       \
      __attribute__((__always_inline__, __gnu_inline__, __artificial__))   \
      vst4q_lane_##funcsuffix(ptrtype *__ptr, intype __b, const int __c) { \
    union {                                                                \
      intype __i;                                                          \
      __builtin_aarch64_simd_xi __o;                                       \
    } __temp = {__b};                                                      \
    __builtin_aarch64_st4_lane##mode(                                      \
        (__builtin_aarch64_simd_##ptr_mode *)__ptr, __temp.__o, __c);      \
  }

__ST4_LANE_FUNC(float16x8x4_t, float16_t, v8hf, hf, f16)
__ST4_LANE_FUNC(float32x4x4_t, float32_t, v4sf, sf, f32)
__ST4_LANE_FUNC(float64x2x4_t, float64_t, v2df, df, f64)
__ST4_LANE_FUNC(poly8x16x4_t, poly8_t, v16qi, qi, p8)
__ST4_LANE_FUNC(poly16x8x4_t, poly16_t, v8hi, hi, p16)
__ST4_LANE_FUNC(poly64x2x4_t, poly64_t, v2di, di, p64)
__ST4_LANE_FUNC(int8x16x4_t, int8_t, v16qi, qi, s8)
__ST4_LANE_FUNC(int16x8x4_t, int16_t, v8hi, hi, s16)
__ST4_LANE_FUNC(int32x4x4_t, int32_t, v4si, si, s32)
__ST4_LANE_FUNC(int64x2x4_t, int64_t, v2di, di, s64)
__ST4_LANE_FUNC(uint8x16x4_t, uint8_t, v16qi, qi, u8)
__ST4_LANE_FUNC(uint16x8x4_t, uint16_t, v8hi, hi, u16)
__ST4_LANE_FUNC(uint32x4x4_t, uint32_t, v4si, si, u32)
__ST4_LANE_FUNC(uint64x2x4_t, uint64_t, v2di, di, u64)

FUNK(int64_t)
vaddlv_s32(int32x2_t a) {
  int64_t result;
  __asm__("saddlp %0.1d, %1.2s" : "=w"(result) : "w"(a) :);
  return result;
}

FUNK(uint64_t)
vaddlv_u32(uint32x2_t a) {
  uint64_t result;
  __asm__("uaddlp %0.1d, %1.2s" : "=w"(result) : "w"(a) :);
  return result;
}

FUNK(int16x4_t)
vqdmulh_laneq_s16(int16x4_t __a, int16x8_t __b, const int __c) {
  return __builtin_aarch64_sqdmulh_laneqv4hi(__a, __b, __c);
}

FUNK(int32x2_t)
vqdmulh_laneq_s32(int32x2_t __a, int32x4_t __b, const int __c) {
  return __builtin_aarch64_sqdmulh_laneqv2si(__a, __b, __c);
}

FUNK(int16x8_t)
vqdmulhq_laneq_s16(int16x8_t __a, int16x8_t __b, const int __c) {
  return __builtin_aarch64_sqdmulh_laneqv8hi(__a, __b, __c);
}

FUNK(int32x4_t)
vqdmulhq_laneq_s32(int32x4_t __a, int32x4_t __b, const int __c) {
  return __builtin_aarch64_sqdmulh_laneqv4si(__a, __b, __c);
}

FUNK(int16x4_t)
vqrdmulh_laneq_s16(int16x4_t __a, int16x8_t __b, const int __c) {
  return __builtin_aarch64_sqrdmulh_laneqv4hi(__a, __b, __c);
}

FUNK(int32x2_t)
vqrdmulh_laneq_s32(int32x2_t __a, int32x4_t __b, const int __c) {
  return __builtin_aarch64_sqrdmulh_laneqv2si(__a, __b, __c);
}

FUNK(int16x8_t)
vqrdmulhq_laneq_s16(int16x8_t __a, int16x8_t __b, const int __c) {
  return __builtin_aarch64_sqrdmulh_laneqv8hi(__a, __b, __c);
}

FUNK(int32x4_t)
vqrdmulhq_laneq_s32(int32x4_t __a, int32x4_t __b, const int __c) {
  return __builtin_aarch64_sqrdmulh_laneqv4si(__a, __b, __c);
}

FUNK(poly8x8_t)
vqtbl1_p8(poly8x16_t a, uint8x8_t b) {
  poly8x8_t result;
  __asm__("tbl %0.8b, {%1.16b}, %2.8b"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int8x8_t)
vqtbl1_s8(int8x16_t a, uint8x8_t b) {
  int8x8_t result;
  __asm__("tbl %0.8b, {%1.16b}, %2.8b"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(uint8x8_t)
vqtbl1_u8(uint8x16_t a, uint8x8_t b) {
  uint8x8_t result;
  __asm__("tbl %0.8b, {%1.16b}, %2.8b"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(poly8x16_t)
vqtbl1q_p8(poly8x16_t a, uint8x16_t b) {
  poly8x16_t result;
  __asm__("tbl %0.16b, {%1.16b}, %2.16b"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int8x16_t)
vqtbl1q_s8(int8x16_t a, uint8x16_t b) {
  int8x16_t result;
  __asm__("tbl %0.16b, {%1.16b}, %2.16b"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(uint8x16_t)
vqtbl1q_u8(uint8x16_t a, uint8x16_t b) {
  uint8x16_t result;
  __asm__("tbl %0.16b, {%1.16b}, %2.16b"
          : "=w"(result)
          : "w"(a), "w"(b)
          : /* No clobbers */);
  return result;
}

FUNK(int8x8_t)
vqtbx1_s8(int8x8_t r, int8x16_t tab, uint8x8_t idx) {
  int8x8_t result = r;
  __asm__("tbx %0.8b,{%1.16b},%2.8b"
          : "+w"(result)
          : "w"(tab), "w"(idx)
          : /* No clobbers */);
  return result;
}

FUNK(uint8x8_t)
vqtbx1_u8(uint8x8_t r, uint8x16_t tab, uint8x8_t idx) {
  uint8x8_t result = r;
  __asm__("tbx %0.8b,{%1.16b},%2.8b"
          : "+w"(result)
          : "w"(tab), "w"(idx)
          : /* No clobbers */);
  return result;
}

FUNK(poly8x8_t)
vqtbx1_p8(poly8x8_t r, poly8x16_t tab, uint8x8_t idx) {
  poly8x8_t result = r;
  __asm__("tbx %0.8b,{%1.16b},%2.8b"
          : "+w"(result)
          : "w"(tab), "w"(idx)
          : /* No clobbers */);
  return result;
}

FUNK(int8x16_t)
vqtbx1q_s8(int8x16_t r, int8x16_t tab, uint8x16_t idx) {
  int8x16_t result = r;
  __asm__("tbx %0.16b,{%1.16b},%2.16b"
          : "+w"(result)
          : "w"(tab), "w"(idx)
          : /* No clobbers */);
  return result;
}

FUNK(uint8x16_t)
vqtbx1q_u8(uint8x16_t r, uint8x16_t tab, uint8x16_t idx) {
  uint8x16_t result = r;
  __asm__("tbx %0.16b,{%1.16b},%2.16b"
          : "+w"(result)
          : "w"(tab), "w"(idx)
          : /* No clobbers */);
  return result;
}

FUNK(poly8x16_t)
vqtbx1q_p8(poly8x16_t r, poly8x16_t tab, uint8x16_t idx) {
  poly8x16_t result = r;
  __asm__("tbx %0.16b,{%1.16b},%2.16b"
          : "+w"(result)
          : "w"(tab), "w"(idx)
          : /* No clobbers */);
  return result;
}

FUNK(int8x8_t)
vtbl1_s8(int8x8_t tab, int8x8_t idx) {
  int8x8_t result;
  int8x16_t temp = vcombine_s8(tab, vcreate_s8(__AARCH64_UINT64_C(0x0)));
  __asm__("tbl %0.8b, {%1.16b}, %2.8b"
          : "=w"(result)
          : "w"(temp), "w"(idx)
          : /* No clobbers */);
  return result;
}

FUNK(uint8x8_t)
vtbl1_u8(uint8x8_t tab, uint8x8_t idx) {
  uint8x8_t result;
  uint8x16_t temp = vcombine_u8(tab, vcreate_u8(__AARCH64_UINT64_C(0x0)));
  __asm__("tbl %0.8b, {%1.16b}, %2.8b"
          : "=w"(result)
          : "w"(temp), "w"(idx)
          : /* No clobbers */);
  return result;
}

FUNK(poly8x8_t)
vtbl1_p8(poly8x8_t tab, uint8x8_t idx) {
  poly8x8_t result;
  poly8x16_t temp = vcombine_p8(tab, vcreate_p8(__AARCH64_UINT64_C(0x0)));
  __asm__("tbl %0.8b, {%1.16b}, %2.8b"
          : "=w"(result)
          : "w"(temp), "w"(idx)
          : /* No clobbers */);
  return result;
}

FUNK(int8x8_t)
vtbl2_s8(int8x8x2_t tab, int8x8_t idx) {
  int8x8_t result;
  int8x16_t temp = vcombine_s8(tab.val[0], tab.val[1]);
  __asm__("tbl %0.8b, {%1.16b}, %2.8b"
          : "=w"(result)
          : "w"(temp), "w"(idx)
          : /* No clobbers */);
  return result;
}

FUNK(uint8x8_t)
vtbl2_u8(uint8x8x2_t tab, uint8x8_t idx) {
  uint8x8_t result;
  uint8x16_t temp = vcombine_u8(tab.val[0], tab.val[1]);
  __asm__("tbl %0.8b, {%1.16b}, %2.8b"
          : "=w"(result)
          : "w"(temp), "w"(idx)
          : /* No clobbers */);
  return result;
}

FUNK(poly8x8_t)
vtbl2_p8(poly8x8x2_t tab, uint8x8_t idx) {
  poly8x8_t result;
  poly8x16_t temp = vcombine_p8(tab.val[0], tab.val[1]);
  __asm__("tbl %0.8b, {%1.16b}, %2.8b"
          : "=w"(result)
          : "w"(temp), "w"(idx)
          : /* No clobbers */);
  return result;
}

FUNK(int8x8_t)
vtbl3_s8(int8x8x3_t tab, int8x8_t idx) {
  int8x8_t result;
  int8x16x2_t temp;
  __builtin_aarch64_simd_oi __o;
  temp.val[0] = vcombine_s8(tab.val[0], tab.val[1]);
  temp.val[1] = vcombine_s8(tab.val[2], vcreate_s8(__AARCH64_UINT64_C(0x0)));
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)temp.val[1], 1);
  result = __builtin_aarch64_tbl3v8qi(__o, idx);
  return result;
}

FUNK(uint8x8_t)
vtbl3_u8(uint8x8x3_t tab, uint8x8_t idx) {
  uint8x8_t result;
  uint8x16x2_t temp;
  __builtin_aarch64_simd_oi __o;
  temp.val[0] = vcombine_u8(tab.val[0], tab.val[1]);
  temp.val[1] = vcombine_u8(tab.val[2], vcreate_u8(__AARCH64_UINT64_C(0x0)));
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)temp.val[1], 1);
  result = (uint8x8_t)__builtin_aarch64_tbl3v8qi(__o, (int8x8_t)idx);
  return result;
}

FUNK(poly8x8_t)
vtbl3_p8(poly8x8x3_t tab, uint8x8_t idx) {
  poly8x8_t result;
  poly8x16x2_t temp;
  __builtin_aarch64_simd_oi __o;
  temp.val[0] = vcombine_p8(tab.val[0], tab.val[1]);
  temp.val[1] = vcombine_p8(tab.val[2], vcreate_p8(__AARCH64_UINT64_C(0x0)));
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)temp.val[1], 1);
  result = (poly8x8_t)__builtin_aarch64_tbl3v8qi(__o, (int8x8_t)idx);
  return result;
}

FUNK(int8x8_t)
vtbl4_s8(int8x8x4_t tab, int8x8_t idx) {
  int8x8_t result;
  int8x16x2_t temp;
  __builtin_aarch64_simd_oi __o;
  temp.val[0] = vcombine_s8(tab.val[0], tab.val[1]);
  temp.val[1] = vcombine_s8(tab.val[2], tab.val[3]);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)temp.val[1], 1);
  result = __builtin_aarch64_tbl3v8qi(__o, idx);
  return result;
}

FUNK(uint8x8_t)
vtbl4_u8(uint8x8x4_t tab, uint8x8_t idx) {
  uint8x8_t result;
  uint8x16x2_t temp;
  __builtin_aarch64_simd_oi __o;
  temp.val[0] = vcombine_u8(tab.val[0], tab.val[1]);
  temp.val[1] = vcombine_u8(tab.val[2], tab.val[3]);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)temp.val[1], 1);
  result = (uint8x8_t)__builtin_aarch64_tbl3v8qi(__o, (int8x8_t)idx);
  return result;
}

FUNK(poly8x8_t)
vtbl4_p8(poly8x8x4_t tab, uint8x8_t idx) {
  poly8x8_t result;
  poly8x16x2_t temp;
  __builtin_aarch64_simd_oi __o;
  temp.val[0] = vcombine_p8(tab.val[0], tab.val[1]);
  temp.val[1] = vcombine_p8(tab.val[2], tab.val[3]);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)temp.val[1], 1);
  result = (poly8x8_t)__builtin_aarch64_tbl3v8qi(__o, (int8x8_t)idx);
  return result;
}

FUNK(int8x8_t)
vtbx2_s8(int8x8_t r, int8x8x2_t tab, int8x8_t idx) {
  int8x8_t result = r;
  int8x16_t temp = vcombine_s8(tab.val[0], tab.val[1]);
  __asm__("tbx %0.8b, {%1.16b}, %2.8b"
          : "+w"(result)
          : "w"(temp), "w"(idx)
          : /* No clobbers */);
  return result;
}

FUNK(uint8x8_t)
vtbx2_u8(uint8x8_t r, uint8x8x2_t tab, uint8x8_t idx) {
  uint8x8_t result = r;
  uint8x16_t temp = vcombine_u8(tab.val[0], tab.val[1]);
  __asm__("tbx %0.8b, {%1.16b}, %2.8b"
          : "+w"(result)
          : "w"(temp), "w"(idx)
          : /* No clobbers */);
  return result;
}

FUNK(poly8x8_t)
vtbx2_p8(poly8x8_t r, poly8x8x2_t tab, uint8x8_t idx) {
  poly8x8_t result = r;
  poly8x16_t temp = vcombine_p8(tab.val[0], tab.val[1]);
  __asm__("tbx %0.8b, {%1.16b}, %2.8b"
          : "+w"(result)
          : "w"(temp), "w"(idx)
          : /* No clobbers */);
  return result;
}

FUNK(float32_t)
vabds_f32(float32_t __a, float32_t __b) {
  return __builtin_aarch64_fabdsf(__a, __b);
}

FUNK(float64_t)
vabdd_f64(float64_t __a, float64_t __b) {
  return __builtin_aarch64_fabddf(__a, __b);
}

FUNK(float32x2_t)
vabd_f32(float32x2_t __a, float32x2_t __b) {
  return __builtin_aarch64_fabdv2sf(__a, __b);
}

FUNK(float64x1_t)
vabd_f64(float64x1_t __a, float64x1_t __b) {
  return (float64x1_t){vabdd_f64(vget_lane_f64(__a, 0), vget_lane_f64(__b, 0))};
}

FUNK(float32x4_t)
vabdq_f32(float32x4_t __a, float32x4_t __b) {
  return __builtin_aarch64_fabdv4sf(__a, __b);
}

FUNK(float64x2_t)
vabdq_f64(float64x2_t __a, float64x2_t __b) {
  return __builtin_aarch64_fabdv2df(__a, __b);
}

FUNK(float32x2_t)
vabs_f32(float32x2_t __a) {
  return __builtin_aarch64_absv2sf(__a);
}

FUNK(float64x1_t)
vabs_f64(float64x1_t __a) {
  return (float64x1_t){__builtin_fabs(__a[0])};
}

FUNK(int8x8_t)
vabs_s8(int8x8_t __a) {
  return __builtin_aarch64_absv8qi(__a);
}

FUNK(int16x4_t)
vabs_s16(int16x4_t __a) {
  return __builtin_aarch64_absv4hi(__a);
}

FUNK(int32x2_t)
vabs_s32(int32x2_t __a) {
  return __builtin_aarch64_absv2si(__a);
}

FUNK(int64x1_t)
vabs_s64(int64x1_t __a) {
  return (int64x1_t){__builtin_aarch64_absdi(__a[0])};
}

FUNK(float32x4_t)
vabsq_f32(float32x4_t __a) {
  return __builtin_aarch64_absv4sf(__a);
}

FUNK(float64x2_t)
vabsq_f64(float64x2_t __a) {
  return __builtin_aarch64_absv2df(__a);
}

FUNK(int8x16_t)
vabsq_s8(int8x16_t __a) {
  return __builtin_aarch64_absv16qi(__a);
}

FUNK(int16x8_t)
vabsq_s16(int16x8_t __a) {
  return __builtin_aarch64_absv8hi(__a);
}

FUNK(int32x4_t)
vabsq_s32(int32x4_t __a) {
  return __builtin_aarch64_absv4si(__a);
}

FUNK(int64x2_t)
vabsq_s64(int64x2_t __a) {
  return __builtin_aarch64_absv2di(__a);
}

FUNK(int64_t)
vabsd_s64(int64_t __a) {
  return __a < 0 ? -(uint64_t)__a : __a;
}

FUNK(int64_t)
vaddd_s64(int64_t __a, int64_t __b) {
  return __a + __b;
}

FUNK(uint64_t)
vaddd_u64(uint64_t __a, uint64_t __b) {
  return __a + __b;
}

FUNK(int8_t)
vaddv_s8(int8x8_t __a) {
  return __builtin_aarch64_reduc_plus_scal_v8qi(__a);
}

FUNK(int16_t)
vaddv_s16(int16x4_t __a) {
  return __builtin_aarch64_reduc_plus_scal_v4hi(__a);
}

FUNK(int32_t)
vaddv_s32(int32x2_t __a) {
  return __builtin_aarch64_reduc_plus_scal_v2si(__a);
}

FUNK(uint8_t)
vaddv_u8(uint8x8_t __a) {
  return (uint8_t)__builtin_aarch64_reduc_plus_scal_v8qi((int8x8_t)__a);
}

FUNK(uint16_t)
vaddv_u16(uint16x4_t __a) {
  return (uint16_t)__builtin_aarch64_reduc_plus_scal_v4hi((int16x4_t)__a);
}

FUNK(uint32_t)
vaddv_u32(uint32x2_t __a) {
  return (int32_t)__builtin_aarch64_reduc_plus_scal_v2si((int32x2_t)__a);
}

FUNK(int8_t)
vaddvq_s8(int8x16_t __a) {
  return __builtin_aarch64_reduc_plus_scal_v16qi(__a);
}

FUNK(int16_t)
vaddvq_s16(int16x8_t __a) {
  return __builtin_aarch64_reduc_plus_scal_v8hi(__a);
}

FUNK(int32_t)
vaddvq_s32(int32x4_t __a) {
  return __builtin_aarch64_reduc_plus_scal_v4si(__a);
}

FUNK(int64_t)
vaddvq_s64(int64x2_t __a) {
  return __builtin_aarch64_reduc_plus_scal_v2di(__a);
}

FUNK(uint8_t)
vaddvq_u8(uint8x16_t __a) {
  return (uint8_t)__builtin_aarch64_reduc_plus_scal_v16qi((int8x16_t)__a);
}

FUNK(uint16_t)
vaddvq_u16(uint16x8_t __a) {
  return (uint16_t)__builtin_aarch64_reduc_plus_scal_v8hi((int16x8_t)__a);
}

FUNK(uint32_t)
vaddvq_u32(uint32x4_t __a) {
  return (uint32_t)__builtin_aarch64_reduc_plus_scal_v4si((int32x4_t)__a);
}

FUNK(uint64_t)
vaddvq_u64(uint64x2_t __a) {
  return (uint64_t)__builtin_aarch64_reduc_plus_scal_v2di((int64x2_t)__a);
}

FUNK(float32_t)
vaddv_f32(float32x2_t __a) {
  return __builtin_aarch64_reduc_plus_scal_v2sf(__a);
}

FUNK(float32_t)
vaddvq_f32(float32x4_t __a) {
  return __builtin_aarch64_reduc_plus_scal_v4sf(__a);
}

FUNK(float64_t)
vaddvq_f64(float64x2_t __a) {
  return __builtin_aarch64_reduc_plus_scal_v2df(__a);
}

FUNK(float16x4_t)
vbsl_f16(uint16x4_t __a, float16x4_t __b, float16x4_t __c) {
  return __builtin_aarch64_simd_bslv4hf_suss(__a, __b, __c);
}

FUNK(float32x2_t)
vbsl_f32(uint32x2_t __a, float32x2_t __b, float32x2_t __c) {
  return __builtin_aarch64_simd_bslv2sf_suss(__a, __b, __c);
}

FUNK(float64x1_t)
vbsl_f64(uint64x1_t __a, float64x1_t __b, float64x1_t __c) {
  return (float64x1_t){
      __builtin_aarch64_simd_bsldf_suss(__a[0], __b[0], __c[0])};
}

FUNK(poly8x8_t)
vbsl_p8(uint8x8_t __a, poly8x8_t __b, poly8x8_t __c) {
  return __builtin_aarch64_simd_bslv8qi_pupp(__a, __b, __c);
}

FUNK(poly16x4_t)
vbsl_p16(uint16x4_t __a, poly16x4_t __b, poly16x4_t __c) {
  return __builtin_aarch64_simd_bslv4hi_pupp(__a, __b, __c);
}
FUNK(poly64x1_t)
vbsl_p64(uint64x1_t __a, poly64x1_t __b, poly64x1_t __c) {
  return (poly64x1_t){
      __builtin_aarch64_simd_bsldi_pupp(__a[0], __b[0], __c[0])};
}

FUNK(int8x8_t)
vbsl_s8(uint8x8_t __a, int8x8_t __b, int8x8_t __c) {
  return __builtin_aarch64_simd_bslv8qi_suss(__a, __b, __c);
}

FUNK(int16x4_t)
vbsl_s16(uint16x4_t __a, int16x4_t __b, int16x4_t __c) {
  return __builtin_aarch64_simd_bslv4hi_suss(__a, __b, __c);
}

FUNK(int32x2_t)
vbsl_s32(uint32x2_t __a, int32x2_t __b, int32x2_t __c) {
  return __builtin_aarch64_simd_bslv2si_suss(__a, __b, __c);
}

FUNK(int64x1_t)
vbsl_s64(uint64x1_t __a, int64x1_t __b, int64x1_t __c) {
  return (int64x1_t){__builtin_aarch64_simd_bsldi_suss(__a[0], __b[0], __c[0])};
}

FUNK(uint8x8_t)
vbsl_u8(uint8x8_t __a, uint8x8_t __b, uint8x8_t __c) {
  return __builtin_aarch64_simd_bslv8qi_uuuu(__a, __b, __c);
}

FUNK(uint16x4_t)
vbsl_u16(uint16x4_t __a, uint16x4_t __b, uint16x4_t __c) {
  return __builtin_aarch64_simd_bslv4hi_uuuu(__a, __b, __c);
}

FUNK(uint32x2_t)
vbsl_u32(uint32x2_t __a, uint32x2_t __b, uint32x2_t __c) {
  return __builtin_aarch64_simd_bslv2si_uuuu(__a, __b, __c);
}

FUNK(uint64x1_t)
vbsl_u64(uint64x1_t __a, uint64x1_t __b, uint64x1_t __c) {
  return (uint64x1_t){
      __builtin_aarch64_simd_bsldi_uuuu(__a[0], __b[0], __c[0])};
}

FUNK(float16x8_t)
vbslq_f16(uint16x8_t __a, float16x8_t __b, float16x8_t __c) {
  return __builtin_aarch64_simd_bslv8hf_suss(__a, __b, __c);
}

FUNK(float32x4_t)
vbslq_f32(uint32x4_t __a, float32x4_t __b, float32x4_t __c) {
  return __builtin_aarch64_simd_bslv4sf_suss(__a, __b, __c);
}

FUNK(float64x2_t)
vbslq_f64(uint64x2_t __a, float64x2_t __b, float64x2_t __c) {
  return __builtin_aarch64_simd_bslv2df_suss(__a, __b, __c);
}

FUNK(poly8x16_t)
vbslq_p8(uint8x16_t __a, poly8x16_t __b, poly8x16_t __c) {
  return __builtin_aarch64_simd_bslv16qi_pupp(__a, __b, __c);
}

FUNK(poly16x8_t)
vbslq_p16(uint16x8_t __a, poly16x8_t __b, poly16x8_t __c) {
  return __builtin_aarch64_simd_bslv8hi_pupp(__a, __b, __c);
}

FUNK(int8x16_t)
vbslq_s8(uint8x16_t __a, int8x16_t __b, int8x16_t __c) {
  return __builtin_aarch64_simd_bslv16qi_suss(__a, __b, __c);
}

FUNK(int16x8_t)
vbslq_s16(uint16x8_t __a, int16x8_t __b, int16x8_t __c) {
  return __builtin_aarch64_simd_bslv8hi_suss(__a, __b, __c);
}

FUNK(poly64x2_t)
vbslq_p64(uint64x2_t __a, poly64x2_t __b, poly64x2_t __c) {
  return __builtin_aarch64_simd_bslv2di_pupp(__a, __b, __c);
}

FUNK(int32x4_t)
vbslq_s32(uint32x4_t __a, int32x4_t __b, int32x4_t __c) {
  return __builtin_aarch64_simd_bslv4si_suss(__a, __b, __c);
}

FUNK(int64x2_t)
vbslq_s64(uint64x2_t __a, int64x2_t __b, int64x2_t __c) {
  return __builtin_aarch64_simd_bslv2di_suss(__a, __b, __c);
}

FUNK(uint8x16_t)
vbslq_u8(uint8x16_t __a, uint8x16_t __b, uint8x16_t __c) {
  return __builtin_aarch64_simd_bslv16qi_uuuu(__a, __b, __c);
}

FUNK(uint16x8_t)
vbslq_u16(uint16x8_t __a, uint16x8_t __b, uint16x8_t __c) {
  return __builtin_aarch64_simd_bslv8hi_uuuu(__a, __b, __c);
}

FUNK(uint32x4_t)
vbslq_u32(uint32x4_t __a, uint32x4_t __b, uint32x4_t __c) {
  return __builtin_aarch64_simd_bslv4si_uuuu(__a, __b, __c);
}

FUNK(uint64x2_t)
vbslq_u64(uint64x2_t __a, uint64x2_t __b, uint64x2_t __c) {
  return __builtin_aarch64_simd_bslv2di_uuuu(__a, __b, __c);
}

#pragma GCC push_options
#pragma GCC target("+nothing+rdma")

FUNK(int16x4_t)
vqrdmlah_s16(int16x4_t __a, int16x4_t __b, int16x4_t __c) {
  return __builtin_aarch64_sqrdmlahv4hi(__a, __b, __c);
}

FUNK(int32x2_t)
vqrdmlah_s32(int32x2_t __a, int32x2_t __b, int32x2_t __c) {
  return __builtin_aarch64_sqrdmlahv2si(__a, __b, __c);
}

FUNK(int16x8_t)
vqrdmlahq_s16(int16x8_t __a, int16x8_t __b, int16x8_t __c) {
  return __builtin_aarch64_sqrdmlahv8hi(__a, __b, __c);
}

FUNK(int32x4_t)
vqrdmlahq_s32(int32x4_t __a, int32x4_t __b, int32x4_t __c) {
  return __builtin_aarch64_sqrdmlahv4si(__a, __b, __c);
}

FUNK(int16x4_t)
vqrdmlsh_s16(int16x4_t __a, int16x4_t __b, int16x4_t __c) {
  return __builtin_aarch64_sqrdmlshv4hi(__a, __b, __c);
}

FUNK(int32x2_t)
vqrdmlsh_s32(int32x2_t __a, int32x2_t __b, int32x2_t __c) {
  return __builtin_aarch64_sqrdmlshv2si(__a, __b, __c);
}

FUNK(int16x8_t)
vqrdmlshq_s16(int16x8_t __a, int16x8_t __b, int16x8_t __c) {
  return __builtin_aarch64_sqrdmlshv8hi(__a, __b, __c);
}

FUNK(int32x4_t)
vqrdmlshq_s32(int32x4_t __a, int32x4_t __b, int32x4_t __c) {
  return __builtin_aarch64_sqrdmlshv4si(__a, __b, __c);
}

FUNK(int16x4_t)
vqrdmlah_laneq_s16(int16x4_t __a, int16x4_t __b, int16x8_t __c, const int __d) {
  return __builtin_aarch64_sqrdmlah_laneqv4hi(__a, __b, __c, __d);
}

FUNK(int32x2_t)
vqrdmlah_laneq_s32(int32x2_t __a, int32x2_t __b, int32x4_t __c, const int __d) {
  return __builtin_aarch64_sqrdmlah_laneqv2si(__a, __b, __c, __d);
}

FUNK(int16x8_t)
vqrdmlahq_laneq_s16(int16x8_t __a, int16x8_t __b, int16x8_t __c,
                    const int __d) {
  return __builtin_aarch64_sqrdmlah_laneqv8hi(__a, __b, __c, __d);
}

FUNK(int32x4_t)
vqrdmlahq_laneq_s32(int32x4_t __a, int32x4_t __b, int32x4_t __c,
                    const int __d) {
  return __builtin_aarch64_sqrdmlah_laneqv4si(__a, __b, __c, __d);
}

FUNK(int16x4_t)
vqrdmlsh_laneq_s16(int16x4_t __a, int16x4_t __b, int16x8_t __c, const int __d) {
  return __builtin_aarch64_sqrdmlsh_laneqv4hi(__a, __b, __c, __d);
}

FUNK(int32x2_t)
vqrdmlsh_laneq_s32(int32x2_t __a, int32x2_t __b, int32x4_t __c, const int __d) {
  return __builtin_aarch64_sqrdmlsh_laneqv2si(__a, __b, __c, __d);
}

FUNK(int16x8_t)
vqrdmlshq_laneq_s16(int16x8_t __a, int16x8_t __b, int16x8_t __c,
                    const int __d) {
  return __builtin_aarch64_sqrdmlsh_laneqv8hi(__a, __b, __c, __d);
}

FUNK(int32x4_t)
vqrdmlshq_laneq_s32(int32x4_t __a, int32x4_t __b, int32x4_t __c,
                    const int __d) {
  return __builtin_aarch64_sqrdmlsh_laneqv4si(__a, __b, __c, __d);
}

FUNK(int16x4_t)
vqrdmlah_lane_s16(int16x4_t __a, int16x4_t __b, int16x4_t __c, const int __d) {
  return __builtin_aarch64_sqrdmlah_lanev4hi(__a, __b, __c, __d);
}

FUNK(int32x2_t)
vqrdmlah_lane_s32(int32x2_t __a, int32x2_t __b, int32x2_t __c, const int __d) {
  return __builtin_aarch64_sqrdmlah_lanev2si(__a, __b, __c, __d);
}

FUNK(int16x8_t)
vqrdmlahq_lane_s16(int16x8_t __a, int16x8_t __b, int16x4_t __c, const int __d) {
  return __builtin_aarch64_sqrdmlah_lanev8hi(__a, __b, __c, __d);
}

FUNK(int32x4_t)
vqrdmlahq_lane_s32(int32x4_t __a, int32x4_t __b, int32x2_t __c, const int __d) {
  return __builtin_aarch64_sqrdmlah_lanev4si(__a, __b, __c, __d);
}

FUNK(int16_t)
vqrdmlahh_s16(int16_t __a, int16_t __b, int16_t __c) {
  return (int16_t)__builtin_aarch64_sqrdmlahhi(__a, __b, __c);
}

FUNK(int16_t)
vqrdmlahh_lane_s16(int16_t __a, int16_t __b, int16x4_t __c, const int __d) {
  return __builtin_aarch64_sqrdmlah_lanehi(__a, __b, __c, __d);
}

FUNK(int16_t)
vqrdmlahh_laneq_s16(int16_t __a, int16_t __b, int16x8_t __c, const int __d) {
  return __builtin_aarch64_sqrdmlah_laneqhi(__a, __b, __c, __d);
}

FUNK(int32_t)
vqrdmlahs_s32(int32_t __a, int32_t __b, int32_t __c) {
  return (int32_t)__builtin_aarch64_sqrdmlahsi(__a, __b, __c);
}

FUNK(int32_t)
vqrdmlahs_lane_s32(int32_t __a, int32_t __b, int32x2_t __c, const int __d) {
  return __builtin_aarch64_sqrdmlah_lanesi(__a, __b, __c, __d);
}

FUNK(int32_t)
vqrdmlahs_laneq_s32(int32_t __a, int32_t __b, int32x4_t __c, const int __d) {
  return __builtin_aarch64_sqrdmlah_laneqsi(__a, __b, __c, __d);
}

FUNK(int16x4_t)
vqrdmlsh_lane_s16(int16x4_t __a, int16x4_t __b, int16x4_t __c, const int __d) {
  return __builtin_aarch64_sqrdmlsh_lanev4hi(__a, __b, __c, __d);
}

FUNK(int32x2_t)
vqrdmlsh_lane_s32(int32x2_t __a, int32x2_t __b, int32x2_t __c, const int __d) {
  return __builtin_aarch64_sqrdmlsh_lanev2si(__a, __b, __c, __d);
}

FUNK(int16x8_t)
vqrdmlshq_lane_s16(int16x8_t __a, int16x8_t __b, int16x4_t __c, const int __d) {
  return __builtin_aarch64_sqrdmlsh_lanev8hi(__a, __b, __c, __d);
}

FUNK(int32x4_t)
vqrdmlshq_lane_s32(int32x4_t __a, int32x4_t __b, int32x2_t __c, const int __d) {
  return __builtin_aarch64_sqrdmlsh_lanev4si(__a, __b, __c, __d);
}

FUNK(int16_t)
vqrdmlshh_s16(int16_t __a, int16_t __b, int16_t __c) {
  return (int16_t)__builtin_aarch64_sqrdmlshhi(__a, __b, __c);
}

FUNK(int16_t)
vqrdmlshh_lane_s16(int16_t __a, int16_t __b, int16x4_t __c, const int __d) {
  return __builtin_aarch64_sqrdmlsh_lanehi(__a, __b, __c, __d);
}

FUNK(int16_t)
vqrdmlshh_laneq_s16(int16_t __a, int16_t __b, int16x8_t __c, const int __d) {
  return __builtin_aarch64_sqrdmlsh_laneqhi(__a, __b, __c, __d);
}

FUNK(int32_t)
vqrdmlshs_s32(int32_t __a, int32_t __b, int32_t __c) {
  return (int32_t)__builtin_aarch64_sqrdmlshsi(__a, __b, __c);
}

FUNK(int32_t)
vqrdmlshs_lane_s32(int32_t __a, int32_t __b, int32x2_t __c, const int __d) {
  return __builtin_aarch64_sqrdmlsh_lanesi(__a, __b, __c, __d);
}

FUNK(int32_t)
vqrdmlshs_laneq_s32(int32_t __a, int32_t __b, int32x4_t __c, const int __d) {
  return __builtin_aarch64_sqrdmlsh_laneqsi(__a, __b, __c, __d);
}

#pragma GCC pop_options

#pragma GCC push_options
#pragma GCC target("+nothing+crypto")

FUNK(uint8x16_t)
vaeseq_u8(uint8x16_t data, uint8x16_t key) {
  return __builtin_aarch64_crypto_aesev16qi_uuu(data, key);
}

FUNK(uint8x16_t)
vaesdq_u8(uint8x16_t data, uint8x16_t key) {
  return __builtin_aarch64_crypto_aesdv16qi_uuu(data, key);
}

FUNK(uint8x16_t)
vaesmcq_u8(uint8x16_t data) {
  return __builtin_aarch64_crypto_aesmcv16qi_uu(data);
}

FUNK(uint8x16_t)
vaesimcq_u8(uint8x16_t data) {
  return __builtin_aarch64_crypto_aesimcv16qi_uu(data);
}
#pragma GCC pop_options

FUNK(uint64x1_t)
vcage_f64(float64x1_t __a, float64x1_t __b) {
  return vabs_f64(__a) >= vabs_f64(__b);
}

FUNK(uint32_t)
vcages_f32(float32_t __a, float32_t __b) {
  return __builtin_fabsf(__a) >= __builtin_fabsf(__b) ? -1 : 0;
}

FUNK(uint32x2_t)
vcage_f32(float32x2_t __a, float32x2_t __b) {
  return vabs_f32(__a) >= vabs_f32(__b);
}

FUNK(uint32x4_t)
vcageq_f32(float32x4_t __a, float32x4_t __b) {
  return vabsq_f32(__a) >= vabsq_f32(__b);
}

FUNK(uint64_t)
vcaged_f64(float64_t __a, float64_t __b) {
  return __builtin_fabs(__a) >= __builtin_fabs(__b) ? -1 : 0;
}

FUNK(uint64x2_t)
vcageq_f64(float64x2_t __a, float64x2_t __b) {
  return vabsq_f64(__a) >= vabsq_f64(__b);
}

FUNK(uint32_t)
vcagts_f32(float32_t __a, float32_t __b) {
  return __builtin_fabsf(__a) > __builtin_fabsf(__b) ? -1 : 0;
}

FUNK(uint32x2_t)
vcagt_f32(float32x2_t __a, float32x2_t __b) {
  return vabs_f32(__a) > vabs_f32(__b);
}

FUNK(uint64x1_t)
vcagt_f64(float64x1_t __a, float64x1_t __b) {
  return vabs_f64(__a) > vabs_f64(__b);
}

FUNK(uint32x4_t)
vcagtq_f32(float32x4_t __a, float32x4_t __b) {
  return vabsq_f32(__a) > vabsq_f32(__b);
}

FUNK(uint64_t)
vcagtd_f64(float64_t __a, float64_t __b) {
  return __builtin_fabs(__a) > __builtin_fabs(__b) ? -1 : 0;
}

FUNK(uint64x2_t)
vcagtq_f64(float64x2_t __a, float64x2_t __b) {
  return vabsq_f64(__a) > vabsq_f64(__b);
}

FUNK(uint32x2_t)
vcale_f32(float32x2_t __a, float32x2_t __b) {
  return vabs_f32(__a) <= vabs_f32(__b);
}

FUNK(uint64x1_t)
vcale_f64(float64x1_t __a, float64x1_t __b) {
  return vabs_f64(__a) <= vabs_f64(__b);
}

FUNK(uint64_t)
vcaled_f64(float64_t __a, float64_t __b) {
  return __builtin_fabs(__a) <= __builtin_fabs(__b) ? -1 : 0;
}

FUNK(uint32_t)
vcales_f32(float32_t __a, float32_t __b) {
  return __builtin_fabsf(__a) <= __builtin_fabsf(__b) ? -1 : 0;
}

FUNK(uint32x4_t)
vcaleq_f32(float32x4_t __a, float32x4_t __b) {
  return vabsq_f32(__a) <= vabsq_f32(__b);
}

FUNK(uint64x2_t)
vcaleq_f64(float64x2_t __a, float64x2_t __b) {
  return vabsq_f64(__a) <= vabsq_f64(__b);
}

FUNK(uint32x2_t)
vcalt_f32(float32x2_t __a, float32x2_t __b) {
  return vabs_f32(__a) < vabs_f32(__b);
}

FUNK(uint64x1_t)
vcalt_f64(float64x1_t __a, float64x1_t __b) {
  return vabs_f64(__a) < vabs_f64(__b);
}

FUNK(uint64_t)
vcaltd_f64(float64_t __a, float64_t __b) {
  return __builtin_fabs(__a) < __builtin_fabs(__b) ? -1 : 0;
}

FUNK(uint32x4_t)
vcaltq_f32(float32x4_t __a, float32x4_t __b) {
  return vabsq_f32(__a) < vabsq_f32(__b);
}

FUNK(uint64x2_t)
vcaltq_f64(float64x2_t __a, float64x2_t __b) {
  return vabsq_f64(__a) < vabsq_f64(__b);
}

FUNK(uint32_t)
vcalts_f32(float32_t __a, float32_t __b) {
  return __builtin_fabsf(__a) < __builtin_fabsf(__b) ? -1 : 0;
}

FUNK(uint32x2_t)
vceq_f32(float32x2_t __a, float32x2_t __b) {
  return (uint32x2_t)(__a == __b);
}

FUNK(uint64x1_t)
vceq_f64(float64x1_t __a, float64x1_t __b) {
  return (uint64x1_t)(__a == __b);
}

FUNK(uint8x8_t)
vceq_p8(poly8x8_t __a, poly8x8_t __b) {
  return (uint8x8_t)(__a == __b);
}

FUNK(uint64x1_t)
vceq_p64(poly64x1_t __a, poly64x1_t __b) {
  return (uint64x1_t)(__a == __b);
}

FUNK(uint8x8_t)
vceq_s8(int8x8_t __a, int8x8_t __b) {
  return (uint8x8_t)(__a == __b);
}

FUNK(uint16x4_t)
vceq_s16(int16x4_t __a, int16x4_t __b) {
  return (uint16x4_t)(__a == __b);
}

FUNK(uint32x2_t)
vceq_s32(int32x2_t __a, int32x2_t __b) {
  return (uint32x2_t)(__a == __b);
}

FUNK(uint64x1_t)
vceq_s64(int64x1_t __a, int64x1_t __b) {
  return (uint64x1_t)(__a == __b);
}

FUNK(uint8x8_t)
vceq_u8(uint8x8_t __a, uint8x8_t __b) {
  return (__a == __b);
}

FUNK(uint16x4_t)
vceq_u16(uint16x4_t __a, uint16x4_t __b) {
  return (__a == __b);
}

FUNK(uint32x2_t)
vceq_u32(uint32x2_t __a, uint32x2_t __b) {
  return (__a == __b);
}

FUNK(uint64x1_t)
vceq_u64(uint64x1_t __a, uint64x1_t __b) {
  return (__a == __b);
}

FUNK(uint32x4_t)
vceqq_f32(float32x4_t __a, float32x4_t __b) {
  return (uint32x4_t)(__a == __b);
}

FUNK(uint64x2_t)
vceqq_f64(float64x2_t __a, float64x2_t __b) {
  return (uint64x2_t)(__a == __b);
}

FUNK(uint8x16_t)
vceqq_p8(poly8x16_t __a, poly8x16_t __b) {
  return (uint8x16_t)(__a == __b);
}

FUNK(uint8x16_t)
vceqq_s8(int8x16_t __a, int8x16_t __b) {
  return (uint8x16_t)(__a == __b);
}

FUNK(uint16x8_t)
vceqq_s16(int16x8_t __a, int16x8_t __b) {
  return (uint16x8_t)(__a == __b);
}

FUNK(uint32x4_t)
vceqq_s32(int32x4_t __a, int32x4_t __b) {
  return (uint32x4_t)(__a == __b);
}

FUNK(uint64x2_t)
vceqq_s64(int64x2_t __a, int64x2_t __b) {
  return (uint64x2_t)(__a == __b);
}

FUNK(uint8x16_t)
vceqq_u8(uint8x16_t __a, uint8x16_t __b) {
  return (__a == __b);
}

FUNK(uint16x8_t)
vceqq_u16(uint16x8_t __a, uint16x8_t __b) {
  return (__a == __b);
}

FUNK(uint32x4_t)
vceqq_u32(uint32x4_t __a, uint32x4_t __b) {
  return (__a == __b);
}

FUNK(uint64x2_t)
vceqq_u64(uint64x2_t __a, uint64x2_t __b) {
  return (__a == __b);
}

FUNK(uint32_t)
vceqs_f32(float32_t __a, float32_t __b) {
  return __a == __b ? -1 : 0;
}

FUNK(uint64_t)
vceqd_s64(int64_t __a, int64_t __b) {
  return __a == __b ? -1ll : 0ll;
}

FUNK(uint64_t)
vceqd_u64(uint64_t __a, uint64_t __b) {
  return __a == __b ? -1ll : 0ll;
}

FUNK(uint64_t)
vceqd_f64(float64_t __a, float64_t __b) {
  return __a == __b ? -1ll : 0ll;
}

FUNK(uint32x2_t)
vceqz_f32(float32x2_t __a) {
  return (uint32x2_t)(__a == 0.0f);
}

FUNK(uint64x1_t)
vceqz_f64(float64x1_t __a) {
  return (uint64x1_t)(__a == (float64x1_t){0.0});
}

FUNK(uint8x8_t)
vceqz_p8(poly8x8_t __a) {
  return (uint8x8_t)(__a == 0);
}

FUNK(uint8x8_t)
vceqz_s8(int8x8_t __a) {
  return (uint8x8_t)(__a == 0);
}

FUNK(uint16x4_t)
vceqz_s16(int16x4_t __a) {
  return (uint16x4_t)(__a == 0);
}

FUNK(uint32x2_t)
vceqz_s32(int32x2_t __a) {
  return (uint32x2_t)(__a == 0);
}

FUNK(uint64x1_t)
vceqz_s64(int64x1_t __a) {
  return (uint64x1_t)(__a == __AARCH64_INT64_C(0));
}

FUNK(uint8x8_t)
vceqz_u8(uint8x8_t __a) {
  return (__a == 0);
}

FUNK(uint16x4_t)
vceqz_u16(uint16x4_t __a) {
  return (__a == 0);
}

FUNK(uint32x2_t)
vceqz_u32(uint32x2_t __a) {
  return (__a == 0);
}

FUNK(uint64x1_t)
vceqz_u64(uint64x1_t __a) {
  return (__a == __AARCH64_UINT64_C(0));
}

FUNK(uint32x4_t)
vceqzq_f32(float32x4_t __a) {
  return (uint32x4_t)(__a == 0.0f);
}

FUNK(uint64x2_t)
vceqzq_f64(float64x2_t __a) {
  return (uint64x2_t)(__a == 0.0f);
}

FUNK(uint8x16_t)
vceqzq_p8(poly8x16_t __a) {
  return (uint8x16_t)(__a == 0);
}

FUNK(uint8x16_t)
vceqzq_s8(int8x16_t __a) {
  return (uint8x16_t)(__a == 0);
}

FUNK(uint16x8_t)
vceqzq_s16(int16x8_t __a) {
  return (uint16x8_t)(__a == 0);
}

FUNK(uint32x4_t)
vceqzq_s32(int32x4_t __a) {
  return (uint32x4_t)(__a == 0);
}

FUNK(uint64x2_t)
vceqzq_s64(int64x2_t __a) {
  return (uint64x2_t)(__a == __AARCH64_INT64_C(0));
}

FUNK(uint8x16_t)
vceqzq_u8(uint8x16_t __a) {
  return (__a == 0);
}

FUNK(uint16x8_t)
vceqzq_u16(uint16x8_t __a) {
  return (__a == 0);
}

FUNK(uint32x4_t)
vceqzq_u32(uint32x4_t __a) {
  return (__a == 0);
}

FUNK(uint64x2_t)
vceqzq_u64(uint64x2_t __a) {
  return (__a == __AARCH64_UINT64_C(0));
}

FUNK(uint32_t)
vceqzs_f32(float32_t __a) {
  return __a == 0.0f ? -1 : 0;
}

FUNK(uint64_t)
vceqzd_s64(int64_t __a) {
  return __a == 0 ? -1ll : 0ll;
}

FUNK(uint64_t)
vceqzd_u64(uint64_t __a) {
  return __a == 0 ? -1ll : 0ll;
}

FUNK(uint64_t)
vceqzd_f64(float64_t __a) {
  return __a == 0.0 ? -1ll : 0ll;
}

FUNK(uint32x2_t)
vcge_f32(float32x2_t __a, float32x2_t __b) {
  return (uint32x2_t)(__a >= __b);
}

FUNK(uint64x1_t)
vcge_f64(float64x1_t __a, float64x1_t __b) {
  return (uint64x1_t)(__a >= __b);
}

FUNK(uint8x8_t)
vcge_s8(int8x8_t __a, int8x8_t __b) {
  return (uint8x8_t)(__a >= __b);
}

FUNK(uint16x4_t)
vcge_s16(int16x4_t __a, int16x4_t __b) {
  return (uint16x4_t)(__a >= __b);
}

FUNK(uint32x2_t)
vcge_s32(int32x2_t __a, int32x2_t __b) {
  return (uint32x2_t)(__a >= __b);
}

FUNK(uint64x1_t)
vcge_s64(int64x1_t __a, int64x1_t __b) {
  return (uint64x1_t)(__a >= __b);
}

FUNK(uint8x8_t)
vcge_u8(uint8x8_t __a, uint8x8_t __b) {
  return (__a >= __b);
}

FUNK(uint16x4_t)
vcge_u16(uint16x4_t __a, uint16x4_t __b) {
  return (__a >= __b);
}

FUNK(uint32x2_t)
vcge_u32(uint32x2_t __a, uint32x2_t __b) {
  return (__a >= __b);
}

FUNK(uint64x1_t)
vcge_u64(uint64x1_t __a, uint64x1_t __b) {
  return (__a >= __b);
}

FUNK(uint32x4_t)
vcgeq_f32(float32x4_t __a, float32x4_t __b) {
  return (uint32x4_t)(__a >= __b);
}

FUNK(uint64x2_t)
vcgeq_f64(float64x2_t __a, float64x2_t __b) {
  return (uint64x2_t)(__a >= __b);
}

FUNK(uint8x16_t)
vcgeq_s8(int8x16_t __a, int8x16_t __b) {
  return (uint8x16_t)(__a >= __b);
}

FUNK(uint16x8_t)
vcgeq_s16(int16x8_t __a, int16x8_t __b) {
  return (uint16x8_t)(__a >= __b);
}

FUNK(uint32x4_t)
vcgeq_s32(int32x4_t __a, int32x4_t __b) {
  return (uint32x4_t)(__a >= __b);
}

FUNK(uint64x2_t)
vcgeq_s64(int64x2_t __a, int64x2_t __b) {
  return (uint64x2_t)(__a >= __b);
}

FUNK(uint8x16_t)
vcgeq_u8(uint8x16_t __a, uint8x16_t __b) {
  return (__a >= __b);
}

FUNK(uint16x8_t)
vcgeq_u16(uint16x8_t __a, uint16x8_t __b) {
  return (__a >= __b);
}

FUNK(uint32x4_t)
vcgeq_u32(uint32x4_t __a, uint32x4_t __b) {
  return (__a >= __b);
}

FUNK(uint64x2_t)
vcgeq_u64(uint64x2_t __a, uint64x2_t __b) {
  return (__a >= __b);
}

FUNK(uint32_t)
vcges_f32(float32_t __a, float32_t __b) {
  return __a >= __b ? -1 : 0;
}

FUNK(uint64_t)
vcged_s64(int64_t __a, int64_t __b) {
  return __a >= __b ? -1ll : 0ll;
}

FUNK(uint64_t)
vcged_u64(uint64_t __a, uint64_t __b) {
  return __a >= __b ? -1ll : 0ll;
}

FUNK(uint64_t)
vcged_f64(float64_t __a, float64_t __b) {
  return __a >= __b ? -1ll : 0ll;
}

FUNK(uint32x2_t)
vcgez_f32(float32x2_t __a) {
  return (uint32x2_t)(__a >= 0.0f);
}

FUNK(uint64x1_t)
vcgez_f64(float64x1_t __a) {
  return (uint64x1_t)(__a[0] >= (float64x1_t){0.0});
}

FUNK(uint8x8_t)
vcgez_s8(int8x8_t __a) {
  return (uint8x8_t)(__a >= 0);
}

FUNK(uint16x4_t)
vcgez_s16(int16x4_t __a) {
  return (uint16x4_t)(__a >= 0);
}

FUNK(uint32x2_t)
vcgez_s32(int32x2_t __a) {
  return (uint32x2_t)(__a >= 0);
}

FUNK(uint64x1_t)
vcgez_s64(int64x1_t __a) {
  return (uint64x1_t)(__a >= __AARCH64_INT64_C(0));
}

FUNK(uint32x4_t)
vcgezq_f32(float32x4_t __a) {
  return (uint32x4_t)(__a >= 0.0f);
}

FUNK(uint64x2_t)
vcgezq_f64(float64x2_t __a) {
  return (uint64x2_t)(__a >= 0.0);
}

FUNK(uint8x16_t)
vcgezq_s8(int8x16_t __a) {
  return (uint8x16_t)(__a >= 0);
}

FUNK(uint16x8_t)
vcgezq_s16(int16x8_t __a) {
  return (uint16x8_t)(__a >= 0);
}

FUNK(uint32x4_t)
vcgezq_s32(int32x4_t __a) {
  return (uint32x4_t)(__a >= 0);
}

FUNK(uint64x2_t)
vcgezq_s64(int64x2_t __a) {
  return (uint64x2_t)(__a >= __AARCH64_INT64_C(0));
}

FUNK(uint32_t)
vcgezs_f32(float32_t __a) {
  return __a >= 0.0f ? -1 : 0;
}

FUNK(uint64_t)
vcgezd_s64(int64_t __a) {
  return __a >= 0 ? -1ll : 0ll;
}

FUNK(uint64_t)
vcgezd_f64(float64_t __a) {
  return __a >= 0.0 ? -1ll : 0ll;
}

FUNK(uint32x2_t)
vcgt_f32(float32x2_t __a, float32x2_t __b) {
  return (uint32x2_t)(__a > __b);
}

FUNK(uint64x1_t)
vcgt_f64(float64x1_t __a, float64x1_t __b) {
  return (uint64x1_t)(__a > __b);
}

FUNK(uint8x8_t)
vcgt_s8(int8x8_t __a, int8x8_t __b) {
  return (uint8x8_t)(__a > __b);
}

FUNK(uint16x4_t)
vcgt_s16(int16x4_t __a, int16x4_t __b) {
  return (uint16x4_t)(__a > __b);
}

FUNK(uint32x2_t)
vcgt_s32(int32x2_t __a, int32x2_t __b) {
  return (uint32x2_t)(__a > __b);
}

FUNK(uint64x1_t)
vcgt_s64(int64x1_t __a, int64x1_t __b) {
  return (uint64x1_t)(__a > __b);
}

FUNK(uint8x8_t)
vcgt_u8(uint8x8_t __a, uint8x8_t __b) {
  return (__a > __b);
}

FUNK(uint16x4_t)
vcgt_u16(uint16x4_t __a, uint16x4_t __b) {
  return (__a > __b);
}

FUNK(uint32x2_t)
vcgt_u32(uint32x2_t __a, uint32x2_t __b) {
  return (__a > __b);
}

FUNK(uint64x1_t)
vcgt_u64(uint64x1_t __a, uint64x1_t __b) {
  return (__a > __b);
}

FUNK(uint32x4_t)
vcgtq_f32(float32x4_t __a, float32x4_t __b) {
  return (uint32x4_t)(__a > __b);
}

FUNK(uint64x2_t)
vcgtq_f64(float64x2_t __a, float64x2_t __b) {
  return (uint64x2_t)(__a > __b);
}

FUNK(uint8x16_t)
vcgtq_s8(int8x16_t __a, int8x16_t __b) {
  return (uint8x16_t)(__a > __b);
}

FUNK(uint16x8_t)
vcgtq_s16(int16x8_t __a, int16x8_t __b) {
  return (uint16x8_t)(__a > __b);
}

FUNK(uint32x4_t)
vcgtq_s32(int32x4_t __a, int32x4_t __b) {
  return (uint32x4_t)(__a > __b);
}

FUNK(uint64x2_t)
vcgtq_s64(int64x2_t __a, int64x2_t __b) {
  return (uint64x2_t)(__a > __b);
}

FUNK(uint8x16_t)
vcgtq_u8(uint8x16_t __a, uint8x16_t __b) {
  return (__a > __b);
}

FUNK(uint16x8_t)
vcgtq_u16(uint16x8_t __a, uint16x8_t __b) {
  return (__a > __b);
}

FUNK(uint32x4_t)
vcgtq_u32(uint32x4_t __a, uint32x4_t __b) {
  return (__a > __b);
}

FUNK(uint64x2_t)
vcgtq_u64(uint64x2_t __a, uint64x2_t __b) {
  return (__a > __b);
}

FUNK(uint32_t)
vcgts_f32(float32_t __a, float32_t __b) {
  return __a > __b ? -1 : 0;
}

FUNK(uint64_t)
vcgtd_s64(int64_t __a, int64_t __b) {
  return __a > __b ? -1ll : 0ll;
}

FUNK(uint64_t)
vcgtd_u64(uint64_t __a, uint64_t __b) {
  return __a > __b ? -1ll : 0ll;
}

FUNK(uint64_t)
vcgtd_f64(float64_t __a, float64_t __b) {
  return __a > __b ? -1ll : 0ll;
}

FUNK(uint32x2_t)
vcgtz_f32(float32x2_t __a) {
  return (uint32x2_t)(__a > 0.0f);
}

FUNK(uint64x1_t)
vcgtz_f64(float64x1_t __a) {
  return (uint64x1_t)(__a > (float64x1_t){0.0});
}

FUNK(uint8x8_t)
vcgtz_s8(int8x8_t __a) {
  return (uint8x8_t)(__a > 0);
}

FUNK(uint16x4_t)
vcgtz_s16(int16x4_t __a) {
  return (uint16x4_t)(__a > 0);
}

FUNK(uint32x2_t)
vcgtz_s32(int32x2_t __a) {
  return (uint32x2_t)(__a > 0);
}

FUNK(uint64x1_t)
vcgtz_s64(int64x1_t __a) {
  return (uint64x1_t)(__a > __AARCH64_INT64_C(0));
}

FUNK(uint32x4_t)
vcgtzq_f32(float32x4_t __a) {
  return (uint32x4_t)(__a > 0.0f);
}

FUNK(uint64x2_t)
vcgtzq_f64(float64x2_t __a) {
  return (uint64x2_t)(__a > 0.0);
}

FUNK(uint8x16_t)
vcgtzq_s8(int8x16_t __a) {
  return (uint8x16_t)(__a > 0);
}

FUNK(uint16x8_t)
vcgtzq_s16(int16x8_t __a) {
  return (uint16x8_t)(__a > 0);
}

FUNK(uint32x4_t)
vcgtzq_s32(int32x4_t __a) {
  return (uint32x4_t)(__a > 0);
}

FUNK(uint64x2_t)
vcgtzq_s64(int64x2_t __a) {
  return (uint64x2_t)(__a > __AARCH64_INT64_C(0));
}

FUNK(uint32_t)
vcgtzs_f32(float32_t __a) {
  return __a > 0.0f ? -1 : 0;
}

FUNK(uint64_t)
vcgtzd_s64(int64_t __a) {
  return __a > 0 ? -1ll : 0ll;
}

FUNK(uint64_t)
vcgtzd_f64(float64_t __a) {
  return __a > 0.0 ? -1ll : 0ll;
}

FUNK(uint32x2_t)
vcle_f32(float32x2_t __a, float32x2_t __b) {
  return (uint32x2_t)(__a <= __b);
}

FUNK(uint64x1_t)
vcle_f64(float64x1_t __a, float64x1_t __b) {
  return (uint64x1_t)(__a <= __b);
}

FUNK(uint8x8_t)
vcle_s8(int8x8_t __a, int8x8_t __b) {
  return (uint8x8_t)(__a <= __b);
}

FUNK(uint16x4_t)
vcle_s16(int16x4_t __a, int16x4_t __b) {
  return (uint16x4_t)(__a <= __b);
}

FUNK(uint32x2_t)
vcle_s32(int32x2_t __a, int32x2_t __b) {
  return (uint32x2_t)(__a <= __b);
}

FUNK(uint64x1_t)
vcle_s64(int64x1_t __a, int64x1_t __b) {
  return (uint64x1_t)(__a <= __b);
}

FUNK(uint8x8_t)
vcle_u8(uint8x8_t __a, uint8x8_t __b) {
  return (__a <= __b);
}

FUNK(uint16x4_t)
vcle_u16(uint16x4_t __a, uint16x4_t __b) {
  return (__a <= __b);
}

FUNK(uint32x2_t)
vcle_u32(uint32x2_t __a, uint32x2_t __b) {
  return (__a <= __b);
}

FUNK(uint64x1_t)
vcle_u64(uint64x1_t __a, uint64x1_t __b) {
  return (__a <= __b);
}

FUNK(uint32x4_t)
vcleq_f32(float32x4_t __a, float32x4_t __b) {
  return (uint32x4_t)(__a <= __b);
}

FUNK(uint64x2_t)
vcleq_f64(float64x2_t __a, float64x2_t __b) {
  return (uint64x2_t)(__a <= __b);
}

FUNK(uint8x16_t)
vcleq_s8(int8x16_t __a, int8x16_t __b) {
  return (uint8x16_t)(__a <= __b);
}

FUNK(uint16x8_t)
vcleq_s16(int16x8_t __a, int16x8_t __b) {
  return (uint16x8_t)(__a <= __b);
}

FUNK(uint32x4_t)
vcleq_s32(int32x4_t __a, int32x4_t __b) {
  return (uint32x4_t)(__a <= __b);
}

FUNK(uint64x2_t)
vcleq_s64(int64x2_t __a, int64x2_t __b) {
  return (uint64x2_t)(__a <= __b);
}

FUNK(uint8x16_t)
vcleq_u8(uint8x16_t __a, uint8x16_t __b) {
  return (__a <= __b);
}

FUNK(uint16x8_t)
vcleq_u16(uint16x8_t __a, uint16x8_t __b) {
  return (__a <= __b);
}

FUNK(uint32x4_t)
vcleq_u32(uint32x4_t __a, uint32x4_t __b) {
  return (__a <= __b);
}

FUNK(uint64x2_t)
vcleq_u64(uint64x2_t __a, uint64x2_t __b) {
  return (__a <= __b);
}

FUNK(uint32_t)
vcles_f32(float32_t __a, float32_t __b) {
  return __a <= __b ? -1 : 0;
}

FUNK(uint64_t)
vcled_s64(int64_t __a, int64_t __b) {
  return __a <= __b ? -1ll : 0ll;
}

FUNK(uint64_t)
vcled_u64(uint64_t __a, uint64_t __b) {
  return __a <= __b ? -1ll : 0ll;
}

FUNK(uint64_t)
vcled_f64(float64_t __a, float64_t __b) {
  return __a <= __b ? -1ll : 0ll;
}

FUNK(uint32x2_t)
vclez_f32(float32x2_t __a) {
  return (uint32x2_t)(__a <= 0.0f);
}

FUNK(uint64x1_t)
vclez_f64(float64x1_t __a) {
  return (uint64x1_t)(__a <= (float64x1_t){0.0});
}

FUNK(uint8x8_t)
vclez_s8(int8x8_t __a) {
  return (uint8x8_t)(__a <= 0);
}

FUNK(uint16x4_t)
vclez_s16(int16x4_t __a) {
  return (uint16x4_t)(__a <= 0);
}

FUNK(uint32x2_t)
vclez_s32(int32x2_t __a) {
  return (uint32x2_t)(__a <= 0);
}

FUNK(uint64x1_t)
vclez_s64(int64x1_t __a) {
  return (uint64x1_t)(__a <= __AARCH64_INT64_C(0));
}

FUNK(uint32x4_t)
vclezq_f32(float32x4_t __a) {
  return (uint32x4_t)(__a <= 0.0f);
}

FUNK(uint64x2_t)
vclezq_f64(float64x2_t __a) {
  return (uint64x2_t)(__a <= 0.0);
}

FUNK(uint8x16_t)
vclezq_s8(int8x16_t __a) {
  return (uint8x16_t)(__a <= 0);
}

FUNK(uint16x8_t)
vclezq_s16(int16x8_t __a) {
  return (uint16x8_t)(__a <= 0);
}

FUNK(uint32x4_t)
vclezq_s32(int32x4_t __a) {
  return (uint32x4_t)(__a <= 0);
}

FUNK(uint64x2_t)
vclezq_s64(int64x2_t __a) {
  return (uint64x2_t)(__a <= __AARCH64_INT64_C(0));
}

FUNK(uint32_t)
vclezs_f32(float32_t __a) {
  return __a <= 0.0f ? -1 : 0;
}

FUNK(uint64_t)
vclezd_s64(int64_t __a) {
  return __a <= 0 ? -1ll : 0ll;
}

FUNK(uint64_t)
vclezd_f64(float64_t __a) {
  return __a <= 0.0 ? -1ll : 0ll;
}

FUNK(uint32x2_t)
vclt_f32(float32x2_t __a, float32x2_t __b) {
  return (uint32x2_t)(__a < __b);
}

FUNK(uint64x1_t)
vclt_f64(float64x1_t __a, float64x1_t __b) {
  return (uint64x1_t)(__a < __b);
}

FUNK(uint8x8_t)
vclt_s8(int8x8_t __a, int8x8_t __b) {
  return (uint8x8_t)(__a < __b);
}

FUNK(uint16x4_t)
vclt_s16(int16x4_t __a, int16x4_t __b) {
  return (uint16x4_t)(__a < __b);
}

FUNK(uint32x2_t)
vclt_s32(int32x2_t __a, int32x2_t __b) {
  return (uint32x2_t)(__a < __b);
}

FUNK(uint64x1_t)
vclt_s64(int64x1_t __a, int64x1_t __b) {
  return (uint64x1_t)(__a < __b);
}

FUNK(uint8x8_t)
vclt_u8(uint8x8_t __a, uint8x8_t __b) {
  return (__a < __b);
}

FUNK(uint16x4_t)
vclt_u16(uint16x4_t __a, uint16x4_t __b) {
  return (__a < __b);
}

FUNK(uint32x2_t)
vclt_u32(uint32x2_t __a, uint32x2_t __b) {
  return (__a < __b);
}

FUNK(uint64x1_t)
vclt_u64(uint64x1_t __a, uint64x1_t __b) {
  return (__a < __b);
}

FUNK(uint32x4_t)
vcltq_f32(float32x4_t __a, float32x4_t __b) {
  return (uint32x4_t)(__a < __b);
}

FUNK(uint64x2_t)
vcltq_f64(float64x2_t __a, float64x2_t __b) {
  return (uint64x2_t)(__a < __b);
}

FUNK(uint8x16_t)
vcltq_s8(int8x16_t __a, int8x16_t __b) {
  return (uint8x16_t)(__a < __b);
}

FUNK(uint16x8_t)
vcltq_s16(int16x8_t __a, int16x8_t __b) {
  return (uint16x8_t)(__a < __b);
}

FUNK(uint32x4_t)
vcltq_s32(int32x4_t __a, int32x4_t __b) {
  return (uint32x4_t)(__a < __b);
}

FUNK(uint64x2_t)
vcltq_s64(int64x2_t __a, int64x2_t __b) {
  return (uint64x2_t)(__a < __b);
}

FUNK(uint8x16_t)
vcltq_u8(uint8x16_t __a, uint8x16_t __b) {
  return (__a < __b);
}

FUNK(uint16x8_t)
vcltq_u16(uint16x8_t __a, uint16x8_t __b) {
  return (__a < __b);
}

FUNK(uint32x4_t)
vcltq_u32(uint32x4_t __a, uint32x4_t __b) {
  return (__a < __b);
}

FUNK(uint64x2_t)
vcltq_u64(uint64x2_t __a, uint64x2_t __b) {
  return (__a < __b);
}

FUNK(uint32_t)
vclts_f32(float32_t __a, float32_t __b) {
  return __a < __b ? -1 : 0;
}

FUNK(uint64_t)
vcltd_s64(int64_t __a, int64_t __b) {
  return __a < __b ? -1ll : 0ll;
}

FUNK(uint64_t)
vcltd_u64(uint64_t __a, uint64_t __b) {
  return __a < __b ? -1ll : 0ll;
}

FUNK(uint64_t)
vcltd_f64(float64_t __a, float64_t __b) {
  return __a < __b ? -1ll : 0ll;
}

FUNK(uint32x2_t)
vcltz_f32(float32x2_t __a) {
  return (uint32x2_t)(__a < 0.0f);
}

FUNK(uint64x1_t)
vcltz_f64(float64x1_t __a) {
  return (uint64x1_t)(__a < (float64x1_t){0.0});
}

FUNK(uint8x8_t)
vcltz_s8(int8x8_t __a) {
  return (uint8x8_t)(__a < 0);
}

FUNK(uint16x4_t)
vcltz_s16(int16x4_t __a) {
  return (uint16x4_t)(__a < 0);
}

FUNK(uint32x2_t)
vcltz_s32(int32x2_t __a) {
  return (uint32x2_t)(__a < 0);
}

FUNK(uint64x1_t)
vcltz_s64(int64x1_t __a) {
  return (uint64x1_t)(__a < __AARCH64_INT64_C(0));
}

FUNK(uint32x4_t)
vcltzq_f32(float32x4_t __a) {
  return (uint32x4_t)(__a < 0.0f);
}

FUNK(uint64x2_t)
vcltzq_f64(float64x2_t __a) {
  return (uint64x2_t)(__a < 0.0);
}

FUNK(uint8x16_t)
vcltzq_s8(int8x16_t __a) {
  return (uint8x16_t)(__a < 0);
}

FUNK(uint16x8_t)
vcltzq_s16(int16x8_t __a) {
  return (uint16x8_t)(__a < 0);
}

FUNK(uint32x4_t)
vcltzq_s32(int32x4_t __a) {
  return (uint32x4_t)(__a < 0);
}

FUNK(uint64x2_t)
vcltzq_s64(int64x2_t __a) {
  return (uint64x2_t)(__a < __AARCH64_INT64_C(0));
}

FUNK(uint32_t)
vcltzs_f32(float32_t __a) {
  return __a < 0.0f ? -1 : 0;
}

FUNK(uint64_t)
vcltzd_s64(int64_t __a) {
  return __a < 0 ? -1ll : 0ll;
}

FUNK(uint64_t)
vcltzd_f64(float64_t __a) {
  return __a < 0.0 ? -1ll : 0ll;
}

FUNK(int8x8_t)
vcls_s8(int8x8_t __a) {
  return __builtin_aarch64_clrsbv8qi(__a);
}

FUNK(int16x4_t)
vcls_s16(int16x4_t __a) {
  return __builtin_aarch64_clrsbv4hi(__a);
}

FUNK(int32x2_t)
vcls_s32(int32x2_t __a) {
  return __builtin_aarch64_clrsbv2si(__a);
}

FUNK(int8x16_t)
vclsq_s8(int8x16_t __a) {
  return __builtin_aarch64_clrsbv16qi(__a);
}

FUNK(int16x8_t)
vclsq_s16(int16x8_t __a) {
  return __builtin_aarch64_clrsbv8hi(__a);
}

FUNK(int32x4_t)
vclsq_s32(int32x4_t __a) {
  return __builtin_aarch64_clrsbv4si(__a);
}

FUNK(int8x8_t)
vclz_s8(int8x8_t __a) {
  return __builtin_aarch64_clzv8qi(__a);
}

FUNK(int16x4_t)
vclz_s16(int16x4_t __a) {
  return __builtin_aarch64_clzv4hi(__a);
}

FUNK(int32x2_t)
vclz_s32(int32x2_t __a) {
  return __builtin_aarch64_clzv2si(__a);
}

FUNK(uint8x8_t)
vclz_u8(uint8x8_t __a) {
  return (uint8x8_t)__builtin_aarch64_clzv8qi((int8x8_t)__a);
}

FUNK(uint16x4_t)
vclz_u16(uint16x4_t __a) {
  return (uint16x4_t)__builtin_aarch64_clzv4hi((int16x4_t)__a);
}

FUNK(uint32x2_t)
vclz_u32(uint32x2_t __a) {
  return (uint32x2_t)__builtin_aarch64_clzv2si((int32x2_t)__a);
}

FUNK(int8x16_t)
vclzq_s8(int8x16_t __a) {
  return __builtin_aarch64_clzv16qi(__a);
}

FUNK(int16x8_t)
vclzq_s16(int16x8_t __a) {
  return __builtin_aarch64_clzv8hi(__a);
}

FUNK(int32x4_t)
vclzq_s32(int32x4_t __a) {
  return __builtin_aarch64_clzv4si(__a);
}

FUNK(uint8x16_t)
vclzq_u8(uint8x16_t __a) {
  return (uint8x16_t)__builtin_aarch64_clzv16qi((int8x16_t)__a);
}

FUNK(uint16x8_t)
vclzq_u16(uint16x8_t __a) {
  return (uint16x8_t)__builtin_aarch64_clzv8hi((int16x8_t)__a);
}

FUNK(uint32x4_t)
vclzq_u32(uint32x4_t __a) {
  return (uint32x4_t)__builtin_aarch64_clzv4si((int32x4_t)__a);
}

FUNK(poly8x8_t)
vcnt_p8(poly8x8_t __a) {
  return (poly8x8_t)__builtin_aarch64_popcountv8qi((int8x8_t)__a);
}

FUNK(int8x8_t)
vcnt_s8(int8x8_t __a) {
  return __builtin_aarch64_popcountv8qi(__a);
}

FUNK(uint8x8_t)
vcnt_u8(uint8x8_t __a) {
  return (uint8x8_t)__builtin_aarch64_popcountv8qi((int8x8_t)__a);
}

FUNK(poly8x16_t)
vcntq_p8(poly8x16_t __a) {
  return (poly8x16_t)__builtin_aarch64_popcountv16qi((int8x16_t)__a);
}

FUNK(int8x16_t)
vcntq_s8(int8x16_t __a) {
  return __builtin_aarch64_popcountv16qi(__a);
}

FUNK(uint8x16_t)
vcntq_u8(uint8x16_t __a) {
  return (uint8x16_t)__builtin_aarch64_popcountv16qi((int8x16_t)__a);
}

FUNK(float32x2_t)
vcopy_lane_f32(float32x2_t __a, const int __lane1, float32x2_t __b,
               const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(float64x1_t)
vcopy_lane_f64(float64x1_t __a, const int __lane1, float64x1_t __b,
               const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(poly8x8_t)
vcopy_lane_p8(poly8x8_t __a, const int __lane1, poly8x8_t __b,
              const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(poly16x4_t)
vcopy_lane_p16(poly16x4_t __a, const int __lane1, poly16x4_t __b,
               const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(poly64x1_t)
vcopy_lane_p64(poly64x1_t __a, const int __lane1, poly64x1_t __b,
               const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(int8x8_t)
vcopy_lane_s8(int8x8_t __a, const int __lane1, int8x8_t __b,
              const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(int16x4_t)
vcopy_lane_s16(int16x4_t __a, const int __lane1, int16x4_t __b,
               const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(int32x2_t)
vcopy_lane_s32(int32x2_t __a, const int __lane1, int32x2_t __b,
               const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(int64x1_t)
vcopy_lane_s64(int64x1_t __a, const int __lane1, int64x1_t __b,
               const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(uint8x8_t)
vcopy_lane_u8(uint8x8_t __a, const int __lane1, uint8x8_t __b,
              const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(uint16x4_t)
vcopy_lane_u16(uint16x4_t __a, const int __lane1, uint16x4_t __b,
               const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(uint32x2_t)
vcopy_lane_u32(uint32x2_t __a, const int __lane1, uint32x2_t __b,
               const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(uint64x1_t)
vcopy_lane_u64(uint64x1_t __a, const int __lane1, uint64x1_t __b,
               const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(float32x2_t)
vcopy_laneq_f32(float32x2_t __a, const int __lane1, float32x4_t __b,
                const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(float64x1_t)
vcopy_laneq_f64(float64x1_t __a, const int __lane1, float64x2_t __b,
                const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(poly8x8_t)
vcopy_laneq_p8(poly8x8_t __a, const int __lane1, poly8x16_t __b,
               const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(poly16x4_t)
vcopy_laneq_p16(poly16x4_t __a, const int __lane1, poly16x8_t __b,
                const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(poly64x1_t)
vcopy_laneq_p64(poly64x1_t __a, const int __lane1, poly64x2_t __b,
                const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(int8x8_t)
vcopy_laneq_s8(int8x8_t __a, const int __lane1, int8x16_t __b,
               const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(int16x4_t)
vcopy_laneq_s16(int16x4_t __a, const int __lane1, int16x8_t __b,
                const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(int32x2_t)
vcopy_laneq_s32(int32x2_t __a, const int __lane1, int32x4_t __b,
                const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(int64x1_t)
vcopy_laneq_s64(int64x1_t __a, const int __lane1, int64x2_t __b,
                const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(uint8x8_t)
vcopy_laneq_u8(uint8x8_t __a, const int __lane1, uint8x16_t __b,
               const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(uint16x4_t)
vcopy_laneq_u16(uint16x4_t __a, const int __lane1, uint16x8_t __b,
                const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(uint32x2_t)
vcopy_laneq_u32(uint32x2_t __a, const int __lane1, uint32x4_t __b,
                const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(uint64x1_t)
vcopy_laneq_u64(uint64x1_t __a, const int __lane1, uint64x2_t __b,
                const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(float32x4_t)
vcopyq_lane_f32(float32x4_t __a, const int __lane1, float32x2_t __b,
                const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(float64x2_t)
vcopyq_lane_f64(float64x2_t __a, const int __lane1, float64x1_t __b,
                const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(poly8x16_t)
vcopyq_lane_p8(poly8x16_t __a, const int __lane1, poly8x8_t __b,
               const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(poly16x8_t)
vcopyq_lane_p16(poly16x8_t __a, const int __lane1, poly16x4_t __b,
                const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(poly64x2_t)
vcopyq_lane_p64(poly64x2_t __a, const int __lane1, poly64x1_t __b,
                const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(int8x16_t)
vcopyq_lane_s8(int8x16_t __a, const int __lane1, int8x8_t __b,
               const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(int16x8_t)
vcopyq_lane_s16(int16x8_t __a, const int __lane1, int16x4_t __b,
                const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(int32x4_t)
vcopyq_lane_s32(int32x4_t __a, const int __lane1, int32x2_t __b,
                const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(int64x2_t)
vcopyq_lane_s64(int64x2_t __a, const int __lane1, int64x1_t __b,
                const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(uint8x16_t)
vcopyq_lane_u8(uint8x16_t __a, const int __lane1, uint8x8_t __b,
               const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(uint16x8_t)
vcopyq_lane_u16(uint16x8_t __a, const int __lane1, uint16x4_t __b,
                const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(uint32x4_t)
vcopyq_lane_u32(uint32x4_t __a, const int __lane1, uint32x2_t __b,
                const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(uint64x2_t)
vcopyq_lane_u64(uint64x2_t __a, const int __lane1, uint64x1_t __b,
                const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(float32x4_t)
vcopyq_laneq_f32(float32x4_t __a, const int __lane1, float32x4_t __b,
                 const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(float64x2_t)
vcopyq_laneq_f64(float64x2_t __a, const int __lane1, float64x2_t __b,
                 const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(poly8x16_t)
vcopyq_laneq_p8(poly8x16_t __a, const int __lane1, poly8x16_t __b,
                const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(poly16x8_t)
vcopyq_laneq_p16(poly16x8_t __a, const int __lane1, poly16x8_t __b,
                 const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(poly64x2_t)
vcopyq_laneq_p64(poly64x2_t __a, const int __lane1, poly64x2_t __b,
                 const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(int8x16_t)
vcopyq_laneq_s8(int8x16_t __a, const int __lane1, int8x16_t __b,
                const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(int16x8_t)
vcopyq_laneq_s16(int16x8_t __a, const int __lane1, int16x8_t __b,
                 const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(int32x4_t)
vcopyq_laneq_s32(int32x4_t __a, const int __lane1, int32x4_t __b,
                 const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(int64x2_t)
vcopyq_laneq_s64(int64x2_t __a, const int __lane1, int64x2_t __b,
                 const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(uint8x16_t)
vcopyq_laneq_u8(uint8x16_t __a, const int __lane1, uint8x16_t __b,
                const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(uint16x8_t)
vcopyq_laneq_u16(uint16x8_t __a, const int __lane1, uint16x8_t __b,
                 const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(uint32x4_t)
vcopyq_laneq_u32(uint32x4_t __a, const int __lane1, uint32x4_t __b,
                 const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(uint64x2_t)
vcopyq_laneq_u64(uint64x2_t __a, const int __lane1, uint64x2_t __b,
                 const int __lane2) {
  return __aarch64_vset_lane_any(__aarch64_vget_lane_any(__b, __lane2), __a,
                                 __lane1);
}

FUNK(float16x4_t)
vcvt_f16_f32(float32x4_t __a) {
  return __builtin_aarch64_float_truncate_lo_v4hf(__a);
}

FUNK(float16x8_t)
vcvt_high_f16_f32(float16x4_t __a, float32x4_t __b) {
  return __builtin_aarch64_float_truncate_hi_v8hf(__a, __b);
}

FUNK(float32x2_t)
vcvt_f32_f64(float64x2_t __a) {
  return __builtin_aarch64_float_truncate_lo_v2sf(__a);
}

FUNK(float32x4_t)
vcvt_high_f32_f64(float32x2_t __a, float64x2_t __b) {
  return __builtin_aarch64_float_truncate_hi_v4sf(__a, __b);
}

FUNK(float32x4_t)
vcvt_f32_f16(float16x4_t __a) {
  return __builtin_aarch64_float_extend_lo_v4sf(__a);
}

FUNK(float64x2_t)
vcvt_f64_f32(float32x2_t __a) {

  return __builtin_aarch64_float_extend_lo_v2df(__a);
}

FUNK(float32x4_t)
vcvt_high_f32_f16(float16x8_t __a) {
  return __builtin_aarch64_vec_unpacks_hi_v8hf(__a);
}

FUNK(float64x2_t)
vcvt_high_f64_f32(float32x4_t __a) {
  return __builtin_aarch64_vec_unpacks_hi_v4sf(__a);
}

FUNK(float64_t)
vcvtd_n_f64_s64(int64_t __a, const int __b) {
  return __builtin_aarch64_scvtfdi(__a, __b);
}

FUNK(float64_t)
vcvtd_n_f64_u64(uint64_t __a, const int __b) {
  return __builtin_aarch64_ucvtfdi_sus(__a, __b);
}

FUNK(float32_t)
vcvts_n_f32_s32(int32_t __a, const int __b) {
  return __builtin_aarch64_scvtfsi(__a, __b);
}

FUNK(float32_t)
vcvts_n_f32_u32(uint32_t __a, const int __b) {
  return __builtin_aarch64_ucvtfsi_sus(__a, __b);
}

FUNK(float32x2_t)
vcvt_n_f32_s32(int32x2_t __a, const int __b) {
  return __builtin_aarch64_scvtfv2si(__a, __b);
}

FUNK(float32x2_t)
vcvt_n_f32_u32(uint32x2_t __a, const int __b) {
  return __builtin_aarch64_ucvtfv2si_sus(__a, __b);
}

FUNK(float64x1_t)
vcvt_n_f64_s64(int64x1_t __a, const int __b) {
  return (float64x1_t){__builtin_aarch64_scvtfdi(vget_lane_s64(__a, 0), __b)};
}

FUNK(float64x1_t)
vcvt_n_f64_u64(uint64x1_t __a, const int __b) {
  return (float64x1_t){
      __builtin_aarch64_ucvtfdi_sus(vget_lane_u64(__a, 0), __b)};
}

FUNK(float32x4_t)
vcvtq_n_f32_s32(int32x4_t __a, const int __b) {
  return __builtin_aarch64_scvtfv4si(__a, __b);
}

FUNK(float32x4_t)
vcvtq_n_f32_u32(uint32x4_t __a, const int __b) {
  return __builtin_aarch64_ucvtfv4si_sus(__a, __b);
}

FUNK(float64x2_t)
vcvtq_n_f64_s64(int64x2_t __a, const int __b) {
  return __builtin_aarch64_scvtfv2di(__a, __b);
}

FUNK(float64x2_t)
vcvtq_n_f64_u64(uint64x2_t __a, const int __b) {
  return __builtin_aarch64_ucvtfv2di_sus(__a, __b);
}

FUNK(int64_t)
vcvtd_n_s64_f64(float64_t __a, const int __b) {
  return __builtin_aarch64_fcvtzsdf(__a, __b);
}

FUNK(uint64_t)
vcvtd_n_u64_f64(float64_t __a, const int __b) {
  return __builtin_aarch64_fcvtzudf_uss(__a, __b);
}

FUNK(int32_t)
vcvts_n_s32_f32(float32_t __a, const int __b) {
  return __builtin_aarch64_fcvtzssf(__a, __b);
}

FUNK(uint32_t)
vcvts_n_u32_f32(float32_t __a, const int __b) {
  return __builtin_aarch64_fcvtzusf_uss(__a, __b);
}

FUNK(int32x2_t)
vcvt_n_s32_f32(float32x2_t __a, const int __b) {
  return __builtin_aarch64_fcvtzsv2sf(__a, __b);
}

FUNK(uint32x2_t)
vcvt_n_u32_f32(float32x2_t __a, const int __b) {
  return __builtin_aarch64_fcvtzuv2sf_uss(__a, __b);
}

FUNK(int64x1_t)
vcvt_n_s64_f64(float64x1_t __a, const int __b) {
  return (int64x1_t){__builtin_aarch64_fcvtzsdf(vget_lane_f64(__a, 0), __b)};
}

FUNK(uint64x1_t)
vcvt_n_u64_f64(float64x1_t __a, const int __b) {
  return (uint64x1_t){
      __builtin_aarch64_fcvtzudf_uss(vget_lane_f64(__a, 0), __b)};
}

FUNK(int32x4_t)
vcvtq_n_s32_f32(float32x4_t __a, const int __b) {
  return __builtin_aarch64_fcvtzsv4sf(__a, __b);
}

FUNK(uint32x4_t)
vcvtq_n_u32_f32(float32x4_t __a, const int __b) {
  return __builtin_aarch64_fcvtzuv4sf_uss(__a, __b);
}

FUNK(int64x2_t)
vcvtq_n_s64_f64(float64x2_t __a, const int __b) {
  return __builtin_aarch64_fcvtzsv2df(__a, __b);
}

FUNK(uint64x2_t)
vcvtq_n_u64_f64(float64x2_t __a, const int __b) {
  return __builtin_aarch64_fcvtzuv2df_uss(__a, __b);
}

FUNK(float64_t)
vcvtd_f64_s64(int64_t __a) {
  return (float64_t)__a;
}

FUNK(float64_t)
vcvtd_f64_u64(uint64_t __a) {
  return (float64_t)__a;
}

FUNK(float32_t)
vcvts_f32_s32(int32_t __a) {
  return (float32_t)__a;
}

FUNK(float32_t)
vcvts_f32_u32(uint32_t __a) {
  return (float32_t)__a;
}

FUNK(float32x2_t)
vcvt_f32_s32(int32x2_t __a) {
  return __builtin_aarch64_floatv2siv2sf(__a);
}

FUNK(float32x2_t)
vcvt_f32_u32(uint32x2_t __a) {
  return __builtin_aarch64_floatunsv2siv2sf((int32x2_t)__a);
}

FUNK(float64x1_t)
vcvt_f64_s64(int64x1_t __a) {
  return (float64x1_t){vget_lane_s64(__a, 0)};
}

FUNK(float64x1_t)
vcvt_f64_u64(uint64x1_t __a) {
  return (float64x1_t){vget_lane_u64(__a, 0)};
}

FUNK(float32x4_t)
vcvtq_f32_s32(int32x4_t __a) {
  return __builtin_aarch64_floatv4siv4sf(__a);
}

FUNK(float32x4_t)
vcvtq_f32_u32(uint32x4_t __a) {
  return __builtin_aarch64_floatunsv4siv4sf((int32x4_t)__a);
}

FUNK(float64x2_t)
vcvtq_f64_s64(int64x2_t __a) {
  return __builtin_aarch64_floatv2div2df(__a);
}

FUNK(float64x2_t)
vcvtq_f64_u64(uint64x2_t __a) {
  return __builtin_aarch64_floatunsv2div2df((int64x2_t)__a);
}

FUNK(int64_t)
vcvtd_s64_f64(float64_t __a) {
  return (int64_t)__a;
}

FUNK(uint64_t)
vcvtd_u64_f64(float64_t __a) {
  return (uint64_t)__a;
}

FUNK(int32_t)
vcvts_s32_f32(float32_t __a) {
  return (int32_t)__a;
}

FUNK(uint32_t)
vcvts_u32_f32(float32_t __a) {
  return (uint32_t)__a;
}

FUNK(int32x2_t)
vcvt_s32_f32(float32x2_t __a) {
  return __builtin_aarch64_lbtruncv2sfv2si(__a);
}

FUNK(uint32x2_t)
vcvt_u32_f32(float32x2_t __a) {
  return __builtin_aarch64_lbtruncuv2sfv2si_us(__a);
}

FUNK(int32x4_t)
vcvtq_s32_f32(float32x4_t __a) {
  return __builtin_aarch64_lbtruncv4sfv4si(__a);
}

FUNK(uint32x4_t)
vcvtq_u32_f32(float32x4_t __a) {
  return __builtin_aarch64_lbtruncuv4sfv4si_us(__a);
}

FUNK(int64x1_t)
vcvt_s64_f64(float64x1_t __a) {
  return (int64x1_t){vcvtd_s64_f64(__a[0])};
}

FUNK(uint64x1_t)
vcvt_u64_f64(float64x1_t __a) {
  return (uint64x1_t){vcvtd_u64_f64(__a[0])};
}

FUNK(int64x2_t)
vcvtq_s64_f64(float64x2_t __a) {
  return __builtin_aarch64_lbtruncv2dfv2di(__a);
}

FUNK(uint64x2_t)
vcvtq_u64_f64(float64x2_t __a) {
  return __builtin_aarch64_lbtruncuv2dfv2di_us(__a);
}

FUNK(int64_t)
vcvtad_s64_f64(float64_t __a) {
  return __builtin_aarch64_lrounddfdi(__a);
}

FUNK(uint64_t)
vcvtad_u64_f64(float64_t __a) {
  return __builtin_aarch64_lroundudfdi_us(__a);
}

FUNK(int32_t)
vcvtas_s32_f32(float32_t __a) {
  return __builtin_aarch64_lroundsfsi(__a);
}

FUNK(uint32_t)
vcvtas_u32_f32(float32_t __a) {
  return __builtin_aarch64_lroundusfsi_us(__a);
}

FUNK(int32x2_t)
vcvta_s32_f32(float32x2_t __a) {
  return __builtin_aarch64_lroundv2sfv2si(__a);
}

FUNK(uint32x2_t)
vcvta_u32_f32(float32x2_t __a) {
  return __builtin_aarch64_lrounduv2sfv2si_us(__a);
}

FUNK(int32x4_t)
vcvtaq_s32_f32(float32x4_t __a) {
  return __builtin_aarch64_lroundv4sfv4si(__a);
}

FUNK(uint32x4_t)
vcvtaq_u32_f32(float32x4_t __a) {
  return __builtin_aarch64_lrounduv4sfv4si_us(__a);
}

FUNK(int64x1_t)
vcvta_s64_f64(float64x1_t __a) {
  return (int64x1_t){vcvtad_s64_f64(__a[0])};
}

FUNK(uint64x1_t)
vcvta_u64_f64(float64x1_t __a) {
  return (uint64x1_t){vcvtad_u64_f64(__a[0])};
}

FUNK(int64x2_t)
vcvtaq_s64_f64(float64x2_t __a) {
  return __builtin_aarch64_lroundv2dfv2di(__a);
}

FUNK(uint64x2_t)
vcvtaq_u64_f64(float64x2_t __a) {
  return __builtin_aarch64_lrounduv2dfv2di_us(__a);
}

FUNK(int64_t)
vcvtmd_s64_f64(float64_t __a) {
  return __builtin_llfloor(__a);
}

FUNK(uint64_t)
vcvtmd_u64_f64(float64_t __a) {
  return __builtin_aarch64_lfloorudfdi_us(__a);
}

FUNK(int32_t)
vcvtms_s32_f32(float32_t __a) {
  return __builtin_ifloorf(__a);
}

FUNK(uint32_t)
vcvtms_u32_f32(float32_t __a) {
  return __builtin_aarch64_lfloorusfsi_us(__a);
}

FUNK(int32x2_t)
vcvtm_s32_f32(float32x2_t __a) {
  return __builtin_aarch64_lfloorv2sfv2si(__a);
}

FUNK(uint32x2_t)
vcvtm_u32_f32(float32x2_t __a) {
  return __builtin_aarch64_lflooruv2sfv2si_us(__a);
}

FUNK(int32x4_t)
vcvtmq_s32_f32(float32x4_t __a) {
  return __builtin_aarch64_lfloorv4sfv4si(__a);
}

FUNK(uint32x4_t)
vcvtmq_u32_f32(float32x4_t __a) {
  return __builtin_aarch64_lflooruv4sfv4si_us(__a);
}

FUNK(int64x1_t)
vcvtm_s64_f64(float64x1_t __a) {
  return (int64x1_t){vcvtmd_s64_f64(__a[0])};
}

FUNK(uint64x1_t)
vcvtm_u64_f64(float64x1_t __a) {
  return (uint64x1_t){vcvtmd_u64_f64(__a[0])};
}

FUNK(int64x2_t)
vcvtmq_s64_f64(float64x2_t __a) {
  return __builtin_aarch64_lfloorv2dfv2di(__a);
}

FUNK(uint64x2_t)
vcvtmq_u64_f64(float64x2_t __a) {
  return __builtin_aarch64_lflooruv2dfv2di_us(__a);
}

FUNK(int64_t)
vcvtnd_s64_f64(float64_t __a) {
  return __builtin_aarch64_lfrintndfdi(__a);
}

FUNK(uint64_t)
vcvtnd_u64_f64(float64_t __a) {
  return __builtin_aarch64_lfrintnudfdi_us(__a);
}

FUNK(int32_t)
vcvtns_s32_f32(float32_t __a) {
  return __builtin_aarch64_lfrintnsfsi(__a);
}

FUNK(uint32_t)
vcvtns_u32_f32(float32_t __a) {
  return __builtin_aarch64_lfrintnusfsi_us(__a);
}

FUNK(int32x2_t)
vcvtn_s32_f32(float32x2_t __a) {
  return __builtin_aarch64_lfrintnv2sfv2si(__a);
}

FUNK(uint32x2_t)
vcvtn_u32_f32(float32x2_t __a) {
  return __builtin_aarch64_lfrintnuv2sfv2si_us(__a);
}

FUNK(int32x4_t)
vcvtnq_s32_f32(float32x4_t __a) {
  return __builtin_aarch64_lfrintnv4sfv4si(__a);
}

FUNK(uint32x4_t)
vcvtnq_u32_f32(float32x4_t __a) {
  return __builtin_aarch64_lfrintnuv4sfv4si_us(__a);
}

FUNK(int64x1_t)
vcvtn_s64_f64(float64x1_t __a) {
  return (int64x1_t){vcvtnd_s64_f64(__a[0])};
}

FUNK(uint64x1_t)
vcvtn_u64_f64(float64x1_t __a) {
  return (uint64x1_t){vcvtnd_u64_f64(__a[0])};
}

FUNK(int64x2_t)
vcvtnq_s64_f64(float64x2_t __a) {
  return __builtin_aarch64_lfrintnv2dfv2di(__a);
}

FUNK(uint64x2_t)
vcvtnq_u64_f64(float64x2_t __a) {
  return __builtin_aarch64_lfrintnuv2dfv2di_us(__a);
}

FUNK(int64_t)
vcvtpd_s64_f64(float64_t __a) {
  return __builtin_llceil(__a);
}

FUNK(uint64_t)
vcvtpd_u64_f64(float64_t __a) {
  return __builtin_aarch64_lceiludfdi_us(__a);
}

FUNK(int32_t)
vcvtps_s32_f32(float32_t __a) {
  return __builtin_iceilf(__a);
}

FUNK(uint32_t)
vcvtps_u32_f32(float32_t __a) {
  return __builtin_aarch64_lceilusfsi_us(__a);
}

FUNK(int32x2_t)
vcvtp_s32_f32(float32x2_t __a) {
  return __builtin_aarch64_lceilv2sfv2si(__a);
}

FUNK(uint32x2_t)
vcvtp_u32_f32(float32x2_t __a) {
  return __builtin_aarch64_lceiluv2sfv2si_us(__a);
}

FUNK(int32x4_t)
vcvtpq_s32_f32(float32x4_t __a) {
  return __builtin_aarch64_lceilv4sfv4si(__a);
}

FUNK(uint32x4_t)
vcvtpq_u32_f32(float32x4_t __a) {
  return __builtin_aarch64_lceiluv4sfv4si_us(__a);
}

FUNK(int64x1_t)
vcvtp_s64_f64(float64x1_t __a) {
  return (int64x1_t){vcvtpd_s64_f64(__a[0])};
}

FUNK(uint64x1_t)
vcvtp_u64_f64(float64x1_t __a) {
  return (uint64x1_t){vcvtpd_u64_f64(__a[0])};
}

FUNK(int64x2_t)
vcvtpq_s64_f64(float64x2_t __a) {
  return __builtin_aarch64_lceilv2dfv2di(__a);
}

FUNK(uint64x2_t)
vcvtpq_u64_f64(float64x2_t __a) {
  return __builtin_aarch64_lceiluv2dfv2di_us(__a);
}

FUNK(float16x4_t)
vdup_n_f16(float16_t __a) {
  return (float16x4_t){__a, __a, __a, __a};
}

FUNK(float32x2_t)
vdup_n_f32(float32_t __a) {
  return (float32x2_t){__a, __a};
}

FUNK(float64x1_t)
vdup_n_f64(float64_t __a) {
  return (float64x1_t){__a};
}

FUNK(poly8x8_t)
vdup_n_p8(poly8_t __a) {
  return (poly8x8_t){__a, __a, __a, __a, __a, __a, __a, __a};
}

FUNK(poly16x4_t)
vdup_n_p16(poly16_t __a) {
  return (poly16x4_t){__a, __a, __a, __a};
}

FUNK(poly64x1_t)
vdup_n_p64(poly64_t __a) {
  return (poly64x1_t){__a};
}

FUNK(int8x8_t)
vdup_n_s8(int8_t __a) {
  return (int8x8_t){__a, __a, __a, __a, __a, __a, __a, __a};
}

FUNK(int16x4_t)
vdup_n_s16(int16_t __a) {
  return (int16x4_t){__a, __a, __a, __a};
}

FUNK(int32x2_t)
vdup_n_s32(int32_t __a) {
  return (int32x2_t){__a, __a};
}

FUNK(int64x1_t)
vdup_n_s64(int64_t __a) {
  return (int64x1_t){__a};
}

FUNK(uint8x8_t)
vdup_n_u8(uint8_t __a) {
  return (uint8x8_t){__a, __a, __a, __a, __a, __a, __a, __a};
}

FUNK(uint16x4_t)
vdup_n_u16(uint16_t __a) {
  return (uint16x4_t){__a, __a, __a, __a};
}

FUNK(uint32x2_t)
vdup_n_u32(uint32_t __a) {
  return (uint32x2_t){__a, __a};
}

FUNK(uint64x1_t)
vdup_n_u64(uint64_t __a) {
  return (uint64x1_t){__a};
}

FUNK(float16x8_t)
vdupq_n_f16(float16_t __a) {
  return (float16x8_t){__a, __a, __a, __a, __a, __a, __a, __a};
}

FUNK(float32x4_t)
vdupq_n_f32(float32_t __a) {
  return (float32x4_t){__a, __a, __a, __a};
}

FUNK(float64x2_t)
vdupq_n_f64(float64_t __a) {
  return (float64x2_t){__a, __a};
}

FUNK(poly8x16_t)
vdupq_n_p8(uint32_t __a) {
  return (poly8x16_t){__a, __a, __a, __a, __a, __a, __a, __a,
                      __a, __a, __a, __a, __a, __a, __a, __a};
}

FUNK(poly16x8_t)
vdupq_n_p16(uint32_t __a) {
  return (poly16x8_t){__a, __a, __a, __a, __a, __a, __a, __a};
}

FUNK(poly64x2_t)
vdupq_n_p64(uint64_t __a) {
  return (poly64x2_t){__a, __a};
}

FUNK(int8x16_t)
vdupq_n_s8(int32_t __a) {
  return (int8x16_t){__a, __a, __a, __a, __a, __a, __a, __a,
                     __a, __a, __a, __a, __a, __a, __a, __a};
}

FUNK(int16x8_t)
vdupq_n_s16(int32_t __a) {
  return (int16x8_t){__a, __a, __a, __a, __a, __a, __a, __a};
}

FUNK(int32x4_t)
vdupq_n_s32(int32_t __a) {
  return (int32x4_t){__a, __a, __a, __a};
}

FUNK(int64x2_t)
vdupq_n_s64(int64_t __a) {
  return (int64x2_t){__a, __a};
}

FUNK(uint8x16_t)
vdupq_n_u8(uint32_t __a) {
  return (uint8x16_t){__a, __a, __a, __a, __a, __a, __a, __a,
                      __a, __a, __a, __a, __a, __a, __a, __a};
}

FUNK(uint16x8_t)
vdupq_n_u16(uint32_t __a) {
  return (uint16x8_t){__a, __a, __a, __a, __a, __a, __a, __a};
}

FUNK(uint32x4_t)
vdupq_n_u32(uint32_t __a) {
  return (uint32x4_t){__a, __a, __a, __a};
}

FUNK(uint64x2_t)
vdupq_n_u64(uint64_t __a) {
  return (uint64x2_t){__a, __a};
}

FUNK(float16x4_t)
vdup_lane_f16(float16x4_t __a, const int __b) {
  return __aarch64_vdup_lane_f16(__a, __b);
}

FUNK(float32x2_t)
vdup_lane_f32(float32x2_t __a, const int __b) {
  return __aarch64_vdup_lane_f32(__a, __b);
}

FUNK(float64x1_t)
vdup_lane_f64(float64x1_t __a, const int __b) {
  return __aarch64_vdup_lane_f64(__a, __b);
}

FUNK(poly8x8_t)
vdup_lane_p8(poly8x8_t __a, const int __b) {
  return __aarch64_vdup_lane_p8(__a, __b);
}

FUNK(poly16x4_t)
vdup_lane_p16(poly16x4_t __a, const int __b) {
  return __aarch64_vdup_lane_p16(__a, __b);
}

FUNK(poly64x1_t)
vdup_lane_p64(poly64x1_t __a, const int __b) {
  return __aarch64_vdup_lane_p64(__a, __b);
}

FUNK(int8x8_t)
vdup_lane_s8(int8x8_t __a, const int __b) {
  return __aarch64_vdup_lane_s8(__a, __b);
}

FUNK(int16x4_t)
vdup_lane_s16(int16x4_t __a, const int __b) {
  return __aarch64_vdup_lane_s16(__a, __b);
}

FUNK(int32x2_t)
vdup_lane_s32(int32x2_t __a, const int __b) {
  return __aarch64_vdup_lane_s32(__a, __b);
}

FUNK(int64x1_t)
vdup_lane_s64(int64x1_t __a, const int __b) {
  return __aarch64_vdup_lane_s64(__a, __b);
}

FUNK(uint8x8_t)
vdup_lane_u8(uint8x8_t __a, const int __b) {
  return __aarch64_vdup_lane_u8(__a, __b);
}

FUNK(uint16x4_t)
vdup_lane_u16(uint16x4_t __a, const int __b) {
  return __aarch64_vdup_lane_u16(__a, __b);
}

FUNK(uint32x2_t)
vdup_lane_u32(uint32x2_t __a, const int __b) {
  return __aarch64_vdup_lane_u32(__a, __b);
}

FUNK(uint64x1_t)
vdup_lane_u64(uint64x1_t __a, const int __b) {
  return __aarch64_vdup_lane_u64(__a, __b);
}

FUNK(float16x4_t)
vdup_laneq_f16(float16x8_t __a, const int __b) {
  return __aarch64_vdup_laneq_f16(__a, __b);
}

FUNK(float32x2_t)
vdup_laneq_f32(float32x4_t __a, const int __b) {
  return __aarch64_vdup_laneq_f32(__a, __b);
}

FUNK(float64x1_t)
vdup_laneq_f64(float64x2_t __a, const int __b) {
  return __aarch64_vdup_laneq_f64(__a, __b);
}

FUNK(poly8x8_t)
vdup_laneq_p8(poly8x16_t __a, const int __b) {
  return __aarch64_vdup_laneq_p8(__a, __b);
}

FUNK(poly16x4_t)
vdup_laneq_p16(poly16x8_t __a, const int __b) {
  return __aarch64_vdup_laneq_p16(__a, __b);
}

FUNK(poly64x1_t)
vdup_laneq_p64(poly64x2_t __a, const int __b) {
  return __aarch64_vdup_laneq_p64(__a, __b);
}

FUNK(int8x8_t)
vdup_laneq_s8(int8x16_t __a, const int __b) {
  return __aarch64_vdup_laneq_s8(__a, __b);
}

FUNK(int16x4_t)
vdup_laneq_s16(int16x8_t __a, const int __b) {
  return __aarch64_vdup_laneq_s16(__a, __b);
}

FUNK(int32x2_t)
vdup_laneq_s32(int32x4_t __a, const int __b) {
  return __aarch64_vdup_laneq_s32(__a, __b);
}

FUNK(int64x1_t)
vdup_laneq_s64(int64x2_t __a, const int __b) {
  return __aarch64_vdup_laneq_s64(__a, __b);
}

FUNK(uint8x8_t)
vdup_laneq_u8(uint8x16_t __a, const int __b) {
  return __aarch64_vdup_laneq_u8(__a, __b);
}

FUNK(uint16x4_t)
vdup_laneq_u16(uint16x8_t __a, const int __b) {
  return __aarch64_vdup_laneq_u16(__a, __b);
}

FUNK(uint32x2_t)
vdup_laneq_u32(uint32x4_t __a, const int __b) {
  return __aarch64_vdup_laneq_u32(__a, __b);
}

FUNK(uint64x1_t)
vdup_laneq_u64(uint64x2_t __a, const int __b) {
  return __aarch64_vdup_laneq_u64(__a, __b);
}

FUNK(float16x8_t)
vdupq_lane_f16(float16x4_t __a, const int __b) {
  return __aarch64_vdupq_lane_f16(__a, __b);
}

FUNK(float32x4_t)
vdupq_lane_f32(float32x2_t __a, const int __b) {
  return __aarch64_vdupq_lane_f32(__a, __b);
}

FUNK(float64x2_t)
vdupq_lane_f64(float64x1_t __a, const int __b) {
  return __aarch64_vdupq_lane_f64(__a, __b);
}

FUNK(poly8x16_t)
vdupq_lane_p8(poly8x8_t __a, const int __b) {
  return __aarch64_vdupq_lane_p8(__a, __b);
}

FUNK(poly16x8_t)
vdupq_lane_p16(poly16x4_t __a, const int __b) {
  return __aarch64_vdupq_lane_p16(__a, __b);
}

FUNK(poly64x2_t)
vdupq_lane_p64(poly64x1_t __a, const int __b) {
  return __aarch64_vdupq_lane_p64(__a, __b);
}

FUNK(int8x16_t)
vdupq_lane_s8(int8x8_t __a, const int __b) {
  return __aarch64_vdupq_lane_s8(__a, __b);
}

FUNK(int16x8_t)
vdupq_lane_s16(int16x4_t __a, const int __b) {
  return __aarch64_vdupq_lane_s16(__a, __b);
}

FUNK(int32x4_t)
vdupq_lane_s32(int32x2_t __a, const int __b) {
  return __aarch64_vdupq_lane_s32(__a, __b);
}

FUNK(int64x2_t)
vdupq_lane_s64(int64x1_t __a, const int __b) {
  return __aarch64_vdupq_lane_s64(__a, __b);
}

FUNK(uint8x16_t)
vdupq_lane_u8(uint8x8_t __a, const int __b) {
  return __aarch64_vdupq_lane_u8(__a, __b);
}

FUNK(uint16x8_t)
vdupq_lane_u16(uint16x4_t __a, const int __b) {
  return __aarch64_vdupq_lane_u16(__a, __b);
}

FUNK(uint32x4_t)
vdupq_lane_u32(uint32x2_t __a, const int __b) {
  return __aarch64_vdupq_lane_u32(__a, __b);
}

FUNK(uint64x2_t)
vdupq_lane_u64(uint64x1_t __a, const int __b) {
  return __aarch64_vdupq_lane_u64(__a, __b);
}

FUNK(float16x8_t)
vdupq_laneq_f16(float16x8_t __a, const int __b) {
  return __aarch64_vdupq_laneq_f16(__a, __b);
}

FUNK(float32x4_t)
vdupq_laneq_f32(float32x4_t __a, const int __b) {
  return __aarch64_vdupq_laneq_f32(__a, __b);
}

FUNK(float64x2_t)
vdupq_laneq_f64(float64x2_t __a, const int __b) {
  return __aarch64_vdupq_laneq_f64(__a, __b);
}

FUNK(poly8x16_t)
vdupq_laneq_p8(poly8x16_t __a, const int __b) {
  return __aarch64_vdupq_laneq_p8(__a, __b);
}

FUNK(poly16x8_t)
vdupq_laneq_p16(poly16x8_t __a, const int __b) {
  return __aarch64_vdupq_laneq_p16(__a, __b);
}

FUNK(poly64x2_t)
vdupq_laneq_p64(poly64x2_t __a, const int __b) {
  return __aarch64_vdupq_laneq_p64(__a, __b);
}

FUNK(int8x16_t)
vdupq_laneq_s8(int8x16_t __a, const int __b) {
  return __aarch64_vdupq_laneq_s8(__a, __b);
}

FUNK(int16x8_t)
vdupq_laneq_s16(int16x8_t __a, const int __b) {
  return __aarch64_vdupq_laneq_s16(__a, __b);
}

FUNK(int32x4_t)
vdupq_laneq_s32(int32x4_t __a, const int __b) {
  return __aarch64_vdupq_laneq_s32(__a, __b);
}

FUNK(int64x2_t)
vdupq_laneq_s64(int64x2_t __a, const int __b) {
  return __aarch64_vdupq_laneq_s64(__a, __b);
}

FUNK(uint8x16_t)
vdupq_laneq_u8(uint8x16_t __a, const int __b) {
  return __aarch64_vdupq_laneq_u8(__a, __b);
}

FUNK(uint16x8_t)
vdupq_laneq_u16(uint16x8_t __a, const int __b) {
  return __aarch64_vdupq_laneq_u16(__a, __b);
}

FUNK(uint32x4_t)
vdupq_laneq_u32(uint32x4_t __a, const int __b) {
  return __aarch64_vdupq_laneq_u32(__a, __b);
}

FUNK(uint64x2_t)
vdupq_laneq_u64(uint64x2_t __a, const int __b) {
  return __aarch64_vdupq_laneq_u64(__a, __b);
}

FUNK(poly8_t)
vdupb_lane_p8(poly8x8_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(int8_t)
vdupb_lane_s8(int8x8_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(uint8_t)
vdupb_lane_u8(uint8x8_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(float16_t)
vduph_lane_f16(float16x4_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(poly16_t)
vduph_lane_p16(poly16x4_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(int16_t)
vduph_lane_s16(int16x4_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(uint16_t)
vduph_lane_u16(uint16x4_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(float32_t)
vdups_lane_f32(float32x2_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(int32_t)
vdups_lane_s32(int32x2_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(uint32_t)
vdups_lane_u32(uint32x2_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(float64_t)
vdupd_lane_f64(float64x1_t __a, const int __b) {
  __AARCH64_LANE_CHECK(__a, __b);
  return __a[0];
}

FUNK(int64_t)
vdupd_lane_s64(int64x1_t __a, const int __b) {
  __AARCH64_LANE_CHECK(__a, __b);
  return __a[0];
}

FUNK(uint64_t)
vdupd_lane_u64(uint64x1_t __a, const int __b) {
  __AARCH64_LANE_CHECK(__a, __b);
  return __a[0];
}

FUNK(poly8_t)
vdupb_laneq_p8(poly8x16_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(int8_t)
vdupb_laneq_s8(int8x16_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(uint8_t)
vdupb_laneq_u8(uint8x16_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(float16_t)
vduph_laneq_f16(float16x8_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(poly16_t)
vduph_laneq_p16(poly16x8_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(int16_t)
vduph_laneq_s16(int16x8_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(uint16_t)
vduph_laneq_u16(uint16x8_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(float32_t)
vdups_laneq_f32(float32x4_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(int32_t)
vdups_laneq_s32(int32x4_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(uint32_t)
vdups_laneq_u32(uint32x4_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(float64_t)
vdupd_laneq_f64(float64x2_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(int64_t)
vdupd_laneq_s64(int64x2_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(uint64_t)
vdupd_laneq_u64(uint64x2_t __a, const int __b) {
  return __aarch64_vget_lane_any(__a, __b);
}

FUNK(float16x4_t)
vext_f16(float16x4_t __a, float16x4_t __b, __const int __c) {
  __AARCH64_LANE_CHECK(__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle(__b, __a,
                           (uint16x4_t){4 - __c, 5 - __c, 6 - __c, 7 - __c});
#else
  return __builtin_shuffle(__a, __b,
                           (uint16x4_t){__c, __c + 1, __c + 2, __c + 3});
#endif
}

FUNK(float32x2_t)
vext_f32(float32x2_t __a, float32x2_t __b, __const int __c) {
  __AARCH64_LANE_CHECK(__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle(__b, __a, (uint32x2_t){2 - __c, 3 - __c});
#else
  return __builtin_shuffle(__a, __b, (uint32x2_t){__c, __c + 1});
#endif
}

FUNK(float64x1_t)
vext_f64(float64x1_t __a, float64x1_t __b, __const int __c) {
  __AARCH64_LANE_CHECK(__a, __c);

  return __a;
}
FUNK(poly8x8_t)
vext_p8(poly8x8_t __a, poly8x8_t __b, __const int __c) {
  __AARCH64_LANE_CHECK(__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle(__b, __a,
                           (uint8x8_t){8 - __c, 9 - __c, 10 - __c, 11 - __c,
                                       12 - __c, 13 - __c, 14 - __c, 15 - __c});
#else
  return __builtin_shuffle(__a, __b,
                           (uint8x8_t){__c, __c + 1, __c + 2, __c + 3, __c + 4,
                                       __c + 5, __c + 6, __c + 7});
#endif
}

FUNK(poly16x4_t)
vext_p16(poly16x4_t __a, poly16x4_t __b, __const int __c) {
  __AARCH64_LANE_CHECK(__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle(__b, __a,
                           (uint16x4_t){4 - __c, 5 - __c, 6 - __c, 7 - __c});
#else
  return __builtin_shuffle(__a, __b,
                           (uint16x4_t){__c, __c + 1, __c + 2, __c + 3});
#endif
}

FUNK(poly64x1_t)
vext_p64(poly64x1_t __a, poly64x1_t __b, __const int __c) {
  __AARCH64_LANE_CHECK(__a, __c);

  return __a;
}

FUNK(int8x8_t)
vext_s8(int8x8_t __a, int8x8_t __b, __const int __c) {
  __AARCH64_LANE_CHECK(__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle(__b, __a,
                           (uint8x8_t){8 - __c, 9 - __c, 10 - __c, 11 - __c,
                                       12 - __c, 13 - __c, 14 - __c, 15 - __c});
#else
  return __builtin_shuffle(__a, __b,
                           (uint8x8_t){__c, __c + 1, __c + 2, __c + 3, __c + 4,
                                       __c + 5, __c + 6, __c + 7});
#endif
}

FUNK(int16x4_t)
vext_s16(int16x4_t __a, int16x4_t __b, __const int __c) {
  __AARCH64_LANE_CHECK(__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle(__b, __a,
                           (uint16x4_t){4 - __c, 5 - __c, 6 - __c, 7 - __c});
#else
  return __builtin_shuffle(__a, __b,
                           (uint16x4_t){__c, __c + 1, __c + 2, __c + 3});
#endif
}

FUNK(int32x2_t)
vext_s32(int32x2_t __a, int32x2_t __b, __const int __c) {
  __AARCH64_LANE_CHECK(__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle(__b, __a, (uint32x2_t){2 - __c, 3 - __c});
#else
  return __builtin_shuffle(__a, __b, (uint32x2_t){__c, __c + 1});
#endif
}

FUNK(int64x1_t)
vext_s64(int64x1_t __a, int64x1_t __b, __const int __c) {
  __AARCH64_LANE_CHECK(__a, __c);

  return __a;
}

FUNK(uint8x8_t)
vext_u8(uint8x8_t __a, uint8x8_t __b, __const int __c) {
  __AARCH64_LANE_CHECK(__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle(__b, __a,
                           (uint8x8_t){8 - __c, 9 - __c, 10 - __c, 11 - __c,
                                       12 - __c, 13 - __c, 14 - __c, 15 - __c});
#else
  return __builtin_shuffle(__a, __b,
                           (uint8x8_t){__c, __c + 1, __c + 2, __c + 3, __c + 4,
                                       __c + 5, __c + 6, __c + 7});
#endif
}

FUNK(uint16x4_t)
vext_u16(uint16x4_t __a, uint16x4_t __b, __const int __c) {
  __AARCH64_LANE_CHECK(__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle(__b, __a,
                           (uint16x4_t){4 - __c, 5 - __c, 6 - __c, 7 - __c});
#else
  return __builtin_shuffle(__a, __b,
                           (uint16x4_t){__c, __c + 1, __c + 2, __c + 3});
#endif
}

FUNK(uint32x2_t)
vext_u32(uint32x2_t __a, uint32x2_t __b, __const int __c) {
  __AARCH64_LANE_CHECK(__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle(__b, __a, (uint32x2_t){2 - __c, 3 - __c});
#else
  return __builtin_shuffle(__a, __b, (uint32x2_t){__c, __c + 1});
#endif
}

FUNK(uint64x1_t)
vext_u64(uint64x1_t __a, uint64x1_t __b, __const int __c) {
  __AARCH64_LANE_CHECK(__a, __c);

  return __a;
}

FUNK(float16x8_t)
vextq_f16(float16x8_t __a, float16x8_t __b, __const int __c) {
  __AARCH64_LANE_CHECK(__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle(
      __b, __a,
      (uint16x8_t){8 - __c, 9 - __c, 10 - __c, 11 - __c, 12 - __c, 13 - __c,
                   14 - __c, 15 - __c});
#else
  return __builtin_shuffle(__a, __b,
                           (uint16x8_t){__c, __c + 1, __c + 2, __c + 3, __c + 4,
                                        __c + 5, __c + 6, __c + 7});
#endif
}

FUNK(float32x4_t)
vextq_f32(float32x4_t __a, float32x4_t __b, __const int __c) {
  __AARCH64_LANE_CHECK(__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle(__b, __a,
                           (uint32x4_t){4 - __c, 5 - __c, 6 - __c, 7 - __c});
#else
  return __builtin_shuffle(__a, __b,
                           (uint32x4_t){__c, __c + 1, __c + 2, __c + 3});
#endif
}

FUNK(float64x2_t)
vextq_f64(float64x2_t __a, float64x2_t __b, __const int __c) {
  __AARCH64_LANE_CHECK(__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle(__b, __a, (uint64x2_t){2 - __c, 3 - __c});
#else
  return __builtin_shuffle(__a, __b, (uint64x2_t){__c, __c + 1});
#endif
}

FUNK(poly8x16_t)
vextq_p8(poly8x16_t __a, poly8x16_t __b, __const int __c) {
  __AARCH64_LANE_CHECK(__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle(
      __b, __a,
      (uint8x16_t){16 - __c, 17 - __c, 18 - __c, 19 - __c, 20 - __c, 21 - __c,
                   22 - __c, 23 - __c, 24 - __c, 25 - __c, 26 - __c, 27 - __c,
                   28 - __c, 29 - __c, 30 - __c, 31 - __c});
#else
  return __builtin_shuffle(
      __a, __b,
      (uint8x16_t){__c, __c + 1, __c + 2, __c + 3, __c + 4, __c + 5, __c + 6,
                   __c + 7, __c + 8, __c + 9, __c + 10, __c + 11, __c + 12,
                   __c + 13, __c + 14, __c + 15});
#endif
}

FUNK(poly16x8_t)
vextq_p16(poly16x8_t __a, poly16x8_t __b, __const int __c) {
  __AARCH64_LANE_CHECK(__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle(
      __b, __a,
      (uint16x8_t){8 - __c, 9 - __c, 10 - __c, 11 - __c, 12 - __c, 13 - __c,
                   14 - __c, 15 - __c});
#else
  return __builtin_shuffle(__a, __b,
                           (uint16x8_t){__c, __c + 1, __c + 2, __c + 3, __c + 4,
                                        __c + 5, __c + 6, __c + 7});
#endif
}

FUNK(poly64x2_t)
vextq_p64(poly64x2_t __a, poly64x2_t __b, __const int __c) {
  __AARCH64_LANE_CHECK(__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle(__b, __a, (uint64x2_t){2 - __c, 3 - __c});
#else
  return __builtin_shuffle(__a, __b, (uint64x2_t){__c, __c + 1});
#endif
}

FUNK(int8x16_t)
vextq_s8(int8x16_t __a, int8x16_t __b, __const int __c) {
  __AARCH64_LANE_CHECK(__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle(
      __b, __a,
      (uint8x16_t){16 - __c, 17 - __c, 18 - __c, 19 - __c, 20 - __c, 21 - __c,
                   22 - __c, 23 - __c, 24 - __c, 25 - __c, 26 - __c, 27 - __c,
                   28 - __c, 29 - __c, 30 - __c, 31 - __c});
#else
  return __builtin_shuffle(
      __a, __b,
      (uint8x16_t){__c, __c + 1, __c + 2, __c + 3, __c + 4, __c + 5, __c + 6,
                   __c + 7, __c + 8, __c + 9, __c + 10, __c + 11, __c + 12,
                   __c + 13, __c + 14, __c + 15});
#endif
}

FUNK(int16x8_t)
vextq_s16(int16x8_t __a, int16x8_t __b, __const int __c) {
  __AARCH64_LANE_CHECK(__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle(
      __b, __a,
      (uint16x8_t){8 - __c, 9 - __c, 10 - __c, 11 - __c, 12 - __c, 13 - __c,
                   14 - __c, 15 - __c});
#else
  return __builtin_shuffle(__a, __b,
                           (uint16x8_t){__c, __c + 1, __c + 2, __c + 3, __c + 4,
                                        __c + 5, __c + 6, __c + 7});
#endif
}

FUNK(int32x4_t)
vextq_s32(int32x4_t __a, int32x4_t __b, __const int __c) {
  __AARCH64_LANE_CHECK(__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle(__b, __a,
                           (uint32x4_t){4 - __c, 5 - __c, 6 - __c, 7 - __c});
#else
  return __builtin_shuffle(__a, __b,
                           (uint32x4_t){__c, __c + 1, __c + 2, __c + 3});
#endif
}

FUNK(int64x2_t)
vextq_s64(int64x2_t __a, int64x2_t __b, __const int __c) {
  __AARCH64_LANE_CHECK(__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle(__b, __a, (uint64x2_t){2 - __c, 3 - __c});
#else
  return __builtin_shuffle(__a, __b, (uint64x2_t){__c, __c + 1});
#endif
}

FUNK(uint8x16_t)
vextq_u8(uint8x16_t __a, uint8x16_t __b, __const int __c) {
  __AARCH64_LANE_CHECK(__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle(
      __b, __a,
      (uint8x16_t){16 - __c, 17 - __c, 18 - __c, 19 - __c, 20 - __c, 21 - __c,
                   22 - __c, 23 - __c, 24 - __c, 25 - __c, 26 - __c, 27 - __c,
                   28 - __c, 29 - __c, 30 - __c, 31 - __c});
#else
  return __builtin_shuffle(
      __a, __b,
      (uint8x16_t){__c, __c + 1, __c + 2, __c + 3, __c + 4, __c + 5, __c + 6,
                   __c + 7, __c + 8, __c + 9, __c + 10, __c + 11, __c + 12,
                   __c + 13, __c + 14, __c + 15});
#endif
}

FUNK(uint16x8_t)
vextq_u16(uint16x8_t __a, uint16x8_t __b, __const int __c) {
  __AARCH64_LANE_CHECK(__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle(
      __b, __a,
      (uint16x8_t){8 - __c, 9 - __c, 10 - __c, 11 - __c, 12 - __c, 13 - __c,
                   14 - __c, 15 - __c});
#else
  return __builtin_shuffle(__a, __b,
                           (uint16x8_t){__c, __c + 1, __c + 2, __c + 3, __c + 4,
                                        __c + 5, __c + 6, __c + 7});
#endif
}

FUNK(uint32x4_t)
vextq_u32(uint32x4_t __a, uint32x4_t __b, __const int __c) {
  __AARCH64_LANE_CHECK(__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle(__b, __a,
                           (uint32x4_t){4 - __c, 5 - __c, 6 - __c, 7 - __c});
#else
  return __builtin_shuffle(__a, __b,
                           (uint32x4_t){__c, __c + 1, __c + 2, __c + 3});
#endif
}

FUNK(uint64x2_t)
vextq_u64(uint64x2_t __a, uint64x2_t __b, __const int __c) {
  __AARCH64_LANE_CHECK(__a, __c);
#ifdef __AARCH64EB__
  return __builtin_shuffle(__b, __a, (uint64x2_t){2 - __c, 3 - __c});
#else
  return __builtin_shuffle(__a, __b, (uint64x2_t){__c, __c + 1});
#endif
}

FUNK(float64x1_t)
vfma_f64(float64x1_t __a, float64x1_t __b, float64x1_t __c) {
  return (float64x1_t){__builtin_fma(__b[0], __c[0], __a[0])};
}

FUNK(float32x2_t)
vfma_f32(float32x2_t __a, float32x2_t __b, float32x2_t __c) {
  return __builtin_aarch64_fmav2sf(__b, __c, __a);
}

FUNK(float32x4_t)
vfmaq_f32(float32x4_t __a, float32x4_t __b, float32x4_t __c) {
  return __builtin_aarch64_fmav4sf(__b, __c, __a);
}

FUNK(float64x2_t)
vfmaq_f64(float64x2_t __a, float64x2_t __b, float64x2_t __c) {
  return __builtin_aarch64_fmav2df(__b, __c, __a);
}

FUNK(float32x2_t)
vfma_n_f32(float32x2_t __a, float32x2_t __b, float32_t __c) {
  return __builtin_aarch64_fmav2sf(__b, vdup_n_f32(__c), __a);
}

FUNK(float64x1_t)
vfma_n_f64(float64x1_t __a, float64x1_t __b, float64_t __c) {
  return (float64x1_t){__b[0] * __c + __a[0]};
}

FUNK(float32x4_t)
vfmaq_n_f32(float32x4_t __a, float32x4_t __b, float32_t __c) {
  return __builtin_aarch64_fmav4sf(__b, vdupq_n_f32(__c), __a);
}

FUNK(float64x2_t)
vfmaq_n_f64(float64x2_t __a, float64x2_t __b, float64_t __c) {
  return __builtin_aarch64_fmav2df(__b, vdupq_n_f64(__c), __a);
}

FUNK(float32x2_t)
vfma_lane_f32(float32x2_t __a, float32x2_t __b, float32x2_t __c,
              const int __lane) {
  return __builtin_aarch64_fmav2sf(__b, __aarch64_vdup_lane_f32(__c, __lane),
                                   __a);
}

FUNK(float64x1_t)
vfma_lane_f64(float64x1_t __a, float64x1_t __b, float64x1_t __c,
              const int __lane) {
  return (float64x1_t){__builtin_fma(__b[0], __c[0], __a[0])};
}

FUNK(float64_t)
vfmad_lane_f64(float64_t __a, float64_t __b, float64x1_t __c,
               const int __lane) {
  return __builtin_fma(__b, __c[0], __a);
}

FUNK(float32_t)
vfmas_lane_f32(float32_t __a, float32_t __b, float32x2_t __c,
               const int __lane) {
  return __builtin_fmaf(__b, __aarch64_vget_lane_any(__c, __lane), __a);
}

FUNK(float32x2_t)
vfma_laneq_f32(float32x2_t __a, float32x2_t __b, float32x4_t __c,
               const int __lane) {
  return __builtin_aarch64_fmav2sf(__b, __aarch64_vdup_laneq_f32(__c, __lane),
                                   __a);
}

FUNK(float64x1_t)
vfma_laneq_f64(float64x1_t __a, float64x1_t __b, float64x2_t __c,
               const int __lane) {
  float64_t __c0 = __aarch64_vget_lane_any(__c, __lane);
  return (float64x1_t){__builtin_fma(__b[0], __c0, __a[0])};
}

FUNK(float64_t)
vfmad_laneq_f64(float64_t __a, float64_t __b, float64x2_t __c,
                const int __lane) {
  return __builtin_fma(__b, __aarch64_vget_lane_any(__c, __lane), __a);
}

FUNK(float32_t)
vfmas_laneq_f32(float32_t __a, float32_t __b, float32x4_t __c,
                const int __lane) {
  return __builtin_fmaf(__b, __aarch64_vget_lane_any(__c, __lane), __a);
}

FUNK(float32x4_t)
vfmaq_lane_f32(float32x4_t __a, float32x4_t __b, float32x2_t __c,
               const int __lane) {
  return __builtin_aarch64_fmav4sf(__b, __aarch64_vdupq_lane_f32(__c, __lane),
                                   __a);
}

FUNK(float64x2_t)
vfmaq_lane_f64(float64x2_t __a, float64x2_t __b, float64x1_t __c,
               const int __lane) {
  return __builtin_aarch64_fmav2df(__b, vdupq_n_f64(__c[0]), __a);
}

FUNK(float32x4_t)
vfmaq_laneq_f32(float32x4_t __a, float32x4_t __b, float32x4_t __c,
                const int __lane) {
  return __builtin_aarch64_fmav4sf(__b, __aarch64_vdupq_laneq_f32(__c, __lane),
                                   __a);
}

FUNK(float64x2_t)
vfmaq_laneq_f64(float64x2_t __a, float64x2_t __b, float64x2_t __c,
                const int __lane) {
  return __builtin_aarch64_fmav2df(__b, __aarch64_vdupq_laneq_f64(__c, __lane),
                                   __a);
}

FUNK(float64x1_t)
vfms_f64(float64x1_t __a, float64x1_t __b, float64x1_t __c) {
  return (float64x1_t){__builtin_fma(-__b[0], __c[0], __a[0])};
}

FUNK(float32x2_t)
vfms_f32(float32x2_t __a, float32x2_t __b, float32x2_t __c) {
  return __builtin_aarch64_fmav2sf(-__b, __c, __a);
}

FUNK(float32x4_t)
vfmsq_f32(float32x4_t __a, float32x4_t __b, float32x4_t __c) {
  return __builtin_aarch64_fmav4sf(-__b, __c, __a);
}

FUNK(float64x2_t)
vfmsq_f64(float64x2_t __a, float64x2_t __b, float64x2_t __c) {
  return __builtin_aarch64_fmav2df(-__b, __c, __a);
}

FUNK(float32x2_t)
vfms_n_f32(float32x2_t __a, float32x2_t __b, float32_t __c) {
  return __builtin_aarch64_fmav2sf(-__b, vdup_n_f32(__c), __a);
}

FUNK(float64x1_t)
vfms_n_f64(float64x1_t __a, float64x1_t __b, float64_t __c) {
  return (float64x1_t){-__b[0] * __c + __a[0]};
}

FUNK(float32x4_t)
vfmsq_n_f32(float32x4_t __a, float32x4_t __b, float32_t __c) {
  return __builtin_aarch64_fmav4sf(-__b, vdupq_n_f32(__c), __a);
}

FUNK(float64x2_t)
vfmsq_n_f64(float64x2_t __a, float64x2_t __b, float64_t __c) {
  return __builtin_aarch64_fmav2df(-__b, vdupq_n_f64(__c), __a);
}

FUNK(float32x2_t)
vfms_lane_f32(float32x2_t __a, float32x2_t __b, float32x2_t __c,
              const int __lane) {
  return __builtin_aarch64_fmav2sf(-__b, __aarch64_vdup_lane_f32(__c, __lane),
                                   __a);
}

FUNK(float64x1_t)
vfms_lane_f64(float64x1_t __a, float64x1_t __b, float64x1_t __c,
              const int __lane) {
  return (float64x1_t){__builtin_fma(-__b[0], __c[0], __a[0])};
}

FUNK(float64_t)
vfmsd_lane_f64(float64_t __a, float64_t __b, float64x1_t __c,
               const int __lane) {
  return __builtin_fma(-__b, __c[0], __a);
}

FUNK(float32_t)
vfmss_lane_f32(float32_t __a, float32_t __b, float32x2_t __c,
               const int __lane) {
  return __builtin_fmaf(-__b, __aarch64_vget_lane_any(__c, __lane), __a);
}

FUNK(float32x2_t)
vfms_laneq_f32(float32x2_t __a, float32x2_t __b, float32x4_t __c,
               const int __lane) {
  return __builtin_aarch64_fmav2sf(-__b, __aarch64_vdup_laneq_f32(__c, __lane),
                                   __a);
}

FUNK(float64x1_t)
vfms_laneq_f64(float64x1_t __a, float64x1_t __b, float64x2_t __c,
               const int __lane) {
  float64_t __c0 = __aarch64_vget_lane_any(__c, __lane);
  return (float64x1_t){__builtin_fma(-__b[0], __c0, __a[0])};
}

FUNK(float64_t)
vfmsd_laneq_f64(float64_t __a, float64_t __b, float64x2_t __c,
                const int __lane) {
  return __builtin_fma(-__b, __aarch64_vget_lane_any(__c, __lane), __a);
}

FUNK(float32_t)
vfmss_laneq_f32(float32_t __a, float32_t __b, float32x4_t __c,
                const int __lane) {
  return __builtin_fmaf(-__b, __aarch64_vget_lane_any(__c, __lane), __a);
}

FUNK(float32x4_t)
vfmsq_lane_f32(float32x4_t __a, float32x4_t __b, float32x2_t __c,
               const int __lane) {
  return __builtin_aarch64_fmav4sf(-__b, __aarch64_vdupq_lane_f32(__c, __lane),
                                   __a);
}

FUNK(float64x2_t)
vfmsq_lane_f64(float64x2_t __a, float64x2_t __b, float64x1_t __c,
               const int __lane) {
  return __builtin_aarch64_fmav2df(-__b, vdupq_n_f64(__c[0]), __a);
}

FUNK(float32x4_t)
vfmsq_laneq_f32(float32x4_t __a, float32x4_t __b, float32x4_t __c,
                const int __lane) {
  return __builtin_aarch64_fmav4sf(-__b, __aarch64_vdupq_laneq_f32(__c, __lane),
                                   __a);
}

FUNK(float64x2_t)
vfmsq_laneq_f64(float64x2_t __a, float64x2_t __b, float64x2_t __c,
                const int __lane) {
  return __builtin_aarch64_fmav2df(-__b, __aarch64_vdupq_laneq_f64(__c, __lane),
                                   __a);
}

FUNK(float16x4_t)
vld1_f16(const float16_t *__a) {
  return __builtin_aarch64_ld1v4hf(__a);
}

FUNK(float32x2_t)
vld1_f32(const float32_t *a) {
  return __builtin_aarch64_ld1v2sf((const __builtin_aarch64_simd_sf *)a);
}

FUNK(float64x1_t)
vld1_f64(const float64_t *a) {
  return (float64x1_t){*a};
}

FUNK(poly8x8_t)
vld1_p8(const poly8_t *a) {
  return (poly8x8_t)__builtin_aarch64_ld1v8qi(
      (const __builtin_aarch64_simd_qi *)a);
}

FUNK(poly16x4_t)
vld1_p16(const poly16_t *a) {
  return (poly16x4_t)__builtin_aarch64_ld1v4hi(
      (const __builtin_aarch64_simd_hi *)a);
}

FUNK(poly64x1_t)
vld1_p64(const poly64_t *a) {
  return (poly64x1_t){*a};
}

FUNK(int8x8_t)
vld1_s8(const int8_t *a) {
  return __builtin_aarch64_ld1v8qi((const __builtin_aarch64_simd_qi *)a);
}

FUNK(int16x4_t)
vld1_s16(const int16_t *a) {
  return __builtin_aarch64_ld1v4hi((const __builtin_aarch64_simd_hi *)a);
}

FUNK(int32x2_t)
vld1_s32(const int32_t *a) {
  return __builtin_aarch64_ld1v2si((const __builtin_aarch64_simd_si *)a);
}

FUNK(int64x1_t)
vld1_s64(const int64_t *a) {
  return (int64x1_t){*a};
}

FUNK(uint8x8_t)
vld1_u8(const uint8_t *a) {
  return (uint8x8_t)__builtin_aarch64_ld1v8qi(
      (const __builtin_aarch64_simd_qi *)a);
}

FUNK(uint16x4_t)
vld1_u16(const uint16_t *a) {
  return (uint16x4_t)__builtin_aarch64_ld1v4hi(
      (const __builtin_aarch64_simd_hi *)a);
}

FUNK(uint32x2_t)
vld1_u32(const uint32_t *a) {
  return (uint32x2_t)__builtin_aarch64_ld1v2si(
      (const __builtin_aarch64_simd_si *)a);
}

FUNK(uint64x1_t)
vld1_u64(const uint64_t *a) {
  return (uint64x1_t){*a};
}

FUNK(uint8x8x3_t)
vld1_u8_x3(const uint8_t *__a) {
  uint8x8x3_t __i;
  __builtin_aarch64_simd_ci __o;
  __o = (__builtin_aarch64_simd_ci)__builtin_aarch64_ld1x3v8qi(
      (const __builtin_aarch64_simd_qi *)__a);
  __i.val[0] = (uint8x8_t)__builtin_aarch64_get_dregciv8qi(__o, 0);
  __i.val[1] = (uint8x8_t)__builtin_aarch64_get_dregciv8qi(__o, 1);
  __i.val[2] = (uint8x8_t)__builtin_aarch64_get_dregciv8qi(__o, 2);
  return __i;
}

FUNK(int8x8x3_t)
vld1_s8_x3(const uint8_t *__a) {
  int8x8x3_t __i;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld1x3v8qi((const __builtin_aarch64_simd_qi *)__a);
  __i.val[0] = (int8x8_t)__builtin_aarch64_get_dregciv8qi(__o, 0);
  __i.val[1] = (int8x8_t)__builtin_aarch64_get_dregciv8qi(__o, 1);
  __i.val[2] = (int8x8_t)__builtin_aarch64_get_dregciv8qi(__o, 2);
  return __i;
}

FUNK(uint16x4x3_t)
vld1_u16_x3(const uint16_t *__a) {
  uint16x4x3_t __i;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld1x3v4hi((const __builtin_aarch64_simd_hi *)__a);
  __i.val[0] = (uint16x4_t)__builtin_aarch64_get_dregciv4hi(__o, 0);
  __i.val[1] = (uint16x4_t)__builtin_aarch64_get_dregciv4hi(__o, 1);
  __i.val[2] = (uint16x4_t)__builtin_aarch64_get_dregciv4hi(__o, 2);
  return __i;
}

FUNK(int16x4x3_t)
vld1_s16_x3(const int16_t *__a) {
  int16x4x3_t __i;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld1x3v4hi((const __builtin_aarch64_simd_hi *)__a);
  __i.val[0] = (int16x4_t)__builtin_aarch64_get_dregciv4hi(__o, 0);
  __i.val[1] = (int16x4_t)__builtin_aarch64_get_dregciv4hi(__o, 1);
  __i.val[2] = (int16x4_t)__builtin_aarch64_get_dregciv4hi(__o, 2);
  return __i;
}

FUNK(uint32x2x3_t)
vld1_u32_x3(const uint32_t *__a) {
  uint32x2x3_t __i;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld1x3v2si((const __builtin_aarch64_simd_si *)__a);
  __i.val[0] = (uint32x2_t)__builtin_aarch64_get_dregciv2si(__o, 0);
  __i.val[1] = (uint32x2_t)__builtin_aarch64_get_dregciv2si(__o, 1);
  __i.val[2] = (uint32x2_t)__builtin_aarch64_get_dregciv2si(__o, 2);
  return __i;
}

FUNK(int32x2x3_t)
vld1_s32_x3(const uint32_t *__a) {
  int32x2x3_t __i;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld1x3v2si((const __builtin_aarch64_simd_si *)__a);
  __i.val[0] = (int32x2_t)__builtin_aarch64_get_dregciv2si(__o, 0);
  __i.val[1] = (int32x2_t)__builtin_aarch64_get_dregciv2si(__o, 1);
  __i.val[2] = (int32x2_t)__builtin_aarch64_get_dregciv2si(__o, 2);
  return __i;
}

FUNK(uint64x1x3_t)
vld1_u64_x3(const uint64_t *__a) {
  uint64x1x3_t __i;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld1x3di((const __builtin_aarch64_simd_di *)__a);
  __i.val[0] = (uint64x1_t)__builtin_aarch64_get_dregcidi(__o, 0);
  __i.val[1] = (uint64x1_t)__builtin_aarch64_get_dregcidi(__o, 1);
  __i.val[2] = (uint64x1_t)__builtin_aarch64_get_dregcidi(__o, 2);
  return __i;
}

FUNK(int64x1x3_t)
vld1_s64_x3(const int64_t *__a) {
  int64x1x3_t __i;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld1x3di((const __builtin_aarch64_simd_di *)__a);
  __i.val[0] = (int64x1_t)__builtin_aarch64_get_dregcidi(__o, 0);
  __i.val[1] = (int64x1_t)__builtin_aarch64_get_dregcidi(__o, 1);
  __i.val[2] = (int64x1_t)__builtin_aarch64_get_dregcidi(__o, 2);

  return __i;
}

FUNK(float16x4x3_t)
vld1_f16_x3(const float16_t *__a) {
  float16x4x3_t __i;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld1x3v4hf((const __builtin_aarch64_simd_hf *)__a);
  __i.val[0] = (float16x4_t)__builtin_aarch64_get_dregciv4hf(__o, 0);
  __i.val[1] = (float16x4_t)__builtin_aarch64_get_dregciv4hf(__o, 1);
  __i.val[2] = (float16x4_t)__builtin_aarch64_get_dregciv4hf(__o, 2);
  return __i;
}

FUNK(float32x2x3_t)
vld1_f32_x3(const float32_t *__a) {
  float32x2x3_t __i;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld1x3v2sf((const __builtin_aarch64_simd_sf *)__a);
  __i.val[0] = (float32x2_t)__builtin_aarch64_get_dregciv2sf(__o, 0);
  __i.val[1] = (float32x2_t)__builtin_aarch64_get_dregciv2sf(__o, 1);
  __i.val[2] = (float32x2_t)__builtin_aarch64_get_dregciv2sf(__o, 2);
  return __i;
}

FUNK(float64x1x3_t)
vld1_f64_x3(const float64_t *__a) {
  float64x1x3_t __i;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld1x3df((const __builtin_aarch64_simd_df *)__a);
  __i.val[0] = (float64x1_t)__builtin_aarch64_get_dregcidi(__o, 0);
  __i.val[1] = (float64x1_t)__builtin_aarch64_get_dregcidi(__o, 1);
  __i.val[2] = (float64x1_t)__builtin_aarch64_get_dregcidi(__o, 2);
  return __i;
}

FUNK(poly8x8x3_t)
vld1_p8_x3(const poly8_t *__a) {
  poly8x8x3_t __i;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld1x3v8qi((const __builtin_aarch64_simd_qi *)__a);
  __i.val[0] = (poly8x8_t)__builtin_aarch64_get_dregciv8qi(__o, 0);
  __i.val[1] = (poly8x8_t)__builtin_aarch64_get_dregciv8qi(__o, 1);
  __i.val[2] = (poly8x8_t)__builtin_aarch64_get_dregciv8qi(__o, 2);
  return __i;
}

FUNK(poly16x4x3_t)
vld1_p16_x3(const poly16_t *__a) {
  poly16x4x3_t __i;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld1x3v4hi((const __builtin_aarch64_simd_hi *)__a);
  __i.val[0] = (poly16x4_t)__builtin_aarch64_get_dregciv4hi(__o, 0);
  __i.val[1] = (poly16x4_t)__builtin_aarch64_get_dregciv4hi(__o, 1);
  __i.val[2] = (poly16x4_t)__builtin_aarch64_get_dregciv4hi(__o, 2);
  return __i;
}

FUNK(poly64x1x3_t)
vld1_p64_x3(const poly64_t *__a) {
  poly64x1x3_t __i;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld1x3di((const __builtin_aarch64_simd_di *)__a);
  __i.val[0] = (poly64x1_t)__builtin_aarch64_get_dregcidi(__o, 0);
  __i.val[1] = (poly64x1_t)__builtin_aarch64_get_dregcidi(__o, 1);
  __i.val[2] = (poly64x1_t)__builtin_aarch64_get_dregcidi(__o, 2);

  return __i;
}

FUNK(uint8x16x3_t)
vld1q_u8_x3(const uint8_t *__a) {
  uint8x16x3_t __i;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld1x3v16qi((const __builtin_aarch64_simd_qi *)__a);
  __i.val[0] = (uint8x16_t)__builtin_aarch64_get_qregciv16qi(__o, 0);
  __i.val[1] = (uint8x16_t)__builtin_aarch64_get_qregciv16qi(__o, 1);
  __i.val[2] = (uint8x16_t)__builtin_aarch64_get_qregciv16qi(__o, 2);
  return __i;
}

FUNK(int8x16x3_t)
vld1q_s8_x3(const int8_t *__a) {
  int8x16x3_t __i;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld1x3v16qi((const __builtin_aarch64_simd_qi *)__a);
  __i.val[0] = (int8x16_t)__builtin_aarch64_get_qregciv16qi(__o, 0);
  __i.val[1] = (int8x16_t)__builtin_aarch64_get_qregciv16qi(__o, 1);
  __i.val[2] = (int8x16_t)__builtin_aarch64_get_qregciv16qi(__o, 2);
  return __i;
}

FUNK(uint16x8x3_t)
vld1q_u16_x3(const uint16_t *__a) {
  uint16x8x3_t __i;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld1x3v8hi((const __builtin_aarch64_simd_hi *)__a);
  __i.val[0] = (uint16x8_t)__builtin_aarch64_get_qregciv8hi(__o, 0);
  __i.val[1] = (uint16x8_t)__builtin_aarch64_get_qregciv8hi(__o, 1);
  __i.val[2] = (uint16x8_t)__builtin_aarch64_get_qregciv8hi(__o, 2);
  return __i;
}

FUNK(int16x8x3_t)
vld1q_s16_x3(const int16_t *__a) {
  int16x8x3_t __i;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld1x3v8hi((const __builtin_aarch64_simd_hi *)__a);
  __i.val[0] = (int16x8_t)__builtin_aarch64_get_qregciv8hi(__o, 0);
  __i.val[1] = (int16x8_t)__builtin_aarch64_get_qregciv8hi(__o, 1);
  __i.val[2] = (int16x8_t)__builtin_aarch64_get_qregciv8hi(__o, 2);
  return __i;
}

FUNK(uint32x4x3_t)
vld1q_u32_x3(const uint32_t *__a) {
  uint32x4x3_t __i;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld1x3v4si((const __builtin_aarch64_simd_si *)__a);
  __i.val[0] = (uint32x4_t)__builtin_aarch64_get_qregciv4si(__o, 0);
  __i.val[1] = (uint32x4_t)__builtin_aarch64_get_qregciv4si(__o, 1);
  __i.val[2] = (uint32x4_t)__builtin_aarch64_get_qregciv4si(__o, 2);
  return __i;
}

FUNK(int32x4x3_t)
vld1q_s32_x3(const int32_t *__a) {
  int32x4x3_t __i;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld1x3v4si((const __builtin_aarch64_simd_si *)__a);
  __i.val[0] = (int32x4_t)__builtin_aarch64_get_qregciv4si(__o, 0);
  __i.val[1] = (int32x4_t)__builtin_aarch64_get_qregciv4si(__o, 1);
  __i.val[2] = (int32x4_t)__builtin_aarch64_get_qregciv4si(__o, 2);
  return __i;
}

FUNK(uint64x2x3_t)
vld1q_u64_x3(const uint64_t *__a) {
  uint64x2x3_t __i;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld1x3v2di((const __builtin_aarch64_simd_di *)__a);
  __i.val[0] = (uint64x2_t)__builtin_aarch64_get_qregciv2di(__o, 0);
  __i.val[1] = (uint64x2_t)__builtin_aarch64_get_qregciv2di(__o, 1);
  __i.val[2] = (uint64x2_t)__builtin_aarch64_get_qregciv2di(__o, 2);
  return __i;
}

FUNK(int64x2x3_t)
vld1q_s64_x3(const int64_t *__a) {
  int64x2x3_t __i;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld1x3v2di((const __builtin_aarch64_simd_di *)__a);
  __i.val[0] = (int64x2_t)__builtin_aarch64_get_qregciv2di(__o, 0);
  __i.val[1] = (int64x2_t)__builtin_aarch64_get_qregciv2di(__o, 1);
  __i.val[2] = (int64x2_t)__builtin_aarch64_get_qregciv2di(__o, 2);
  return __i;
}

FUNK(float16x8x3_t)
vld1q_f16_x3(const float16_t *__a) {
  float16x8x3_t __i;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld1x3v8hf((const __builtin_aarch64_simd_hf *)__a);
  __i.val[0] = (float16x8_t)__builtin_aarch64_get_qregciv8hf(__o, 0);
  __i.val[1] = (float16x8_t)__builtin_aarch64_get_qregciv8hf(__o, 1);
  __i.val[2] = (float16x8_t)__builtin_aarch64_get_qregciv8hf(__o, 2);
  return __i;
}

FUNK(float32x4x3_t)
vld1q_f32_x3(const float32_t *__a) {
  float32x4x3_t __i;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld1x3v4sf((const __builtin_aarch64_simd_sf *)__a);
  __i.val[0] = (float32x4_t)__builtin_aarch64_get_qregciv4sf(__o, 0);
  __i.val[1] = (float32x4_t)__builtin_aarch64_get_qregciv4sf(__o, 1);
  __i.val[2] = (float32x4_t)__builtin_aarch64_get_qregciv4sf(__o, 2);
  return __i;
}

FUNK(float64x2x3_t)
vld1q_f64_x3(const float64_t *__a) {
  float64x2x3_t __i;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld1x3v2df((const __builtin_aarch64_simd_df *)__a);
  __i.val[0] = (float64x2_t)__builtin_aarch64_get_qregciv2df(__o, 0);
  __i.val[1] = (float64x2_t)__builtin_aarch64_get_qregciv2df(__o, 1);
  __i.val[2] = (float64x2_t)__builtin_aarch64_get_qregciv2df(__o, 2);
  return __i;
}

FUNK(poly8x16x3_t)
vld1q_p8_x3(const poly8_t *__a) {
  poly8x16x3_t __i;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld1x3v16qi((const __builtin_aarch64_simd_qi *)__a);
  __i.val[0] = (poly8x16_t)__builtin_aarch64_get_qregciv16qi(__o, 0);
  __i.val[1] = (poly8x16_t)__builtin_aarch64_get_qregciv16qi(__o, 1);
  __i.val[2] = (poly8x16_t)__builtin_aarch64_get_qregciv16qi(__o, 2);
  return __i;
}

FUNK(poly16x8x3_t)
vld1q_p16_x3(const poly16_t *__a) {
  poly16x8x3_t __i;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld1x3v8hi((const __builtin_aarch64_simd_hi *)__a);
  __i.val[0] = (poly16x8_t)__builtin_aarch64_get_qregciv8hi(__o, 0);
  __i.val[1] = (poly16x8_t)__builtin_aarch64_get_qregciv8hi(__o, 1);
  __i.val[2] = (poly16x8_t)__builtin_aarch64_get_qregciv8hi(__o, 2);
  return __i;
}

FUNK(poly64x2x3_t)
vld1q_p64_x3(const poly64_t *__a) {
  poly64x2x3_t __i;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld1x3v2di((const __builtin_aarch64_simd_di *)__a);
  __i.val[0] = (poly64x2_t)__builtin_aarch64_get_qregciv2di(__o, 0);
  __i.val[1] = (poly64x2_t)__builtin_aarch64_get_qregciv2di(__o, 1);
  __i.val[2] = (poly64x2_t)__builtin_aarch64_get_qregciv2di(__o, 2);
  return __i;
}

FUNK(float16x8_t)
vld1q_f16(const float16_t *__a) {
  return __builtin_aarch64_ld1v8hf(__a);
}

FUNK(float32x4_t)
vld1q_f32(const float32_t *a) {
  return __builtin_aarch64_ld1v4sf((const __builtin_aarch64_simd_sf *)a);
}

FUNK(float64x2_t)
vld1q_f64(const float64_t *a) {
  return __builtin_aarch64_ld1v2df((const __builtin_aarch64_simd_df *)a);
}

FUNK(poly8x16_t)
vld1q_p8(const poly8_t *a) {
  return (poly8x16_t)__builtin_aarch64_ld1v16qi(
      (const __builtin_aarch64_simd_qi *)a);
}

FUNK(poly16x8_t)
vld1q_p16(const poly16_t *a) {
  return (poly16x8_t)__builtin_aarch64_ld1v8hi(
      (const __builtin_aarch64_simd_hi *)a);
}

FUNK(poly64x2_t)
vld1q_p64(const poly64_t *a) {
  return (poly64x2_t)__builtin_aarch64_ld1v2di(
      (const __builtin_aarch64_simd_di *)a);
}

FUNK(int8x16_t)
vld1q_s8(const int8_t *a) {
  return __builtin_aarch64_ld1v16qi((const __builtin_aarch64_simd_qi *)a);
}

FUNK(int16x8_t)
vld1q_s16(const int16_t *a) {
  return __builtin_aarch64_ld1v8hi((const __builtin_aarch64_simd_hi *)a);
}

FUNK(int32x4_t)
vld1q_s32(const int32_t *a) {
  return __builtin_aarch64_ld1v4si((const __builtin_aarch64_simd_si *)a);
}

FUNK(int64x2_t)
vld1q_s64(const int64_t *a) {
  return __builtin_aarch64_ld1v2di((const __builtin_aarch64_simd_di *)a);
}

FUNK(uint8x16_t)
vld1q_u8(const uint8_t *a) {
  return (uint8x16_t)__builtin_aarch64_ld1v16qi(
      (const __builtin_aarch64_simd_qi *)a);
}

FUNK(uint8x8x2_t)
vld1_u8_x2(const uint8_t *__a) {
  uint8x8x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld1x2v8qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (uint8x8_t)__builtin_aarch64_get_dregoiv8qi(__o, 0);
  ret.val[1] = (uint8x8_t)__builtin_aarch64_get_dregoiv8qi(__o, 1);
  return ret;
}

FUNK(int8x8x2_t)
vld1_s8_x2(const int8_t *__a) {
  int8x8x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld1x2v8qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (int8x8_t)__builtin_aarch64_get_dregoiv8qi(__o, 0);
  ret.val[1] = (int8x8_t)__builtin_aarch64_get_dregoiv8qi(__o, 1);
  return ret;
}

FUNK(uint16x4x2_t)
vld1_u16_x2(const uint16_t *__a) {
  uint16x4x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld1x2v4hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (uint16x4_t)__builtin_aarch64_get_dregoiv4hi(__o, 0);
  ret.val[1] = (uint16x4_t)__builtin_aarch64_get_dregoiv4hi(__o, 1);
  return ret;
}

FUNK(int16x4x2_t)
vld1_s16_x2(const int16_t *__a) {
  int16x4x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld1x2v4hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (int16x4_t)__builtin_aarch64_get_dregoiv4hi(__o, 0);
  ret.val[1] = (int16x4_t)__builtin_aarch64_get_dregoiv4hi(__o, 1);
  return ret;
}

FUNK(uint32x2x2_t)
vld1_u32_x2(const uint32_t *__a) {
  uint32x2x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld1x2v2si((const __builtin_aarch64_simd_si *)__a);
  ret.val[0] = (uint32x2_t)__builtin_aarch64_get_dregoiv2si(__o, 0);
  ret.val[1] = (uint32x2_t)__builtin_aarch64_get_dregoiv2si(__o, 1);
  return ret;
}

FUNK(int32x2x2_t)
vld1_s32_x2(const int32_t *__a) {
  int32x2x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld1x2v2si((const __builtin_aarch64_simd_si *)__a);
  ret.val[0] = (int32x2_t)__builtin_aarch64_get_dregoiv2si(__o, 0);
  ret.val[1] = (int32x2_t)__builtin_aarch64_get_dregoiv2si(__o, 1);
  return ret;
}

FUNK(uint64x1x2_t)
vld1_u64_x2(const uint64_t *__a) {
  uint64x1x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld1x2di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (uint64x1_t)__builtin_aarch64_get_dregoidi(__o, 0);
  ret.val[1] = (uint64x1_t)__builtin_aarch64_get_dregoidi(__o, 1);
  return ret;
}

FUNK(int64x1x2_t)
vld1_s64_x2(const int64_t *__a) {
  int64x1x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld1x2di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (int64x1_t)__builtin_aarch64_get_dregoidi(__o, 0);
  ret.val[1] = (int64x1_t)__builtin_aarch64_get_dregoidi(__o, 1);
  return ret;
}

FUNK(float16x4x2_t)
vld1_f16_x2(const float16_t *__a) {
  float16x4x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld1x2v4hf((const __builtin_aarch64_simd_hf *)__a);
  ret.val[0] = (float16x4_t)__builtin_aarch64_get_dregoiv4hf(__o, 0);
  ret.val[1] = (float16x4_t)__builtin_aarch64_get_dregoiv4hf(__o, 1);
  return ret;
}

FUNK(float32x2x2_t)
vld1_f32_x2(const float32_t *__a) {
  float32x2x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld1x2v2sf((const __builtin_aarch64_simd_sf *)__a);
  ret.val[0] = (float32x2_t)__builtin_aarch64_get_dregoiv2sf(__o, 0);
  ret.val[1] = (float32x2_t)__builtin_aarch64_get_dregoiv2sf(__o, 1);
  return ret;
}

FUNK(float64x1x2_t)
vld1_f64_x2(const float64_t *__a) {
  float64x1x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld1x2df((const __builtin_aarch64_simd_df *)__a);
  ret.val[0] = (float64x1_t){__builtin_aarch64_get_dregoidf(__o, 0)};
  ret.val[1] = (float64x1_t){__builtin_aarch64_get_dregoidf(__o, 1)};
  return ret;
}

FUNK(poly8x8x2_t)
vld1_p8_x2(const poly8_t *__a) {
  poly8x8x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld1x2v8qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (poly8x8_t)__builtin_aarch64_get_dregoiv8qi(__o, 0);
  ret.val[1] = (poly8x8_t)__builtin_aarch64_get_dregoiv8qi(__o, 1);
  return ret;
}

FUNK(poly16x4x2_t)
vld1_p16_x2(const poly16_t *__a) {
  poly16x4x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld1x2v4hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (poly16x4_t)__builtin_aarch64_get_dregoiv4hi(__o, 0);
  ret.val[1] = (poly16x4_t)__builtin_aarch64_get_dregoiv4hi(__o, 1);
  return ret;
}

FUNK(poly64x1x2_t)
vld1_p64_x2(const poly64_t *__a) {
  poly64x1x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld1x2di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (poly64x1_t)__builtin_aarch64_get_dregoidi(__o, 0);
  ret.val[1] = (poly64x1_t)__builtin_aarch64_get_dregoidi(__o, 1);
  return ret;
}

FUNK(uint8x16x2_t)
vld1q_u8_x2(const uint8_t *__a) {
  uint8x16x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld1x2v16qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (uint8x16_t)__builtin_aarch64_get_qregoiv16qi(__o, 0);
  ret.val[1] = (uint8x16_t)__builtin_aarch64_get_qregoiv16qi(__o, 1);
  return ret;
}

FUNK(int8x16x2_t)
vld1q_s8_x2(const int8_t *__a) {
  int8x16x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld1x2v16qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (int8x16_t)__builtin_aarch64_get_qregoiv16qi(__o, 0);
  ret.val[1] = (int8x16_t)__builtin_aarch64_get_qregoiv16qi(__o, 1);
  return ret;
}

FUNK(uint16x8x2_t)
vld1q_u16_x2(const uint16_t *__a) {
  uint16x8x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld1x2v8hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (uint16x8_t)__builtin_aarch64_get_qregoiv8hi(__o, 0);
  ret.val[1] = (uint16x8_t)__builtin_aarch64_get_qregoiv8hi(__o, 1);
  return ret;
}

FUNK(int16x8x2_t)
vld1q_s16_x2(const int16_t *__a) {
  int16x8x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld1x2v8hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (int16x8_t)__builtin_aarch64_get_qregoiv8hi(__o, 0);
  ret.val[1] = (int16x8_t)__builtin_aarch64_get_qregoiv8hi(__o, 1);
  return ret;
}

FUNK(uint32x4x2_t)
vld1q_u32_x2(const uint32_t *__a) {
  uint32x4x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld1x2v4si((const __builtin_aarch64_simd_si *)__a);
  ret.val[0] = (uint32x4_t)__builtin_aarch64_get_qregoiv4si(__o, 0);
  ret.val[1] = (uint32x4_t)__builtin_aarch64_get_qregoiv4si(__o, 1);
  return ret;
}

FUNK(int32x4x2_t)
vld1q_s32_x2(const int32_t *__a) {
  int32x4x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld1x2v4si((const __builtin_aarch64_simd_si *)__a);
  ret.val[0] = (int32x4_t)__builtin_aarch64_get_qregoiv4si(__o, 0);
  ret.val[1] = (int32x4_t)__builtin_aarch64_get_qregoiv4si(__o, 1);
  return ret;
}

FUNK(uint64x2x2_t)
vld1q_u64_x2(const uint64_t *__a) {
  uint64x2x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld1x2v2di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (uint64x2_t)__builtin_aarch64_get_qregoiv2di(__o, 0);
  ret.val[1] = (uint64x2_t)__builtin_aarch64_get_qregoiv2di(__o, 1);
  return ret;
}

FUNK(int64x2x2_t)
vld1q_s64_x2(const int64_t *__a) {
  int64x2x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld1x2v2di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (int64x2_t)__builtin_aarch64_get_qregoiv2di(__o, 0);
  ret.val[1] = (int64x2_t)__builtin_aarch64_get_qregoiv2di(__o, 1);
  return ret;
}

FUNK(float16x8x2_t)
vld1q_f16_x2(const float16_t *__a) {
  float16x8x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld1x2v8hf((const __builtin_aarch64_simd_hf *)__a);
  ret.val[0] = (float16x8_t)__builtin_aarch64_get_qregoiv8hf(__o, 0);
  ret.val[1] = (float16x8_t)__builtin_aarch64_get_qregoiv8hf(__o, 1);
  return ret;
}

FUNK(float32x4x2_t)
vld1q_f32_x2(const float32_t *__a) {
  float32x4x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld1x2v4sf((const __builtin_aarch64_simd_sf *)__a);
  ret.val[0] = (float32x4_t)__builtin_aarch64_get_qregoiv4sf(__o, 0);
  ret.val[1] = (float32x4_t)__builtin_aarch64_get_qregoiv4sf(__o, 1);
  return ret;
}

FUNK(float64x2x2_t)
vld1q_f64_x2(const float64_t *__a) {
  float64x2x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld1x2v2df((const __builtin_aarch64_simd_df *)__a);
  ret.val[0] = (float64x2_t)__builtin_aarch64_get_qregoiv2df(__o, 0);
  ret.val[1] = (float64x2_t)__builtin_aarch64_get_qregoiv2df(__o, 1);
  return ret;
}

FUNK(poly8x16x2_t)
vld1q_p8_x2(const poly8_t *__a) {
  poly8x16x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld1x2v16qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (poly8x16_t)__builtin_aarch64_get_qregoiv16qi(__o, 0);
  ret.val[1] = (poly8x16_t)__builtin_aarch64_get_qregoiv16qi(__o, 1);
  return ret;
}

FUNK(poly16x8x2_t)
vld1q_p16_x2(const poly16_t *__a) {
  poly16x8x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld1x2v8hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (poly16x8_t)__builtin_aarch64_get_qregoiv8hi(__o, 0);
  ret.val[1] = (poly16x8_t)__builtin_aarch64_get_qregoiv8hi(__o, 1);
  return ret;
}

FUNK(poly64x2x2_t)
vld1q_p64_x2(const poly64_t *__a) {
  poly64x2x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld1x2v2di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (poly64x2_t)__builtin_aarch64_get_qregoiv2di(__o, 0);
  ret.val[1] = (poly64x2_t)__builtin_aarch64_get_qregoiv2di(__o, 1);
  return ret;
}

FUNK(uint16x8_t)
vld1q_u16(const uint16_t *a) {
  return (uint16x8_t)__builtin_aarch64_ld1v8hi(
      (const __builtin_aarch64_simd_hi *)a);
}

FUNK(uint32x4_t)
vld1q_u32(const uint32_t *a) {
  return (uint32x4_t)__builtin_aarch64_ld1v4si(
      (const __builtin_aarch64_simd_si *)a);
}

FUNK(uint64x2_t)
vld1q_u64(const uint64_t *a) {
  return (uint64x2_t)__builtin_aarch64_ld1v2di(
      (const __builtin_aarch64_simd_di *)a);
}

FUNK(float16x4_t)
vld1_dup_f16(const float16_t *__a) {
  return vdup_n_f16(*__a);
}

FUNK(float32x2_t)
vld1_dup_f32(const float32_t *__a) {
  return vdup_n_f32(*__a);
}

FUNK(float64x1_t)
vld1_dup_f64(const float64_t *__a) {
  return vdup_n_f64(*__a);
}

FUNK(poly8x8_t)
vld1_dup_p8(const poly8_t *__a) {
  return vdup_n_p8(*__a);
}

FUNK(poly16x4_t)
vld1_dup_p16(const poly16_t *__a) {
  return vdup_n_p16(*__a);
}

FUNK(poly64x1_t)
vld1_dup_p64(const poly64_t *__a) {
  return vdup_n_p64(*__a);
}

FUNK(int8x8_t)
vld1_dup_s8(const int8_t *__a) {
  return vdup_n_s8(*__a);
}

FUNK(int16x4_t)
vld1_dup_s16(const int16_t *__a) {
  return vdup_n_s16(*__a);
}

FUNK(int32x2_t)
vld1_dup_s32(const int32_t *__a) {
  return vdup_n_s32(*__a);
}

FUNK(int64x1_t)
vld1_dup_s64(const int64_t *__a) {
  return vdup_n_s64(*__a);
}

FUNK(uint8x8_t)
vld1_dup_u8(const uint8_t *__a) {
  return vdup_n_u8(*__a);
}

FUNK(uint16x4_t)
vld1_dup_u16(const uint16_t *__a) {
  return vdup_n_u16(*__a);
}

FUNK(uint32x2_t)
vld1_dup_u32(const uint32_t *__a) {
  return vdup_n_u32(*__a);
}

FUNK(uint64x1_t)
vld1_dup_u64(const uint64_t *__a) {
  return vdup_n_u64(*__a);
}

FUNK(float16x8_t)
vld1q_dup_f16(const float16_t *__a) {
  return vdupq_n_f16(*__a);
}

FUNK(float32x4_t)
vld1q_dup_f32(const float32_t *__a) {
  return vdupq_n_f32(*__a);
}

FUNK(float64x2_t)
vld1q_dup_f64(const float64_t *__a) {
  return vdupq_n_f64(*__a);
}

FUNK(poly8x16_t)
vld1q_dup_p8(const poly8_t *__a) {
  return vdupq_n_p8(*__a);
}

FUNK(poly16x8_t)
vld1q_dup_p16(const poly16_t *__a) {
  return vdupq_n_p16(*__a);
}

FUNK(poly64x2_t)
vld1q_dup_p64(const poly64_t *__a) {
  return vdupq_n_p64(*__a);
}

FUNK(int8x16_t)
vld1q_dup_s8(const int8_t *__a) {
  return vdupq_n_s8(*__a);
}

FUNK(int16x8_t)
vld1q_dup_s16(const int16_t *__a) {
  return vdupq_n_s16(*__a);
}

FUNK(int32x4_t)
vld1q_dup_s32(const int32_t *__a) {
  return vdupq_n_s32(*__a);
}

FUNK(int64x2_t)
vld1q_dup_s64(const int64_t *__a) {
  return vdupq_n_s64(*__a);
}

FUNK(uint8x16_t)
vld1q_dup_u8(const uint8_t *__a) {
  return vdupq_n_u8(*__a);
}

FUNK(uint16x8_t)
vld1q_dup_u16(const uint16_t *__a) {
  return vdupq_n_u16(*__a);
}

FUNK(uint32x4_t)
vld1q_dup_u32(const uint32_t *__a) {
  return vdupq_n_u32(*__a);
}

FUNK(uint64x2_t)
vld1q_dup_u64(const uint64_t *__a) {
  return vdupq_n_u64(*__a);
}

FUNK(float16x4_t)
vld1_lane_f16(const float16_t *__src, float16x4_t __vec, const int __lane) {
  return __aarch64_vset_lane_any(*__src, __vec, __lane);
}

FUNK(float32x2_t)
vld1_lane_f32(const float32_t *__src, float32x2_t __vec, const int __lane) {
  return __aarch64_vset_lane_any(*__src, __vec, __lane);
}

FUNK(float64x1_t)
vld1_lane_f64(const float64_t *__src, float64x1_t __vec, const int __lane) {
  return __aarch64_vset_lane_any(*__src, __vec, __lane);
}

FUNK(poly8x8_t)
vld1_lane_p8(const poly8_t *__src, poly8x8_t __vec, const int __lane) {
  return __aarch64_vset_lane_any(*__src, __vec, __lane);
}

FUNK(poly16x4_t)
vld1_lane_p16(const poly16_t *__src, poly16x4_t __vec, const int __lane) {
  return __aarch64_vset_lane_any(*__src, __vec, __lane);
}

FUNK(poly64x1_t)
vld1_lane_p64(const poly64_t *__src, poly64x1_t __vec, const int __lane) {
  return __aarch64_vset_lane_any(*__src, __vec, __lane);
}

FUNK(int8x8_t)
vld1_lane_s8(const int8_t *__src, int8x8_t __vec, const int __lane) {
  return __aarch64_vset_lane_any(*__src, __vec, __lane);
}

FUNK(int16x4_t)
vld1_lane_s16(const int16_t *__src, int16x4_t __vec, const int __lane) {
  return __aarch64_vset_lane_any(*__src, __vec, __lane);
}

FUNK(int32x2_t)
vld1_lane_s32(const int32_t *__src, int32x2_t __vec, const int __lane) {
  return __aarch64_vset_lane_any(*__src, __vec, __lane);
}

FUNK(int64x1_t)
vld1_lane_s64(const int64_t *__src, int64x1_t __vec, const int __lane) {
  return __aarch64_vset_lane_any(*__src, __vec, __lane);
}

FUNK(uint8x8_t)
vld1_lane_u8(const uint8_t *__src, uint8x8_t __vec, const int __lane) {
  return __aarch64_vset_lane_any(*__src, __vec, __lane);
}

FUNK(uint16x4_t)
vld1_lane_u16(const uint16_t *__src, uint16x4_t __vec, const int __lane) {
  return __aarch64_vset_lane_any(*__src, __vec, __lane);
}

FUNK(uint32x2_t)
vld1_lane_u32(const uint32_t *__src, uint32x2_t __vec, const int __lane) {
  return __aarch64_vset_lane_any(*__src, __vec, __lane);
}

FUNK(uint64x1_t)
vld1_lane_u64(const uint64_t *__src, uint64x1_t __vec, const int __lane) {
  return __aarch64_vset_lane_any(*__src, __vec, __lane);
}

FUNK(float16x8_t)
vld1q_lane_f16(const float16_t *__src, float16x8_t __vec, const int __lane) {
  return __aarch64_vset_lane_any(*__src, __vec, __lane);
}

FUNK(float32x4_t)
vld1q_lane_f32(const float32_t *__src, float32x4_t __vec, const int __lane) {
  return __aarch64_vset_lane_any(*__src, __vec, __lane);
}

FUNK(float64x2_t)
vld1q_lane_f64(const float64_t *__src, float64x2_t __vec, const int __lane) {
  return __aarch64_vset_lane_any(*__src, __vec, __lane);
}

FUNK(poly8x16_t)
vld1q_lane_p8(const poly8_t *__src, poly8x16_t __vec, const int __lane) {
  return __aarch64_vset_lane_any(*__src, __vec, __lane);
}

FUNK(poly16x8_t)
vld1q_lane_p16(const poly16_t *__src, poly16x8_t __vec, const int __lane) {
  return __aarch64_vset_lane_any(*__src, __vec, __lane);
}

FUNK(poly64x2_t)
vld1q_lane_p64(const poly64_t *__src, poly64x2_t __vec, const int __lane) {
  return __aarch64_vset_lane_any(*__src, __vec, __lane);
}

FUNK(int8x16_t)
vld1q_lane_s8(const int8_t *__src, int8x16_t __vec, const int __lane) {
  return __aarch64_vset_lane_any(*__src, __vec, __lane);
}

FUNK(int16x8_t)
vld1q_lane_s16(const int16_t *__src, int16x8_t __vec, const int __lane) {
  return __aarch64_vset_lane_any(*__src, __vec, __lane);
}

FUNK(int32x4_t)
vld1q_lane_s32(const int32_t *__src, int32x4_t __vec, const int __lane) {
  return __aarch64_vset_lane_any(*__src, __vec, __lane);
}

FUNK(int64x2_t)
vld1q_lane_s64(const int64_t *__src, int64x2_t __vec, const int __lane) {
  return __aarch64_vset_lane_any(*__src, __vec, __lane);
}

FUNK(uint8x16_t)
vld1q_lane_u8(const uint8_t *__src, uint8x16_t __vec, const int __lane) {
  return __aarch64_vset_lane_any(*__src, __vec, __lane);
}

FUNK(uint16x8_t)
vld1q_lane_u16(const uint16_t *__src, uint16x8_t __vec, const int __lane) {
  return __aarch64_vset_lane_any(*__src, __vec, __lane);
}

FUNK(uint32x4_t)
vld1q_lane_u32(const uint32_t *__src, uint32x4_t __vec, const int __lane) {
  return __aarch64_vset_lane_any(*__src, __vec, __lane);
}

FUNK(uint64x2_t)
vld1q_lane_u64(const uint64_t *__src, uint64x2_t __vec, const int __lane) {
  return __aarch64_vset_lane_any(*__src, __vec, __lane);
}

FUNK(int64x1x2_t)
vld2_s64(const int64_t *__a) {
  int64x1x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (int64x1_t)__builtin_aarch64_get_dregoidi(__o, 0);
  ret.val[1] = (int64x1_t)__builtin_aarch64_get_dregoidi(__o, 1);
  return ret;
}

FUNK(uint64x1x2_t)
vld2_u64(const uint64_t *__a) {
  uint64x1x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (uint64x1_t)__builtin_aarch64_get_dregoidi(__o, 0);
  ret.val[1] = (uint64x1_t)__builtin_aarch64_get_dregoidi(__o, 1);
  return ret;
}

FUNK(float64x1x2_t)
vld2_f64(const float64_t *__a) {
  float64x1x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2df((const __builtin_aarch64_simd_df *)__a);
  ret.val[0] = (float64x1_t){__builtin_aarch64_get_dregoidf(__o, 0)};
  ret.val[1] = (float64x1_t){__builtin_aarch64_get_dregoidf(__o, 1)};
  return ret;
}

FUNK(int8x8x2_t)
vld2_s8(const int8_t *__a) {
  int8x8x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2v8qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (int8x8_t)__builtin_aarch64_get_dregoiv8qi(__o, 0);
  ret.val[1] = (int8x8_t)__builtin_aarch64_get_dregoiv8qi(__o, 1);
  return ret;
}

FUNK(poly8x8x2_t)
vld2_p8(const poly8_t *__a) {
  poly8x8x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2v8qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (poly8x8_t)__builtin_aarch64_get_dregoiv8qi(__o, 0);
  ret.val[1] = (poly8x8_t)__builtin_aarch64_get_dregoiv8qi(__o, 1);
  return ret;
}

FUNK(poly64x1x2_t)
vld2_p64(const poly64_t *__a) {
  poly64x1x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (poly64x1_t)__builtin_aarch64_get_dregoidi_pss(__o, 0);
  ret.val[1] = (poly64x1_t)__builtin_aarch64_get_dregoidi_pss(__o, 1);
  return ret;
}

FUNK(int16x4x2_t)
vld2_s16(const int16_t *__a) {
  int16x4x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2v4hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (int16x4_t)__builtin_aarch64_get_dregoiv4hi(__o, 0);
  ret.val[1] = (int16x4_t)__builtin_aarch64_get_dregoiv4hi(__o, 1);
  return ret;
}

FUNK(poly16x4x2_t)
vld2_p16(const poly16_t *__a) {
  poly16x4x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2v4hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (poly16x4_t)__builtin_aarch64_get_dregoiv4hi(__o, 0);
  ret.val[1] = (poly16x4_t)__builtin_aarch64_get_dregoiv4hi(__o, 1);
  return ret;
}

FUNK(int32x2x2_t)
vld2_s32(const int32_t *__a) {
  int32x2x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2v2si((const __builtin_aarch64_simd_si *)__a);
  ret.val[0] = (int32x2_t)__builtin_aarch64_get_dregoiv2si(__o, 0);
  ret.val[1] = (int32x2_t)__builtin_aarch64_get_dregoiv2si(__o, 1);
  return ret;
}

FUNK(uint8x8x2_t)
vld2_u8(const uint8_t *__a) {
  uint8x8x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2v8qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (uint8x8_t)__builtin_aarch64_get_dregoiv8qi(__o, 0);
  ret.val[1] = (uint8x8_t)__builtin_aarch64_get_dregoiv8qi(__o, 1);
  return ret;
}

FUNK(uint16x4x2_t)
vld2_u16(const uint16_t *__a) {
  uint16x4x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2v4hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (uint16x4_t)__builtin_aarch64_get_dregoiv4hi(__o, 0);
  ret.val[1] = (uint16x4_t)__builtin_aarch64_get_dregoiv4hi(__o, 1);
  return ret;
}

FUNK(uint32x2x2_t)
vld2_u32(const uint32_t *__a) {
  uint32x2x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2v2si((const __builtin_aarch64_simd_si *)__a);
  ret.val[0] = (uint32x2_t)__builtin_aarch64_get_dregoiv2si(__o, 0);
  ret.val[1] = (uint32x2_t)__builtin_aarch64_get_dregoiv2si(__o, 1);
  return ret;
}

FUNK(float16x4x2_t)
vld2_f16(const float16_t *__a) {
  float16x4x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2v4hf(__a);
  ret.val[0] = __builtin_aarch64_get_dregoiv4hf(__o, 0);
  ret.val[1] = __builtin_aarch64_get_dregoiv4hf(__o, 1);
  return ret;
}

FUNK(float32x2x2_t)
vld2_f32(const float32_t *__a) {
  float32x2x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2v2sf((const __builtin_aarch64_simd_sf *)__a);
  ret.val[0] = (float32x2_t)__builtin_aarch64_get_dregoiv2sf(__o, 0);
  ret.val[1] = (float32x2_t)__builtin_aarch64_get_dregoiv2sf(__o, 1);
  return ret;
}

FUNK(int8x16x2_t)
vld2q_s8(const int8_t *__a) {
  int8x16x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2v16qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (int8x16_t)__builtin_aarch64_get_qregoiv16qi(__o, 0);
  ret.val[1] = (int8x16_t)__builtin_aarch64_get_qregoiv16qi(__o, 1);
  return ret;
}

FUNK(poly8x16x2_t)
vld2q_p8(const poly8_t *__a) {
  poly8x16x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2v16qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (poly8x16_t)__builtin_aarch64_get_qregoiv16qi(__o, 0);
  ret.val[1] = (poly8x16_t)__builtin_aarch64_get_qregoiv16qi(__o, 1);
  return ret;
}

FUNK(int16x8x2_t)
vld2q_s16(const int16_t *__a) {
  int16x8x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2v8hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (int16x8_t)__builtin_aarch64_get_qregoiv8hi(__o, 0);
  ret.val[1] = (int16x8_t)__builtin_aarch64_get_qregoiv8hi(__o, 1);
  return ret;
}

FUNK(poly16x8x2_t)
vld2q_p16(const poly16_t *__a) {
  poly16x8x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2v8hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (poly16x8_t)__builtin_aarch64_get_qregoiv8hi(__o, 0);
  ret.val[1] = (poly16x8_t)__builtin_aarch64_get_qregoiv8hi(__o, 1);
  return ret;
}

FUNK(poly64x2x2_t)
vld2q_p64(const poly64_t *__a) {
  poly64x2x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2v2di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (poly64x2_t)__builtin_aarch64_get_qregoiv2di_pss(__o, 0);
  ret.val[1] = (poly64x2_t)__builtin_aarch64_get_qregoiv2di_pss(__o, 1);
  return ret;
}

FUNK(int32x4x2_t)
vld2q_s32(const int32_t *__a) {
  int32x4x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2v4si((const __builtin_aarch64_simd_si *)__a);
  ret.val[0] = (int32x4_t)__builtin_aarch64_get_qregoiv4si(__o, 0);
  ret.val[1] = (int32x4_t)__builtin_aarch64_get_qregoiv4si(__o, 1);
  return ret;
}

FUNK(int64x2x2_t)
vld2q_s64(const int64_t *__a) {
  int64x2x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2v2di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (int64x2_t)__builtin_aarch64_get_qregoiv2di(__o, 0);
  ret.val[1] = (int64x2_t)__builtin_aarch64_get_qregoiv2di(__o, 1);
  return ret;
}

FUNK(uint8x16x2_t)
vld2q_u8(const uint8_t *__a) {
  uint8x16x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2v16qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (uint8x16_t)__builtin_aarch64_get_qregoiv16qi(__o, 0);
  ret.val[1] = (uint8x16_t)__builtin_aarch64_get_qregoiv16qi(__o, 1);
  return ret;
}

FUNK(uint16x8x2_t)
vld2q_u16(const uint16_t *__a) {
  uint16x8x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2v8hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (uint16x8_t)__builtin_aarch64_get_qregoiv8hi(__o, 0);
  ret.val[1] = (uint16x8_t)__builtin_aarch64_get_qregoiv8hi(__o, 1);
  return ret;
}

FUNK(uint32x4x2_t)
vld2q_u32(const uint32_t *__a) {
  uint32x4x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2v4si((const __builtin_aarch64_simd_si *)__a);
  ret.val[0] = (uint32x4_t)__builtin_aarch64_get_qregoiv4si(__o, 0);
  ret.val[1] = (uint32x4_t)__builtin_aarch64_get_qregoiv4si(__o, 1);
  return ret;
}

FUNK(uint64x2x2_t)
vld2q_u64(const uint64_t *__a) {
  uint64x2x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2v2di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (uint64x2_t)__builtin_aarch64_get_qregoiv2di(__o, 0);
  ret.val[1] = (uint64x2_t)__builtin_aarch64_get_qregoiv2di(__o, 1);
  return ret;
}

FUNK(float16x8x2_t)
vld2q_f16(const float16_t *__a) {
  float16x8x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2v8hf(__a);
  ret.val[0] = __builtin_aarch64_get_qregoiv8hf(__o, 0);
  ret.val[1] = __builtin_aarch64_get_qregoiv8hf(__o, 1);
  return ret;
}

FUNK(float32x4x2_t)
vld2q_f32(const float32_t *__a) {
  float32x4x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2v4sf((const __builtin_aarch64_simd_sf *)__a);
  ret.val[0] = (float32x4_t)__builtin_aarch64_get_qregoiv4sf(__o, 0);
  ret.val[1] = (float32x4_t)__builtin_aarch64_get_qregoiv4sf(__o, 1);
  return ret;
}

FUNK(float64x2x2_t)
vld2q_f64(const float64_t *__a) {
  float64x2x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2v2df((const __builtin_aarch64_simd_df *)__a);
  ret.val[0] = (float64x2_t)__builtin_aarch64_get_qregoiv2df(__o, 0);
  ret.val[1] = (float64x2_t)__builtin_aarch64_get_qregoiv2df(__o, 1);
  return ret;
}

FUNK(int64x1x3_t)
vld3_s64(const int64_t *__a) {
  int64x1x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (int64x1_t)__builtin_aarch64_get_dregcidi(__o, 0);
  ret.val[1] = (int64x1_t)__builtin_aarch64_get_dregcidi(__o, 1);
  ret.val[2] = (int64x1_t)__builtin_aarch64_get_dregcidi(__o, 2);
  return ret;
}

FUNK(uint64x1x3_t)
vld3_u64(const uint64_t *__a) {
  uint64x1x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (uint64x1_t)__builtin_aarch64_get_dregcidi(__o, 0);
  ret.val[1] = (uint64x1_t)__builtin_aarch64_get_dregcidi(__o, 1);
  ret.val[2] = (uint64x1_t)__builtin_aarch64_get_dregcidi(__o, 2);
  return ret;
}

FUNK(float64x1x3_t)
vld3_f64(const float64_t *__a) {
  float64x1x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3df((const __builtin_aarch64_simd_df *)__a);
  ret.val[0] = (float64x1_t){__builtin_aarch64_get_dregcidf(__o, 0)};
  ret.val[1] = (float64x1_t){__builtin_aarch64_get_dregcidf(__o, 1)};
  ret.val[2] = (float64x1_t){__builtin_aarch64_get_dregcidf(__o, 2)};
  return ret;
}

FUNK(int8x8x3_t)
vld3_s8(const int8_t *__a) {
  int8x8x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3v8qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (int8x8_t)__builtin_aarch64_get_dregciv8qi(__o, 0);
  ret.val[1] = (int8x8_t)__builtin_aarch64_get_dregciv8qi(__o, 1);
  ret.val[2] = (int8x8_t)__builtin_aarch64_get_dregciv8qi(__o, 2);
  return ret;
}

FUNK(poly8x8x3_t)
vld3_p8(const poly8_t *__a) {
  poly8x8x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3v8qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (poly8x8_t)__builtin_aarch64_get_dregciv8qi(__o, 0);
  ret.val[1] = (poly8x8_t)__builtin_aarch64_get_dregciv8qi(__o, 1);
  ret.val[2] = (poly8x8_t)__builtin_aarch64_get_dregciv8qi(__o, 2);
  return ret;
}

FUNK(int16x4x3_t)
vld3_s16(const int16_t *__a) {
  int16x4x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3v4hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (int16x4_t)__builtin_aarch64_get_dregciv4hi(__o, 0);
  ret.val[1] = (int16x4_t)__builtin_aarch64_get_dregciv4hi(__o, 1);
  ret.val[2] = (int16x4_t)__builtin_aarch64_get_dregciv4hi(__o, 2);
  return ret;
}

FUNK(poly16x4x3_t)
vld3_p16(const poly16_t *__a) {
  poly16x4x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3v4hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (poly16x4_t)__builtin_aarch64_get_dregciv4hi(__o, 0);
  ret.val[1] = (poly16x4_t)__builtin_aarch64_get_dregciv4hi(__o, 1);
  ret.val[2] = (poly16x4_t)__builtin_aarch64_get_dregciv4hi(__o, 2);
  return ret;
}

FUNK(int32x2x3_t)
vld3_s32(const int32_t *__a) {
  int32x2x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3v2si((const __builtin_aarch64_simd_si *)__a);
  ret.val[0] = (int32x2_t)__builtin_aarch64_get_dregciv2si(__o, 0);
  ret.val[1] = (int32x2_t)__builtin_aarch64_get_dregciv2si(__o, 1);
  ret.val[2] = (int32x2_t)__builtin_aarch64_get_dregciv2si(__o, 2);
  return ret;
}

FUNK(uint8x8x3_t)
vld3_u8(const uint8_t *__a) {
  uint8x8x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3v8qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (uint8x8_t)__builtin_aarch64_get_dregciv8qi(__o, 0);
  ret.val[1] = (uint8x8_t)__builtin_aarch64_get_dregciv8qi(__o, 1);
  ret.val[2] = (uint8x8_t)__builtin_aarch64_get_dregciv8qi(__o, 2);
  return ret;
}

FUNK(uint16x4x3_t)
vld3_u16(const uint16_t *__a) {
  uint16x4x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3v4hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (uint16x4_t)__builtin_aarch64_get_dregciv4hi(__o, 0);
  ret.val[1] = (uint16x4_t)__builtin_aarch64_get_dregciv4hi(__o, 1);
  ret.val[2] = (uint16x4_t)__builtin_aarch64_get_dregciv4hi(__o, 2);
  return ret;
}

FUNK(uint32x2x3_t)
vld3_u32(const uint32_t *__a) {
  uint32x2x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3v2si((const __builtin_aarch64_simd_si *)__a);
  ret.val[0] = (uint32x2_t)__builtin_aarch64_get_dregciv2si(__o, 0);
  ret.val[1] = (uint32x2_t)__builtin_aarch64_get_dregciv2si(__o, 1);
  ret.val[2] = (uint32x2_t)__builtin_aarch64_get_dregciv2si(__o, 2);
  return ret;
}

FUNK(float16x4x3_t)
vld3_f16(const float16_t *__a) {
  float16x4x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3v4hf(__a);
  ret.val[0] = __builtin_aarch64_get_dregciv4hf(__o, 0);
  ret.val[1] = __builtin_aarch64_get_dregciv4hf(__o, 1);
  ret.val[2] = __builtin_aarch64_get_dregciv4hf(__o, 2);
  return ret;
}

FUNK(float32x2x3_t)
vld3_f32(const float32_t *__a) {
  float32x2x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3v2sf((const __builtin_aarch64_simd_sf *)__a);
  ret.val[0] = (float32x2_t)__builtin_aarch64_get_dregciv2sf(__o, 0);
  ret.val[1] = (float32x2_t)__builtin_aarch64_get_dregciv2sf(__o, 1);
  ret.val[2] = (float32x2_t)__builtin_aarch64_get_dregciv2sf(__o, 2);
  return ret;
}

FUNK(poly64x1x3_t)
vld3_p64(const poly64_t *__a) {
  poly64x1x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (poly64x1_t)__builtin_aarch64_get_dregcidi_pss(__o, 0);
  ret.val[1] = (poly64x1_t)__builtin_aarch64_get_dregcidi_pss(__o, 1);
  ret.val[2] = (poly64x1_t)__builtin_aarch64_get_dregcidi_pss(__o, 2);
  return ret;
}

FUNK(int8x16x3_t)
vld3q_s8(const int8_t *__a) {
  int8x16x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3v16qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (int8x16_t)__builtin_aarch64_get_qregciv16qi(__o, 0);
  ret.val[1] = (int8x16_t)__builtin_aarch64_get_qregciv16qi(__o, 1);
  ret.val[2] = (int8x16_t)__builtin_aarch64_get_qregciv16qi(__o, 2);
  return ret;
}

FUNK(poly8x16x3_t)
vld3q_p8(const poly8_t *__a) {
  poly8x16x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3v16qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (poly8x16_t)__builtin_aarch64_get_qregciv16qi(__o, 0);
  ret.val[1] = (poly8x16_t)__builtin_aarch64_get_qregciv16qi(__o, 1);
  ret.val[2] = (poly8x16_t)__builtin_aarch64_get_qregciv16qi(__o, 2);
  return ret;
}

FUNK(int16x8x3_t)
vld3q_s16(const int16_t *__a) {
  int16x8x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3v8hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (int16x8_t)__builtin_aarch64_get_qregciv8hi(__o, 0);
  ret.val[1] = (int16x8_t)__builtin_aarch64_get_qregciv8hi(__o, 1);
  ret.val[2] = (int16x8_t)__builtin_aarch64_get_qregciv8hi(__o, 2);
  return ret;
}

FUNK(poly16x8x3_t)
vld3q_p16(const poly16_t *__a) {
  poly16x8x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3v8hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (poly16x8_t)__builtin_aarch64_get_qregciv8hi(__o, 0);
  ret.val[1] = (poly16x8_t)__builtin_aarch64_get_qregciv8hi(__o, 1);
  ret.val[2] = (poly16x8_t)__builtin_aarch64_get_qregciv8hi(__o, 2);
  return ret;
}

FUNK(int32x4x3_t)
vld3q_s32(const int32_t *__a) {
  int32x4x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3v4si((const __builtin_aarch64_simd_si *)__a);
  ret.val[0] = (int32x4_t)__builtin_aarch64_get_qregciv4si(__o, 0);
  ret.val[1] = (int32x4_t)__builtin_aarch64_get_qregciv4si(__o, 1);
  ret.val[2] = (int32x4_t)__builtin_aarch64_get_qregciv4si(__o, 2);
  return ret;
}

FUNK(int64x2x3_t)
vld3q_s64(const int64_t *__a) {
  int64x2x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3v2di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (int64x2_t)__builtin_aarch64_get_qregciv2di(__o, 0);
  ret.val[1] = (int64x2_t)__builtin_aarch64_get_qregciv2di(__o, 1);
  ret.val[2] = (int64x2_t)__builtin_aarch64_get_qregciv2di(__o, 2);
  return ret;
}

FUNK(uint8x16x3_t)
vld3q_u8(const uint8_t *__a) {
  uint8x16x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3v16qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (uint8x16_t)__builtin_aarch64_get_qregciv16qi(__o, 0);
  ret.val[1] = (uint8x16_t)__builtin_aarch64_get_qregciv16qi(__o, 1);
  ret.val[2] = (uint8x16_t)__builtin_aarch64_get_qregciv16qi(__o, 2);
  return ret;
}

FUNK(uint16x8x3_t)
vld3q_u16(const uint16_t *__a) {
  uint16x8x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3v8hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (uint16x8_t)__builtin_aarch64_get_qregciv8hi(__o, 0);
  ret.val[1] = (uint16x8_t)__builtin_aarch64_get_qregciv8hi(__o, 1);
  ret.val[2] = (uint16x8_t)__builtin_aarch64_get_qregciv8hi(__o, 2);
  return ret;
}

FUNK(uint32x4x3_t)
vld3q_u32(const uint32_t *__a) {
  uint32x4x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3v4si((const __builtin_aarch64_simd_si *)__a);
  ret.val[0] = (uint32x4_t)__builtin_aarch64_get_qregciv4si(__o, 0);
  ret.val[1] = (uint32x4_t)__builtin_aarch64_get_qregciv4si(__o, 1);
  ret.val[2] = (uint32x4_t)__builtin_aarch64_get_qregciv4si(__o, 2);
  return ret;
}

FUNK(uint64x2x3_t)
vld3q_u64(const uint64_t *__a) {
  uint64x2x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3v2di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (uint64x2_t)__builtin_aarch64_get_qregciv2di(__o, 0);
  ret.val[1] = (uint64x2_t)__builtin_aarch64_get_qregciv2di(__o, 1);
  ret.val[2] = (uint64x2_t)__builtin_aarch64_get_qregciv2di(__o, 2);
  return ret;
}

FUNK(float16x8x3_t)
vld3q_f16(const float16_t *__a) {
  float16x8x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3v8hf(__a);
  ret.val[0] = __builtin_aarch64_get_qregciv8hf(__o, 0);
  ret.val[1] = __builtin_aarch64_get_qregciv8hf(__o, 1);
  ret.val[2] = __builtin_aarch64_get_qregciv8hf(__o, 2);
  return ret;
}

FUNK(float32x4x3_t)
vld3q_f32(const float32_t *__a) {
  float32x4x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3v4sf((const __builtin_aarch64_simd_sf *)__a);
  ret.val[0] = (float32x4_t)__builtin_aarch64_get_qregciv4sf(__o, 0);
  ret.val[1] = (float32x4_t)__builtin_aarch64_get_qregciv4sf(__o, 1);
  ret.val[2] = (float32x4_t)__builtin_aarch64_get_qregciv4sf(__o, 2);
  return ret;
}

FUNK(float64x2x3_t)
vld3q_f64(const float64_t *__a) {
  float64x2x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3v2df((const __builtin_aarch64_simd_df *)__a);
  ret.val[0] = (float64x2_t)__builtin_aarch64_get_qregciv2df(__o, 0);
  ret.val[1] = (float64x2_t)__builtin_aarch64_get_qregciv2df(__o, 1);
  ret.val[2] = (float64x2_t)__builtin_aarch64_get_qregciv2df(__o, 2);
  return ret;
}

FUNK(poly64x2x3_t)
vld3q_p64(const poly64_t *__a) {
  poly64x2x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3v2di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (poly64x2_t)__builtin_aarch64_get_qregciv2di_pss(__o, 0);
  ret.val[1] = (poly64x2_t)__builtin_aarch64_get_qregciv2di_pss(__o, 1);
  ret.val[2] = (poly64x2_t)__builtin_aarch64_get_qregciv2di_pss(__o, 2);
  return ret;
}

FUNK(int64x1x4_t)
vld4_s64(const int64_t *__a) {
  int64x1x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (int64x1_t)__builtin_aarch64_get_dregxidi(__o, 0);
  ret.val[1] = (int64x1_t)__builtin_aarch64_get_dregxidi(__o, 1);
  ret.val[2] = (int64x1_t)__builtin_aarch64_get_dregxidi(__o, 2);
  ret.val[3] = (int64x1_t)__builtin_aarch64_get_dregxidi(__o, 3);
  return ret;
}

FUNK(uint64x1x4_t)
vld4_u64(const uint64_t *__a) {
  uint64x1x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (uint64x1_t)__builtin_aarch64_get_dregxidi(__o, 0);
  ret.val[1] = (uint64x1_t)__builtin_aarch64_get_dregxidi(__o, 1);
  ret.val[2] = (uint64x1_t)__builtin_aarch64_get_dregxidi(__o, 2);
  ret.val[3] = (uint64x1_t)__builtin_aarch64_get_dregxidi(__o, 3);
  return ret;
}

FUNK(float64x1x4_t)
vld4_f64(const float64_t *__a) {
  float64x1x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4df((const __builtin_aarch64_simd_df *)__a);
  ret.val[0] = (float64x1_t){__builtin_aarch64_get_dregxidf(__o, 0)};
  ret.val[1] = (float64x1_t){__builtin_aarch64_get_dregxidf(__o, 1)};
  ret.val[2] = (float64x1_t){__builtin_aarch64_get_dregxidf(__o, 2)};
  ret.val[3] = (float64x1_t){__builtin_aarch64_get_dregxidf(__o, 3)};
  return ret;
}

FUNK(int8x8x4_t)
vld4_s8(const int8_t *__a) {
  int8x8x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4v8qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (int8x8_t)__builtin_aarch64_get_dregxiv8qi(__o, 0);
  ret.val[1] = (int8x8_t)__builtin_aarch64_get_dregxiv8qi(__o, 1);
  ret.val[2] = (int8x8_t)__builtin_aarch64_get_dregxiv8qi(__o, 2);
  ret.val[3] = (int8x8_t)__builtin_aarch64_get_dregxiv8qi(__o, 3);
  return ret;
}

FUNK(poly8x8x4_t)
vld4_p8(const poly8_t *__a) {
  poly8x8x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4v8qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (poly8x8_t)__builtin_aarch64_get_dregxiv8qi(__o, 0);
  ret.val[1] = (poly8x8_t)__builtin_aarch64_get_dregxiv8qi(__o, 1);
  ret.val[2] = (poly8x8_t)__builtin_aarch64_get_dregxiv8qi(__o, 2);
  ret.val[3] = (poly8x8_t)__builtin_aarch64_get_dregxiv8qi(__o, 3);
  return ret;
}

FUNK(int16x4x4_t)
vld4_s16(const int16_t *__a) {
  int16x4x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4v4hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (int16x4_t)__builtin_aarch64_get_dregxiv4hi(__o, 0);
  ret.val[1] = (int16x4_t)__builtin_aarch64_get_dregxiv4hi(__o, 1);
  ret.val[2] = (int16x4_t)__builtin_aarch64_get_dregxiv4hi(__o, 2);
  ret.val[3] = (int16x4_t)__builtin_aarch64_get_dregxiv4hi(__o, 3);
  return ret;
}

FUNK(poly16x4x4_t)
vld4_p16(const poly16_t *__a) {
  poly16x4x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4v4hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (poly16x4_t)__builtin_aarch64_get_dregxiv4hi(__o, 0);
  ret.val[1] = (poly16x4_t)__builtin_aarch64_get_dregxiv4hi(__o, 1);
  ret.val[2] = (poly16x4_t)__builtin_aarch64_get_dregxiv4hi(__o, 2);
  ret.val[3] = (poly16x4_t)__builtin_aarch64_get_dregxiv4hi(__o, 3);
  return ret;
}

FUNK(int32x2x4_t)
vld4_s32(const int32_t *__a) {
  int32x2x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4v2si((const __builtin_aarch64_simd_si *)__a);
  ret.val[0] = (int32x2_t)__builtin_aarch64_get_dregxiv2si(__o, 0);
  ret.val[1] = (int32x2_t)__builtin_aarch64_get_dregxiv2si(__o, 1);
  ret.val[2] = (int32x2_t)__builtin_aarch64_get_dregxiv2si(__o, 2);
  ret.val[3] = (int32x2_t)__builtin_aarch64_get_dregxiv2si(__o, 3);
  return ret;
}

FUNK(uint8x8x4_t)
vld4_u8(const uint8_t *__a) {
  uint8x8x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4v8qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (uint8x8_t)__builtin_aarch64_get_dregxiv8qi(__o, 0);
  ret.val[1] = (uint8x8_t)__builtin_aarch64_get_dregxiv8qi(__o, 1);
  ret.val[2] = (uint8x8_t)__builtin_aarch64_get_dregxiv8qi(__o, 2);
  ret.val[3] = (uint8x8_t)__builtin_aarch64_get_dregxiv8qi(__o, 3);
  return ret;
}

FUNK(uint16x4x4_t)
vld4_u16(const uint16_t *__a) {
  uint16x4x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4v4hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (uint16x4_t)__builtin_aarch64_get_dregxiv4hi(__o, 0);
  ret.val[1] = (uint16x4_t)__builtin_aarch64_get_dregxiv4hi(__o, 1);
  ret.val[2] = (uint16x4_t)__builtin_aarch64_get_dregxiv4hi(__o, 2);
  ret.val[3] = (uint16x4_t)__builtin_aarch64_get_dregxiv4hi(__o, 3);
  return ret;
}

FUNK(uint32x2x4_t)
vld4_u32(const uint32_t *__a) {
  uint32x2x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4v2si((const __builtin_aarch64_simd_si *)__a);
  ret.val[0] = (uint32x2_t)__builtin_aarch64_get_dregxiv2si(__o, 0);
  ret.val[1] = (uint32x2_t)__builtin_aarch64_get_dregxiv2si(__o, 1);
  ret.val[2] = (uint32x2_t)__builtin_aarch64_get_dregxiv2si(__o, 2);
  ret.val[3] = (uint32x2_t)__builtin_aarch64_get_dregxiv2si(__o, 3);
  return ret;
}

FUNK(float16x4x4_t)
vld4_f16(const float16_t *__a) {
  float16x4x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4v4hf(__a);
  ret.val[0] = __builtin_aarch64_get_dregxiv4hf(__o, 0);
  ret.val[1] = __builtin_aarch64_get_dregxiv4hf(__o, 1);
  ret.val[2] = __builtin_aarch64_get_dregxiv4hf(__o, 2);
  ret.val[3] = __builtin_aarch64_get_dregxiv4hf(__o, 3);
  return ret;
}

FUNK(float32x2x4_t)
vld4_f32(const float32_t *__a) {
  float32x2x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4v2sf((const __builtin_aarch64_simd_sf *)__a);
  ret.val[0] = (float32x2_t)__builtin_aarch64_get_dregxiv2sf(__o, 0);
  ret.val[1] = (float32x2_t)__builtin_aarch64_get_dregxiv2sf(__o, 1);
  ret.val[2] = (float32x2_t)__builtin_aarch64_get_dregxiv2sf(__o, 2);
  ret.val[3] = (float32x2_t)__builtin_aarch64_get_dregxiv2sf(__o, 3);
  return ret;
}

FUNK(poly64x1x4_t)
vld4_p64(const poly64_t *__a) {
  poly64x1x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (poly64x1_t)__builtin_aarch64_get_dregxidi_pss(__o, 0);
  ret.val[1] = (poly64x1_t)__builtin_aarch64_get_dregxidi_pss(__o, 1);
  ret.val[2] = (poly64x1_t)__builtin_aarch64_get_dregxidi_pss(__o, 2);
  ret.val[3] = (poly64x1_t)__builtin_aarch64_get_dregxidi_pss(__o, 3);
  return ret;
}

FUNK(int8x16x4_t)
vld4q_s8(const int8_t *__a) {
  int8x16x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4v16qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (int8x16_t)__builtin_aarch64_get_qregxiv16qi(__o, 0);
  ret.val[1] = (int8x16_t)__builtin_aarch64_get_qregxiv16qi(__o, 1);
  ret.val[2] = (int8x16_t)__builtin_aarch64_get_qregxiv16qi(__o, 2);
  ret.val[3] = (int8x16_t)__builtin_aarch64_get_qregxiv16qi(__o, 3);
  return ret;
}

FUNK(poly8x16x4_t)
vld4q_p8(const poly8_t *__a) {
  poly8x16x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4v16qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (poly8x16_t)__builtin_aarch64_get_qregxiv16qi(__o, 0);
  ret.val[1] = (poly8x16_t)__builtin_aarch64_get_qregxiv16qi(__o, 1);
  ret.val[2] = (poly8x16_t)__builtin_aarch64_get_qregxiv16qi(__o, 2);
  ret.val[3] = (poly8x16_t)__builtin_aarch64_get_qregxiv16qi(__o, 3);
  return ret;
}

FUNK(int16x8x4_t)
vld4q_s16(const int16_t *__a) {
  int16x8x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4v8hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (int16x8_t)__builtin_aarch64_get_qregxiv8hi(__o, 0);
  ret.val[1] = (int16x8_t)__builtin_aarch64_get_qregxiv8hi(__o, 1);
  ret.val[2] = (int16x8_t)__builtin_aarch64_get_qregxiv8hi(__o, 2);
  ret.val[3] = (int16x8_t)__builtin_aarch64_get_qregxiv8hi(__o, 3);
  return ret;
}

FUNK(poly16x8x4_t)
vld4q_p16(const poly16_t *__a) {
  poly16x8x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4v8hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (poly16x8_t)__builtin_aarch64_get_qregxiv8hi(__o, 0);
  ret.val[1] = (poly16x8_t)__builtin_aarch64_get_qregxiv8hi(__o, 1);
  ret.val[2] = (poly16x8_t)__builtin_aarch64_get_qregxiv8hi(__o, 2);
  ret.val[3] = (poly16x8_t)__builtin_aarch64_get_qregxiv8hi(__o, 3);
  return ret;
}

FUNK(int32x4x4_t)
vld4q_s32(const int32_t *__a) {
  int32x4x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4v4si((const __builtin_aarch64_simd_si *)__a);
  ret.val[0] = (int32x4_t)__builtin_aarch64_get_qregxiv4si(__o, 0);
  ret.val[1] = (int32x4_t)__builtin_aarch64_get_qregxiv4si(__o, 1);
  ret.val[2] = (int32x4_t)__builtin_aarch64_get_qregxiv4si(__o, 2);
  ret.val[3] = (int32x4_t)__builtin_aarch64_get_qregxiv4si(__o, 3);
  return ret;
}

FUNK(int64x2x4_t)
vld4q_s64(const int64_t *__a) {
  int64x2x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4v2di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (int64x2_t)__builtin_aarch64_get_qregxiv2di(__o, 0);
  ret.val[1] = (int64x2_t)__builtin_aarch64_get_qregxiv2di(__o, 1);
  ret.val[2] = (int64x2_t)__builtin_aarch64_get_qregxiv2di(__o, 2);
  ret.val[3] = (int64x2_t)__builtin_aarch64_get_qregxiv2di(__o, 3);
  return ret;
}

FUNK(uint8x16x4_t)
vld4q_u8(const uint8_t *__a) {
  uint8x16x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4v16qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (uint8x16_t)__builtin_aarch64_get_qregxiv16qi(__o, 0);
  ret.val[1] = (uint8x16_t)__builtin_aarch64_get_qregxiv16qi(__o, 1);
  ret.val[2] = (uint8x16_t)__builtin_aarch64_get_qregxiv16qi(__o, 2);
  ret.val[3] = (uint8x16_t)__builtin_aarch64_get_qregxiv16qi(__o, 3);
  return ret;
}

FUNK(uint16x8x4_t)
vld4q_u16(const uint16_t *__a) {
  uint16x8x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4v8hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (uint16x8_t)__builtin_aarch64_get_qregxiv8hi(__o, 0);
  ret.val[1] = (uint16x8_t)__builtin_aarch64_get_qregxiv8hi(__o, 1);
  ret.val[2] = (uint16x8_t)__builtin_aarch64_get_qregxiv8hi(__o, 2);
  ret.val[3] = (uint16x8_t)__builtin_aarch64_get_qregxiv8hi(__o, 3);
  return ret;
}

FUNK(uint32x4x4_t)
vld4q_u32(const uint32_t *__a) {
  uint32x4x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4v4si((const __builtin_aarch64_simd_si *)__a);
  ret.val[0] = (uint32x4_t)__builtin_aarch64_get_qregxiv4si(__o, 0);
  ret.val[1] = (uint32x4_t)__builtin_aarch64_get_qregxiv4si(__o, 1);
  ret.val[2] = (uint32x4_t)__builtin_aarch64_get_qregxiv4si(__o, 2);
  ret.val[3] = (uint32x4_t)__builtin_aarch64_get_qregxiv4si(__o, 3);
  return ret;
}

FUNK(uint64x2x4_t)
vld4q_u64(const uint64_t *__a) {
  uint64x2x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4v2di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (uint64x2_t)__builtin_aarch64_get_qregxiv2di(__o, 0);
  ret.val[1] = (uint64x2_t)__builtin_aarch64_get_qregxiv2di(__o, 1);
  ret.val[2] = (uint64x2_t)__builtin_aarch64_get_qregxiv2di(__o, 2);
  ret.val[3] = (uint64x2_t)__builtin_aarch64_get_qregxiv2di(__o, 3);
  return ret;
}

FUNK(float16x8x4_t)
vld4q_f16(const float16_t *__a) {
  float16x8x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4v8hf(__a);
  ret.val[0] = __builtin_aarch64_get_qregxiv8hf(__o, 0);
  ret.val[1] = __builtin_aarch64_get_qregxiv8hf(__o, 1);
  ret.val[2] = __builtin_aarch64_get_qregxiv8hf(__o, 2);
  ret.val[3] = __builtin_aarch64_get_qregxiv8hf(__o, 3);
  return ret;
}

FUNK(float32x4x4_t)
vld4q_f32(const float32_t *__a) {
  float32x4x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4v4sf((const __builtin_aarch64_simd_sf *)__a);
  ret.val[0] = (float32x4_t)__builtin_aarch64_get_qregxiv4sf(__o, 0);
  ret.val[1] = (float32x4_t)__builtin_aarch64_get_qregxiv4sf(__o, 1);
  ret.val[2] = (float32x4_t)__builtin_aarch64_get_qregxiv4sf(__o, 2);
  ret.val[3] = (float32x4_t)__builtin_aarch64_get_qregxiv4sf(__o, 3);
  return ret;
}

FUNK(float64x2x4_t)
vld4q_f64(const float64_t *__a) {
  float64x2x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4v2df((const __builtin_aarch64_simd_df *)__a);
  ret.val[0] = (float64x2_t)__builtin_aarch64_get_qregxiv2df(__o, 0);
  ret.val[1] = (float64x2_t)__builtin_aarch64_get_qregxiv2df(__o, 1);
  ret.val[2] = (float64x2_t)__builtin_aarch64_get_qregxiv2df(__o, 2);
  ret.val[3] = (float64x2_t)__builtin_aarch64_get_qregxiv2df(__o, 3);
  return ret;
}

FUNK(poly64x2x4_t)
vld4q_p64(const poly64_t *__a) {
  poly64x2x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4v2di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (poly64x2_t)__builtin_aarch64_get_qregxiv2di_pss(__o, 0);
  ret.val[1] = (poly64x2_t)__builtin_aarch64_get_qregxiv2di_pss(__o, 1);
  ret.val[2] = (poly64x2_t)__builtin_aarch64_get_qregxiv2di_pss(__o, 2);
  ret.val[3] = (poly64x2_t)__builtin_aarch64_get_qregxiv2di_pss(__o, 3);
  return ret;
}

FUNK(int8x8x2_t)
vld2_dup_s8(const int8_t *__a) {
  int8x8x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2rv8qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (int8x8_t)__builtin_aarch64_get_dregoiv8qi(__o, 0);
  ret.val[1] = (int8x8_t)__builtin_aarch64_get_dregoiv8qi(__o, 1);
  return ret;
}

FUNK(int16x4x2_t)
vld2_dup_s16(const int16_t *__a) {
  int16x4x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2rv4hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (int16x4_t)__builtin_aarch64_get_dregoiv4hi(__o, 0);
  ret.val[1] = (int16x4_t)__builtin_aarch64_get_dregoiv4hi(__o, 1);
  return ret;
}

FUNK(int32x2x2_t)
vld2_dup_s32(const int32_t *__a) {
  int32x2x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2rv2si((const __builtin_aarch64_simd_si *)__a);
  ret.val[0] = (int32x2_t)__builtin_aarch64_get_dregoiv2si(__o, 0);
  ret.val[1] = (int32x2_t)__builtin_aarch64_get_dregoiv2si(__o, 1);
  return ret;
}

FUNK(float16x4x2_t)
vld2_dup_f16(const float16_t *__a) {
  float16x4x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2rv4hf((const __builtin_aarch64_simd_hf *)__a);
  ret.val[0] = __builtin_aarch64_get_dregoiv4hf(__o, 0);
  ret.val[1] = (float16x4_t)__builtin_aarch64_get_dregoiv4hf(__o, 1);
  return ret;
}

FUNK(float32x2x2_t)
vld2_dup_f32(const float32_t *__a) {
  float32x2x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2rv2sf((const __builtin_aarch64_simd_sf *)__a);
  ret.val[0] = (float32x2_t)__builtin_aarch64_get_dregoiv2sf(__o, 0);
  ret.val[1] = (float32x2_t)__builtin_aarch64_get_dregoiv2sf(__o, 1);
  return ret;
}

FUNK(float64x1x2_t)
vld2_dup_f64(const float64_t *__a) {
  float64x1x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2rdf((const __builtin_aarch64_simd_df *)__a);
  ret.val[0] = (float64x1_t){__builtin_aarch64_get_dregoidf(__o, 0)};
  ret.val[1] = (float64x1_t){__builtin_aarch64_get_dregoidf(__o, 1)};
  return ret;
}

FUNK(uint8x8x2_t)
vld2_dup_u8(const uint8_t *__a) {
  uint8x8x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2rv8qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (uint8x8_t)__builtin_aarch64_get_dregoiv8qi(__o, 0);
  ret.val[1] = (uint8x8_t)__builtin_aarch64_get_dregoiv8qi(__o, 1);
  return ret;
}

FUNK(uint16x4x2_t)
vld2_dup_u16(const uint16_t *__a) {
  uint16x4x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2rv4hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (uint16x4_t)__builtin_aarch64_get_dregoiv4hi(__o, 0);
  ret.val[1] = (uint16x4_t)__builtin_aarch64_get_dregoiv4hi(__o, 1);
  return ret;
}

FUNK(uint32x2x2_t)
vld2_dup_u32(const uint32_t *__a) {
  uint32x2x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2rv2si((const __builtin_aarch64_simd_si *)__a);
  ret.val[0] = (uint32x2_t)__builtin_aarch64_get_dregoiv2si(__o, 0);
  ret.val[1] = (uint32x2_t)__builtin_aarch64_get_dregoiv2si(__o, 1);
  return ret;
}

FUNK(poly8x8x2_t)
vld2_dup_p8(const poly8_t *__a) {
  poly8x8x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2rv8qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (poly8x8_t)__builtin_aarch64_get_dregoiv8qi(__o, 0);
  ret.val[1] = (poly8x8_t)__builtin_aarch64_get_dregoiv8qi(__o, 1);
  return ret;
}

FUNK(poly16x4x2_t)
vld2_dup_p16(const poly16_t *__a) {
  poly16x4x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2rv4hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (poly16x4_t)__builtin_aarch64_get_dregoiv4hi(__o, 0);
  ret.val[1] = (poly16x4_t)__builtin_aarch64_get_dregoiv4hi(__o, 1);
  return ret;
}

FUNK(poly64x1x2_t)
vld2_dup_p64(const poly64_t *__a) {
  poly64x1x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2rv2di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (poly64x1_t)__builtin_aarch64_get_dregoidi_pss(__o, 0);
  ret.val[1] = (poly64x1_t)__builtin_aarch64_get_dregoidi_pss(__o, 1);
  return ret;
}

FUNK(int64x1x2_t)
vld2_dup_s64(const int64_t *__a) {
  int64x1x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2rdi((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (int64x1_t)__builtin_aarch64_get_dregoidi(__o, 0);
  ret.val[1] = (int64x1_t)__builtin_aarch64_get_dregoidi(__o, 1);
  return ret;
}

FUNK(uint64x1x2_t)
vld2_dup_u64(const uint64_t *__a) {
  uint64x1x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2rdi((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (uint64x1_t)__builtin_aarch64_get_dregoidi(__o, 0);
  ret.val[1] = (uint64x1_t)__builtin_aarch64_get_dregoidi(__o, 1);
  return ret;
}

FUNK(int8x16x2_t)
vld2q_dup_s8(const int8_t *__a) {
  int8x16x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2rv16qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (int8x16_t)__builtin_aarch64_get_qregoiv16qi(__o, 0);
  ret.val[1] = (int8x16_t)__builtin_aarch64_get_qregoiv16qi(__o, 1);
  return ret;
}

FUNK(poly8x16x2_t)
vld2q_dup_p8(const poly8_t *__a) {
  poly8x16x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2rv16qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (poly8x16_t)__builtin_aarch64_get_qregoiv16qi(__o, 0);
  ret.val[1] = (poly8x16_t)__builtin_aarch64_get_qregoiv16qi(__o, 1);
  return ret;
}

FUNK(int16x8x2_t)
vld2q_dup_s16(const int16_t *__a) {
  int16x8x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2rv8hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (int16x8_t)__builtin_aarch64_get_qregoiv8hi(__o, 0);
  ret.val[1] = (int16x8_t)__builtin_aarch64_get_qregoiv8hi(__o, 1);
  return ret;
}

FUNK(poly16x8x2_t)
vld2q_dup_p16(const poly16_t *__a) {
  poly16x8x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2rv8hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (poly16x8_t)__builtin_aarch64_get_qregoiv8hi(__o, 0);
  ret.val[1] = (poly16x8_t)__builtin_aarch64_get_qregoiv8hi(__o, 1);
  return ret;
}

FUNK(int32x4x2_t)
vld2q_dup_s32(const int32_t *__a) {
  int32x4x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2rv4si((const __builtin_aarch64_simd_si *)__a);
  ret.val[0] = (int32x4_t)__builtin_aarch64_get_qregoiv4si(__o, 0);
  ret.val[1] = (int32x4_t)__builtin_aarch64_get_qregoiv4si(__o, 1);
  return ret;
}

FUNK(int64x2x2_t)
vld2q_dup_s64(const int64_t *__a) {
  int64x2x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2rv2di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (int64x2_t)__builtin_aarch64_get_qregoiv2di(__o, 0);
  ret.val[1] = (int64x2_t)__builtin_aarch64_get_qregoiv2di(__o, 1);
  return ret;
}

FUNK(uint8x16x2_t)
vld2q_dup_u8(const uint8_t *__a) {
  uint8x16x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2rv16qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (uint8x16_t)__builtin_aarch64_get_qregoiv16qi(__o, 0);
  ret.val[1] = (uint8x16_t)__builtin_aarch64_get_qregoiv16qi(__o, 1);
  return ret;
}

FUNK(uint16x8x2_t)
vld2q_dup_u16(const uint16_t *__a) {
  uint16x8x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2rv8hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (uint16x8_t)__builtin_aarch64_get_qregoiv8hi(__o, 0);
  ret.val[1] = (uint16x8_t)__builtin_aarch64_get_qregoiv8hi(__o, 1);
  return ret;
}

FUNK(uint32x4x2_t)
vld2q_dup_u32(const uint32_t *__a) {
  uint32x4x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2rv4si((const __builtin_aarch64_simd_si *)__a);
  ret.val[0] = (uint32x4_t)__builtin_aarch64_get_qregoiv4si(__o, 0);
  ret.val[1] = (uint32x4_t)__builtin_aarch64_get_qregoiv4si(__o, 1);
  return ret;
}

FUNK(uint64x2x2_t)
vld2q_dup_u64(const uint64_t *__a) {
  uint64x2x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2rv2di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (uint64x2_t)__builtin_aarch64_get_qregoiv2di(__o, 0);
  ret.val[1] = (uint64x2_t)__builtin_aarch64_get_qregoiv2di(__o, 1);
  return ret;
}

FUNK(float16x8x2_t)
vld2q_dup_f16(const float16_t *__a) {
  float16x8x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2rv8hf((const __builtin_aarch64_simd_hf *)__a);
  ret.val[0] = (float16x8_t)__builtin_aarch64_get_qregoiv8hf(__o, 0);
  ret.val[1] = __builtin_aarch64_get_qregoiv8hf(__o, 1);
  return ret;
}

FUNK(float32x4x2_t)
vld2q_dup_f32(const float32_t *__a) {
  float32x4x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2rv4sf((const __builtin_aarch64_simd_sf *)__a);
  ret.val[0] = (float32x4_t)__builtin_aarch64_get_qregoiv4sf(__o, 0);
  ret.val[1] = (float32x4_t)__builtin_aarch64_get_qregoiv4sf(__o, 1);
  return ret;
}

FUNK(float64x2x2_t)
vld2q_dup_f64(const float64_t *__a) {
  float64x2x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2rv2df((const __builtin_aarch64_simd_df *)__a);
  ret.val[0] = (float64x2_t)__builtin_aarch64_get_qregoiv2df(__o, 0);
  ret.val[1] = (float64x2_t)__builtin_aarch64_get_qregoiv2df(__o, 1);
  return ret;
}

FUNK(poly64x2x2_t)
vld2q_dup_p64(const poly64_t *__a) {
  poly64x2x2_t ret;
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_ld2rv2di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (poly64x2_t)__builtin_aarch64_get_qregoiv2di_pss(__o, 0);
  ret.val[1] = (poly64x2_t)__builtin_aarch64_get_qregoiv2di_pss(__o, 1);
  return ret;
}

FUNK(int64x1x3_t)
vld3_dup_s64(const int64_t *__a) {
  int64x1x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3rdi((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (int64x1_t)__builtin_aarch64_get_dregcidi(__o, 0);
  ret.val[1] = (int64x1_t)__builtin_aarch64_get_dregcidi(__o, 1);
  ret.val[2] = (int64x1_t)__builtin_aarch64_get_dregcidi(__o, 2);
  return ret;
}

FUNK(uint64x1x3_t)
vld3_dup_u64(const uint64_t *__a) {
  uint64x1x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3rdi((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (uint64x1_t)__builtin_aarch64_get_dregcidi(__o, 0);
  ret.val[1] = (uint64x1_t)__builtin_aarch64_get_dregcidi(__o, 1);
  ret.val[2] = (uint64x1_t)__builtin_aarch64_get_dregcidi(__o, 2);
  return ret;
}

FUNK(float64x1x3_t)
vld3_dup_f64(const float64_t *__a) {
  float64x1x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3rdf((const __builtin_aarch64_simd_df *)__a);
  ret.val[0] = (float64x1_t){__builtin_aarch64_get_dregcidf(__o, 0)};
  ret.val[1] = (float64x1_t){__builtin_aarch64_get_dregcidf(__o, 1)};
  ret.val[2] = (float64x1_t){__builtin_aarch64_get_dregcidf(__o, 2)};
  return ret;
}

FUNK(int8x8x3_t)
vld3_dup_s8(const int8_t *__a) {
  int8x8x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3rv8qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (int8x8_t)__builtin_aarch64_get_dregciv8qi(__o, 0);
  ret.val[1] = (int8x8_t)__builtin_aarch64_get_dregciv8qi(__o, 1);
  ret.val[2] = (int8x8_t)__builtin_aarch64_get_dregciv8qi(__o, 2);
  return ret;
}

FUNK(poly8x8x3_t)
vld3_dup_p8(const poly8_t *__a) {
  poly8x8x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3rv8qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (poly8x8_t)__builtin_aarch64_get_dregciv8qi(__o, 0);
  ret.val[1] = (poly8x8_t)__builtin_aarch64_get_dregciv8qi(__o, 1);
  ret.val[2] = (poly8x8_t)__builtin_aarch64_get_dregciv8qi(__o, 2);
  return ret;
}

FUNK(int16x4x3_t)
vld3_dup_s16(const int16_t *__a) {
  int16x4x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3rv4hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (int16x4_t)__builtin_aarch64_get_dregciv4hi(__o, 0);
  ret.val[1] = (int16x4_t)__builtin_aarch64_get_dregciv4hi(__o, 1);
  ret.val[2] = (int16x4_t)__builtin_aarch64_get_dregciv4hi(__o, 2);
  return ret;
}

FUNK(poly16x4x3_t)
vld3_dup_p16(const poly16_t *__a) {
  poly16x4x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3rv4hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (poly16x4_t)__builtin_aarch64_get_dregciv4hi(__o, 0);
  ret.val[1] = (poly16x4_t)__builtin_aarch64_get_dregciv4hi(__o, 1);
  ret.val[2] = (poly16x4_t)__builtin_aarch64_get_dregciv4hi(__o, 2);
  return ret;
}

FUNK(int32x2x3_t)
vld3_dup_s32(const int32_t *__a) {
  int32x2x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3rv2si((const __builtin_aarch64_simd_si *)__a);
  ret.val[0] = (int32x2_t)__builtin_aarch64_get_dregciv2si(__o, 0);
  ret.val[1] = (int32x2_t)__builtin_aarch64_get_dregciv2si(__o, 1);
  ret.val[2] = (int32x2_t)__builtin_aarch64_get_dregciv2si(__o, 2);
  return ret;
}

FUNK(uint8x8x3_t)
vld3_dup_u8(const uint8_t *__a) {
  uint8x8x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3rv8qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (uint8x8_t)__builtin_aarch64_get_dregciv8qi(__o, 0);
  ret.val[1] = (uint8x8_t)__builtin_aarch64_get_dregciv8qi(__o, 1);
  ret.val[2] = (uint8x8_t)__builtin_aarch64_get_dregciv8qi(__o, 2);
  return ret;
}

FUNK(uint16x4x3_t)
vld3_dup_u16(const uint16_t *__a) {
  uint16x4x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3rv4hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (uint16x4_t)__builtin_aarch64_get_dregciv4hi(__o, 0);
  ret.val[1] = (uint16x4_t)__builtin_aarch64_get_dregciv4hi(__o, 1);
  ret.val[2] = (uint16x4_t)__builtin_aarch64_get_dregciv4hi(__o, 2);
  return ret;
}

FUNK(uint32x2x3_t)
vld3_dup_u32(const uint32_t *__a) {
  uint32x2x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3rv2si((const __builtin_aarch64_simd_si *)__a);
  ret.val[0] = (uint32x2_t)__builtin_aarch64_get_dregciv2si(__o, 0);
  ret.val[1] = (uint32x2_t)__builtin_aarch64_get_dregciv2si(__o, 1);
  ret.val[2] = (uint32x2_t)__builtin_aarch64_get_dregciv2si(__o, 2);
  return ret;
}

FUNK(float16x4x3_t)
vld3_dup_f16(const float16_t *__a) {
  float16x4x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3rv4hf((const __builtin_aarch64_simd_hf *)__a);
  ret.val[0] = (float16x4_t)__builtin_aarch64_get_dregciv4hf(__o, 0);
  ret.val[1] = (float16x4_t)__builtin_aarch64_get_dregciv4hf(__o, 1);
  ret.val[2] = (float16x4_t)__builtin_aarch64_get_dregciv4hf(__o, 2);
  return ret;
}

FUNK(float32x2x3_t)
vld3_dup_f32(const float32_t *__a) {
  float32x2x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3rv2sf((const __builtin_aarch64_simd_sf *)__a);
  ret.val[0] = (float32x2_t)__builtin_aarch64_get_dregciv2sf(__o, 0);
  ret.val[1] = (float32x2_t)__builtin_aarch64_get_dregciv2sf(__o, 1);
  ret.val[2] = (float32x2_t)__builtin_aarch64_get_dregciv2sf(__o, 2);
  return ret;
}

FUNK(poly64x1x3_t)
vld3_dup_p64(const poly64_t *__a) {
  poly64x1x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3rv2di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (poly64x1_t)__builtin_aarch64_get_dregcidi_pss(__o, 0);
  ret.val[1] = (poly64x1_t)__builtin_aarch64_get_dregcidi_pss(__o, 1);
  ret.val[2] = (poly64x1_t)__builtin_aarch64_get_dregcidi_pss(__o, 2);
  return ret;
}

FUNK(int8x16x3_t)
vld3q_dup_s8(const int8_t *__a) {
  int8x16x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3rv16qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (int8x16_t)__builtin_aarch64_get_qregciv16qi(__o, 0);
  ret.val[1] = (int8x16_t)__builtin_aarch64_get_qregciv16qi(__o, 1);
  ret.val[2] = (int8x16_t)__builtin_aarch64_get_qregciv16qi(__o, 2);
  return ret;
}

FUNK(poly8x16x3_t)
vld3q_dup_p8(const poly8_t *__a) {
  poly8x16x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3rv16qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (poly8x16_t)__builtin_aarch64_get_qregciv16qi(__o, 0);
  ret.val[1] = (poly8x16_t)__builtin_aarch64_get_qregciv16qi(__o, 1);
  ret.val[2] = (poly8x16_t)__builtin_aarch64_get_qregciv16qi(__o, 2);
  return ret;
}

FUNK(int16x8x3_t)
vld3q_dup_s16(const int16_t *__a) {
  int16x8x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3rv8hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (int16x8_t)__builtin_aarch64_get_qregciv8hi(__o, 0);
  ret.val[1] = (int16x8_t)__builtin_aarch64_get_qregciv8hi(__o, 1);
  ret.val[2] = (int16x8_t)__builtin_aarch64_get_qregciv8hi(__o, 2);
  return ret;
}

FUNK(poly16x8x3_t)
vld3q_dup_p16(const poly16_t *__a) {
  poly16x8x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3rv8hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (poly16x8_t)__builtin_aarch64_get_qregciv8hi(__o, 0);
  ret.val[1] = (poly16x8_t)__builtin_aarch64_get_qregciv8hi(__o, 1);
  ret.val[2] = (poly16x8_t)__builtin_aarch64_get_qregciv8hi(__o, 2);
  return ret;
}

FUNK(int32x4x3_t)
vld3q_dup_s32(const int32_t *__a) {
  int32x4x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3rv4si((const __builtin_aarch64_simd_si *)__a);
  ret.val[0] = (int32x4_t)__builtin_aarch64_get_qregciv4si(__o, 0);
  ret.val[1] = (int32x4_t)__builtin_aarch64_get_qregciv4si(__o, 1);
  ret.val[2] = (int32x4_t)__builtin_aarch64_get_qregciv4si(__o, 2);
  return ret;
}

FUNK(int64x2x3_t)
vld3q_dup_s64(const int64_t *__a) {
  int64x2x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3rv2di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (int64x2_t)__builtin_aarch64_get_qregciv2di(__o, 0);
  ret.val[1] = (int64x2_t)__builtin_aarch64_get_qregciv2di(__o, 1);
  ret.val[2] = (int64x2_t)__builtin_aarch64_get_qregciv2di(__o, 2);
  return ret;
}

FUNK(uint8x16x3_t)
vld3q_dup_u8(const uint8_t *__a) {
  uint8x16x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3rv16qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (uint8x16_t)__builtin_aarch64_get_qregciv16qi(__o, 0);
  ret.val[1] = (uint8x16_t)__builtin_aarch64_get_qregciv16qi(__o, 1);
  ret.val[2] = (uint8x16_t)__builtin_aarch64_get_qregciv16qi(__o, 2);
  return ret;
}

FUNK(uint16x8x3_t)
vld3q_dup_u16(const uint16_t *__a) {
  uint16x8x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3rv8hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (uint16x8_t)__builtin_aarch64_get_qregciv8hi(__o, 0);
  ret.val[1] = (uint16x8_t)__builtin_aarch64_get_qregciv8hi(__o, 1);
  ret.val[2] = (uint16x8_t)__builtin_aarch64_get_qregciv8hi(__o, 2);
  return ret;
}

FUNK(uint32x4x3_t)
vld3q_dup_u32(const uint32_t *__a) {
  uint32x4x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3rv4si((const __builtin_aarch64_simd_si *)__a);
  ret.val[0] = (uint32x4_t)__builtin_aarch64_get_qregciv4si(__o, 0);
  ret.val[1] = (uint32x4_t)__builtin_aarch64_get_qregciv4si(__o, 1);
  ret.val[2] = (uint32x4_t)__builtin_aarch64_get_qregciv4si(__o, 2);
  return ret;
}

FUNK(uint64x2x3_t)
vld3q_dup_u64(const uint64_t *__a) {
  uint64x2x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3rv2di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (uint64x2_t)__builtin_aarch64_get_qregciv2di(__o, 0);
  ret.val[1] = (uint64x2_t)__builtin_aarch64_get_qregciv2di(__o, 1);
  ret.val[2] = (uint64x2_t)__builtin_aarch64_get_qregciv2di(__o, 2);
  return ret;
}

FUNK(float16x8x3_t)
vld3q_dup_f16(const float16_t *__a) {
  float16x8x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3rv8hf((const __builtin_aarch64_simd_hf *)__a);
  ret.val[0] = (float16x8_t)__builtin_aarch64_get_qregciv8hf(__o, 0);
  ret.val[1] = (float16x8_t)__builtin_aarch64_get_qregciv8hf(__o, 1);
  ret.val[2] = (float16x8_t)__builtin_aarch64_get_qregciv8hf(__o, 2);
  return ret;
}

FUNK(float32x4x3_t)
vld3q_dup_f32(const float32_t *__a) {
  float32x4x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3rv4sf((const __builtin_aarch64_simd_sf *)__a);
  ret.val[0] = (float32x4_t)__builtin_aarch64_get_qregciv4sf(__o, 0);
  ret.val[1] = (float32x4_t)__builtin_aarch64_get_qregciv4sf(__o, 1);
  ret.val[2] = (float32x4_t)__builtin_aarch64_get_qregciv4sf(__o, 2);
  return ret;
}

FUNK(float64x2x3_t)
vld3q_dup_f64(const float64_t *__a) {
  float64x2x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3rv2df((const __builtin_aarch64_simd_df *)__a);
  ret.val[0] = (float64x2_t)__builtin_aarch64_get_qregciv2df(__o, 0);
  ret.val[1] = (float64x2_t)__builtin_aarch64_get_qregciv2df(__o, 1);
  ret.val[2] = (float64x2_t)__builtin_aarch64_get_qregciv2df(__o, 2);
  return ret;
}

FUNK(poly64x2x3_t)
vld3q_dup_p64(const poly64_t *__a) {
  poly64x2x3_t ret;
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_ld3rv2di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (poly64x2_t)__builtin_aarch64_get_qregciv2di_pss(__o, 0);
  ret.val[1] = (poly64x2_t)__builtin_aarch64_get_qregciv2di_pss(__o, 1);
  ret.val[2] = (poly64x2_t)__builtin_aarch64_get_qregciv2di_pss(__o, 2);
  return ret;
}

FUNK(int64x1x4_t)
vld4_dup_s64(const int64_t *__a) {
  int64x1x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4rdi((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (int64x1_t)__builtin_aarch64_get_dregxidi(__o, 0);
  ret.val[1] = (int64x1_t)__builtin_aarch64_get_dregxidi(__o, 1);
  ret.val[2] = (int64x1_t)__builtin_aarch64_get_dregxidi(__o, 2);
  ret.val[3] = (int64x1_t)__builtin_aarch64_get_dregxidi(__o, 3);
  return ret;
}

FUNK(uint64x1x4_t)
vld4_dup_u64(const uint64_t *__a) {
  uint64x1x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4rdi((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (uint64x1_t)__builtin_aarch64_get_dregxidi(__o, 0);
  ret.val[1] = (uint64x1_t)__builtin_aarch64_get_dregxidi(__o, 1);
  ret.val[2] = (uint64x1_t)__builtin_aarch64_get_dregxidi(__o, 2);
  ret.val[3] = (uint64x1_t)__builtin_aarch64_get_dregxidi(__o, 3);
  return ret;
}

FUNK(float64x1x4_t)
vld4_dup_f64(const float64_t *__a) {
  float64x1x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4rdf((const __builtin_aarch64_simd_df *)__a);
  ret.val[0] = (float64x1_t){__builtin_aarch64_get_dregxidf(__o, 0)};
  ret.val[1] = (float64x1_t){__builtin_aarch64_get_dregxidf(__o, 1)};
  ret.val[2] = (float64x1_t){__builtin_aarch64_get_dregxidf(__o, 2)};
  ret.val[3] = (float64x1_t){__builtin_aarch64_get_dregxidf(__o, 3)};
  return ret;
}

FUNK(int8x8x4_t)
vld4_dup_s8(const int8_t *__a) {
  int8x8x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4rv8qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (int8x8_t)__builtin_aarch64_get_dregxiv8qi(__o, 0);
  ret.val[1] = (int8x8_t)__builtin_aarch64_get_dregxiv8qi(__o, 1);
  ret.val[2] = (int8x8_t)__builtin_aarch64_get_dregxiv8qi(__o, 2);
  ret.val[3] = (int8x8_t)__builtin_aarch64_get_dregxiv8qi(__o, 3);
  return ret;
}

FUNK(poly8x8x4_t)
vld4_dup_p8(const poly8_t *__a) {
  poly8x8x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4rv8qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (poly8x8_t)__builtin_aarch64_get_dregxiv8qi(__o, 0);
  ret.val[1] = (poly8x8_t)__builtin_aarch64_get_dregxiv8qi(__o, 1);
  ret.val[2] = (poly8x8_t)__builtin_aarch64_get_dregxiv8qi(__o, 2);
  ret.val[3] = (poly8x8_t)__builtin_aarch64_get_dregxiv8qi(__o, 3);
  return ret;
}

FUNK(int16x4x4_t)
vld4_dup_s16(const int16_t *__a) {
  int16x4x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4rv4hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (int16x4_t)__builtin_aarch64_get_dregxiv4hi(__o, 0);
  ret.val[1] = (int16x4_t)__builtin_aarch64_get_dregxiv4hi(__o, 1);
  ret.val[2] = (int16x4_t)__builtin_aarch64_get_dregxiv4hi(__o, 2);
  ret.val[3] = (int16x4_t)__builtin_aarch64_get_dregxiv4hi(__o, 3);
  return ret;
}

FUNK(poly16x4x4_t)
vld4_dup_p16(const poly16_t *__a) {
  poly16x4x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4rv4hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (poly16x4_t)__builtin_aarch64_get_dregxiv4hi(__o, 0);
  ret.val[1] = (poly16x4_t)__builtin_aarch64_get_dregxiv4hi(__o, 1);
  ret.val[2] = (poly16x4_t)__builtin_aarch64_get_dregxiv4hi(__o, 2);
  ret.val[3] = (poly16x4_t)__builtin_aarch64_get_dregxiv4hi(__o, 3);
  return ret;
}

FUNK(int32x2x4_t)
vld4_dup_s32(const int32_t *__a) {
  int32x2x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4rv2si((const __builtin_aarch64_simd_si *)__a);
  ret.val[0] = (int32x2_t)__builtin_aarch64_get_dregxiv2si(__o, 0);
  ret.val[1] = (int32x2_t)__builtin_aarch64_get_dregxiv2si(__o, 1);
  ret.val[2] = (int32x2_t)__builtin_aarch64_get_dregxiv2si(__o, 2);
  ret.val[3] = (int32x2_t)__builtin_aarch64_get_dregxiv2si(__o, 3);
  return ret;
}

FUNK(uint8x8x4_t)
vld4_dup_u8(const uint8_t *__a) {
  uint8x8x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4rv8qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (uint8x8_t)__builtin_aarch64_get_dregxiv8qi(__o, 0);
  ret.val[1] = (uint8x8_t)__builtin_aarch64_get_dregxiv8qi(__o, 1);
  ret.val[2] = (uint8x8_t)__builtin_aarch64_get_dregxiv8qi(__o, 2);
  ret.val[3] = (uint8x8_t)__builtin_aarch64_get_dregxiv8qi(__o, 3);
  return ret;
}

FUNK(uint16x4x4_t)
vld4_dup_u16(const uint16_t *__a) {
  uint16x4x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4rv4hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (uint16x4_t)__builtin_aarch64_get_dregxiv4hi(__o, 0);
  ret.val[1] = (uint16x4_t)__builtin_aarch64_get_dregxiv4hi(__o, 1);
  ret.val[2] = (uint16x4_t)__builtin_aarch64_get_dregxiv4hi(__o, 2);
  ret.val[3] = (uint16x4_t)__builtin_aarch64_get_dregxiv4hi(__o, 3);
  return ret;
}

FUNK(uint32x2x4_t)
vld4_dup_u32(const uint32_t *__a) {
  uint32x2x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4rv2si((const __builtin_aarch64_simd_si *)__a);
  ret.val[0] = (uint32x2_t)__builtin_aarch64_get_dregxiv2si(__o, 0);
  ret.val[1] = (uint32x2_t)__builtin_aarch64_get_dregxiv2si(__o, 1);
  ret.val[2] = (uint32x2_t)__builtin_aarch64_get_dregxiv2si(__o, 2);
  ret.val[3] = (uint32x2_t)__builtin_aarch64_get_dregxiv2si(__o, 3);
  return ret;
}

FUNK(float16x4x4_t)
vld4_dup_f16(const float16_t *__a) {
  float16x4x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4rv4hf((const __builtin_aarch64_simd_hf *)__a);
  ret.val[0] = (float16x4_t)__builtin_aarch64_get_dregxiv4hf(__o, 0);
  ret.val[1] = (float16x4_t)__builtin_aarch64_get_dregxiv4hf(__o, 1);
  ret.val[2] = (float16x4_t)__builtin_aarch64_get_dregxiv4hf(__o, 2);
  ret.val[3] = (float16x4_t)__builtin_aarch64_get_dregxiv4hf(__o, 3);
  return ret;
}

FUNK(float32x2x4_t)
vld4_dup_f32(const float32_t *__a) {
  float32x2x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4rv2sf((const __builtin_aarch64_simd_sf *)__a);
  ret.val[0] = (float32x2_t)__builtin_aarch64_get_dregxiv2sf(__o, 0);
  ret.val[1] = (float32x2_t)__builtin_aarch64_get_dregxiv2sf(__o, 1);
  ret.val[2] = (float32x2_t)__builtin_aarch64_get_dregxiv2sf(__o, 2);
  ret.val[3] = (float32x2_t)__builtin_aarch64_get_dregxiv2sf(__o, 3);
  return ret;
}

FUNK(poly64x1x4_t)
vld4_dup_p64(const poly64_t *__a) {
  poly64x1x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4rv2di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (poly64x1_t)__builtin_aarch64_get_dregxidi_pss(__o, 0);
  ret.val[1] = (poly64x1_t)__builtin_aarch64_get_dregxidi_pss(__o, 1);
  ret.val[2] = (poly64x1_t)__builtin_aarch64_get_dregxidi_pss(__o, 2);
  ret.val[3] = (poly64x1_t)__builtin_aarch64_get_dregxidi_pss(__o, 3);
  return ret;
}

FUNK(int8x16x4_t)
vld4q_dup_s8(const int8_t *__a) {
  int8x16x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4rv16qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (int8x16_t)__builtin_aarch64_get_qregxiv16qi(__o, 0);
  ret.val[1] = (int8x16_t)__builtin_aarch64_get_qregxiv16qi(__o, 1);
  ret.val[2] = (int8x16_t)__builtin_aarch64_get_qregxiv16qi(__o, 2);
  ret.val[3] = (int8x16_t)__builtin_aarch64_get_qregxiv16qi(__o, 3);
  return ret;
}

FUNK(poly8x16x4_t)
vld4q_dup_p8(const poly8_t *__a) {
  poly8x16x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4rv16qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (poly8x16_t)__builtin_aarch64_get_qregxiv16qi(__o, 0);
  ret.val[1] = (poly8x16_t)__builtin_aarch64_get_qregxiv16qi(__o, 1);
  ret.val[2] = (poly8x16_t)__builtin_aarch64_get_qregxiv16qi(__o, 2);
  ret.val[3] = (poly8x16_t)__builtin_aarch64_get_qregxiv16qi(__o, 3);
  return ret;
}

FUNK(int16x8x4_t)
vld4q_dup_s16(const int16_t *__a) {
  int16x8x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4rv8hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (int16x8_t)__builtin_aarch64_get_qregxiv8hi(__o, 0);
  ret.val[1] = (int16x8_t)__builtin_aarch64_get_qregxiv8hi(__o, 1);
  ret.val[2] = (int16x8_t)__builtin_aarch64_get_qregxiv8hi(__o, 2);
  ret.val[3] = (int16x8_t)__builtin_aarch64_get_qregxiv8hi(__o, 3);
  return ret;
}

FUNK(poly16x8x4_t)
vld4q_dup_p16(const poly16_t *__a) {
  poly16x8x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4rv8hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (poly16x8_t)__builtin_aarch64_get_qregxiv8hi(__o, 0);
  ret.val[1] = (poly16x8_t)__builtin_aarch64_get_qregxiv8hi(__o, 1);
  ret.val[2] = (poly16x8_t)__builtin_aarch64_get_qregxiv8hi(__o, 2);
  ret.val[3] = (poly16x8_t)__builtin_aarch64_get_qregxiv8hi(__o, 3);
  return ret;
}

FUNK(int32x4x4_t)
vld4q_dup_s32(const int32_t *__a) {
  int32x4x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4rv4si((const __builtin_aarch64_simd_si *)__a);
  ret.val[0] = (int32x4_t)__builtin_aarch64_get_qregxiv4si(__o, 0);
  ret.val[1] = (int32x4_t)__builtin_aarch64_get_qregxiv4si(__o, 1);
  ret.val[2] = (int32x4_t)__builtin_aarch64_get_qregxiv4si(__o, 2);
  ret.val[3] = (int32x4_t)__builtin_aarch64_get_qregxiv4si(__o, 3);
  return ret;
}

FUNK(int64x2x4_t)
vld4q_dup_s64(const int64_t *__a) {
  int64x2x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4rv2di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (int64x2_t)__builtin_aarch64_get_qregxiv2di(__o, 0);
  ret.val[1] = (int64x2_t)__builtin_aarch64_get_qregxiv2di(__o, 1);
  ret.val[2] = (int64x2_t)__builtin_aarch64_get_qregxiv2di(__o, 2);
  ret.val[3] = (int64x2_t)__builtin_aarch64_get_qregxiv2di(__o, 3);
  return ret;
}

FUNK(uint8x16x4_t)
vld4q_dup_u8(const uint8_t *__a) {
  uint8x16x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4rv16qi((const __builtin_aarch64_simd_qi *)__a);
  ret.val[0] = (uint8x16_t)__builtin_aarch64_get_qregxiv16qi(__o, 0);
  ret.val[1] = (uint8x16_t)__builtin_aarch64_get_qregxiv16qi(__o, 1);
  ret.val[2] = (uint8x16_t)__builtin_aarch64_get_qregxiv16qi(__o, 2);
  ret.val[3] = (uint8x16_t)__builtin_aarch64_get_qregxiv16qi(__o, 3);
  return ret;
}

FUNK(uint16x8x4_t)
vld4q_dup_u16(const uint16_t *__a) {
  uint16x8x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4rv8hi((const __builtin_aarch64_simd_hi *)__a);
  ret.val[0] = (uint16x8_t)__builtin_aarch64_get_qregxiv8hi(__o, 0);
  ret.val[1] = (uint16x8_t)__builtin_aarch64_get_qregxiv8hi(__o, 1);
  ret.val[2] = (uint16x8_t)__builtin_aarch64_get_qregxiv8hi(__o, 2);
  ret.val[3] = (uint16x8_t)__builtin_aarch64_get_qregxiv8hi(__o, 3);
  return ret;
}

FUNK(uint32x4x4_t)
vld4q_dup_u32(const uint32_t *__a) {
  uint32x4x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4rv4si((const __builtin_aarch64_simd_si *)__a);
  ret.val[0] = (uint32x4_t)__builtin_aarch64_get_qregxiv4si(__o, 0);
  ret.val[1] = (uint32x4_t)__builtin_aarch64_get_qregxiv4si(__o, 1);
  ret.val[2] = (uint32x4_t)__builtin_aarch64_get_qregxiv4si(__o, 2);
  ret.val[3] = (uint32x4_t)__builtin_aarch64_get_qregxiv4si(__o, 3);
  return ret;
}

FUNK(uint64x2x4_t)
vld4q_dup_u64(const uint64_t *__a) {
  uint64x2x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4rv2di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (uint64x2_t)__builtin_aarch64_get_qregxiv2di(__o, 0);
  ret.val[1] = (uint64x2_t)__builtin_aarch64_get_qregxiv2di(__o, 1);
  ret.val[2] = (uint64x2_t)__builtin_aarch64_get_qregxiv2di(__o, 2);
  ret.val[3] = (uint64x2_t)__builtin_aarch64_get_qregxiv2di(__o, 3);
  return ret;
}

FUNK(float16x8x4_t)
vld4q_dup_f16(const float16_t *__a) {
  float16x8x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4rv8hf((const __builtin_aarch64_simd_hf *)__a);
  ret.val[0] = (float16x8_t)__builtin_aarch64_get_qregxiv8hf(__o, 0);
  ret.val[1] = (float16x8_t)__builtin_aarch64_get_qregxiv8hf(__o, 1);
  ret.val[2] = (float16x8_t)__builtin_aarch64_get_qregxiv8hf(__o, 2);
  ret.val[3] = (float16x8_t)__builtin_aarch64_get_qregxiv8hf(__o, 3);
  return ret;
}

FUNK(float32x4x4_t)
vld4q_dup_f32(const float32_t *__a) {
  float32x4x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4rv4sf((const __builtin_aarch64_simd_sf *)__a);
  ret.val[0] = (float32x4_t)__builtin_aarch64_get_qregxiv4sf(__o, 0);
  ret.val[1] = (float32x4_t)__builtin_aarch64_get_qregxiv4sf(__o, 1);
  ret.val[2] = (float32x4_t)__builtin_aarch64_get_qregxiv4sf(__o, 2);
  ret.val[3] = (float32x4_t)__builtin_aarch64_get_qregxiv4sf(__o, 3);
  return ret;
}

FUNK(float64x2x4_t)
vld4q_dup_f64(const float64_t *__a) {
  float64x2x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4rv2df((const __builtin_aarch64_simd_df *)__a);
  ret.val[0] = (float64x2_t)__builtin_aarch64_get_qregxiv2df(__o, 0);
  ret.val[1] = (float64x2_t)__builtin_aarch64_get_qregxiv2df(__o, 1);
  ret.val[2] = (float64x2_t)__builtin_aarch64_get_qregxiv2df(__o, 2);
  ret.val[3] = (float64x2_t)__builtin_aarch64_get_qregxiv2df(__o, 3);
  return ret;
}

FUNK(poly64x2x4_t)
vld4q_dup_p64(const poly64_t *__a) {
  poly64x2x4_t ret;
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_ld4rv2di((const __builtin_aarch64_simd_di *)__a);
  ret.val[0] = (poly64x2_t)__builtin_aarch64_get_qregxiv2di_pss(__o, 0);
  ret.val[1] = (poly64x2_t)__builtin_aarch64_get_qregxiv2di_pss(__o, 1);
  ret.val[2] = (poly64x2_t)__builtin_aarch64_get_qregxiv2di_pss(__o, 2);
  ret.val[3] = (poly64x2_t)__builtin_aarch64_get_qregxiv2di_pss(__o, 3);
  return ret;
}

#define __LD2_LANE_FUNC(intype, vectype, largetype, ptrtype, mode, qmode,     \
                        ptrmode, funcsuffix, signedtype)                      \
  FUNK(intype)                                                                \
  vld2_lane_##funcsuffix(const ptrtype *__ptr, intype __b, const int __c) {   \
    __builtin_aarch64_simd_oi __o;                                            \
    largetype __temp;                                                         \
    __temp.val[0] =                                                           \
        vcombine_##funcsuffix(__b.val[0], vcreate_##funcsuffix(0));           \
    __temp.val[1] =                                                           \
        vcombine_##funcsuffix(__b.val[1], vcreate_##funcsuffix(0));           \
    __o = __builtin_aarch64_set_qregoi##qmode(__o, (signedtype)__temp.val[0], \
                                              0);                             \
    __o = __builtin_aarch64_set_qregoi##qmode(__o, (signedtype)__temp.val[1], \
                                              1);                             \
    __o = __builtin_aarch64_ld2_lane##mode(                                   \
        (__builtin_aarch64_simd_##ptrmode *)__ptr, __o, __c);                 \
    __b.val[0] = (vectype)__builtin_aarch64_get_dregoidi(__o, 0);             \
    __b.val[1] = (vectype)__builtin_aarch64_get_dregoidi(__o, 1);             \
    return __b;                                                               \
  }

__LD2_LANE_FUNC(float16x4x2_t, float16x4_t, float16x8x2_t, float16_t, v4hf,
                v8hf, hf, f16, float16x8_t)
__LD2_LANE_FUNC(float32x2x2_t, float32x2_t, float32x4x2_t, float32_t, v2sf,
                v4sf, sf, f32, float32x4_t)
__LD2_LANE_FUNC(float64x1x2_t, float64x1_t, float64x2x2_t, float64_t, df, v2df,
                df, f64, float64x2_t)
__LD2_LANE_FUNC(poly8x8x2_t, poly8x8_t, poly8x16x2_t, poly8_t, v8qi, v16qi, qi,
                p8, int8x16_t)
__LD2_LANE_FUNC(poly16x4x2_t, poly16x4_t, poly16x8x2_t, poly16_t, v4hi, v8hi,
                hi, p16, int16x8_t)
__LD2_LANE_FUNC(poly64x1x2_t, poly64x1_t, poly64x2x2_t, poly64_t, di, v2di_ssps,
                di, p64, poly64x2_t)
__LD2_LANE_FUNC(int8x8x2_t, int8x8_t, int8x16x2_t, int8_t, v8qi, v16qi, qi, s8,
                int8x16_t)
__LD2_LANE_FUNC(int16x4x2_t, int16x4_t, int16x8x2_t, int16_t, v4hi, v8hi, hi,
                s16, int16x8_t)
__LD2_LANE_FUNC(int32x2x2_t, int32x2_t, int32x4x2_t, int32_t, v2si, v4si, si,
                s32, int32x4_t)
__LD2_LANE_FUNC(int64x1x2_t, int64x1_t, int64x2x2_t, int64_t, di, v2di, di, s64,
                int64x2_t)
__LD2_LANE_FUNC(uint8x8x2_t, uint8x8_t, uint8x16x2_t, uint8_t, v8qi, v16qi, qi,
                u8, int8x16_t)
__LD2_LANE_FUNC(uint16x4x2_t, uint16x4_t, uint16x8x2_t, uint16_t, v4hi, v8hi,
                hi, u16, int16x8_t)
__LD2_LANE_FUNC(uint32x2x2_t, uint32x2_t, uint32x4x2_t, uint32_t, v2si, v4si,
                si, u32, int32x4_t)
__LD2_LANE_FUNC(uint64x1x2_t, uint64x1_t, uint64x2x2_t, uint64_t, di, v2di, di,
                u64, int64x2_t)

#undef __LD2_LANE_FUNC

#define __LD2_LANE_FUNC(intype, vtype, ptrtype, mode, ptrmode, funcsuffix)   \
  FUNK(intype)                                                               \
  vld2q_lane_##funcsuffix(const ptrtype *__ptr, intype __b, const int __c) { \
    __builtin_aarch64_simd_oi __o;                                           \
    intype ret;                                                              \
    __o = __builtin_aarch64_set_qregoiv4si(__o, (int32x4_t)__b.val[0], 0);   \
    __o = __builtin_aarch64_set_qregoiv4si(__o, (int32x4_t)__b.val[1], 1);   \
    __o = __builtin_aarch64_ld2_lane##mode(                                  \
        (__builtin_aarch64_simd_##ptrmode *)__ptr, __o, __c);                \
    ret.val[0] = (vtype)__builtin_aarch64_get_qregoiv4si(__o, 0);            \
    ret.val[1] = (vtype)__builtin_aarch64_get_qregoiv4si(__o, 1);            \
    return ret;                                                              \
  }

__LD2_LANE_FUNC(float16x8x2_t, float16x8_t, float16_t, v8hf, hf, f16)
__LD2_LANE_FUNC(float32x4x2_t, float32x4_t, float32_t, v4sf, sf, f32)
__LD2_LANE_FUNC(float64x2x2_t, float64x2_t, float64_t, v2df, df, f64)
__LD2_LANE_FUNC(poly8x16x2_t, poly8x16_t, poly8_t, v16qi, qi, p8)
__LD2_LANE_FUNC(poly16x8x2_t, poly16x8_t, poly16_t, v8hi, hi, p16)
__LD2_LANE_FUNC(poly64x2x2_t, poly64x2_t, poly64_t, v2di, di, p64)
__LD2_LANE_FUNC(int8x16x2_t, int8x16_t, int8_t, v16qi, qi, s8)
__LD2_LANE_FUNC(int16x8x2_t, int16x8_t, int16_t, v8hi, hi, s16)
__LD2_LANE_FUNC(int32x4x2_t, int32x4_t, int32_t, v4si, si, s32)
__LD2_LANE_FUNC(int64x2x2_t, int64x2_t, int64_t, v2di, di, s64)
__LD2_LANE_FUNC(uint8x16x2_t, uint8x16_t, uint8_t, v16qi, qi, u8)
__LD2_LANE_FUNC(uint16x8x2_t, uint16x8_t, uint16_t, v8hi, hi, u16)
__LD2_LANE_FUNC(uint32x4x2_t, uint32x4_t, uint32_t, v4si, si, u32)
__LD2_LANE_FUNC(uint64x2x2_t, uint64x2_t, uint64_t, v2di, di, u64)

#undef __LD2_LANE_FUNC

#define __LD3_LANE_FUNC(intype, vectype, largetype, ptrtype, mode, qmode,     \
                        ptrmode, funcsuffix, signedtype)                      \
  FUNK(intype)                                                                \
  vld3_lane_##funcsuffix(const ptrtype *__ptr, intype __b, const int __c) {   \
    __builtin_aarch64_simd_ci __o;                                            \
    largetype __temp;                                                         \
    __temp.val[0] =                                                           \
        vcombine_##funcsuffix(__b.val[0], vcreate_##funcsuffix(0));           \
    __temp.val[1] =                                                           \
        vcombine_##funcsuffix(__b.val[1], vcreate_##funcsuffix(0));           \
    __temp.val[2] =                                                           \
        vcombine_##funcsuffix(__b.val[2], vcreate_##funcsuffix(0));           \
    __o = __builtin_aarch64_set_qregci##qmode(__o, (signedtype)__temp.val[0], \
                                              0);                             \
    __o = __builtin_aarch64_set_qregci##qmode(__o, (signedtype)__temp.val[1], \
                                              1);                             \
    __o = __builtin_aarch64_set_qregci##qmode(__o, (signedtype)__temp.val[2], \
                                              2);                             \
    __o = __builtin_aarch64_ld3_lane##mode(                                   \
        (__builtin_aarch64_simd_##ptrmode *)__ptr, __o, __c);                 \
    __b.val[0] = (vectype)__builtin_aarch64_get_dregcidi(__o, 0);             \
    __b.val[1] = (vectype)__builtin_aarch64_get_dregcidi(__o, 1);             \
    __b.val[2] = (vectype)__builtin_aarch64_get_dregcidi(__o, 2);             \
    return __b;                                                               \
  }

__LD3_LANE_FUNC(float16x4x3_t, float16x4_t, float16x8x3_t, float16_t, v4hf,
                v8hf, hf, f16, float16x8_t)
__LD3_LANE_FUNC(float32x2x3_t, float32x2_t, float32x4x3_t, float32_t, v2sf,
                v4sf, sf, f32, float32x4_t)
__LD3_LANE_FUNC(float64x1x3_t, float64x1_t, float64x2x3_t, float64_t, df, v2df,
                df, f64, float64x2_t)
__LD3_LANE_FUNC(poly8x8x3_t, poly8x8_t, poly8x16x3_t, poly8_t, v8qi, v16qi, qi,
                p8, int8x16_t)
__LD3_LANE_FUNC(poly16x4x3_t, poly16x4_t, poly16x8x3_t, poly16_t, v4hi, v8hi,
                hi, p16, int16x8_t)
__LD3_LANE_FUNC(poly64x1x3_t, poly64x1_t, poly64x2x3_t, poly64_t, di, v2di_ssps,
                di, p64, poly64x2_t)
__LD3_LANE_FUNC(int8x8x3_t, int8x8_t, int8x16x3_t, int8_t, v8qi, v16qi, qi, s8,
                int8x16_t)
__LD3_LANE_FUNC(int16x4x3_t, int16x4_t, int16x8x3_t, int16_t, v4hi, v8hi, hi,
                s16, int16x8_t)
__LD3_LANE_FUNC(int32x2x3_t, int32x2_t, int32x4x3_t, int32_t, v2si, v4si, si,
                s32, int32x4_t)
__LD3_LANE_FUNC(int64x1x3_t, int64x1_t, int64x2x3_t, int64_t, di, v2di, di, s64,
                int64x2_t)
__LD3_LANE_FUNC(uint8x8x3_t, uint8x8_t, uint8x16x3_t, uint8_t, v8qi, v16qi, qi,
                u8, int8x16_t)
__LD3_LANE_FUNC(uint16x4x3_t, uint16x4_t, uint16x8x3_t, uint16_t, v4hi, v8hi,
                hi, u16, int16x8_t)
__LD3_LANE_FUNC(uint32x2x3_t, uint32x2_t, uint32x4x3_t, uint32_t, v2si, v4si,
                si, u32, int32x4_t)
__LD3_LANE_FUNC(uint64x1x3_t, uint64x1_t, uint64x2x3_t, uint64_t, di, v2di, di,
                u64, int64x2_t)

#undef __LD3_LANE_FUNC

#define __LD3_LANE_FUNC(intype, vtype, ptrtype, mode, ptrmode, funcsuffix)   \
  FUNK(intype)                                                               \
  vld3q_lane_##funcsuffix(const ptrtype *__ptr, intype __b, const int __c) { \
    __builtin_aarch64_simd_ci __o;                                           \
    intype ret;                                                              \
    __o = __builtin_aarch64_set_qregciv4si(__o, (int32x4_t)__b.val[0], 0);   \
    __o = __builtin_aarch64_set_qregciv4si(__o, (int32x4_t)__b.val[1], 1);   \
    __o = __builtin_aarch64_set_qregciv4si(__o, (int32x4_t)__b.val[2], 2);   \
    __o = __builtin_aarch64_ld3_lane##mode(                                  \
        (__builtin_aarch64_simd_##ptrmode *)__ptr, __o, __c);                \
    ret.val[0] = (vtype)__builtin_aarch64_get_qregciv4si(__o, 0);            \
    ret.val[1] = (vtype)__builtin_aarch64_get_qregciv4si(__o, 1);            \
    ret.val[2] = (vtype)__builtin_aarch64_get_qregciv4si(__o, 2);            \
    return ret;                                                              \
  }

__LD3_LANE_FUNC(float16x8x3_t, float16x8_t, float16_t, v8hf, hf, f16)
__LD3_LANE_FUNC(float32x4x3_t, float32x4_t, float32_t, v4sf, sf, f32)
__LD3_LANE_FUNC(float64x2x3_t, float64x2_t, float64_t, v2df, df, f64)
__LD3_LANE_FUNC(poly8x16x3_t, poly8x16_t, poly8_t, v16qi, qi, p8)
__LD3_LANE_FUNC(poly16x8x3_t, poly16x8_t, poly16_t, v8hi, hi, p16)
__LD3_LANE_FUNC(poly64x2x3_t, poly64x2_t, poly64_t, v2di, di, p64)
__LD3_LANE_FUNC(int8x16x3_t, int8x16_t, int8_t, v16qi, qi, s8)
__LD3_LANE_FUNC(int16x8x3_t, int16x8_t, int16_t, v8hi, hi, s16)
__LD3_LANE_FUNC(int32x4x3_t, int32x4_t, int32_t, v4si, si, s32)
__LD3_LANE_FUNC(int64x2x3_t, int64x2_t, int64_t, v2di, di, s64)
__LD3_LANE_FUNC(uint8x16x3_t, uint8x16_t, uint8_t, v16qi, qi, u8)
__LD3_LANE_FUNC(uint16x8x3_t, uint16x8_t, uint16_t, v8hi, hi, u16)
__LD3_LANE_FUNC(uint32x4x3_t, uint32x4_t, uint32_t, v4si, si, u32)
__LD3_LANE_FUNC(uint64x2x3_t, uint64x2_t, uint64_t, v2di, di, u64)

#undef __LD3_LANE_FUNC

#define __LD4_LANE_FUNC(intype, vectype, largetype, ptrtype, mode, qmode,     \
                        ptrmode, funcsuffix, signedtype)                      \
  FUNK(intype)                                                                \
  vld4_lane_##funcsuffix(const ptrtype *__ptr, intype __b, const int __c) {   \
    __builtin_aarch64_simd_xi __o;                                            \
    largetype __temp;                                                         \
    __temp.val[0] =                                                           \
        vcombine_##funcsuffix(__b.val[0], vcreate_##funcsuffix(0));           \
    __temp.val[1] =                                                           \
        vcombine_##funcsuffix(__b.val[1], vcreate_##funcsuffix(0));           \
    __temp.val[2] =                                                           \
        vcombine_##funcsuffix(__b.val[2], vcreate_##funcsuffix(0));           \
    __temp.val[3] =                                                           \
        vcombine_##funcsuffix(__b.val[3], vcreate_##funcsuffix(0));           \
    __o = __builtin_aarch64_set_qregxi##qmode(__o, (signedtype)__temp.val[0], \
                                              0);                             \
    __o = __builtin_aarch64_set_qregxi##qmode(__o, (signedtype)__temp.val[1], \
                                              1);                             \
    __o = __builtin_aarch64_set_qregxi##qmode(__o, (signedtype)__temp.val[2], \
                                              2);                             \
    __o = __builtin_aarch64_set_qregxi##qmode(__o, (signedtype)__temp.val[3], \
                                              3);                             \
    __o = __builtin_aarch64_ld4_lane##mode(                                   \
        (__builtin_aarch64_simd_##ptrmode *)__ptr, __o, __c);                 \
    __b.val[0] = (vectype)__builtin_aarch64_get_dregxidi(__o, 0);             \
    __b.val[1] = (vectype)__builtin_aarch64_get_dregxidi(__o, 1);             \
    __b.val[2] = (vectype)__builtin_aarch64_get_dregxidi(__o, 2);             \
    __b.val[3] = (vectype)__builtin_aarch64_get_dregxidi(__o, 3);             \
    return __b;                                                               \
  }

__LD4_LANE_FUNC(float16x4x4_t, float16x4_t, float16x8x4_t, float16_t, v4hf,
                v8hf, hf, f16, float16x8_t)
__LD4_LANE_FUNC(float32x2x4_t, float32x2_t, float32x4x4_t, float32_t, v2sf,
                v4sf, sf, f32, float32x4_t)
__LD4_LANE_FUNC(float64x1x4_t, float64x1_t, float64x2x4_t, float64_t, df, v2df,
                df, f64, float64x2_t)
__LD4_LANE_FUNC(poly8x8x4_t, poly8x8_t, poly8x16x4_t, poly8_t, v8qi, v16qi, qi,
                p8, int8x16_t)
__LD4_LANE_FUNC(poly16x4x4_t, poly16x4_t, poly16x8x4_t, poly16_t, v4hi, v8hi,
                hi, p16, int16x8_t)
__LD4_LANE_FUNC(poly64x1x4_t, poly64x1_t, poly64x2x4_t, poly64_t, di, v2di_ssps,
                di, p64, poly64x2_t)
__LD4_LANE_FUNC(int8x8x4_t, int8x8_t, int8x16x4_t, int8_t, v8qi, v16qi, qi, s8,
                int8x16_t)
__LD4_LANE_FUNC(int16x4x4_t, int16x4_t, int16x8x4_t, int16_t, v4hi, v8hi, hi,
                s16, int16x8_t)
__LD4_LANE_FUNC(int32x2x4_t, int32x2_t, int32x4x4_t, int32_t, v2si, v4si, si,
                s32, int32x4_t)
__LD4_LANE_FUNC(int64x1x4_t, int64x1_t, int64x2x4_t, int64_t, di, v2di, di, s64,
                int64x2_t)
__LD4_LANE_FUNC(uint8x8x4_t, uint8x8_t, uint8x16x4_t, uint8_t, v8qi, v16qi, qi,
                u8, int8x16_t)
__LD4_LANE_FUNC(uint16x4x4_t, uint16x4_t, uint16x8x4_t, uint16_t, v4hi, v8hi,
                hi, u16, int16x8_t)
__LD4_LANE_FUNC(uint32x2x4_t, uint32x2_t, uint32x4x4_t, uint32_t, v2si, v4si,
                si, u32, int32x4_t)
__LD4_LANE_FUNC(uint64x1x4_t, uint64x1_t, uint64x2x4_t, uint64_t, di, v2di, di,
                u64, int64x2_t)

#undef __LD4_LANE_FUNC

#define __LD4_LANE_FUNC(intype, vtype, ptrtype, mode, ptrmode, funcsuffix)   \
  FUNK(intype)                                                               \
  vld4q_lane_##funcsuffix(const ptrtype *__ptr, intype __b, const int __c) { \
    __builtin_aarch64_simd_xi __o;                                           \
    intype ret;                                                              \
    __o = __builtin_aarch64_set_qregxiv4si(__o, (int32x4_t)__b.val[0], 0);   \
    __o = __builtin_aarch64_set_qregxiv4si(__o, (int32x4_t)__b.val[1], 1);   \
    __o = __builtin_aarch64_set_qregxiv4si(__o, (int32x4_t)__b.val[2], 2);   \
    __o = __builtin_aarch64_set_qregxiv4si(__o, (int32x4_t)__b.val[3], 3);   \
    __o = __builtin_aarch64_ld4_lane##mode(                                  \
        (__builtin_aarch64_simd_##ptrmode *)__ptr, __o, __c);                \
    ret.val[0] = (vtype)__builtin_aarch64_get_qregxiv4si(__o, 0);            \
    ret.val[1] = (vtype)__builtin_aarch64_get_qregxiv4si(__o, 1);            \
    ret.val[2] = (vtype)__builtin_aarch64_get_qregxiv4si(__o, 2);            \
    ret.val[3] = (vtype)__builtin_aarch64_get_qregxiv4si(__o, 3);            \
    return ret;                                                              \
  }

__LD4_LANE_FUNC(float16x8x4_t, float16x8_t, float16_t, v8hf, hf, f16)
__LD4_LANE_FUNC(float32x4x4_t, float32x4_t, float32_t, v4sf, sf, f32)
__LD4_LANE_FUNC(float64x2x4_t, float64x2_t, float64_t, v2df, df, f64)
__LD4_LANE_FUNC(poly8x16x4_t, poly8x16_t, poly8_t, v16qi, qi, p8)
__LD4_LANE_FUNC(poly16x8x4_t, poly16x8_t, poly16_t, v8hi, hi, p16)
__LD4_LANE_FUNC(poly64x2x4_t, poly64x2_t, poly64_t, v2di, di, p64)
__LD4_LANE_FUNC(int8x16x4_t, int8x16_t, int8_t, v16qi, qi, s8)
__LD4_LANE_FUNC(int16x8x4_t, int16x8_t, int16_t, v8hi, hi, s16)
__LD4_LANE_FUNC(int32x4x4_t, int32x4_t, int32_t, v4si, si, s32)
__LD4_LANE_FUNC(int64x2x4_t, int64x2_t, int64_t, v2di, di, s64)
__LD4_LANE_FUNC(uint8x16x4_t, uint8x16_t, uint8_t, v16qi, qi, u8)
__LD4_LANE_FUNC(uint16x8x4_t, uint16x8_t, uint16_t, v8hi, hi, u16)
__LD4_LANE_FUNC(uint32x4x4_t, uint32x4_t, uint32_t, v4si, si, u32)
__LD4_LANE_FUNC(uint64x2x4_t, uint64x2_t, uint64_t, v2di, di, u64)

#undef __LD4_LANE_FUNC

FUNK(float32x2_t)
vmax_f32(float32x2_t __a, float32x2_t __b) {
  return __builtin_aarch64_smax_nanv2sf(__a, __b);
}

FUNK(float64x1_t)
vmax_f64(float64x1_t __a, float64x1_t __b) {
  return (float64x1_t){__builtin_aarch64_smax_nandf(vget_lane_f64(__a, 0),
                                                    vget_lane_f64(__b, 0))};
}

FUNK(int8x8_t)
vmax_s8(int8x8_t __a, int8x8_t __b) {
  return __builtin_aarch64_smaxv8qi(__a, __b);
}

FUNK(int16x4_t)
vmax_s16(int16x4_t __a, int16x4_t __b) {
  return __builtin_aarch64_smaxv4hi(__a, __b);
}

FUNK(int32x2_t)
vmax_s32(int32x2_t __a, int32x2_t __b) {
  return __builtin_aarch64_smaxv2si(__a, __b);
}

FUNK(uint8x8_t)
vmax_u8(uint8x8_t __a, uint8x8_t __b) {
  return (uint8x8_t)__builtin_aarch64_umaxv8qi((int8x8_t)__a, (int8x8_t)__b);
}

FUNK(uint16x4_t)
vmax_u16(uint16x4_t __a, uint16x4_t __b) {
  return (uint16x4_t)__builtin_aarch64_umaxv4hi((int16x4_t)__a, (int16x4_t)__b);
}

FUNK(uint32x2_t)
vmax_u32(uint32x2_t __a, uint32x2_t __b) {
  return (uint32x2_t)__builtin_aarch64_umaxv2si((int32x2_t)__a, (int32x2_t)__b);
}

FUNK(float32x4_t)
vmaxq_f32(float32x4_t __a, float32x4_t __b) {
  return __builtin_aarch64_smax_nanv4sf(__a, __b);
}

FUNK(float64x2_t)
vmaxq_f64(float64x2_t __a, float64x2_t __b) {
  return __builtin_aarch64_smax_nanv2df(__a, __b);
}

FUNK(int8x16_t)
vmaxq_s8(int8x16_t __a, int8x16_t __b) {
  return __builtin_aarch64_smaxv16qi(__a, __b);
}

FUNK(int16x8_t)
vmaxq_s16(int16x8_t __a, int16x8_t __b) {
  return __builtin_aarch64_smaxv8hi(__a, __b);
}

FUNK(int32x4_t)
vmaxq_s32(int32x4_t __a, int32x4_t __b) {
  return __builtin_aarch64_smaxv4si(__a, __b);
}

FUNK(uint8x16_t)
vmaxq_u8(uint8x16_t __a, uint8x16_t __b) {
  return (uint8x16_t)__builtin_aarch64_umaxv16qi((int8x16_t)__a,
                                                 (int8x16_t)__b);
}

FUNK(uint16x8_t)
vmaxq_u16(uint16x8_t __a, uint16x8_t __b) {
  return (uint16x8_t)__builtin_aarch64_umaxv8hi((int16x8_t)__a, (int16x8_t)__b);
}

FUNK(uint32x4_t)
vmaxq_u32(uint32x4_t __a, uint32x4_t __b) {
  return (uint32x4_t)__builtin_aarch64_umaxv4si((int32x4_t)__a, (int32x4_t)__b);
}

FUNK(float32x2_t)
vmulx_f32(float32x2_t __a, float32x2_t __b) {
  return __builtin_aarch64_fmulxv2sf(__a, __b);
}

FUNK(float32x4_t)
vmulxq_f32(float32x4_t __a, float32x4_t __b) {
  return __builtin_aarch64_fmulxv4sf(__a, __b);
}

FUNK(float64x1_t)
vmulx_f64(float64x1_t __a, float64x1_t __b) {
  return (float64x1_t){__builtin_aarch64_fmulxdf(__a[0], __b[0])};
}

FUNK(float64x2_t)
vmulxq_f64(float64x2_t __a, float64x2_t __b) {
  return __builtin_aarch64_fmulxv2df(__a, __b);
}

FUNK(float32_t)
vmulxs_f32(float32_t __a, float32_t __b) {
  return __builtin_aarch64_fmulxsf(__a, __b);
}

FUNK(float64_t)
vmulxd_f64(float64_t __a, float64_t __b) {
  return __builtin_aarch64_fmulxdf(__a, __b);
}

FUNK(float32x2_t)
vmulx_lane_f32(float32x2_t __a, float32x2_t __v, const int __lane) {
  return vmulx_f32(__a, __aarch64_vdup_lane_f32(__v, __lane));
}

FUNK(float64x1_t)
vmulx_lane_f64(float64x1_t __a, float64x1_t __v, const int __lane) {
  return vmulx_f64(__a, __aarch64_vdup_lane_f64(__v, __lane));
}

FUNK(float32x4_t)
vmulxq_lane_f32(float32x4_t __a, float32x2_t __v, const int __lane) {
  return vmulxq_f32(__a, __aarch64_vdupq_lane_f32(__v, __lane));
}

FUNK(float64x2_t)
vmulxq_lane_f64(float64x2_t __a, float64x1_t __v, const int __lane) {
  return vmulxq_f64(__a, __aarch64_vdupq_lane_f64(__v, __lane));
}

FUNK(float32x2_t)
vmulx_laneq_f32(float32x2_t __a, float32x4_t __v, const int __lane) {
  return vmulx_f32(__a, __aarch64_vdup_laneq_f32(__v, __lane));
}

FUNK(float64x1_t)
vmulx_laneq_f64(float64x1_t __a, float64x2_t __v, const int __lane) {
  return vmulx_f64(__a, __aarch64_vdup_laneq_f64(__v, __lane));
}

FUNK(float32x4_t)
vmulxq_laneq_f32(float32x4_t __a, float32x4_t __v, const int __lane) {
  return vmulxq_f32(__a, __aarch64_vdupq_laneq_f32(__v, __lane));
}

FUNK(float64x2_t)
vmulxq_laneq_f64(float64x2_t __a, float64x2_t __v, const int __lane) {
  return vmulxq_f64(__a, __aarch64_vdupq_laneq_f64(__v, __lane));
}

FUNK(float32_t)
vmulxs_lane_f32(float32_t __a, float32x2_t __v, const int __lane) {
  return vmulxs_f32(__a, __aarch64_vget_lane_any(__v, __lane));
}

FUNK(float32_t)
vmulxs_laneq_f32(float32_t __a, float32x4_t __v, const int __lane) {
  return vmulxs_f32(__a, __aarch64_vget_lane_any(__v, __lane));
}

FUNK(float64_t)
vmulxd_lane_f64(float64_t __a, float64x1_t __v, const int __lane) {
  return vmulxd_f64(__a, __aarch64_vget_lane_any(__v, __lane));
}

FUNK(float64_t)
vmulxd_laneq_f64(float64_t __a, float64x2_t __v, const int __lane) {
  return vmulxd_f64(__a, __aarch64_vget_lane_any(__v, __lane));
}

FUNK(int8x8_t)
vpmax_s8(int8x8_t a, int8x8_t b) {
  return __builtin_aarch64_smaxpv8qi(a, b);
}

FUNK(int16x4_t)
vpmax_s16(int16x4_t a, int16x4_t b) {
  return __builtin_aarch64_smaxpv4hi(a, b);
}

FUNK(int32x2_t)
vpmax_s32(int32x2_t a, int32x2_t b) {
  return __builtin_aarch64_smaxpv2si(a, b);
}

FUNK(uint8x8_t)
vpmax_u8(uint8x8_t a, uint8x8_t b) {
  return (uint8x8_t)__builtin_aarch64_umaxpv8qi((int8x8_t)a, (int8x8_t)b);
}

FUNK(uint16x4_t)
vpmax_u16(uint16x4_t a, uint16x4_t b) {
  return (uint16x4_t)__builtin_aarch64_umaxpv4hi((int16x4_t)a, (int16x4_t)b);
}

FUNK(uint32x2_t)
vpmax_u32(uint32x2_t a, uint32x2_t b) {
  return (uint32x2_t)__builtin_aarch64_umaxpv2si((int32x2_t)a, (int32x2_t)b);
}

FUNK(int8x16_t)
vpmaxq_s8(int8x16_t a, int8x16_t b) {
  return __builtin_aarch64_smaxpv16qi(a, b);
}

FUNK(int16x8_t)
vpmaxq_s16(int16x8_t a, int16x8_t b) {
  return __builtin_aarch64_smaxpv8hi(a, b);
}

FUNK(int32x4_t)
vpmaxq_s32(int32x4_t a, int32x4_t b) {
  return __builtin_aarch64_smaxpv4si(a, b);
}

FUNK(uint8x16_t)
vpmaxq_u8(uint8x16_t a, uint8x16_t b) {
  return (uint8x16_t)__builtin_aarch64_umaxpv16qi((int8x16_t)a, (int8x16_t)b);
}

FUNK(uint16x8_t)
vpmaxq_u16(uint16x8_t a, uint16x8_t b) {
  return (uint16x8_t)__builtin_aarch64_umaxpv8hi((int16x8_t)a, (int16x8_t)b);
}

FUNK(uint32x4_t)
vpmaxq_u32(uint32x4_t a, uint32x4_t b) {
  return (uint32x4_t)__builtin_aarch64_umaxpv4si((int32x4_t)a, (int32x4_t)b);
}

FUNK(float32x2_t)
vpmax_f32(float32x2_t a, float32x2_t b) {
  return __builtin_aarch64_smax_nanpv2sf(a, b);
}

FUNK(float32x4_t)
vpmaxq_f32(float32x4_t a, float32x4_t b) {
  return __builtin_aarch64_smax_nanpv4sf(a, b);
}

FUNK(float64x2_t)
vpmaxq_f64(float64x2_t a, float64x2_t b) {
  return __builtin_aarch64_smax_nanpv2df(a, b);
}

FUNK(float64_t)
vpmaxqd_f64(float64x2_t a) {
  return __builtin_aarch64_reduc_smax_nan_scal_v2df(a);
}

FUNK(float32_t)
vpmaxs_f32(float32x2_t a) {
  return __builtin_aarch64_reduc_smax_nan_scal_v2sf(a);
}

FUNK(float32x2_t)
vpmaxnm_f32(float32x2_t a, float32x2_t b) {
  return __builtin_aarch64_smaxpv2sf(a, b);
}

FUNK(float32x4_t)
vpmaxnmq_f32(float32x4_t a, float32x4_t b) {
  return __builtin_aarch64_smaxpv4sf(a, b);
}

FUNK(float64x2_t)
vpmaxnmq_f64(float64x2_t a, float64x2_t b) {
  return __builtin_aarch64_smaxpv2df(a, b);
}

FUNK(float64_t)
vpmaxnmqd_f64(float64x2_t a) {
  return __builtin_aarch64_reduc_smax_scal_v2df(a);
}

FUNK(float32_t)
vpmaxnms_f32(float32x2_t a) {
  return __builtin_aarch64_reduc_smax_scal_v2sf(a);
}

FUNK(int8x8_t)
vpmin_s8(int8x8_t a, int8x8_t b) {
  return __builtin_aarch64_sminpv8qi(a, b);
}

FUNK(int16x4_t)
vpmin_s16(int16x4_t a, int16x4_t b) {
  return __builtin_aarch64_sminpv4hi(a, b);
}

FUNK(int32x2_t)
vpmin_s32(int32x2_t a, int32x2_t b) {
  return __builtin_aarch64_sminpv2si(a, b);
}

FUNK(uint8x8_t)
vpmin_u8(uint8x8_t a, uint8x8_t b) {
  return (uint8x8_t)__builtin_aarch64_uminpv8qi((int8x8_t)a, (int8x8_t)b);
}

FUNK(uint16x4_t)
vpmin_u16(uint16x4_t a, uint16x4_t b) {
  return (uint16x4_t)__builtin_aarch64_uminpv4hi((int16x4_t)a, (int16x4_t)b);
}

FUNK(uint32x2_t)
vpmin_u32(uint32x2_t a, uint32x2_t b) {
  return (uint32x2_t)__builtin_aarch64_uminpv2si((int32x2_t)a, (int32x2_t)b);
}

FUNK(int8x16_t)
vpminq_s8(int8x16_t a, int8x16_t b) {
  return __builtin_aarch64_sminpv16qi(a, b);
}

FUNK(int16x8_t)
vpminq_s16(int16x8_t a, int16x8_t b) {
  return __builtin_aarch64_sminpv8hi(a, b);
}

FUNK(int32x4_t)
vpminq_s32(int32x4_t a, int32x4_t b) {
  return __builtin_aarch64_sminpv4si(a, b);
}

FUNK(uint8x16_t)
vpminq_u8(uint8x16_t a, uint8x16_t b) {
  return (uint8x16_t)__builtin_aarch64_uminpv16qi((int8x16_t)a, (int8x16_t)b);
}

FUNK(uint16x8_t)
vpminq_u16(uint16x8_t a, uint16x8_t b) {
  return (uint16x8_t)__builtin_aarch64_uminpv8hi((int16x8_t)a, (int16x8_t)b);
}

FUNK(uint32x4_t)
vpminq_u32(uint32x4_t a, uint32x4_t b) {
  return (uint32x4_t)__builtin_aarch64_uminpv4si((int32x4_t)a, (int32x4_t)b);
}

FUNK(float32x2_t)
vpmin_f32(float32x2_t a, float32x2_t b) {
  return __builtin_aarch64_smin_nanpv2sf(a, b);
}

FUNK(float32x4_t)
vpminq_f32(float32x4_t a, float32x4_t b) {
  return __builtin_aarch64_smin_nanpv4sf(a, b);
}

FUNK(float64x2_t)
vpminq_f64(float64x2_t a, float64x2_t b) {
  return __builtin_aarch64_smin_nanpv2df(a, b);
}

FUNK(float64_t)
vpminqd_f64(float64x2_t a) {
  return __builtin_aarch64_reduc_smin_nan_scal_v2df(a);
}

FUNK(float32_t)
vpmins_f32(float32x2_t a) {
  return __builtin_aarch64_reduc_smin_nan_scal_v2sf(a);
}

FUNK(float32x2_t)
vpminnm_f32(float32x2_t a, float32x2_t b) {
  return __builtin_aarch64_sminpv2sf(a, b);
}

FUNK(float32x4_t)
vpminnmq_f32(float32x4_t a, float32x4_t b) {
  return __builtin_aarch64_sminpv4sf(a, b);
}

FUNK(float64x2_t)
vpminnmq_f64(float64x2_t a, float64x2_t b) {
  return __builtin_aarch64_sminpv2df(a, b);
}

FUNK(float64_t)
vpminnmqd_f64(float64x2_t a) {
  return __builtin_aarch64_reduc_smin_scal_v2df(a);
}

FUNK(float32_t)
vpminnms_f32(float32x2_t a) {
  return __builtin_aarch64_reduc_smin_scal_v2sf(a);
}

FUNK(float32x2_t)
vmaxnm_f32(float32x2_t __a, float32x2_t __b) {
  return __builtin_aarch64_fmaxv2sf(__a, __b);
}

FUNK(float64x1_t)
vmaxnm_f64(float64x1_t __a, float64x1_t __b) {
  return (float64x1_t){
      __builtin_aarch64_fmaxdf(vget_lane_f64(__a, 0), vget_lane_f64(__b, 0))};
}

FUNK(float32x4_t)
vmaxnmq_f32(float32x4_t __a, float32x4_t __b) {
  return __builtin_aarch64_fmaxv4sf(__a, __b);
}

FUNK(float64x2_t)
vmaxnmq_f64(float64x2_t __a, float64x2_t __b) {
  return __builtin_aarch64_fmaxv2df(__a, __b);
}

FUNK(float32_t)
vmaxv_f32(float32x2_t __a) {
  return __builtin_aarch64_reduc_smax_nan_scal_v2sf(__a);
}

FUNK(int8_t)
vmaxv_s8(int8x8_t __a) {
  return __builtin_aarch64_reduc_smax_scal_v8qi(__a);
}

FUNK(int16_t)
vmaxv_s16(int16x4_t __a) {
  return __builtin_aarch64_reduc_smax_scal_v4hi(__a);
}

FUNK(int32_t)
vmaxv_s32(int32x2_t __a) {
  return __builtin_aarch64_reduc_smax_scal_v2si(__a);
}

FUNK(uint8_t)
vmaxv_u8(uint8x8_t __a) {
  return __builtin_aarch64_reduc_umax_scal_v8qi_uu(__a);
}

FUNK(uint16_t)
vmaxv_u16(uint16x4_t __a) {
  return __builtin_aarch64_reduc_umax_scal_v4hi_uu(__a);
}

FUNK(uint32_t)
vmaxv_u32(uint32x2_t __a) {
  return __builtin_aarch64_reduc_umax_scal_v2si_uu(__a);
}

FUNK(float32_t)
vmaxvq_f32(float32x4_t __a) {
  return __builtin_aarch64_reduc_smax_nan_scal_v4sf(__a);
}

FUNK(float64_t)
vmaxvq_f64(float64x2_t __a) {
  return __builtin_aarch64_reduc_smax_nan_scal_v2df(__a);
}

FUNK(int8_t)
vmaxvq_s8(int8x16_t __a) {
  return __builtin_aarch64_reduc_smax_scal_v16qi(__a);
}

FUNK(int16_t)
vmaxvq_s16(int16x8_t __a) {
  return __builtin_aarch64_reduc_smax_scal_v8hi(__a);
}

FUNK(int32_t)
vmaxvq_s32(int32x4_t __a) {
  return __builtin_aarch64_reduc_smax_scal_v4si(__a);
}

FUNK(uint8_t)
vmaxvq_u8(uint8x16_t __a) {
  return __builtin_aarch64_reduc_umax_scal_v16qi_uu(__a);
}

FUNK(uint16_t)
vmaxvq_u16(uint16x8_t __a) {
  return __builtin_aarch64_reduc_umax_scal_v8hi_uu(__a);
}

FUNK(uint32_t)
vmaxvq_u32(uint32x4_t __a) {
  return __builtin_aarch64_reduc_umax_scal_v4si_uu(__a);
}

FUNK(float32_t)
vmaxnmv_f32(float32x2_t __a) {
  return __builtin_aarch64_reduc_smax_scal_v2sf(__a);
}

FUNK(float32_t)
vmaxnmvq_f32(float32x4_t __a) {
  return __builtin_aarch64_reduc_smax_scal_v4sf(__a);
}

FUNK(float64_t)
vmaxnmvq_f64(float64x2_t __a) {
  return __builtin_aarch64_reduc_smax_scal_v2df(__a);
}

FUNK(float32x2_t)
vmin_f32(float32x2_t __a, float32x2_t __b) {
  return __builtin_aarch64_smin_nanv2sf(__a, __b);
}

FUNK(float64x1_t)
vmin_f64(float64x1_t __a, float64x1_t __b) {
  return (float64x1_t){__builtin_aarch64_smin_nandf(vget_lane_f64(__a, 0),
                                                    vget_lane_f64(__b, 0))};
}

FUNK(int8x8_t)
vmin_s8(int8x8_t __a, int8x8_t __b) {
  return __builtin_aarch64_sminv8qi(__a, __b);
}

FUNK(int16x4_t)
vmin_s16(int16x4_t __a, int16x4_t __b) {
  return __builtin_aarch64_sminv4hi(__a, __b);
}

FUNK(int32x2_t)
vmin_s32(int32x2_t __a, int32x2_t __b) {
  return __builtin_aarch64_sminv2si(__a, __b);
}

FUNK(uint8x8_t)
vmin_u8(uint8x8_t __a, uint8x8_t __b) {
  return (uint8x8_t)__builtin_aarch64_uminv8qi((int8x8_t)__a, (int8x8_t)__b);
}

FUNK(uint16x4_t)
vmin_u16(uint16x4_t __a, uint16x4_t __b) {
  return (uint16x4_t)__builtin_aarch64_uminv4hi((int16x4_t)__a, (int16x4_t)__b);
}

FUNK(uint32x2_t)
vmin_u32(uint32x2_t __a, uint32x2_t __b) {
  return (uint32x2_t)__builtin_aarch64_uminv2si((int32x2_t)__a, (int32x2_t)__b);
}

FUNK(float32x4_t)
vminq_f32(float32x4_t __a, float32x4_t __b) {
  return __builtin_aarch64_smin_nanv4sf(__a, __b);
}

FUNK(float64x2_t)
vminq_f64(float64x2_t __a, float64x2_t __b) {
  return __builtin_aarch64_smin_nanv2df(__a, __b);
}

FUNK(int8x16_t)
vminq_s8(int8x16_t __a, int8x16_t __b) {
  return __builtin_aarch64_sminv16qi(__a, __b);
}

FUNK(int16x8_t)
vminq_s16(int16x8_t __a, int16x8_t __b) {
  return __builtin_aarch64_sminv8hi(__a, __b);
}

FUNK(int32x4_t)
vminq_s32(int32x4_t __a, int32x4_t __b) {
  return __builtin_aarch64_sminv4si(__a, __b);
}

FUNK(uint8x16_t)
vminq_u8(uint8x16_t __a, uint8x16_t __b) {
  return (uint8x16_t)__builtin_aarch64_uminv16qi((int8x16_t)__a,
                                                 (int8x16_t)__b);
}

FUNK(uint16x8_t)
vminq_u16(uint16x8_t __a, uint16x8_t __b) {
  return (uint16x8_t)__builtin_aarch64_uminv8hi((int16x8_t)__a, (int16x8_t)__b);
}

FUNK(uint32x4_t)
vminq_u32(uint32x4_t __a, uint32x4_t __b) {
  return (uint32x4_t)__builtin_aarch64_uminv4si((int32x4_t)__a, (int32x4_t)__b);
}

FUNK(float32x2_t)
vminnm_f32(float32x2_t __a, float32x2_t __b) {
  return __builtin_aarch64_fminv2sf(__a, __b);
}

FUNK(float64x1_t)
vminnm_f64(float64x1_t __a, float64x1_t __b) {
  return (float64x1_t){
      __builtin_aarch64_fmindf(vget_lane_f64(__a, 0), vget_lane_f64(__b, 0))};
}

FUNK(float32x4_t)
vminnmq_f32(float32x4_t __a, float32x4_t __b) {
  return __builtin_aarch64_fminv4sf(__a, __b);
}

FUNK(float64x2_t)
vminnmq_f64(float64x2_t __a, float64x2_t __b) {
  return __builtin_aarch64_fminv2df(__a, __b);
}

FUNK(float32_t)
vminv_f32(float32x2_t __a) {
  return __builtin_aarch64_reduc_smin_nan_scal_v2sf(__a);
}

FUNK(int8_t)
vminv_s8(int8x8_t __a) {
  return __builtin_aarch64_reduc_smin_scal_v8qi(__a);
}

FUNK(int16_t)
vminv_s16(int16x4_t __a) {
  return __builtin_aarch64_reduc_smin_scal_v4hi(__a);
}

FUNK(int32_t)
vminv_s32(int32x2_t __a) {
  return __builtin_aarch64_reduc_smin_scal_v2si(__a);
}

FUNK(uint8_t)
vminv_u8(uint8x8_t __a) {
  return __builtin_aarch64_reduc_umin_scal_v8qi_uu(__a);
}

FUNK(uint16_t)
vminv_u16(uint16x4_t __a) {
  return __builtin_aarch64_reduc_umin_scal_v4hi_uu(__a);
}

FUNK(uint32_t)
vminv_u32(uint32x2_t __a) {
  return __builtin_aarch64_reduc_umin_scal_v2si_uu(__a);
}

FUNK(float32_t)
vminvq_f32(float32x4_t __a) {
  return __builtin_aarch64_reduc_smin_nan_scal_v4sf(__a);
}

FUNK(float64_t)
vminvq_f64(float64x2_t __a) {
  return __builtin_aarch64_reduc_smin_nan_scal_v2df(__a);
}

FUNK(int8_t)
vminvq_s8(int8x16_t __a) {
  return __builtin_aarch64_reduc_smin_scal_v16qi(__a);
}

FUNK(int16_t)
vminvq_s16(int16x8_t __a) {
  return __builtin_aarch64_reduc_smin_scal_v8hi(__a);
}

FUNK(int32_t)
vminvq_s32(int32x4_t __a) {
  return __builtin_aarch64_reduc_smin_scal_v4si(__a);
}

FUNK(uint8_t)
vminvq_u8(uint8x16_t __a) {
  return __builtin_aarch64_reduc_umin_scal_v16qi_uu(__a);
}

FUNK(uint16_t)
vminvq_u16(uint16x8_t __a) {
  return __builtin_aarch64_reduc_umin_scal_v8hi_uu(__a);
}

FUNK(uint32_t)
vminvq_u32(uint32x4_t __a) {
  return __builtin_aarch64_reduc_umin_scal_v4si_uu(__a);
}

FUNK(float32_t)
vminnmv_f32(float32x2_t __a) {
  return __builtin_aarch64_reduc_smin_scal_v2sf(__a);
}

FUNK(float32_t)
vminnmvq_f32(float32x4_t __a) {
  return __builtin_aarch64_reduc_smin_scal_v4sf(__a);
}

FUNK(float64_t)
vminnmvq_f64(float64x2_t __a) {
  return __builtin_aarch64_reduc_smin_scal_v2df(__a);
}

FUNK(float32x2_t)
vmla_f32(float32x2_t a, float32x2_t b, float32x2_t c) {
  return a + b * c;
}

FUNK(float64x1_t)
vmla_f64(float64x1_t __a, float64x1_t __b, float64x1_t __c) {
  return __a + __b * __c;
}

FUNK(float32x4_t)
vmlaq_f32(float32x4_t a, float32x4_t b, float32x4_t c) {
  return a + b * c;
}

FUNK(float64x2_t)
vmlaq_f64(float64x2_t a, float64x2_t b, float64x2_t c) {
  return a + b * c;
}

FUNK(float32x2_t)
vmla_lane_f32(float32x2_t __a, float32x2_t __b, float32x2_t __c,
              const int __lane) {
  return (__a + (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(int16x4_t)
vmla_lane_s16(int16x4_t __a, int16x4_t __b, int16x4_t __c, const int __lane) {
  return (__a + (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(int32x2_t)
vmla_lane_s32(int32x2_t __a, int32x2_t __b, int32x2_t __c, const int __lane) {
  return (__a + (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(uint16x4_t)
vmla_lane_u16(uint16x4_t __a, uint16x4_t __b, uint16x4_t __c,
              const int __lane) {
  return (__a + (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(uint32x2_t)
vmla_lane_u32(uint32x2_t __a, uint32x2_t __b, uint32x2_t __c,
              const int __lane) {
  return (__a + (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(float32x2_t)
vmla_laneq_f32(float32x2_t __a, float32x2_t __b, float32x4_t __c,
               const int __lane) {
  return (__a + (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(int16x4_t)
vmla_laneq_s16(int16x4_t __a, int16x4_t __b, int16x8_t __c, const int __lane) {
  return (__a + (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(int32x2_t)
vmla_laneq_s32(int32x2_t __a, int32x2_t __b, int32x4_t __c, const int __lane) {
  return (__a + (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(uint16x4_t)
vmla_laneq_u16(uint16x4_t __a, uint16x4_t __b, uint16x8_t __c,
               const int __lane) {
  return (__a + (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(uint32x2_t)
vmla_laneq_u32(uint32x2_t __a, uint32x2_t __b, uint32x4_t __c,
               const int __lane) {
  return (__a + (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(float32x4_t)
vmlaq_lane_f32(float32x4_t __a, float32x4_t __b, float32x2_t __c,
               const int __lane) {
  return (__a + (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(int16x8_t)
vmlaq_lane_s16(int16x8_t __a, int16x8_t __b, int16x4_t __c, const int __lane) {
  return (__a + (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(int32x4_t)
vmlaq_lane_s32(int32x4_t __a, int32x4_t __b, int32x2_t __c, const int __lane) {
  return (__a + (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(uint16x8_t)
vmlaq_lane_u16(uint16x8_t __a, uint16x8_t __b, uint16x4_t __c,
               const int __lane) {
  return (__a + (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(uint32x4_t)
vmlaq_lane_u32(uint32x4_t __a, uint32x4_t __b, uint32x2_t __c,
               const int __lane) {
  return (__a + (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(float32x4_t)
vmlaq_laneq_f32(float32x4_t __a, float32x4_t __b, float32x4_t __c,
                const int __lane) {
  return (__a + (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(int16x8_t)
vmlaq_laneq_s16(int16x8_t __a, int16x8_t __b, int16x8_t __c, const int __lane) {
  return (__a + (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(int32x4_t)
vmlaq_laneq_s32(int32x4_t __a, int32x4_t __b, int32x4_t __c, const int __lane) {
  return (__a + (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(uint16x8_t)
vmlaq_laneq_u16(uint16x8_t __a, uint16x8_t __b, uint16x8_t __c,
                const int __lane) {
  return (__a + (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(uint32x4_t)
vmlaq_laneq_u32(uint32x4_t __a, uint32x4_t __b, uint32x4_t __c,
                const int __lane) {
  return (__a + (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(float32x2_t)
vmls_f32(float32x2_t a, float32x2_t b, float32x2_t c) {
  return a - b * c;
}

FUNK(float64x1_t)
vmls_f64(float64x1_t __a, float64x1_t __b, float64x1_t __c) {
  return __a - __b * __c;
}

FUNK(float32x4_t)
vmlsq_f32(float32x4_t a, float32x4_t b, float32x4_t c) {
  return a - b * c;
}

FUNK(float64x2_t)
vmlsq_f64(float64x2_t a, float64x2_t b, float64x2_t c) {
  return a - b * c;
}

FUNK(float32x2_t)
vmls_lane_f32(float32x2_t __a, float32x2_t __b, float32x2_t __c,
              const int __lane) {
  return (__a - (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(int16x4_t)
vmls_lane_s16(int16x4_t __a, int16x4_t __b, int16x4_t __c, const int __lane) {
  return (__a - (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(int32x2_t)
vmls_lane_s32(int32x2_t __a, int32x2_t __b, int32x2_t __c, const int __lane) {
  return (__a - (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(uint16x4_t)
vmls_lane_u16(uint16x4_t __a, uint16x4_t __b, uint16x4_t __c,
              const int __lane) {
  return (__a - (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(uint32x2_t)
vmls_lane_u32(uint32x2_t __a, uint32x2_t __b, uint32x2_t __c,
              const int __lane) {
  return (__a - (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(float32x2_t)
vmls_laneq_f32(float32x2_t __a, float32x2_t __b, float32x4_t __c,
               const int __lane) {
  return (__a - (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(int16x4_t)
vmls_laneq_s16(int16x4_t __a, int16x4_t __b, int16x8_t __c, const int __lane) {
  return (__a - (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(int32x2_t)
vmls_laneq_s32(int32x2_t __a, int32x2_t __b, int32x4_t __c, const int __lane) {
  return (__a - (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(uint16x4_t)
vmls_laneq_u16(uint16x4_t __a, uint16x4_t __b, uint16x8_t __c,
               const int __lane) {
  return (__a - (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(uint32x2_t)
vmls_laneq_u32(uint32x2_t __a, uint32x2_t __b, uint32x4_t __c,
               const int __lane) {
  return (__a - (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(float32x4_t)
vmlsq_lane_f32(float32x4_t __a, float32x4_t __b, float32x2_t __c,
               const int __lane) {
  return (__a - (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(int16x8_t)
vmlsq_lane_s16(int16x8_t __a, int16x8_t __b, int16x4_t __c, const int __lane) {
  return (__a - (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(int32x4_t)
vmlsq_lane_s32(int32x4_t __a, int32x4_t __b, int32x2_t __c, const int __lane) {
  return (__a - (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(uint16x8_t)
vmlsq_lane_u16(uint16x8_t __a, uint16x8_t __b, uint16x4_t __c,
               const int __lane) {
  return (__a - (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(uint32x4_t)
vmlsq_lane_u32(uint32x4_t __a, uint32x4_t __b, uint32x2_t __c,
               const int __lane) {
  return (__a - (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(float32x4_t)
vmlsq_laneq_f32(float32x4_t __a, float32x4_t __b, float32x4_t __c,
                const int __lane) {
  return (__a - (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(int16x8_t)
vmlsq_laneq_s16(int16x8_t __a, int16x8_t __b, int16x8_t __c, const int __lane) {
  return (__a - (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(int32x4_t)
vmlsq_laneq_s32(int32x4_t __a, int32x4_t __b, int32x4_t __c, const int __lane) {
  return (__a - (__b * __aarch64_vget_lane_any(__c, __lane)));
}
FUNK(uint16x8_t)
vmlsq_laneq_u16(uint16x8_t __a, uint16x8_t __b, uint16x8_t __c,
                const int __lane) {
  return (__a - (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(uint32x4_t)
vmlsq_laneq_u32(uint32x4_t __a, uint32x4_t __b, uint32x4_t __c,
                const int __lane) {
  return (__a - (__b * __aarch64_vget_lane_any(__c, __lane)));
}

FUNK(float16x4_t)
vmov_n_f16(float16_t __a) {
  return vdup_n_f16(__a);
}

FUNK(float32x2_t)
vmov_n_f32(float32_t __a) {
  return vdup_n_f32(__a);
}

FUNK(float64x1_t)
vmov_n_f64(float64_t __a) {
  return (float64x1_t){__a};
}

FUNK(poly8x8_t)
vmov_n_p8(poly8_t __a) {
  return vdup_n_p8(__a);
}

FUNK(poly16x4_t)
vmov_n_p16(poly16_t __a) {
  return vdup_n_p16(__a);
}

FUNK(poly64x1_t)
vmov_n_p64(poly64_t __a) {
  return vdup_n_p64(__a);
}

FUNK(int8x8_t)
vmov_n_s8(int8_t __a) {
  return vdup_n_s8(__a);
}

FUNK(int16x4_t)
vmov_n_s16(int16_t __a) {
  return vdup_n_s16(__a);
}

FUNK(int32x2_t)
vmov_n_s32(int32_t __a) {
  return vdup_n_s32(__a);
}

FUNK(int64x1_t)
vmov_n_s64(int64_t __a) {
  return (int64x1_t){__a};
}

FUNK(uint8x8_t)
vmov_n_u8(uint8_t __a) {
  return vdup_n_u8(__a);
}

FUNK(uint16x4_t)
vmov_n_u16(uint16_t __a) {
  return vdup_n_u16(__a);
}

FUNK(uint32x2_t)
vmov_n_u32(uint32_t __a) {
  return vdup_n_u32(__a);
}

FUNK(uint64x1_t)
vmov_n_u64(uint64_t __a) {
  return (uint64x1_t){__a};
}

FUNK(float16x8_t)
vmovq_n_f16(float16_t __a) {
  return vdupq_n_f16(__a);
}

FUNK(float32x4_t)
vmovq_n_f32(float32_t __a) {
  return vdupq_n_f32(__a);
}

FUNK(float64x2_t)
vmovq_n_f64(float64_t __a) {
  return vdupq_n_f64(__a);
}

FUNK(poly8x16_t)
vmovq_n_p8(poly8_t __a) {
  return vdupq_n_p8(__a);
}

FUNK(poly16x8_t)
vmovq_n_p16(poly16_t __a) {
  return vdupq_n_p16(__a);
}

FUNK(poly64x2_t)
vmovq_n_p64(poly64_t __a) {
  return vdupq_n_p64(__a);
}

FUNK(int8x16_t)
vmovq_n_s8(int8_t __a) {
  return vdupq_n_s8(__a);
}

FUNK(int16x8_t)
vmovq_n_s16(int16_t __a) {
  return vdupq_n_s16(__a);
}

FUNK(int32x4_t)
vmovq_n_s32(int32_t __a) {
  return vdupq_n_s32(__a);
}

FUNK(int64x2_t)
vmovq_n_s64(int64_t __a) {
  return vdupq_n_s64(__a);
}

FUNK(uint8x16_t)
vmovq_n_u8(uint8_t __a) {
  return vdupq_n_u8(__a);
}

FUNK(uint16x8_t)
vmovq_n_u16(uint16_t __a) {
  return vdupq_n_u16(__a);
}

FUNK(uint32x4_t)
vmovq_n_u32(uint32_t __a) {
  return vdupq_n_u32(__a);
}

FUNK(uint64x2_t)
vmovq_n_u64(uint64_t __a) {
  return vdupq_n_u64(__a);
}

FUNK(float32x2_t)
vmul_lane_f32(float32x2_t __a, float32x2_t __b, const int __lane) {
  return __a * __aarch64_vget_lane_any(__b, __lane);
}

FUNK(float64x1_t)
vmul_lane_f64(float64x1_t __a, float64x1_t __b, const int __lane) {
  return __a * __b;
}

FUNK(int16x4_t)
vmul_lane_s16(int16x4_t __a, int16x4_t __b, const int __lane) {
  return __a * __aarch64_vget_lane_any(__b, __lane);
}

FUNK(int32x2_t)
vmul_lane_s32(int32x2_t __a, int32x2_t __b, const int __lane) {
  return __a * __aarch64_vget_lane_any(__b, __lane);
}

FUNK(uint16x4_t)
vmul_lane_u16(uint16x4_t __a, uint16x4_t __b, const int __lane) {
  return __a * __aarch64_vget_lane_any(__b, __lane);
}

FUNK(uint32x2_t)
vmul_lane_u32(uint32x2_t __a, uint32x2_t __b, const int __lane) {
  return __a * __aarch64_vget_lane_any(__b, __lane);
}

FUNK(float64_t)
vmuld_lane_f64(float64_t __a, float64x1_t __b, const int __lane) {
  return __a * __aarch64_vget_lane_any(__b, __lane);
}

FUNK(float64_t)
vmuld_laneq_f64(float64_t __a, float64x2_t __b, const int __lane) {
  return __a * __aarch64_vget_lane_any(__b, __lane);
}

FUNK(float32_t)
vmuls_lane_f32(float32_t __a, float32x2_t __b, const int __lane) {
  return __a * __aarch64_vget_lane_any(__b, __lane);
}

FUNK(float32_t)
vmuls_laneq_f32(float32_t __a, float32x4_t __b, const int __lane) {
  return __a * __aarch64_vget_lane_any(__b, __lane);
}

FUNK(float32x2_t)
vmul_laneq_f32(float32x2_t __a, float32x4_t __b, const int __lane) {
  return __a * __aarch64_vget_lane_any(__b, __lane);
}

FUNK(float64x1_t)
vmul_laneq_f64(float64x1_t __a, float64x2_t __b, const int __lane) {
  return __a * __aarch64_vget_lane_any(__b, __lane);
}

FUNK(int16x4_t)
vmul_laneq_s16(int16x4_t __a, int16x8_t __b, const int __lane) {
  return __a * __aarch64_vget_lane_any(__b, __lane);
}

FUNK(int32x2_t)
vmul_laneq_s32(int32x2_t __a, int32x4_t __b, const int __lane) {
  return __a * __aarch64_vget_lane_any(__b, __lane);
}

FUNK(uint16x4_t)
vmul_laneq_u16(uint16x4_t __a, uint16x8_t __b, const int __lane) {
  return __a * __aarch64_vget_lane_any(__b, __lane);
}

FUNK(uint32x2_t)
vmul_laneq_u32(uint32x2_t __a, uint32x4_t __b, const int __lane) {
  return __a * __aarch64_vget_lane_any(__b, __lane);
}

FUNK(float64x1_t)
vmul_n_f64(float64x1_t __a, float64_t __b) {
  return (float64x1_t){vget_lane_f64(__a, 0) * __b};
}

FUNK(float32x4_t)
vmulq_lane_f32(float32x4_t __a, float32x2_t __b, const int __lane) {
  return __a * __aarch64_vget_lane_any(__b, __lane);
}

FUNK(float64x2_t)
vmulq_lane_f64(float64x2_t __a, float64x1_t __b, const int __lane) {
  __AARCH64_LANE_CHECK(__a, __lane);
  return __a * __b[0];
}

FUNK(int16x8_t)
vmulq_lane_s16(int16x8_t __a, int16x4_t __b, const int __lane) {
  return __a * __aarch64_vget_lane_any(__b, __lane);
}

FUNK(int32x4_t)
vmulq_lane_s32(int32x4_t __a, int32x2_t __b, const int __lane) {
  return __a * __aarch64_vget_lane_any(__b, __lane);
}

FUNK(uint16x8_t)
vmulq_lane_u16(uint16x8_t __a, uint16x4_t __b, const int __lane) {
  return __a * __aarch64_vget_lane_any(__b, __lane);
}

FUNK(uint32x4_t)
vmulq_lane_u32(uint32x4_t __a, uint32x2_t __b, const int __lane) {
  return __a * __aarch64_vget_lane_any(__b, __lane);
}

FUNK(float32x4_t)
vmulq_laneq_f32(float32x4_t __a, float32x4_t __b, const int __lane) {
  return __a * __aarch64_vget_lane_any(__b, __lane);
}

FUNK(float64x2_t)
vmulq_laneq_f64(float64x2_t __a, float64x2_t __b, const int __lane) {
  return __a * __aarch64_vget_lane_any(__b, __lane);
}

FUNK(int16x8_t)
vmulq_laneq_s16(int16x8_t __a, int16x8_t __b, const int __lane) {
  return __a * __aarch64_vget_lane_any(__b, __lane);
}

FUNK(int32x4_t)
vmulq_laneq_s32(int32x4_t __a, int32x4_t __b, const int __lane) {
  return __a * __aarch64_vget_lane_any(__b, __lane);
}

FUNK(uint16x8_t)
vmulq_laneq_u16(uint16x8_t __a, uint16x8_t __b, const int __lane) {
  return __a * __aarch64_vget_lane_any(__b, __lane);
}

FUNK(uint32x4_t)
vmulq_laneq_u32(uint32x4_t __a, uint32x4_t __b, const int __lane) {
  return __a * __aarch64_vget_lane_any(__b, __lane);
}

FUNK(float32x2_t)
vmul_n_f32(float32x2_t __a, float32_t __b) {
  return __a * __b;
}

FUNK(float32x4_t)
vmulq_n_f32(float32x4_t __a, float32_t __b) {
  return __a * __b;
}

FUNK(float64x2_t)
vmulq_n_f64(float64x2_t __a, float64_t __b) {
  return __a * __b;
}

FUNK(int16x4_t)
vmul_n_s16(int16x4_t __a, int16_t __b) {
  return __a * __b;
}

FUNK(int16x8_t)
vmulq_n_s16(int16x8_t __a, int16_t __b) {
  return __a * __b;
}

FUNK(int32x2_t)
vmul_n_s32(int32x2_t __a, int32_t __b) {
  return __a * __b;
}

FUNK(int32x4_t)
vmulq_n_s32(int32x4_t __a, int32_t __b) {
  return __a * __b;
}

FUNK(uint16x4_t)
vmul_n_u16(uint16x4_t __a, uint16_t __b) {
  return __a * __b;
}

FUNK(uint16x8_t)
vmulq_n_u16(uint16x8_t __a, uint16_t __b) {
  return __a * __b;
}

FUNK(uint32x2_t)
vmul_n_u32(uint32x2_t __a, uint32_t __b) {
  return __a * __b;
}

FUNK(uint32x4_t)
vmulq_n_u32(uint32x4_t __a, uint32_t __b) {
  return __a * __b;
}

FUNK(poly8x8_t)
vmvn_p8(poly8x8_t __a) {
  return (poly8x8_t) ~((int8x8_t)__a);
}

FUNK(int8x8_t)
vmvn_s8(int8x8_t __a) {
  return ~__a;
}

FUNK(int16x4_t)
vmvn_s16(int16x4_t __a) {
  return ~__a;
}

FUNK(int32x2_t)
vmvn_s32(int32x2_t __a) {
  return ~__a;
}

FUNK(uint8x8_t)
vmvn_u8(uint8x8_t __a) {
  return ~__a;
}

FUNK(uint16x4_t)
vmvn_u16(uint16x4_t __a) {
  return ~__a;
}

FUNK(uint32x2_t)
vmvn_u32(uint32x2_t __a) {
  return ~__a;
}

FUNK(poly8x16_t)
vmvnq_p8(poly8x16_t __a) {
  return (poly8x16_t) ~((int8x16_t)__a);
}

FUNK(int8x16_t)
vmvnq_s8(int8x16_t __a) {
  return ~__a;
}

FUNK(int16x8_t)
vmvnq_s16(int16x8_t __a) {
  return ~__a;
}

FUNK(int32x4_t)
vmvnq_s32(int32x4_t __a) {
  return ~__a;
}

FUNK(uint8x16_t)
vmvnq_u8(uint8x16_t __a) {
  return ~__a;
}

FUNK(uint16x8_t)
vmvnq_u16(uint16x8_t __a) {
  return ~__a;
}

FUNK(uint32x4_t)
vmvnq_u32(uint32x4_t __a) {
  return ~__a;
}

FUNK(float32x2_t)
vneg_f32(float32x2_t __a) {
  return -__a;
}

FUNK(float64x1_t)
vneg_f64(float64x1_t __a) {
  return -__a;
}

FUNK(int8x8_t)
vneg_s8(int8x8_t __a) {
  return -__a;
}

FUNK(int16x4_t)
vneg_s16(int16x4_t __a) {
  return -__a;
}

FUNK(int32x2_t)
vneg_s32(int32x2_t __a) {
  return -__a;
}

FUNK(int64x1_t)
vneg_s64(int64x1_t __a) {
  return -__a;
}

FUNK(int64_t)
vnegd_s64(int64_t __a) {
  return -(uint64_t)__a;
}

FUNK(float32x4_t)
vnegq_f32(float32x4_t __a) {
  return -__a;
}

FUNK(float64x2_t)
vnegq_f64(float64x2_t __a) {
  return -__a;
}

FUNK(int8x16_t)
vnegq_s8(int8x16_t __a) {
  return -__a;
}

FUNK(int16x8_t)
vnegq_s16(int16x8_t __a) {
  return -__a;
}

FUNK(int32x4_t)
vnegq_s32(int32x4_t __a) {
  return -__a;
}

FUNK(int64x2_t)
vnegq_s64(int64x2_t __a) {
  return -__a;
}

FUNK(float32x2_t)
vpadd_f32(float32x2_t __a, float32x2_t __b) {
  return __builtin_aarch64_faddpv2sf(__a, __b);
}

FUNK(float32x4_t)
vpaddq_f32(float32x4_t __a, float32x4_t __b) {
  return __builtin_aarch64_faddpv4sf(__a, __b);
}

FUNK(float64x2_t)
vpaddq_f64(float64x2_t __a, float64x2_t __b) {
  return __builtin_aarch64_faddpv2df(__a, __b);
}

FUNK(int8x8_t)
vpadd_s8(int8x8_t __a, int8x8_t __b) {
  return __builtin_aarch64_addpv8qi(__a, __b);
}

FUNK(int16x4_t)
vpadd_s16(int16x4_t __a, int16x4_t __b) {
  return __builtin_aarch64_addpv4hi(__a, __b);
}

FUNK(int32x2_t)
vpadd_s32(int32x2_t __a, int32x2_t __b) {
  return __builtin_aarch64_addpv2si(__a, __b);
}

FUNK(uint8x8_t)
vpadd_u8(uint8x8_t __a, uint8x8_t __b) {
  return (uint8x8_t)__builtin_aarch64_addpv8qi((int8x8_t)__a, (int8x8_t)__b);
}

FUNK(uint16x4_t)
vpadd_u16(uint16x4_t __a, uint16x4_t __b) {
  return (uint16x4_t)__builtin_aarch64_addpv4hi((int16x4_t)__a, (int16x4_t)__b);
}

FUNK(uint32x2_t)
vpadd_u32(uint32x2_t __a, uint32x2_t __b) {
  return (uint32x2_t)__builtin_aarch64_addpv2si((int32x2_t)__a, (int32x2_t)__b);
}

FUNK(float32_t)
vpadds_f32(float32x2_t __a) {
  return __builtin_aarch64_reduc_plus_scal_v2sf(__a);
}

FUNK(float64_t)
vpaddd_f64(float64x2_t __a) {
  return __builtin_aarch64_reduc_plus_scal_v2df(__a);
}

FUNK(int64_t)
vpaddd_s64(int64x2_t __a) {
  return __builtin_aarch64_addpdi(__a);
}

FUNK(uint64_t)
vpaddd_u64(uint64x2_t __a) {
  return __builtin_aarch64_addpdi((int64x2_t)__a);
}

FUNK(int64x2_t)
vqabsq_s64(int64x2_t __a) {
  return (int64x2_t)__builtin_aarch64_sqabsv2di(__a);
}

FUNK(int8_t)
vqabsb_s8(int8_t __a) {
  return (int8_t)__builtin_aarch64_sqabsqi(__a);
}

FUNK(int16_t)
vqabsh_s16(int16_t __a) {
  return (int16_t)__builtin_aarch64_sqabshi(__a);
}

FUNK(int32_t)
vqabss_s32(int32_t __a) {
  return (int32_t)__builtin_aarch64_sqabssi(__a);
}

FUNK(int64_t)
vqabsd_s64(int64_t __a) {
  return __builtin_aarch64_sqabsdi(__a);
}

FUNK(int8_t)
vqaddb_s8(int8_t __a, int8_t __b) {
  return (int8_t)__builtin_aarch64_sqaddqi(__a, __b);
}

FUNK(int16_t)
vqaddh_s16(int16_t __a, int16_t __b) {
  return (int16_t)__builtin_aarch64_sqaddhi(__a, __b);
}

FUNK(int32_t)
vqadds_s32(int32_t __a, int32_t __b) {
  return (int32_t)__builtin_aarch64_sqaddsi(__a, __b);
}

FUNK(int64_t)
vqaddd_s64(int64_t __a, int64_t __b) {
  return __builtin_aarch64_sqadddi(__a, __b);
}

FUNK(uint8_t)
vqaddb_u8(uint8_t __a, uint8_t __b) {
  return (uint8_t)__builtin_aarch64_uqaddqi_uuu(__a, __b);
}

FUNK(uint16_t)
vqaddh_u16(uint16_t __a, uint16_t __b) {
  return (uint16_t)__builtin_aarch64_uqaddhi_uuu(__a, __b);
}

FUNK(uint32_t)
vqadds_u32(uint32_t __a, uint32_t __b) {
  return (uint32_t)__builtin_aarch64_uqaddsi_uuu(__a, __b);
}

FUNK(uint64_t)
vqaddd_u64(uint64_t __a, uint64_t __b) {
  return __builtin_aarch64_uqadddi_uuu(__a, __b);
}

FUNK(int32x4_t)
vqdmlal_s16(int32x4_t __a, int16x4_t __b, int16x4_t __c) {
  return __builtin_aarch64_sqdmlalv4hi(__a, __b, __c);
}

FUNK(int32x4_t)
vqdmlal_high_s16(int32x4_t __a, int16x8_t __b, int16x8_t __c) {
  return __builtin_aarch64_sqdmlal2v8hi(__a, __b, __c);
}

FUNK(int32x4_t)
vqdmlal_high_lane_s16(int32x4_t __a, int16x8_t __b, int16x4_t __c,
                      int const __d) {
  return __builtin_aarch64_sqdmlal2_lanev8hi(__a, __b, __c, __d);
}

FUNK(int32x4_t)
vqdmlal_high_laneq_s16(int32x4_t __a, int16x8_t __b, int16x8_t __c,
                       int const __d) {
  return __builtin_aarch64_sqdmlal2_laneqv8hi(__a, __b, __c, __d);
}

FUNK(int32x4_t)
vqdmlal_high_n_s16(int32x4_t __a, int16x8_t __b, int16_t __c) {
  return __builtin_aarch64_sqdmlal2_nv8hi(__a, __b, __c);
}

FUNK(int32x4_t)
vqdmlal_lane_s16(int32x4_t __a, int16x4_t __b, int16x4_t __c, int const __d) {
  return __builtin_aarch64_sqdmlal_lanev4hi(__a, __b, __c, __d);
}

FUNK(int32x4_t)
vqdmlal_laneq_s16(int32x4_t __a, int16x4_t __b, int16x8_t __c, int const __d) {
  return __builtin_aarch64_sqdmlal_laneqv4hi(__a, __b, __c, __d);
}

FUNK(int32x4_t)
vqdmlal_n_s16(int32x4_t __a, int16x4_t __b, int16_t __c) {
  return __builtin_aarch64_sqdmlal_nv4hi(__a, __b, __c);
}

FUNK(int64x2_t)
vqdmlal_s32(int64x2_t __a, int32x2_t __b, int32x2_t __c) {
  return __builtin_aarch64_sqdmlalv2si(__a, __b, __c);
}

FUNK(int64x2_t)
vqdmlal_high_s32(int64x2_t __a, int32x4_t __b, int32x4_t __c) {
  return __builtin_aarch64_sqdmlal2v4si(__a, __b, __c);
}

FUNK(int64x2_t)
vqdmlal_high_lane_s32(int64x2_t __a, int32x4_t __b, int32x2_t __c,
                      int const __d) {
  return __builtin_aarch64_sqdmlal2_lanev4si(__a, __b, __c, __d);
}

FUNK(int64x2_t)
vqdmlal_high_laneq_s32(int64x2_t __a, int32x4_t __b, int32x4_t __c,
                       int const __d) {
  return __builtin_aarch64_sqdmlal2_laneqv4si(__a, __b, __c, __d);
}

FUNK(int64x2_t)
vqdmlal_high_n_s32(int64x2_t __a, int32x4_t __b, int32_t __c) {
  return __builtin_aarch64_sqdmlal2_nv4si(__a, __b, __c);
}

FUNK(int64x2_t)
vqdmlal_lane_s32(int64x2_t __a, int32x2_t __b, int32x2_t __c, int const __d) {
  return __builtin_aarch64_sqdmlal_lanev2si(__a, __b, __c, __d);
}

FUNK(int64x2_t)
vqdmlal_laneq_s32(int64x2_t __a, int32x2_t __b, int32x4_t __c, int const __d) {
  return __builtin_aarch64_sqdmlal_laneqv2si(__a, __b, __c, __d);
}

FUNK(int64x2_t)
vqdmlal_n_s32(int64x2_t __a, int32x2_t __b, int32_t __c) {
  return __builtin_aarch64_sqdmlal_nv2si(__a, __b, __c);
}

FUNK(int32_t)
vqdmlalh_s16(int32_t __a, int16_t __b, int16_t __c) {
  return __builtin_aarch64_sqdmlalhi(__a, __b, __c);
}

FUNK(int32_t)
vqdmlalh_lane_s16(int32_t __a, int16_t __b, int16x4_t __c, const int __d) {
  return __builtin_aarch64_sqdmlal_lanehi(__a, __b, __c, __d);
}

FUNK(int32_t)
vqdmlalh_laneq_s16(int32_t __a, int16_t __b, int16x8_t __c, const int __d) {
  return __builtin_aarch64_sqdmlal_laneqhi(__a, __b, __c, __d);
}

FUNK(int64_t)
vqdmlals_s32(int64_t __a, int32_t __b, int32_t __c) {
  return __builtin_aarch64_sqdmlalsi(__a, __b, __c);
}

FUNK(int64_t)
vqdmlals_lane_s32(int64_t __a, int32_t __b, int32x2_t __c, const int __d) {
  return __builtin_aarch64_sqdmlal_lanesi(__a, __b, __c, __d);
}

FUNK(int64_t)
vqdmlals_laneq_s32(int64_t __a, int32_t __b, int32x4_t __c, const int __d) {
  return __builtin_aarch64_sqdmlal_laneqsi(__a, __b, __c, __d);
}

FUNK(int32x4_t)
vqdmlsl_s16(int32x4_t __a, int16x4_t __b, int16x4_t __c) {
  return __builtin_aarch64_sqdmlslv4hi(__a, __b, __c);
}

FUNK(int32x4_t)
vqdmlsl_high_s16(int32x4_t __a, int16x8_t __b, int16x8_t __c) {
  return __builtin_aarch64_sqdmlsl2v8hi(__a, __b, __c);
}

FUNK(int32x4_t)
vqdmlsl_high_lane_s16(int32x4_t __a, int16x8_t __b, int16x4_t __c,
                      int const __d) {
  return __builtin_aarch64_sqdmlsl2_lanev8hi(__a, __b, __c, __d);
}

FUNK(int32x4_t)
vqdmlsl_high_laneq_s16(int32x4_t __a, int16x8_t __b, int16x8_t __c,
                       int const __d) {
  return __builtin_aarch64_sqdmlsl2_laneqv8hi(__a, __b, __c, __d);
}

FUNK(int32x4_t)
vqdmlsl_high_n_s16(int32x4_t __a, int16x8_t __b, int16_t __c) {
  return __builtin_aarch64_sqdmlsl2_nv8hi(__a, __b, __c);
}

FUNK(int32x4_t)
vqdmlsl_lane_s16(int32x4_t __a, int16x4_t __b, int16x4_t __c, int const __d) {
  return __builtin_aarch64_sqdmlsl_lanev4hi(__a, __b, __c, __d);
}

FUNK(int32x4_t)
vqdmlsl_laneq_s16(int32x4_t __a, int16x4_t __b, int16x8_t __c, int const __d) {
  return __builtin_aarch64_sqdmlsl_laneqv4hi(__a, __b, __c, __d);
}

FUNK(int32x4_t)
vqdmlsl_n_s16(int32x4_t __a, int16x4_t __b, int16_t __c) {
  return __builtin_aarch64_sqdmlsl_nv4hi(__a, __b, __c);
}

FUNK(int64x2_t)
vqdmlsl_s32(int64x2_t __a, int32x2_t __b, int32x2_t __c) {
  return __builtin_aarch64_sqdmlslv2si(__a, __b, __c);
}

FUNK(int64x2_t)
vqdmlsl_high_s32(int64x2_t __a, int32x4_t __b, int32x4_t __c) {
  return __builtin_aarch64_sqdmlsl2v4si(__a, __b, __c);
}

FUNK(int64x2_t)
vqdmlsl_high_lane_s32(int64x2_t __a, int32x4_t __b, int32x2_t __c,
                      int const __d) {
  return __builtin_aarch64_sqdmlsl2_lanev4si(__a, __b, __c, __d);
}

FUNK(int64x2_t)
vqdmlsl_high_laneq_s32(int64x2_t __a, int32x4_t __b, int32x4_t __c,
                       int const __d) {
  return __builtin_aarch64_sqdmlsl2_laneqv4si(__a, __b, __c, __d);
}

FUNK(int64x2_t)
vqdmlsl_high_n_s32(int64x2_t __a, int32x4_t __b, int32_t __c) {
  return __builtin_aarch64_sqdmlsl2_nv4si(__a, __b, __c);
}

FUNK(int64x2_t)
vqdmlsl_lane_s32(int64x2_t __a, int32x2_t __b, int32x2_t __c, int const __d) {
  return __builtin_aarch64_sqdmlsl_lanev2si(__a, __b, __c, __d);
}

FUNK(int64x2_t)
vqdmlsl_laneq_s32(int64x2_t __a, int32x2_t __b, int32x4_t __c, int const __d) {
  return __builtin_aarch64_sqdmlsl_laneqv2si(__a, __b, __c, __d);
}

FUNK(int64x2_t)
vqdmlsl_n_s32(int64x2_t __a, int32x2_t __b, int32_t __c) {
  return __builtin_aarch64_sqdmlsl_nv2si(__a, __b, __c);
}

FUNK(int32_t)
vqdmlslh_s16(int32_t __a, int16_t __b, int16_t __c) {
  return __builtin_aarch64_sqdmlslhi(__a, __b, __c);
}

FUNK(int32_t)
vqdmlslh_lane_s16(int32_t __a, int16_t __b, int16x4_t __c, const int __d) {
  return __builtin_aarch64_sqdmlsl_lanehi(__a, __b, __c, __d);
}

FUNK(int32_t)
vqdmlslh_laneq_s16(int32_t __a, int16_t __b, int16x8_t __c, const int __d) {
  return __builtin_aarch64_sqdmlsl_laneqhi(__a, __b, __c, __d);
}

FUNK(int64_t)
vqdmlsls_s32(int64_t __a, int32_t __b, int32_t __c) {
  return __builtin_aarch64_sqdmlslsi(__a, __b, __c);
}

FUNK(int64_t)
vqdmlsls_lane_s32(int64_t __a, int32_t __b, int32x2_t __c, const int __d) {
  return __builtin_aarch64_sqdmlsl_lanesi(__a, __b, __c, __d);
}

FUNK(int64_t)
vqdmlsls_laneq_s32(int64_t __a, int32_t __b, int32x4_t __c, const int __d) {
  return __builtin_aarch64_sqdmlsl_laneqsi(__a, __b, __c, __d);
}

FUNK(int16x4_t)
vqdmulh_lane_s16(int16x4_t __a, int16x4_t __b, const int __c) {
  return __builtin_aarch64_sqdmulh_lanev4hi(__a, __b, __c);
}

FUNK(int32x2_t)
vqdmulh_lane_s32(int32x2_t __a, int32x2_t __b, const int __c) {
  return __builtin_aarch64_sqdmulh_lanev2si(__a, __b, __c);
}

FUNK(int16x8_t)
vqdmulhq_lane_s16(int16x8_t __a, int16x4_t __b, const int __c) {
  return __builtin_aarch64_sqdmulh_lanev8hi(__a, __b, __c);
}

FUNK(int32x4_t)
vqdmulhq_lane_s32(int32x4_t __a, int32x2_t __b, const int __c) {
  return __builtin_aarch64_sqdmulh_lanev4si(__a, __b, __c);
}

FUNK(int16_t)
vqdmulhh_s16(int16_t __a, int16_t __b) {
  return (int16_t)__builtin_aarch64_sqdmulhhi(__a, __b);
}

FUNK(int16_t)
vqdmulhh_lane_s16(int16_t __a, int16x4_t __b, const int __c) {
  return __builtin_aarch64_sqdmulh_lanehi(__a, __b, __c);
}

FUNK(int16_t)
vqdmulhh_laneq_s16(int16_t __a, int16x8_t __b, const int __c) {
  return __builtin_aarch64_sqdmulh_laneqhi(__a, __b, __c);
}

FUNK(int32_t)
vqdmulhs_s32(int32_t __a, int32_t __b) {
  return (int32_t)__builtin_aarch64_sqdmulhsi(__a, __b);
}

FUNK(int32_t)
vqdmulhs_lane_s32(int32_t __a, int32x2_t __b, const int __c) {
  return __builtin_aarch64_sqdmulh_lanesi(__a, __b, __c);
}

FUNK(int32_t)
vqdmulhs_laneq_s32(int32_t __a, int32x4_t __b, const int __c) {
  return __builtin_aarch64_sqdmulh_laneqsi(__a, __b, __c);
}

FUNK(int32x4_t)
vqdmull_s16(int16x4_t __a, int16x4_t __b) {
  return __builtin_aarch64_sqdmullv4hi(__a, __b);
}

FUNK(int32x4_t)
vqdmull_high_s16(int16x8_t __a, int16x8_t __b) {
  return __builtin_aarch64_sqdmull2v8hi(__a, __b);
}

FUNK(int32x4_t)
vqdmull_high_lane_s16(int16x8_t __a, int16x4_t __b, int const __c) {
  return __builtin_aarch64_sqdmull2_lanev8hi(__a, __b, __c);
}

FUNK(int32x4_t)
vqdmull_high_laneq_s16(int16x8_t __a, int16x8_t __b, int const __c) {
  return __builtin_aarch64_sqdmull2_laneqv8hi(__a, __b, __c);
}

FUNK(int32x4_t)
vqdmull_high_n_s16(int16x8_t __a, int16_t __b) {
  return __builtin_aarch64_sqdmull2_nv8hi(__a, __b);
}

FUNK(int32x4_t)
vqdmull_lane_s16(int16x4_t __a, int16x4_t __b, int const __c) {
  return __builtin_aarch64_sqdmull_lanev4hi(__a, __b, __c);
}

FUNK(int32x4_t)
vqdmull_laneq_s16(int16x4_t __a, int16x8_t __b, int const __c) {
  return __builtin_aarch64_sqdmull_laneqv4hi(__a, __b, __c);
}

FUNK(int32x4_t)
vqdmull_n_s16(int16x4_t __a, int16_t __b) {
  return __builtin_aarch64_sqdmull_nv4hi(__a, __b);
}

FUNK(int64x2_t)
vqdmull_s32(int32x2_t __a, int32x2_t __b) {
  return __builtin_aarch64_sqdmullv2si(__a, __b);
}

FUNK(int64x2_t)
vqdmull_high_s32(int32x4_t __a, int32x4_t __b) {
  return __builtin_aarch64_sqdmull2v4si(__a, __b);
}

FUNK(int64x2_t)
vqdmull_high_lane_s32(int32x4_t __a, int32x2_t __b, int const __c) {
  return __builtin_aarch64_sqdmull2_lanev4si(__a, __b, __c);
}

FUNK(int64x2_t)
vqdmull_high_laneq_s32(int32x4_t __a, int32x4_t __b, int const __c) {
  return __builtin_aarch64_sqdmull2_laneqv4si(__a, __b, __c);
}

FUNK(int64x2_t)
vqdmull_high_n_s32(int32x4_t __a, int32_t __b) {
  return __builtin_aarch64_sqdmull2_nv4si(__a, __b);
}

FUNK(int64x2_t)
vqdmull_lane_s32(int32x2_t __a, int32x2_t __b, int const __c) {
  return __builtin_aarch64_sqdmull_lanev2si(__a, __b, __c);
}

FUNK(int64x2_t)
vqdmull_laneq_s32(int32x2_t __a, int32x4_t __b, int const __c) {
  return __builtin_aarch64_sqdmull_laneqv2si(__a, __b, __c);
}

FUNK(int64x2_t)
vqdmull_n_s32(int32x2_t __a, int32_t __b) {
  return __builtin_aarch64_sqdmull_nv2si(__a, __b);
}

FUNK(int32_t)
vqdmullh_s16(int16_t __a, int16_t __b) {
  return (int32_t)__builtin_aarch64_sqdmullhi(__a, __b);
}

FUNK(int32_t)
vqdmullh_lane_s16(int16_t __a, int16x4_t __b, const int __c) {
  return __builtin_aarch64_sqdmull_lanehi(__a, __b, __c);
}

FUNK(int32_t)
vqdmullh_laneq_s16(int16_t __a, int16x8_t __b, const int __c) {
  return __builtin_aarch64_sqdmull_laneqhi(__a, __b, __c);
}

FUNK(int64_t)
vqdmulls_s32(int32_t __a, int32_t __b) {
  return __builtin_aarch64_sqdmullsi(__a, __b);
}

FUNK(int64_t)
vqdmulls_lane_s32(int32_t __a, int32x2_t __b, const int __c) {
  return __builtin_aarch64_sqdmull_lanesi(__a, __b, __c);
}

FUNK(int64_t)
vqdmulls_laneq_s32(int32_t __a, int32x4_t __b, const int __c) {
  return __builtin_aarch64_sqdmull_laneqsi(__a, __b, __c);
}

FUNK(int8x8_t)
vqmovn_s16(int16x8_t __a) {
  return (int8x8_t)__builtin_aarch64_sqmovnv8hi(__a);
}

FUNK(int16x4_t)
vqmovn_s32(int32x4_t __a) {
  return (int16x4_t)__builtin_aarch64_sqmovnv4si(__a);
}

FUNK(int32x2_t)
vqmovn_s64(int64x2_t __a) {
  return (int32x2_t)__builtin_aarch64_sqmovnv2di(__a);
}

FUNK(uint8x8_t)
vqmovn_u16(uint16x8_t __a) {
  return (uint8x8_t)__builtin_aarch64_uqmovnv8hi((int16x8_t)__a);
}

FUNK(uint16x4_t)
vqmovn_u32(uint32x4_t __a) {
  return (uint16x4_t)__builtin_aarch64_uqmovnv4si((int32x4_t)__a);
}

FUNK(uint32x2_t)
vqmovn_u64(uint64x2_t __a) {
  return (uint32x2_t)__builtin_aarch64_uqmovnv2di((int64x2_t)__a);
}

FUNK(int8_t)
vqmovnh_s16(int16_t __a) {
  return (int8_t)__builtin_aarch64_sqmovnhi(__a);
}

FUNK(int16_t)
vqmovns_s32(int32_t __a) {
  return (int16_t)__builtin_aarch64_sqmovnsi(__a);
}

FUNK(int32_t)
vqmovnd_s64(int64_t __a) {
  return (int32_t)__builtin_aarch64_sqmovndi(__a);
}

FUNK(uint8_t)
vqmovnh_u16(uint16_t __a) {
  return (uint8_t)__builtin_aarch64_uqmovnhi(__a);
}

FUNK(uint16_t)
vqmovns_u32(uint32_t __a) {
  return (uint16_t)__builtin_aarch64_uqmovnsi(__a);
}

FUNK(uint32_t)
vqmovnd_u64(uint64_t __a) {
  return (uint32_t)__builtin_aarch64_uqmovndi(__a);
}

FUNK(uint8x8_t)
vqmovun_s16(int16x8_t __a) {
  return (uint8x8_t)__builtin_aarch64_sqmovunv8hi(__a);
}

FUNK(uint16x4_t)
vqmovun_s32(int32x4_t __a) {
  return (uint16x4_t)__builtin_aarch64_sqmovunv4si(__a);
}

FUNK(uint32x2_t)
vqmovun_s64(int64x2_t __a) {
  return (uint32x2_t)__builtin_aarch64_sqmovunv2di(__a);
}

FUNK(int8_t)
vqmovunh_s16(int16_t __a) {
  return (int8_t)__builtin_aarch64_sqmovunhi(__a);
}

FUNK(int16_t)
vqmovuns_s32(int32_t __a) {
  return (int16_t)__builtin_aarch64_sqmovunsi(__a);
}

FUNK(int32_t)
vqmovund_s64(int64_t __a) {
  return (int32_t)__builtin_aarch64_sqmovundi(__a);
}

FUNK(int64x2_t)
vqnegq_s64(int64x2_t __a) {
  return (int64x2_t)__builtin_aarch64_sqnegv2di(__a);
}

FUNK(int8_t)
vqnegb_s8(int8_t __a) {
  return (int8_t)__builtin_aarch64_sqnegqi(__a);
}

FUNK(int16_t)
vqnegh_s16(int16_t __a) {
  return (int16_t)__builtin_aarch64_sqneghi(__a);
}

FUNK(int32_t)
vqnegs_s32(int32_t __a) {
  return (int32_t)__builtin_aarch64_sqnegsi(__a);
}

FUNK(int64_t)
vqnegd_s64(int64_t __a) {
  return __builtin_aarch64_sqnegdi(__a);
}

FUNK(int16x4_t)
vqrdmulh_lane_s16(int16x4_t __a, int16x4_t __b, const int __c) {
  return __builtin_aarch64_sqrdmulh_lanev4hi(__a, __b, __c);
}

FUNK(int32x2_t)
vqrdmulh_lane_s32(int32x2_t __a, int32x2_t __b, const int __c) {
  return __builtin_aarch64_sqrdmulh_lanev2si(__a, __b, __c);
}

FUNK(int16x8_t)
vqrdmulhq_lane_s16(int16x8_t __a, int16x4_t __b, const int __c) {
  return __builtin_aarch64_sqrdmulh_lanev8hi(__a, __b, __c);
}

FUNK(int32x4_t)
vqrdmulhq_lane_s32(int32x4_t __a, int32x2_t __b, const int __c) {
  return __builtin_aarch64_sqrdmulh_lanev4si(__a, __b, __c);
}

FUNK(int16_t)
vqrdmulhh_s16(int16_t __a, int16_t __b) {
  return (int16_t)__builtin_aarch64_sqrdmulhhi(__a, __b);
}

FUNK(int16_t)
vqrdmulhh_lane_s16(int16_t __a, int16x4_t __b, const int __c) {
  return __builtin_aarch64_sqrdmulh_lanehi(__a, __b, __c);
}

FUNK(int16_t)
vqrdmulhh_laneq_s16(int16_t __a, int16x8_t __b, const int __c) {
  return __builtin_aarch64_sqrdmulh_laneqhi(__a, __b, __c);
}

FUNK(int32_t)
vqrdmulhs_s32(int32_t __a, int32_t __b) {
  return (int32_t)__builtin_aarch64_sqrdmulhsi(__a, __b);
}

FUNK(int32_t)
vqrdmulhs_lane_s32(int32_t __a, int32x2_t __b, const int __c) {
  return __builtin_aarch64_sqrdmulh_lanesi(__a, __b, __c);
}

FUNK(int32_t)
vqrdmulhs_laneq_s32(int32_t __a, int32x4_t __b, const int __c) {
  return __builtin_aarch64_sqrdmulh_laneqsi(__a, __b, __c);
}

FUNK(int8x8_t)
vqrshl_s8(int8x8_t __a, int8x8_t __b) {
  return __builtin_aarch64_sqrshlv8qi(__a, __b);
}

FUNK(int16x4_t)
vqrshl_s16(int16x4_t __a, int16x4_t __b) {
  return __builtin_aarch64_sqrshlv4hi(__a, __b);
}

FUNK(int32x2_t)
vqrshl_s32(int32x2_t __a, int32x2_t __b) {
  return __builtin_aarch64_sqrshlv2si(__a, __b);
}

FUNK(int64x1_t)
vqrshl_s64(int64x1_t __a, int64x1_t __b) {
  return (int64x1_t){__builtin_aarch64_sqrshldi(__a[0], __b[0])};
}

FUNK(uint8x8_t)
vqrshl_u8(uint8x8_t __a, int8x8_t __b) {
  return __builtin_aarch64_uqrshlv8qi_uus(__a, __b);
}

FUNK(uint16x4_t)
vqrshl_u16(uint16x4_t __a, int16x4_t __b) {
  return __builtin_aarch64_uqrshlv4hi_uus(__a, __b);
}

FUNK(uint32x2_t)
vqrshl_u32(uint32x2_t __a, int32x2_t __b) {
  return __builtin_aarch64_uqrshlv2si_uus(__a, __b);
}

FUNK(uint64x1_t)
vqrshl_u64(uint64x1_t __a, int64x1_t __b) {
  return (uint64x1_t){__builtin_aarch64_uqrshldi_uus(__a[0], __b[0])};
}

FUNK(int8x16_t)
vqrshlq_s8(int8x16_t __a, int8x16_t __b) {
  return __builtin_aarch64_sqrshlv16qi(__a, __b);
}

FUNK(int16x8_t)
vqrshlq_s16(int16x8_t __a, int16x8_t __b) {
  return __builtin_aarch64_sqrshlv8hi(__a, __b);
}

FUNK(int32x4_t)
vqrshlq_s32(int32x4_t __a, int32x4_t __b) {
  return __builtin_aarch64_sqrshlv4si(__a, __b);
}

FUNK(int64x2_t)
vqrshlq_s64(int64x2_t __a, int64x2_t __b) {
  return __builtin_aarch64_sqrshlv2di(__a, __b);
}

FUNK(uint8x16_t)
vqrshlq_u8(uint8x16_t __a, int8x16_t __b) {
  return __builtin_aarch64_uqrshlv16qi_uus(__a, __b);
}

FUNK(uint16x8_t)
vqrshlq_u16(uint16x8_t __a, int16x8_t __b) {
  return __builtin_aarch64_uqrshlv8hi_uus(__a, __b);
}

FUNK(uint32x4_t)
vqrshlq_u32(uint32x4_t __a, int32x4_t __b) {
  return __builtin_aarch64_uqrshlv4si_uus(__a, __b);
}

FUNK(uint64x2_t)
vqrshlq_u64(uint64x2_t __a, int64x2_t __b) {
  return __builtin_aarch64_uqrshlv2di_uus(__a, __b);
}

FUNK(int8_t)
vqrshlb_s8(int8_t __a, int8_t __b) {
  return __builtin_aarch64_sqrshlqi(__a, __b);
}

FUNK(int16_t)
vqrshlh_s16(int16_t __a, int16_t __b) {
  return __builtin_aarch64_sqrshlhi(__a, __b);
}

FUNK(int32_t)
vqrshls_s32(int32_t __a, int32_t __b) {
  return __builtin_aarch64_sqrshlsi(__a, __b);
}

FUNK(int64_t)
vqrshld_s64(int64_t __a, int64_t __b) {
  return __builtin_aarch64_sqrshldi(__a, __b);
}

FUNK(uint8_t)
vqrshlb_u8(uint8_t __a, uint8_t __b) {
  return __builtin_aarch64_uqrshlqi_uus(__a, __b);
}

FUNK(uint16_t)
vqrshlh_u16(uint16_t __a, uint16_t __b) {
  return __builtin_aarch64_uqrshlhi_uus(__a, __b);
}

FUNK(uint32_t)
vqrshls_u32(uint32_t __a, uint32_t __b) {
  return __builtin_aarch64_uqrshlsi_uus(__a, __b);
}

FUNK(uint64_t)
vqrshld_u64(uint64_t __a, uint64_t __b) {
  return __builtin_aarch64_uqrshldi_uus(__a, __b);
}

FUNK(int8x8_t)
vqrshrn_n_s16(int16x8_t __a, const int __b) {
  return (int8x8_t)__builtin_aarch64_sqrshrn_nv8hi(__a, __b);
}

FUNK(int16x4_t)
vqrshrn_n_s32(int32x4_t __a, const int __b) {
  return (int16x4_t)__builtin_aarch64_sqrshrn_nv4si(__a, __b);
}

FUNK(int32x2_t)
vqrshrn_n_s64(int64x2_t __a, const int __b) {
  return (int32x2_t)__builtin_aarch64_sqrshrn_nv2di(__a, __b);
}

FUNK(uint8x8_t)
vqrshrn_n_u16(uint16x8_t __a, const int __b) {
  return __builtin_aarch64_uqrshrn_nv8hi_uus(__a, __b);
}

FUNK(uint16x4_t)
vqrshrn_n_u32(uint32x4_t __a, const int __b) {
  return __builtin_aarch64_uqrshrn_nv4si_uus(__a, __b);
}

FUNK(uint32x2_t)
vqrshrn_n_u64(uint64x2_t __a, const int __b) {
  return __builtin_aarch64_uqrshrn_nv2di_uus(__a, __b);
}

FUNK(int8_t)
vqrshrnh_n_s16(int16_t __a, const int __b) {
  return (int8_t)__builtin_aarch64_sqrshrn_nhi(__a, __b);
}

FUNK(int16_t)
vqrshrns_n_s32(int32_t __a, const int __b) {
  return (int16_t)__builtin_aarch64_sqrshrn_nsi(__a, __b);
}

FUNK(int32_t)
vqrshrnd_n_s64(int64_t __a, const int __b) {
  return (int32_t)__builtin_aarch64_sqrshrn_ndi(__a, __b);
}

FUNK(uint8_t)
vqrshrnh_n_u16(uint16_t __a, const int __b) {
  return __builtin_aarch64_uqrshrn_nhi_uus(__a, __b);
}

FUNK(uint16_t)
vqrshrns_n_u32(uint32_t __a, const int __b) {
  return __builtin_aarch64_uqrshrn_nsi_uus(__a, __b);
}

FUNK(uint32_t)
vqrshrnd_n_u64(uint64_t __a, const int __b) {
  return __builtin_aarch64_uqrshrn_ndi_uus(__a, __b);
}

FUNK(uint8x8_t)
vqrshrun_n_s16(int16x8_t __a, const int __b) {
  return (uint8x8_t)__builtin_aarch64_sqrshrun_nv8hi(__a, __b);
}

FUNK(uint16x4_t)
vqrshrun_n_s32(int32x4_t __a, const int __b) {
  return (uint16x4_t)__builtin_aarch64_sqrshrun_nv4si(__a, __b);
}

FUNK(uint32x2_t)
vqrshrun_n_s64(int64x2_t __a, const int __b) {
  return (uint32x2_t)__builtin_aarch64_sqrshrun_nv2di(__a, __b);
}

FUNK(int8_t)
vqrshrunh_n_s16(int16_t __a, const int __b) {
  return (int8_t)__builtin_aarch64_sqrshrun_nhi(__a, __b);
}

FUNK(int16_t)
vqrshruns_n_s32(int32_t __a, const int __b) {
  return (int16_t)__builtin_aarch64_sqrshrun_nsi(__a, __b);
}

FUNK(int32_t)
vqrshrund_n_s64(int64_t __a, const int __b) {
  return (int32_t)__builtin_aarch64_sqrshrun_ndi(__a, __b);
}

FUNK(int8x8_t)
vqshl_s8(int8x8_t __a, int8x8_t __b) {
  return __builtin_aarch64_sqshlv8qi(__a, __b);
}

FUNK(int16x4_t)
vqshl_s16(int16x4_t __a, int16x4_t __b) {
  return __builtin_aarch64_sqshlv4hi(__a, __b);
}

FUNK(int32x2_t)
vqshl_s32(int32x2_t __a, int32x2_t __b) {
  return __builtin_aarch64_sqshlv2si(__a, __b);
}

FUNK(int64x1_t)
vqshl_s64(int64x1_t __a, int64x1_t __b) {
  return (int64x1_t){__builtin_aarch64_sqshldi(__a[0], __b[0])};
}

FUNK(uint8x8_t)
vqshl_u8(uint8x8_t __a, int8x8_t __b) {
  return __builtin_aarch64_uqshlv8qi_uus(__a, __b);
}

FUNK(uint16x4_t)
vqshl_u16(uint16x4_t __a, int16x4_t __b) {
  return __builtin_aarch64_uqshlv4hi_uus(__a, __b);
}

FUNK(uint32x2_t)
vqshl_u32(uint32x2_t __a, int32x2_t __b) {
  return __builtin_aarch64_uqshlv2si_uus(__a, __b);
}

FUNK(uint64x1_t)
vqshl_u64(uint64x1_t __a, int64x1_t __b) {
  return (uint64x1_t){__builtin_aarch64_uqshldi_uus(__a[0], __b[0])};
}

FUNK(int8x16_t)
vqshlq_s8(int8x16_t __a, int8x16_t __b) {
  return __builtin_aarch64_sqshlv16qi(__a, __b);
}

FUNK(int16x8_t)
vqshlq_s16(int16x8_t __a, int16x8_t __b) {
  return __builtin_aarch64_sqshlv8hi(__a, __b);
}

FUNK(int32x4_t)
vqshlq_s32(int32x4_t __a, int32x4_t __b) {
  return __builtin_aarch64_sqshlv4si(__a, __b);
}

FUNK(int64x2_t)
vqshlq_s64(int64x2_t __a, int64x2_t __b) {
  return __builtin_aarch64_sqshlv2di(__a, __b);
}

FUNK(uint8x16_t)
vqshlq_u8(uint8x16_t __a, int8x16_t __b) {
  return __builtin_aarch64_uqshlv16qi_uus(__a, __b);
}

FUNK(uint16x8_t)
vqshlq_u16(uint16x8_t __a, int16x8_t __b) {
  return __builtin_aarch64_uqshlv8hi_uus(__a, __b);
}

FUNK(uint32x4_t)
vqshlq_u32(uint32x4_t __a, int32x4_t __b) {
  return __builtin_aarch64_uqshlv4si_uus(__a, __b);
}

FUNK(uint64x2_t)
vqshlq_u64(uint64x2_t __a, int64x2_t __b) {
  return __builtin_aarch64_uqshlv2di_uus(__a, __b);
}

FUNK(int8_t)
vqshlb_s8(int8_t __a, int8_t __b) {
  return __builtin_aarch64_sqshlqi(__a, __b);
}

FUNK(int16_t)
vqshlh_s16(int16_t __a, int16_t __b) {
  return __builtin_aarch64_sqshlhi(__a, __b);
}

FUNK(int32_t)
vqshls_s32(int32_t __a, int32_t __b) {
  return __builtin_aarch64_sqshlsi(__a, __b);
}

FUNK(int64_t)
vqshld_s64(int64_t __a, int64_t __b) {
  return __builtin_aarch64_sqshldi(__a, __b);
}

FUNK(uint8_t)
vqshlb_u8(uint8_t __a, uint8_t __b) {
  return __builtin_aarch64_uqshlqi_uus(__a, __b);
}

FUNK(uint16_t)
vqshlh_u16(uint16_t __a, uint16_t __b) {
  return __builtin_aarch64_uqshlhi_uus(__a, __b);
}

FUNK(uint32_t)
vqshls_u32(uint32_t __a, uint32_t __b) {
  return __builtin_aarch64_uqshlsi_uus(__a, __b);
}

FUNK(uint64_t)
vqshld_u64(uint64_t __a, uint64_t __b) {
  return __builtin_aarch64_uqshldi_uus(__a, __b);
}

FUNK(int8x8_t)
vqshl_n_s8(int8x8_t __a, const int __b) {
  return (int8x8_t)__builtin_aarch64_sqshl_nv8qi(__a, __b);
}

FUNK(int16x4_t)
vqshl_n_s16(int16x4_t __a, const int __b) {
  return (int16x4_t)__builtin_aarch64_sqshl_nv4hi(__a, __b);
}

FUNK(int32x2_t)
vqshl_n_s32(int32x2_t __a, const int __b) {
  return (int32x2_t)__builtin_aarch64_sqshl_nv2si(__a, __b);
}

FUNK(int64x1_t)
vqshl_n_s64(int64x1_t __a, const int __b) {
  return (int64x1_t){__builtin_aarch64_sqshl_ndi(__a[0], __b)};
}

FUNK(uint8x8_t)
vqshl_n_u8(uint8x8_t __a, const int __b) {
  return __builtin_aarch64_uqshl_nv8qi_uus(__a, __b);
}

FUNK(uint16x4_t)
vqshl_n_u16(uint16x4_t __a, const int __b) {
  return __builtin_aarch64_uqshl_nv4hi_uus(__a, __b);
}

FUNK(uint32x2_t)
vqshl_n_u32(uint32x2_t __a, const int __b) {
  return __builtin_aarch64_uqshl_nv2si_uus(__a, __b);
}

FUNK(uint64x1_t)
vqshl_n_u64(uint64x1_t __a, const int __b) {
  return (uint64x1_t){__builtin_aarch64_uqshl_ndi_uus(__a[0], __b)};
}

FUNK(int8x16_t)
vqshlq_n_s8(int8x16_t __a, const int __b) {
  return (int8x16_t)__builtin_aarch64_sqshl_nv16qi(__a, __b);
}

FUNK(int16x8_t)
vqshlq_n_s16(int16x8_t __a, const int __b) {
  return (int16x8_t)__builtin_aarch64_sqshl_nv8hi(__a, __b);
}

FUNK(int32x4_t)
vqshlq_n_s32(int32x4_t __a, const int __b) {
  return (int32x4_t)__builtin_aarch64_sqshl_nv4si(__a, __b);
}

FUNK(int64x2_t)
vqshlq_n_s64(int64x2_t __a, const int __b) {
  return (int64x2_t)__builtin_aarch64_sqshl_nv2di(__a, __b);
}

FUNK(uint8x16_t)
vqshlq_n_u8(uint8x16_t __a, const int __b) {
  return __builtin_aarch64_uqshl_nv16qi_uus(__a, __b);
}

FUNK(uint16x8_t)
vqshlq_n_u16(uint16x8_t __a, const int __b) {
  return __builtin_aarch64_uqshl_nv8hi_uus(__a, __b);
}

FUNK(uint32x4_t)
vqshlq_n_u32(uint32x4_t __a, const int __b) {
  return __builtin_aarch64_uqshl_nv4si_uus(__a, __b);
}

FUNK(uint64x2_t)
vqshlq_n_u64(uint64x2_t __a, const int __b) {
  return __builtin_aarch64_uqshl_nv2di_uus(__a, __b);
}

FUNK(int8_t)
vqshlb_n_s8(int8_t __a, const int __b) {
  return (int8_t)__builtin_aarch64_sqshl_nqi(__a, __b);
}

FUNK(int16_t)
vqshlh_n_s16(int16_t __a, const int __b) {
  return (int16_t)__builtin_aarch64_sqshl_nhi(__a, __b);
}

FUNK(int32_t)
vqshls_n_s32(int32_t __a, const int __b) {
  return (int32_t)__builtin_aarch64_sqshl_nsi(__a, __b);
}

FUNK(int64_t)
vqshld_n_s64(int64_t __a, const int __b) {
  return __builtin_aarch64_sqshl_ndi(__a, __b);
}

FUNK(uint8_t)
vqshlb_n_u8(uint8_t __a, const int __b) {
  return __builtin_aarch64_uqshl_nqi_uus(__a, __b);
}

FUNK(uint16_t)
vqshlh_n_u16(uint16_t __a, const int __b) {
  return __builtin_aarch64_uqshl_nhi_uus(__a, __b);
}

FUNK(uint32_t)
vqshls_n_u32(uint32_t __a, const int __b) {
  return __builtin_aarch64_uqshl_nsi_uus(__a, __b);
}

FUNK(uint64_t)
vqshld_n_u64(uint64_t __a, const int __b) {
  return __builtin_aarch64_uqshl_ndi_uus(__a, __b);
}

FUNK(uint8x8_t)
vqshlu_n_s8(int8x8_t __a, const int __b) {
  return __builtin_aarch64_sqshlu_nv8qi_uss(__a, __b);
}

FUNK(uint16x4_t)
vqshlu_n_s16(int16x4_t __a, const int __b) {
  return __builtin_aarch64_sqshlu_nv4hi_uss(__a, __b);
}

FUNK(uint32x2_t)
vqshlu_n_s32(int32x2_t __a, const int __b) {
  return __builtin_aarch64_sqshlu_nv2si_uss(__a, __b);
}

FUNK(uint64x1_t)
vqshlu_n_s64(int64x1_t __a, const int __b) {
  return (uint64x1_t){__builtin_aarch64_sqshlu_ndi_uss(__a[0], __b)};
}

FUNK(uint8x16_t)
vqshluq_n_s8(int8x16_t __a, const int __b) {
  return __builtin_aarch64_sqshlu_nv16qi_uss(__a, __b);
}

FUNK(uint16x8_t)
vqshluq_n_s16(int16x8_t __a, const int __b) {
  return __builtin_aarch64_sqshlu_nv8hi_uss(__a, __b);
}

FUNK(uint32x4_t)
vqshluq_n_s32(int32x4_t __a, const int __b) {
  return __builtin_aarch64_sqshlu_nv4si_uss(__a, __b);
}

FUNK(uint64x2_t)
vqshluq_n_s64(int64x2_t __a, const int __b) {
  return __builtin_aarch64_sqshlu_nv2di_uss(__a, __b);
}

FUNK(int8_t)
vqshlub_n_s8(int8_t __a, const int __b) {
  return (int8_t)__builtin_aarch64_sqshlu_nqi_uss(__a, __b);
}

FUNK(int16_t)
vqshluh_n_s16(int16_t __a, const int __b) {
  return (int16_t)__builtin_aarch64_sqshlu_nhi_uss(__a, __b);
}

FUNK(int32_t)
vqshlus_n_s32(int32_t __a, const int __b) {
  return (int32_t)__builtin_aarch64_sqshlu_nsi_uss(__a, __b);
}

FUNK(uint64_t)
vqshlud_n_s64(int64_t __a, const int __b) {
  return __builtin_aarch64_sqshlu_ndi_uss(__a, __b);
}

FUNK(int8x8_t)
vqshrn_n_s16(int16x8_t __a, const int __b) {
  return (int8x8_t)__builtin_aarch64_sqshrn_nv8hi(__a, __b);
}

FUNK(int16x4_t)
vqshrn_n_s32(int32x4_t __a, const int __b) {
  return (int16x4_t)__builtin_aarch64_sqshrn_nv4si(__a, __b);
}

FUNK(int32x2_t)
vqshrn_n_s64(int64x2_t __a, const int __b) {
  return (int32x2_t)__builtin_aarch64_sqshrn_nv2di(__a, __b);
}

FUNK(uint8x8_t)
vqshrn_n_u16(uint16x8_t __a, const int __b) {
  return __builtin_aarch64_uqshrn_nv8hi_uus(__a, __b);
}

FUNK(uint16x4_t)
vqshrn_n_u32(uint32x4_t __a, const int __b) {
  return __builtin_aarch64_uqshrn_nv4si_uus(__a, __b);
}

FUNK(uint32x2_t)
vqshrn_n_u64(uint64x2_t __a, const int __b) {
  return __builtin_aarch64_uqshrn_nv2di_uus(__a, __b);
}

FUNK(int8_t)
vqshrnh_n_s16(int16_t __a, const int __b) {
  return (int8_t)__builtin_aarch64_sqshrn_nhi(__a, __b);
}

FUNK(int16_t)
vqshrns_n_s32(int32_t __a, const int __b) {
  return (int16_t)__builtin_aarch64_sqshrn_nsi(__a, __b);
}

FUNK(int32_t)
vqshrnd_n_s64(int64_t __a, const int __b) {
  return (int32_t)__builtin_aarch64_sqshrn_ndi(__a, __b);
}

FUNK(uint8_t)
vqshrnh_n_u16(uint16_t __a, const int __b) {
  return __builtin_aarch64_uqshrn_nhi_uus(__a, __b);
}

FUNK(uint16_t)
vqshrns_n_u32(uint32_t __a, const int __b) {
  return __builtin_aarch64_uqshrn_nsi_uus(__a, __b);
}

FUNK(uint32_t)
vqshrnd_n_u64(uint64_t __a, const int __b) {
  return __builtin_aarch64_uqshrn_ndi_uus(__a, __b);
}

FUNK(uint8x8_t)
vqshrun_n_s16(int16x8_t __a, const int __b) {
  return (uint8x8_t)__builtin_aarch64_sqshrun_nv8hi(__a, __b);
}

FUNK(uint16x4_t)
vqshrun_n_s32(int32x4_t __a, const int __b) {
  return (uint16x4_t)__builtin_aarch64_sqshrun_nv4si(__a, __b);
}

FUNK(uint32x2_t)
vqshrun_n_s64(int64x2_t __a, const int __b) {
  return (uint32x2_t)__builtin_aarch64_sqshrun_nv2di(__a, __b);
}

FUNK(int8_t)
vqshrunh_n_s16(int16_t __a, const int __b) {
  return (int8_t)__builtin_aarch64_sqshrun_nhi(__a, __b);
}

FUNK(int16_t)
vqshruns_n_s32(int32_t __a, const int __b) {
  return (int16_t)__builtin_aarch64_sqshrun_nsi(__a, __b);
}

FUNK(int32_t)
vqshrund_n_s64(int64_t __a, const int __b) {
  return (int32_t)__builtin_aarch64_sqshrun_ndi(__a, __b);
}

FUNK(int8_t)
vqsubb_s8(int8_t __a, int8_t __b) {
  return (int8_t)__builtin_aarch64_sqsubqi(__a, __b);
}

FUNK(int16_t)
vqsubh_s16(int16_t __a, int16_t __b) {
  return (int16_t)__builtin_aarch64_sqsubhi(__a, __b);
}

FUNK(int32_t)
vqsubs_s32(int32_t __a, int32_t __b) {
  return (int32_t)__builtin_aarch64_sqsubsi(__a, __b);
}

FUNK(int64_t)
vqsubd_s64(int64_t __a, int64_t __b) {
  return __builtin_aarch64_sqsubdi(__a, __b);
}

FUNK(uint8_t)
vqsubb_u8(uint8_t __a, uint8_t __b) {
  return (uint8_t)__builtin_aarch64_uqsubqi_uuu(__a, __b);
}

FUNK(uint16_t)
vqsubh_u16(uint16_t __a, uint16_t __b) {
  return (uint16_t)__builtin_aarch64_uqsubhi_uuu(__a, __b);
}

FUNK(uint32_t)
vqsubs_u32(uint32_t __a, uint32_t __b) {
  return (uint32_t)__builtin_aarch64_uqsubsi_uuu(__a, __b);
}

FUNK(uint64_t)
vqsubd_u64(uint64_t __a, uint64_t __b) {
  return __builtin_aarch64_uqsubdi_uuu(__a, __b);
}

FUNK(int8x8_t)
vqtbl2_s8(int8x16x2_t tab, uint8x8_t idx) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv16qi(__o, tab.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, tab.val[1], 1);
  return __builtin_aarch64_tbl3v8qi(__o, (int8x8_t)idx);
}

FUNK(uint8x8_t)
vqtbl2_u8(uint8x16x2_t tab, uint8x8_t idx) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)tab.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)tab.val[1], 1);
  return (uint8x8_t)__builtin_aarch64_tbl3v8qi(__o, (int8x8_t)idx);
}

FUNK(poly8x8_t)
vqtbl2_p8(poly8x16x2_t tab, uint8x8_t idx) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)tab.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)tab.val[1], 1);
  return (poly8x8_t)__builtin_aarch64_tbl3v8qi(__o, (int8x8_t)idx);
}

FUNK(int8x16_t)
vqtbl2q_s8(int8x16x2_t tab, uint8x16_t idx) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)tab.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)tab.val[1], 1);
  return __builtin_aarch64_tbl3v16qi(__o, (int8x16_t)idx);
}

FUNK(uint8x16_t)
vqtbl2q_u8(uint8x16x2_t tab, uint8x16_t idx) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)tab.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)tab.val[1], 1);
  return (uint8x16_t)__builtin_aarch64_tbl3v16qi(__o, (int8x16_t)idx);
}

FUNK(poly8x16_t)
vqtbl2q_p8(poly8x16x2_t tab, uint8x16_t idx) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)tab.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)tab.val[1], 1);
  return (poly8x16_t)__builtin_aarch64_tbl3v16qi(__o, (int8x16_t)idx);
}

FUNK(int8x8_t)
vqtbl3_s8(int8x16x3_t tab, uint8x8_t idx) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)tab.val[0], 0);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)tab.val[1], 1);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)tab.val[2], 2);
  return __builtin_aarch64_qtbl3v8qi(__o, (int8x8_t)idx);
}

FUNK(uint8x8_t)
vqtbl3_u8(uint8x16x3_t tab, uint8x8_t idx) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)tab.val[0], 0);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)tab.val[1], 1);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)tab.val[2], 2);
  return (uint8x8_t)__builtin_aarch64_qtbl3v8qi(__o, (int8x8_t)idx);
}

FUNK(poly8x8_t)
vqtbl3_p8(poly8x16x3_t tab, uint8x8_t idx) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)tab.val[0], 0);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)tab.val[1], 1);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)tab.val[2], 2);
  return (poly8x8_t)__builtin_aarch64_qtbl3v8qi(__o, (int8x8_t)idx);
}

FUNK(int8x16_t)
vqtbl3q_s8(int8x16x3_t tab, uint8x16_t idx) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)tab.val[0], 0);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)tab.val[1], 1);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)tab.val[2], 2);
  return __builtin_aarch64_qtbl3v16qi(__o, (int8x16_t)idx);
}

FUNK(uint8x16_t)
vqtbl3q_u8(uint8x16x3_t tab, uint8x16_t idx) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)tab.val[0], 0);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)tab.val[1], 1);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)tab.val[2], 2);
  return (uint8x16_t)__builtin_aarch64_qtbl3v16qi(__o, (int8x16_t)idx);
}

FUNK(poly8x16_t)
vqtbl3q_p8(poly8x16x3_t tab, uint8x16_t idx) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)tab.val[0], 0);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)tab.val[1], 1);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)tab.val[2], 2);
  return (poly8x16_t)__builtin_aarch64_qtbl3v16qi(__o, (int8x16_t)idx);
}

FUNK(int8x8_t)
vqtbl4_s8(int8x16x4_t tab, uint8x8_t idx) {
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[3], 3);
  return __builtin_aarch64_qtbl4v8qi(__o, (int8x8_t)idx);
}

FUNK(uint8x8_t)
vqtbl4_u8(uint8x16x4_t tab, uint8x8_t idx) {
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[3], 3);
  return (uint8x8_t)__builtin_aarch64_qtbl4v8qi(__o, (int8x8_t)idx);
}

FUNK(poly8x8_t)
vqtbl4_p8(poly8x16x4_t tab, uint8x8_t idx) {
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[3], 3);
  return (poly8x8_t)__builtin_aarch64_qtbl4v8qi(__o, (int8x8_t)idx);
}

FUNK(int8x16_t)
vqtbl4q_s8(int8x16x4_t tab, uint8x16_t idx) {
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[3], 3);
  return __builtin_aarch64_qtbl4v16qi(__o, (int8x16_t)idx);
}

FUNK(uint8x16_t)
vqtbl4q_u8(uint8x16x4_t tab, uint8x16_t idx) {
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[3], 3);
  return (uint8x16_t)__builtin_aarch64_qtbl4v16qi(__o, (int8x16_t)idx);
}

FUNK(poly8x16_t)
vqtbl4q_p8(poly8x16x4_t tab, uint8x16_t idx) {
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[3], 3);
  return (poly8x16_t)__builtin_aarch64_qtbl4v16qi(__o, (int8x16_t)idx);
}

FUNK(int8x8_t)
vqtbx2_s8(int8x8_t r, int8x16x2_t tab, uint8x8_t idx) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv16qi(__o, tab.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, tab.val[1], 1);
  return __builtin_aarch64_tbx4v8qi(r, __o, (int8x8_t)idx);
}

FUNK(uint8x8_t)
vqtbx2_u8(uint8x8_t r, uint8x16x2_t tab, uint8x8_t idx) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)tab.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)tab.val[1], 1);
  return (uint8x8_t)__builtin_aarch64_tbx4v8qi((int8x8_t)r, __o, (int8x8_t)idx);
}

FUNK(poly8x8_t)
vqtbx2_p8(poly8x8_t r, poly8x16x2_t tab, uint8x8_t idx) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)tab.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)tab.val[1], 1);
  return (poly8x8_t)__builtin_aarch64_tbx4v8qi((int8x8_t)r, __o, (int8x8_t)idx);
}

FUNK(int8x16_t)
vqtbx2q_s8(int8x16_t r, int8x16x2_t tab, uint8x16_t idx) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv16qi(__o, tab.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, tab.val[1], 1);
  return __builtin_aarch64_tbx4v16qi(r, __o, (int8x16_t)idx);
}

FUNK(uint8x16_t)
vqtbx2q_u8(uint8x16_t r, uint8x16x2_t tab, uint8x16_t idx) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)tab.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)tab.val[1], 1);
  return (uint8x16_t)__builtin_aarch64_tbx4v16qi((int8x16_t)r, __o,
                                                 (int8x16_t)idx);
}

FUNK(poly8x16_t)
vqtbx2q_p8(poly8x16_t r, poly8x16x2_t tab, uint8x16_t idx) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)tab.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)tab.val[1], 1);
  return (poly8x16_t)__builtin_aarch64_tbx4v16qi((int8x16_t)r, __o,
                                                 (int8x16_t)idx);
}

FUNK(int8x8_t)
vqtbx3_s8(int8x8_t r, int8x16x3_t tab, uint8x8_t idx) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv16qi(__o, tab.val[0], 0);
  __o = __builtin_aarch64_set_qregciv16qi(__o, tab.val[1], 1);
  __o = __builtin_aarch64_set_qregciv16qi(__o, tab.val[2], 2);
  return __builtin_aarch64_qtbx3v8qi(r, __o, (int8x8_t)idx);
}

FUNK(uint8x8_t)
vqtbx3_u8(uint8x8_t r, uint8x16x3_t tab, uint8x8_t idx) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)tab.val[0], 0);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)tab.val[1], 1);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)tab.val[2], 2);
  return (uint8x8_t)__builtin_aarch64_qtbx3v8qi((int8x8_t)r, __o,
                                                (int8x8_t)idx);
}

FUNK(poly8x8_t)
vqtbx3_p8(poly8x8_t r, poly8x16x3_t tab, uint8x8_t idx) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)tab.val[0], 0);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)tab.val[1], 1);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)tab.val[2], 2);
  return (poly8x8_t)__builtin_aarch64_qtbx3v8qi((int8x8_t)r, __o,
                                                (int8x8_t)idx);
}

FUNK(int8x16_t)
vqtbx3q_s8(int8x16_t r, int8x16x3_t tab, uint8x16_t idx) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv16qi(__o, tab.val[0], 0);
  __o = __builtin_aarch64_set_qregciv16qi(__o, tab.val[1], 1);
  __o = __builtin_aarch64_set_qregciv16qi(__o, tab.val[2], 2);
  return __builtin_aarch64_qtbx3v16qi(r, __o, (int8x16_t)idx);
}

FUNK(uint8x16_t)
vqtbx3q_u8(uint8x16_t r, uint8x16x3_t tab, uint8x16_t idx) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)tab.val[0], 0);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)tab.val[1], 1);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)tab.val[2], 2);
  return (uint8x16_t)__builtin_aarch64_qtbx3v16qi((int8x16_t)r, __o,
                                                  (int8x16_t)idx);
}

FUNK(poly8x16_t)
vqtbx3q_p8(poly8x16_t r, poly8x16x3_t tab, uint8x16_t idx) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)tab.val[0], 0);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)tab.val[1], 1);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)tab.val[2], 2);
  return (poly8x16_t)__builtin_aarch64_qtbx3v16qi((int8x16_t)r, __o,
                                                  (int8x16_t)idx);
}

FUNK(int8x8_t)
vqtbx4_s8(int8x8_t r, int8x16x4_t tab, uint8x8_t idx) {
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_set_qregxiv16qi(__o, tab.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, tab.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, tab.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, tab.val[3], 3);
  return __builtin_aarch64_qtbx4v8qi(r, __o, (int8x8_t)idx);
}

FUNK(uint8x8_t)
vqtbx4_u8(uint8x8_t r, uint8x16x4_t tab, uint8x8_t idx) {
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[3], 3);
  return (uint8x8_t)__builtin_aarch64_qtbx4v8qi((int8x8_t)r, __o,
                                                (int8x8_t)idx);
}

FUNK(poly8x8_t)
vqtbx4_p8(poly8x8_t r, poly8x16x4_t tab, uint8x8_t idx) {
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[3], 3);
  return (poly8x8_t)__builtin_aarch64_qtbx4v8qi((int8x8_t)r, __o,
                                                (int8x8_t)idx);
}

FUNK(int8x16_t)
vqtbx4q_s8(int8x16_t r, int8x16x4_t tab, uint8x16_t idx) {
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_set_qregxiv16qi(__o, tab.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, tab.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, tab.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, tab.val[3], 3);
  return __builtin_aarch64_qtbx4v16qi(r, __o, (int8x16_t)idx);
}

FUNK(uint8x16_t)
vqtbx4q_u8(uint8x16_t r, uint8x16x4_t tab, uint8x16_t idx) {
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[3], 3);
  return (uint8x16_t)__builtin_aarch64_qtbx4v16qi((int8x16_t)r, __o,
                                                  (int8x16_t)idx);
}

FUNK(poly8x16_t)
vqtbx4q_p8(poly8x16_t r, poly8x16x4_t tab, uint8x16_t idx) {
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)tab.val[3], 3);
  return (poly8x16_t)__builtin_aarch64_qtbx4v16qi((int8x16_t)r, __o,
                                                  (int8x16_t)idx);
}

FUNK(poly8x8_t)
vrbit_p8(poly8x8_t __a) {
  return (poly8x8_t)__builtin_aarch64_rbitv8qi((int8x8_t)__a);
}

FUNK(int8x8_t)
vrbit_s8(int8x8_t __a) {
  return __builtin_aarch64_rbitv8qi(__a);
}

FUNK(uint8x8_t)
vrbit_u8(uint8x8_t __a) {
  return (uint8x8_t)__builtin_aarch64_rbitv8qi((int8x8_t)__a);
}

FUNK(poly8x16_t)
vrbitq_p8(poly8x16_t __a) {
  return (poly8x16_t)__builtin_aarch64_rbitv16qi((int8x16_t)__a);
}

FUNK(int8x16_t)
vrbitq_s8(int8x16_t __a) {
  return __builtin_aarch64_rbitv16qi(__a);
}

FUNK(uint8x16_t)
vrbitq_u8(uint8x16_t __a) {
  return (uint8x16_t)__builtin_aarch64_rbitv16qi((int8x16_t)__a);
}

FUNK(uint32x2_t)
vrecpe_u32(uint32x2_t __a) {
  return (uint32x2_t)__builtin_aarch64_urecpev2si((int32x2_t)__a);
}

FUNK(uint32x4_t)
vrecpeq_u32(uint32x4_t __a) {
  return (uint32x4_t)__builtin_aarch64_urecpev4si((int32x4_t)__a);
}

FUNK(float32_t)
vrecpes_f32(float32_t __a) {
  return __builtin_aarch64_frecpesf(__a);
}

FUNK(float64_t)
vrecped_f64(float64_t __a) {
  return __builtin_aarch64_frecpedf(__a);
}

FUNK(float32x2_t)
vrecpe_f32(float32x2_t __a) {
  return __builtin_aarch64_frecpev2sf(__a);
}

FUNK(float64x1_t)
vrecpe_f64(float64x1_t __a) {
  return (float64x1_t){vrecped_f64(vget_lane_f64(__a, 0))};
}

FUNK(float32x4_t)
vrecpeq_f32(float32x4_t __a) {
  return __builtin_aarch64_frecpev4sf(__a);
}

FUNK(float64x2_t)
vrecpeq_f64(float64x2_t __a) {
  return __builtin_aarch64_frecpev2df(__a);
}

FUNK(float32_t)
vrecpss_f32(float32_t __a, float32_t __b) {
  return __builtin_aarch64_frecpssf(__a, __b);
}

FUNK(float64_t)
vrecpsd_f64(float64_t __a, float64_t __b) {
  return __builtin_aarch64_frecpsdf(__a, __b);
}

FUNK(float32x2_t)
vrecps_f32(float32x2_t __a, float32x2_t __b) {
  return __builtin_aarch64_frecpsv2sf(__a, __b);
}

FUNK(float64x1_t)
vrecps_f64(float64x1_t __a, float64x1_t __b) {
  return (float64x1_t){
      vrecpsd_f64(vget_lane_f64(__a, 0), vget_lane_f64(__b, 0))};
}

FUNK(float32x4_t)
vrecpsq_f32(float32x4_t __a, float32x4_t __b) {
  return __builtin_aarch64_frecpsv4sf(__a, __b);
}

FUNK(float64x2_t)
vrecpsq_f64(float64x2_t __a, float64x2_t __b) {
  return __builtin_aarch64_frecpsv2df(__a, __b);
}

FUNK(float32_t)
vrecpxs_f32(float32_t __a) {
  return __builtin_aarch64_frecpxsf(__a);
}

FUNK(float64_t)
vrecpxd_f64(float64_t __a) {
  return __builtin_aarch64_frecpxdf(__a);
}

FUNK(poly8x8_t)
vrev16_p8(poly8x8_t a) {
  return __builtin_shuffle(a, (uint8x8_t){1, 0, 3, 2, 5, 4, 7, 6});
}

FUNK(int8x8_t)
vrev16_s8(int8x8_t a) {
  return __builtin_shuffle(a, (uint8x8_t){1, 0, 3, 2, 5, 4, 7, 6});
}

FUNK(uint8x8_t)
vrev16_u8(uint8x8_t a) {
  return __builtin_shuffle(a, (uint8x8_t){1, 0, 3, 2, 5, 4, 7, 6});
}

FUNK(poly8x16_t)
vrev16q_p8(poly8x16_t a) {
  return __builtin_shuffle(
      a, (uint8x16_t){1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14});
}

FUNK(int8x16_t)
vrev16q_s8(int8x16_t a) {
  return __builtin_shuffle(
      a, (uint8x16_t){1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14});
}

FUNK(uint8x16_t)
vrev16q_u8(uint8x16_t a) {
  return __builtin_shuffle(
      a, (uint8x16_t){1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14});
}

FUNK(poly8x8_t)
vrev32_p8(poly8x8_t a) {
  return __builtin_shuffle(a, (uint8x8_t){3, 2, 1, 0, 7, 6, 5, 4});
}

FUNK(poly16x4_t)
vrev32_p16(poly16x4_t a) {
  return __builtin_shuffle(a, (uint16x4_t){1, 0, 3, 2});
}

FUNK(int8x8_t)
vrev32_s8(int8x8_t a) {
  return __builtin_shuffle(a, (uint8x8_t){3, 2, 1, 0, 7, 6, 5, 4});
}

FUNK(int16x4_t)
vrev32_s16(int16x4_t a) {
  return __builtin_shuffle(a, (uint16x4_t){1, 0, 3, 2});
}

FUNK(uint8x8_t)
vrev32_u8(uint8x8_t a) {
  return __builtin_shuffle(a, (uint8x8_t){3, 2, 1, 0, 7, 6, 5, 4});
}

FUNK(uint16x4_t)
vrev32_u16(uint16x4_t a) {
  return __builtin_shuffle(a, (uint16x4_t){1, 0, 3, 2});
}

FUNK(poly8x16_t)
vrev32q_p8(poly8x16_t a) {
  return __builtin_shuffle(
      a, (uint8x16_t){3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12});
}

FUNK(poly16x8_t)
vrev32q_p16(poly16x8_t a) {
  return __builtin_shuffle(a, (uint16x8_t){1, 0, 3, 2, 5, 4, 7, 6});
}

FUNK(int8x16_t)
vrev32q_s8(int8x16_t a) {
  return __builtin_shuffle(
      a, (uint8x16_t){3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12});
}

FUNK(int16x8_t)
vrev32q_s16(int16x8_t a) {
  return __builtin_shuffle(a, (uint16x8_t){1, 0, 3, 2, 5, 4, 7, 6});
}

FUNK(uint8x16_t)
vrev32q_u8(uint8x16_t a) {
  return __builtin_shuffle(
      a, (uint8x16_t){3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12});
}

FUNK(uint16x8_t)
vrev32q_u16(uint16x8_t a) {
  return __builtin_shuffle(a, (uint16x8_t){1, 0, 3, 2, 5, 4, 7, 6});
}

FUNK(float16x4_t)
vrev64_f16(float16x4_t __a) {
  return __builtin_shuffle(__a, (uint16x4_t){3, 2, 1, 0});
}

FUNK(float32x2_t)
vrev64_f32(float32x2_t a) {
  return __builtin_shuffle(a, (uint32x2_t){1, 0});
}

FUNK(poly8x8_t)
vrev64_p8(poly8x8_t a) {
  return __builtin_shuffle(a, (uint8x8_t){7, 6, 5, 4, 3, 2, 1, 0});
}

FUNK(poly16x4_t)
vrev64_p16(poly16x4_t a) {
  return __builtin_shuffle(a, (uint16x4_t){3, 2, 1, 0});
}

FUNK(int8x8_t)
vrev64_s8(int8x8_t a) {
  return __builtin_shuffle(a, (uint8x8_t){7, 6, 5, 4, 3, 2, 1, 0});
}

FUNK(int16x4_t)
vrev64_s16(int16x4_t a) {
  return __builtin_shuffle(a, (uint16x4_t){3, 2, 1, 0});
}

FUNK(int32x2_t)
vrev64_s32(int32x2_t a) {
  return __builtin_shuffle(a, (uint32x2_t){1, 0});
}

FUNK(uint8x8_t)
vrev64_u8(uint8x8_t a) {
  return __builtin_shuffle(a, (uint8x8_t){7, 6, 5, 4, 3, 2, 1, 0});
}

FUNK(uint16x4_t)
vrev64_u16(uint16x4_t a) {
  return __builtin_shuffle(a, (uint16x4_t){3, 2, 1, 0});
}

FUNK(uint32x2_t)
vrev64_u32(uint32x2_t a) {
  return __builtin_shuffle(a, (uint32x2_t){1, 0});
}

FUNK(float16x8_t)
vrev64q_f16(float16x8_t __a) {
  return __builtin_shuffle(__a, (uint16x8_t){3, 2, 1, 0, 7, 6, 5, 4});
}

FUNK(float32x4_t)
vrev64q_f32(float32x4_t a) {
  return __builtin_shuffle(a, (uint32x4_t){1, 0, 3, 2});
}

FUNK(poly8x16_t)
vrev64q_p8(poly8x16_t a) {
  return __builtin_shuffle(
      a, (uint8x16_t){7, 6, 5, 4, 3, 2, 1, 0, 15, 14, 13, 12, 11, 10, 9, 8});
}

FUNK(poly16x8_t)
vrev64q_p16(poly16x8_t a) {
  return __builtin_shuffle(a, (uint16x8_t){3, 2, 1, 0, 7, 6, 5, 4});
}

FUNK(int8x16_t)
vrev64q_s8(int8x16_t a) {
  return __builtin_shuffle(
      a, (uint8x16_t){7, 6, 5, 4, 3, 2, 1, 0, 15, 14, 13, 12, 11, 10, 9, 8});
}

FUNK(int16x8_t)
vrev64q_s16(int16x8_t a) {
  return __builtin_shuffle(a, (uint16x8_t){3, 2, 1, 0, 7, 6, 5, 4});
}

FUNK(int32x4_t)
vrev64q_s32(int32x4_t a) {
  return __builtin_shuffle(a, (uint32x4_t){1, 0, 3, 2});
}

FUNK(uint8x16_t)
vrev64q_u8(uint8x16_t a) {
  return __builtin_shuffle(
      a, (uint8x16_t){7, 6, 5, 4, 3, 2, 1, 0, 15, 14, 13, 12, 11, 10, 9, 8});
}

FUNK(uint16x8_t)
vrev64q_u16(uint16x8_t a) {
  return __builtin_shuffle(a, (uint16x8_t){3, 2, 1, 0, 7, 6, 5, 4});
}

FUNK(uint32x4_t)
vrev64q_u32(uint32x4_t a) {
  return __builtin_shuffle(a, (uint32x4_t){1, 0, 3, 2});
}

FUNK(float32x2_t)
vrnd_f32(float32x2_t __a) {
  return __builtin_aarch64_btruncv2sf(__a);
}

FUNK(float64x1_t)
vrnd_f64(float64x1_t __a) {
  return vset_lane_f64(__builtin_trunc(vget_lane_f64(__a, 0)), __a, 0);
}

FUNK(float32x4_t)
vrndq_f32(float32x4_t __a) {
  return __builtin_aarch64_btruncv4sf(__a);
}

FUNK(float64x2_t)
vrndq_f64(float64x2_t __a) {
  return __builtin_aarch64_btruncv2df(__a);
}

FUNK(float32x2_t)
vrnda_f32(float32x2_t __a) {
  return __builtin_aarch64_roundv2sf(__a);
}

FUNK(float64x1_t)
vrnda_f64(float64x1_t __a) {
  return vset_lane_f64(__builtin_round(vget_lane_f64(__a, 0)), __a, 0);
}

FUNK(float32x4_t)
vrndaq_f32(float32x4_t __a) {
  return __builtin_aarch64_roundv4sf(__a);
}

FUNK(float64x2_t)
vrndaq_f64(float64x2_t __a) {
  return __builtin_aarch64_roundv2df(__a);
}

FUNK(float32x2_t)
vrndi_f32(float32x2_t __a) {
  return __builtin_aarch64_nearbyintv2sf(__a);
}

FUNK(float64x1_t)
vrndi_f64(float64x1_t __a) {
  return vset_lane_f64(__builtin_nearbyint(vget_lane_f64(__a, 0)), __a, 0);
}

FUNK(float32x4_t)
vrndiq_f32(float32x4_t __a) {
  return __builtin_aarch64_nearbyintv4sf(__a);
}

FUNK(float64x2_t)
vrndiq_f64(float64x2_t __a) {
  return __builtin_aarch64_nearbyintv2df(__a);
}

FUNK(float32x2_t)
vrndm_f32(float32x2_t __a) {
  return __builtin_aarch64_floorv2sf(__a);
}

FUNK(float64x1_t)
vrndm_f64(float64x1_t __a) {
  return vset_lane_f64(__builtin_floor(vget_lane_f64(__a, 0)), __a, 0);
}

FUNK(float32x4_t)
vrndmq_f32(float32x4_t __a) {
  return __builtin_aarch64_floorv4sf(__a);
}

FUNK(float64x2_t)
vrndmq_f64(float64x2_t __a) {
  return __builtin_aarch64_floorv2df(__a);
}

FUNK(float32x2_t)
vrndn_f32(float32x2_t __a) {
  return __builtin_aarch64_frintnv2sf(__a);
}

FUNK(float64x1_t)
vrndn_f64(float64x1_t __a) {
  return (float64x1_t){__builtin_aarch64_frintndf(__a[0])};
}

FUNK(float32x4_t)
vrndnq_f32(float32x4_t __a) {
  return __builtin_aarch64_frintnv4sf(__a);
}

FUNK(float64x2_t)
vrndnq_f64(float64x2_t __a) {
  return __builtin_aarch64_frintnv2df(__a);
}

FUNK(float32x2_t)
vrndp_f32(float32x2_t __a) {
  return __builtin_aarch64_ceilv2sf(__a);
}

FUNK(float64x1_t)
vrndp_f64(float64x1_t __a) {
  return vset_lane_f64(__builtin_ceil(vget_lane_f64(__a, 0)), __a, 0);
}

FUNK(float32x4_t)
vrndpq_f32(float32x4_t __a) {
  return __builtin_aarch64_ceilv4sf(__a);
}

FUNK(float64x2_t)
vrndpq_f64(float64x2_t __a) {
  return __builtin_aarch64_ceilv2df(__a);
}

FUNK(float32x2_t)
vrndx_f32(float32x2_t __a) {
  return __builtin_aarch64_rintv2sf(__a);
}

FUNK(float64x1_t)
vrndx_f64(float64x1_t __a) {
  return vset_lane_f64(__builtin_rint(vget_lane_f64(__a, 0)), __a, 0);
}

FUNK(float32x4_t)
vrndxq_f32(float32x4_t __a) {
  return __builtin_aarch64_rintv4sf(__a);
}

FUNK(float64x2_t)
vrndxq_f64(float64x2_t __a) {
  return __builtin_aarch64_rintv2df(__a);
}

FUNK(int8x8_t)
vrshl_s8(int8x8_t __a, int8x8_t __b) {
  return (int8x8_t)__builtin_aarch64_srshlv8qi(__a, __b);
}

FUNK(int16x4_t)
vrshl_s16(int16x4_t __a, int16x4_t __b) {
  return (int16x4_t)__builtin_aarch64_srshlv4hi(__a, __b);
}

FUNK(int32x2_t)
vrshl_s32(int32x2_t __a, int32x2_t __b) {
  return (int32x2_t)__builtin_aarch64_srshlv2si(__a, __b);
}

FUNK(int64x1_t)
vrshl_s64(int64x1_t __a, int64x1_t __b) {
  return (int64x1_t){__builtin_aarch64_srshldi(__a[0], __b[0])};
}

FUNK(uint8x8_t)
vrshl_u8(uint8x8_t __a, int8x8_t __b) {
  return __builtin_aarch64_urshlv8qi_uus(__a, __b);
}

FUNK(uint16x4_t)
vrshl_u16(uint16x4_t __a, int16x4_t __b) {
  return __builtin_aarch64_urshlv4hi_uus(__a, __b);
}

FUNK(uint32x2_t)
vrshl_u32(uint32x2_t __a, int32x2_t __b) {
  return __builtin_aarch64_urshlv2si_uus(__a, __b);
}

FUNK(uint64x1_t)
vrshl_u64(uint64x1_t __a, int64x1_t __b) {
  return (uint64x1_t){__builtin_aarch64_urshldi_uus(__a[0], __b[0])};
}

FUNK(int8x16_t)
vrshlq_s8(int8x16_t __a, int8x16_t __b) {
  return (int8x16_t)__builtin_aarch64_srshlv16qi(__a, __b);
}

FUNK(int16x8_t)
vrshlq_s16(int16x8_t __a, int16x8_t __b) {
  return (int16x8_t)__builtin_aarch64_srshlv8hi(__a, __b);
}

FUNK(int32x4_t)
vrshlq_s32(int32x4_t __a, int32x4_t __b) {
  return (int32x4_t)__builtin_aarch64_srshlv4si(__a, __b);
}

FUNK(int64x2_t)
vrshlq_s64(int64x2_t __a, int64x2_t __b) {
  return (int64x2_t)__builtin_aarch64_srshlv2di(__a, __b);
}

FUNK(uint8x16_t)
vrshlq_u8(uint8x16_t __a, int8x16_t __b) {
  return __builtin_aarch64_urshlv16qi_uus(__a, __b);
}

FUNK(uint16x8_t)
vrshlq_u16(uint16x8_t __a, int16x8_t __b) {
  return __builtin_aarch64_urshlv8hi_uus(__a, __b);
}

FUNK(uint32x4_t)
vrshlq_u32(uint32x4_t __a, int32x4_t __b) {
  return __builtin_aarch64_urshlv4si_uus(__a, __b);
}

FUNK(uint64x2_t)
vrshlq_u64(uint64x2_t __a, int64x2_t __b) {
  return __builtin_aarch64_urshlv2di_uus(__a, __b);
}

FUNK(int64_t)
vrshld_s64(int64_t __a, int64_t __b) {
  return __builtin_aarch64_srshldi(__a, __b);
}

FUNK(uint64_t)
vrshld_u64(uint64_t __a, int64_t __b) {
  return __builtin_aarch64_urshldi_uus(__a, __b);
}

FUNK(int8x8_t)
vrshr_n_s8(int8x8_t __a, const int __b) {
  return (int8x8_t)__builtin_aarch64_srshr_nv8qi(__a, __b);
}

FUNK(int16x4_t)
vrshr_n_s16(int16x4_t __a, const int __b) {
  return (int16x4_t)__builtin_aarch64_srshr_nv4hi(__a, __b);
}

FUNK(int32x2_t)
vrshr_n_s32(int32x2_t __a, const int __b) {
  return (int32x2_t)__builtin_aarch64_srshr_nv2si(__a, __b);
}

FUNK(int64x1_t)
vrshr_n_s64(int64x1_t __a, const int __b) {
  return (int64x1_t){__builtin_aarch64_srshr_ndi(__a[0], __b)};
}

FUNK(uint8x8_t)
vrshr_n_u8(uint8x8_t __a, const int __b) {
  return __builtin_aarch64_urshr_nv8qi_uus(__a, __b);
}

FUNK(uint16x4_t)
vrshr_n_u16(uint16x4_t __a, const int __b) {
  return __builtin_aarch64_urshr_nv4hi_uus(__a, __b);
}

FUNK(uint32x2_t)
vrshr_n_u32(uint32x2_t __a, const int __b) {
  return __builtin_aarch64_urshr_nv2si_uus(__a, __b);
}

FUNK(uint64x1_t)
vrshr_n_u64(uint64x1_t __a, const int __b) {
  return (uint64x1_t){__builtin_aarch64_urshr_ndi_uus(__a[0], __b)};
}

FUNK(int8x16_t)
vrshrq_n_s8(int8x16_t __a, const int __b) {
  return (int8x16_t)__builtin_aarch64_srshr_nv16qi(__a, __b);
}

FUNK(int16x8_t)
vrshrq_n_s16(int16x8_t __a, const int __b) {
  return (int16x8_t)__builtin_aarch64_srshr_nv8hi(__a, __b);
}

FUNK(int32x4_t)
vrshrq_n_s32(int32x4_t __a, const int __b) {
  return (int32x4_t)__builtin_aarch64_srshr_nv4si(__a, __b);
}

FUNK(int64x2_t)
vrshrq_n_s64(int64x2_t __a, const int __b) {
  return (int64x2_t)__builtin_aarch64_srshr_nv2di(__a, __b);
}

FUNK(uint8x16_t)
vrshrq_n_u8(uint8x16_t __a, const int __b) {
  return __builtin_aarch64_urshr_nv16qi_uus(__a, __b);
}

FUNK(uint16x8_t)
vrshrq_n_u16(uint16x8_t __a, const int __b) {
  return __builtin_aarch64_urshr_nv8hi_uus(__a, __b);
}

FUNK(uint32x4_t)
vrshrq_n_u32(uint32x4_t __a, const int __b) {
  return __builtin_aarch64_urshr_nv4si_uus(__a, __b);
}

FUNK(uint64x2_t)
vrshrq_n_u64(uint64x2_t __a, const int __b) {
  return __builtin_aarch64_urshr_nv2di_uus(__a, __b);
}

FUNK(int64_t)
vrshrd_n_s64(int64_t __a, const int __b) {
  return __builtin_aarch64_srshr_ndi(__a, __b);
}

FUNK(uint64_t)
vrshrd_n_u64(uint64_t __a, const int __b) {
  return __builtin_aarch64_urshr_ndi_uus(__a, __b);
}

FUNK(float32_t)
vrsqrtes_f32(float32_t __a) {
  return __builtin_aarch64_rsqrtesf(__a);
}

FUNK(float64_t)
vrsqrted_f64(float64_t __a) {
  return __builtin_aarch64_rsqrtedf(__a);
}

FUNK(float32x2_t)
vrsqrte_f32(float32x2_t __a) {
  return __builtin_aarch64_rsqrtev2sf(__a);
}

FUNK(float64x1_t)
vrsqrte_f64(float64x1_t __a) {
  return (float64x1_t){vrsqrted_f64(vget_lane_f64(__a, 0))};
}

FUNK(float32x4_t)
vrsqrteq_f32(float32x4_t __a) {
  return __builtin_aarch64_rsqrtev4sf(__a);
}

FUNK(float64x2_t)
vrsqrteq_f64(float64x2_t __a) {
  return __builtin_aarch64_rsqrtev2df(__a);
}

FUNK(float32_t)
vrsqrtss_f32(float32_t __a, float32_t __b) {
  return __builtin_aarch64_rsqrtssf(__a, __b);
}

FUNK(float64_t)
vrsqrtsd_f64(float64_t __a, float64_t __b) {
  return __builtin_aarch64_rsqrtsdf(__a, __b);
}

FUNK(float32x2_t)
vrsqrts_f32(float32x2_t __a, float32x2_t __b) {
  return __builtin_aarch64_rsqrtsv2sf(__a, __b);
}

FUNK(float64x1_t)
vrsqrts_f64(float64x1_t __a, float64x1_t __b) {
  return (float64x1_t){
      vrsqrtsd_f64(vget_lane_f64(__a, 0), vget_lane_f64(__b, 0))};
}

FUNK(float32x4_t)
vrsqrtsq_f32(float32x4_t __a, float32x4_t __b) {
  return __builtin_aarch64_rsqrtsv4sf(__a, __b);
}

FUNK(float64x2_t)
vrsqrtsq_f64(float64x2_t __a, float64x2_t __b) {
  return __builtin_aarch64_rsqrtsv2df(__a, __b);
}

FUNK(int8x8_t)
vrsra_n_s8(int8x8_t __a, int8x8_t __b, const int __c) {
  return (int8x8_t)__builtin_aarch64_srsra_nv8qi(__a, __b, __c);
}

FUNK(int16x4_t)
vrsra_n_s16(int16x4_t __a, int16x4_t __b, const int __c) {
  return (int16x4_t)__builtin_aarch64_srsra_nv4hi(__a, __b, __c);
}

FUNK(int32x2_t)
vrsra_n_s32(int32x2_t __a, int32x2_t __b, const int __c) {
  return (int32x2_t)__builtin_aarch64_srsra_nv2si(__a, __b, __c);
}

FUNK(int64x1_t)
vrsra_n_s64(int64x1_t __a, int64x1_t __b, const int __c) {
  return (int64x1_t){__builtin_aarch64_srsra_ndi(__a[0], __b[0], __c)};
}

FUNK(uint8x8_t)
vrsra_n_u8(uint8x8_t __a, uint8x8_t __b, const int __c) {
  return __builtin_aarch64_ursra_nv8qi_uuus(__a, __b, __c);
}

FUNK(uint16x4_t)
vrsra_n_u16(uint16x4_t __a, uint16x4_t __b, const int __c) {
  return __builtin_aarch64_ursra_nv4hi_uuus(__a, __b, __c);
}

FUNK(uint32x2_t)
vrsra_n_u32(uint32x2_t __a, uint32x2_t __b, const int __c) {
  return __builtin_aarch64_ursra_nv2si_uuus(__a, __b, __c);
}

FUNK(uint64x1_t)
vrsra_n_u64(uint64x1_t __a, uint64x1_t __b, const int __c) {
  return (uint64x1_t){__builtin_aarch64_ursra_ndi_uuus(__a[0], __b[0], __c)};
}

FUNK(int8x16_t)
vrsraq_n_s8(int8x16_t __a, int8x16_t __b, const int __c) {
  return (int8x16_t)__builtin_aarch64_srsra_nv16qi(__a, __b, __c);
}

FUNK(int16x8_t)
vrsraq_n_s16(int16x8_t __a, int16x8_t __b, const int __c) {
  return (int16x8_t)__builtin_aarch64_srsra_nv8hi(__a, __b, __c);
}

FUNK(int32x4_t)
vrsraq_n_s32(int32x4_t __a, int32x4_t __b, const int __c) {
  return (int32x4_t)__builtin_aarch64_srsra_nv4si(__a, __b, __c);
}

FUNK(int64x2_t)
vrsraq_n_s64(int64x2_t __a, int64x2_t __b, const int __c) {
  return (int64x2_t)__builtin_aarch64_srsra_nv2di(__a, __b, __c);
}

FUNK(uint8x16_t)
vrsraq_n_u8(uint8x16_t __a, uint8x16_t __b, const int __c) {
  return __builtin_aarch64_ursra_nv16qi_uuus(__a, __b, __c);
}

FUNK(uint16x8_t)
vrsraq_n_u16(uint16x8_t __a, uint16x8_t __b, const int __c) {
  return __builtin_aarch64_ursra_nv8hi_uuus(__a, __b, __c);
}

FUNK(uint32x4_t)
vrsraq_n_u32(uint32x4_t __a, uint32x4_t __b, const int __c) {
  return __builtin_aarch64_ursra_nv4si_uuus(__a, __b, __c);
}

FUNK(uint64x2_t)
vrsraq_n_u64(uint64x2_t __a, uint64x2_t __b, const int __c) {
  return __builtin_aarch64_ursra_nv2di_uuus(__a, __b, __c);
}

FUNK(int64_t)
vrsrad_n_s64(int64_t __a, int64_t __b, const int __c) {
  return __builtin_aarch64_srsra_ndi(__a, __b, __c);
}

FUNK(uint64_t)
vrsrad_n_u64(uint64_t __a, uint64_t __b, const int __c) {
  return __builtin_aarch64_ursra_ndi_uuus(__a, __b, __c);
}

#pragma GCC push_options
#pragma GCC target("+nothing+crypto")

FUNK(uint32x4_t)
vsha1cq_u32(uint32x4_t hash_abcd, uint32_t hash_e, uint32x4_t wk) {
  return __builtin_aarch64_crypto_sha1cv4si_uuuu(hash_abcd, hash_e, wk);
}

FUNK(uint32x4_t)
vsha1mq_u32(uint32x4_t hash_abcd, uint32_t hash_e, uint32x4_t wk) {
  return __builtin_aarch64_crypto_sha1mv4si_uuuu(hash_abcd, hash_e, wk);
}

FUNK(uint32x4_t)
vsha1pq_u32(uint32x4_t hash_abcd, uint32_t hash_e, uint32x4_t wk) {
  return __builtin_aarch64_crypto_sha1pv4si_uuuu(hash_abcd, hash_e, wk);
}

FUNK(uint32_t)
vsha1h_u32(uint32_t hash_e) {
  return __builtin_aarch64_crypto_sha1hsi_uu(hash_e);
}

FUNK(uint32x4_t)
vsha1su0q_u32(uint32x4_t w0_3, uint32x4_t w4_7, uint32x4_t w8_11) {
  return __builtin_aarch64_crypto_sha1su0v4si_uuuu(w0_3, w4_7, w8_11);
}

FUNK(uint32x4_t)
vsha1su1q_u32(uint32x4_t tw0_3, uint32x4_t w12_15) {
  return __builtin_aarch64_crypto_sha1su1v4si_uuu(tw0_3, w12_15);
}

FUNK(uint32x4_t)
vsha256hq_u32(uint32x4_t hash_abcd, uint32x4_t hash_efgh, uint32x4_t wk) {
  return __builtin_aarch64_crypto_sha256hv4si_uuuu(hash_abcd, hash_efgh, wk);
}

FUNK(uint32x4_t)
vsha256h2q_u32(uint32x4_t hash_efgh, uint32x4_t hash_abcd, uint32x4_t wk) {
  return __builtin_aarch64_crypto_sha256h2v4si_uuuu(hash_efgh, hash_abcd, wk);
}

FUNK(uint32x4_t)
vsha256su0q_u32(uint32x4_t w0_3, uint32x4_t w4_7) {
  return __builtin_aarch64_crypto_sha256su0v4si_uuu(w0_3, w4_7);
}

FUNK(uint32x4_t)
vsha256su1q_u32(uint32x4_t tw0_3, uint32x4_t w8_11, uint32x4_t w12_15) {
  return __builtin_aarch64_crypto_sha256su1v4si_uuuu(tw0_3, w8_11, w12_15);
}

FUNK(poly128_t)
vmull_p64(poly64_t a, poly64_t b) {
  return __builtin_aarch64_crypto_pmulldi_ppp(a, b);
}

FUNK(poly128_t)
vmull_high_p64(poly64x2_t a, poly64x2_t b) {
  return __builtin_aarch64_crypto_pmullv2di_ppp(a, b);
}

#pragma GCC pop_options

FUNK(int8x8_t)
vshl_n_s8(int8x8_t __a, const int __b) {
  return (int8x8_t)__builtin_aarch64_ashlv8qi(__a, __b);
}

FUNK(int16x4_t)
vshl_n_s16(int16x4_t __a, const int __b) {
  return (int16x4_t)__builtin_aarch64_ashlv4hi(__a, __b);
}

FUNK(int32x2_t)
vshl_n_s32(int32x2_t __a, const int __b) {
  return (int32x2_t)__builtin_aarch64_ashlv2si(__a, __b);
}

FUNK(int64x1_t)
vshl_n_s64(int64x1_t __a, const int __b) {
  return (int64x1_t){__builtin_aarch64_ashldi(__a[0], __b)};
}

FUNK(uint8x8_t)
vshl_n_u8(uint8x8_t __a, const int __b) {
  return (uint8x8_t)__builtin_aarch64_ashlv8qi((int8x8_t)__a, __b);
}

FUNK(uint16x4_t)
vshl_n_u16(uint16x4_t __a, const int __b) {
  return (uint16x4_t)__builtin_aarch64_ashlv4hi((int16x4_t)__a, __b);
}

FUNK(uint32x2_t)
vshl_n_u32(uint32x2_t __a, const int __b) {
  return (uint32x2_t)__builtin_aarch64_ashlv2si((int32x2_t)__a, __b);
}

FUNK(uint64x1_t)
vshl_n_u64(uint64x1_t __a, const int __b) {
  return (uint64x1_t){__builtin_aarch64_ashldi((int64_t)__a[0], __b)};
}

FUNK(int8x16_t)
vshlq_n_s8(int8x16_t __a, const int __b) {
  return (int8x16_t)__builtin_aarch64_ashlv16qi(__a, __b);
}

FUNK(int16x8_t)
vshlq_n_s16(int16x8_t __a, const int __b) {
  return (int16x8_t)__builtin_aarch64_ashlv8hi(__a, __b);
}

FUNK(int32x4_t)
vshlq_n_s32(int32x4_t __a, const int __b) {
  return (int32x4_t)__builtin_aarch64_ashlv4si(__a, __b);
}

FUNK(int64x2_t)
vshlq_n_s64(int64x2_t __a, const int __b) {
  return (int64x2_t)__builtin_aarch64_ashlv2di(__a, __b);
}

FUNK(uint8x16_t)
vshlq_n_u8(uint8x16_t __a, const int __b) {
  return (uint8x16_t)__builtin_aarch64_ashlv16qi((int8x16_t)__a, __b);
}

FUNK(uint16x8_t)
vshlq_n_u16(uint16x8_t __a, const int __b) {
  return (uint16x8_t)__builtin_aarch64_ashlv8hi((int16x8_t)__a, __b);
}

FUNK(uint32x4_t)
vshlq_n_u32(uint32x4_t __a, const int __b) {
  return (uint32x4_t)__builtin_aarch64_ashlv4si((int32x4_t)__a, __b);
}

FUNK(uint64x2_t)
vshlq_n_u64(uint64x2_t __a, const int __b) {
  return (uint64x2_t)__builtin_aarch64_ashlv2di((int64x2_t)__a, __b);
}

FUNK(int64_t)
vshld_n_s64(int64_t __a, const int __b) {
  return __builtin_aarch64_ashldi(__a, __b);
}

FUNK(uint64_t)
vshld_n_u64(uint64_t __a, const int __b) {
  return (uint64_t)__builtin_aarch64_ashldi(__a, __b);
}

FUNK(int8x8_t)
vshl_s8(int8x8_t __a, int8x8_t __b) {
  return __builtin_aarch64_sshlv8qi(__a, __b);
}

FUNK(int16x4_t)
vshl_s16(int16x4_t __a, int16x4_t __b) {
  return __builtin_aarch64_sshlv4hi(__a, __b);
}

FUNK(int32x2_t)
vshl_s32(int32x2_t __a, int32x2_t __b) {
  return __builtin_aarch64_sshlv2si(__a, __b);
}

FUNK(int64x1_t)
vshl_s64(int64x1_t __a, int64x1_t __b) {
  return (int64x1_t){__builtin_aarch64_sshldi(__a[0], __b[0])};
}

FUNK(uint8x8_t)
vshl_u8(uint8x8_t __a, int8x8_t __b) {
  return __builtin_aarch64_ushlv8qi_uus(__a, __b);
}

FUNK(uint16x4_t)
vshl_u16(uint16x4_t __a, int16x4_t __b) {
  return __builtin_aarch64_ushlv4hi_uus(__a, __b);
}

FUNK(uint32x2_t)
vshl_u32(uint32x2_t __a, int32x2_t __b) {
  return __builtin_aarch64_ushlv2si_uus(__a, __b);
}

FUNK(uint64x1_t)
vshl_u64(uint64x1_t __a, int64x1_t __b) {
  return (uint64x1_t){__builtin_aarch64_ushldi_uus(__a[0], __b[0])};
}

FUNK(int8x16_t)
vshlq_s8(int8x16_t __a, int8x16_t __b) {
  return __builtin_aarch64_sshlv16qi(__a, __b);
}

FUNK(int16x8_t)
vshlq_s16(int16x8_t __a, int16x8_t __b) {
  return __builtin_aarch64_sshlv8hi(__a, __b);
}

FUNK(int32x4_t)
vshlq_s32(int32x4_t __a, int32x4_t __b) {
  return __builtin_aarch64_sshlv4si(__a, __b);
}

FUNK(int64x2_t)
vshlq_s64(int64x2_t __a, int64x2_t __b) {
  return __builtin_aarch64_sshlv2di(__a, __b);
}

FUNK(uint8x16_t)
vshlq_u8(uint8x16_t __a, int8x16_t __b) {
  return __builtin_aarch64_ushlv16qi_uus(__a, __b);
}

FUNK(uint16x8_t)
vshlq_u16(uint16x8_t __a, int16x8_t __b) {
  return __builtin_aarch64_ushlv8hi_uus(__a, __b);
}

FUNK(uint32x4_t)
vshlq_u32(uint32x4_t __a, int32x4_t __b) {
  return __builtin_aarch64_ushlv4si_uus(__a, __b);
}

FUNK(uint64x2_t)
vshlq_u64(uint64x2_t __a, int64x2_t __b) {
  return __builtin_aarch64_ushlv2di_uus(__a, __b);
}

FUNK(int64_t)
vshld_s64(int64_t __a, int64_t __b) {
  return __builtin_aarch64_sshldi(__a, __b);
}

FUNK(uint64_t)
vshld_u64(uint64_t __a, uint64_t __b) {
  return __builtin_aarch64_ushldi_uus(__a, __b);
}

FUNK(int16x8_t)
vshll_high_n_s8(int8x16_t __a, const int __b) {
  return __builtin_aarch64_sshll2_nv16qi(__a, __b);
}

FUNK(int32x4_t)
vshll_high_n_s16(int16x8_t __a, const int __b) {
  return __builtin_aarch64_sshll2_nv8hi(__a, __b);
}

FUNK(int64x2_t)
vshll_high_n_s32(int32x4_t __a, const int __b) {
  return __builtin_aarch64_sshll2_nv4si(__a, __b);
}

FUNK(uint16x8_t)
vshll_high_n_u8(uint8x16_t __a, const int __b) {
  return (uint16x8_t)__builtin_aarch64_ushll2_nv16qi((int8x16_t)__a, __b);
}

FUNK(uint32x4_t)
vshll_high_n_u16(uint16x8_t __a, const int __b) {
  return (uint32x4_t)__builtin_aarch64_ushll2_nv8hi((int16x8_t)__a, __b);
}

FUNK(uint64x2_t)
vshll_high_n_u32(uint32x4_t __a, const int __b) {
  return (uint64x2_t)__builtin_aarch64_ushll2_nv4si((int32x4_t)__a, __b);
}

FUNK(int16x8_t)
vshll_n_s8(int8x8_t __a, const int __b) {
  return __builtin_aarch64_sshll_nv8qi(__a, __b);
}

FUNK(int32x4_t)
vshll_n_s16(int16x4_t __a, const int __b) {
  return __builtin_aarch64_sshll_nv4hi(__a, __b);
}

FUNK(int64x2_t)
vshll_n_s32(int32x2_t __a, const int __b) {
  return __builtin_aarch64_sshll_nv2si(__a, __b);
}

FUNK(uint16x8_t)
vshll_n_u8(uint8x8_t __a, const int __b) {
  return __builtin_aarch64_ushll_nv8qi_uus(__a, __b);
}

FUNK(uint32x4_t)
vshll_n_u16(uint16x4_t __a, const int __b) {
  return __builtin_aarch64_ushll_nv4hi_uus(__a, __b);
}

FUNK(uint64x2_t)
vshll_n_u32(uint32x2_t __a, const int __b) {
  return __builtin_aarch64_ushll_nv2si_uus(__a, __b);
}

FUNK(int8x8_t)
vshr_n_s8(int8x8_t __a, const int __b) {
  return (int8x8_t)__builtin_aarch64_ashrv8qi(__a, __b);
}

FUNK(int16x4_t)
vshr_n_s16(int16x4_t __a, const int __b) {
  return (int16x4_t)__builtin_aarch64_ashrv4hi(__a, __b);
}

FUNK(int32x2_t)
vshr_n_s32(int32x2_t __a, const int __b) {
  return (int32x2_t)__builtin_aarch64_ashrv2si(__a, __b);
}

FUNK(int64x1_t)
vshr_n_s64(int64x1_t __a, const int __b) {
  return (int64x1_t){__builtin_aarch64_ashr_simddi(__a[0], __b)};
}

FUNK(uint8x8_t)
vshr_n_u8(uint8x8_t __a, const int __b) {
  return (uint8x8_t)__builtin_aarch64_lshrv8qi((int8x8_t)__a, __b);
}

FUNK(uint16x4_t)
vshr_n_u16(uint16x4_t __a, const int __b) {
  return (uint16x4_t)__builtin_aarch64_lshrv4hi((int16x4_t)__a, __b);
}

FUNK(uint32x2_t)
vshr_n_u32(uint32x2_t __a, const int __b) {
  return (uint32x2_t)__builtin_aarch64_lshrv2si((int32x2_t)__a, __b);
}

FUNK(uint64x1_t)
vshr_n_u64(uint64x1_t __a, const int __b) {
  return (uint64x1_t){__builtin_aarch64_lshr_simddi_uus(__a[0], __b)};
}

FUNK(int8x16_t)
vshrq_n_s8(int8x16_t __a, const int __b) {
  return (int8x16_t)__builtin_aarch64_ashrv16qi(__a, __b);
}

FUNK(int16x8_t)
vshrq_n_s16(int16x8_t __a, const int __b) {
  return (int16x8_t)__builtin_aarch64_ashrv8hi(__a, __b);
}

FUNK(int32x4_t)
vshrq_n_s32(int32x4_t __a, const int __b) {
  return (int32x4_t)__builtin_aarch64_ashrv4si(__a, __b);
}

FUNK(int64x2_t)
vshrq_n_s64(int64x2_t __a, const int __b) {
  return (int64x2_t)__builtin_aarch64_ashrv2di(__a, __b);
}

FUNK(uint8x16_t)
vshrq_n_u8(uint8x16_t __a, const int __b) {
  return (uint8x16_t)__builtin_aarch64_lshrv16qi((int8x16_t)__a, __b);
}

FUNK(uint16x8_t)
vshrq_n_u16(uint16x8_t __a, const int __b) {
  return (uint16x8_t)__builtin_aarch64_lshrv8hi((int16x8_t)__a, __b);
}

FUNK(uint32x4_t)
vshrq_n_u32(uint32x4_t __a, const int __b) {
  return (uint32x4_t)__builtin_aarch64_lshrv4si((int32x4_t)__a, __b);
}

FUNK(uint64x2_t)
vshrq_n_u64(uint64x2_t __a, const int __b) {
  return (uint64x2_t)__builtin_aarch64_lshrv2di((int64x2_t)__a, __b);
}

FUNK(int64_t)
vshrd_n_s64(int64_t __a, const int __b) {
  return __builtin_aarch64_ashr_simddi(__a, __b);
}

FUNK(uint64_t)
vshrd_n_u64(uint64_t __a, const int __b) {
  return __builtin_aarch64_lshr_simddi_uus(__a, __b);
}

FUNK(int8x8_t)
vsli_n_s8(int8x8_t __a, int8x8_t __b, const int __c) {
  return (int8x8_t)__builtin_aarch64_ssli_nv8qi(__a, __b, __c);
}

FUNK(int16x4_t)
vsli_n_s16(int16x4_t __a, int16x4_t __b, const int __c) {
  return (int16x4_t)__builtin_aarch64_ssli_nv4hi(__a, __b, __c);
}

FUNK(int32x2_t)
vsli_n_s32(int32x2_t __a, int32x2_t __b, const int __c) {
  return (int32x2_t)__builtin_aarch64_ssli_nv2si(__a, __b, __c);
}

FUNK(int64x1_t)
vsli_n_s64(int64x1_t __a, int64x1_t __b, const int __c) {
  return (int64x1_t){__builtin_aarch64_ssli_ndi(__a[0], __b[0], __c)};
}

FUNK(uint8x8_t)
vsli_n_u8(uint8x8_t __a, uint8x8_t __b, const int __c) {
  return __builtin_aarch64_usli_nv8qi_uuus(__a, __b, __c);
}

FUNK(uint16x4_t)
vsli_n_u16(uint16x4_t __a, uint16x4_t __b, const int __c) {
  return __builtin_aarch64_usli_nv4hi_uuus(__a, __b, __c);
}

FUNK(uint32x2_t)
vsli_n_u32(uint32x2_t __a, uint32x2_t __b, const int __c) {
  return __builtin_aarch64_usli_nv2si_uuus(__a, __b, __c);
}

FUNK(uint64x1_t)
vsli_n_u64(uint64x1_t __a, uint64x1_t __b, const int __c) {
  return (uint64x1_t){__builtin_aarch64_usli_ndi_uuus(__a[0], __b[0], __c)};
}

FUNK(poly64x1_t)
vsli_n_p64(poly64x1_t __a, poly64x1_t __b, const int __c) {
  return (poly64x1_t){__builtin_aarch64_ssli_ndi_ppps(__a[0], __b[0], __c)};
}

FUNK(int8x16_t)
vsliq_n_s8(int8x16_t __a, int8x16_t __b, const int __c) {
  return (int8x16_t)__builtin_aarch64_ssli_nv16qi(__a, __b, __c);
}

FUNK(int16x8_t)
vsliq_n_s16(int16x8_t __a, int16x8_t __b, const int __c) {
  return (int16x8_t)__builtin_aarch64_ssli_nv8hi(__a, __b, __c);
}

FUNK(int32x4_t)
vsliq_n_s32(int32x4_t __a, int32x4_t __b, const int __c) {
  return (int32x4_t)__builtin_aarch64_ssli_nv4si(__a, __b, __c);
}

FUNK(int64x2_t)
vsliq_n_s64(int64x2_t __a, int64x2_t __b, const int __c) {
  return (int64x2_t)__builtin_aarch64_ssli_nv2di(__a, __b, __c);
}

FUNK(uint8x16_t)
vsliq_n_u8(uint8x16_t __a, uint8x16_t __b, const int __c) {
  return __builtin_aarch64_usli_nv16qi_uuus(__a, __b, __c);
}

FUNK(uint16x8_t)
vsliq_n_u16(uint16x8_t __a, uint16x8_t __b, const int __c) {
  return __builtin_aarch64_usli_nv8hi_uuus(__a, __b, __c);
}

FUNK(uint32x4_t)
vsliq_n_u32(uint32x4_t __a, uint32x4_t __b, const int __c) {
  return __builtin_aarch64_usli_nv4si_uuus(__a, __b, __c);
}

FUNK(uint64x2_t)
vsliq_n_u64(uint64x2_t __a, uint64x2_t __b, const int __c) {
  return __builtin_aarch64_usli_nv2di_uuus(__a, __b, __c);
}

FUNK(poly64x2_t)
vsliq_n_p64(poly64x2_t __a, poly64x2_t __b, const int __c) {
  return __builtin_aarch64_ssli_nv2di_ppps(__a, __b, __c);
}

FUNK(int64_t)
vslid_n_s64(int64_t __a, int64_t __b, const int __c) {
  return __builtin_aarch64_ssli_ndi(__a, __b, __c);
}

FUNK(uint64_t)
vslid_n_u64(uint64_t __a, uint64_t __b, const int __c) {
  return __builtin_aarch64_usli_ndi_uuus(__a, __b, __c);
}

FUNK(uint8x8_t)
vsqadd_u8(uint8x8_t __a, int8x8_t __b) {
  return __builtin_aarch64_usqaddv8qi_uus(__a, __b);
}

FUNK(uint16x4_t)
vsqadd_u16(uint16x4_t __a, int16x4_t __b) {
  return __builtin_aarch64_usqaddv4hi_uus(__a, __b);
}

FUNK(uint32x2_t)
vsqadd_u32(uint32x2_t __a, int32x2_t __b) {
  return __builtin_aarch64_usqaddv2si_uus(__a, __b);
}

FUNK(uint64x1_t)
vsqadd_u64(uint64x1_t __a, int64x1_t __b) {
  return (uint64x1_t){__builtin_aarch64_usqadddi_uus(__a[0], __b[0])};
}

FUNK(uint8x16_t)
vsqaddq_u8(uint8x16_t __a, int8x16_t __b) {
  return __builtin_aarch64_usqaddv16qi_uus(__a, __b);
}

FUNK(uint16x8_t)
vsqaddq_u16(uint16x8_t __a, int16x8_t __b) {
  return __builtin_aarch64_usqaddv8hi_uus(__a, __b);
}

FUNK(uint32x4_t)
vsqaddq_u32(uint32x4_t __a, int32x4_t __b) {
  return __builtin_aarch64_usqaddv4si_uus(__a, __b);
}

FUNK(uint64x2_t)
vsqaddq_u64(uint64x2_t __a, int64x2_t __b) {
  return __builtin_aarch64_usqaddv2di_uus(__a, __b);
}

FUNK(uint8_t)
vsqaddb_u8(uint8_t __a, int8_t __b) {
  return __builtin_aarch64_usqaddqi_uus(__a, __b);
}

FUNK(uint16_t)
vsqaddh_u16(uint16_t __a, int16_t __b) {
  return __builtin_aarch64_usqaddhi_uus(__a, __b);
}

FUNK(uint32_t)
vsqadds_u32(uint32_t __a, int32_t __b) {
  return __builtin_aarch64_usqaddsi_uus(__a, __b);
}

FUNK(uint64_t)
vsqaddd_u64(uint64_t __a, int64_t __b) {
  return __builtin_aarch64_usqadddi_uus(__a, __b);
}

FUNK(float32x2_t)
vsqrt_f32(float32x2_t a) {
  return __builtin_aarch64_sqrtv2sf(a);
}

FUNK(float32x4_t)
vsqrtq_f32(float32x4_t a) {
  return __builtin_aarch64_sqrtv4sf(a);
}

FUNK(float64x1_t)
vsqrt_f64(float64x1_t a) {
  return (float64x1_t){__builtin_aarch64_sqrtdf(a[0])};
}

FUNK(float64x2_t)
vsqrtq_f64(float64x2_t a) {
  return __builtin_aarch64_sqrtv2df(a);
}

FUNK(int8x8_t)
vsra_n_s8(int8x8_t __a, int8x8_t __b, const int __c) {
  return (int8x8_t)__builtin_aarch64_ssra_nv8qi(__a, __b, __c);
}

FUNK(int16x4_t)
vsra_n_s16(int16x4_t __a, int16x4_t __b, const int __c) {
  return (int16x4_t)__builtin_aarch64_ssra_nv4hi(__a, __b, __c);
}

FUNK(int32x2_t)
vsra_n_s32(int32x2_t __a, int32x2_t __b, const int __c) {
  return (int32x2_t)__builtin_aarch64_ssra_nv2si(__a, __b, __c);
}

FUNK(int64x1_t)
vsra_n_s64(int64x1_t __a, int64x1_t __b, const int __c) {
  return (int64x1_t){__builtin_aarch64_ssra_ndi(__a[0], __b[0], __c)};
}

FUNK(uint8x8_t)
vsra_n_u8(uint8x8_t __a, uint8x8_t __b, const int __c) {
  return __builtin_aarch64_usra_nv8qi_uuus(__a, __b, __c);
}

FUNK(uint16x4_t)
vsra_n_u16(uint16x4_t __a, uint16x4_t __b, const int __c) {
  return __builtin_aarch64_usra_nv4hi_uuus(__a, __b, __c);
}

FUNK(uint32x2_t)
vsra_n_u32(uint32x2_t __a, uint32x2_t __b, const int __c) {
  return __builtin_aarch64_usra_nv2si_uuus(__a, __b, __c);
}

FUNK(uint64x1_t)
vsra_n_u64(uint64x1_t __a, uint64x1_t __b, const int __c) {
  return (uint64x1_t){__builtin_aarch64_usra_ndi_uuus(__a[0], __b[0], __c)};
}

FUNK(int8x16_t)
vsraq_n_s8(int8x16_t __a, int8x16_t __b, const int __c) {
  return (int8x16_t)__builtin_aarch64_ssra_nv16qi(__a, __b, __c);
}

FUNK(int16x8_t)
vsraq_n_s16(int16x8_t __a, int16x8_t __b, const int __c) {
  return (int16x8_t)__builtin_aarch64_ssra_nv8hi(__a, __b, __c);
}

FUNK(int32x4_t)
vsraq_n_s32(int32x4_t __a, int32x4_t __b, const int __c) {
  return (int32x4_t)__builtin_aarch64_ssra_nv4si(__a, __b, __c);
}

FUNK(int64x2_t)
vsraq_n_s64(int64x2_t __a, int64x2_t __b, const int __c) {
  return (int64x2_t)__builtin_aarch64_ssra_nv2di(__a, __b, __c);
}

FUNK(uint8x16_t)
vsraq_n_u8(uint8x16_t __a, uint8x16_t __b, const int __c) {
  return __builtin_aarch64_usra_nv16qi_uuus(__a, __b, __c);
}

FUNK(uint16x8_t)
vsraq_n_u16(uint16x8_t __a, uint16x8_t __b, const int __c) {
  return __builtin_aarch64_usra_nv8hi_uuus(__a, __b, __c);
}

FUNK(uint32x4_t)
vsraq_n_u32(uint32x4_t __a, uint32x4_t __b, const int __c) {
  return __builtin_aarch64_usra_nv4si_uuus(__a, __b, __c);
}

FUNK(uint64x2_t)
vsraq_n_u64(uint64x2_t __a, uint64x2_t __b, const int __c) {
  return __builtin_aarch64_usra_nv2di_uuus(__a, __b, __c);
}

FUNK(int64_t)
vsrad_n_s64(int64_t __a, int64_t __b, const int __c) {
  return __builtin_aarch64_ssra_ndi(__a, __b, __c);
}

FUNK(uint64_t)
vsrad_n_u64(uint64_t __a, uint64_t __b, const int __c) {
  return __builtin_aarch64_usra_ndi_uuus(__a, __b, __c);
}

FUNK(int8x8_t)
vsri_n_s8(int8x8_t __a, int8x8_t __b, const int __c) {
  return (int8x8_t)__builtin_aarch64_ssri_nv8qi(__a, __b, __c);
}

FUNK(int16x4_t)
vsri_n_s16(int16x4_t __a, int16x4_t __b, const int __c) {
  return (int16x4_t)__builtin_aarch64_ssri_nv4hi(__a, __b, __c);
}

FUNK(int32x2_t)
vsri_n_s32(int32x2_t __a, int32x2_t __b, const int __c) {
  return (int32x2_t)__builtin_aarch64_ssri_nv2si(__a, __b, __c);
}

FUNK(int64x1_t)
vsri_n_s64(int64x1_t __a, int64x1_t __b, const int __c) {
  return (int64x1_t){__builtin_aarch64_ssri_ndi(__a[0], __b[0], __c)};
}

FUNK(uint8x8_t)
vsri_n_u8(uint8x8_t __a, uint8x8_t __b, const int __c) {
  return __builtin_aarch64_usri_nv8qi_uuus(__a, __b, __c);
}

FUNK(uint16x4_t)
vsri_n_u16(uint16x4_t __a, uint16x4_t __b, const int __c) {
  return __builtin_aarch64_usri_nv4hi_uuus(__a, __b, __c);
}

FUNK(uint32x2_t)
vsri_n_u32(uint32x2_t __a, uint32x2_t __b, const int __c) {
  return __builtin_aarch64_usri_nv2si_uuus(__a, __b, __c);
}

FUNK(uint64x1_t)
vsri_n_u64(uint64x1_t __a, uint64x1_t __b, const int __c) {
  return (uint64x1_t){__builtin_aarch64_usri_ndi_uuus(__a[0], __b[0], __c)};
}

FUNK(int8x16_t)
vsriq_n_s8(int8x16_t __a, int8x16_t __b, const int __c) {
  return (int8x16_t)__builtin_aarch64_ssri_nv16qi(__a, __b, __c);
}

FUNK(int16x8_t)
vsriq_n_s16(int16x8_t __a, int16x8_t __b, const int __c) {
  return (int16x8_t)__builtin_aarch64_ssri_nv8hi(__a, __b, __c);
}

FUNK(int32x4_t)
vsriq_n_s32(int32x4_t __a, int32x4_t __b, const int __c) {
  return (int32x4_t)__builtin_aarch64_ssri_nv4si(__a, __b, __c);
}

FUNK(int64x2_t)
vsriq_n_s64(int64x2_t __a, int64x2_t __b, const int __c) {
  return (int64x2_t)__builtin_aarch64_ssri_nv2di(__a, __b, __c);
}

FUNK(uint8x16_t)
vsriq_n_u8(uint8x16_t __a, uint8x16_t __b, const int __c) {
  return __builtin_aarch64_usri_nv16qi_uuus(__a, __b, __c);
}

FUNK(uint16x8_t)
vsriq_n_u16(uint16x8_t __a, uint16x8_t __b, const int __c) {
  return __builtin_aarch64_usri_nv8hi_uuus(__a, __b, __c);
}

FUNK(uint32x4_t)
vsriq_n_u32(uint32x4_t __a, uint32x4_t __b, const int __c) {
  return __builtin_aarch64_usri_nv4si_uuus(__a, __b, __c);
}

FUNK(uint64x2_t)
vsriq_n_u64(uint64x2_t __a, uint64x2_t __b, const int __c) {
  return __builtin_aarch64_usri_nv2di_uuus(__a, __b, __c);
}

FUNK(int64_t)
vsrid_n_s64(int64_t __a, int64_t __b, const int __c) {
  return __builtin_aarch64_ssri_ndi(__a, __b, __c);
}

FUNK(uint64_t)
vsrid_n_u64(uint64_t __a, uint64_t __b, const int __c) {
  return __builtin_aarch64_usri_ndi_uuus(__a, __b, __c);
}

FUNK(void)
vst1_f16(float16_t *__a, float16x4_t __b) {
  __builtin_aarch64_st1v4hf(__a, __b);
}

FUNK(void)
vst1_f32(float32_t *a, float32x2_t b) {
  __builtin_aarch64_st1v2sf((__builtin_aarch64_simd_sf *)a, b);
}

FUNK(void)
vst1_f64(float64_t *a, float64x1_t b) {
  *a = b[0];
}

FUNK(void)
vst1_p8(poly8_t *a, poly8x8_t b) {
  __builtin_aarch64_st1v8qi((__builtin_aarch64_simd_qi *)a, (int8x8_t)b);
}

FUNK(void)
vst1_p16(poly16_t *a, poly16x4_t b) {
  __builtin_aarch64_st1v4hi((__builtin_aarch64_simd_hi *)a, (int16x4_t)b);
}

FUNK(void)
vst1_p64(poly64_t *a, poly64x1_t b) {
  *a = b[0];
}

FUNK(void)
vst1_s8(int8_t *a, int8x8_t b) {
  __builtin_aarch64_st1v8qi((__builtin_aarch64_simd_qi *)a, b);
}

FUNK(void)
vst1_s16(int16_t *a, int16x4_t b) {
  __builtin_aarch64_st1v4hi((__builtin_aarch64_simd_hi *)a, b);
}

FUNK(void)
vst1_s32(int32_t *a, int32x2_t b) {
  __builtin_aarch64_st1v2si((__builtin_aarch64_simd_si *)a, b);
}

FUNK(void)
vst1_s64(int64_t *a, int64x1_t b) {
  *a = b[0];
}

FUNK(void)
vst1_u8(uint8_t *a, uint8x8_t b) {
  __builtin_aarch64_st1v8qi((__builtin_aarch64_simd_qi *)a, (int8x8_t)b);
}

FUNK(void)
vst1_u16(uint16_t *a, uint16x4_t b) {
  __builtin_aarch64_st1v4hi((__builtin_aarch64_simd_hi *)a, (int16x4_t)b);
}

FUNK(void)
vst1_u32(uint32_t *a, uint32x2_t b) {
  __builtin_aarch64_st1v2si((__builtin_aarch64_simd_si *)a, (int32x2_t)b);
}

FUNK(void)
vst1_u64(uint64_t *a, uint64x1_t b) {
  *a = b[0];
}

FUNK(void)
vst1q_f16(float16_t *__a, float16x8_t __b) {
  __builtin_aarch64_st1v8hf(__a, __b);
}

FUNK(void)
vst1q_f32(float32_t *a, float32x4_t b) {
  __builtin_aarch64_st1v4sf((__builtin_aarch64_simd_sf *)a, b);
}

FUNK(void)
vst1q_f64(float64_t *a, float64x2_t b) {
  __builtin_aarch64_st1v2df((__builtin_aarch64_simd_df *)a, b);
}

FUNK(void)
vst1q_p8(poly8_t *a, poly8x16_t b) {
  __builtin_aarch64_st1v16qi((__builtin_aarch64_simd_qi *)a, (int8x16_t)b);
}

FUNK(void)
vst1q_p16(poly16_t *a, poly16x8_t b) {
  __builtin_aarch64_st1v8hi((__builtin_aarch64_simd_hi *)a, (int16x8_t)b);
}

FUNK(void)
vst1q_p64(poly64_t *a, poly64x2_t b) {
  __builtin_aarch64_st1v2di_sp((__builtin_aarch64_simd_di *)a, (poly64x2_t)b);
}

FUNK(void)
vst1q_s8(int8_t *a, int8x16_t b) {
  __builtin_aarch64_st1v16qi((__builtin_aarch64_simd_qi *)a, b);
}

FUNK(void)
vst1q_s16(int16_t *a, int16x8_t b) {
  __builtin_aarch64_st1v8hi((__builtin_aarch64_simd_hi *)a, b);
}

FUNK(void)
vst1q_s32(int32_t *a, int32x4_t b) {
  __builtin_aarch64_st1v4si((__builtin_aarch64_simd_si *)a, b);
}

FUNK(void)
vst1q_s64(int64_t *a, int64x2_t b) {
  __builtin_aarch64_st1v2di((__builtin_aarch64_simd_di *)a, b);
}

FUNK(void)
vst1q_u8(uint8_t *a, uint8x16_t b) {
  __builtin_aarch64_st1v16qi((__builtin_aarch64_simd_qi *)a, (int8x16_t)b);
}

FUNK(void)
vst1q_u16(uint16_t *a, uint16x8_t b) {
  __builtin_aarch64_st1v8hi((__builtin_aarch64_simd_hi *)a, (int16x8_t)b);
}

FUNK(void)
vst1q_u32(uint32_t *a, uint32x4_t b) {
  __builtin_aarch64_st1v4si((__builtin_aarch64_simd_si *)a, (int32x4_t)b);
}

FUNK(void)
vst1q_u64(uint64_t *a, uint64x2_t b) {
  __builtin_aarch64_st1v2di((__builtin_aarch64_simd_di *)a, (int64x2_t)b);
}

FUNK(void)
vst1_lane_f16(float16_t *__a, float16x4_t __b, const int __lane) {
  *__a = __aarch64_vget_lane_any(__b, __lane);
}

FUNK(void)
vst1_lane_f32(float32_t *__a, float32x2_t __b, const int __lane) {
  *__a = __aarch64_vget_lane_any(__b, __lane);
}

FUNK(void)
vst1_lane_f64(float64_t *__a, float64x1_t __b, const int __lane) {
  *__a = __aarch64_vget_lane_any(__b, __lane);
}

FUNK(void)
vst1_lane_p8(poly8_t *__a, poly8x8_t __b, const int __lane) {
  *__a = __aarch64_vget_lane_any(__b, __lane);
}

FUNK(void)
vst1_lane_p16(poly16_t *__a, poly16x4_t __b, const int __lane) {
  *__a = __aarch64_vget_lane_any(__b, __lane);
}

FUNK(void)
vst1_lane_p64(poly64_t *__a, poly64x1_t __b, const int __lane) {
  *__a = __aarch64_vget_lane_any(__b, __lane);
}

FUNK(void)
vst1_lane_s8(int8_t *__a, int8x8_t __b, const int __lane) {
  *__a = __aarch64_vget_lane_any(__b, __lane);
}

FUNK(void)
vst1_lane_s16(int16_t *__a, int16x4_t __b, const int __lane) {
  *__a = __aarch64_vget_lane_any(__b, __lane);
}

FUNK(void)
vst1_lane_s32(int32_t *__a, int32x2_t __b, const int __lane) {
  *__a = __aarch64_vget_lane_any(__b, __lane);
}

FUNK(void)
vst1_lane_s64(int64_t *__a, int64x1_t __b, const int __lane) {
  *__a = __aarch64_vget_lane_any(__b, __lane);
}

FUNK(void)
vst1_lane_u8(uint8_t *__a, uint8x8_t __b, const int __lane) {
  *__a = __aarch64_vget_lane_any(__b, __lane);
}

FUNK(void)
vst1_lane_u16(uint16_t *__a, uint16x4_t __b, const int __lane) {
  *__a = __aarch64_vget_lane_any(__b, __lane);
}

FUNK(void)
vst1_lane_u32(uint32_t *__a, uint32x2_t __b, const int __lane) {
  *__a = __aarch64_vget_lane_any(__b, __lane);
}

FUNK(void)
vst1_lane_u64(uint64_t *__a, uint64x1_t __b, const int __lane) {
  *__a = __aarch64_vget_lane_any(__b, __lane);
}

FUNK(void)
vst1q_lane_f16(float16_t *__a, float16x8_t __b, const int __lane) {
  *__a = __aarch64_vget_lane_any(__b, __lane);
}

FUNK(void)
vst1q_lane_f32(float32_t *__a, float32x4_t __b, const int __lane) {
  *__a = __aarch64_vget_lane_any(__b, __lane);
}

FUNK(void)
vst1q_lane_f64(float64_t *__a, float64x2_t __b, const int __lane) {
  *__a = __aarch64_vget_lane_any(__b, __lane);
}

FUNK(void)
vst1q_lane_p8(poly8_t *__a, poly8x16_t __b, const int __lane) {
  *__a = __aarch64_vget_lane_any(__b, __lane);
}

FUNK(void)
vst1q_lane_p16(poly16_t *__a, poly16x8_t __b, const int __lane) {
  *__a = __aarch64_vget_lane_any(__b, __lane);
}

FUNK(void)
vst1q_lane_p64(poly64_t *__a, poly64x2_t __b, const int __lane) {
  *__a = __aarch64_vget_lane_any(__b, __lane);
}

FUNK(void)
vst1q_lane_s8(int8_t *__a, int8x16_t __b, const int __lane) {
  *__a = __aarch64_vget_lane_any(__b, __lane);
}

FUNK(void)
vst1q_lane_s16(int16_t *__a, int16x8_t __b, const int __lane) {
  *__a = __aarch64_vget_lane_any(__b, __lane);
}

FUNK(void)
vst1q_lane_s32(int32_t *__a, int32x4_t __b, const int __lane) {
  *__a = __aarch64_vget_lane_any(__b, __lane);
}

FUNK(void)
vst1q_lane_s64(int64_t *__a, int64x2_t __b, const int __lane) {
  *__a = __aarch64_vget_lane_any(__b, __lane);
}

FUNK(void)
vst1q_lane_u8(uint8_t *__a, uint8x16_t __b, const int __lane) {
  *__a = __aarch64_vget_lane_any(__b, __lane);
}

FUNK(void)
vst1q_lane_u16(uint16_t *__a, uint16x8_t __b, const int __lane) {
  *__a = __aarch64_vget_lane_any(__b, __lane);
}

FUNK(void)
vst1q_lane_u32(uint32_t *__a, uint32x4_t __b, const int __lane) {
  *__a = __aarch64_vget_lane_any(__b, __lane);
}

FUNK(void)
vst1q_lane_u64(uint64_t *__a, uint64x2_t __b, const int __lane) {
  *__a = __aarch64_vget_lane_any(__b, __lane);
}

FUNK(void)
vst1_s64_x2(int64_t *__a, int64x1x2_t val) {
  __builtin_aarch64_simd_oi __o;
  int64x2x2_t temp;
  temp.val[0] = vcombine_s64(val.val[0], vcreate_s64(__AARCH64_INT64_C(0)));
  temp.val[1] = vcombine_s64(val.val[1], vcreate_s64(__AARCH64_INT64_C(0)));
  __o = __builtin_aarch64_set_qregoiv2di(__o, (int64x2_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv2di(__o, (int64x2_t)temp.val[1], 1);
  __builtin_aarch64_st1x2di((__builtin_aarch64_simd_di *)__a, __o);
}

FUNK(void)
vst1_u64_x2(uint64_t *__a, uint64x1x2_t val) {
  __builtin_aarch64_simd_oi __o;
  uint64x2x2_t temp;
  temp.val[0] = vcombine_u64(val.val[0], vcreate_u64(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_u64(val.val[1], vcreate_u64(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregoiv2di(__o, (int64x2_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv2di(__o, (int64x2_t)temp.val[1], 1);
  __builtin_aarch64_st1x2di((__builtin_aarch64_simd_di *)__a, __o);
}

FUNK(void)
vst1_f64_x2(float64_t *__a, float64x1x2_t val) {
  __builtin_aarch64_simd_oi __o;
  float64x2x2_t temp;
  temp.val[0] = vcombine_f64(val.val[0], vcreate_f64(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_f64(val.val[1], vcreate_f64(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregoiv2df(__o, (float64x2_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv2df(__o, (float64x2_t)temp.val[1], 1);
  __builtin_aarch64_st1x2df((__builtin_aarch64_simd_df *)__a, __o);
}

FUNK(void)
vst1_s8_x2(int8_t *__a, int8x8x2_t val) {
  __builtin_aarch64_simd_oi __o;
  int8x16x2_t temp;
  temp.val[0] = vcombine_s8(val.val[0], vcreate_s8(__AARCH64_INT64_C(0)));
  temp.val[1] = vcombine_s8(val.val[1], vcreate_s8(__AARCH64_INT64_C(0)));
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)temp.val[1], 1);
  __builtin_aarch64_st1x2v8qi((__builtin_aarch64_simd_qi *)__a, __o);
}

FUNK(void)
vst1_p8_x2(poly8_t *__a, poly8x8x2_t val) {
  __builtin_aarch64_simd_oi __o;
  poly8x16x2_t temp;
  temp.val[0] = vcombine_p8(val.val[0], vcreate_p8(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_p8(val.val[1], vcreate_p8(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)temp.val[1], 1);
  __builtin_aarch64_st1x2v8qi((__builtin_aarch64_simd_qi *)__a, __o);
}

FUNK(void)
vst1_s16_x2(int16_t *__a, int16x4x2_t val) {
  __builtin_aarch64_simd_oi __o;
  int16x8x2_t temp;
  temp.val[0] = vcombine_s16(val.val[0], vcreate_s16(__AARCH64_INT64_C(0)));
  temp.val[1] = vcombine_s16(val.val[1], vcreate_s16(__AARCH64_INT64_C(0)));
  __o = __builtin_aarch64_set_qregoiv8hi(__o, (int16x8_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv8hi(__o, (int16x8_t)temp.val[1], 1);
  __builtin_aarch64_st1x2v4hi((__builtin_aarch64_simd_hi *)__a, __o);
}

FUNK(void)
vst1_p16_x2(poly16_t *__a, poly16x4x2_t val) {
  __builtin_aarch64_simd_oi __o;
  poly16x8x2_t temp;
  temp.val[0] = vcombine_p16(val.val[0], vcreate_p16(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_p16(val.val[1], vcreate_p16(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregoiv8hi(__o, (int16x8_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv8hi(__o, (int16x8_t)temp.val[1], 1);
  __builtin_aarch64_st1x2v4hi((__builtin_aarch64_simd_hi *)__a, __o);
}

FUNK(void)
vst1_s32_x2(int32_t *__a, int32x2x2_t val) {
  __builtin_aarch64_simd_oi __o;
  int32x4x2_t temp;
  temp.val[0] = vcombine_s32(val.val[0], vcreate_s32(__AARCH64_INT64_C(0)));
  temp.val[1] = vcombine_s32(val.val[1], vcreate_s32(__AARCH64_INT64_C(0)));
  __o = __builtin_aarch64_set_qregoiv4si(__o, (int32x4_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv4si(__o, (int32x4_t)temp.val[1], 1);
  __builtin_aarch64_st1x2v2si((__builtin_aarch64_simd_si *)__a, __o);
}

FUNK(void)
vst1_u8_x2(uint8_t *__a, uint8x8x2_t val) {
  __builtin_aarch64_simd_oi __o;
  uint8x16x2_t temp;
  temp.val[0] = vcombine_u8(val.val[0], vcreate_u8(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_u8(val.val[1], vcreate_u8(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)temp.val[1], 1);
  __builtin_aarch64_st1x2v8qi((__builtin_aarch64_simd_qi *)__a, __o);
}

FUNK(void)
vst1_u16_x2(uint16_t *__a, uint16x4x2_t val) {
  __builtin_aarch64_simd_oi __o;
  uint16x8x2_t temp;
  temp.val[0] = vcombine_u16(val.val[0], vcreate_u16(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_u16(val.val[1], vcreate_u16(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregoiv8hi(__o, (int16x8_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv8hi(__o, (int16x8_t)temp.val[1], 1);
  __builtin_aarch64_st1x2v4hi((__builtin_aarch64_simd_hi *)__a, __o);
}

FUNK(void)
vst1_u32_x2(uint32_t *__a, uint32x2x2_t val) {
  __builtin_aarch64_simd_oi __o;
  uint32x4x2_t temp;
  temp.val[0] = vcombine_u32(val.val[0], vcreate_u32(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_u32(val.val[1], vcreate_u32(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregoiv4si(__o, (int32x4_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv4si(__o, (int32x4_t)temp.val[1], 1);
  __builtin_aarch64_st1x2v2si((__builtin_aarch64_simd_si *)__a, __o);
}

FUNK(void)
vst1_f16_x2(float16_t *__a, float16x4x2_t val) {
  __builtin_aarch64_simd_oi __o;
  float16x8x2_t temp;
  temp.val[0] = vcombine_f16(val.val[0], vcreate_f16(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_f16(val.val[1], vcreate_f16(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregoiv8hf(__o, temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv8hf(__o, temp.val[1], 1);
  __builtin_aarch64_st1x2v4hf(__a, __o);
}

FUNK(void)
vst1_f32_x2(float32_t *__a, float32x2x2_t val) {
  __builtin_aarch64_simd_oi __o;
  float32x4x2_t temp;
  temp.val[0] = vcombine_f32(val.val[0], vcreate_f32(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_f32(val.val[1], vcreate_f32(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregoiv4sf(__o, (float32x4_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv4sf(__o, (float32x4_t)temp.val[1], 1);
  __builtin_aarch64_st1x2v2sf((__builtin_aarch64_simd_sf *)__a, __o);
}

FUNK(void)
vst1_p64_x2(poly64_t *__a, poly64x1x2_t val) {
  __builtin_aarch64_simd_oi __o;
  poly64x2x2_t temp;
  temp.val[0] = vcombine_p64(val.val[0], vcreate_p64(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_p64(val.val[1], vcreate_p64(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregoiv2di_ssps(__o, (poly64x2_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv2di_ssps(__o, (poly64x2_t)temp.val[1], 1);
  __builtin_aarch64_st1x2di((__builtin_aarch64_simd_di *)__a, __o);
}

FUNK(void)
vst1q_s8_x2(int8_t *__a, int8x16x2_t val) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)val.val[1], 1);
  __builtin_aarch64_st1x2v16qi((__builtin_aarch64_simd_qi *)__a, __o);
}

FUNK(void)
vst1q_p8_x2(poly8_t *__a, poly8x16x2_t val) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)val.val[1], 1);
  __builtin_aarch64_st1x2v16qi((__builtin_aarch64_simd_qi *)__a, __o);
}

FUNK(void)
vst1q_s16_x2(int16_t *__a, int16x8x2_t val) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv8hi(__o, (int16x8_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv8hi(__o, (int16x8_t)val.val[1], 1);
  __builtin_aarch64_st1x2v8hi((__builtin_aarch64_simd_hi *)__a, __o);
}

FUNK(void)
vst1q_p16_x2(poly16_t *__a, poly16x8x2_t val) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv8hi(__o, (int16x8_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv8hi(__o, (int16x8_t)val.val[1], 1);
  __builtin_aarch64_st1x2v8hi((__builtin_aarch64_simd_hi *)__a, __o);
}

FUNK(void)
vst1q_s32_x2(int32_t *__a, int32x4x2_t val) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv4si(__o, (int32x4_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv4si(__o, (int32x4_t)val.val[1], 1);
  __builtin_aarch64_st1x2v4si((__builtin_aarch64_simd_si *)__a, __o);
}

FUNK(void)
vst1q_s64_x2(int64_t *__a, int64x2x2_t val) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv2di(__o, (int64x2_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv2di(__o, (int64x2_t)val.val[1], 1);
  __builtin_aarch64_st1x2v2di((__builtin_aarch64_simd_di *)__a, __o);
}

FUNK(void)
vst1q_u8_x2(uint8_t *__a, uint8x16x2_t val) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)val.val[1], 1);
  __builtin_aarch64_st1x2v16qi((__builtin_aarch64_simd_qi *)__a, __o);
}

FUNK(void)
vst1q_u16_x2(uint16_t *__a, uint16x8x2_t val) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv8hi(__o, (int16x8_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv8hi(__o, (int16x8_t)val.val[1], 1);
  __builtin_aarch64_st1x2v8hi((__builtin_aarch64_simd_hi *)__a, __o);
}

FUNK(void)
vst1q_u32_x2(uint32_t *__a, uint32x4x2_t val) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv4si(__o, (int32x4_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv4si(__o, (int32x4_t)val.val[1], 1);
  __builtin_aarch64_st1x2v4si((__builtin_aarch64_simd_si *)__a, __o);
}

FUNK(void)
vst1q_u64_x2(uint64_t *__a, uint64x2x2_t val) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv2di(__o, (int64x2_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv2di(__o, (int64x2_t)val.val[1], 1);
  __builtin_aarch64_st1x2v2di((__builtin_aarch64_simd_di *)__a, __o);
}

FUNK(void)
vst1q_f16_x2(float16_t *__a, float16x8x2_t val) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv8hf(__o, val.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv8hf(__o, val.val[1], 1);
  __builtin_aarch64_st1x2v8hf(__a, __o);
}

FUNK(void)
vst1q_f32_x2(float32_t *__a, float32x4x2_t val) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv4sf(__o, (float32x4_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv4sf(__o, (float32x4_t)val.val[1], 1);
  __builtin_aarch64_st1x2v4sf((__builtin_aarch64_simd_sf *)__a, __o);
}

FUNK(void)
vst1q_f64_x2(float64_t *__a, float64x2x2_t val) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv2df(__o, (float64x2_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv2df(__o, (float64x2_t)val.val[1], 1);
  __builtin_aarch64_st1x2v2df((__builtin_aarch64_simd_df *)__a, __o);
}

FUNK(void)
vst1q_p64_x2(poly64_t *__a, poly64x2x2_t val) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv2di_ssps(__o, (poly64x2_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv2di_ssps(__o, (poly64x2_t)val.val[1], 1);
  __builtin_aarch64_st1x2v2di((__builtin_aarch64_simd_di *)__a, __o);
}

FUNK(void)
vst1_s64_x3(int64_t *__a, int64x1x3_t val) {
  __builtin_aarch64_simd_ci __o;
  int64x2x3_t temp;
  temp.val[0] = vcombine_s64(val.val[0], vcreate_s64(__AARCH64_INT64_C(0)));
  temp.val[1] = vcombine_s64(val.val[1], vcreate_s64(__AARCH64_INT64_C(0)));
  temp.val[2] = vcombine_s64(val.val[2], vcreate_s64(__AARCH64_INT64_C(0)));
  __o = __builtin_aarch64_set_qregciv2di(__o, (int64x2_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregciv2di(__o, (int64x2_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregciv2di(__o, (int64x2_t)temp.val[2], 2);
  __builtin_aarch64_st1x3di((__builtin_aarch64_simd_di *)__a, __o);
}

FUNK(void)
vst1_u64_x3(uint64_t *__a, uint64x1x3_t val) {
  __builtin_aarch64_simd_ci __o;
  uint64x2x3_t temp;
  temp.val[0] = vcombine_u64(val.val[0], vcreate_u64(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_u64(val.val[1], vcreate_u64(__AARCH64_UINT64_C(0)));
  temp.val[2] = vcombine_u64(val.val[2], vcreate_u64(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregciv2di(__o, (int64x2_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregciv2di(__o, (int64x2_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregciv2di(__o, (int64x2_t)temp.val[2], 2);
  __builtin_aarch64_st1x3di((__builtin_aarch64_simd_di *)__a, __o);
}

FUNK(void)
vst1_f64_x3(float64_t *__a, float64x1x3_t val) {
  __builtin_aarch64_simd_ci __o;
  float64x2x3_t temp;
  temp.val[0] = vcombine_f64(val.val[0], vcreate_f64(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_f64(val.val[1], vcreate_f64(__AARCH64_UINT64_C(0)));
  temp.val[2] = vcombine_f64(val.val[2], vcreate_f64(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregciv2df(__o, (float64x2_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregciv2df(__o, (float64x2_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregciv2df(__o, (float64x2_t)temp.val[2], 2);
  __builtin_aarch64_st1x3df((__builtin_aarch64_simd_df *)__a, __o);
}

FUNK(void)
vst1_s8_x3(int8_t *__a, int8x8x3_t val) {
  __builtin_aarch64_simd_ci __o;
  int8x16x3_t temp;
  temp.val[0] = vcombine_s8(val.val[0], vcreate_s8(__AARCH64_INT64_C(0)));
  temp.val[1] = vcombine_s8(val.val[1], vcreate_s8(__AARCH64_INT64_C(0)));
  temp.val[2] = vcombine_s8(val.val[2], vcreate_s8(__AARCH64_INT64_C(0)));
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)temp.val[2], 2);
  __builtin_aarch64_st1x3v8qi((__builtin_aarch64_simd_qi *)__a, __o);
}

FUNK(void)
vst1_p8_x3(poly8_t *__a, poly8x8x3_t val) {
  __builtin_aarch64_simd_ci __o;
  poly8x16x3_t temp;
  temp.val[0] = vcombine_p8(val.val[0], vcreate_p8(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_p8(val.val[1], vcreate_p8(__AARCH64_UINT64_C(0)));
  temp.val[2] = vcombine_p8(val.val[2], vcreate_p8(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)temp.val[2], 2);
  __builtin_aarch64_st1x3v8qi((__builtin_aarch64_simd_qi *)__a, __o);
}

FUNK(void)
vst1_s16_x3(int16_t *__a, int16x4x3_t val) {
  __builtin_aarch64_simd_ci __o;
  int16x8x3_t temp;
  temp.val[0] = vcombine_s16(val.val[0], vcreate_s16(__AARCH64_INT64_C(0)));
  temp.val[1] = vcombine_s16(val.val[1], vcreate_s16(__AARCH64_INT64_C(0)));
  temp.val[2] = vcombine_s16(val.val[2], vcreate_s16(__AARCH64_INT64_C(0)));
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)temp.val[2], 2);
  __builtin_aarch64_st1x3v4hi((__builtin_aarch64_simd_hi *)__a, __o);
}

FUNK(void)
vst1_p16_x3(poly16_t *__a, poly16x4x3_t val) {
  __builtin_aarch64_simd_ci __o;
  poly16x8x3_t temp;
  temp.val[0] = vcombine_p16(val.val[0], vcreate_p16(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_p16(val.val[1], vcreate_p16(__AARCH64_UINT64_C(0)));
  temp.val[2] = vcombine_p16(val.val[2], vcreate_p16(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)temp.val[2], 2);
  __builtin_aarch64_st1x3v4hi((__builtin_aarch64_simd_hi *)__a, __o);
}

FUNK(void)
vst1_s32_x3(int32_t *__a, int32x2x3_t val) {
  __builtin_aarch64_simd_ci __o;
  int32x4x3_t temp;
  temp.val[0] = vcombine_s32(val.val[0], vcreate_s32(__AARCH64_INT64_C(0)));
  temp.val[1] = vcombine_s32(val.val[1], vcreate_s32(__AARCH64_INT64_C(0)));
  temp.val[2] = vcombine_s32(val.val[2], vcreate_s32(__AARCH64_INT64_C(0)));
  __o = __builtin_aarch64_set_qregciv4si(__o, (int32x4_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregciv4si(__o, (int32x4_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregciv4si(__o, (int32x4_t)temp.val[2], 2);
  __builtin_aarch64_st1x3v2si((__builtin_aarch64_simd_si *)__a, __o);
}

FUNK(void)
vst1_u8_x3(uint8_t *__a, uint8x8x3_t val) {
  __builtin_aarch64_simd_ci __o;
  uint8x16x3_t temp;
  temp.val[0] = vcombine_u8(val.val[0], vcreate_u8(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_u8(val.val[1], vcreate_u8(__AARCH64_UINT64_C(0)));
  temp.val[2] = vcombine_u8(val.val[2], vcreate_u8(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)temp.val[2], 2);
  __builtin_aarch64_st1x3v8qi((__builtin_aarch64_simd_qi *)__a, __o);
}

FUNK(void)
vst1_u16_x3(uint16_t *__a, uint16x4x3_t val) {
  __builtin_aarch64_simd_ci __o;
  uint16x8x3_t temp;
  temp.val[0] = vcombine_u16(val.val[0], vcreate_u16(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_u16(val.val[1], vcreate_u16(__AARCH64_UINT64_C(0)));
  temp.val[2] = vcombine_u16(val.val[2], vcreate_u16(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)temp.val[2], 2);
  __builtin_aarch64_st1x3v4hi((__builtin_aarch64_simd_hi *)__a, __o);
}

FUNK(void)
vst1_u32_x3(uint32_t *__a, uint32x2x3_t val) {
  __builtin_aarch64_simd_ci __o;
  uint32x4x3_t temp;
  temp.val[0] = vcombine_u32(val.val[0], vcreate_u32(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_u32(val.val[1], vcreate_u32(__AARCH64_UINT64_C(0)));
  temp.val[2] = vcombine_u32(val.val[2], vcreate_u32(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregciv4si(__o, (int32x4_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregciv4si(__o, (int32x4_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregciv4si(__o, (int32x4_t)temp.val[2], 2);
  __builtin_aarch64_st1x3v2si((__builtin_aarch64_simd_si *)__a, __o);
}

FUNK(void)
vst1_f16_x3(float16_t *__a, float16x4x3_t val) {
  __builtin_aarch64_simd_ci __o;
  float16x8x3_t temp;
  temp.val[0] = vcombine_f16(val.val[0], vcreate_f16(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_f16(val.val[1], vcreate_f16(__AARCH64_UINT64_C(0)));
  temp.val[2] = vcombine_f16(val.val[2], vcreate_f16(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregciv8hf(__o, (float16x8_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregciv8hf(__o, (float16x8_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregciv8hf(__o, (float16x8_t)temp.val[2], 2);
  __builtin_aarch64_st1x3v4hf((__builtin_aarch64_simd_hf *)__a, __o);
}

FUNK(void)
vst1_f32_x3(float32_t *__a, float32x2x3_t val) {
  __builtin_aarch64_simd_ci __o;
  float32x4x3_t temp;
  temp.val[0] = vcombine_f32(val.val[0], vcreate_f32(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_f32(val.val[1], vcreate_f32(__AARCH64_UINT64_C(0)));
  temp.val[2] = vcombine_f32(val.val[2], vcreate_f32(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregciv4sf(__o, (float32x4_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregciv4sf(__o, (float32x4_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregciv4sf(__o, (float32x4_t)temp.val[2], 2);
  __builtin_aarch64_st1x3v2sf((__builtin_aarch64_simd_sf *)__a, __o);
}

FUNK(void)
vst1_p64_x3(poly64_t *__a, poly64x1x3_t val) {
  __builtin_aarch64_simd_ci __o;
  poly64x2x3_t temp;
  temp.val[0] = vcombine_p64(val.val[0], vcreate_p64(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_p64(val.val[1], vcreate_p64(__AARCH64_UINT64_C(0)));
  temp.val[2] = vcombine_p64(val.val[2], vcreate_p64(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregciv2di_ssps(__o, (poly64x2_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregciv2di_ssps(__o, (poly64x2_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregciv2di_ssps(__o, (poly64x2_t)temp.val[2], 2);
  __builtin_aarch64_st1x3di((__builtin_aarch64_simd_di *)__a, __o);
}

FUNK(void)
vst1q_s8_x3(int8_t *__a, int8x16x3_t val) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)val.val[2], 2);
  __builtin_aarch64_st1x3v16qi((__builtin_aarch64_simd_qi *)__a, __o);
}

FUNK(void)
vst1q_p8_x3(poly8_t *__a, poly8x16x3_t val) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)val.val[2], 2);
  __builtin_aarch64_st1x3v16qi((__builtin_aarch64_simd_qi *)__a, __o);
}

FUNK(void)
vst1q_s16_x3(int16_t *__a, int16x8x3_t val) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)val.val[2], 2);
  __builtin_aarch64_st1x3v8hi((__builtin_aarch64_simd_hi *)__a, __o);
}

FUNK(void)
vst1q_p16_x3(poly16_t *__a, poly16x8x3_t val) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)val.val[2], 2);
  __builtin_aarch64_st1x3v8hi((__builtin_aarch64_simd_hi *)__a, __o);
}

FUNK(void)
vst1q_s32_x3(int32_t *__a, int32x4x3_t val) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv4si(__o, (int32x4_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregciv4si(__o, (int32x4_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregciv4si(__o, (int32x4_t)val.val[2], 2);
  __builtin_aarch64_st1x3v4si((__builtin_aarch64_simd_si *)__a, __o);
}

FUNK(void)
vst1q_s64_x3(int64_t *__a, int64x2x3_t val) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv2di(__o, (int64x2_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregciv2di(__o, (int64x2_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregciv2di(__o, (int64x2_t)val.val[2], 2);
  __builtin_aarch64_st1x3v2di((__builtin_aarch64_simd_di *)__a, __o);
}

FUNK(void)
vst1q_u8_x3(uint8_t *__a, uint8x16x3_t val) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)val.val[2], 2);
  __builtin_aarch64_st1x3v16qi((__builtin_aarch64_simd_qi *)__a, __o);
}

FUNK(void)
vst1q_u16_x3(uint16_t *__a, uint16x8x3_t val) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)val.val[2], 2);
  __builtin_aarch64_st1x3v8hi((__builtin_aarch64_simd_hi *)__a, __o);
}

FUNK(void)
vst1q_u32_x3(uint32_t *__a, uint32x4x3_t val) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv4si(__o, (int32x4_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregciv4si(__o, (int32x4_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregciv4si(__o, (int32x4_t)val.val[2], 2);
  __builtin_aarch64_st1x3v4si((__builtin_aarch64_simd_si *)__a, __o);
}

FUNK(void)
vst1q_u64_x3(uint64_t *__a, uint64x2x3_t val) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv2di(__o, (int64x2_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregciv2di(__o, (int64x2_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregciv2di(__o, (int64x2_t)val.val[2], 2);
  __builtin_aarch64_st1x3v2di((__builtin_aarch64_simd_di *)__a, __o);
}

FUNK(void)
vst1q_f16_x3(float16_t *__a, float16x8x3_t val) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv8hf(__o, (float16x8_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregciv8hf(__o, (float16x8_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregciv8hf(__o, (float16x8_t)val.val[2], 2);
  __builtin_aarch64_st1x3v8hf((__builtin_aarch64_simd_hf *)__a, __o);
}

FUNK(void)
vst1q_f32_x3(float32_t *__a, float32x4x3_t val) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv4sf(__o, (float32x4_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregciv4sf(__o, (float32x4_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregciv4sf(__o, (float32x4_t)val.val[2], 2);
  __builtin_aarch64_st1x3v4sf((__builtin_aarch64_simd_sf *)__a, __o);
}

FUNK(void)
vst1q_f64_x3(float64_t *__a, float64x2x3_t val) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv2df(__o, (float64x2_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregciv2df(__o, (float64x2_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregciv2df(__o, (float64x2_t)val.val[2], 2);
  __builtin_aarch64_st1x3v2df((__builtin_aarch64_simd_df *)__a, __o);
}

FUNK(void)
vst1q_p64_x3(poly64_t *__a, poly64x2x3_t val) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv2di_ssps(__o, (poly64x2_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregciv2di_ssps(__o, (poly64x2_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregciv2di_ssps(__o, (poly64x2_t)val.val[2], 2);
  __builtin_aarch64_st1x3v2di((__builtin_aarch64_simd_di *)__a, __o);
}

FUNK(void)
vst2_s64(int64_t *__a, int64x1x2_t val) {
  __builtin_aarch64_simd_oi __o;
  int64x2x2_t temp;
  temp.val[0] = vcombine_s64(val.val[0], vcreate_s64(__AARCH64_INT64_C(0)));
  temp.val[1] = vcombine_s64(val.val[1], vcreate_s64(__AARCH64_INT64_C(0)));
  __o = __builtin_aarch64_set_qregoiv2di(__o, (int64x2_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv2di(__o, (int64x2_t)temp.val[1], 1);
  __builtin_aarch64_st2di((__builtin_aarch64_simd_di *)__a, __o);
}

FUNK(void)
vst2_u64(uint64_t *__a, uint64x1x2_t val) {
  __builtin_aarch64_simd_oi __o;
  uint64x2x2_t temp;
  temp.val[0] = vcombine_u64(val.val[0], vcreate_u64(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_u64(val.val[1], vcreate_u64(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregoiv2di(__o, (int64x2_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv2di(__o, (int64x2_t)temp.val[1], 1);
  __builtin_aarch64_st2di((__builtin_aarch64_simd_di *)__a, __o);
}

FUNK(void)
vst2_f64(float64_t *__a, float64x1x2_t val) {
  __builtin_aarch64_simd_oi __o;
  float64x2x2_t temp;
  temp.val[0] = vcombine_f64(val.val[0], vcreate_f64(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_f64(val.val[1], vcreate_f64(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregoiv2df(__o, (float64x2_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv2df(__o, (float64x2_t)temp.val[1], 1);
  __builtin_aarch64_st2df((__builtin_aarch64_simd_df *)__a, __o);
}

FUNK(void)
vst2_s8(int8_t *__a, int8x8x2_t val) {
  __builtin_aarch64_simd_oi __o;
  int8x16x2_t temp;
  temp.val[0] = vcombine_s8(val.val[0], vcreate_s8(__AARCH64_INT64_C(0)));
  temp.val[1] = vcombine_s8(val.val[1], vcreate_s8(__AARCH64_INT64_C(0)));
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)temp.val[1], 1);
  __builtin_aarch64_st2v8qi((__builtin_aarch64_simd_qi *)__a, __o);
}

FUNK(void)
vst2_p8(poly8_t *__a, poly8x8x2_t val) {
  __builtin_aarch64_simd_oi __o;
  poly8x16x2_t temp;
  temp.val[0] = vcombine_p8(val.val[0], vcreate_p8(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_p8(val.val[1], vcreate_p8(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)temp.val[1], 1);
  __builtin_aarch64_st2v8qi((__builtin_aarch64_simd_qi *)__a, __o);
}

FUNK(void)
vst2_s16(int16_t *__a, int16x4x2_t val) {
  __builtin_aarch64_simd_oi __o;
  int16x8x2_t temp;
  temp.val[0] = vcombine_s16(val.val[0], vcreate_s16(__AARCH64_INT64_C(0)));
  temp.val[1] = vcombine_s16(val.val[1], vcreate_s16(__AARCH64_INT64_C(0)));
  __o = __builtin_aarch64_set_qregoiv8hi(__o, (int16x8_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv8hi(__o, (int16x8_t)temp.val[1], 1);
  __builtin_aarch64_st2v4hi((__builtin_aarch64_simd_hi *)__a, __o);
}

FUNK(void)
vst2_p16(poly16_t *__a, poly16x4x2_t val) {
  __builtin_aarch64_simd_oi __o;
  poly16x8x2_t temp;
  temp.val[0] = vcombine_p16(val.val[0], vcreate_p16(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_p16(val.val[1], vcreate_p16(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregoiv8hi(__o, (int16x8_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv8hi(__o, (int16x8_t)temp.val[1], 1);
  __builtin_aarch64_st2v4hi((__builtin_aarch64_simd_hi *)__a, __o);
}

FUNK(void)
vst2_s32(int32_t *__a, int32x2x2_t val) {
  __builtin_aarch64_simd_oi __o;
  int32x4x2_t temp;
  temp.val[0] = vcombine_s32(val.val[0], vcreate_s32(__AARCH64_INT64_C(0)));
  temp.val[1] = vcombine_s32(val.val[1], vcreate_s32(__AARCH64_INT64_C(0)));
  __o = __builtin_aarch64_set_qregoiv4si(__o, (int32x4_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv4si(__o, (int32x4_t)temp.val[1], 1);
  __builtin_aarch64_st2v2si((__builtin_aarch64_simd_si *)__a, __o);
}

FUNK(void)
vst2_u8(uint8_t *__a, uint8x8x2_t val) {
  __builtin_aarch64_simd_oi __o;
  uint8x16x2_t temp;
  temp.val[0] = vcombine_u8(val.val[0], vcreate_u8(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_u8(val.val[1], vcreate_u8(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)temp.val[1], 1);
  __builtin_aarch64_st2v8qi((__builtin_aarch64_simd_qi *)__a, __o);
}

FUNK(void)
vst2_u16(uint16_t *__a, uint16x4x2_t val) {
  __builtin_aarch64_simd_oi __o;
  uint16x8x2_t temp;
  temp.val[0] = vcombine_u16(val.val[0], vcreate_u16(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_u16(val.val[1], vcreate_u16(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregoiv8hi(__o, (int16x8_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv8hi(__o, (int16x8_t)temp.val[1], 1);
  __builtin_aarch64_st2v4hi((__builtin_aarch64_simd_hi *)__a, __o);
}

FUNK(void)
vst2_u32(uint32_t *__a, uint32x2x2_t val) {
  __builtin_aarch64_simd_oi __o;
  uint32x4x2_t temp;
  temp.val[0] = vcombine_u32(val.val[0], vcreate_u32(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_u32(val.val[1], vcreate_u32(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregoiv4si(__o, (int32x4_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv4si(__o, (int32x4_t)temp.val[1], 1);
  __builtin_aarch64_st2v2si((__builtin_aarch64_simd_si *)__a, __o);
}

FUNK(void)
vst2_f16(float16_t *__a, float16x4x2_t val) {
  __builtin_aarch64_simd_oi __o;
  float16x8x2_t temp;
  temp.val[0] = vcombine_f16(val.val[0], vcreate_f16(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_f16(val.val[1], vcreate_f16(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregoiv8hf(__o, temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv8hf(__o, temp.val[1], 1);
  __builtin_aarch64_st2v4hf(__a, __o);
}

FUNK(void)
vst2_f32(float32_t *__a, float32x2x2_t val) {
  __builtin_aarch64_simd_oi __o;
  float32x4x2_t temp;
  temp.val[0] = vcombine_f32(val.val[0], vcreate_f32(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_f32(val.val[1], vcreate_f32(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregoiv4sf(__o, (float32x4_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv4sf(__o, (float32x4_t)temp.val[1], 1);
  __builtin_aarch64_st2v2sf((__builtin_aarch64_simd_sf *)__a, __o);
}

FUNK(void)
vst2_p64(poly64_t *__a, poly64x1x2_t val) {
  __builtin_aarch64_simd_oi __o;
  poly64x2x2_t temp;
  temp.val[0] = vcombine_p64(val.val[0], vcreate_p64(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_p64(val.val[1], vcreate_p64(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregoiv2di_ssps(__o, (poly64x2_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv2di_ssps(__o, (poly64x2_t)temp.val[1], 1);
  __builtin_aarch64_st2di((__builtin_aarch64_simd_di *)__a, __o);
}

FUNK(void)
vst2q_s8(int8_t *__a, int8x16x2_t val) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)val.val[1], 1);
  __builtin_aarch64_st2v16qi((__builtin_aarch64_simd_qi *)__a, __o);
}

FUNK(void)
vst2q_p8(poly8_t *__a, poly8x16x2_t val) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)val.val[1], 1);
  __builtin_aarch64_st2v16qi((__builtin_aarch64_simd_qi *)__a, __o);
}

FUNK(void)
vst2q_s16(int16_t *__a, int16x8x2_t val) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv8hi(__o, (int16x8_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv8hi(__o, (int16x8_t)val.val[1], 1);
  __builtin_aarch64_st2v8hi((__builtin_aarch64_simd_hi *)__a, __o);
}

FUNK(void)
vst2q_p16(poly16_t *__a, poly16x8x2_t val) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv8hi(__o, (int16x8_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv8hi(__o, (int16x8_t)val.val[1], 1);
  __builtin_aarch64_st2v8hi((__builtin_aarch64_simd_hi *)__a, __o);
}

FUNK(void)
vst2q_s32(int32_t *__a, int32x4x2_t val) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv4si(__o, (int32x4_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv4si(__o, (int32x4_t)val.val[1], 1);
  __builtin_aarch64_st2v4si((__builtin_aarch64_simd_si *)__a, __o);
}

FUNK(void)
vst2q_s64(int64_t *__a, int64x2x2_t val) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv2di(__o, (int64x2_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv2di(__o, (int64x2_t)val.val[1], 1);
  __builtin_aarch64_st2v2di((__builtin_aarch64_simd_di *)__a, __o);
}

FUNK(void)
vst2q_u8(uint8_t *__a, uint8x16x2_t val) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)val.val[1], 1);
  __builtin_aarch64_st2v16qi((__builtin_aarch64_simd_qi *)__a, __o);
}

FUNK(void)
vst2q_u16(uint16_t *__a, uint16x8x2_t val) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv8hi(__o, (int16x8_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv8hi(__o, (int16x8_t)val.val[1], 1);
  __builtin_aarch64_st2v8hi((__builtin_aarch64_simd_hi *)__a, __o);
}

FUNK(void)
vst2q_u32(uint32_t *__a, uint32x4x2_t val) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv4si(__o, (int32x4_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv4si(__o, (int32x4_t)val.val[1], 1);
  __builtin_aarch64_st2v4si((__builtin_aarch64_simd_si *)__a, __o);
}

FUNK(void)
vst2q_u64(uint64_t *__a, uint64x2x2_t val) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv2di(__o, (int64x2_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv2di(__o, (int64x2_t)val.val[1], 1);
  __builtin_aarch64_st2v2di((__builtin_aarch64_simd_di *)__a, __o);
}

FUNK(void)
vst2q_f16(float16_t *__a, float16x8x2_t val) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv8hf(__o, val.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv8hf(__o, val.val[1], 1);
  __builtin_aarch64_st2v8hf(__a, __o);
}

FUNK(void)
vst2q_f32(float32_t *__a, float32x4x2_t val) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv4sf(__o, (float32x4_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv4sf(__o, (float32x4_t)val.val[1], 1);
  __builtin_aarch64_st2v4sf((__builtin_aarch64_simd_sf *)__a, __o);
}

FUNK(void)
vst2q_f64(float64_t *__a, float64x2x2_t val) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv2df(__o, (float64x2_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv2df(__o, (float64x2_t)val.val[1], 1);
  __builtin_aarch64_st2v2df((__builtin_aarch64_simd_df *)__a, __o);
}

FUNK(void)
vst2q_p64(poly64_t *__a, poly64x2x2_t val) {
  __builtin_aarch64_simd_oi __o;
  __o = __builtin_aarch64_set_qregoiv2di_ssps(__o, (poly64x2_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv2di_ssps(__o, (poly64x2_t)val.val[1], 1);
  __builtin_aarch64_st2v2di((__builtin_aarch64_simd_di *)__a, __o);
}

FUNK(void)
vst3_s64(int64_t *__a, int64x1x3_t val) {
  __builtin_aarch64_simd_ci __o;
  int64x2x3_t temp;
  temp.val[0] = vcombine_s64(val.val[0], vcreate_s64(__AARCH64_INT64_C(0)));
  temp.val[1] = vcombine_s64(val.val[1], vcreate_s64(__AARCH64_INT64_C(0)));
  temp.val[2] = vcombine_s64(val.val[2], vcreate_s64(__AARCH64_INT64_C(0)));
  __o = __builtin_aarch64_set_qregciv2di(__o, (int64x2_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregciv2di(__o, (int64x2_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregciv2di(__o, (int64x2_t)temp.val[2], 2);
  __builtin_aarch64_st3di((__builtin_aarch64_simd_di *)__a, __o);
}

FUNK(void)
vst3_u64(uint64_t *__a, uint64x1x3_t val) {
  __builtin_aarch64_simd_ci __o;
  uint64x2x3_t temp;
  temp.val[0] = vcombine_u64(val.val[0], vcreate_u64(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_u64(val.val[1], vcreate_u64(__AARCH64_UINT64_C(0)));
  temp.val[2] = vcombine_u64(val.val[2], vcreate_u64(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregciv2di(__o, (int64x2_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregciv2di(__o, (int64x2_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregciv2di(__o, (int64x2_t)temp.val[2], 2);
  __builtin_aarch64_st3di((__builtin_aarch64_simd_di *)__a, __o);
}

FUNK(void)
vst3_f64(float64_t *__a, float64x1x3_t val) {
  __builtin_aarch64_simd_ci __o;
  float64x2x3_t temp;
  temp.val[0] = vcombine_f64(val.val[0], vcreate_f64(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_f64(val.val[1], vcreate_f64(__AARCH64_UINT64_C(0)));
  temp.val[2] = vcombine_f64(val.val[2], vcreate_f64(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregciv2df(__o, (float64x2_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregciv2df(__o, (float64x2_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregciv2df(__o, (float64x2_t)temp.val[2], 2);
  __builtin_aarch64_st3df((__builtin_aarch64_simd_df *)__a, __o);
}

FUNK(void)
vst3_s8(int8_t *__a, int8x8x3_t val) {
  __builtin_aarch64_simd_ci __o;
  int8x16x3_t temp;
  temp.val[0] = vcombine_s8(val.val[0], vcreate_s8(__AARCH64_INT64_C(0)));
  temp.val[1] = vcombine_s8(val.val[1], vcreate_s8(__AARCH64_INT64_C(0)));
  temp.val[2] = vcombine_s8(val.val[2], vcreate_s8(__AARCH64_INT64_C(0)));
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)temp.val[2], 2);
  __builtin_aarch64_st3v8qi((__builtin_aarch64_simd_qi *)__a, __o);
}

FUNK(void)
vst3_p8(poly8_t *__a, poly8x8x3_t val) {
  __builtin_aarch64_simd_ci __o;
  poly8x16x3_t temp;
  temp.val[0] = vcombine_p8(val.val[0], vcreate_p8(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_p8(val.val[1], vcreate_p8(__AARCH64_UINT64_C(0)));
  temp.val[2] = vcombine_p8(val.val[2], vcreate_p8(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)temp.val[2], 2);
  __builtin_aarch64_st3v8qi((__builtin_aarch64_simd_qi *)__a, __o);
}

FUNK(void)
vst3_s16(int16_t *__a, int16x4x3_t val) {
  __builtin_aarch64_simd_ci __o;
  int16x8x3_t temp;
  temp.val[0] = vcombine_s16(val.val[0], vcreate_s16(__AARCH64_INT64_C(0)));
  temp.val[1] = vcombine_s16(val.val[1], vcreate_s16(__AARCH64_INT64_C(0)));
  temp.val[2] = vcombine_s16(val.val[2], vcreate_s16(__AARCH64_INT64_C(0)));
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)temp.val[2], 2);
  __builtin_aarch64_st3v4hi((__builtin_aarch64_simd_hi *)__a, __o);
}

FUNK(void)
vst3_p16(poly16_t *__a, poly16x4x3_t val) {
  __builtin_aarch64_simd_ci __o;
  poly16x8x3_t temp;
  temp.val[0] = vcombine_p16(val.val[0], vcreate_p16(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_p16(val.val[1], vcreate_p16(__AARCH64_UINT64_C(0)));
  temp.val[2] = vcombine_p16(val.val[2], vcreate_p16(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)temp.val[2], 2);
  __builtin_aarch64_st3v4hi((__builtin_aarch64_simd_hi *)__a, __o);
}

FUNK(void)
vst3_s32(int32_t *__a, int32x2x3_t val) {
  __builtin_aarch64_simd_ci __o;
  int32x4x3_t temp;
  temp.val[0] = vcombine_s32(val.val[0], vcreate_s32(__AARCH64_INT64_C(0)));
  temp.val[1] = vcombine_s32(val.val[1], vcreate_s32(__AARCH64_INT64_C(0)));
  temp.val[2] = vcombine_s32(val.val[2], vcreate_s32(__AARCH64_INT64_C(0)));
  __o = __builtin_aarch64_set_qregciv4si(__o, (int32x4_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregciv4si(__o, (int32x4_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregciv4si(__o, (int32x4_t)temp.val[2], 2);
  __builtin_aarch64_st3v2si((__builtin_aarch64_simd_si *)__a, __o);
}

FUNK(void)
vst3_u8(uint8_t *__a, uint8x8x3_t val) {
  __builtin_aarch64_simd_ci __o;
  uint8x16x3_t temp;
  temp.val[0] = vcombine_u8(val.val[0], vcreate_u8(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_u8(val.val[1], vcreate_u8(__AARCH64_UINT64_C(0)));
  temp.val[2] = vcombine_u8(val.val[2], vcreate_u8(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)temp.val[2], 2);
  __builtin_aarch64_st3v8qi((__builtin_aarch64_simd_qi *)__a, __o);
}

FUNK(void)
vst3_u16(uint16_t *__a, uint16x4x3_t val) {
  __builtin_aarch64_simd_ci __o;
  uint16x8x3_t temp;
  temp.val[0] = vcombine_u16(val.val[0], vcreate_u16(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_u16(val.val[1], vcreate_u16(__AARCH64_UINT64_C(0)));
  temp.val[2] = vcombine_u16(val.val[2], vcreate_u16(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)temp.val[2], 2);
  __builtin_aarch64_st3v4hi((__builtin_aarch64_simd_hi *)__a, __o);
}

FUNK(void)
vst3_u32(uint32_t *__a, uint32x2x3_t val) {
  __builtin_aarch64_simd_ci __o;
  uint32x4x3_t temp;
  temp.val[0] = vcombine_u32(val.val[0], vcreate_u32(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_u32(val.val[1], vcreate_u32(__AARCH64_UINT64_C(0)));
  temp.val[2] = vcombine_u32(val.val[2], vcreate_u32(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregciv4si(__o, (int32x4_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregciv4si(__o, (int32x4_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregciv4si(__o, (int32x4_t)temp.val[2], 2);
  __builtin_aarch64_st3v2si((__builtin_aarch64_simd_si *)__a, __o);
}

FUNK(void)
vst3_f16(float16_t *__a, float16x4x3_t val) {
  __builtin_aarch64_simd_ci __o;
  float16x8x3_t temp;
  temp.val[0] = vcombine_f16(val.val[0], vcreate_f16(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_f16(val.val[1], vcreate_f16(__AARCH64_UINT64_C(0)));
  temp.val[2] = vcombine_f16(val.val[2], vcreate_f16(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregciv8hf(__o, (float16x8_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregciv8hf(__o, (float16x8_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregciv8hf(__o, (float16x8_t)temp.val[2], 2);
  __builtin_aarch64_st3v4hf((__builtin_aarch64_simd_hf *)__a, __o);
}

FUNK(void)
vst3_f32(float32_t *__a, float32x2x3_t val) {
  __builtin_aarch64_simd_ci __o;
  float32x4x3_t temp;
  temp.val[0] = vcombine_f32(val.val[0], vcreate_f32(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_f32(val.val[1], vcreate_f32(__AARCH64_UINT64_C(0)));
  temp.val[2] = vcombine_f32(val.val[2], vcreate_f32(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregciv4sf(__o, (float32x4_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregciv4sf(__o, (float32x4_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregciv4sf(__o, (float32x4_t)temp.val[2], 2);
  __builtin_aarch64_st3v2sf((__builtin_aarch64_simd_sf *)__a, __o);
}

FUNK(void)
vst3_p64(poly64_t *__a, poly64x1x3_t val) {
  __builtin_aarch64_simd_ci __o;
  poly64x2x3_t temp;
  temp.val[0] = vcombine_p64(val.val[0], vcreate_p64(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_p64(val.val[1], vcreate_p64(__AARCH64_UINT64_C(0)));
  temp.val[2] = vcombine_p64(val.val[2], vcreate_p64(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregciv2di_ssps(__o, (poly64x2_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregciv2di_ssps(__o, (poly64x2_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregciv2di_ssps(__o, (poly64x2_t)temp.val[2], 2);
  __builtin_aarch64_st3di((__builtin_aarch64_simd_di *)__a, __o);
}

FUNK(void)
vst3q_s8(int8_t *__a, int8x16x3_t val) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)val.val[2], 2);
  __builtin_aarch64_st3v16qi((__builtin_aarch64_simd_qi *)__a, __o);
}

FUNK(void)
vst3q_p8(poly8_t *__a, poly8x16x3_t val) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)val.val[2], 2);
  __builtin_aarch64_st3v16qi((__builtin_aarch64_simd_qi *)__a, __o);
}

FUNK(void)
vst3q_s16(int16_t *__a, int16x8x3_t val) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)val.val[2], 2);
  __builtin_aarch64_st3v8hi((__builtin_aarch64_simd_hi *)__a, __o);
}

FUNK(void)
vst3q_p16(poly16_t *__a, poly16x8x3_t val) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)val.val[2], 2);
  __builtin_aarch64_st3v8hi((__builtin_aarch64_simd_hi *)__a, __o);
}

FUNK(void)
vst3q_s32(int32_t *__a, int32x4x3_t val) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv4si(__o, (int32x4_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregciv4si(__o, (int32x4_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregciv4si(__o, (int32x4_t)val.val[2], 2);
  __builtin_aarch64_st3v4si((__builtin_aarch64_simd_si *)__a, __o);
}

FUNK(void)
vst3q_s64(int64_t *__a, int64x2x3_t val) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv2di(__o, (int64x2_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregciv2di(__o, (int64x2_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregciv2di(__o, (int64x2_t)val.val[2], 2);
  __builtin_aarch64_st3v2di((__builtin_aarch64_simd_di *)__a, __o);
}

FUNK(void)
vst3q_u8(uint8_t *__a, uint8x16x3_t val) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregciv16qi(__o, (int8x16_t)val.val[2], 2);
  __builtin_aarch64_st3v16qi((__builtin_aarch64_simd_qi *)__a, __o);
}

FUNK(void)
vst3q_u16(uint16_t *__a, uint16x8x3_t val) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregciv8hi(__o, (int16x8_t)val.val[2], 2);
  __builtin_aarch64_st3v8hi((__builtin_aarch64_simd_hi *)__a, __o);
}

FUNK(void)
vst3q_u32(uint32_t *__a, uint32x4x3_t val) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv4si(__o, (int32x4_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregciv4si(__o, (int32x4_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregciv4si(__o, (int32x4_t)val.val[2], 2);
  __builtin_aarch64_st3v4si((__builtin_aarch64_simd_si *)__a, __o);
}

FUNK(void)
vst3q_u64(uint64_t *__a, uint64x2x3_t val) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv2di(__o, (int64x2_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregciv2di(__o, (int64x2_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregciv2di(__o, (int64x2_t)val.val[2], 2);
  __builtin_aarch64_st3v2di((__builtin_aarch64_simd_di *)__a, __o);
}

FUNK(void)
vst3q_f16(float16_t *__a, float16x8x3_t val) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv8hf(__o, (float16x8_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregciv8hf(__o, (float16x8_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregciv8hf(__o, (float16x8_t)val.val[2], 2);
  __builtin_aarch64_st3v8hf((__builtin_aarch64_simd_hf *)__a, __o);
}

FUNK(void)
vst3q_f32(float32_t *__a, float32x4x3_t val) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv4sf(__o, (float32x4_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregciv4sf(__o, (float32x4_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregciv4sf(__o, (float32x4_t)val.val[2], 2);
  __builtin_aarch64_st3v4sf((__builtin_aarch64_simd_sf *)__a, __o);
}

FUNK(void)
vst3q_f64(float64_t *__a, float64x2x3_t val) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv2df(__o, (float64x2_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregciv2df(__o, (float64x2_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregciv2df(__o, (float64x2_t)val.val[2], 2);
  __builtin_aarch64_st3v2df((__builtin_aarch64_simd_df *)__a, __o);
}

FUNK(void)
vst3q_p64(poly64_t *__a, poly64x2x3_t val) {
  __builtin_aarch64_simd_ci __o;
  __o = __builtin_aarch64_set_qregciv2di_ssps(__o, (poly64x2_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregciv2di_ssps(__o, (poly64x2_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregciv2di_ssps(__o, (poly64x2_t)val.val[2], 2);
  __builtin_aarch64_st3v2di((__builtin_aarch64_simd_di *)__a, __o);
}

FUNK(void)
vst4_s64(int64_t *__a, int64x1x4_t val) {
  __builtin_aarch64_simd_xi __o;
  int64x2x4_t temp;
  temp.val[0] = vcombine_s64(val.val[0], vcreate_s64(__AARCH64_INT64_C(0)));
  temp.val[1] = vcombine_s64(val.val[1], vcreate_s64(__AARCH64_INT64_C(0)));
  temp.val[2] = vcombine_s64(val.val[2], vcreate_s64(__AARCH64_INT64_C(0)));
  temp.val[3] = vcombine_s64(val.val[3], vcreate_s64(__AARCH64_INT64_C(0)));
  __o = __builtin_aarch64_set_qregxiv2di(__o, (int64x2_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv2di(__o, (int64x2_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv2di(__o, (int64x2_t)temp.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv2di(__o, (int64x2_t)temp.val[3], 3);
  __builtin_aarch64_st4di((__builtin_aarch64_simd_di *)__a, __o);
}

FUNK(void)
vst4_u64(uint64_t *__a, uint64x1x4_t val) {
  __builtin_aarch64_simd_xi __o;
  uint64x2x4_t temp;
  temp.val[0] = vcombine_u64(val.val[0], vcreate_u64(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_u64(val.val[1], vcreate_u64(__AARCH64_UINT64_C(0)));
  temp.val[2] = vcombine_u64(val.val[2], vcreate_u64(__AARCH64_UINT64_C(0)));
  temp.val[3] = vcombine_u64(val.val[3], vcreate_u64(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregxiv2di(__o, (int64x2_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv2di(__o, (int64x2_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv2di(__o, (int64x2_t)temp.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv2di(__o, (int64x2_t)temp.val[3], 3);
  __builtin_aarch64_st4di((__builtin_aarch64_simd_di *)__a, __o);
}

FUNK(void)
vst4_f64(float64_t *__a, float64x1x4_t val) {
  __builtin_aarch64_simd_xi __o;
  float64x2x4_t temp;
  temp.val[0] = vcombine_f64(val.val[0], vcreate_f64(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_f64(val.val[1], vcreate_f64(__AARCH64_UINT64_C(0)));
  temp.val[2] = vcombine_f64(val.val[2], vcreate_f64(__AARCH64_UINT64_C(0)));
  temp.val[3] = vcombine_f64(val.val[3], vcreate_f64(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregxiv2df(__o, (float64x2_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv2df(__o, (float64x2_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv2df(__o, (float64x2_t)temp.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv2df(__o, (float64x2_t)temp.val[3], 3);
  __builtin_aarch64_st4df((__builtin_aarch64_simd_df *)__a, __o);
}

FUNK(void)
vst4_s8(int8_t *__a, int8x8x4_t val) {
  __builtin_aarch64_simd_xi __o;
  int8x16x4_t temp;
  temp.val[0] = vcombine_s8(val.val[0], vcreate_s8(__AARCH64_INT64_C(0)));
  temp.val[1] = vcombine_s8(val.val[1], vcreate_s8(__AARCH64_INT64_C(0)));
  temp.val[2] = vcombine_s8(val.val[2], vcreate_s8(__AARCH64_INT64_C(0)));
  temp.val[3] = vcombine_s8(val.val[3], vcreate_s8(__AARCH64_INT64_C(0)));
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)temp.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)temp.val[3], 3);
  __builtin_aarch64_st4v8qi((__builtin_aarch64_simd_qi *)__a, __o);
}

FUNK(void)
vst4_p8(poly8_t *__a, poly8x8x4_t val) {
  __builtin_aarch64_simd_xi __o;
  poly8x16x4_t temp;
  temp.val[0] = vcombine_p8(val.val[0], vcreate_p8(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_p8(val.val[1], vcreate_p8(__AARCH64_UINT64_C(0)));
  temp.val[2] = vcombine_p8(val.val[2], vcreate_p8(__AARCH64_UINT64_C(0)));
  temp.val[3] = vcombine_p8(val.val[3], vcreate_p8(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)temp.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)temp.val[3], 3);
  __builtin_aarch64_st4v8qi((__builtin_aarch64_simd_qi *)__a, __o);
}

FUNK(void)
vst4_s16(int16_t *__a, int16x4x4_t val) {
  __builtin_aarch64_simd_xi __o;
  int16x8x4_t temp;
  temp.val[0] = vcombine_s16(val.val[0], vcreate_s16(__AARCH64_INT64_C(0)));
  temp.val[1] = vcombine_s16(val.val[1], vcreate_s16(__AARCH64_INT64_C(0)));
  temp.val[2] = vcombine_s16(val.val[2], vcreate_s16(__AARCH64_INT64_C(0)));
  temp.val[3] = vcombine_s16(val.val[3], vcreate_s16(__AARCH64_INT64_C(0)));
  __o = __builtin_aarch64_set_qregxiv8hi(__o, (int16x8_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv8hi(__o, (int16x8_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv8hi(__o, (int16x8_t)temp.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv8hi(__o, (int16x8_t)temp.val[3], 3);
  __builtin_aarch64_st4v4hi((__builtin_aarch64_simd_hi *)__a, __o);
}

FUNK(void)
vst4_p16(poly16_t *__a, poly16x4x4_t val) {
  __builtin_aarch64_simd_xi __o;
  poly16x8x4_t temp;
  temp.val[0] = vcombine_p16(val.val[0], vcreate_p16(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_p16(val.val[1], vcreate_p16(__AARCH64_UINT64_C(0)));
  temp.val[2] = vcombine_p16(val.val[2], vcreate_p16(__AARCH64_UINT64_C(0)));
  temp.val[3] = vcombine_p16(val.val[3], vcreate_p16(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregxiv8hi(__o, (int16x8_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv8hi(__o, (int16x8_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv8hi(__o, (int16x8_t)temp.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv8hi(__o, (int16x8_t)temp.val[3], 3);
  __builtin_aarch64_st4v4hi((__builtin_aarch64_simd_hi *)__a, __o);
}

FUNK(void)
vst4_s32(int32_t *__a, int32x2x4_t val) {
  __builtin_aarch64_simd_xi __o;
  int32x4x4_t temp;
  temp.val[0] = vcombine_s32(val.val[0], vcreate_s32(__AARCH64_INT64_C(0)));
  temp.val[1] = vcombine_s32(val.val[1], vcreate_s32(__AARCH64_INT64_C(0)));
  temp.val[2] = vcombine_s32(val.val[2], vcreate_s32(__AARCH64_INT64_C(0)));
  temp.val[3] = vcombine_s32(val.val[3], vcreate_s32(__AARCH64_INT64_C(0)));
  __o = __builtin_aarch64_set_qregxiv4si(__o, (int32x4_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv4si(__o, (int32x4_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv4si(__o, (int32x4_t)temp.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv4si(__o, (int32x4_t)temp.val[3], 3);
  __builtin_aarch64_st4v2si((__builtin_aarch64_simd_si *)__a, __o);
}

FUNK(void)
vst4_u8(uint8_t *__a, uint8x8x4_t val) {
  __builtin_aarch64_simd_xi __o;
  uint8x16x4_t temp;
  temp.val[0] = vcombine_u8(val.val[0], vcreate_u8(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_u8(val.val[1], vcreate_u8(__AARCH64_UINT64_C(0)));
  temp.val[2] = vcombine_u8(val.val[2], vcreate_u8(__AARCH64_UINT64_C(0)));
  temp.val[3] = vcombine_u8(val.val[3], vcreate_u8(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)temp.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)temp.val[3], 3);
  __builtin_aarch64_st4v8qi((__builtin_aarch64_simd_qi *)__a, __o);
}

FUNK(void)
vst4_u16(uint16_t *__a, uint16x4x4_t val) {
  __builtin_aarch64_simd_xi __o;
  uint16x8x4_t temp;
  temp.val[0] = vcombine_u16(val.val[0], vcreate_u16(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_u16(val.val[1], vcreate_u16(__AARCH64_UINT64_C(0)));
  temp.val[2] = vcombine_u16(val.val[2], vcreate_u16(__AARCH64_UINT64_C(0)));
  temp.val[3] = vcombine_u16(val.val[3], vcreate_u16(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregxiv8hi(__o, (int16x8_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv8hi(__o, (int16x8_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv8hi(__o, (int16x8_t)temp.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv8hi(__o, (int16x8_t)temp.val[3], 3);
  __builtin_aarch64_st4v4hi((__builtin_aarch64_simd_hi *)__a, __o);
}

FUNK(void)
vst4_u32(uint32_t *__a, uint32x2x4_t val) {
  __builtin_aarch64_simd_xi __o;
  uint32x4x4_t temp;
  temp.val[0] = vcombine_u32(val.val[0], vcreate_u32(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_u32(val.val[1], vcreate_u32(__AARCH64_UINT64_C(0)));
  temp.val[2] = vcombine_u32(val.val[2], vcreate_u32(__AARCH64_UINT64_C(0)));
  temp.val[3] = vcombine_u32(val.val[3], vcreate_u32(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregxiv4si(__o, (int32x4_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv4si(__o, (int32x4_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv4si(__o, (int32x4_t)temp.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv4si(__o, (int32x4_t)temp.val[3], 3);
  __builtin_aarch64_st4v2si((__builtin_aarch64_simd_si *)__a, __o);
}

FUNK(void)
vst4_f16(float16_t *__a, float16x4x4_t val) {
  __builtin_aarch64_simd_xi __o;
  float16x8x4_t temp;
  temp.val[0] = vcombine_f16(val.val[0], vcreate_f16(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_f16(val.val[1], vcreate_f16(__AARCH64_UINT64_C(0)));
  temp.val[2] = vcombine_f16(val.val[2], vcreate_f16(__AARCH64_UINT64_C(0)));
  temp.val[3] = vcombine_f16(val.val[3], vcreate_f16(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregxiv8hf(__o, (float16x8_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv8hf(__o, (float16x8_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv8hf(__o, (float16x8_t)temp.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv8hf(__o, (float16x8_t)temp.val[3], 3);
  __builtin_aarch64_st4v4hf((__builtin_aarch64_simd_hf *)__a, __o);
}

FUNK(void)
vst4_f32(float32_t *__a, float32x2x4_t val) {
  __builtin_aarch64_simd_xi __o;
  float32x4x4_t temp;
  temp.val[0] = vcombine_f32(val.val[0], vcreate_f32(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_f32(val.val[1], vcreate_f32(__AARCH64_UINT64_C(0)));
  temp.val[2] = vcombine_f32(val.val[2], vcreate_f32(__AARCH64_UINT64_C(0)));
  temp.val[3] = vcombine_f32(val.val[3], vcreate_f32(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregxiv4sf(__o, (float32x4_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv4sf(__o, (float32x4_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv4sf(__o, (float32x4_t)temp.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv4sf(__o, (float32x4_t)temp.val[3], 3);
  __builtin_aarch64_st4v2sf((__builtin_aarch64_simd_sf *)__a, __o);
}

FUNK(void)
vst4_p64(poly64_t *__a, poly64x1x4_t val) {
  __builtin_aarch64_simd_xi __o;
  poly64x2x4_t temp;
  temp.val[0] = vcombine_p64(val.val[0], vcreate_p64(__AARCH64_UINT64_C(0)));
  temp.val[1] = vcombine_p64(val.val[1], vcreate_p64(__AARCH64_UINT64_C(0)));
  temp.val[2] = vcombine_p64(val.val[2], vcreate_p64(__AARCH64_UINT64_C(0)));
  temp.val[3] = vcombine_p64(val.val[3], vcreate_p64(__AARCH64_UINT64_C(0)));
  __o = __builtin_aarch64_set_qregxiv2di_ssps(__o, (poly64x2_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv2di_ssps(__o, (poly64x2_t)temp.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv2di_ssps(__o, (poly64x2_t)temp.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv2di_ssps(__o, (poly64x2_t)temp.val[3], 3);
  __builtin_aarch64_st4di((__builtin_aarch64_simd_di *)__a, __o);
}

FUNK(void)
vst4q_s8(int8_t *__a, int8x16x4_t val) {
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)val.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)val.val[3], 3);
  __builtin_aarch64_st4v16qi((__builtin_aarch64_simd_qi *)__a, __o);
}

FUNK(void)
vst4q_p8(poly8_t *__a, poly8x16x4_t val) {
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)val.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)val.val[3], 3);
  __builtin_aarch64_st4v16qi((__builtin_aarch64_simd_qi *)__a, __o);
}

FUNK(void)
vst4q_s16(int16_t *__a, int16x8x4_t val) {
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_set_qregxiv8hi(__o, (int16x8_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv8hi(__o, (int16x8_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv8hi(__o, (int16x8_t)val.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv8hi(__o, (int16x8_t)val.val[3], 3);
  __builtin_aarch64_st4v8hi((__builtin_aarch64_simd_hi *)__a, __o);
}

FUNK(void)
vst4q_p16(poly16_t *__a, poly16x8x4_t val) {
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_set_qregxiv8hi(__o, (int16x8_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv8hi(__o, (int16x8_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv8hi(__o, (int16x8_t)val.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv8hi(__o, (int16x8_t)val.val[3], 3);
  __builtin_aarch64_st4v8hi((__builtin_aarch64_simd_hi *)__a, __o);
}

FUNK(void)
vst4q_s32(int32_t *__a, int32x4x4_t val) {
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_set_qregxiv4si(__o, (int32x4_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv4si(__o, (int32x4_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv4si(__o, (int32x4_t)val.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv4si(__o, (int32x4_t)val.val[3], 3);
  __builtin_aarch64_st4v4si((__builtin_aarch64_simd_si *)__a, __o);
}

FUNK(void)
vst4q_s64(int64_t *__a, int64x2x4_t val) {
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_set_qregxiv2di(__o, (int64x2_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv2di(__o, (int64x2_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv2di(__o, (int64x2_t)val.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv2di(__o, (int64x2_t)val.val[3], 3);
  __builtin_aarch64_st4v2di((__builtin_aarch64_simd_di *)__a, __o);
}

FUNK(void)
vst4q_u8(uint8_t *__a, uint8x16x4_t val) {
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)val.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv16qi(__o, (int8x16_t)val.val[3], 3);
  __builtin_aarch64_st4v16qi((__builtin_aarch64_simd_qi *)__a, __o);
}

FUNK(void)
vst4q_u16(uint16_t *__a, uint16x8x4_t val) {
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_set_qregxiv8hi(__o, (int16x8_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv8hi(__o, (int16x8_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv8hi(__o, (int16x8_t)val.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv8hi(__o, (int16x8_t)val.val[3], 3);
  __builtin_aarch64_st4v8hi((__builtin_aarch64_simd_hi *)__a, __o);
}

FUNK(void)
vst4q_u32(uint32_t *__a, uint32x4x4_t val) {
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_set_qregxiv4si(__o, (int32x4_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv4si(__o, (int32x4_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv4si(__o, (int32x4_t)val.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv4si(__o, (int32x4_t)val.val[3], 3);
  __builtin_aarch64_st4v4si((__builtin_aarch64_simd_si *)__a, __o);
}

FUNK(void)
vst4q_u64(uint64_t *__a, uint64x2x4_t val) {
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_set_qregxiv2di(__o, (int64x2_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv2di(__o, (int64x2_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv2di(__o, (int64x2_t)val.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv2di(__o, (int64x2_t)val.val[3], 3);
  __builtin_aarch64_st4v2di((__builtin_aarch64_simd_di *)__a, __o);
}

FUNK(void)
vst4q_f16(float16_t *__a, float16x8x4_t val) {
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_set_qregxiv8hf(__o, (float16x8_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv8hf(__o, (float16x8_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv8hf(__o, (float16x8_t)val.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv8hf(__o, (float16x8_t)val.val[3], 3);
  __builtin_aarch64_st4v8hf((__builtin_aarch64_simd_hf *)__a, __o);
}

FUNK(void)
vst4q_f32(float32_t *__a, float32x4x4_t val) {
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_set_qregxiv4sf(__o, (float32x4_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv4sf(__o, (float32x4_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv4sf(__o, (float32x4_t)val.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv4sf(__o, (float32x4_t)val.val[3], 3);
  __builtin_aarch64_st4v4sf((__builtin_aarch64_simd_sf *)__a, __o);
}

FUNK(void)
vst4q_f64(float64_t *__a, float64x2x4_t val) {
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_set_qregxiv2df(__o, (float64x2_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv2df(__o, (float64x2_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv2df(__o, (float64x2_t)val.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv2df(__o, (float64x2_t)val.val[3], 3);
  __builtin_aarch64_st4v2df((__builtin_aarch64_simd_df *)__a, __o);
}

FUNK(void)
vst4q_p64(poly64_t *__a, poly64x2x4_t val) {
  __builtin_aarch64_simd_xi __o;
  __o = __builtin_aarch64_set_qregxiv2di_ssps(__o, (poly64x2_t)val.val[0], 0);
  __o = __builtin_aarch64_set_qregxiv2di_ssps(__o, (poly64x2_t)val.val[1], 1);
  __o = __builtin_aarch64_set_qregxiv2di_ssps(__o, (poly64x2_t)val.val[2], 2);
  __o = __builtin_aarch64_set_qregxiv2di_ssps(__o, (poly64x2_t)val.val[3], 3);
  __builtin_aarch64_st4v2di((__builtin_aarch64_simd_di *)__a, __o);
}

FUNK(int64_t)
vsubd_s64(int64_t __a, int64_t __b) {
  return __a - __b;
}

FUNK(uint64_t)
vsubd_u64(uint64_t __a, uint64_t __b) {
  return __a - __b;
}

FUNK(int8x8_t)
vtbx1_s8(int8x8_t __r, int8x8_t __tab, int8x8_t __idx) {
  uint8x8_t __mask = vclt_u8(vreinterpret_u8_s8(__idx), vmov_n_u8(8));
  int8x8_t __tbl = vtbl1_s8(__tab, __idx);

  return vbsl_s8(__mask, __tbl, __r);
}

FUNK(uint8x8_t)
vtbx1_u8(uint8x8_t __r, uint8x8_t __tab, uint8x8_t __idx) {
  uint8x8_t __mask = vclt_u8(__idx, vmov_n_u8(8));
  uint8x8_t __tbl = vtbl1_u8(__tab, __idx);

  return vbsl_u8(__mask, __tbl, __r);
}

FUNK(poly8x8_t)
vtbx1_p8(poly8x8_t __r, poly8x8_t __tab, uint8x8_t __idx) {
  uint8x8_t __mask = vclt_u8(__idx, vmov_n_u8(8));
  poly8x8_t __tbl = vtbl1_p8(__tab, __idx);

  return vbsl_p8(__mask, __tbl, __r);
}

FUNK(int8x8_t)
vtbx3_s8(int8x8_t __r, int8x8x3_t __tab, int8x8_t __idx) {
  uint8x8_t __mask = vclt_u8(vreinterpret_u8_s8(__idx), vmov_n_u8(24));
  int8x8_t __tbl = vtbl3_s8(__tab, __idx);

  return vbsl_s8(__mask, __tbl, __r);
}

FUNK(uint8x8_t)
vtbx3_u8(uint8x8_t __r, uint8x8x3_t __tab, uint8x8_t __idx) {
  uint8x8_t __mask = vclt_u8(__idx, vmov_n_u8(24));
  uint8x8_t __tbl = vtbl3_u8(__tab, __idx);

  return vbsl_u8(__mask, __tbl, __r);
}

FUNK(poly8x8_t)
vtbx3_p8(poly8x8_t __r, poly8x8x3_t __tab, uint8x8_t __idx) {
  uint8x8_t __mask = vclt_u8(__idx, vmov_n_u8(24));
  poly8x8_t __tbl = vtbl3_p8(__tab, __idx);

  return vbsl_p8(__mask, __tbl, __r);
}

FUNK(int8x8_t)
vtbx4_s8(int8x8_t __r, int8x8x4_t __tab, int8x8_t __idx) {
  int8x8_t result;
  int8x16x2_t temp;
  __builtin_aarch64_simd_oi __o;
  temp.val[0] = vcombine_s8(__tab.val[0], __tab.val[1]);
  temp.val[1] = vcombine_s8(__tab.val[2], __tab.val[3]);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)temp.val[1], 1);
  result = __builtin_aarch64_tbx4v8qi(__r, __o, __idx);
  return result;
}

FUNK(uint8x8_t)
vtbx4_u8(uint8x8_t __r, uint8x8x4_t __tab, uint8x8_t __idx) {
  uint8x8_t result;
  uint8x16x2_t temp;
  __builtin_aarch64_simd_oi __o;
  temp.val[0] = vcombine_u8(__tab.val[0], __tab.val[1]);
  temp.val[1] = vcombine_u8(__tab.val[2], __tab.val[3]);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)temp.val[1], 1);
  result = (uint8x8_t)__builtin_aarch64_tbx4v8qi((int8x8_t)__r, __o,
                                                 (int8x8_t)__idx);
  return result;
}

FUNK(poly8x8_t)
vtbx4_p8(poly8x8_t __r, poly8x8x4_t __tab, uint8x8_t __idx) {
  poly8x8_t result;
  poly8x16x2_t temp;
  __builtin_aarch64_simd_oi __o;
  temp.val[0] = vcombine_p8(__tab.val[0], __tab.val[1]);
  temp.val[1] = vcombine_p8(__tab.val[2], __tab.val[3]);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)temp.val[0], 0);
  __o = __builtin_aarch64_set_qregoiv16qi(__o, (int8x16_t)temp.val[1], 1);
  result = (poly8x8_t)__builtin_aarch64_tbx4v8qi((int8x8_t)__r, __o,
                                                 (int8x8_t)__idx);
  return result;
}

FUNK(float16x4_t)
vtrn1_f16(float16x4_t __a, float16x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x4_t){5, 1, 7, 3});
#else
  return __builtin_shuffle(__a, __b, (uint16x4_t){0, 4, 2, 6});
#endif
}

FUNK(float32x2_t)
vtrn1_f32(float32x2_t __a, float32x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x2_t){3, 1});
#else
  return __builtin_shuffle(__a, __b, (uint32x2_t){0, 2});
#endif
}

FUNK(poly8x8_t)
vtrn1_p8(poly8x8_t __a, poly8x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint8x8_t){9, 1, 11, 3, 13, 5, 15, 7});
#else
  return __builtin_shuffle(__a, __b, (uint8x8_t){0, 8, 2, 10, 4, 12, 6, 14});
#endif
}

FUNK(poly16x4_t)
vtrn1_p16(poly16x4_t __a, poly16x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x4_t){5, 1, 7, 3});
#else
  return __builtin_shuffle(__a, __b, (uint16x4_t){0, 4, 2, 6});
#endif
}

FUNK(int8x8_t)
vtrn1_s8(int8x8_t __a, int8x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint8x8_t){9, 1, 11, 3, 13, 5, 15, 7});
#else
  return __builtin_shuffle(__a, __b, (uint8x8_t){0, 8, 2, 10, 4, 12, 6, 14});
#endif
}

FUNK(int16x4_t)
vtrn1_s16(int16x4_t __a, int16x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x4_t){5, 1, 7, 3});
#else
  return __builtin_shuffle(__a, __b, (uint16x4_t){0, 4, 2, 6});
#endif
}

FUNK(int32x2_t)
vtrn1_s32(int32x2_t __a, int32x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x2_t){3, 1});
#else
  return __builtin_shuffle(__a, __b, (uint32x2_t){0, 2});
#endif
}

FUNK(uint8x8_t)
vtrn1_u8(uint8x8_t __a, uint8x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint8x8_t){9, 1, 11, 3, 13, 5, 15, 7});
#else
  return __builtin_shuffle(__a, __b, (uint8x8_t){0, 8, 2, 10, 4, 12, 6, 14});
#endif
}

FUNK(uint16x4_t)
vtrn1_u16(uint16x4_t __a, uint16x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x4_t){5, 1, 7, 3});
#else
  return __builtin_shuffle(__a, __b, (uint16x4_t){0, 4, 2, 6});
#endif
}

FUNK(uint32x2_t)
vtrn1_u32(uint32x2_t __a, uint32x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x2_t){3, 1});
#else
  return __builtin_shuffle(__a, __b, (uint32x2_t){0, 2});
#endif
}

FUNK(float16x8_t)
vtrn1q_f16(float16x8_t __a, float16x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x8_t){9, 1, 11, 3, 13, 5, 15, 7});
#else
  return __builtin_shuffle(__a, __b, (uint16x8_t){0, 8, 2, 10, 4, 12, 6, 14});
#endif
}

FUNK(float32x4_t)
vtrn1q_f32(float32x4_t __a, float32x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x4_t){5, 1, 7, 3});
#else
  return __builtin_shuffle(__a, __b, (uint32x4_t){0, 4, 2, 6});
#endif
}

FUNK(float64x2_t)
vtrn1q_f64(float64x2_t __a, float64x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint64x2_t){3, 1});
#else
  return __builtin_shuffle(__a, __b, (uint64x2_t){0, 2});
#endif
}

FUNK(poly8x16_t)
vtrn1q_p8(poly8x16_t __a, poly8x16_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(
      __a, __b,
      (uint8x16_t){17, 1, 19, 3, 21, 5, 23, 7, 25, 9, 27, 11, 29, 13, 31, 15});
#else
  return __builtin_shuffle(
      __a, __b,
      (uint8x16_t){0, 16, 2, 18, 4, 20, 6, 22, 8, 24, 10, 26, 12, 28, 14, 30});
#endif
}

FUNK(poly16x8_t)
vtrn1q_p16(poly16x8_t __a, poly16x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x8_t){9, 1, 11, 3, 13, 5, 15, 7});
#else
  return __builtin_shuffle(__a, __b, (uint16x8_t){0, 8, 2, 10, 4, 12, 6, 14});
#endif
}

FUNK(int8x16_t)
vtrn1q_s8(int8x16_t __a, int8x16_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(
      __a, __b,
      (uint8x16_t){17, 1, 19, 3, 21, 5, 23, 7, 25, 9, 27, 11, 29, 13, 31, 15});
#else
  return __builtin_shuffle(
      __a, __b,
      (uint8x16_t){0, 16, 2, 18, 4, 20, 6, 22, 8, 24, 10, 26, 12, 28, 14, 30});
#endif
}

FUNK(int16x8_t)
vtrn1q_s16(int16x8_t __a, int16x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x8_t){9, 1, 11, 3, 13, 5, 15, 7});
#else
  return __builtin_shuffle(__a, __b, (uint16x8_t){0, 8, 2, 10, 4, 12, 6, 14});
#endif
}

FUNK(int32x4_t)
vtrn1q_s32(int32x4_t __a, int32x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x4_t){5, 1, 7, 3});
#else
  return __builtin_shuffle(__a, __b, (uint32x4_t){0, 4, 2, 6});
#endif
}

FUNK(int64x2_t)
vtrn1q_s64(int64x2_t __a, int64x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint64x2_t){3, 1});
#else
  return __builtin_shuffle(__a, __b, (uint64x2_t){0, 2});
#endif
}

FUNK(uint8x16_t)
vtrn1q_u8(uint8x16_t __a, uint8x16_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(
      __a, __b,
      (uint8x16_t){17, 1, 19, 3, 21, 5, 23, 7, 25, 9, 27, 11, 29, 13, 31, 15});
#else
  return __builtin_shuffle(
      __a, __b,
      (uint8x16_t){0, 16, 2, 18, 4, 20, 6, 22, 8, 24, 10, 26, 12, 28, 14, 30});
#endif
}

FUNK(uint16x8_t)
vtrn1q_u16(uint16x8_t __a, uint16x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x8_t){9, 1, 11, 3, 13, 5, 15, 7});
#else
  return __builtin_shuffle(__a, __b, (uint16x8_t){0, 8, 2, 10, 4, 12, 6, 14});
#endif
}

FUNK(uint32x4_t)
vtrn1q_u32(uint32x4_t __a, uint32x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x4_t){5, 1, 7, 3});
#else
  return __builtin_shuffle(__a, __b, (uint32x4_t){0, 4, 2, 6});
#endif
}

FUNK(uint64x2_t)
vtrn1q_u64(uint64x2_t __a, uint64x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint64x2_t){3, 1});
#else
  return __builtin_shuffle(__a, __b, (uint64x2_t){0, 2});
#endif
}

FUNK(float16x4_t)
vtrn2_f16(float16x4_t __a, float16x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x4_t){4, 0, 6, 2});
#else
  return __builtin_shuffle(__a, __b, (uint16x4_t){1, 5, 3, 7});
#endif
}

FUNK(float32x2_t)
vtrn2_f32(float32x2_t __a, float32x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x2_t){2, 0});
#else
  return __builtin_shuffle(__a, __b, (uint32x2_t){1, 3});
#endif
}

FUNK(poly8x8_t)
vtrn2_p8(poly8x8_t __a, poly8x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint8x8_t){8, 0, 10, 2, 12, 4, 14, 6});
#else
  return __builtin_shuffle(__a, __b, (uint8x8_t){1, 9, 3, 11, 5, 13, 7, 15});
#endif
}

FUNK(poly16x4_t)
vtrn2_p16(poly16x4_t __a, poly16x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x4_t){4, 0, 6, 2});
#else
  return __builtin_shuffle(__a, __b, (uint16x4_t){1, 5, 3, 7});
#endif
}

FUNK(int8x8_t)
vtrn2_s8(int8x8_t __a, int8x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint8x8_t){8, 0, 10, 2, 12, 4, 14, 6});
#else
  return __builtin_shuffle(__a, __b, (uint8x8_t){1, 9, 3, 11, 5, 13, 7, 15});
#endif
}

FUNK(int16x4_t)
vtrn2_s16(int16x4_t __a, int16x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x4_t){4, 0, 6, 2});
#else
  return __builtin_shuffle(__a, __b, (uint16x4_t){1, 5, 3, 7});
#endif
}

FUNK(int32x2_t)
vtrn2_s32(int32x2_t __a, int32x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x2_t){2, 0});
#else
  return __builtin_shuffle(__a, __b, (uint32x2_t){1, 3});
#endif
}

FUNK(uint8x8_t)
vtrn2_u8(uint8x8_t __a, uint8x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint8x8_t){8, 0, 10, 2, 12, 4, 14, 6});
#else
  return __builtin_shuffle(__a, __b, (uint8x8_t){1, 9, 3, 11, 5, 13, 7, 15});
#endif
}

FUNK(uint16x4_t)
vtrn2_u16(uint16x4_t __a, uint16x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x4_t){4, 0, 6, 2});
#else
  return __builtin_shuffle(__a, __b, (uint16x4_t){1, 5, 3, 7});
#endif
}

FUNK(uint32x2_t)
vtrn2_u32(uint32x2_t __a, uint32x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x2_t){2, 0});
#else
  return __builtin_shuffle(__a, __b, (uint32x2_t){1, 3});
#endif
}

FUNK(float16x8_t)
vtrn2q_f16(float16x8_t __a, float16x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x8_t){8, 0, 10, 2, 12, 4, 14, 6});
#else
  return __builtin_shuffle(__a, __b, (uint16x8_t){1, 9, 3, 11, 5, 13, 7, 15});
#endif
}

FUNK(float32x4_t)
vtrn2q_f32(float32x4_t __a, float32x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x4_t){4, 0, 6, 2});
#else
  return __builtin_shuffle(__a, __b, (uint32x4_t){1, 5, 3, 7});
#endif
}

FUNK(float64x2_t)
vtrn2q_f64(float64x2_t __a, float64x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint64x2_t){2, 0});
#else
  return __builtin_shuffle(__a, __b, (uint64x2_t){1, 3});
#endif
}

FUNK(poly8x16_t)
vtrn2q_p8(poly8x16_t __a, poly8x16_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(
      __a, __b,
      (uint8x16_t){16, 0, 18, 2, 20, 4, 22, 6, 24, 8, 26, 10, 28, 12, 30, 14});
#else
  return __builtin_shuffle(
      __a, __b,
      (uint8x16_t){1, 17, 3, 19, 5, 21, 7, 23, 9, 25, 11, 27, 13, 29, 15, 31});
#endif
}

FUNK(poly16x8_t)
vtrn2q_p16(poly16x8_t __a, poly16x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x8_t){8, 0, 10, 2, 12, 4, 14, 6});
#else
  return __builtin_shuffle(__a, __b, (uint16x8_t){1, 9, 3, 11, 5, 13, 7, 15});
#endif
}

FUNK(int8x16_t)
vtrn2q_s8(int8x16_t __a, int8x16_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(
      __a, __b,
      (uint8x16_t){16, 0, 18, 2, 20, 4, 22, 6, 24, 8, 26, 10, 28, 12, 30, 14});
#else
  return __builtin_shuffle(
      __a, __b,
      (uint8x16_t){1, 17, 3, 19, 5, 21, 7, 23, 9, 25, 11, 27, 13, 29, 15, 31});
#endif
}

FUNK(int16x8_t)
vtrn2q_s16(int16x8_t __a, int16x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x8_t){8, 0, 10, 2, 12, 4, 14, 6});
#else
  return __builtin_shuffle(__a, __b, (uint16x8_t){1, 9, 3, 11, 5, 13, 7, 15});
#endif
}

FUNK(int32x4_t)
vtrn2q_s32(int32x4_t __a, int32x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x4_t){4, 0, 6, 2});
#else
  return __builtin_shuffle(__a, __b, (uint32x4_t){1, 5, 3, 7});
#endif
}

FUNK(int64x2_t)
vtrn2q_s64(int64x2_t __a, int64x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint64x2_t){2, 0});
#else
  return __builtin_shuffle(__a, __b, (uint64x2_t){1, 3});
#endif
}

FUNK(uint8x16_t)
vtrn2q_u8(uint8x16_t __a, uint8x16_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(
      __a, __b,
      (uint8x16_t){16, 0, 18, 2, 20, 4, 22, 6, 24, 8, 26, 10, 28, 12, 30, 14});
#else
  return __builtin_shuffle(
      __a, __b,
      (uint8x16_t){1, 17, 3, 19, 5, 21, 7, 23, 9, 25, 11, 27, 13, 29, 15, 31});
#endif
}

FUNK(uint16x8_t)
vtrn2q_u16(uint16x8_t __a, uint16x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x8_t){8, 0, 10, 2, 12, 4, 14, 6});
#else
  return __builtin_shuffle(__a, __b, (uint16x8_t){1, 9, 3, 11, 5, 13, 7, 15});
#endif
}

FUNK(uint32x4_t)
vtrn2q_u32(uint32x4_t __a, uint32x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x4_t){4, 0, 6, 2});
#else
  return __builtin_shuffle(__a, __b, (uint32x4_t){1, 5, 3, 7});
#endif
}

FUNK(uint64x2_t)
vtrn2q_u64(uint64x2_t __a, uint64x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint64x2_t){2, 0});
#else
  return __builtin_shuffle(__a, __b, (uint64x2_t){1, 3});
#endif
}

FUNK(float16x4x2_t)
vtrn_f16(float16x4_t __a, float16x4_t __b) {
  return (float16x4x2_t){vtrn1_f16(__a, __b), vtrn2_f16(__a, __b)};
}

FUNK(float32x2x2_t)
vtrn_f32(float32x2_t a, float32x2_t b) {
  return (float32x2x2_t){vtrn1_f32(a, b), vtrn2_f32(a, b)};
}

FUNK(poly8x8x2_t)
vtrn_p8(poly8x8_t a, poly8x8_t b) {
  return (poly8x8x2_t){vtrn1_p8(a, b), vtrn2_p8(a, b)};
}

FUNK(poly16x4x2_t)
vtrn_p16(poly16x4_t a, poly16x4_t b) {
  return (poly16x4x2_t){vtrn1_p16(a, b), vtrn2_p16(a, b)};
}

FUNK(int8x8x2_t)
vtrn_s8(int8x8_t a, int8x8_t b) {
  return (int8x8x2_t){vtrn1_s8(a, b), vtrn2_s8(a, b)};
}

FUNK(int16x4x2_t)
vtrn_s16(int16x4_t a, int16x4_t b) {
  return (int16x4x2_t){vtrn1_s16(a, b), vtrn2_s16(a, b)};
}

FUNK(int32x2x2_t)
vtrn_s32(int32x2_t a, int32x2_t b) {
  return (int32x2x2_t){vtrn1_s32(a, b), vtrn2_s32(a, b)};
}

FUNK(uint8x8x2_t)
vtrn_u8(uint8x8_t a, uint8x8_t b) {
  return (uint8x8x2_t){vtrn1_u8(a, b), vtrn2_u8(a, b)};
}

FUNK(uint16x4x2_t)
vtrn_u16(uint16x4_t a, uint16x4_t b) {
  return (uint16x4x2_t){vtrn1_u16(a, b), vtrn2_u16(a, b)};
}

FUNK(uint32x2x2_t)
vtrn_u32(uint32x2_t a, uint32x2_t b) {
  return (uint32x2x2_t){vtrn1_u32(a, b), vtrn2_u32(a, b)};
}

FUNK(float16x8x2_t)
vtrnq_f16(float16x8_t __a, float16x8_t __b) {
  return (float16x8x2_t){vtrn1q_f16(__a, __b), vtrn2q_f16(__a, __b)};
}

FUNK(float32x4x2_t)
vtrnq_f32(float32x4_t a, float32x4_t b) {
  return (float32x4x2_t){vtrn1q_f32(a, b), vtrn2q_f32(a, b)};
}

FUNK(poly8x16x2_t)
vtrnq_p8(poly8x16_t a, poly8x16_t b) {
  return (poly8x16x2_t){vtrn1q_p8(a, b), vtrn2q_p8(a, b)};
}

FUNK(poly16x8x2_t)
vtrnq_p16(poly16x8_t a, poly16x8_t b) {
  return (poly16x8x2_t){vtrn1q_p16(a, b), vtrn2q_p16(a, b)};
}

FUNK(int8x16x2_t)
vtrnq_s8(int8x16_t a, int8x16_t b) {
  return (int8x16x2_t){vtrn1q_s8(a, b), vtrn2q_s8(a, b)};
}

FUNK(int16x8x2_t)
vtrnq_s16(int16x8_t a, int16x8_t b) {
  return (int16x8x2_t){vtrn1q_s16(a, b), vtrn2q_s16(a, b)};
}

FUNK(int32x4x2_t)
vtrnq_s32(int32x4_t a, int32x4_t b) {
  return (int32x4x2_t){vtrn1q_s32(a, b), vtrn2q_s32(a, b)};
}

FUNK(uint8x16x2_t)
vtrnq_u8(uint8x16_t a, uint8x16_t b) {
  return (uint8x16x2_t){vtrn1q_u8(a, b), vtrn2q_u8(a, b)};
}

FUNK(uint16x8x2_t)
vtrnq_u16(uint16x8_t a, uint16x8_t b) {
  return (uint16x8x2_t){vtrn1q_u16(a, b), vtrn2q_u16(a, b)};
}

FUNK(uint32x4x2_t)
vtrnq_u32(uint32x4_t a, uint32x4_t b) {
  return (uint32x4x2_t){vtrn1q_u32(a, b), vtrn2q_u32(a, b)};
}

FUNK(uint8x8_t)
vtst_s8(int8x8_t __a, int8x8_t __b) {
  return (uint8x8_t)((__a & __b) != 0);
}

FUNK(uint16x4_t)
vtst_s16(int16x4_t __a, int16x4_t __b) {
  return (uint16x4_t)((__a & __b) != 0);
}

FUNK(uint32x2_t)
vtst_s32(int32x2_t __a, int32x2_t __b) {
  return (uint32x2_t)((__a & __b) != 0);
}

FUNK(uint64x1_t)
vtst_s64(int64x1_t __a, int64x1_t __b) {
  return (uint64x1_t)((__a & __b) != __AARCH64_INT64_C(0));
}

FUNK(uint8x8_t)
vtst_u8(uint8x8_t __a, uint8x8_t __b) {
  return ((__a & __b) != 0);
}

FUNK(uint16x4_t)
vtst_u16(uint16x4_t __a, uint16x4_t __b) {
  return ((__a & __b) != 0);
}

FUNK(uint32x2_t)
vtst_u32(uint32x2_t __a, uint32x2_t __b) {
  return ((__a & __b) != 0);
}

FUNK(uint64x1_t)
vtst_u64(uint64x1_t __a, uint64x1_t __b) {
  return ((__a & __b) != __AARCH64_UINT64_C(0));
}

FUNK(uint8x16_t)
vtstq_s8(int8x16_t __a, int8x16_t __b) {
  return (uint8x16_t)((__a & __b) != 0);
}

FUNK(uint16x8_t)
vtstq_s16(int16x8_t __a, int16x8_t __b) {
  return (uint16x8_t)((__a & __b) != 0);
}

FUNK(uint32x4_t)
vtstq_s32(int32x4_t __a, int32x4_t __b) {
  return (uint32x4_t)((__a & __b) != 0);
}

FUNK(uint64x2_t)
vtstq_s64(int64x2_t __a, int64x2_t __b) {
  return (uint64x2_t)((__a & __b) != __AARCH64_INT64_C(0));
}

FUNK(uint8x16_t)
vtstq_u8(uint8x16_t __a, uint8x16_t __b) {
  return ((__a & __b) != 0);
}

FUNK(uint16x8_t)
vtstq_u16(uint16x8_t __a, uint16x8_t __b) {
  return ((__a & __b) != 0);
}

FUNK(uint32x4_t)
vtstq_u32(uint32x4_t __a, uint32x4_t __b) {
  return ((__a & __b) != 0);
}

FUNK(uint64x2_t)
vtstq_u64(uint64x2_t __a, uint64x2_t __b) {
  return ((__a & __b) != __AARCH64_UINT64_C(0));
}

FUNK(uint64_t)
vtstd_s64(int64_t __a, int64_t __b) {
  return (__a & __b) ? -1ll : 0ll;
}

FUNK(uint64_t)
vtstd_u64(uint64_t __a, uint64_t __b) {
  return (__a & __b) ? -1ll : 0ll;
}

FUNK(int8x8_t)
vuqadd_s8(int8x8_t __a, uint8x8_t __b) {
  return __builtin_aarch64_suqaddv8qi_ssu(__a, __b);
}

FUNK(int16x4_t)
vuqadd_s16(int16x4_t __a, uint16x4_t __b) {
  return __builtin_aarch64_suqaddv4hi_ssu(__a, __b);
}

FUNK(int32x2_t)
vuqadd_s32(int32x2_t __a, uint32x2_t __b) {
  return __builtin_aarch64_suqaddv2si_ssu(__a, __b);
}

FUNK(int64x1_t)
vuqadd_s64(int64x1_t __a, uint64x1_t __b) {
  return (int64x1_t){__builtin_aarch64_suqadddi_ssu(__a[0], __b[0])};
}

FUNK(int8x16_t)
vuqaddq_s8(int8x16_t __a, uint8x16_t __b) {
  return __builtin_aarch64_suqaddv16qi_ssu(__a, __b);
}

FUNK(int16x8_t)
vuqaddq_s16(int16x8_t __a, uint16x8_t __b) {
  return __builtin_aarch64_suqaddv8hi_ssu(__a, __b);
}

FUNK(int32x4_t)
vuqaddq_s32(int32x4_t __a, uint32x4_t __b) {
  return __builtin_aarch64_suqaddv4si_ssu(__a, __b);
}

FUNK(int64x2_t)
vuqaddq_s64(int64x2_t __a, uint64x2_t __b) {
  return __builtin_aarch64_suqaddv2di_ssu(__a, __b);
}

FUNK(int8_t)
vuqaddb_s8(int8_t __a, uint8_t __b) {
  return __builtin_aarch64_suqaddqi_ssu(__a, __b);
}

FUNK(int16_t)
vuqaddh_s16(int16_t __a, uint16_t __b) {
  return __builtin_aarch64_suqaddhi_ssu(__a, __b);
}

FUNK(int32_t)
vuqadds_s32(int32_t __a, uint32_t __b) {
  return __builtin_aarch64_suqaddsi_ssu(__a, __b);
}

FUNK(int64_t)
vuqaddd_s64(int64_t __a, uint64_t __b) {
  return __builtin_aarch64_suqadddi_ssu(__a, __b);
}

#define __DEFINTERLEAVE(op, rettype, intype, funcsuffix, Q)   \
  FUNK(rettype) v##op##Q##_##funcsuffix(intype a, intype b) { \
    return (rettype){v##op##1##Q##_##funcsuffix(a, b),        \
                     v##op##2##Q##_##funcsuffix(a, b)};       \
  }

#define __INTERLEAVE_LIST(op)                             \
  __DEFINTERLEAVE(op, float16x4x2_t, float16x4_t, f16, )  \
  __DEFINTERLEAVE(op, float32x2x2_t, float32x2_t, f32, )  \
  __DEFINTERLEAVE(op, poly8x8x2_t, poly8x8_t, p8, )       \
  __DEFINTERLEAVE(op, poly16x4x2_t, poly16x4_t, p16, )    \
  __DEFINTERLEAVE(op, int8x8x2_t, int8x8_t, s8, )         \
  __DEFINTERLEAVE(op, int16x4x2_t, int16x4_t, s16, )      \
  __DEFINTERLEAVE(op, int32x2x2_t, int32x2_t, s32, )      \
  __DEFINTERLEAVE(op, uint8x8x2_t, uint8x8_t, u8, )       \
  __DEFINTERLEAVE(op, uint16x4x2_t, uint16x4_t, u16, )    \
  __DEFINTERLEAVE(op, uint32x2x2_t, uint32x2_t, u32, )    \
  __DEFINTERLEAVE(op, float16x8x2_t, float16x8_t, f16, q) \
  __DEFINTERLEAVE(op, float32x4x2_t, float32x4_t, f32, q) \
  __DEFINTERLEAVE(op, poly8x16x2_t, poly8x16_t, p8, q)    \
  __DEFINTERLEAVE(op, poly16x8x2_t, poly16x8_t, p16, q)   \
  __DEFINTERLEAVE(op, int8x16x2_t, int8x16_t, s8, q)      \
  __DEFINTERLEAVE(op, int16x8x2_t, int16x8_t, s16, q)     \
  __DEFINTERLEAVE(op, int32x4x2_t, int32x4_t, s32, q)     \
  __DEFINTERLEAVE(op, uint8x16x2_t, uint8x16_t, u8, q)    \
  __DEFINTERLEAVE(op, uint16x8x2_t, uint16x8_t, u16, q)   \
  __DEFINTERLEAVE(op, uint32x4x2_t, uint32x4_t, u32, q)

FUNK(float16x4_t)
vuzp1_f16(float16x4_t __a, float16x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x4_t){5, 7, 1, 3});
#else
  return __builtin_shuffle(__a, __b, (uint16x4_t){0, 2, 4, 6});
#endif
}

FUNK(float32x2_t)
vuzp1_f32(float32x2_t __a, float32x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x2_t){3, 1});
#else
  return __builtin_shuffle(__a, __b, (uint32x2_t){0, 2});
#endif
}

FUNK(poly8x8_t)
vuzp1_p8(poly8x8_t __a, poly8x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint8x8_t){9, 11, 13, 15, 1, 3, 5, 7});
#else
  return __builtin_shuffle(__a, __b, (uint8x8_t){0, 2, 4, 6, 8, 10, 12, 14});
#endif
}

FUNK(poly16x4_t)
vuzp1_p16(poly16x4_t __a, poly16x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x4_t){5, 7, 1, 3});
#else
  return __builtin_shuffle(__a, __b, (uint16x4_t){0, 2, 4, 6});
#endif
}

FUNK(int8x8_t)
vuzp1_s8(int8x8_t __a, int8x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint8x8_t){9, 11, 13, 15, 1, 3, 5, 7});
#else
  return __builtin_shuffle(__a, __b, (uint8x8_t){0, 2, 4, 6, 8, 10, 12, 14});
#endif
}

FUNK(int16x4_t)
vuzp1_s16(int16x4_t __a, int16x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x4_t){5, 7, 1, 3});
#else
  return __builtin_shuffle(__a, __b, (uint16x4_t){0, 2, 4, 6});
#endif
}

FUNK(int32x2_t)
vuzp1_s32(int32x2_t __a, int32x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x2_t){3, 1});
#else
  return __builtin_shuffle(__a, __b, (uint32x2_t){0, 2});
#endif
}

FUNK(uint8x8_t)
vuzp1_u8(uint8x8_t __a, uint8x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint8x8_t){9, 11, 13, 15, 1, 3, 5, 7});
#else
  return __builtin_shuffle(__a, __b, (uint8x8_t){0, 2, 4, 6, 8, 10, 12, 14});
#endif
}

FUNK(uint16x4_t)
vuzp1_u16(uint16x4_t __a, uint16x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x4_t){5, 7, 1, 3});
#else
  return __builtin_shuffle(__a, __b, (uint16x4_t){0, 2, 4, 6});
#endif
}

FUNK(uint32x2_t)
vuzp1_u32(uint32x2_t __a, uint32x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x2_t){3, 1});
#else
  return __builtin_shuffle(__a, __b, (uint32x2_t){0, 2});
#endif
}

FUNK(float16x8_t)
vuzp1q_f16(float16x8_t __a, float16x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x8_t){9, 11, 13, 15, 1, 3, 5, 7});
#else
  return __builtin_shuffle(__a, __b, (uint16x8_t){0, 2, 4, 6, 8, 10, 12, 14});
#endif
}

FUNK(float32x4_t)
vuzp1q_f32(float32x4_t __a, float32x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x4_t){5, 7, 1, 3});
#else
  return __builtin_shuffle(__a, __b, (uint32x4_t){0, 2, 4, 6});
#endif
}

FUNK(float64x2_t)
vuzp1q_f64(float64x2_t __a, float64x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint64x2_t){3, 1});
#else
  return __builtin_shuffle(__a, __b, (uint64x2_t){0, 2});
#endif
}

FUNK(poly8x16_t)
vuzp1q_p8(poly8x16_t __a, poly8x16_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(
      __a, __b,
      (uint8x16_t){17, 19, 21, 23, 25, 27, 29, 31, 1, 3, 5, 7, 9, 11, 13, 15});
#else
  return __builtin_shuffle(
      __a, __b,
      (uint8x16_t){0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30});
#endif
}

FUNK(poly16x8_t)
vuzp1q_p16(poly16x8_t __a, poly16x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x8_t){9, 11, 13, 15, 1, 3, 5, 7});
#else
  return __builtin_shuffle(__a, __b, (uint16x8_t){0, 2, 4, 6, 8, 10, 12, 14});
#endif
}

FUNK(int8x16_t)
vuzp1q_s8(int8x16_t __a, int8x16_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(
      __a, __b,
      (uint8x16_t){17, 19, 21, 23, 25, 27, 29, 31, 1, 3, 5, 7, 9, 11, 13, 15});
#else
  return __builtin_shuffle(
      __a, __b,
      (uint8x16_t){0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30});
#endif
}

FUNK(int16x8_t)
vuzp1q_s16(int16x8_t __a, int16x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x8_t){9, 11, 13, 15, 1, 3, 5, 7});
#else
  return __builtin_shuffle(__a, __b, (uint16x8_t){0, 2, 4, 6, 8, 10, 12, 14});
#endif
}

FUNK(int32x4_t)
vuzp1q_s32(int32x4_t __a, int32x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x4_t){5, 7, 1, 3});
#else
  return __builtin_shuffle(__a, __b, (uint32x4_t){0, 2, 4, 6});
#endif
}

FUNK(int64x2_t)
vuzp1q_s64(int64x2_t __a, int64x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint64x2_t){3, 1});
#else
  return __builtin_shuffle(__a, __b, (uint64x2_t){0, 2});
#endif
}

FUNK(uint8x16_t)
vuzp1q_u8(uint8x16_t __a, uint8x16_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(
      __a, __b,
      (uint8x16_t){17, 19, 21, 23, 25, 27, 29, 31, 1, 3, 5, 7, 9, 11, 13, 15});
#else
  return __builtin_shuffle(
      __a, __b,
      (uint8x16_t){0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30});
#endif
}

FUNK(uint16x8_t)
vuzp1q_u16(uint16x8_t __a, uint16x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x8_t){9, 11, 13, 15, 1, 3, 5, 7});
#else
  return __builtin_shuffle(__a, __b, (uint16x8_t){0, 2, 4, 6, 8, 10, 12, 14});
#endif
}

FUNK(uint32x4_t)
vuzp1q_u32(uint32x4_t __a, uint32x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x4_t){5, 7, 1, 3});
#else
  return __builtin_shuffle(__a, __b, (uint32x4_t){0, 2, 4, 6});
#endif
}

FUNK(uint64x2_t)
vuzp1q_u64(uint64x2_t __a, uint64x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint64x2_t){3, 1});
#else
  return __builtin_shuffle(__a, __b, (uint64x2_t){0, 2});
#endif
}

FUNK(float16x4_t)
vuzp2_f16(float16x4_t __a, float16x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x4_t){4, 6, 0, 2});
#else
  return __builtin_shuffle(__a, __b, (uint16x4_t){1, 3, 5, 7});
#endif
}

FUNK(float32x2_t)
vuzp2_f32(float32x2_t __a, float32x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x2_t){2, 0});
#else
  return __builtin_shuffle(__a, __b, (uint32x2_t){1, 3});
#endif
}

FUNK(poly8x8_t)
vuzp2_p8(poly8x8_t __a, poly8x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint8x8_t){8, 10, 12, 14, 0, 2, 4, 6});
#else
  return __builtin_shuffle(__a, __b, (uint8x8_t){1, 3, 5, 7, 9, 11, 13, 15});
#endif
}

FUNK(poly16x4_t)
vuzp2_p16(poly16x4_t __a, poly16x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x4_t){4, 6, 0, 2});
#else
  return __builtin_shuffle(__a, __b, (uint16x4_t){1, 3, 5, 7});
#endif
}

FUNK(int8x8_t)
vuzp2_s8(int8x8_t __a, int8x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint8x8_t){8, 10, 12, 14, 0, 2, 4, 6});
#else
  return __builtin_shuffle(__a, __b, (uint8x8_t){1, 3, 5, 7, 9, 11, 13, 15});
#endif
}

FUNK(int16x4_t)
vuzp2_s16(int16x4_t __a, int16x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x4_t){4, 6, 0, 2});
#else
  return __builtin_shuffle(__a, __b, (uint16x4_t){1, 3, 5, 7});
#endif
}

FUNK(int32x2_t)
vuzp2_s32(int32x2_t __a, int32x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x2_t){2, 0});
#else
  return __builtin_shuffle(__a, __b, (uint32x2_t){1, 3});
#endif
}

FUNK(uint8x8_t)
vuzp2_u8(uint8x8_t __a, uint8x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint8x8_t){8, 10, 12, 14, 0, 2, 4, 6});
#else
  return __builtin_shuffle(__a, __b, (uint8x8_t){1, 3, 5, 7, 9, 11, 13, 15});
#endif
}

FUNK(uint16x4_t)
vuzp2_u16(uint16x4_t __a, uint16x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x4_t){4, 6, 0, 2});
#else
  return __builtin_shuffle(__a, __b, (uint16x4_t){1, 3, 5, 7});
#endif
}

FUNK(uint32x2_t)
vuzp2_u32(uint32x2_t __a, uint32x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x2_t){2, 0});
#else
  return __builtin_shuffle(__a, __b, (uint32x2_t){1, 3});
#endif
}

FUNK(float16x8_t)
vuzp2q_f16(float16x8_t __a, float16x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x8_t){8, 10, 12, 14, 0, 2, 4, 6});
#else
  return __builtin_shuffle(__a, __b, (uint16x8_t){1, 3, 5, 7, 9, 11, 13, 15});
#endif
}

FUNK(float32x4_t)
vuzp2q_f32(float32x4_t __a, float32x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x4_t){4, 6, 0, 2});
#else
  return __builtin_shuffle(__a, __b, (uint32x4_t){1, 3, 5, 7});
#endif
}

FUNK(float64x2_t)
vuzp2q_f64(float64x2_t __a, float64x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint64x2_t){2, 0});
#else
  return __builtin_shuffle(__a, __b, (uint64x2_t){1, 3});
#endif
}

FUNK(poly8x16_t)
vuzp2q_p8(poly8x16_t __a, poly8x16_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(
      __a, __b,
      (uint8x16_t){16, 18, 20, 22, 24, 26, 28, 30, 0, 2, 4, 6, 8, 10, 12, 14});
#else
  return __builtin_shuffle(
      __a, __b,
      (uint8x16_t){1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31});
#endif
}

FUNK(poly16x8_t)
vuzp2q_p16(poly16x8_t __a, poly16x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x8_t){8, 10, 12, 14, 0, 2, 4, 6});
#else
  return __builtin_shuffle(__a, __b, (uint16x8_t){1, 3, 5, 7, 9, 11, 13, 15});
#endif
}

FUNK(int8x16_t)
vuzp2q_s8(int8x16_t __a, int8x16_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(
      __a, __b,
      (uint8x16_t){16, 18, 20, 22, 24, 26, 28, 30, 0, 2, 4, 6, 8, 10, 12, 14});
#else
  return __builtin_shuffle(
      __a, __b,
      (uint8x16_t){1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31});
#endif
}

FUNK(int16x8_t)
vuzp2q_s16(int16x8_t __a, int16x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x8_t){8, 10, 12, 14, 0, 2, 4, 6});
#else
  return __builtin_shuffle(__a, __b, (uint16x8_t){1, 3, 5, 7, 9, 11, 13, 15});
#endif
}

FUNK(int32x4_t)
vuzp2q_s32(int32x4_t __a, int32x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x4_t){4, 6, 0, 2});
#else
  return __builtin_shuffle(__a, __b, (uint32x4_t){1, 3, 5, 7});
#endif
}

FUNK(int64x2_t)
vuzp2q_s64(int64x2_t __a, int64x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint64x2_t){2, 0});
#else
  return __builtin_shuffle(__a, __b, (uint64x2_t){1, 3});
#endif
}

FUNK(uint8x16_t)
vuzp2q_u8(uint8x16_t __a, uint8x16_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(
      __a, __b,
      (uint8x16_t){16, 18, 20, 22, 24, 26, 28, 30, 0, 2, 4, 6, 8, 10, 12, 14});
#else
  return __builtin_shuffle(
      __a, __b,
      (uint8x16_t){1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31});
#endif
}

FUNK(uint16x8_t)
vuzp2q_u16(uint16x8_t __a, uint16x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x8_t){8, 10, 12, 14, 0, 2, 4, 6});
#else
  return __builtin_shuffle(__a, __b, (uint16x8_t){1, 3, 5, 7, 9, 11, 13, 15});
#endif
}

FUNK(uint32x4_t)
vuzp2q_u32(uint32x4_t __a, uint32x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x4_t){4, 6, 0, 2});
#else
  return __builtin_shuffle(__a, __b, (uint32x4_t){1, 3, 5, 7});
#endif
}

FUNK(uint64x2_t)
vuzp2q_u64(uint64x2_t __a, uint64x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint64x2_t){2, 0});
#else
  return __builtin_shuffle(__a, __b, (uint64x2_t){1, 3});
#endif
}

__INTERLEAVE_LIST(uzp)

FUNK(float16x4_t)
vzip1_f16(float16x4_t __a, float16x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x4_t){6, 2, 7, 3});
#else
  return __builtin_shuffle(__a, __b, (uint16x4_t){0, 4, 1, 5});
#endif
}

FUNK(float32x2_t)
vzip1_f32(float32x2_t __a, float32x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x2_t){3, 1});
#else
  return __builtin_shuffle(__a, __b, (uint32x2_t){0, 2});
#endif
}

FUNK(poly8x8_t)
vzip1_p8(poly8x8_t __a, poly8x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint8x8_t){12, 4, 13, 5, 14, 6, 15, 7});
#else
  return __builtin_shuffle(__a, __b, (uint8x8_t){0, 8, 1, 9, 2, 10, 3, 11});
#endif
}

FUNK(poly16x4_t)
vzip1_p16(poly16x4_t __a, poly16x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x4_t){6, 2, 7, 3});
#else
  return __builtin_shuffle(__a, __b, (uint16x4_t){0, 4, 1, 5});
#endif
}

FUNK(int8x8_t)
vzip1_s8(int8x8_t __a, int8x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint8x8_t){12, 4, 13, 5, 14, 6, 15, 7});
#else
  return __builtin_shuffle(__a, __b, (uint8x8_t){0, 8, 1, 9, 2, 10, 3, 11});
#endif
}

FUNK(int16x4_t)
vzip1_s16(int16x4_t __a, int16x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x4_t){6, 2, 7, 3});
#else
  return __builtin_shuffle(__a, __b, (uint16x4_t){0, 4, 1, 5});
#endif
}

FUNK(int32x2_t)
vzip1_s32(int32x2_t __a, int32x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x2_t){3, 1});
#else
  return __builtin_shuffle(__a, __b, (uint32x2_t){0, 2});
#endif
}

FUNK(uint8x8_t)
vzip1_u8(uint8x8_t __a, uint8x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint8x8_t){12, 4, 13, 5, 14, 6, 15, 7});
#else
  return __builtin_shuffle(__a, __b, (uint8x8_t){0, 8, 1, 9, 2, 10, 3, 11});
#endif
}

FUNK(uint16x4_t)
vzip1_u16(uint16x4_t __a, uint16x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x4_t){6, 2, 7, 3});
#else
  return __builtin_shuffle(__a, __b, (uint16x4_t){0, 4, 1, 5});
#endif
}

FUNK(uint32x2_t)
vzip1_u32(uint32x2_t __a, uint32x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x2_t){3, 1});
#else
  return __builtin_shuffle(__a, __b, (uint32x2_t){0, 2});
#endif
}

FUNK(float16x8_t)
vzip1q_f16(float16x8_t __a, float16x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x8_t){12, 4, 13, 5, 14, 6, 15, 7});
#else
  return __builtin_shuffle(__a, __b, (uint16x8_t){0, 8, 1, 9, 2, 10, 3, 11});
#endif
}

FUNK(float32x4_t)
vzip1q_f32(float32x4_t __a, float32x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x4_t){6, 2, 7, 3});
#else
  return __builtin_shuffle(__a, __b, (uint32x4_t){0, 4, 1, 5});
#endif
}

FUNK(float64x2_t)
vzip1q_f64(float64x2_t __a, float64x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint64x2_t){3, 1});
#else
  return __builtin_shuffle(__a, __b, (uint64x2_t){0, 2});
#endif
}

FUNK(poly8x16_t)
vzip1q_p8(poly8x16_t __a, poly8x16_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b,
                           (uint8x16_t){24, 8, 25, 9, 26, 10, 27, 11, 28, 12,
                                        29, 13, 30, 14, 31, 15});
#else
  return __builtin_shuffle(
      __a, __b,
      (uint8x16_t){0, 16, 1, 17, 2, 18, 3, 19, 4, 20, 5, 21, 6, 22, 7, 23});
#endif
}

FUNK(poly16x8_t)
vzip1q_p16(poly16x8_t __a, poly16x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x8_t){12, 4, 13, 5, 14, 6, 15, 7});
#else
  return __builtin_shuffle(__a, __b, (uint16x8_t){0, 8, 1, 9, 2, 10, 3, 11});
#endif
}

FUNK(int8x16_t)
vzip1q_s8(int8x16_t __a, int8x16_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b,
                           (uint8x16_t){24, 8, 25, 9, 26, 10, 27, 11, 28, 12,
                                        29, 13, 30, 14, 31, 15});
#else
  return __builtin_shuffle(
      __a, __b,
      (uint8x16_t){0, 16, 1, 17, 2, 18, 3, 19, 4, 20, 5, 21, 6, 22, 7, 23});
#endif
}

FUNK(int16x8_t)
vzip1q_s16(int16x8_t __a, int16x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x8_t){12, 4, 13, 5, 14, 6, 15, 7});
#else
  return __builtin_shuffle(__a, __b, (uint16x8_t){0, 8, 1, 9, 2, 10, 3, 11});
#endif
}

FUNK(int32x4_t)
vzip1q_s32(int32x4_t __a, int32x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x4_t){6, 2, 7, 3});
#else
  return __builtin_shuffle(__a, __b, (uint32x4_t){0, 4, 1, 5});
#endif
}

FUNK(int64x2_t)
vzip1q_s64(int64x2_t __a, int64x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint64x2_t){3, 1});
#else
  return __builtin_shuffle(__a, __b, (uint64x2_t){0, 2});
#endif
}

FUNK(uint8x16_t)
vzip1q_u8(uint8x16_t __a, uint8x16_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b,
                           (uint8x16_t){24, 8, 25, 9, 26, 10, 27, 11, 28, 12,
                                        29, 13, 30, 14, 31, 15});
#else
  return __builtin_shuffle(
      __a, __b,
      (uint8x16_t){0, 16, 1, 17, 2, 18, 3, 19, 4, 20, 5, 21, 6, 22, 7, 23});
#endif
}

FUNK(uint16x8_t)
vzip1q_u16(uint16x8_t __a, uint16x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x8_t){12, 4, 13, 5, 14, 6, 15, 7});
#else
  return __builtin_shuffle(__a, __b, (uint16x8_t){0, 8, 1, 9, 2, 10, 3, 11});
#endif
}

FUNK(uint32x4_t)
vzip1q_u32(uint32x4_t __a, uint32x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x4_t){6, 2, 7, 3});
#else
  return __builtin_shuffle(__a, __b, (uint32x4_t){0, 4, 1, 5});
#endif
}

FUNK(uint64x2_t)
vzip1q_u64(uint64x2_t __a, uint64x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint64x2_t){3, 1});
#else
  return __builtin_shuffle(__a, __b, (uint64x2_t){0, 2});
#endif
}

FUNK(float16x4_t)
vzip2_f16(float16x4_t __a, float16x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x4_t){4, 0, 5, 1});
#else
  return __builtin_shuffle(__a, __b, (uint16x4_t){2, 6, 3, 7});
#endif
}

FUNK(float32x2_t)
vzip2_f32(float32x2_t __a, float32x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x2_t){2, 0});
#else
  return __builtin_shuffle(__a, __b, (uint32x2_t){1, 3});
#endif
}

FUNK(poly8x8_t)
vzip2_p8(poly8x8_t __a, poly8x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint8x8_t){8, 0, 9, 1, 10, 2, 11, 3});
#else
  return __builtin_shuffle(__a, __b, (uint8x8_t){4, 12, 5, 13, 6, 14, 7, 15});
#endif
}

FUNK(poly16x4_t)
vzip2_p16(poly16x4_t __a, poly16x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x4_t){4, 0, 5, 1});
#else
  return __builtin_shuffle(__a, __b, (uint16x4_t){2, 6, 3, 7});
#endif
}

FUNK(int8x8_t)
vzip2_s8(int8x8_t __a, int8x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint8x8_t){8, 0, 9, 1, 10, 2, 11, 3});
#else
  return __builtin_shuffle(__a, __b, (uint8x8_t){4, 12, 5, 13, 6, 14, 7, 15});
#endif
}

FUNK(int16x4_t)
vzip2_s16(int16x4_t __a, int16x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x4_t){4, 0, 5, 1});
#else
  return __builtin_shuffle(__a, __b, (uint16x4_t){2, 6, 3, 7});
#endif
}

FUNK(int32x2_t)
vzip2_s32(int32x2_t __a, int32x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x2_t){2, 0});
#else
  return __builtin_shuffle(__a, __b, (uint32x2_t){1, 3});
#endif
}

FUNK(uint8x8_t)
vzip2_u8(uint8x8_t __a, uint8x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint8x8_t){8, 0, 9, 1, 10, 2, 11, 3});
#else
  return __builtin_shuffle(__a, __b, (uint8x8_t){4, 12, 5, 13, 6, 14, 7, 15});
#endif
}

FUNK(uint16x4_t)
vzip2_u16(uint16x4_t __a, uint16x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x4_t){4, 0, 5, 1});
#else
  return __builtin_shuffle(__a, __b, (uint16x4_t){2, 6, 3, 7});
#endif
}

FUNK(uint32x2_t)
vzip2_u32(uint32x2_t __a, uint32x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x2_t){2, 0});
#else
  return __builtin_shuffle(__a, __b, (uint32x2_t){1, 3});
#endif
}

FUNK(float16x8_t)
vzip2q_f16(float16x8_t __a, float16x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x8_t){8, 0, 9, 1, 10, 2, 11, 3});
#else
  return __builtin_shuffle(__a, __b, (uint16x8_t){4, 12, 5, 13, 6, 14, 7, 15});
#endif
}

FUNK(float32x4_t)
vzip2q_f32(float32x4_t __a, float32x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x4_t){4, 0, 5, 1});
#else
  return __builtin_shuffle(__a, __b, (uint32x4_t){2, 6, 3, 7});
#endif
}

FUNK(float64x2_t)
vzip2q_f64(float64x2_t __a, float64x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint64x2_t){2, 0});
#else
  return __builtin_shuffle(__a, __b, (uint64x2_t){1, 3});
#endif
}

FUNK(poly8x16_t)
vzip2q_p8(poly8x16_t __a, poly8x16_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(
      __a, __b,
      (uint8x16_t){16, 0, 17, 1, 18, 2, 19, 3, 20, 4, 21, 5, 22, 6, 23, 7});
#else
  return __builtin_shuffle(__a, __b,
                           (uint8x16_t){8, 24, 9, 25, 10, 26, 11, 27, 12, 28,
                                        13, 29, 14, 30, 15, 31});
#endif
}

FUNK(poly16x8_t)
vzip2q_p16(poly16x8_t __a, poly16x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x8_t){8, 0, 9, 1, 10, 2, 11, 3});
#else
  return __builtin_shuffle(__a, __b, (uint16x8_t){4, 12, 5, 13, 6, 14, 7, 15});
#endif
}

FUNK(int8x16_t)
vzip2q_s8(int8x16_t __a, int8x16_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(
      __a, __b,
      (uint8x16_t){16, 0, 17, 1, 18, 2, 19, 3, 20, 4, 21, 5, 22, 6, 23, 7});
#else
  return __builtin_shuffle(__a, __b,
                           (uint8x16_t){8, 24, 9, 25, 10, 26, 11, 27, 12, 28,
                                        13, 29, 14, 30, 15, 31});
#endif
}

FUNK(int16x8_t)
vzip2q_s16(int16x8_t __a, int16x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x8_t){8, 0, 9, 1, 10, 2, 11, 3});
#else
  return __builtin_shuffle(__a, __b, (uint16x8_t){4, 12, 5, 13, 6, 14, 7, 15});
#endif
}

FUNK(int32x4_t)
vzip2q_s32(int32x4_t __a, int32x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x4_t){4, 0, 5, 1});
#else
  return __builtin_shuffle(__a, __b, (uint32x4_t){2, 6, 3, 7});
#endif
}

FUNK(int64x2_t)
vzip2q_s64(int64x2_t __a, int64x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint64x2_t){2, 0});
#else
  return __builtin_shuffle(__a, __b, (uint64x2_t){1, 3});
#endif
}

FUNK(uint8x16_t)
vzip2q_u8(uint8x16_t __a, uint8x16_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(
      __a, __b,
      (uint8x16_t){16, 0, 17, 1, 18, 2, 19, 3, 20, 4, 21, 5, 22, 6, 23, 7});
#else
  return __builtin_shuffle(__a, __b,
                           (uint8x16_t){8, 24, 9, 25, 10, 26, 11, 27, 12, 28,
                                        13, 29, 14, 30, 15, 31});
#endif
}

FUNK(uint16x8_t)
vzip2q_u16(uint16x8_t __a, uint16x8_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint16x8_t){8, 0, 9, 1, 10, 2, 11, 3});
#else
  return __builtin_shuffle(__a, __b, (uint16x8_t){4, 12, 5, 13, 6, 14, 7, 15});
#endif
}

FUNK(uint32x4_t)
vzip2q_u32(uint32x4_t __a, uint32x4_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint32x4_t){4, 0, 5, 1});
#else
  return __builtin_shuffle(__a, __b, (uint32x4_t){2, 6, 3, 7});
#endif
}

FUNK(uint64x2_t)
vzip2q_u64(uint64x2_t __a, uint64x2_t __b) {
#ifdef __AARCH64EB__
  return __builtin_shuffle(__a, __b, (uint64x2_t){2, 0});
#else
  return __builtin_shuffle(__a, __b, (uint64x2_t){1, 3});
#endif
}

__INTERLEAVE_LIST(zip)

#undef __INTERLEAVE_LIST
#undef __DEFINTERLEAVE

#pragma GCC pop_options

#include "third_party/aarch64/arm_fp16.h"

#pragma GCC push_options
#pragma GCC target("arch=armv8.2-a+fp16")

FUNK(float16x4_t)
vabs_f16(float16x4_t __a) {
  return __builtin_aarch64_absv4hf(__a);
}

FUNK(float16x8_t)
vabsq_f16(float16x8_t __a) {
  return __builtin_aarch64_absv8hf(__a);
}

FUNK(uint16x4_t)
vceqz_f16(float16x4_t __a) {
  return __builtin_aarch64_cmeqv4hf_uss(__a, vdup_n_f16(0.0f));
}

FUNK(uint16x8_t)
vceqzq_f16(float16x8_t __a) {
  return __builtin_aarch64_cmeqv8hf_uss(__a, vdupq_n_f16(0.0f));
}

FUNK(uint16x4_t)
vcgez_f16(float16x4_t __a) {
  return __builtin_aarch64_cmgev4hf_uss(__a, vdup_n_f16(0.0f));
}

FUNK(uint16x8_t)
vcgezq_f16(float16x8_t __a) {
  return __builtin_aarch64_cmgev8hf_uss(__a, vdupq_n_f16(0.0f));
}

FUNK(uint16x4_t)
vcgtz_f16(float16x4_t __a) {
  return __builtin_aarch64_cmgtv4hf_uss(__a, vdup_n_f16(0.0f));
}

FUNK(uint16x8_t)
vcgtzq_f16(float16x8_t __a) {
  return __builtin_aarch64_cmgtv8hf_uss(__a, vdupq_n_f16(0.0f));
}

FUNK(uint16x4_t)
vclez_f16(float16x4_t __a) {
  return __builtin_aarch64_cmlev4hf_uss(__a, vdup_n_f16(0.0f));
}

FUNK(uint16x8_t)
vclezq_f16(float16x8_t __a) {
  return __builtin_aarch64_cmlev8hf_uss(__a, vdupq_n_f16(0.0f));
}

FUNK(uint16x4_t)
vcltz_f16(float16x4_t __a) {
  return __builtin_aarch64_cmltv4hf_uss(__a, vdup_n_f16(0.0f));
}

FUNK(uint16x8_t)
vcltzq_f16(float16x8_t __a) {
  return __builtin_aarch64_cmltv8hf_uss(__a, vdupq_n_f16(0.0f));
}

FUNK(float16x4_t)
vcvt_f16_s16(int16x4_t __a) {
  return __builtin_aarch64_floatv4hiv4hf(__a);
}

FUNK(float16x8_t)
vcvtq_f16_s16(int16x8_t __a) {
  return __builtin_aarch64_floatv8hiv8hf(__a);
}

FUNK(float16x4_t)
vcvt_f16_u16(uint16x4_t __a) {
  return __builtin_aarch64_floatunsv4hiv4hf((int16x4_t)__a);
}

FUNK(float16x8_t)
vcvtq_f16_u16(uint16x8_t __a) {
  return __builtin_aarch64_floatunsv8hiv8hf((int16x8_t)__a);
}

FUNK(int16x4_t)
vcvt_s16_f16(float16x4_t __a) {
  return __builtin_aarch64_lbtruncv4hfv4hi(__a);
}

FUNK(int16x8_t)
vcvtq_s16_f16(float16x8_t __a) {
  return __builtin_aarch64_lbtruncv8hfv8hi(__a);
}

FUNK(uint16x4_t)
vcvt_u16_f16(float16x4_t __a) {
  return __builtin_aarch64_lbtruncuv4hfv4hi_us(__a);
}

FUNK(uint16x8_t)
vcvtq_u16_f16(float16x8_t __a) {
  return __builtin_aarch64_lbtruncuv8hfv8hi_us(__a);
}

FUNK(int16x4_t)
vcvta_s16_f16(float16x4_t __a) {
  return __builtin_aarch64_lroundv4hfv4hi(__a);
}

FUNK(int16x8_t)
vcvtaq_s16_f16(float16x8_t __a) {
  return __builtin_aarch64_lroundv8hfv8hi(__a);
}

FUNK(uint16x4_t)
vcvta_u16_f16(float16x4_t __a) {
  return __builtin_aarch64_lrounduv4hfv4hi_us(__a);
}

FUNK(uint16x8_t)
vcvtaq_u16_f16(float16x8_t __a) {
  return __builtin_aarch64_lrounduv8hfv8hi_us(__a);
}

FUNK(int16x4_t)
vcvtm_s16_f16(float16x4_t __a) {
  return __builtin_aarch64_lfloorv4hfv4hi(__a);
}

FUNK(int16x8_t)
vcvtmq_s16_f16(float16x8_t __a) {
  return __builtin_aarch64_lfloorv8hfv8hi(__a);
}

FUNK(uint16x4_t)
vcvtm_u16_f16(float16x4_t __a) {
  return __builtin_aarch64_lflooruv4hfv4hi_us(__a);
}

FUNK(uint16x8_t)
vcvtmq_u16_f16(float16x8_t __a) {
  return __builtin_aarch64_lflooruv8hfv8hi_us(__a);
}

FUNK(int16x4_t)
vcvtn_s16_f16(float16x4_t __a) {
  return __builtin_aarch64_lfrintnv4hfv4hi(__a);
}

FUNK(int16x8_t)
vcvtnq_s16_f16(float16x8_t __a) {
  return __builtin_aarch64_lfrintnv8hfv8hi(__a);
}

FUNK(uint16x4_t)
vcvtn_u16_f16(float16x4_t __a) {
  return __builtin_aarch64_lfrintnuv4hfv4hi_us(__a);
}

FUNK(uint16x8_t)
vcvtnq_u16_f16(float16x8_t __a) {
  return __builtin_aarch64_lfrintnuv8hfv8hi_us(__a);
}

FUNK(int16x4_t)
vcvtp_s16_f16(float16x4_t __a) {
  return __builtin_aarch64_lceilv4hfv4hi(__a);
}

FUNK(int16x8_t)
vcvtpq_s16_f16(float16x8_t __a) {
  return __builtin_aarch64_lceilv8hfv8hi(__a);
}

FUNK(uint16x4_t)
vcvtp_u16_f16(float16x4_t __a) {
  return __builtin_aarch64_lceiluv4hfv4hi_us(__a);
}

FUNK(uint16x8_t)
vcvtpq_u16_f16(float16x8_t __a) {
  return __builtin_aarch64_lceiluv8hfv8hi_us(__a);
}

FUNK(float16x4_t)
vneg_f16(float16x4_t __a) {
  return -__a;
}

FUNK(float16x8_t)
vnegq_f16(float16x8_t __a) {
  return -__a;
}

FUNK(float16x4_t)
vrecpe_f16(float16x4_t __a) {
  return __builtin_aarch64_frecpev4hf(__a);
}

FUNK(float16x8_t)
vrecpeq_f16(float16x8_t __a) {
  return __builtin_aarch64_frecpev8hf(__a);
}

FUNK(float16x4_t)
vrnd_f16(float16x4_t __a) {
  return __builtin_aarch64_btruncv4hf(__a);
}

FUNK(float16x8_t)
vrndq_f16(float16x8_t __a) {
  return __builtin_aarch64_btruncv8hf(__a);
}

FUNK(float16x4_t)
vrnda_f16(float16x4_t __a) {
  return __builtin_aarch64_roundv4hf(__a);
}

FUNK(float16x8_t)
vrndaq_f16(float16x8_t __a) {
  return __builtin_aarch64_roundv8hf(__a);
}

FUNK(float16x4_t)
vrndi_f16(float16x4_t __a) {
  return __builtin_aarch64_nearbyintv4hf(__a);
}

FUNK(float16x8_t)
vrndiq_f16(float16x8_t __a) {
  return __builtin_aarch64_nearbyintv8hf(__a);
}

FUNK(float16x4_t)
vrndm_f16(float16x4_t __a) {
  return __builtin_aarch64_floorv4hf(__a);
}

FUNK(float16x8_t)
vrndmq_f16(float16x8_t __a) {
  return __builtin_aarch64_floorv8hf(__a);
}

FUNK(float16x4_t)
vrndn_f16(float16x4_t __a) {
  return __builtin_aarch64_frintnv4hf(__a);
}

FUNK(float16x8_t)
vrndnq_f16(float16x8_t __a) {
  return __builtin_aarch64_frintnv8hf(__a);
}

FUNK(float16x4_t)
vrndp_f16(float16x4_t __a) {
  return __builtin_aarch64_ceilv4hf(__a);
}

FUNK(float16x8_t)
vrndpq_f16(float16x8_t __a) {
  return __builtin_aarch64_ceilv8hf(__a);
}

FUNK(float16x4_t)
vrndx_f16(float16x4_t __a) {
  return __builtin_aarch64_rintv4hf(__a);
}

FUNK(float16x8_t)
vrndxq_f16(float16x8_t __a) {
  return __builtin_aarch64_rintv8hf(__a);
}

FUNK(float16x4_t)
vrsqrte_f16(float16x4_t a) {
  return __builtin_aarch64_rsqrtev4hf(a);
}

FUNK(float16x8_t)
vrsqrteq_f16(float16x8_t a) {
  return __builtin_aarch64_rsqrtev8hf(a);
}

FUNK(float16x4_t)
vsqrt_f16(float16x4_t a) {
  return __builtin_aarch64_sqrtv4hf(a);
}

FUNK(float16x8_t)
vsqrtq_f16(float16x8_t a) {
  return __builtin_aarch64_sqrtv8hf(a);
}

FUNK(float16x4_t)
vadd_f16(float16x4_t __a, float16x4_t __b) {
  return __a + __b;
}

FUNK(float16x8_t)
vaddq_f16(float16x8_t __a, float16x8_t __b) {
  return __a + __b;
}

FUNK(float16x4_t)
vabd_f16(float16x4_t a, float16x4_t b) {
  return __builtin_aarch64_fabdv4hf(a, b);
}

FUNK(float16x8_t)
vabdq_f16(float16x8_t a, float16x8_t b) {
  return __builtin_aarch64_fabdv8hf(a, b);
}

FUNK(uint16x4_t)
vcage_f16(float16x4_t __a, float16x4_t __b) {
  return __builtin_aarch64_facgev4hf_uss(__a, __b);
}

FUNK(uint16x8_t)
vcageq_f16(float16x8_t __a, float16x8_t __b) {
  return __builtin_aarch64_facgev8hf_uss(__a, __b);
}

FUNK(uint16x4_t)
vcagt_f16(float16x4_t __a, float16x4_t __b) {
  return __builtin_aarch64_facgtv4hf_uss(__a, __b);
}

FUNK(uint16x8_t)
vcagtq_f16(float16x8_t __a, float16x8_t __b) {
  return __builtin_aarch64_facgtv8hf_uss(__a, __b);
}

FUNK(uint16x4_t)
vcale_f16(float16x4_t __a, float16x4_t __b) {
  return __builtin_aarch64_faclev4hf_uss(__a, __b);
}

FUNK(uint16x8_t)
vcaleq_f16(float16x8_t __a, float16x8_t __b) {
  return __builtin_aarch64_faclev8hf_uss(__a, __b);
}

FUNK(uint16x4_t)
vcalt_f16(float16x4_t __a, float16x4_t __b) {
  return __builtin_aarch64_facltv4hf_uss(__a, __b);
}

FUNK(uint16x8_t)
vcaltq_f16(float16x8_t __a, float16x8_t __b) {
  return __builtin_aarch64_facltv8hf_uss(__a, __b);
}

FUNK(uint16x4_t)
vceq_f16(float16x4_t __a, float16x4_t __b) {
  return __builtin_aarch64_cmeqv4hf_uss(__a, __b);
}

FUNK(uint16x8_t)
vceqq_f16(float16x8_t __a, float16x8_t __b) {
  return __builtin_aarch64_cmeqv8hf_uss(__a, __b);
}

FUNK(uint16x4_t)
vcge_f16(float16x4_t __a, float16x4_t __b) {
  return __builtin_aarch64_cmgev4hf_uss(__a, __b);
}

FUNK(uint16x8_t)
vcgeq_f16(float16x8_t __a, float16x8_t __b) {
  return __builtin_aarch64_cmgev8hf_uss(__a, __b);
}

FUNK(uint16x4_t)
vcgt_f16(float16x4_t __a, float16x4_t __b) {
  return __builtin_aarch64_cmgtv4hf_uss(__a, __b);
}

FUNK(uint16x8_t)
vcgtq_f16(float16x8_t __a, float16x8_t __b) {
  return __builtin_aarch64_cmgtv8hf_uss(__a, __b);
}

FUNK(uint16x4_t)
vcle_f16(float16x4_t __a, float16x4_t __b) {
  return __builtin_aarch64_cmlev4hf_uss(__a, __b);
}

FUNK(uint16x8_t)
vcleq_f16(float16x8_t __a, float16x8_t __b) {
  return __builtin_aarch64_cmlev8hf_uss(__a, __b);
}

FUNK(uint16x4_t)
vclt_f16(float16x4_t __a, float16x4_t __b) {
  return __builtin_aarch64_cmltv4hf_uss(__a, __b);
}

FUNK(uint16x8_t)
vcltq_f16(float16x8_t __a, float16x8_t __b) {
  return __builtin_aarch64_cmltv8hf_uss(__a, __b);
}

FUNK(float16x4_t)
vcvt_n_f16_s16(int16x4_t __a, const int __b) {
  return __builtin_aarch64_scvtfv4hi(__a, __b);
}

FUNK(float16x8_t)
vcvtq_n_f16_s16(int16x8_t __a, const int __b) {
  return __builtin_aarch64_scvtfv8hi(__a, __b);
}

FUNK(float16x4_t)
vcvt_n_f16_u16(uint16x4_t __a, const int __b) {
  return __builtin_aarch64_ucvtfv4hi_sus(__a, __b);
}

FUNK(float16x8_t)
vcvtq_n_f16_u16(uint16x8_t __a, const int __b) {
  return __builtin_aarch64_ucvtfv8hi_sus(__a, __b);
}

FUNK(int16x4_t)
vcvt_n_s16_f16(float16x4_t __a, const int __b) {
  return __builtin_aarch64_fcvtzsv4hf(__a, __b);
}

FUNK(int16x8_t)
vcvtq_n_s16_f16(float16x8_t __a, const int __b) {
  return __builtin_aarch64_fcvtzsv8hf(__a, __b);
}

FUNK(uint16x4_t)
vcvt_n_u16_f16(float16x4_t __a, const int __b) {
  return __builtin_aarch64_fcvtzuv4hf_uss(__a, __b);
}

FUNK(uint16x8_t)
vcvtq_n_u16_f16(float16x8_t __a, const int __b) {
  return __builtin_aarch64_fcvtzuv8hf_uss(__a, __b);
}

FUNK(float16x4_t)
vdiv_f16(float16x4_t __a, float16x4_t __b) {
  return __a / __b;
}

FUNK(float16x8_t)
vdivq_f16(float16x8_t __a, float16x8_t __b) {
  return __a / __b;
}

FUNK(float16x4_t)
vmax_f16(float16x4_t __a, float16x4_t __b) {
  return __builtin_aarch64_smax_nanv4hf(__a, __b);
}

FUNK(float16x8_t)
vmaxq_f16(float16x8_t __a, float16x8_t __b) {
  return __builtin_aarch64_smax_nanv8hf(__a, __b);
}

FUNK(float16x4_t)
vmaxnm_f16(float16x4_t __a, float16x4_t __b) {
  return __builtin_aarch64_fmaxv4hf(__a, __b);
}

FUNK(float16x8_t)
vmaxnmq_f16(float16x8_t __a, float16x8_t __b) {
  return __builtin_aarch64_fmaxv8hf(__a, __b);
}

FUNK(float16x4_t)
vmin_f16(float16x4_t __a, float16x4_t __b) {
  return __builtin_aarch64_smin_nanv4hf(__a, __b);
}

FUNK(float16x8_t)
vminq_f16(float16x8_t __a, float16x8_t __b) {
  return __builtin_aarch64_smin_nanv8hf(__a, __b);
}

FUNK(float16x4_t)
vminnm_f16(float16x4_t __a, float16x4_t __b) {
  return __builtin_aarch64_fminv4hf(__a, __b);
}

FUNK(float16x8_t)
vminnmq_f16(float16x8_t __a, float16x8_t __b) {
  return __builtin_aarch64_fminv8hf(__a, __b);
}

FUNK(float16x4_t)
vmul_f16(float16x4_t __a, float16x4_t __b) {
  return __a * __b;
}

FUNK(float16x8_t)
vmulq_f16(float16x8_t __a, float16x8_t __b) {
  return __a * __b;
}

FUNK(float16x4_t)
vmulx_f16(float16x4_t __a, float16x4_t __b) {
  return __builtin_aarch64_fmulxv4hf(__a, __b);
}

FUNK(float16x8_t)
vmulxq_f16(float16x8_t __a, float16x8_t __b) {
  return __builtin_aarch64_fmulxv8hf(__a, __b);
}

FUNK(float16x4_t)
vpadd_f16(float16x4_t a, float16x4_t b) {
  return __builtin_aarch64_faddpv4hf(a, b);
}

FUNK(float16x8_t)
vpaddq_f16(float16x8_t a, float16x8_t b) {
  return __builtin_aarch64_faddpv8hf(a, b);
}

FUNK(float16x4_t)
vpmax_f16(float16x4_t a, float16x4_t b) {
  return __builtin_aarch64_smax_nanpv4hf(a, b);
}

FUNK(float16x8_t)
vpmaxq_f16(float16x8_t a, float16x8_t b) {
  return __builtin_aarch64_smax_nanpv8hf(a, b);
}

FUNK(float16x4_t)
vpmaxnm_f16(float16x4_t a, float16x4_t b) {
  return __builtin_aarch64_smaxpv4hf(a, b);
}

FUNK(float16x8_t)
vpmaxnmq_f16(float16x8_t a, float16x8_t b) {
  return __builtin_aarch64_smaxpv8hf(a, b);
}

FUNK(float16x4_t)
vpmin_f16(float16x4_t a, float16x4_t b) {
  return __builtin_aarch64_smin_nanpv4hf(a, b);
}

FUNK(float16x8_t)
vpminq_f16(float16x8_t a, float16x8_t b) {
  return __builtin_aarch64_smin_nanpv8hf(a, b);
}

FUNK(float16x4_t)
vpminnm_f16(float16x4_t a, float16x4_t b) {
  return __builtin_aarch64_sminpv4hf(a, b);
}

FUNK(float16x8_t)
vpminnmq_f16(float16x8_t a, float16x8_t b) {
  return __builtin_aarch64_sminpv8hf(a, b);
}

FUNK(float16x4_t)
vrecps_f16(float16x4_t __a, float16x4_t __b) {
  return __builtin_aarch64_frecpsv4hf(__a, __b);
}

FUNK(float16x8_t)
vrecpsq_f16(float16x8_t __a, float16x8_t __b) {
  return __builtin_aarch64_frecpsv8hf(__a, __b);
}

FUNK(float16x4_t)
vrsqrts_f16(float16x4_t a, float16x4_t b) {
  return __builtin_aarch64_rsqrtsv4hf(a, b);
}

FUNK(float16x8_t)
vrsqrtsq_f16(float16x8_t a, float16x8_t b) {
  return __builtin_aarch64_rsqrtsv8hf(a, b);
}

FUNK(float16x4_t)
vsub_f16(float16x4_t __a, float16x4_t __b) {
  return __a - __b;
}

FUNK(float16x8_t)
vsubq_f16(float16x8_t __a, float16x8_t __b) {
  return __a - __b;
}

FUNK(float16x4_t)
vfma_f16(float16x4_t __a, float16x4_t __b, float16x4_t __c) {
  return __builtin_aarch64_fmav4hf(__b, __c, __a);
}

FUNK(float16x8_t)
vfmaq_f16(float16x8_t __a, float16x8_t __b, float16x8_t __c) {
  return __builtin_aarch64_fmav8hf(__b, __c, __a);
}

FUNK(float16x4_t)
vfms_f16(float16x4_t __a, float16x4_t __b, float16x4_t __c) {
  return __builtin_aarch64_fnmav4hf(__b, __c, __a);
}

FUNK(float16x8_t)
vfmsq_f16(float16x8_t __a, float16x8_t __b, float16x8_t __c) {
  return __builtin_aarch64_fnmav8hf(__b, __c, __a);
}

FUNK(float16_t)
vfmah_lane_f16(float16_t __a, float16_t __b, float16x4_t __c,
               const int __lane) {
  return vfmah_f16(__a, __b, __aarch64_vget_lane_any(__c, __lane));
}

FUNK(float16_t)
vfmah_laneq_f16(float16_t __a, float16_t __b, float16x8_t __c,
                const int __lane) {
  return vfmah_f16(__a, __b, __aarch64_vget_lane_any(__c, __lane));
}

FUNK(float16x4_t)
vfma_lane_f16(float16x4_t __a, float16x4_t __b, float16x4_t __c,
              const int __lane) {
  return vfma_f16(__a, __b, __aarch64_vdup_lane_f16(__c, __lane));
}

FUNK(float16x8_t)
vfmaq_lane_f16(float16x8_t __a, float16x8_t __b, float16x4_t __c,
               const int __lane) {
  return vfmaq_f16(__a, __b, __aarch64_vdupq_lane_f16(__c, __lane));
}

FUNK(float16x4_t)
vfma_laneq_f16(float16x4_t __a, float16x4_t __b, float16x8_t __c,
               const int __lane) {
  return vfma_f16(__a, __b, __aarch64_vdup_laneq_f16(__c, __lane));
}

FUNK(float16x8_t)
vfmaq_laneq_f16(float16x8_t __a, float16x8_t __b, float16x8_t __c,
                const int __lane) {
  return vfmaq_f16(__a, __b, __aarch64_vdupq_laneq_f16(__c, __lane));
}

FUNK(float16x4_t)
vfma_n_f16(float16x4_t __a, float16x4_t __b, float16_t __c) {
  return vfma_f16(__a, __b, vdup_n_f16(__c));
}

FUNK(float16x8_t)
vfmaq_n_f16(float16x8_t __a, float16x8_t __b, float16_t __c) {
  return vfmaq_f16(__a, __b, vdupq_n_f16(__c));
}

FUNK(float16_t)
vfmsh_lane_f16(float16_t __a, float16_t __b, float16x4_t __c,
               const int __lane) {
  return vfmsh_f16(__a, __b, __aarch64_vget_lane_any(__c, __lane));
}

FUNK(float16_t)
vfmsh_laneq_f16(float16_t __a, float16_t __b, float16x8_t __c,
                const int __lane) {
  return vfmsh_f16(__a, __b, __aarch64_vget_lane_any(__c, __lane));
}

FUNK(float16x4_t)
vfms_lane_f16(float16x4_t __a, float16x4_t __b, float16x4_t __c,
              const int __lane) {
  return vfms_f16(__a, __b, __aarch64_vdup_lane_f16(__c, __lane));
}

FUNK(float16x8_t)
vfmsq_lane_f16(float16x8_t __a, float16x8_t __b, float16x4_t __c,
               const int __lane) {
  return vfmsq_f16(__a, __b, __aarch64_vdupq_lane_f16(__c, __lane));
}

FUNK(float16x4_t)
vfms_laneq_f16(float16x4_t __a, float16x4_t __b, float16x8_t __c,
               const int __lane) {
  return vfms_f16(__a, __b, __aarch64_vdup_laneq_f16(__c, __lane));
}

FUNK(float16x8_t)
vfmsq_laneq_f16(float16x8_t __a, float16x8_t __b, float16x8_t __c,
                const int __lane) {
  return vfmsq_f16(__a, __b, __aarch64_vdupq_laneq_f16(__c, __lane));
}

FUNK(float16x4_t)
vfms_n_f16(float16x4_t __a, float16x4_t __b, float16_t __c) {
  return vfms_f16(__a, __b, vdup_n_f16(__c));
}

FUNK(float16x8_t)
vfmsq_n_f16(float16x8_t __a, float16x8_t __b, float16_t __c) {
  return vfmsq_f16(__a, __b, vdupq_n_f16(__c));
}

FUNK(float16_t)
vmulh_lane_f16(float16_t __a, float16x4_t __b, const int __lane) {
  return __a * __aarch64_vget_lane_any(__b, __lane);
}

FUNK(float16x4_t)
vmul_lane_f16(float16x4_t __a, float16x4_t __b, const int __lane) {
  return vmul_f16(__a, vdup_n_f16(__aarch64_vget_lane_any(__b, __lane)));
}

FUNK(float16x8_t)
vmulq_lane_f16(float16x8_t __a, float16x4_t __b, const int __lane) {
  return vmulq_f16(__a, vdupq_n_f16(__aarch64_vget_lane_any(__b, __lane)));
}

FUNK(float16_t)
vmulh_laneq_f16(float16_t __a, float16x8_t __b, const int __lane) {
  return __a * __aarch64_vget_lane_any(__b, __lane);
}

FUNK(float16x4_t)
vmul_laneq_f16(float16x4_t __a, float16x8_t __b, const int __lane) {
  return vmul_f16(__a, vdup_n_f16(__aarch64_vget_lane_any(__b, __lane)));
}

FUNK(float16x8_t)
vmulq_laneq_f16(float16x8_t __a, float16x8_t __b, const int __lane) {
  return vmulq_f16(__a, vdupq_n_f16(__aarch64_vget_lane_any(__b, __lane)));
}

FUNK(float16x4_t)
vmul_n_f16(float16x4_t __a, float16_t __b) {
  return vmul_lane_f16(__a, vdup_n_f16(__b), 0);
}

FUNK(float16x8_t)
vmulq_n_f16(float16x8_t __a, float16_t __b) {
  return vmulq_laneq_f16(__a, vdupq_n_f16(__b), 0);
}

FUNK(float16_t)
vmulxh_lane_f16(float16_t __a, float16x4_t __b, const int __lane) {
  return vmulxh_f16(__a, __aarch64_vget_lane_any(__b, __lane));
}

FUNK(float16x4_t)
vmulx_lane_f16(float16x4_t __a, float16x4_t __b, const int __lane) {
  return vmulx_f16(__a, __aarch64_vdup_lane_f16(__b, __lane));
}

FUNK(float16x8_t)
vmulxq_lane_f16(float16x8_t __a, float16x4_t __b, const int __lane) {
  return vmulxq_f16(__a, __aarch64_vdupq_lane_f16(__b, __lane));
}

FUNK(float16_t)
vmulxh_laneq_f16(float16_t __a, float16x8_t __b, const int __lane) {
  return vmulxh_f16(__a, __aarch64_vget_lane_any(__b, __lane));
}

FUNK(float16x4_t)
vmulx_laneq_f16(float16x4_t __a, float16x8_t __b, const int __lane) {
  return vmulx_f16(__a, __aarch64_vdup_laneq_f16(__b, __lane));
}

FUNK(float16x8_t)
vmulxq_laneq_f16(float16x8_t __a, float16x8_t __b, const int __lane) {
  return vmulxq_f16(__a, __aarch64_vdupq_laneq_f16(__b, __lane));
}

FUNK(float16x4_t)
vmulx_n_f16(float16x4_t __a, float16_t __b) {
  return vmulx_f16(__a, vdup_n_f16(__b));
}

FUNK(float16x8_t)
vmulxq_n_f16(float16x8_t __a, float16_t __b) {
  return vmulxq_f16(__a, vdupq_n_f16(__b));
}

FUNK(float16_t)
vmaxv_f16(float16x4_t __a) {
  return __builtin_aarch64_reduc_smax_nan_scal_v4hf(__a);
}

FUNK(float16_t)
vmaxvq_f16(float16x8_t __a) {
  return __builtin_aarch64_reduc_smax_nan_scal_v8hf(__a);
}

FUNK(float16_t)
vminv_f16(float16x4_t __a) {
  return __builtin_aarch64_reduc_smin_nan_scal_v4hf(__a);
}

FUNK(float16_t)
vminvq_f16(float16x8_t __a) {
  return __builtin_aarch64_reduc_smin_nan_scal_v8hf(__a);
}

FUNK(float16_t)
vmaxnmv_f16(float16x4_t __a) {
  return __builtin_aarch64_reduc_smax_scal_v4hf(__a);
}

FUNK(float16_t)
vmaxnmvq_f16(float16x8_t __a) {
  return __builtin_aarch64_reduc_smax_scal_v8hf(__a);
}

FUNK(float16_t)
vminnmv_f16(float16x4_t __a) {
  return __builtin_aarch64_reduc_smin_scal_v4hf(__a);
}

FUNK(float16_t)
vminnmvq_f16(float16x8_t __a) {
  return __builtin_aarch64_reduc_smin_scal_v8hf(__a);
}

#pragma GCC pop_options

#pragma GCC push_options
#pragma GCC target("arch=armv8.2-a+dotprod")

FUNK(uint32x2_t)
vdot_u32(uint32x2_t __r, uint8x8_t __a, uint8x8_t __b) {
  return __builtin_aarch64_udotv8qi_uuuu(__r, __a, __b);
}

FUNK(uint32x4_t)
vdotq_u32(uint32x4_t __r, uint8x16_t __a, uint8x16_t __b) {
  return __builtin_aarch64_udotv16qi_uuuu(__r, __a, __b);
}

FUNK(int32x2_t)
vdot_s32(int32x2_t __r, int8x8_t __a, int8x8_t __b) {
  return __builtin_aarch64_sdotv8qi(__r, __a, __b);
}

FUNK(int32x4_t)
vdotq_s32(int32x4_t __r, int8x16_t __a, int8x16_t __b) {
  return __builtin_aarch64_sdotv16qi(__r, __a, __b);
}

FUNK(uint32x2_t)
vdot_lane_u32(uint32x2_t __r, uint8x8_t __a, uint8x8_t __b, const int __index) {
  return __builtin_aarch64_udot_lanev8qi_uuuus(__r, __a, __b, __index);
}

FUNK(uint32x2_t)
vdot_laneq_u32(uint32x2_t __r, uint8x8_t __a, uint8x16_t __b,
               const int __index) {
  return __builtin_aarch64_udot_laneqv8qi_uuuus(__r, __a, __b, __index);
}

FUNK(uint32x4_t)
vdotq_lane_u32(uint32x4_t __r, uint8x16_t __a, uint8x8_t __b,
               const int __index) {
  return __builtin_aarch64_udot_lanev16qi_uuuus(__r, __a, __b, __index);
}

FUNK(uint32x4_t)
vdotq_laneq_u32(uint32x4_t __r, uint8x16_t __a, uint8x16_t __b,
                const int __index) {
  return __builtin_aarch64_udot_laneqv16qi_uuuus(__r, __a, __b, __index);
}

FUNK(int32x2_t)
vdot_lane_s32(int32x2_t __r, int8x8_t __a, int8x8_t __b, const int __index) {
  return __builtin_aarch64_sdot_lanev8qi(__r, __a, __b, __index);
}

FUNK(int32x2_t)
vdot_laneq_s32(int32x2_t __r, int8x8_t __a, int8x16_t __b, const int __index) {
  return __builtin_aarch64_sdot_laneqv8qi(__r, __a, __b, __index);
}

FUNK(int32x4_t)
vdotq_lane_s32(int32x4_t __r, int8x16_t __a, int8x8_t __b, const int __index) {
  return __builtin_aarch64_sdot_lanev16qi(__r, __a, __b, __index);
}

FUNK(int32x4_t)
vdotq_laneq_s32(int32x4_t __r, int8x16_t __a, int8x16_t __b,
                const int __index) {
  return __builtin_aarch64_sdot_laneqv16qi(__r, __a, __b, __index);
}
#pragma GCC pop_options

#pragma GCC push_options
#pragma GCC target("arch=armv8.2-a+sm4")

FUNK(uint32x4_t)
vsm3ss1q_u32(uint32x4_t __a, uint32x4_t __b, uint32x4_t __c) {
  return __builtin_aarch64_sm3ss1qv4si_uuuu(__a, __b, __c);
}

FUNK(uint32x4_t)
vsm3tt1aq_u32(uint32x4_t __a, uint32x4_t __b, uint32x4_t __c,
              const int __imm2) {
  return __builtin_aarch64_sm3tt1aqv4si_uuuus(__a, __b, __c, __imm2);
}

FUNK(uint32x4_t)
vsm3tt1bq_u32(uint32x4_t __a, uint32x4_t __b, uint32x4_t __c,
              const int __imm2) {
  return __builtin_aarch64_sm3tt1bqv4si_uuuus(__a, __b, __c, __imm2);
}

FUNK(uint32x4_t)
vsm3tt2aq_u32(uint32x4_t __a, uint32x4_t __b, uint32x4_t __c,
              const int __imm2) {
  return __builtin_aarch64_sm3tt2aqv4si_uuuus(__a, __b, __c, __imm2);
}

FUNK(uint32x4_t)
vsm3tt2bq_u32(uint32x4_t __a, uint32x4_t __b, uint32x4_t __c,
              const int __imm2) {
  return __builtin_aarch64_sm3tt2bqv4si_uuuus(__a, __b, __c, __imm2);
}

FUNK(uint32x4_t)
vsm3partw1q_u32(uint32x4_t __a, uint32x4_t __b, uint32x4_t __c) {
  return __builtin_aarch64_sm3partw1qv4si_uuuu(__a, __b, __c);
}
FUNK(uint32x4_t)
vsm3partw2q_u32(uint32x4_t __a, uint32x4_t __b, uint32x4_t __c) {
  return __builtin_aarch64_sm3partw2qv4si_uuuu(__a, __b, __c);
}

FUNK(uint32x4_t)
vsm4eq_u32(uint32x4_t __a, uint32x4_t __b) {
  return __builtin_aarch64_sm4eqv4si_uuu(__a, __b);
}

FUNK(uint32x4_t)
vsm4ekeyq_u32(uint32x4_t __a, uint32x4_t __b) {
  return __builtin_aarch64_sm4ekeyqv4si_uuu(__a, __b);
}

#pragma GCC pop_options

#pragma GCC push_options
#pragma GCC target("arch=armv8.2-a+sha3")

FUNK(uint64x2_t)
vsha512hq_u64(uint64x2_t __a, uint64x2_t __b, uint64x2_t __c) {
  return __builtin_aarch64_crypto_sha512hqv2di_uuuu(__a, __b, __c);
}

FUNK(uint64x2_t)
vsha512h2q_u64(uint64x2_t __a, uint64x2_t __b, uint64x2_t __c) {
  return __builtin_aarch64_crypto_sha512h2qv2di_uuuu(__a, __b, __c);
}

FUNK(uint64x2_t)
vsha512su0q_u64(uint64x2_t __a, uint64x2_t __b) {
  return __builtin_aarch64_crypto_sha512su0qv2di_uuu(__a, __b);
}

FUNK(uint64x2_t)
vsha512su1q_u64(uint64x2_t __a, uint64x2_t __b, uint64x2_t __c) {
  return __builtin_aarch64_crypto_sha512su1qv2di_uuuu(__a, __b, __c);
}

FUNK(uint8x16_t)
veor3q_u8(uint8x16_t __a, uint8x16_t __b, uint8x16_t __c) {
  return __builtin_aarch64_eor3qv16qi_uuuu(__a, __b, __c);
}

FUNK(uint16x8_t)
veor3q_u16(uint16x8_t __a, uint16x8_t __b, uint16x8_t __c) {
  return __builtin_aarch64_eor3qv8hi_uuuu(__a, __b, __c);
}

FUNK(uint32x4_t)
veor3q_u32(uint32x4_t __a, uint32x4_t __b, uint32x4_t __c) {
  return __builtin_aarch64_eor3qv4si_uuuu(__a, __b, __c);
}

FUNK(uint64x2_t)
veor3q_u64(uint64x2_t __a, uint64x2_t __b, uint64x2_t __c) {
  return __builtin_aarch64_eor3qv2di_uuuu(__a, __b, __c);
}

FUNK(int8x16_t)
veor3q_s8(int8x16_t __a, int8x16_t __b, int8x16_t __c) {
  return __builtin_aarch64_eor3qv16qi(__a, __b, __c);
}

FUNK(int16x8_t)
veor3q_s16(int16x8_t __a, int16x8_t __b, int16x8_t __c) {
  return __builtin_aarch64_eor3qv8hi(__a, __b, __c);
}

FUNK(int32x4_t)
veor3q_s32(int32x4_t __a, int32x4_t __b, int32x4_t __c) {
  return __builtin_aarch64_eor3qv4si(__a, __b, __c);
}

FUNK(int64x2_t)
veor3q_s64(int64x2_t __a, int64x2_t __b, int64x2_t __c) {
  return __builtin_aarch64_eor3qv2di(__a, __b, __c);
}

FUNK(uint64x2_t)
vrax1q_u64(uint64x2_t __a, uint64x2_t __b) {
  return __builtin_aarch64_rax1qv2di_uuu(__a, __b);
}

FUNK(uint64x2_t)
vxarq_u64(uint64x2_t __a, uint64x2_t __b, const int imm6) {
  return __builtin_aarch64_xarqv2di_uuus(__a, __b, imm6);
}

FUNK(uint8x16_t)
vbcaxq_u8(uint8x16_t __a, uint8x16_t __b, uint8x16_t __c) {
  return __builtin_aarch64_bcaxqv16qi_uuuu(__a, __b, __c);
}

FUNK(uint16x8_t)
vbcaxq_u16(uint16x8_t __a, uint16x8_t __b, uint16x8_t __c) {
  return __builtin_aarch64_bcaxqv8hi_uuuu(__a, __b, __c);
}

FUNK(uint32x4_t)
vbcaxq_u32(uint32x4_t __a, uint32x4_t __b, uint32x4_t __c) {
  return __builtin_aarch64_bcaxqv4si_uuuu(__a, __b, __c);
}

FUNK(uint64x2_t)
vbcaxq_u64(uint64x2_t __a, uint64x2_t __b, uint64x2_t __c) {
  return __builtin_aarch64_bcaxqv2di_uuuu(__a, __b, __c);
}

FUNK(int8x16_t)
vbcaxq_s8(int8x16_t __a, int8x16_t __b, int8x16_t __c) {
  return __builtin_aarch64_bcaxqv16qi(__a, __b, __c);
}

FUNK(int16x8_t)
vbcaxq_s16(int16x8_t __a, int16x8_t __b, int16x8_t __c) {
  return __builtin_aarch64_bcaxqv8hi(__a, __b, __c);
}

FUNK(int32x4_t)
vbcaxq_s32(int32x4_t __a, int32x4_t __b, int32x4_t __c) {
  return __builtin_aarch64_bcaxqv4si(__a, __b, __c);
}

FUNK(int64x2_t)
vbcaxq_s64(int64x2_t __a, int64x2_t __b, int64x2_t __c) {
  return __builtin_aarch64_bcaxqv2di(__a, __b, __c);
}

#pragma GCC pop_options

#pragma GCC push_options
#pragma GCC target("arch=armv8.3-a")

#pragma GCC push_options
#pragma GCC target("+fp16")
FUNK(float16x4_t)
vcadd_rot90_f16(float16x4_t __a, float16x4_t __b) {
  return __builtin_aarch64_fcadd90v4hf(__a, __b);
}

FUNK(float16x8_t)
vcaddq_rot90_f16(float16x8_t __a, float16x8_t __b) {
  return __builtin_aarch64_fcadd90v8hf(__a, __b);
}

FUNK(float16x4_t)
vcadd_rot270_f16(float16x4_t __a, float16x4_t __b) {
  return __builtin_aarch64_fcadd270v4hf(__a, __b);
}

FUNK(float16x8_t)
vcaddq_rot270_f16(float16x8_t __a, float16x8_t __b) {
  return __builtin_aarch64_fcadd270v8hf(__a, __b);
}

FUNK(float16x4_t)
vcmla_f16(float16x4_t __r, float16x4_t __a, float16x4_t __b) {
  return __builtin_aarch64_fcmla0v4hf(__r, __a, __b);
}

FUNK(float16x8_t)
vcmlaq_f16(float16x8_t __r, float16x8_t __a, float16x8_t __b) {
  return __builtin_aarch64_fcmla0v8hf(__r, __a, __b);
}

FUNK(float16x4_t)
vcmla_lane_f16(float16x4_t __r, float16x4_t __a, float16x4_t __b,
               const int __index) {
  return __builtin_aarch64_fcmla_lane0v4hf(__r, __a, __b, __index);
}

FUNK(float16x4_t)
vcmla_laneq_f16(float16x4_t __r, float16x4_t __a, float16x8_t __b,
                const int __index) {
  return __builtin_aarch64_fcmla_laneq0v4hf(__r, __a, __b, __index);
}

FUNK(float16x8_t)
vcmlaq_lane_f16(float16x8_t __r, float16x8_t __a, float16x4_t __b,
                const int __index) {
  return __builtin_aarch64_fcmlaq_lane0v8hf(__r, __a, __b, __index);
}

FUNK(float16x8_t)
vcmlaq_rot90_lane_f16(float16x8_t __r, float16x8_t __a, float16x4_t __b,
                      const int __index) {
  return __builtin_aarch64_fcmlaq_lane90v8hf(__r, __a, __b, __index);
}

FUNK(float16x4_t)
vcmla_rot90_laneq_f16(float16x4_t __r, float16x4_t __a, float16x8_t __b,
                      const int __index) {
  return __builtin_aarch64_fcmla_laneq90v4hf(__r, __a, __b, __index);
}

FUNK(float16x4_t)
vcmla_rot90_lane_f16(float16x4_t __r, float16x4_t __a, float16x4_t __b,
                     const int __index) {
  return __builtin_aarch64_fcmla_lane90v4hf(__r, __a, __b, __index);
}

FUNK(float16x8_t)
vcmlaq_rot90_f16(float16x8_t __r, float16x8_t __a, float16x8_t __b) {
  return __builtin_aarch64_fcmla90v8hf(__r, __a, __b);
}

FUNK(float16x4_t)
vcmla_rot90_f16(float16x4_t __r, float16x4_t __a, float16x4_t __b) {
  return __builtin_aarch64_fcmla90v4hf(__r, __a, __b);
}

FUNK(float16x8_t)
vcmlaq_laneq_f16(float16x8_t __r, float16x8_t __a, float16x8_t __b,
                 const int __index) {
  return __builtin_aarch64_fcmla_lane0v8hf(__r, __a, __b, __index);
}

FUNK(float16x4_t)
vcmla_rot180_laneq_f16(float16x4_t __r, float16x4_t __a, float16x8_t __b,
                       const int __index) {
  return __builtin_aarch64_fcmla_laneq180v4hf(__r, __a, __b, __index);
}

FUNK(float16x4_t)
vcmla_rot180_lane_f16(float16x4_t __r, float16x4_t __a, float16x4_t __b,
                      const int __index) {
  return __builtin_aarch64_fcmla_lane180v4hf(__r, __a, __b, __index);
}

FUNK(float16x8_t)
vcmlaq_rot180_f16(float16x8_t __r, float16x8_t __a, float16x8_t __b) {
  return __builtin_aarch64_fcmla180v8hf(__r, __a, __b);
}

FUNK(float16x4_t)
vcmla_rot180_f16(float16x4_t __r, float16x4_t __a, float16x4_t __b) {
  return __builtin_aarch64_fcmla180v4hf(__r, __a, __b);
}

FUNK(float16x8_t)
vcmlaq_rot90_laneq_f16(float16x8_t __r, float16x8_t __a, float16x8_t __b,
                       const int __index) {
  return __builtin_aarch64_fcmla_lane90v8hf(__r, __a, __b, __index);
}

FUNK(float16x8_t)
vcmlaq_rot270_laneq_f16(float16x8_t __r, float16x8_t __a, float16x8_t __b,
                        const int __index) {
  return __builtin_aarch64_fcmla_lane270v8hf(__r, __a, __b, __index);
}

FUNK(float16x8_t)
vcmlaq_rot270_lane_f16(float16x8_t __r, float16x8_t __a, float16x4_t __b,
                       const int __index) {
  return __builtin_aarch64_fcmlaq_lane270v8hf(__r, __a, __b, __index);
}

FUNK(float16x4_t)
vcmla_rot270_laneq_f16(float16x4_t __r, float16x4_t __a, float16x8_t __b,
                       const int __index) {
  return __builtin_aarch64_fcmla_laneq270v4hf(__r, __a, __b, __index);
}

FUNK(float16x8_t)
vcmlaq_rot270_f16(float16x8_t __r, float16x8_t __a, float16x8_t __b) {
  return __builtin_aarch64_fcmla270v8hf(__r, __a, __b);
}

FUNK(float16x4_t)
vcmla_rot270_f16(float16x4_t __r, float16x4_t __a, float16x4_t __b) {
  return __builtin_aarch64_fcmla270v4hf(__r, __a, __b);
}

FUNK(float16x8_t)
vcmlaq_rot180_laneq_f16(float16x8_t __r, float16x8_t __a, float16x8_t __b,
                        const int __index) {
  return __builtin_aarch64_fcmla_lane180v8hf(__r, __a, __b, __index);
}

FUNK(float16x8_t)
vcmlaq_rot180_lane_f16(float16x8_t __r, float16x8_t __a, float16x4_t __b,
                       const int __index) {
  return __builtin_aarch64_fcmlaq_lane180v8hf(__r, __a, __b, __index);
}

FUNK(float16x4_t)
vcmla_rot270_lane_f16(float16x4_t __r, float16x4_t __a, float16x4_t __b,
                      const int __index) {
  return __builtin_aarch64_fcmla_lane270v4hf(__r, __a, __b, __index);
}
#pragma GCC pop_options

FUNK(float32x2_t)
vcadd_rot90_f32(float32x2_t __a, float32x2_t __b) {
  return __builtin_aarch64_fcadd90v2sf(__a, __b);
}

FUNK(float32x4_t)
vcaddq_rot90_f32(float32x4_t __a, float32x4_t __b) {
  return __builtin_aarch64_fcadd90v4sf(__a, __b);
}

FUNK(float64x2_t)
vcaddq_rot90_f64(float64x2_t __a, float64x2_t __b) {
  return __builtin_aarch64_fcadd90v2df(__a, __b);
}

FUNK(float32x2_t)
vcadd_rot270_f32(float32x2_t __a, float32x2_t __b) {
  return __builtin_aarch64_fcadd270v2sf(__a, __b);
}

FUNK(float32x4_t)
vcaddq_rot270_f32(float32x4_t __a, float32x4_t __b) {
  return __builtin_aarch64_fcadd270v4sf(__a, __b);
}

FUNK(float64x2_t)
vcaddq_rot270_f64(float64x2_t __a, float64x2_t __b) {
  return __builtin_aarch64_fcadd270v2df(__a, __b);
}

FUNK(float32x2_t)
vcmla_f32(float32x2_t __r, float32x2_t __a, float32x2_t __b) {
  return __builtin_aarch64_fcmla0v2sf(__r, __a, __b);
}

FUNK(float32x4_t)
vcmlaq_f32(float32x4_t __r, float32x4_t __a, float32x4_t __b) {
  return __builtin_aarch64_fcmla0v4sf(__r, __a, __b);
}

FUNK(float64x2_t)
vcmlaq_f64(float64x2_t __r, float64x2_t __a, float64x2_t __b) {
  return __builtin_aarch64_fcmla0v2df(__r, __a, __b);
}

FUNK(float32x2_t)
vcmla_lane_f32(float32x2_t __r, float32x2_t __a, float32x2_t __b,
               const int __index) {
  return __builtin_aarch64_fcmla_lane0v2sf(__r, __a, __b, __index);
}

FUNK(float32x2_t)
vcmla_laneq_f32(float32x2_t __r, float32x2_t __a, float32x4_t __b,
                const int __index) {
  return __builtin_aarch64_fcmla_laneq0v2sf(__r, __a, __b, __index);
}

FUNK(float32x4_t)
vcmlaq_lane_f32(float32x4_t __r, float32x4_t __a, float32x2_t __b,
                const int __index) {
  return __builtin_aarch64_fcmlaq_lane0v4sf(__r, __a, __b, __index);
}

FUNK(float32x4_t)
vcmlaq_laneq_f32(float32x4_t __r, float32x4_t __a, float32x4_t __b,
                 const int __index) {
  return __builtin_aarch64_fcmla_lane0v4sf(__r, __a, __b, __index);
}

FUNK(float32x2_t)
vcmla_rot90_f32(float32x2_t __r, float32x2_t __a, float32x2_t __b) {
  return __builtin_aarch64_fcmla90v2sf(__r, __a, __b);
}

FUNK(float32x4_t)
vcmlaq_rot90_f32(float32x4_t __r, float32x4_t __a, float32x4_t __b) {
  return __builtin_aarch64_fcmla90v4sf(__r, __a, __b);
}

FUNK(float64x2_t)
vcmlaq_rot90_f64(float64x2_t __r, float64x2_t __a, float64x2_t __b) {
  return __builtin_aarch64_fcmla90v2df(__r, __a, __b);
}

FUNK(float32x2_t)
vcmla_rot90_lane_f32(float32x2_t __r, float32x2_t __a, float32x2_t __b,
                     const int __index) {
  return __builtin_aarch64_fcmla_lane90v2sf(__r, __a, __b, __index);
}

FUNK(float32x2_t)
vcmla_rot90_laneq_f32(float32x2_t __r, float32x2_t __a, float32x4_t __b,
                      const int __index) {
  return __builtin_aarch64_fcmla_laneq90v2sf(__r, __a, __b, __index);
}

FUNK(float32x4_t)
vcmlaq_rot90_lane_f32(float32x4_t __r, float32x4_t __a, float32x2_t __b,
                      const int __index) {
  return __builtin_aarch64_fcmlaq_lane90v4sf(__r, __a, __b, __index);
}

FUNK(float32x4_t)
vcmlaq_rot90_laneq_f32(float32x4_t __r, float32x4_t __a, float32x4_t __b,
                       const int __index) {
  return __builtin_aarch64_fcmla_lane90v4sf(__r, __a, __b, __index);
}

FUNK(float32x2_t)
vcmla_rot180_f32(float32x2_t __r, float32x2_t __a, float32x2_t __b) {
  return __builtin_aarch64_fcmla180v2sf(__r, __a, __b);
}

FUNK(float32x4_t)
vcmlaq_rot180_f32(float32x4_t __r, float32x4_t __a, float32x4_t __b) {
  return __builtin_aarch64_fcmla180v4sf(__r, __a, __b);
}

FUNK(float64x2_t)
vcmlaq_rot180_f64(float64x2_t __r, float64x2_t __a, float64x2_t __b) {
  return __builtin_aarch64_fcmla180v2df(__r, __a, __b);
}

FUNK(float32x2_t)
vcmla_rot180_lane_f32(float32x2_t __r, float32x2_t __a, float32x2_t __b,
                      const int __index) {
  return __builtin_aarch64_fcmla_lane180v2sf(__r, __a, __b, __index);
}

FUNK(float32x2_t)
vcmla_rot180_laneq_f32(float32x2_t __r, float32x2_t __a, float32x4_t __b,
                       const int __index) {
  return __builtin_aarch64_fcmla_laneq180v2sf(__r, __a, __b, __index);
}

FUNK(float32x4_t)
vcmlaq_rot180_lane_f32(float32x4_t __r, float32x4_t __a, float32x2_t __b,
                       const int __index) {
  return __builtin_aarch64_fcmlaq_lane180v4sf(__r, __a, __b, __index);
}

FUNK(float32x4_t)
vcmlaq_rot180_laneq_f32(float32x4_t __r, float32x4_t __a, float32x4_t __b,
                        const int __index) {
  return __builtin_aarch64_fcmla_lane180v4sf(__r, __a, __b, __index);
}

FUNK(float32x2_t)
vcmla_rot270_f32(float32x2_t __r, float32x2_t __a, float32x2_t __b) {
  return __builtin_aarch64_fcmla270v2sf(__r, __a, __b);
}

FUNK(float32x4_t)
vcmlaq_rot270_f32(float32x4_t __r, float32x4_t __a, float32x4_t __b) {
  return __builtin_aarch64_fcmla270v4sf(__r, __a, __b);
}

FUNK(float64x2_t)
vcmlaq_rot270_f64(float64x2_t __r, float64x2_t __a, float64x2_t __b) {
  return __builtin_aarch64_fcmla270v2df(__r, __a, __b);
}

FUNK(float32x2_t)
vcmla_rot270_lane_f32(float32x2_t __r, float32x2_t __a, float32x2_t __b,
                      const int __index) {
  return __builtin_aarch64_fcmla_lane270v2sf(__r, __a, __b, __index);
}

FUNK(float32x2_t)
vcmla_rot270_laneq_f32(float32x2_t __r, float32x2_t __a, float32x4_t __b,
                       const int __index) {
  return __builtin_aarch64_fcmla_laneq270v2sf(__r, __a, __b, __index);
}

FUNK(float32x4_t)
vcmlaq_rot270_lane_f32(float32x4_t __r, float32x4_t __a, float32x2_t __b,
                       const int __index) {
  return __builtin_aarch64_fcmlaq_lane270v4sf(__r, __a, __b, __index);
}

FUNK(float32x4_t)
vcmlaq_rot270_laneq_f32(float32x4_t __r, float32x4_t __a, float32x4_t __b,
                        const int __index) {
  return __builtin_aarch64_fcmla_lane270v4sf(__r, __a, __b, __index);
}

#pragma GCC pop_options

#pragma GCC push_options
#pragma GCC target("arch=armv8.2-a+fp16fml")

FUNK(float32x2_t)
vfmlal_low_f16(float32x2_t __r, float16x4_t __a, float16x4_t __b) {
  return __builtin_aarch64_fmlal_lowv2sf(__r, __a, __b);
}

FUNK(float32x2_t)
vfmlsl_low_f16(float32x2_t __r, float16x4_t __a, float16x4_t __b) {
  return __builtin_aarch64_fmlsl_lowv2sf(__r, __a, __b);
}

FUNK(float32x4_t)
vfmlalq_low_f16(float32x4_t __r, float16x8_t __a, float16x8_t __b) {
  return __builtin_aarch64_fmlalq_lowv4sf(__r, __a, __b);
}

FUNK(float32x4_t)
vfmlslq_low_f16(float32x4_t __r, float16x8_t __a, float16x8_t __b) {
  return __builtin_aarch64_fmlslq_lowv4sf(__r, __a, __b);
}

FUNK(float32x2_t)
vfmlal_high_f16(float32x2_t __r, float16x4_t __a, float16x4_t __b) {
  return __builtin_aarch64_fmlal_highv2sf(__r, __a, __b);
}

FUNK(float32x2_t)
vfmlsl_high_f16(float32x2_t __r, float16x4_t __a, float16x4_t __b) {
  return __builtin_aarch64_fmlsl_highv2sf(__r, __a, __b);
}

FUNK(float32x4_t)
vfmlalq_high_f16(float32x4_t __r, float16x8_t __a, float16x8_t __b) {
  return __builtin_aarch64_fmlalq_highv4sf(__r, __a, __b);
}

FUNK(float32x4_t)
vfmlslq_high_f16(float32x4_t __r, float16x8_t __a, float16x8_t __b) {
  return __builtin_aarch64_fmlslq_highv4sf(__r, __a, __b);
}

FUNK(float32x2_t)
vfmlal_lane_low_f16(float32x2_t __r, float16x4_t __a, float16x4_t __b,
                    const int __lane) {
  return __builtin_aarch64_fmlal_lane_lowv2sf(__r, __a, __b, __lane);
}

FUNK(float32x2_t)
vfmlsl_lane_low_f16(float32x2_t __r, float16x4_t __a, float16x4_t __b,
                    const int __lane) {
  return __builtin_aarch64_fmlsl_lane_lowv2sf(__r, __a, __b, __lane);
}

FUNK(float32x2_t)
vfmlal_laneq_low_f16(float32x2_t __r, float16x4_t __a, float16x8_t __b,
                     const int __lane) {
  return __builtin_aarch64_fmlal_laneq_lowv2sf(__r, __a, __b, __lane);
}

FUNK(float32x2_t)
vfmlsl_laneq_low_f16(float32x2_t __r, float16x4_t __a, float16x8_t __b,
                     const int __lane) {
  return __builtin_aarch64_fmlsl_laneq_lowv2sf(__r, __a, __b, __lane);
}

FUNK(float32x4_t)
vfmlalq_lane_low_f16(float32x4_t __r, float16x8_t __a, float16x4_t __b,
                     const int __lane) {
  return __builtin_aarch64_fmlalq_lane_lowv4sf(__r, __a, __b, __lane);
}

FUNK(float32x4_t)
vfmlslq_lane_low_f16(float32x4_t __r, float16x8_t __a, float16x4_t __b,
                     const int __lane) {
  return __builtin_aarch64_fmlslq_lane_lowv4sf(__r, __a, __b, __lane);
}

FUNK(float32x4_t)
vfmlalq_laneq_low_f16(float32x4_t __r, float16x8_t __a, float16x8_t __b,
                      const int __lane) {
  return __builtin_aarch64_fmlalq_laneq_lowv4sf(__r, __a, __b, __lane);
}

FUNK(float32x4_t)
vfmlslq_laneq_low_f16(float32x4_t __r, float16x8_t __a, float16x8_t __b,
                      const int __lane) {
  return __builtin_aarch64_fmlslq_laneq_lowv4sf(__r, __a, __b, __lane);
}

FUNK(float32x2_t)
vfmlal_lane_high_f16(float32x2_t __r, float16x4_t __a, float16x4_t __b,
                     const int __lane) {
  return __builtin_aarch64_fmlal_lane_highv2sf(__r, __a, __b, __lane);
}

FUNK(float32x2_t)
vfmlsl_lane_high_f16(float32x2_t __r, float16x4_t __a, float16x4_t __b,
                     const int __lane) {
  return __builtin_aarch64_fmlsl_lane_highv2sf(__r, __a, __b, __lane);
}

FUNK(float32x2_t)
vfmlal_laneq_high_f16(float32x2_t __r, float16x4_t __a, float16x8_t __b,
                      const int __lane) {
  return __builtin_aarch64_fmlal_laneq_highv2sf(__r, __a, __b, __lane);
}

FUNK(float32x2_t)
vfmlsl_laneq_high_f16(float32x2_t __r, float16x4_t __a, float16x8_t __b,
                      const int __lane) {
  return __builtin_aarch64_fmlsl_laneq_highv2sf(__r, __a, __b, __lane);
}

FUNK(float32x4_t)
vfmlalq_lane_high_f16(float32x4_t __r, float16x8_t __a, float16x4_t __b,
                      const int __lane) {
  return __builtin_aarch64_fmlalq_lane_highv4sf(__r, __a, __b, __lane);
}

FUNK(float32x4_t)
vfmlslq_lane_high_f16(float32x4_t __r, float16x8_t __a, float16x4_t __b,
                      const int __lane) {
  return __builtin_aarch64_fmlslq_lane_highv4sf(__r, __a, __b, __lane);
}

FUNK(float32x4_t)
vfmlalq_laneq_high_f16(float32x4_t __r, float16x8_t __a, float16x8_t __b,
                       const int __lane) {
  return __builtin_aarch64_fmlalq_laneq_highv4sf(__r, __a, __b, __lane);
}

FUNK(float32x4_t)
vfmlslq_laneq_high_f16(float32x4_t __r, float16x8_t __a, float16x8_t __b,
                       const int __lane) {
  return __builtin_aarch64_fmlslq_laneq_highv4sf(__r, __a, __b, __lane);
}

#pragma GCC pop_options

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

#pragma GCC push_options /* -Wno-missing-braces */
#undef FUNK
#endif /* __aarch64__ */
#endif /* _AARCH64_NEON_H_ */
