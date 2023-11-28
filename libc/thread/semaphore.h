#ifndef COSMOPOLITAN_LIBC_CALLS_SEMAPHORE_H_
#define COSMOPOLITAN_LIBC_CALLS_SEMAPHORE_H_
#include "libc/calls/struct/timespec.h"
COSMOPOLITAN_C_START_

#define SEM_FAILED        ((sem_t *)0)
#define SEM_MAGIC_NAMED   0xDEADBEEFu
#define SEM_MAGIC_UNNAMED 0xFEEDABEEu
#define SEM_MAGIC_KERNEL  0xCAFEBABEu

typedef struct {
  union {
    struct {
      _Atomic(int) sem_value;
      _Atomic(int) sem_waiters;
      _Atomic(int) sem_prefs; /* named only */
      unsigned sem_magic;
      int64_t sem_dev;     /* named only */
      int64_t sem_ino;     /* named only */
      int sem_pid;         /* unnamed only */
      char sem_lazydelete; /* named only */
      char sem_pshared;
      int *sem_kernel;
    };
    void *sem_space[32];
  };
} sem_t;

int sem_init(sem_t *, int, unsigned);
int sem_destroy(sem_t *);
int sem_post(sem_t *);
int sem_wait(sem_t *);
int sem_trywait(sem_t *);
int sem_timedwait(sem_t *, const struct timespec *);
int sem_getvalue(sem_t *, int *);
sem_t *sem_open(const char *, int, ...);
int sem_close(sem_t *);
int sem_unlink(const char *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_SEMAPHORE_H_ */
