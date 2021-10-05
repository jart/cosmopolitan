#ifndef COSMOPOLITAN_LIBC_THREAD_YIELD_H_
#define COSMOPOLITAN_LIBC_THREAD_YIELD_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/**
 * @fileoverview yield thread to OS scheduler
 */

int cthread_yield(void);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_THREAD_YIELD_H_ */ 
