#ifndef COSMOPOLITAN_LIBC_LOG_ASAN_H_
#define COSMOPOLITAN_LIBC_LOG_ASAN_H_

#define kAsanScale         3
#define kAsanMagic         0x7fff8000
#define kAsanHeapFree      -1
#define kAsanStackFree     -2
#define kAsanRelocated     -3
#define kAsanHeapUnderrun  -4
#define kAsanHeapOverrun   -5
#define kAsanGlobalOverrun -6
#define kAsanStackUnderrun -7
#define kAsanStackOverrun  -8
#define kAsanAllocaOverrun -9
#define kAsanUnscoped      -10

#define SHADOW(x) (((x) >> kAsanScale) + kAsanMagic)

void __asan_map_shadow(void *, size_t);

#endif /* COSMOPOLITAN_LIBC_LOG_ASAN_H_ */
