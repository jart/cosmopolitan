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
#include "libc/dce.h"
#include "third_party/nsync/mu_semaphore.h"
#include "third_party/nsync/mu_semaphore.internal.h"

asm(".ident\t\"\\n\\n\
*NSYNC (Apache 2.0)\\n\
Copyright 2016 Google, Inc.\\n\
https://github.com/google/nsync\"");
// clang-format off

/* Initialize *s; the initial value is 0. */
void nsync_mu_semaphore_init (nsync_semaphore *s) {
	if (IsNetbsd ()) {
		return nsync_mu_semaphore_init_sem (s);
	} else {
		return nsync_mu_semaphore_init_futex (s);
	}
}

/* Wait until the count of *s exceeds 0, and decrement it. */
errno_t nsync_mu_semaphore_p (nsync_semaphore *s) {
	if (IsNetbsd ()) {
		return nsync_mu_semaphore_p_sem (s);
	} else {
		return nsync_mu_semaphore_p_futex (s);
	}
}

/* Wait until one of:
   the count of *s is non-zero, in which case decrement *s and return 0;
   or abs_deadline expires, in which case return ETIMEDOUT. */
errno_t nsync_mu_semaphore_p_with_deadline (nsync_semaphore *s, nsync_time abs_deadline) {
	if (IsNetbsd ()) {
		return nsync_mu_semaphore_p_with_deadline_sem (s, abs_deadline);
	} else {
		return nsync_mu_semaphore_p_with_deadline_futex (s, abs_deadline);
	}
}

/* Ensure that the count of *s is at least 1. */
void nsync_mu_semaphore_v (nsync_semaphore *s) {
	if (IsNetbsd ()) {
		return nsync_mu_semaphore_v_sem (s);
	} else {
		return nsync_mu_semaphore_v_futex (s);
	}
}
