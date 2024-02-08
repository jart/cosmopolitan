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
#include "libc/intrin/dll.h"
#include "third_party/nsync/atomic.h"
#include "third_party/nsync/common.internal.h"
#include "third_party/nsync/mu_semaphore.h"
#include "third_party/nsync/races.internal.h"
#include "third_party/nsync/wait_s.internal.h"

asm(".ident\t\"\\n\\n\
*NSYNC (Apache 2.0)\\n\
Copyright 2016 Google, Inc.\\n\
https://github.com/google/nsync\"");

/* Attempt to remove waiter *w from *mu's
   waiter queue.  If successful, leave the lock held in mode *l_type, and
   return non-zero; otherwise return zero.  Requires that the current thread
   hold neither *mu nor its spinlock, that remove_count be the value of
   w.remove_count when *w was inserted into the queue (which it will still be if
   it has not been removed).

   This is a tricky part of the design.  Here is the rationale.

   When a condition times out or is cancelled, we must "turn off" the
   condition, making it always true, so the lock will be acquired in the normal
   way.  The naive approach would be to set a field atomically to tell future
   waiters to ignore the condition.  Unfortunately, that would violate the
   same_condition list invariants, and the same_condition optimization is
   probably worth keeping.

   To fixup the same_condition list, we must have mutual exclusion with the loop
   in nsync_mu_unlock_slow_() that is examining waiters, evaluating their conditions, and
   removing them from the queue.  That loop uses both the spinlock (to allow
   queue changes), and the mutex itself (to allow condition evaluation).
   Therefore, to "turn off" the condition requires acquiring both the spinlock
   and the mutex.  This has two consequences:
   - If we must acquire *mu to "turn off" the condition, we might as well give
     the lock to this waiter and return from nsync_cv_wait_with_deadline() after we've
     done so.  It would be wasted work to put it back on the waiter queue, and
     have it wake up and acquire yet again.  (There are possibilities for
     starvation here that we ignore, under the assumption that the client
     avoids timeouts that are extremely short relative to the durations of his
     section durations.)
   - We can't use *w to wait for the lock to be free, because *w is already on
     the waiter queue with the wrong condition; we now want to wait with no
     condition.  So either we must spin to acquire the lock, or we must
     allocate _another_ waiter object.  The latter option is feasible, but
     delicate:  the thread would have two waiter objects, and would have to
     handle being woken by either one or both, and possibly removing one that
     was not awoken.  For the moment, we spin, because it's easier, and seems
     not to cause problems in practice, since the spinloop backs off
     aggressively. */
static int mu_try_acquire_after_timeout_or_cancel (nsync_mu *mu, lock_type *l_type,
						   waiter *w, uint32_t remove_count) {
	int success = 0;
	unsigned spin_attempts = 0;
	uint32_t old_word = ATM_LOAD (&mu->word);
	/* Spin until we can acquire the spinlock and a writer lock on *mu. */
	while ((old_word&(MU_WZERO_TO_ACQUIRE|MU_SPINLOCK)) != 0 ||
	       !ATM_CAS_ACQ (&mu->word, old_word,
			     (old_word+MU_WADD_TO_ACQUIRE+MU_SPINLOCK) &
			     ~MU_WCLEAR_ON_ACQUIRE)) {
		/* Failed to acquire.  If we can, set the MU_WRITER_WAITING bit
		   to avoid being starved by readers. */
		if ((old_word & (MU_WRITER_WAITING | MU_SPINLOCK)) == 0) {
			/* If the following CAS succeeds, it effectively
			   acquires and releases the spinlock atomically, so
			   must be both an acquire and release barrier.
			   MU_WRITER_WAITING will be cleared via
			   MU_WCLEAR_ON_ACQUIRE when this loop succeeds.
			   An optimization; failures are ignored.  */
			ATM_CAS_RELACQ (&mu->word, old_word,
					old_word|MU_WRITER_WAITING);
		}
		spin_attempts = nsync_spin_delay_ (spin_attempts);
		old_word = ATM_LOAD (&mu->word);
	}
	/* Check that w wasn't removed from the queue after our caller checked,
	   but before we acquired the spinlock.
	   The check of remove_count confirms that the waiter *w is still
	   governed by *mu's spinlock.  Otherwise, some other thread may be
	   about to set w.waiting==0. */
	if (ATM_LOAD (&w->nw.waiting) != 0 && remove_count == ATM_LOAD (&w->remove_count)) {
		/* This thread's condition is now irrelevant, and it
		   holds a writer lock.  Remove it from the queue,
		   and possibly convert back to a reader lock. */
		mu->waiters = nsync_remove_from_mu_queue_ (mu->waiters, &w->nw.q);
		ATM_STORE (&w->nw.waiting, 0);

		/* Release spinlock but keep desired lock type. */
		ATM_STORE_REL (&mu->word, old_word+l_type->add_to_acquire); /* release store */
		success = 1;
	} else {
		/* Release spinlock and *mu. */
		ATM_STORE_REL (&mu->word, old_word); /* release store */
	}
	return (success);
}

/* Return when at least one of:  the condition is true, the
   deadline expires, or cancel_note is notified.  It may unlock and relock *mu
   while blocked waiting for one of these events, but always returns with *mu
   held.  It returns 0 iff the condition is true on return, and otherwise
   either ETIMEDOUT or ECANCELED, depending on why the call returned early.  Use
   abs_deadline==nsync_time_no_deadline for no deadline, and cancel_note==NULL for no
   cancellation.

   Requires that *mu be held on entry.
   Requires that condition.eval() neither modify state protected by *mu, nor
   return a value dependent on state not protected by *mu.  To depend on time,
   use the abs_deadline parameter.
   (Conventional use of condition variables have the same restrictions on the
   conditions tested by the while-loop.)
   The implementation calls condition.eval() only with *mu held, though not
   always from the calling thread, and may elect to hold only a read lock
   during the call, even if the client is attempting to acquire only write
   locks.

   The nsync_mu_wait() and nsync_mu_wait_with_deadline() calls can be used instead of condition
   variables.  In many straightforward situations they are of equivalent
   performance and are somewhat easier to use, because unlike condition
   variables, they do not require that the waits be placed in a loop, and they
   do not require explicit wakeup calls.  In the current implementation, use of
   nsync_mu_wait() and nsync_mu_wait_with_deadline() can take longer if many distinct
   wait conditions are used.  In such cases, use an explicit condition variable
   per wakeup condition for best performance. */
int nsync_mu_wait_with_deadline (nsync_mu *mu,
				 int (*condition) (const void *condition_arg),
				 const void *condition_arg,
				 int (*condition_arg_eq) (const void *a, const void *b),
				 nsync_time abs_deadline, nsync_note cancel_note) {
	lock_type *l_type;
	int first_wait;
	int condition_is_true;
	waiter *w;
	int outcome;
	/* Work out in which mode the lock is held. */
	uint32_t old_word;
	IGNORE_RACES_START ();
	BLOCK_CANCELATION;
	old_word = ATM_LOAD (&mu->word);
	if ((old_word & MU_ANY_LOCK) == 0) {
		nsync_panic_ ("nsync_mu not held in some mode when calling "
		       "nsync_mu_wait_with_deadline()\n");
	}
	l_type = nsync_writer_type_;
	if ((old_word & MU_RHELD_IF_NON_ZERO) != 0) {
		l_type = nsync_reader_type_;
	}

	first_wait = 1; /* first time through the loop below. */
	condition_is_true = (condition == NULL || (*condition) (condition_arg));

	/* Loop until either the condition becomes true, or "outcome" indicates
	   cancellation or timeout. */
	w = NULL;
	outcome = 0;
	while (outcome == 0 && !condition_is_true) {
		uint32_t has_condition;
		uint32_t remove_count;
		uint32_t add_to_acquire;
		int had_waiters;
		int sem_outcome;
		unsigned attempts;
		int have_lock;
		if (w == NULL) {
			w = nsync_waiter_new_ (); /* get a waiter struct if we need one. */
		}

		/* Prepare to wait. */
		w->cv_mu = NULL; /* not a condition variable wait */
		w->l_type = l_type;
		w->cond.f = condition;
		w->cond.v = condition_arg;
		w->cond.eq = condition_arg_eq;
		has_condition = 0; /* set to MU_CONDITION if condition is non-NULL */
		if (condition != NULL) {
			has_condition = MU_CONDITION;
		}
		ATM_STORE (&w->nw.waiting, 1);
		remove_count = ATM_LOAD (&w->remove_count);

		/* Acquire spinlock. */
		old_word = nsync_spin_test_and_set_ (&mu->word, MU_SPINLOCK,
			MU_SPINLOCK|MU_WAITING|has_condition, MU_ALL_FALSE);
		had_waiters = ((old_word & (MU_DESIG_WAKER | MU_WAITING)) == MU_WAITING);
		/* Queue the waiter. */
		if (first_wait) {
			nsync_maybe_merge_conditions_ (dll_last (mu->waiters),
						       &w->nw.q);
			/* first wait goes to end of queue */
			dll_make_last (&mu->waiters, &w->nw.q);
			first_wait = 0;
		} else {
			nsync_maybe_merge_conditions_ (&w->nw.q,
						       dll_first (mu->waiters));
			/* subsequent waits go to front of queue */
			dll_make_first (&mu->waiters, &w->nw.q);
		}
		/* Release spinlock and *mu. */
		do {
			old_word = ATM_LOAD (&mu->word);
			add_to_acquire = l_type->add_to_acquire;
			if (((old_word-l_type->add_to_acquire)&MU_ANY_LOCK) == 0 && had_waiters) {
				add_to_acquire = 0; /* release happens in nsync_mu_unlock_slow_ */
			}
		} while (!ATM_CAS_REL (&mu->word, old_word,
				       (old_word - add_to_acquire) & ~MU_SPINLOCK));
		if (add_to_acquire == 0) {
			/* The lock will be fully released, there are waiters, and
			   no designated waker, so wake waiters. */
			nsync_mu_unlock_slow_ (mu, l_type);
		}

		/* wait until awoken or a timeout. */
		sem_outcome = 0;
		attempts = 0;
		have_lock = 0;
		while (ATM_LOAD_ACQ (&w->nw.waiting) != 0) { /* acquire load */
			if (sem_outcome == 0) {
				sem_outcome = nsync_sem_wait_with_cancel_ (w, abs_deadline,
									   cancel_note);
				if (sem_outcome != 0 && ATM_LOAD (&w->nw.waiting) != 0) {
					/* A timeout or cancellation occurred, and no wakeup.
					   Acquire the spinlock and mu, and confirm. */
					have_lock = mu_try_acquire_after_timeout_or_cancel (
						mu, l_type, w, remove_count);
					if (have_lock) { /* Successful acquire. */
						outcome = sem_outcome;
					}
				}
			}

			if (ATM_LOAD (&w->nw.waiting) != 0) {
				attempts = nsync_spin_delay_ (attempts); /* will ultimately yield */
			}
		}

		if (!have_lock) {
			/* If we didn't reacquire due to a cancellation/timeout, acquire now. */
			nsync_mu_lock_slow_ (mu, w, MU_DESIG_WAKER, l_type);
		}
		condition_is_true = (condition == NULL || (*condition) (condition_arg));
	}
	if (w != NULL) {
		nsync_waiter_free_ (w); /* free waiter if we allocated one. */
	}
	if (condition_is_true) {
		outcome = 0; /* condition is true trumps other outcomes. */
	}
	ALLOW_CANCELATION;
	IGNORE_RACES_END ();
	return (outcome);
}

/* Return when the condition is true.  Perhaps unlock and relock *mu
   while blocked waiting for the condition to become true.  It is equivalent to
   a call to nsync_mu_wait_with_deadline() with abs_deadline==nsync_time_no_deadline, and
   cancel_note==NULL.

   Requires that *mu be held on entry.
   Calls condition.eval() only with *mu held, though not always from the
   calling thread.
   See wait_with_deadline() for the restrictions on condition and performance
   considerations. */
void nsync_mu_wait (nsync_mu *mu, int (*condition) (const void *condition_arg),
                    const void *condition_arg,
		    int (*condition_arg_eq) (const void *a, const void *b)) {
	if (nsync_mu_wait_with_deadline (mu, condition, condition_arg, condition_arg_eq,
					 nsync_time_no_deadline, NULL) != 0) {
		nsync_panic_ ("nsync_mu_wait woke but condition not true\n");
	}
}

/* Unlock *mu, which must be held in write mode, and wake waiters, if
   appropriate.  Unlike nsync_mu_unlock(), this call is not required to wake
   nsync_mu_wait/nsync_mu_wait_with_deadline calls on conditions that were
   false before this thread acquired the lock.  This call should be used only
   at the end of critical sections for which:
   - nsync_mu_wait/nsync_mu_wait_with_deadline are in use on the same mutex,
   - this critical section cannot make the condition true for any of those
     nsync_mu_wait/nsync_mu_wait_with_deadline waits, and
   - when performance is significantly improved by doing so.  */
void nsync_mu_unlock_without_wakeup (nsync_mu *mu) {
	IGNORE_RACES_START ();
	/* See comment in nsync_mu_unlock(). */
	if (!ATM_CAS_REL (&mu->word, MU_WLOCK, 0)) {
		uint32_t old_word = ATM_LOAD (&mu->word);
		uint32_t new_word = old_word - MU_WLOCK;
		if ((new_word & (MU_RLOCK_FIELD | MU_WLOCK)) != 0) {
			if ((old_word & MU_RLOCK_FIELD) != 0) {
				nsync_panic_ ("attempt to nsync_mu_unlock() an nsync_mu "
					      "held in read mode\n");
			} else {
				nsync_panic_ ("attempt to nsync_mu_unlock() an nsync_mu "
					      "not held in write mode\n");
			}
		} else if ((old_word & (MU_WAITING | MU_DESIG_WAKER | MU_ALL_FALSE)) ==
			   MU_WAITING || !ATM_CAS_REL (&mu->word, old_word, new_word)) {
			nsync_mu_unlock_slow_ (mu, nsync_writer_type_);
		}
	}
	IGNORE_RACES_END ();
}
