#ifndef COSMOPOLITAN_LIBC_INTRIN_ASAN_H_
#define COSMOPOLITAN_LIBC_INTRIN_ASAN_H_
#include "libc/calls/struct/iovec.h"
#include "libc/intrin/asancodes.h"
#include "libc/macros.internal.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define SHADOW(x)   ((signed char *)(((intptr_t)(x) >> kAsanScale) + kAsanMagic))
#define UNSHADOW(x) ((void *)(MAX(0, (intptr_t)(x)-kAsanMagic) << kAsanScale))

typedef void __asan_die_f(void);

struct AsanFault {
  signed char kind;
  const signed char *shadow;
};

void __asan_unpoison(void *, long);
void __asan_poison(void *, long, signed char);
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
int __asan_is_leaky(void *);
int __asan_malloc_trim(size_t);
int __asan_print_trace(void *);
void *__asan_calloc(size_t, size_t);
void *__asan_realloc(void *, size_t);
void *__asan_memalign(size_t, size_t);
size_t __asan_get_heap_size(const void *);
void *__asan_realloc_in_place(void *, size_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_ASAN_H_ */
