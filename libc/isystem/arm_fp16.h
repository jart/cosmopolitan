#ifndef COSMOPOLITAN_LIBC_ISYSTEM_ARM_FP16_H_
#define COSMOPOLITAN_LIBC_ISYSTEM_ARM_FP16_H_
#ifdef __clang__
#include "third_party/aarch64/clang/arm_fp16.h"
#else
#include "third_party/aarch64/arm_fp16.internal.h"
#endif
#endif /* COSMOPOLITAN_LIBC_ISYSTEM_ARM_FP16_H_ */
