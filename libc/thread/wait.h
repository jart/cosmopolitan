#ifndef COSMOPOLITAN_LIBC_THREAD_WAIT_H_
#define COSMOPOLITAN_LIBC_THREAD_WAIT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/**
 * @fileoverview wait on memory
 */
struct timespec;

int cthread_memory_wait32(uint32_t*, uint32_t, const struct timespec*);
int cthread_memory_wake32(uint32_t*, int);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_THREAD_WAIT_H_ */
