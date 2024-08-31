#ifndef COSMOPOLITAN_LIBC_ISYSTEM_ARM_SVE_H_
#define COSMOPOLITAN_LIBC_ISYSTEM_ARM_SVE_H_
#ifdef __clang__
#include "third_party/aarch64/clang/arm_sve.h"
#else
#include "third_party/aarch64/arm_sve.internal.h"
#endif
#endif /* COSMOPOLITAN_LIBC_ISYSTEM_ARM_SVE_H_ */
