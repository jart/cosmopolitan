#if defined(__aarch64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _AARCH64_BF16_H_
#define _AARCH64_BF16_H_
typedef __bf16 bfloat16_t;
typedef float float32_t;
#pragma GCC push_options
#pragma GCC target ("+nothing+bf16+nosimd")
__extension__ extern __inline bfloat16_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvth_bf16_f32 (float32_t __a)
{
  return __builtin_aarch64_bfcvtbf (__a);
}
__extension__ extern __inline float32_t
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcvtah_f32_bf16 (bfloat16_t __a)
{
  return __builtin_aarch64_bfcvtsf (__a);
}
#pragma GCC pop_options
#endif
#endif
