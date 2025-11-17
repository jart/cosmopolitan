#ifndef COSMOPOLITAN_THIRD_PARTY_HACLSTAR_MACROS_H_
#define COSMOPOLITAN_THIRD_PARTY_HACLSTAR_MACROS_H_
#include "libc/limits.h"
#include "libc/runtime/runtime.h"
COSMOPOLITAN_C_START_

#define KRML_HOST_FREE free
#define KRML_HOST_MALLOC malloc
#define KRML_HOST_CALLOC calloc
#define KRML_ALIGNED_MALLOC memalign
#define KRML_ALIGNED_FREE free

#define KRML_MAYBE_UNUSED_VAR(v) (void)(v)

#define KRML_PRE_ALIGN(X)
#define KRML_POST_ALIGN(X) __attribute__((__aligned__(X)))

#define KRML_HOST_EPRINTF(...) (void)0
#define KRML_HOST_EXIT(code) __builtin_trap()
#define KRML_HOST_PRINTF(...) (void)0

#define KRML_CHECK_SIZE(size_elt, sz) \
    do { \
        _Pragma("GCC diagnostic ignored \"-Wtype-limits\"") if ( \
          ((size_t)(sz)) > ((size_t)(SIZE_MAX / (size_elt)))) \
          __builtin_trap(); \
    } while (0)

#define KRML_EABORT(t, msg) \
    (KRML_HOST_PRINTF( \
       "KaRaMeL abort at %s:%d\n%s\n", __FILE__, __LINE__, msg), \
     KRML_HOST_EXIT(255), \
     *((t*)KRML_HOST_MALLOC(sizeof(t))))

#define Lib_Memzero0_memzero(dst, len, t, _ret_t) \
    explicit_bzero(dst, len * sizeof(t))

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_HACLSTAR_MACROS_H_ */
