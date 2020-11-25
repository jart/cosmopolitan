#ifndef COSMOPOLITAN_LIBC_LOG_ASAN_H_
#define COSMOPOLITAN_LIBC_LOG_ASAN_H_

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
#define kAsanAllocaOverrun      -10
#define kAsanUnscoped           -11

#define SHADOW(x) (((x) >> kAsanScale) + kAsanMagic)

void __asan_map_shadow(void *, size_t);

#endif /* COSMOPOLITAN_LIBC_LOG_ASAN_H_ */
