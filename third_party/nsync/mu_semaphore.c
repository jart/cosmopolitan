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
#include "third_party/nsync/mu_semaphore.h"
#include "libc/calls/cp.internal.h"
#include "libc/dce.h"
#include "third_party/nsync/mu_semaphore.internal.h"

/* Apple's ulock (part by Cosmo futexes) is an internal API, but:
   1. Unlike GCD it's cancellable, i.e. can be EINTR'd by signals
   2. We currently always use ulock anyway for joining threads */
#define PREFER_GCD_OVER_ULOCK 1

asm(".ident\t\"\\n\\n\
*NSYNC (Apache 2.0)\\n\
Copyright 2016 Google, Inc.\\n\
https://github.com/google/nsync\"");

/* Initialize *s; the initial value is 0. */
void nsync_mu_semaphore_init (nsync_semaphore *s) {
	if (PREFER_GCD_OVER_ULOCK && IsXnuSilicon ()) {
		return nsync_mu_semaphore_init_gcd (s);
	} else if (IsNetbsd ()) {
		return nsync_mu_semaphore_init_sem (s);
	} else {
		return nsync_mu_semaphore_init_futex (s);
	}
}

/* Wait until the count of *s exceeds 0, and decrement it. If POSIX cancellations
   are currently disabled by the thread, then this function always succeeds. When
   they're enabled in MASKED mode, this function may return ECANCELED. Otherwise,
   cancellation will occur by unwinding cleanup handlers pushed to the stack. */
errno_t nsync_mu_semaphore_p (nsync_semaphore *s) {
	errno_t err;
	BEGIN_CANCELATION_POINT;
	if (PREFER_GCD_OVER_ULOCK && IsXnuSilicon ()) {
		err = nsync_mu_semaphore_p_gcd (s);
	} else if (IsNetbsd ()) {
		err = nsync_mu_semaphore_p_sem (s);
	} else {
		err = nsync_mu_semaphore_p_futex (s);
	}
	END_CANCELATION_POINT;
	return err;
}

/* Like nsync_mu_semaphore_p() this waits for the count of *s to exceed 0,
   while additionally supporting a time parameter specifying at what point
   in the future ETIMEDOUT should be returned, if neither cancelation, or
   semaphore release happens. */
errno_t nsync_mu_semaphore_p_with_deadline (nsync_semaphore *s, nsync_time abs_deadline) {
	errno_t err;
	BEGIN_CANCELATION_POINT;
	if (PREFER_GCD_OVER_ULOCK && IsXnuSilicon ()) {
		err = nsync_mu_semaphore_p_with_deadline_gcd (s, abs_deadline);
	} else if (IsNetbsd ()) {
		err = nsync_mu_semaphore_p_with_deadline_sem (s, abs_deadline);
	} else {
		err = nsync_mu_semaphore_p_with_deadline_futex (s, abs_deadline);
	}
	END_CANCELATION_POINT;
	return err;
}

/* Ensure that the count of *s is at least 1. */
void nsync_mu_semaphore_v (nsync_semaphore *s) {
	if (PREFER_GCD_OVER_ULOCK && IsXnuSilicon ()) {
		return nsync_mu_semaphore_v_gcd (s);
	} else if (IsNetbsd ()) {
		return nsync_mu_semaphore_v_sem (s);
	} else {
		return nsync_mu_semaphore_v_futex (s);
	}
}
