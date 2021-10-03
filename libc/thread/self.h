#ifndef COSMOPOLITAN_LIBC_THREAD_SELF_H_
#define COSMOPOLITAN_LIBC_THREAD_SELF_H_
#include "libc/thread/descriptor.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/**
 * @fileoverview get the thread descriptor of the current thread
 */

inline cthread_t cthread_self(void) {
  cthread_t self;
  asm ("mov %%fs:0, %0" : "=r"(self));
  return self;
}


COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_THREAD_SELF_H_ */ 
