// clang-format off
/* Copyright 2016 Google Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License. */

#include "libc/thread/headers.h"

NSYNC_CPP_START_

static int futex (int *uaddr, int op, int val, const struct timespec *timeout, int *uaddr2,
		  int val3) {
	return (syscall (__NR_futex, uaddr, op, val, timeout, uaddr2, val3));
}

/* Check that atomic operations on nsync_atomic_uint32_ can be applied to int. */
static const int assert_int_size = 1 /
	(sizeof (assert_int_size) == sizeof (uint32_t) &&
	 sizeof (nsync_atomic_uint32_) == sizeof (uint32_t));

#if defined(FUTEX_PRIVATE_FLAG)
#define FUTEX_PRIVATE_FLAG_ FUTEX_PRIVATE_FLAG
#else
#define FUTEX_PRIVATE_FLAG_ 0
#endif

#if defined(FUTEX_WAIT_BITSET)
#define FUTEX_WAIT_ (FUTEX_WAIT_BITSET | FUTEX_PRIVATE_FLAG_ | FUTEX_CLOCK_REALTIME)
#define FUTEX_WAIT_BITS_ FUTEX_BITSET_MATCH_ANY
#else
#define FUTEX_WAIT_ (FUTEX_WAIT | FUTEX_PRIVATE_FLAG_)
#define FUTEX_WAIT_BITS_ 0
#endif
#define FUTEX_WAKE_ (FUTEX_WAKE | FUTEX_PRIVATE_FLAG_)
#define FUTEX_TIMEOUT_IS_ABSOLUTE (FUTEX_WAIT_BITS_ != 0)

#define ASSERT(x) do { if (!(x)) { *(volatile int *)0 = 0; } } while (0)

struct futex {
	int i;  /* lo half=count; hi half=waiter count */
};

static nsync_semaphore *sem_big_enough_for_futex = (nsync_semaphore *) (uintptr_t)(1 /
	(sizeof (struct futex) <= sizeof (*sem_big_enough_for_futex)));

/* Initialize *s; the initial value is 0. */
void nsync_mu_semaphore_init (nsync_semaphore *s) {
	struct futex *f = (struct futex *) s;
	f->i = 0;
}

/* Wait until the count of *s exceeds 0, and decrement it. */
void nsync_mu_semaphore_p (nsync_semaphore *s) {
	struct futex *f = (struct futex *) s;
	int i;
	do {
		i = ATM_LOAD ((nsync_atomic_uint32_ *) &f->i);
		if (i == 0) {
                        int futex_result = futex (&f->i, FUTEX_WAIT_, i, NULL,
                                                  NULL, FUTEX_WAIT_BITS_);
			ASSERT (futex_result == 0 || errno == EINTR ||
				errno == EWOULDBLOCK);
		}
	} while (i == 0 || !ATM_CAS_ACQ ((nsync_atomic_uint32_ *) &f->i, i, i-1));
}

/* Wait until one of:
   the count of *s is non-zero, in which case decrement *s and return 0;
   or abs_deadline expires, in which case return ETIMEDOUT. */
int nsync_mu_semaphore_p_with_deadline (nsync_semaphore *s, nsync_time abs_deadline) {
	struct futex *f = (struct futex *)s;
	int i;
	int result = 0;
	do {
		i = ATM_LOAD ((nsync_atomic_uint32_ *) &f->i);
		if (i == 0) {
			int futex_result;
			struct timespec ts_buf;
			const struct timespec *ts = NULL;
			if (nsync_time_cmp (abs_deadline, nsync_time_no_deadline) != 0) {
				memset (&ts_buf, 0, sizeof (ts_buf));
				if (FUTEX_TIMEOUT_IS_ABSOLUTE) {
					ts_buf.tv_sec = NSYNC_TIME_SEC (abs_deadline);
					ts_buf.tv_nsec = NSYNC_TIME_NSEC (abs_deadline);
				} else {
					nsync_time now;
					now = nsync_time_now ();
					if (nsync_time_cmp (now, abs_deadline) > 0) {
						ts_buf.tv_sec = 0;
						ts_buf.tv_nsec = 0;
					} else {
						nsync_time rel_deadline;
						rel_deadline = nsync_time_sub (abs_deadline, now);
						ts_buf.tv_sec = NSYNC_TIME_SEC (rel_deadline);
						ts_buf.tv_nsec = NSYNC_TIME_NSEC (rel_deadline);
					}
				}
				ts = &ts_buf;
			}
			futex_result = futex (&f->i, FUTEX_WAIT_, i, ts, NULL, FUTEX_WAIT_BITS_);
			ASSERT (futex_result == 0 || errno == EINTR || errno == EWOULDBLOCK ||
				errno == ETIMEDOUT);
			/* Some systems don't wait as long as they are told. */ 
			if (futex_result == -1 && errno == ETIMEDOUT &&
			    nsync_time_cmp (abs_deadline, nsync_time_now ()) <= 0) {
				result = ETIMEDOUT;
			}
		}
	} while (result == 0 && (i == 0 || !ATM_CAS_ACQ ((nsync_atomic_uint32_ *) &f->i, i, i - 1)));
	return (result);
}

/* Ensure that the count of *s is at least 1. */
void nsync_mu_semaphore_v (nsync_semaphore *s) {
	struct futex *f = (struct futex *) s;
        uint32_t old_value;
        do {    
                old_value = ATM_LOAD ((nsync_atomic_uint32_ *) &f->i);
        } while (!ATM_CAS_REL ((nsync_atomic_uint32_ *) &f->i, old_value, old_value+1));
	ASSERT (futex (&f->i, FUTEX_WAKE_, 1, NULL, NULL, 0) >= 0);
}

NSYNC_CPP_END_
