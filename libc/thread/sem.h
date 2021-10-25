#ifndef COSMOPOLITAN_LIBC_THREAD_NATIVESEM_H_
#define COSMOPOLITAN_LIBC_THREAD_NATIVESEM_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/**
 * @fileoverview native semaphore for implementation details
 */

typedef union cthread_sem_t {
  struct {
    uint64_t count;
  } linux;
} cthread_sem_t;

struct timespec;

int cthread_sem_init(cthread_sem_t*, int);
int cthread_sem_destroy(cthread_sem_t*);

int cthread_sem_wait(cthread_sem_t*, int, const struct timespec*);
int cthread_sem_signal(cthread_sem_t*);


COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_THREAD_SELF_H_ */ 
 
