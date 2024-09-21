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
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/directmap.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/extend.h"
#include "libc/nt/enum/filemapflags.h"
#include "libc/nt/enum/pageflags.h"
#include "libc/nt/memory.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdalign.h"
#include "libc/stdalign.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#include "third_party/nsync/atomic.h"
#include "third_party/nsync/atomic.internal.h"
#include "third_party/nsync/common.internal.h"
#include "third_party/nsync/mu_semaphore.h"
#include "third_party/nsync/mu_semaphore.internal.h"
#include "third_party/nsync/wait_s.internal.h"
__static_yoink("nsync_notice");

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

/* Spin until (*w & test) == 0, then atomically perform *w = ((*w | set) &
   ~clear), perform an acquire barrier, and return the previous value of *w.
   */
uint32_t nsync_spin_test_and_set_ (nsync_atomic_uint32_ *w, uint32_t test,
				   uint32_t set, uint32_t clear, void *symbol) {
	unsigned attempts = 0; /* CV_SPINLOCK retry count */
	uint32_t old = ATM_LOAD (w);
	while ((old & test) != 0 || !ATM_CAS_ACQ (w, old, (old | set) & ~clear)) {
		attempts = pthread_delay_np (symbol, attempts);
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

#define MASQUE 0x00fffffffffffff8
#define PTR(x) ((uintptr_t)(x) & MASQUE)
#define TAG(x) ROL((uintptr_t)(x) & ~MASQUE, 8)
#define ABA(p, t) ((uintptr_t)(p) | (ROR((uintptr_t)(t), 8) & ~MASQUE))
#define ROL(x, n) (((x) << (n)) | ((x) >> (64 - (n))))
#define ROR(x, n) (((x) >> (n)) | ((x) << (64 - (n))))

static atomic_uintptr_t free_waiters;

static void free_waiters_push (waiter *w) {
	uintptr_t tip;
	ASSERT (!TAG(w));
	tip = atomic_load_explicit (&free_waiters, memory_order_relaxed);
	for (;;) {
		w->next_free = (waiter *) PTR (tip);
		if (atomic_compare_exchange_weak_explicit (&free_waiters,
							   &tip,
							   ABA (w, TAG (tip) + 1),
							   memory_order_release,
							   memory_order_relaxed))
			break;
		pthread_pause_np ();
	}
}

static waiter *free_waiters_pop (void) {
	waiter *w;
	uintptr_t tip;
	tip = atomic_load_explicit (&free_waiters, memory_order_relaxed);
	while ((w = (waiter *) PTR (tip))) {
		if (atomic_compare_exchange_weak_explicit (&free_waiters,
							   &tip,
							   ABA (w->next_free, TAG (tip) + 1),
							   memory_order_acquire,
							   memory_order_relaxed))
			break;
		pthread_pause_np ();
	}
	return w;
}

static void free_waiters_populate (void) {
	int n;
	if (IsNetbsd ()) {
		// netbsd needs a real file descriptor per semaphore
		n = 1;
	} else {
		n = __pagesize / sizeof(waiter);
	}
	waiter *waiters = mmap (0, n * sizeof(waiter),
				PROT_READ | PROT_WRITE,
				MAP_PRIVATE | MAP_ANONYMOUS,
				-1, 0);
	if (waiters == MAP_FAILED)
		nsync_panic_ ("out of memory\n");
	for (size_t i = 0; i < n; ++i) {
		waiter *w = &waiters[i];
		w->tag = WAITER_TAG;
		w->nw.tag = NSYNC_WAITER_TAG;
		if (!nsync_mu_semaphore_init (&w->sem)) {
			if (!i)
				nsync_panic_ ("out of semaphores\n");
			break;
		}
		w->nw.sem = &w->sem;
		dll_init (&w->nw.q);
		w->nw.flags = NSYNC_WAITER_FLAG_MUCV;
		dll_init (&w->same_condition);
		free_waiters_push (w);
	}
}

/* -------------------------------- */

#define waiter_for_thread __get_tls()->tib_nsync

void nsync_waiter_destroy (void *v) {
	waiter *w = (waiter *) v;
	/* Reset waiter_for_thread in case another thread-local variable reuses
	   the waiter in its destructor while the waiter is taken by the other
	   thread from free_waiters. This can happen as the destruction order
	   of thread-local variables can be arbitrary in some platform e.g.
	   POSIX. */
	waiter_for_thread = NULL;
	ASSERT ((w->flags & (WAITER_RESERVED|WAITER_IN_USE)) == WAITER_RESERVED);
	w->flags &= ~WAITER_RESERVED;
	free_waiters_push (w);
}

/* Return a pointer to an unused waiter struct.
   Ensures that the enclosed timer is stopped and its channel drained. */
waiter *nsync_waiter_new_ (void) {
	waiter *w;
	waiter *tw;
	tw = waiter_for_thread;
	w = tw;
	if (w == NULL || (w->flags & (WAITER_RESERVED|WAITER_IN_USE)) != WAITER_RESERVED) {
		while (!(w = free_waiters_pop ()))
			free_waiters_populate ();
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
		free_waiters_push (w);
		if (w == waiter_for_thread)
			waiter_for_thread = 0;
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
