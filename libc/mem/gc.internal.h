#ifndef COSMOPOLITAN_LIBC_MEM_GC_INTERNAL_H_
#define COSMOPOLITAN_LIBC_MEM_GC_INTERNAL_H_
#include "libc/mem/gc.h"

#define gc(THING)          _gc(THING)
#define defer(FN, ARG)     _defer(FN, ARG)
#define gclongjmp(JB, ARG) _gclongjmp(JB, ARG)

#endif /* COSMOPOLITAN_LIBC_MEM_GC_INTERNAL_H_ */
