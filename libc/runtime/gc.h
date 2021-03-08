#ifndef COSMOPOLITAN_LIBC_RUNTIME_GC_H_
#define COSMOPOLITAN_LIBC_RUNTIME_GC_H_
#include "libc/calls/calls.h"
#include "libc/nexgen32e/stackframe.h"
#include "libc/runtime/runtime.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void *_gc(void *) hidden;
void *_defer(void *, void *) hidden;
void __defer(struct StackFrame *, void *, void *) hidden;
void __deferer(struct StackFrame *, void *, void *) hidden;
void _gclongjmp(jmp_buf, int) nothrow wontreturn;

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define _gc(THING) _defer((void *)_weakfree, (void *)(THING))
#define _defer(FN, ARG)                                                \
  ({                                                                   \
    autotype(ARG) Arg = (ARG);                                         \
    /* prevent weird opts like tail call */                            \
    asm volatile("" : "+g"(Arg) : : "memory");                         \
    __defer((struct StackFrame *)__builtin_frame_address(0), FN, Arg); \
    asm volatile("" : "+g"(Arg) : : "memory");                         \
    Arg;                                                               \
  })
#endif /* defined(__GNUC__) && !defined(__STRICT_ANSI__) */

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_GC_H_ */
