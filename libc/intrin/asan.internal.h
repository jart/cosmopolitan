#ifndef COSMOPOLITAN_LIBC_INTRIN_ASAN_H_
#define COSMOPOLITAN_LIBC_INTRIN_ASAN_H_
#include "libc/calls/struct/iovec.h"

#define kAsanScale              3
#define kAsanMagic              0x7fff8000
#define kAsanHeapFree           -1
#define kAsanStackFree          -2
#define kAsanRelocated          -3
#define kAsanHeapUnderrun       -4
#define kAsanHeapOverrun        -5
#define kAsanGlobalOverrun      -6
#define kAsanGlobalUnregistered -7
#define kAsanStackUnderrun      -8
#define kAsanStackOverrun       -9
#define kAsanAllocaUnderrun     -10
#define kAsanAllocaOverrun      -11
#define kAsanUnscoped           -12
#define kAsanUnmapped           -13

#define SHADOW(x) ((signed char *)(((uintptr_t)(x) >> kAsanScale) + kAsanMagic))

void __asan_map_shadow(uintptr_t, size_t);
void __asan_poison(uintptr_t, size_t, int);
void __asan_unpoison(uintptr_t, size_t);
bool __asan_is_valid(const void *, size_t);
bool __asan_is_valid_iov(const struct iovec *, int);

#endif /* COSMOPOLITAN_LIBC_INTRIN_ASAN_H_ */
