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
#include "libc/calls/calls.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/extend.internal.h"
#include "libc/nt/enum/filemapflags.h"
#include "libc/nt/enum/pageflags.h"
#include "libc/nt/memory.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/thread/tls.h"
#include "third_party/nsync/atomic.h"
#include "third_party/nsync/atomic.internal.h"
#include "third_party/nsync/common.internal.h"
#include "third_party/nsync/mu_semaphore.h"
#include "third_party/nsync/races.internal.h"
#include "third_party/nsync/wait_s.internal.h"

asm(".ident\t\"\\n\\n\
*NSYNC (Apache 2.0)\\n\
Copyright 2016 Google, Inc.\\n\
https://github.com/google/nsync\"");

/* This package provides a mutex nsync_mu and a Mesa-style condition
 * variable nsync_cv. */

/* Implementation notes

   The implementations of nsync_mu and nsync_cv both use spinlocks to protect
   their waiter queues.  The spinlocks are implemented with atomic operations
   and a delay loop found below.  They could use pthread_mutex_t, but I wished
   to have an implementation independent of pthread mutexes and condition
   variables.

   nsync_mu and nsync_cv use the same type of doubly-linked list of waiters
   (see waiter.c).  This allows waiters to be transferred from the cv queue to
   the mu queue when a thread is logically woken from the cv but would
   immediately go to sleep on the mu.  See the wake_waiters() call.

   In mu, the "designated waker" is a thread that was waiting on mu, has been
   woken up, but as yet has neither acquired nor gone back to waiting.  The
   presence of such a thread is indicated by the MU_DESIG_WAKER bit in the mu
   word.  This bit allows the nsync_mu_unlock() code to avoid waking a second
   waiter when there's already one that will wake the next thread when the time
   comes.  This speeds things up when the lock is heavily contended, and the
   critical sections are small.

   The weasel words "with high probability" in the specification of
   nsync_mu_trylock() and nsync_mu_rtrylock() prevent clients from believing
   that they can determine with certainty whether another thread has given up a
   lock yet.  This, together with the requirement that a thread that acquired a
   mutex must release it (rather than it being released by another thread),
   prohibits clients from using mu as a sort of semaphore.  The intent is that
   it be used only for traditional mutual exclusion, and that clients that need
   a semaphore should use one.  This leaves room for certain future
   optimizations, and make it easier to apply detection of potential races via
   candidate lock-set algorithms, should that ever be desired.

   The nsync_mu_wait_with_deadline() and nsync_mu_wait_with_deadline() calls use an
   absolute rather than a relative timeout.  This is less error prone, as
   described in the comment on nsync_cv_wait_with_deadline().  Alas, relative
   timeouts are seductive in trivial examples (such as tests).  These are the
   first things that people try, so they are likely to be requested.  If enough
   people complain we could give them that particular piece of rope.

   Excessive evaluations of the same wait condition are avoided by maintaining
   waiter.same_condition as a doubly-linked list of waiters with the same
   non-NULL wait condition that are also adjacent in the waiter list.  This does
   well even with large numbers of threads if there is at most one
   wait condition that can be false at any given time (such as in a
   producer/consumer queue, which cannot be both empty and full
   simultaneously).  One could imagine a queueing mechanism that would
   guarantee to evaluate each condition at most once per wakeup, but that would
   be substantially more complex, and would still degrade if the number of
   distinct wakeup conditions were high.  So clients are advised to resort to
   condition variables if they have many distinct wakeup conditions. */

/* Used in spinloops to delay resumption of the loop.
   Usage:
       unsigned attempts = 0;
       while (try_something) {
	  attempts = nsync_spin_delay_ (attempts);
       } */
unsigned nsync_spin_delay_ (unsigned attempts) {
	if (attempts < 7) {
		volatile int i;
		for (i = 0; i != 1 << attempts; i++) {
		}
		attempts++;
	} else {
		nsync_yield_ ();
	}
	return (attempts);
}

/* Spin until (*w & test) == 0, then atomically perform *w = ((*w | set) &
   ~clear), perform an acquire barrier, and return the previous value of *w.
   */
uint32_t nsync_spin_test_and_set_ (nsync_atomic_uint32_ *w, uint32_t test,
				   uint32_t set, uint32_t clear) {
	unsigned attempts = 0; /* CV_SPINLOCK retry count */
	uint32_t old = ATM_LOAD (w);
	while ((old & test) != 0 || !ATM_CAS_ACQ (w, old, (old | set) & ~clear)) {
		attempts = nsync_spin_delay_ (attempts);
		old = ATM_LOAD (w);
	}
	return (old);
}

/* ====================================================================================== */

struct nsync_waiter_s *nsync_dll_nsync_waiter_ (struct Dll *e) {
	struct nsync_waiter_s *nw = DLL_CONTAINER(struct nsync_waiter_s, q, e);
	ASSERT (nw->tag == NSYNC_WAITER_TAG);
	ASSERT (e == &nw->q);
	return (nw);
}

waiter *nsync_dll_waiter_ (struct Dll *e) {
	struct nsync_waiter_s *nw = DLL_NSYNC_WAITER (e);
	waiter *w = DLL_CONTAINER (waiter, nw, nw);
	ASSERT ((nw->flags & NSYNC_WAITER_FLAG_MUCV) != 0);
	ASSERT (w->tag == WAITER_TAG);
	ASSERT (e == &w->nw.q);
	return (w);
}

waiter *nsync_dll_waiter_samecond_ (struct Dll *e) {
	waiter *w = DLL_CONTAINER (struct waiter_s, same_condition, e);
	ASSERT (w->tag == WAITER_TAG);
	ASSERT (e == &w->same_condition);
	return (w);
}

/* -------------------------------- */

static struct {
	nsync_atomic_uint32_ mu;
	size_t used;
	char *p, *e;
} malloc;

static void *nsync_malloc (size_t size) {
	void *res = 0;
	nsync_spin_test_and_set_ (&malloc.mu, 1, 1, 0);
	if (malloc.p + malloc.used + size > malloc.e) {
		if (!malloc.p) {
			malloc.p = malloc.e = (char *)kMemtrackNsyncStart;
		}
		malloc.e = _extend (malloc.p, malloc.used + size, malloc.e, MAP_PRIVATE,
				    kMemtrackNsyncStart + kMemtrackNsyncSize);
		if (!malloc.e) {
			nsync_panic_ ("out of memory\n");
		}
	}
	res = malloc.p + malloc.used;
	malloc.used = (malloc.used + size + 15) & -16;
	ATM_STORE_REL (&malloc.mu, 0);
	return res;
}

/* -------------------------------- */

static struct Dll *free_waiters = NULL;

/* free_waiters points to a doubly-linked list of free waiter structs. */
static nsync_atomic_uint32_ free_waiters_mu; /* spinlock; protects free_waiters */

#define waiter_for_thread __get_tls()->tib_nsync

void nsync_waiter_destroy (void *v) {
	waiter *w = (waiter *) v;
	/* Reset waiter_for_thread in case another thread-local variable reuses
	   the waiter in its destructor while the waiter is taken by the other
	   thread from free_waiters. This can happen as the destruction order
	   of thread-local variables can be arbitrary in some platform e.g.
	   POSIX. */
	waiter_for_thread = NULL;
	IGNORE_RACES_START ();
	ASSERT ((w->flags & (WAITER_RESERVED|WAITER_IN_USE)) == WAITER_RESERVED);
	w->flags &= ~WAITER_RESERVED;
	nsync_spin_test_and_set_ (&free_waiters_mu, 1, 1, 0);
	dll_make_first (&free_waiters, &w->nw.q);
	ATM_STORE_REL (&free_waiters_mu, 0); /* release store */
	IGNORE_RACES_END ();
}

/* Return a pointer to an unused waiter struct.
   Ensures that the enclosed timer is stopped and its channel drained. */
waiter *nsync_waiter_new_ (void) {
	struct Dll *q;
	waiter *tw;
	waiter *w;
	tw = waiter_for_thread;
	w = tw;
	if (w == NULL || (w->flags & (WAITER_RESERVED|WAITER_IN_USE)) != WAITER_RESERVED) {
		w = NULL;
		nsync_spin_test_and_set_ (&free_waiters_mu, 1, 1, 0);
		q = dll_first (free_waiters);
		if (q != NULL) { /* If free list is non-empty, dequeue an item. */
			dll_remove (&free_waiters, q);
			w = DLL_WAITER (q);
		}
		ATM_STORE_REL (&free_waiters_mu, 0); /* release store */
		if (w == NULL) { /* If free list was empty, allocate an item. */
			w = (waiter *) nsync_malloc (sizeof (*w));
			w->tag = WAITER_TAG;
			w->nw.tag = NSYNC_WAITER_TAG;
			nsync_mu_semaphore_init (&w->sem);
			w->nw.sem = &w->sem;
			dll_init (&w->nw.q);
			NSYNC_ATOMIC_UINT32_STORE_ (&w->nw.waiting, 0);
			w->nw.flags = NSYNC_WAITER_FLAG_MUCV;
			ATM_STORE (&w->remove_count, 0);
			dll_init (&w->same_condition);
			w->flags = 0;
		}
		if (tw == NULL) {
			w->flags |= WAITER_RESERVED;
			waiter_for_thread = w;
		}
	}
	w->flags |= WAITER_IN_USE;
	return (w);
}

/* Return an unused waiter struct *w to the free pool. */
void nsync_waiter_free_ (waiter *w) {
	ASSERT ((w->flags & WAITER_IN_USE) != 0);
	w->flags &= ~WAITER_IN_USE;
	if ((w->flags & WAITER_RESERVED) == 0) {
		nsync_spin_test_and_set_ (&free_waiters_mu, 1, 1, 0);
		dll_make_first (&free_waiters, &w->nw.q);
		ATM_STORE_REL (&free_waiters_mu, 0); /* release store */
	}
}

/* ====================================================================================== */

/* writer_type points to a lock_type that describes how to manipulate a mu for a writer. */
static lock_type Xwriter_type = {
	MU_WZERO_TO_ACQUIRE,
	MU_WADD_TO_ACQUIRE,
	MU_WHELD_IF_NON_ZERO,
	MU_WSET_WHEN_WAITING,
	MU_WCLEAR_ON_ACQUIRE,
	MU_WCLEAR_ON_UNCONTENDED_RELEASE
};
lock_type *nsync_writer_type_ = &Xwriter_type;


/* reader_type points to a lock_type that describes how to manipulate a mu for a reader. */
static lock_type Xreader_type = {
	MU_RZERO_TO_ACQUIRE,
	MU_RADD_TO_ACQUIRE,
	MU_RHELD_IF_NON_ZERO,
	MU_RSET_WHEN_WAITING,
	MU_RCLEAR_ON_ACQUIRE,
	MU_RCLEAR_ON_UNCONTENDED_RELEASE
};
lock_type *nsync_reader_type_ = &Xreader_type;
