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
#include "third_party/nsync/atomic.h"
#include "third_party/nsync/atomic.internal.h"
#include "third_party/nsync/common.internal.h"
#include "third_party/nsync/mu_semaphore.h"
#include "third_party/nsync/once.h"
#include "third_party/nsync/races.internal.h"
#include "third_party/nsync/wait_s.internal.h"

asm(".ident\t\"\\n\\n\
*NSYNC (Apache 2.0)\\n\
Copyright 2016 Google, Inc.\\n\
https://github.com/google/nsync\"");

/* An once_sync_s struct contains a lock, and a condition variable on which
   threads may wait for an nsync_once to be initialized by another thread.

   A separate struct is used only to keep nsync_once small.

   A given nsync_once can be associated with any once_sync_s struct, but cannot
   be associated with more than one.  nsync_once instances are mapped to
   once_sync_s instances by a trivial hashing scheme implemented by
   NSYNC_ONCE_SYNC_().

   The number of once_sync_s structs in the following array is greater than one
   only to reduce the probability of contention if a great many distinct
   nsync_once variables are initialized concurrently.  */
static struct once_sync_s {
	nsync_mu once_mu;
	nsync_cv once_cv;
} once_sync[64];

/* Return a pointer to the once_sync_s struct associated with the nsync_once *p. */
#define NSYNC_ONCE_SYNC_(p) &once_sync[(((uintptr_t) (p)) / sizeof (*(p))) % \
				       (sizeof (once_sync) / sizeof (once_sync[0]))]

/* Implement nsync_run_once, nsync_run_once_arg, nsync_run_once_spin, or
   nsync_run_once_arg_spin, chosen as described below.

   If s!=NULL, s is required to point to the once_sync_s associated with *once,
   and the semantics of nsync_run_once or nsync_run_once_arg are provided.
   If s==NULL, the semantics of nsync_run_once_spin, or nsync_run_once_arg_spin
   are provided.
   
   If f!=NULL, the semantics of nsync_run_once or nsync_run_once_spin are
   provided.  Otherwise, farg is required to be non-NULL, and the semantics of
   nsync_run_once_arg or nsync_run_once_arg_spin are provided.  */
static void nsync_run_once_impl (nsync_once *once, struct once_sync_s *s,
				 void (*f) (void), void (*farg) (void *arg), void *arg) {
	uint32_t o = ATM_LOAD_ACQ (once);
	if (o != 2) {
		unsigned attempts = 0;
		if (s != NULL) {
			nsync_mu_lock (&s->once_mu);
		}
		while (o == 0 && !ATM_CAS_ACQ (once, 0, 1)) {
			o = ATM_LOAD (once);
		}
		if (o == 0) {
			if (s != NULL) {
				nsync_mu_unlock (&s->once_mu);
			}
			if (f != NULL) {
				(*f) ();
			} else {
				(*farg) (arg);
			}
			if (s != NULL) {
				nsync_mu_lock (&s->once_mu);
				nsync_cv_broadcast (&s->once_cv);
			}
			ATM_STORE_REL (once, 2);
		}
		while (ATM_LOAD_ACQ (once) != 2) {
			if (s != NULL) {
				nsync_time deadline;
				if (attempts < 50) {
					attempts += 10;
				}
				deadline = nsync_time_add (nsync_time_now (), nsync_time_ms (attempts));
				nsync_cv_wait_with_deadline (&s->once_cv, &s->once_mu, deadline, NULL);
			} else {
				attempts = nsync_spin_delay_ (attempts);
			}
		}
		if (s != NULL) {
			nsync_mu_unlock (&s->once_mu);
		}
	}
}

void nsync_run_once (nsync_once *once, void (*f) (void)) {
	uint32_t o;
	IGNORE_RACES_START ();
	o = ATM_LOAD_ACQ (once);
	if (o != 2) {
		struct once_sync_s *s = NSYNC_ONCE_SYNC_ (once);
		nsync_run_once_impl (once, s, f, NULL, NULL);
	}
	IGNORE_RACES_END ();
}

void nsync_run_once_arg (nsync_once *once, void (*farg) (void *arg), void *arg) {
	uint32_t o;
	IGNORE_RACES_START ();
	o = ATM_LOAD_ACQ (once);
	if (o != 2) {
		struct once_sync_s *s = NSYNC_ONCE_SYNC_ (once);
		nsync_run_once_impl (once, s, NULL, farg, arg);
	}
	IGNORE_RACES_END ();
}

void nsync_run_once_spin (nsync_once *once, void (*f) (void)) {
	uint32_t o;
	IGNORE_RACES_START ();
	o = ATM_LOAD_ACQ (once);
	if (o != 2) {
		nsync_run_once_impl (once, NULL, f, NULL, NULL);
	}
	IGNORE_RACES_END ();
}

void nsync_run_once_arg_spin (nsync_once *once, void (*farg) (void *arg), void *arg) {
	uint32_t o;
	IGNORE_RACES_START ();
	o = ATM_LOAD_ACQ (once);
	if (o != 2) {
		nsync_run_once_impl (once, NULL, NULL, farg, arg);
	}
	IGNORE_RACES_END ();
}
