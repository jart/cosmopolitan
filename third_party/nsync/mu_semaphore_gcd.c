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
#include "libc/calls/sig.internal.h"
#include "libc/errno.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/runtime/clktck.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/str/str.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#include "third_party/nsync/atomic.h"
#include "third_party/nsync/atomic.internal.h"
#include "third_party/nsync/futex.internal.h"
#include "third_party/nsync/mu_semaphore.internal.h"
#include "third_party/nsync/time.h"

/**
 * @fileoverview Semaphores w/ Apple's Grand Central Dispatch API.
 */

#define DISPATCH_TIME_FOREVER ~0ull

static dispatch_semaphore_t dispatch_semaphore_create(long count) {
	dispatch_semaphore_t ds;
	ds = __syslib->__dispatch_semaphore_create (count);
	STRACE ("dispatch_semaphore_create(%ld) → %#lx", count, ds);
	return (ds);
}

static void dispatch_release (dispatch_semaphore_t ds) {
	__syslib->__dispatch_release (ds);
	STRACE ("dispatch_release(%#lx)", ds);
}

static long dispatch_semaphore_wait (dispatch_semaphore_t ds,
				     dispatch_time_t dt) {
	long rc = __syslib->__dispatch_semaphore_wait (ds, dt);
	STRACE ("dispatch_semaphore_wait(%#lx, %ld) → %ld", ds, dt, rc);
	return (rc);
}

static long dispatch_semaphore_signal (dispatch_semaphore_t ds) {
	long rc = __syslib->__dispatch_semaphore_signal (ds);
	(void)rc;
	STRACE ("dispatch_semaphore_signal(%#lx) → %ld", ds, rc);
	return (ds);
}

static dispatch_time_t dispatch_walltime (const struct timespec *base,
					  int64_t offset) {
	return __syslib->__dispatch_walltime (base, offset);
}

static errno_t nsync_dispatch_semaphore_wait (nsync_semaphore *s,
					      nsync_time abs_deadline) {
	errno_t result = 0;
	dispatch_time_t dt;
	if (nsync_time_cmp (abs_deadline, nsync_time_no_deadline) == 0) {
		dt = DISPATCH_TIME_FOREVER;
	} else {
		dt = dispatch_walltime (&abs_deadline, 0);
	}
	if (dispatch_semaphore_wait (*(dispatch_semaphore_t *)s, dt) != 0) {
		result = ETIMEDOUT;
	}
	return (result);
}

/* Initialize *s; the initial value is 0.  */
void nsync_mu_semaphore_init_gcd (nsync_semaphore *s) {
	*(dispatch_semaphore_t *)s = dispatch_semaphore_create (0);
}

/* Wait until the count of *s exceeds 0, and decrement it. If POSIX cancellations
   are currently disabled by the thread, then this function always succeeds. When
   they're enabled in MASKED mode, this function may return ECANCELED. Otherwise,
   cancellation will occur by unwinding cleanup handlers pushed to the stack. */
errno_t nsync_mu_semaphore_p_gcd (nsync_semaphore *s) {
	return nsync_mu_semaphore_p_with_deadline_gcd (s, nsync_time_no_deadline);
}

/* Like nsync_mu_semaphore_p() this waits for the count of *s to exceed 0,
   while additionally supporting a time parameter specifying at what point
   in the future ETIMEDOUT should be returned, if neither cancellation, or
   semaphore release happens. */
errno_t nsync_mu_semaphore_p_with_deadline_gcd (nsync_semaphore *s,
						nsync_time abs_deadline) {
	errno_t result = 0;
	struct PosixThread *pt;
	if (!__tls_enabled ||
	    !_weaken (pthread_testcancel_np) ||
	    !(pt = _pthread_self()) ||
	    (pt->pt_flags & PT_NOCANCEL)) {
		result = nsync_dispatch_semaphore_wait (s, abs_deadline);
	} else {
		struct timespec now, until, slice = {0, 1000000000 / CLK_TCK};
		for (;;) {
			if (_weaken (pthread_testcancel_np) () == ECANCELED) {
				result = ECANCELED;
				break;
			}
			now = timespec_real();
			if (timespec_cmp (now, abs_deadline) >= 0) {
				result = ETIMEDOUT;
				break;
			}
			until = timespec_add (now, slice);
			if (timespec_cmp (until, abs_deadline) > 0) {
				until = abs_deadline;
			}
			if (!nsync_dispatch_semaphore_wait (s, until)) {
				break;
			}
		}
	}
	return (result);
}

/* Ensure that the count of *s is at least 1. */
void nsync_mu_semaphore_v_gcd (nsync_semaphore *s) {
	dispatch_semaphore_signal (*(dispatch_semaphore_t *)s);
}
