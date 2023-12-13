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
#include "libc/calls/blockcancel.internal.h"
#include "libc/mem/mem.h"
#include "libc/thread/thread.h"
#include "third_party/nsync/atomic.h"
#include "third_party/nsync/atomic.internal.h"
#include "third_party/nsync/common.internal.h"
#include "third_party/nsync/mu_semaphore.h"
#include "third_party/nsync/races.internal.h"
#include "third_party/nsync/wait_s.internal.h"
#include "third_party/nsync/waiter.h"

asm(".ident\t\"\\n\\n\
*NSYNC (Apache 2.0)\\n\
Copyright 2016 Google, Inc.\\n\
https://github.com/google/nsync\"");

int nsync_wait_n (void *mu, void (*lock) (void *), void (*unlock) (void *),
		  nsync_time abs_deadline,
		  int count, struct nsync_waitable_s *waitable[]) {
	int ready;
	IGNORE_RACES_START ();
	BLOCK_CANCELATION;
	for (ready = 0; ready != count &&
			nsync_time_cmp ((*waitable[ready]->funcs->ready_time) (
						waitable[ready]->v, NULL),
					nsync_time_zero) > 0;
	     ready++) {
	}
	if (ready == count && nsync_time_cmp (abs_deadline, nsync_time_zero) > 0) {
		int i;
		int unlocked = 0;
		int j;
		int enqueued = 1;
		waiter *w = nsync_waiter_new_ ();
		struct nsync_waiter_s nw_set[4];
		struct nsync_waiter_s *nw = nw_set;
		if (count > (int) (sizeof (nw_set) / sizeof (nw_set[0]))) {
			nw = (struct nsync_waiter_s *) malloc (count * sizeof (nw[0]));
		}
		for (i = 0; i != count && enqueued; i++) {
			nw[i].tag = NSYNC_WAITER_TAG;
			nw[i].sem = &w->sem;
			dll_init (&nw[i].q);
			ATM_STORE (&nw[i].waiting, 0);
			nw[i].flags = 0;
			enqueued = (*waitable[i]->funcs->enqueue) (waitable[i]->v, &nw[i]);
		}

		if (i == count) {
			nsync_time min_ntime;
			if (mu != NULL) {
				(*unlock) (mu);
				unlocked = 1;
			}
			do {
				min_ntime = abs_deadline;
				for (j = 0; j != count; j++) {
					nsync_time ntime;
					ntime = (*waitable[j]->funcs->ready_time) (
						waitable[j]->v, &nw[j]);
					if (nsync_time_cmp (ntime, min_ntime) < 0) {
						min_ntime = ntime;
					}
				}
			} while (nsync_time_cmp (min_ntime, nsync_time_zero) > 0 &&
				 nsync_mu_semaphore_p_with_deadline (&w->sem,
					min_ntime) == 0);
		}

		/* An attempt was made above to enqueue waitable[0..i-1].
                   Dequeue any that are still enqueued, and remember the index
                   of the first ready (i.e., not still enqueued) object, if any.  */
		for (j = 0; j != i; j++) {
			int was_still_enqueued =
				(*waitable[j]->funcs->dequeue) (waitable[j]->v, &nw[j]);
			if (!was_still_enqueued && ready == count) {
				ready = j;
			}
		}

		if (nw != nw_set) {
			free (nw);
		}
		nsync_waiter_free_ (w);
		if (unlocked) {
			(*lock) (mu);
		}
	}
	ALLOW_CANCELATION;
	IGNORE_RACES_END ();
	return (ready);
}
