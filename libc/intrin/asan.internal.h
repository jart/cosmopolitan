#ifndef COSMOPOLITAN_LIBC_INTRIN_ASAN_H_
#define COSMOPOLITAN_LIBC_INTRIN_ASAN_H_
#include "libc/calls/struct/iovec.h"
#include "libc/intrin/asancodes.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/stackframe.h"
COSMOPOLITAN_C_START_

#ifdef MODE_DBG
// MODE=dbg
// O(32mb) of morgue memory
// Θ(64) bytes of malloc overhead
#define ASAN_MORGUE_ITEMS     512
#define ASAN_MORGUE_THRESHOLD 65536
#define ASAN_TRACE_ITEMS      16
#else
// MODE=asan
// O(32mb) of morgue memory
// Θ(32) bytes of malloc overhead
#define ASAN_MORGUE_ITEMS     512
#define ASAN_MORGUE_THRESHOLD 65536
#define ASAN_TRACE_ITEMS      4
#endif

#define SHADOW(x)   ((signed char *)(((intptr_t)(x) >> kAsanScale) + kAsanMagic))
#define UNSHADOW(x) ((void *)(MAX(0, (intptr_t)(x) - kAsanMagic) << kAsanScale))

#define __asan_unreachable()                                      \
  do {                                                            \
    kprintf("%s:%d: __asan_unreachable()\n", __FILE__, __LINE__); \
    __builtin_trap();                                             \
  } while (0)

typedef void __asan_die_f(void);

struct AsanFault {
  signed char kind;
  const signed char *shadow;
};

struct AsanTrace {
  uint32_t p[ASAN_TRACE_ITEMS];  // assumes linkage into 32-bit space
};

void __asan_unpoison(void *, long);
void __asan_poison(void *, long, signed char);
void __asan_verify(const void *, size_t);
void __asan_verify_str(const char *);
void __asan_shadow(void *, size_t);
void __asan_unshadow(void *, size_t);
bool __asan_is_valid(const void *, long) nosideeffect;
bool __asan_is_valid_str(const char *) nosideeffect;
bool __asan_is_valid_strlist(char *const *) nosideeffect;
bool __asan_is_valid_iov(const struct iovec *, int) nosideeffect;
struct AsanFault __asan_check(const void *, long) nosideeffect;
struct AsanFault __asan_check_str(const char *) nosideeffect;

int __asan_is_leaky(void *);
int __asan_print_trace(void *);
bool __asan_is_mapped(const void *);
__asan_die_f *__asan_die(void) __wur;
void __asan_memset(void *, char, size_t);
size_t __asan_get_heap_size(const void *);
void *__asan_memcpy(void *, const void *, size_t);
void __asan_rawtrace(struct AsanTrace *, const struct StackFrame *);
void __asan_report_memory_origin(const unsigned char *, int, signed char);
void __asan_report_memory_origin_image(intptr_t, int);
void __asan_report_memory_origin_heap(const unsigned char *, int);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_INTRIN_ASAN_H_ */
