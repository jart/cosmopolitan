#ifndef COSMOPOLITAN_LIBC_ISYSTEM_ARM_NEON_H_
#define COSMOPOLITAN_LIBC_ISYSTEM_ARM_NEON_H_
#ifdef __clang__
#include "third_party/aarch64/clang/arm_neon.h"
#else
#include "third_party/aarch64/arm_neon.internal.h"
#endif
#endif /* COSMOPOLITAN_LIBC_ISYSTEM_ARM_NEON_H_ */
