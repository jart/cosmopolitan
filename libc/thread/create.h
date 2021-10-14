#ifndef COSMOPOLITAN_LIBC_THREAD_CREATE_H_
#define COSMOPOLITAN_LIBC_THREAD_CREATE_H_
#include "libc/thread/attr.h"
#include "libc/thread/descriptor.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/**
 * @fileoverview Create a cosmopolitan thread
 */

int cthread_create(cthread_t* restrict, const cthread_attr_t* restrict,
                   int (*)(void*), void* restrict);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_THREAD_CREATE_H_ */
