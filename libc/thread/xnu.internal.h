#ifndef COSMOPOLITAN_LIBC_THREAD_XNU_INTERNAL_H_
#define COSMOPOLITAN_LIBC_THREAD_XNU_INTERNAL_H_
COSMOPOLITAN_C_START_

/*
 * XNU thread system calls.
 * See darwin-libpthread/kern/kern_support.c
 */

errno_t sys_clone_xnu(void *func, void *func_arg, void *stack, void *pthread,
                      uint32_t flags);
int sys_bsdthread_register(
    void (*threadstart)(void *pthread, int machport, void *(*func)(void *),
                        void *arg, intptr_t *, unsigned),
    void (*wqthread)(void *pthread, void *machport, void *, void *, int),
    uint32_t flags, void *stack_addr_hint, void *targetconc_ptr,
    uint32_t dispatchqueue_offset, uint32_t tsd_offset);

int bsdthread_terminate(void *stackaddr, size_t freesize, uint32_t port,
                        uint32_t sem);
int __pthread_kill(uint32_t port, int sig);
int bsdthread_ctl(void *cmd, void *arg1, void *arg2, void *arg3);
uint64_t thread_selfid(void);
uint64_t thread_selfusage(void);
int thread_selfcounts(int type, void *buf, uint64_t nbytes);
int thread_fast_set_cthread_self(void *tls);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_THREAD_XNU_INTERNAL_H_ */
