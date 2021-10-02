#ifndef COSMOPOLITAN_LIBC_INTRIN_ASAN_H_
#define COSMOPOLITAN_LIBC_INTRIN_ASAN_H_
#include "libc/calls/struct/iovec.h"

#define kAsanScale              3
#define kAsanMagic              0x7fff8000
#define kAsanHeapFree           -1  /* F */
#define kAsanStackFree          -2  /* F */
#define kAsanRelocated          -3  /* R */
#define kAsanHeapUnderrun       -4  /* U */
#define kAsanHeapOverrun        -5  /* O */
#define kAsanGlobalOverrun      -6  /* O */
#define kAsanGlobalUnregistered -7  /* G */
#define kAsanStackUnderrun      -8  /* U */
#define kAsanStackOverrun       -9  /* O */
#define kAsanAllocaUnderrun     -10 /* U */
#define kAsanAllocaOverrun      -11 /* O */
#define kAsanUnscoped           -12 /* S */
#define kAsanUnmapped           -13 /* M */
#define kAsanProtected          -14 /* P */
#define kAsanStackGuard         -15 /* _ */
#define kAsanNullPage           -16

#define SHADOW(x)   ((signed char *)(((uintptr_t)(x) >> kAsanScale) + kAsanMagic))
#define UNSHADOW(x) ((void *)(((uintptr_t)(x) + 0x7fff8000) << 3))

struct AsanFault {
  char kind;
  signed char *shadow;
};

void __asan_unpoison(uintptr_t, size_t);
void __asan_verify(const void *, size_t);
void __asan_map_shadow(uintptr_t, size_t);
void __asan_poison(uintptr_t, size_t, int);
bool __asan_is_valid(const void *, size_t);
bool __asan_is_valid_strlist(char *const *);
bool __asan_is_valid_iov(const struct iovec *, int);
struct AsanFault __asan_check(const void *, size_t);
void __asan_report_memory_fault(void *, int, const char *) wontreturn;
void __asan_report(void *, int, const char *, char) wontreturn;
void *__asan_memalign(size_t, size_t);
void __asan_free(void *);
void *__asan_malloc(size_t);
void *__asan_calloc(size_t, size_t);
void *__asan_realloc(void *, size_t);
void *__asan_realloc_in_place(void *, size_t);
void *__asan_valloc(size_t);
void *__asan_pvalloc(size_t);
int __asan_malloc_trim(size_t);
void __asan_die(const char *) wontreturn;

#endif /* COSMOPOLITAN_LIBC_INTRIN_ASAN_H_ */
