#ifndef COSMOPOLITAN_LIBC_INTRIN_ASAN_H_
#define COSMOPOLITAN_LIBC_INTRIN_ASAN_H_
#include "libc/calls/struct/iovec.h"
#include "libc/macros.internal.h"

#define kAsanScale          3
#define kAsanMagic          0x7fff8000
#define kAsanNullPage       -1  /* ∅ 0xff */
#define kAsanProtected      -2  /* P 0xfe */
#define kAsanHeapFree       -3  /* F 0xfd */
#define kAsanHeapRelocated  -4  /* R 0xfc */
#define kAsanAllocaOverrun  -5  /* 𝑂 0xfb */
#define kAsanHeapUnderrun   -6  /* U 0xfa */
#define kAsanHeapOverrun    -7  /* O 0xf9 */
#define kAsanStackUnscoped  -8  /* s 0xf8 */
#define kAsanStackOverflow  -9  /* ! 0xf7 */
#define kAsanGlobalOrder    -10 /* I 0xf6 */
#define kAsanStackFree      -11 /* r 0xf5 */
#define kAsanStackPartial   -12 /* p 0xf4 */
#define kAsanStackOverrun   -13 /* o 0xf3 */
#define kAsanStackMiddle    -14 /* m 0xf2 */
#define kAsanStackUnderrun  -15 /* u 0xf1 */
#define kAsanAllocaUnderrun -16 /* 𝑈 0xf0 */
#define kAsanUnmapped       -17 /* M 0xef */
#define kAsanGlobalRedzone  -18 /* G 0xee */
#define kAsanGlobalGone     -19 /* 𝐺 0xed */

#define SHADOW(x)   ((signed char *)(((intptr_t)(x) >> kAsanScale) + kAsanMagic))
#define UNSHADOW(x) ((void *)(MAX(0, (intptr_t)(x)-kAsanMagic) << kAsanScale))

typedef void __asan_die_f(void);

struct AsanFault {
  signed char kind;
  signed char *shadow;
};

extern bool __asan_noreentry;

void __asan_unpoison(long, long);
void __asan_poison(long, long, signed char);
void __asan_verify(const void *, size_t);
void __asan_map_shadow(uintptr_t, size_t);
bool __asan_is_valid(const void *, long) nosideeffect;
bool __asan_is_valid_strlist(char *const *) strlenesque;
bool __asan_is_valid_iov(const struct iovec *, int) nosideeffect;
wint_t __asan_symbolize_access_poison(signed char) pureconst;
const char *__asan_describe_access_poison(signed char) pureconst;
struct AsanFault __asan_check(const void *, long) nosideeffect;

void __asan_free(void *);
void *__asan_malloc(size_t);
int __asan_malloc_trim(size_t);
int __asan_print_trace(void *);
void *__asan_calloc(size_t, size_t);
void *__asan_realloc(void *, size_t);
void *__asan_memalign(size_t, size_t);
size_t __asan_get_heap_size(const void *);
void *__asan_realloc_in_place(void *, size_t);

#endif /* COSMOPOLITAN_LIBC_INTRIN_ASAN_H_ */
