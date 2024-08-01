#ifndef COSMOPOLITAN_THIRD_PARTY_NSYNC_MU_SEMAPHORE_INTERNAL_H_
#define COSMOPOLITAN_THIRD_PARTY_NSYNC_MU_SEMAPHORE_INTERNAL_H_
#include "third_party/nsync/mu_semaphore.h"
#include "third_party/nsync/time.h"
COSMOPOLITAN_C_START_

/* XNU ulock (used by cosmo futexes) is an internal API, however:

     1. Unlike GCD it's cancelable i.e. can be EINTR'd by signals
     2. We have no choice but to use ulock for joining threads
     3. Grand Central Dispatch requires a busy loop workaround
     4. ulock makes our mutexes use 20% more system time (meh)
     5. ulock makes our mutexes use 40% less wall time (good)
     6. ulock makes our mutexes use 64% less user time (woop)

   ulock is an outstanding system call that must be used.
   gcd is not an acceptable alternative to ulock. */

#define NSYNC_USE_GRAND_CENTRAL 0

bool nsync_mu_semaphore_init_futex(nsync_semaphore *);
errno_t nsync_mu_semaphore_p_futex(nsync_semaphore *);
errno_t nsync_mu_semaphore_p_with_deadline_futex(nsync_semaphore *, nsync_time);
void nsync_mu_semaphore_v_futex(nsync_semaphore *);

bool nsync_mu_semaphore_init_sem(nsync_semaphore *);
errno_t nsync_mu_semaphore_p_sem(nsync_semaphore *);
errno_t nsync_mu_semaphore_p_with_deadline_sem(nsync_semaphore *, nsync_time);
void nsync_mu_semaphore_v_sem(nsync_semaphore *);

bool nsync_mu_semaphore_init_gcd(nsync_semaphore *);
errno_t nsync_mu_semaphore_p_gcd(nsync_semaphore *);
errno_t nsync_mu_semaphore_p_with_deadline_gcd(nsync_semaphore *, nsync_time);
void nsync_mu_semaphore_v_gcd(nsync_semaphore *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_NSYNC_MU_SEMAPHORE_INTERNAL_H_ */
