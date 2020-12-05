#ifndef COSMOPOLITAN_LIBC_RUNTIME_GC_H_
#define COSMOPOLITAN_LIBC_RUNTIME_GC_H_
#include "libc/calls/calls.h"
#include "libc/nexgen32e/stackframe.h"
#include "libc/runtime/runtime.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/**
 * @fileoverview Cosmopolitan Return-Oriented Garbage Collector.
 *
 * This is the same thing as {@code std::unique_ptr<>} in C++ or the
 * {@code defer} keyword in Go. We harness the power of ROP for good
 * using very few lines of code.
 */

/**
 * Releases resource when function returns.
 *
 * @warning do not return a gc()'d pointer
 * @warning do not realloc() with gc()'d pointer
 */
#define gc(THING) defer((void *)weakfree, (void *)(THING))

/**
 * Same as longjmp() but runs gc() / defer() destructors.
 */
void gclongjmp(jmp_buf, int) nothrow wontreturn paramsnonnull();

/**
 * Calls FN(ARG) when function returns.
 */
#define defer(FN, ARG)                                                 \
  ({                                                                   \
    autotype(ARG) Arg = (ARG);                                         \
    /* prevent weird opts like tail call */                            \
    asm volatile("" : "+g"(Arg) : : "memory");                         \
    __defer((struct StackFrame *)__builtin_frame_address(0), FN, Arg); \
    asm volatile("" : "+g"(Arg) : : "memory");                         \
    Arg;                                                               \
  })

void __defer(struct StackFrame *, void *, void *) hidden paramsnonnull((1, 2));

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_GC_H_ */
