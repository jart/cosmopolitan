#ifndef COSMOPOLITAN_LIBC_THREAD_SELF_H_
#define COSMOPOLITAN_LIBC_THREAD_SELF_H_
#include "libc/thread/descriptor.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/**
 * @fileoverview get the thread descriptor of the current thread
 */

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
inline cthread_t cthread_self(void) {
  cthread_t self;
  asm("mov\t%%fs:0,%0" : "=r"(self));
  return self;
}
#else
cthread_t cthread_self(void);
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_THREAD_SELF_H_ */
