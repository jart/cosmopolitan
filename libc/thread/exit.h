#ifndef COSMOPOLITAN_LIBC_THREAD_EXIT_H_
#define COSMOPOLITAN_LIBC_THREAD_EXIT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/**
 * @fileoverview exit the current thread
 */

wontreturn void cthread_exit(int);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_THREAD_EXIT_H_ */
