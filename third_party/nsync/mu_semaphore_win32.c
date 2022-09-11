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
#include "libc/errno.h"
#include "libc/nt/enum/wait.h"
#include "libc/nt/synchronization.h"
#include "libc/runtime/runtime.h"
#include "third_party/nsync/mu_semaphore.h"
#include "third_party/nsync/mu_semaphore.internal.h"
#include "third_party/nsync/time.h"

asm(".ident\t\"\\n\\n\
*NSYNC (Apache 2.0)\\n\
Copyright 2016 Google, Inc.\\n\
https://github.com/google/nsync\"");
// clang-format off

/* Initialize *s; the initial value is 0. */
void nsync_mu_semaphore_init_win32 (nsync_semaphore *s) {
	int64_t *h = (int64_t *) s;
	*h = CreateSemaphore(NULL, 0, 1, NULL);
	if (!*h) notpossible;
}

/* Wait until the count of *s exceeds 0, and decrement it. */
void nsync_mu_semaphore_p_win32 (nsync_semaphore *s) {
	int64_t *h = (int64_t *) s;
	WaitForSingleObject(*h, -1u);
}

/* Wait until one of:
   the count of *s is non-zero, in which case decrement *s and return 0;
   or abs_deadline expires, in which case return ETIMEDOUT. */
int nsync_mu_semaphore_p_with_deadline_win32 (nsync_semaphore *s, nsync_time abs_deadline) {
	int64_t *h = (int64_t *) s;
	int result;

	if (nsync_time_cmp (abs_deadline, nsync_time_no_deadline) == 0) {
		result = WaitForSingleObject(*h, -1u);
	} else {
		nsync_time now;
		now = nsync_time_now ();
		do {
			if (nsync_time_cmp (abs_deadline, now) <= 0) {
				result = WaitForSingleObject (*h, 0);
			} else {
				nsync_time delay;
				delay = nsync_time_sub (abs_deadline, now);
				if (NSYNC_TIME_SEC (delay) > 1000*1000) {
					result = WaitForSingleObject (*h, 1000*1000);
				} else {
					result = WaitForSingleObject (*h,
						(unsigned) (NSYNC_TIME_SEC (delay) * 1000 +
							(NSYNC_TIME_NSEC (delay) + 999999) / (1000 * 1000)));
				}
			}
			if (result == kNtWaitTimeout) {
				now = nsync_time_now ();
			}
		} while (result == kNtWaitTimeout && /* Windows generates early wakeups. */
			 nsync_time_cmp (abs_deadline, now) > 0);
	}
	return (result == kNtWaitTimeout ? ETIMEDOUT : 0);
}

/* Ensure that the count of *s is at least 1. */
void nsync_mu_semaphore_v_win32 (nsync_semaphore *s) {
	int64_t *h = (int64_t *) s;
	ReleaseSemaphore(*h, 1, NULL);
}
