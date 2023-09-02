/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│vi: set et ft=c ts=8 tw=8 fenc=utf-8                                       :vi│
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
#include "libc/intrin/strace.internal.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/str/str.h"
#include "libc/thread/thread.h"
#include "third_party/nsync/atomic.h"
#include "third_party/nsync/atomic.internal.h"
#include "third_party/nsync/futex.internal.h"
#include "third_party/nsync/mu_semaphore.internal.h"
#include "third_party/nsync/time.h"
// clang-format off

/**
 * @fileoverview Semaphores w/ Apple's Grand Central Dispatch API.
 */

#define DISPATCH_TIME_FOREVER ~0ull

static dispatch_semaphore_t dispatch_semaphore_create(long count) {
	dispatch_semaphore_t ds;
	ds = __syslib->dispatch_semaphore_create (count);
	STRACE ("dispatch_semaphore_create(%ld) → %#lx", count, ds);
	return (ds);
}

static long dispatch_semaphore_wait (dispatch_semaphore_t ds,
				     dispatch_time_t dt) {
	long rc = __syslib->dispatch_semaphore_wait (ds, dt);
	STRACE ("dispatch_semaphore_wait(%#lx, %ld) → %ld", ds, dt, rc);
	return (rc);
}

static long dispatch_semaphore_signal (dispatch_semaphore_t ds) {
	long rc = __syslib->dispatch_semaphore_signal (ds);
	(void)rc;
	STRACE ("dispatch_semaphore_signal(%#lx) → %ld", ds, rc);
	return (ds);
}

static dispatch_time_t dispatch_walltime (const struct timespec *base,
					  int64_t offset) {
	return __syslib->dispatch_walltime (base, offset);
}

/* Initialize *s; the initial value is 0.  */
void nsync_mu_semaphore_init_gcd (nsync_semaphore *s) {
	*(dispatch_semaphore_t *)s = dispatch_semaphore_create (0);
}

/* Wait until the count of *s exceeds 0, and decrement it. */
errno_t nsync_mu_semaphore_p_gcd (nsync_semaphore *s) {
	dispatch_semaphore_wait (*(dispatch_semaphore_t *)s,
				 DISPATCH_TIME_FOREVER);
	return (0);
}

/* Wait until one of:
   the count of *s is non-zero, in which case decrement *s and return 0;
   or abs_deadline expires, in which case return ETIMEDOUT. */
errno_t nsync_mu_semaphore_p_with_deadline_gcd (nsync_semaphore *s,
						nsync_time abs_deadline) {
	errno_t result = 0;
	if (nsync_time_cmp (abs_deadline, nsync_time_no_deadline) == 0) {
		dispatch_semaphore_wait (*(dispatch_semaphore_t *)s,
					 DISPATCH_TIME_FOREVER);
	} else {
		struct timespec ts;
		bzero (&ts, sizeof (ts));
		ts.tv_sec = NSYNC_TIME_SEC (abs_deadline);
		ts.tv_nsec = NSYNC_TIME_NSEC (abs_deadline);
		if (dispatch_semaphore_wait (*(dispatch_semaphore_t *)s,
					     dispatch_walltime (&abs_deadline, 0)) != 0) {
			result = ETIMEDOUT;
		}
	}
	return (result);
}

/* Ensure that the count of *s is at least 1. */
void nsync_mu_semaphore_v_gcd (nsync_semaphore *s) {
	dispatch_semaphore_signal (*(dispatch_semaphore_t *)s);
}
