#ifndef COSMOPOLITAN_LIBC_RUNTIME_GC_INTERNAL_H_
#define COSMOPOLITAN_LIBC_RUNTIME_GC_INTERNAL_H_
#include "libc/runtime/gc.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

#define gc(THING)          _gc(THING)
#define defer(FN, ARG)     _defer(FN, ARG)
#define gclongjmp(JB, ARG) _gclongjmp(JB, ARG)

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_GC_INTERNAL_H_ */
