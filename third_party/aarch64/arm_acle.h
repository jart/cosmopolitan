#ifndef _GCC_ARM_ACLE_H
#define _GCC_ARM_ACLE_H
#include "libc/inttypes.h"
#include "libc/limits.h"
#include "libc/literal.h"

#pragma GCC push_options

#pragma GCC target("+nothing+crc")

#ifdef __cplusplus
extern "C" {
#endif

__extension__ static __inline uint32_t __attribute__((__always_inline__))
__crc32b(uint32_t __a, uint8_t __b) {
  return __builtin_aarch64_crc32b(__a, __b);
}

__extension__ static __inline uint32_t __attribute__((__always_inline__))
__crc32cb(uint32_t __a, uint8_t __b) {
  return __builtin_aarch64_crc32cb(__a, __b);
}

__extension__ static __inline uint32_t __attribute__((__always_inline__))
__crc32ch(uint32_t __a, uint16_t __b) {
  return __builtin_aarch64_crc32ch(__a, __b);
}

__extension__ static __inline uint32_t __attribute__((__always_inline__))
__crc32cw(uint32_t __a, uint32_t __b) {
  return __builtin_aarch64_crc32cw(__a, __b);
}

__extension__ static __inline uint32_t __attribute__((__always_inline__))
__crc32cd(uint32_t __a, uint64_t __b) {
  return __builtin_aarch64_crc32cx(__a, __b);
}

__extension__ static __inline uint32_t __attribute__((__always_inline__))
__crc32h(uint32_t __a, uint16_t __b) {
  return __builtin_aarch64_crc32h(__a, __b);
}

__extension__ static __inline uint32_t __attribute__((__always_inline__))
__crc32w(uint32_t __a, uint32_t __b) {
  return __builtin_aarch64_crc32w(__a, __b);
}

__extension__ static __inline uint32_t __attribute__((__always_inline__))
__crc32d(uint32_t __a, uint64_t __b) {
  return __builtin_aarch64_crc32x(__a, __b);
}

#ifdef __cplusplus
}
#endif

#pragma GCC pop_options

#endif
