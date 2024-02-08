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
#include "libc/errno.h"
#include "libc/intrin/dll.h"
#include "libc/runtime/runtime.h"
#include "third_party/nsync/atomic.h"
#include "third_party/nsync/atomic.internal.h"
#include "third_party/nsync/common.internal.h"
#include "third_party/nsync/mu_semaphore.h"
#include "third_party/nsync/wait_s.internal.h"

asm(".ident\t\"\\n\\n\
*NSYNC (Apache 2.0)\\n\
Copyright 2016 Google, Inc.\\n\
https://github.com/google/nsync\"");

/* Wait until one of:
     w->sem is non-zero----decrement it and return 0.
     abs_deadline expires---return ETIMEDOUT.
     cancel_note is non-NULL and *cancel_note becomes notified---return ECANCELED. */
int nsync_sem_wait_with_cancel_ (waiter *w, nsync_time abs_deadline,
			         nsync_note cancel_note) {
	int sem_outcome;
	if (cancel_note == NULL) {
		sem_outcome = nsync_mu_semaphore_p_with_deadline (&w->sem, abs_deadline);
	} else {
		nsync_time cancel_time;
		cancel_time = nsync_note_notified_deadline_ (cancel_note);
		sem_outcome = ECANCELED;
		if (nsync_time_cmp (cancel_time, nsync_time_zero) > 0) {
			struct nsync_waiter_s nw;
			nw.tag = NSYNC_WAITER_TAG;
			nw.sem = &w->sem;
			dll_init (&nw.q);
			ATM_STORE (&nw.waiting, 1);
			nw.flags = 0;
			nsync_mu_lock (&cancel_note->note_mu);
			cancel_time = NOTIFIED_TIME (cancel_note);
			if (nsync_time_cmp (cancel_time, nsync_time_zero) > 0) {
				nsync_time local_abs_deadline;
				int deadline_is_nearer = 0;
				dll_make_last (&cancel_note->waiters, &nw.q);
				local_abs_deadline = cancel_time;
				if (nsync_time_cmp (abs_deadline, cancel_time) < 0) {
					local_abs_deadline = abs_deadline;
					deadline_is_nearer = 1;
				}
				nsync_mu_unlock (&cancel_note->note_mu);
				sem_outcome = nsync_mu_semaphore_p_with_deadline (&w->sem,
					local_abs_deadline);
				if (sem_outcome == ETIMEDOUT && !deadline_is_nearer) {
					sem_outcome = ECANCELED;
					nsync_note_notify (cancel_note);
				}
				nsync_mu_lock (&cancel_note->note_mu);
				cancel_time = NOTIFIED_TIME (cancel_note);
				if (nsync_time_cmp (cancel_time,
						    nsync_time_zero) > 0) {
					dll_remove (&cancel_note->waiters, &nw.q);
				}
			}
			nsync_mu_unlock (&cancel_note->note_mu);
		}
	}
	return (sem_outcome);
}
