/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│ vi: set noet ft=c ts=8 sw=8 fenc=utf-8                                   :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2016 Google Inc.                                                   │
│                                                                              │
│ Licensed under the Apache License, Version 2.0 (the "License");              │
│ you may not use this file except in compliance with the License.             │
│ You may obtain a copy of the License at                                      │
│                                                                              │
│     http://www.apache.org/licenses/LICENSE-2.0                               │
│                                                                              │
│ Unless required by applicable law or agreed to in writing, software          │
│ distributed under the License is distributed on an "AS IS" BASIS,            │
│ WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.     │
│ See the License for the specific language governing permissions and          │
│ limitations under the License.                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/assert.h"
#include "libc/errno.h"
#include "libc/str/str.h"
#include "libc/thread/thread.h"
#include "third_party/nsync/atomic.h"
#include "third_party/nsync/atomic.internal.h"
#include "third_party/nsync/futex.internal.h"
#include "third_party/nsync/mu_semaphore.internal.h"

/**
 * @fileoverview Semaphores w/ Linux Futexes API.
 */

#define ASSERT(x) npassert(x)

/* Check that atomic operations on nsync_atomic_uint32_ can be applied to int. */
static const int assert_int_size = 1 /
	(sizeof (assert_int_size) == sizeof (uint32_t) &&
	 sizeof (nsync_atomic_uint32_) == sizeof (uint32_t));

struct futex {
	int i;  /* lo half=count; hi half=waiter count */
};

static nsync_semaphore *sem_big_enough_for_futex = (nsync_semaphore *) (uintptr_t)(1 /
	(sizeof (struct futex) <= sizeof (*sem_big_enough_for_futex)));

/* Initialize *s; the initial value is 0. */
void nsync_mu_semaphore_init_futex (nsync_semaphore *s) {
	struct futex *f = (struct futex *) s;
	f->i = 0;
}

/* Wait until the count of *s exceeds 0, and decrement it. If POSIX cancellations
   are currently disabled by the thread, then this function always succeeds. When
   they're enabled in MASKED mode, this function may return ECANCELED. Otherwise,
   cancellation will occur by unwinding cleanup handlers pushed to the stack. */
errno_t nsync_mu_semaphore_p_futex (nsync_semaphore *s) {
	struct futex *f = (struct futex *) s;
	int i;
	errno_t result = 0;
	do {
		i = ATM_LOAD ((nsync_atomic_uint32_ *) &f->i);
		if (i == 0) {
			int futex_result;
			futex_result = -nsync_futex_wait_ (
				(atomic_int *)&f->i, i,
				PTHREAD_PROCESS_PRIVATE, 0);
			ASSERT (futex_result == 0 ||
				futex_result == EINTR ||
				futex_result == EAGAIN ||
				futex_result == ECANCELED ||
				futex_result == EWOULDBLOCK);
			if (futex_result == ECANCELED) {
				result = ECANCELED;
			}
		}
	} while (result == 0 && (i == 0 || !ATM_CAS_ACQ ((nsync_atomic_uint32_ *) &f->i, i, i-1)));
	return result;
}

/* Like nsync_mu_semaphore_p() this waits for the count of *s to exceed 0,
   while additionally supporting a time parameter specifying at what point
   in the future ETIMEDOUT should be returned, if neither cancellation, or
   semaphore release happens. */
errno_t nsync_mu_semaphore_p_with_deadline_futex (nsync_semaphore *s, nsync_time abs_deadline) {
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
				bzero (&ts_buf, sizeof (ts_buf));
				ts_buf.tv_sec = NSYNC_TIME_SEC (abs_deadline);
				ts_buf.tv_nsec = NSYNC_TIME_NSEC (abs_deadline);
				ts = &ts_buf;
			}
			futex_result = nsync_futex_wait_ ((atomic_int *)&f->i, i,
							  PTHREAD_PROCESS_PRIVATE, ts);
			ASSERT (futex_result == 0 ||
				futex_result == -EINTR ||
				futex_result == -EAGAIN ||
				futex_result == -ECANCELED ||
				futex_result == -ETIMEDOUT ||
				futex_result == -EWOULDBLOCK);
			/* Some systems don't wait as long as they are told. */
			if (futex_result == -ETIMEDOUT &&
			    nsync_time_cmp (abs_deadline, nsync_time_now ()) <= 0) {
				result = ETIMEDOUT;
			}
			if (futex_result == -ECANCELED) {
				result = ECANCELED;
			}
		}
	} while (result == 0 && (i == 0 || !ATM_CAS_ACQ ((nsync_atomic_uint32_ *) &f->i, i, i - 1)));
	return (result);
}

/* Ensure that the count of *s is at least 1. */
void nsync_mu_semaphore_v_futex (nsync_semaphore *s) {
	struct futex *f = (struct futex *) s;
        uint32_t old_value;
	do {
		old_value = ATM_LOAD ((nsync_atomic_uint32_ *) &f->i);
	} while (!ATM_CAS_REL ((nsync_atomic_uint32_ *) &f->i, old_value, old_value+1));
	ASSERT (nsync_futex_wake_ ((atomic_int *)&f->i, 1, PTHREAD_PROCESS_PRIVATE) >= 0);
}
