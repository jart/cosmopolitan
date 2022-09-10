#ifndef COSMOPOLITAN_LIBC_CALLS_SEMAPHORE_H_
#define COSMOPOLITAN_LIBC_CALLS_SEMAPHORE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define SEM_FAILED ((sem_t *)0)

typedef struct {
  volatile int __val[4 * sizeof(long) / sizeof(int)];
} sem_t;

int sem_close(sem_t *);
int sem_destroy(sem_t *);
int sem_getvalue(sem_t *, int *);
int sem_init(sem_t *, int, unsigned);
sem_t *sem_open(const char *, int, ...);
int sem_post(sem_t *);
int sem_timedwait(sem_t *, const struct timespec *);
int sem_trywait(sem_t *);
int sem_unlink(const char *);
int sem_wait(sem_t *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_SEMAPHORE_H_ */
