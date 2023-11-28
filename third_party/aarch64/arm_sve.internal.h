#if defined(__aarch64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _ARM_SVE_H_
#define _ARM_SVE_H_
#include "third_party/aarch64/arm_bf16.internal.h"
typedef __fp16 float16_t;
typedef float float32_t;
typedef double float64_t;
#pragma GCC aarch64 "third_party/aarch64/arm_sve.internal.h"
#endif
#endif
