#ifndef COSMOPOLITAN_LIBC_THREAD_XNU_INTERNAL_H_
#define COSMOPOLITAN_LIBC_THREAD_XNU_INTERNAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/**
 * XNU thread system calls.
 * @see darwin-libpthread/kern/kern_support.c
 */

void *bsdthread_create(void *func, void *func_arg, void *stack, void *pthread,
                       uint32_t flags);
int bsdthread_terminate(void *stackaddr, size_t freesize, uint32_t port,
                        uint32_t sem);
int bsdthread_register(void *threadstart, void *wqthread, uint32_t flags,
                       void *stack_addr_hint, void *targetconc_ptr,
                       uint32_t dispatchqueue_offset, uint32_t tsd_offset);
int bsdthread_ctl(void *cmd, void *arg1, void *arg2, void *arg3);
uint64_t thread_selfid(void);
uint64_t thread_selfusage(void);
int thread_selfcounts(int type, void *buf, uint64_t nbytes);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_THREAD_XNU_INTERNAL_H_ */
