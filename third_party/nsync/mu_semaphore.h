#ifndef NSYNC_SEM_H_
#define NSYNC_SEM_H_
#include "third_party/nsync/time.h"
COSMOPOLITAN_C_START_

typedef struct nsync_semaphore_s_ {
  void *sem_space[3];
} nsync_semaphore;

/* Initialize *s; the initial value is 0. */
void nsync_mu_semaphore_init(nsync_semaphore *s);

/* Wait until the count of *s exceeds 0, and decrement it. */
errno_t nsync_mu_semaphore_p(nsync_semaphore *s);

/* Wait until one of: the count of *s is non-zero, in which case
   decrement *s and return 0; or abs_deadline expires, in which case
   return ETIMEDOUT. */
errno_t nsync_mu_semaphore_p_with_deadline(nsync_semaphore *s,
                                           nsync_time abs_deadline);

/* Ensure that the count of *s is at least 1. */
void nsync_mu_semaphore_v(nsync_semaphore *s);

COSMOPOLITAN_C_END_
#endif /* NSYNC_SEM_H_ */
