#ifndef COSMOPOLITAN_LIBC_THREAD_NATIVESEM_H_
#define COSMOPOLITAN_LIBC_THREAD_NATIVESEM_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/**
 * @fileoverview native semaphore for implementation details
 */

typedef union cthread_native_sem_t {
  struct {
    uint64_t count;
  } linux;
} cthread_native_sem_t;

struct timespec;

int cthread_native_sem_init(cthread_native_sem_t*, int);
int cthread_native_sem_destroy(cthread_native_sem_t*);

int cthread_native_sem_wait(cthread_native_sem_t*, int, int, const struct timespec*);
int cthread_native_sem_signal(cthread_native_sem_t*);


COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_THREAD_SELF_H_ */ 
 
