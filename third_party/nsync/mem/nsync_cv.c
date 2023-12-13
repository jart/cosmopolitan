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
#include "libc/calls/cp.internal.h"
#include "libc/errno.h"
#include "libc/intrin/dll.h"
#include "libc/str/str.h"
#include "libc/thread/thread.h"
#include "third_party/nsync/atomic.internal.h"
#include "third_party/nsync/common.internal.h"
#include "third_party/nsync/cv.h"
#include "third_party/nsync/races.internal.h"
#include "third_party/nsync/wait_s.internal.h"
#include "third_party/nsync/waiter.h"

// once we're paying the cost of nsync we might as well get the benefit
// of a better pthread_once(), since no other component pulls it in now
__static_yoink("nsync_run_once");

asm(".ident\t\"\\n\\n\
*NSYNC (Apache 2.0)\\n\
Copyright 2016 Google, Inc.\\n\
https://github.com/google/nsync\"");

/* Initialize *cv. */
void nsync_cv_init (nsync_cv *cv) {
        bzero ((void *) cv, sizeof (*cv));
}

/* Wake the cv waiters in the circular list pointed to by
   to_wake_list, which may not be NULL.  If the waiter is associated with a
   nsync_mu, the "wakeup" may consist of transferring the waiters to the nsync_mu's
   queue.  Requires that every waiter is associated with the same mutex.
   all_readers indicates whether all the waiters on the list are readers.  */
static void wake_waiters (struct Dll *to_wake_list, int all_readers) {
	struct Dll *p = NULL;
	struct Dll *next = NULL;
	struct Dll *first_waiter = dll_first (to_wake_list);
	struct nsync_waiter_s *first_nw = DLL_NSYNC_WAITER (first_waiter);
	waiter *first_w = NULL;
	nsync_mu *pmu = NULL;
	if ((first_nw->flags & NSYNC_WAITER_FLAG_MUCV) != 0) {
		first_w = DLL_WAITER (first_waiter);
		pmu = first_w->cv_mu;
	}
	if (pmu != NULL) { /* waiter is associated with the nsync_mu *pmu. */
		/* We will transfer elements of to_wake_list to *pmu if all of:
		    - some thread holds the lock, and
		    - *pmu's spinlock is not held, and
		    - either *pmu cannot be acquired in the mode of the first
		      waiter, or there's more than one thread on to_wake_list
		      and not all are readers, and
		    - we acquire the spinlock on the first try.
		   The spinlock acquisition also marks *pmu as having waiters.
		   The requirement that some thread holds the lock ensures
		   that at least one of the transferred waiters will be woken.
		   */
		uint32_t old_mu_word = ATM_LOAD (&pmu->word);
		int first_cant_acquire = ((old_mu_word & first_w->l_type->zero_to_acquire) != 0);
		next = dll_next (to_wake_list, first_waiter);
		if ((old_mu_word&MU_ANY_LOCK) != 0 &&
		    (old_mu_word&MU_SPINLOCK) == 0 &&
		    (first_cant_acquire || (next != NULL && !all_readers)) &&
		    ATM_CAS_ACQ (&pmu->word, old_mu_word,
				 (old_mu_word|MU_SPINLOCK|MU_WAITING) &
				 ~MU_ALL_FALSE)) {

			uint32_t set_on_release = 0;

			/* For any waiter that should be transferred, rather
			   than woken, move it from to_wake_list to pmu->waiters. */
			int first_is_writer = first_w->l_type == nsync_writer_type_;
			int transferred_a_writer = 0;
			int woke_areader = 0;
			/* Transfer the first waiter iff it can't acquire *pmu. */
			if (first_cant_acquire) {
				dll_remove (&to_wake_list, first_waiter);
				dll_make_last (&pmu->waiters, first_waiter);
				/* tell nsync_cv_wait_with_deadline() that we
				   moved the waiter to *pmu's queue.  */
				first_w->cv_mu = NULL;
				/* first_nw.waiting is already 1, from being on
				   cv's waiter queue.  */
				transferred_a_writer = first_is_writer;
			} else {
				woke_areader = !first_is_writer;
			}
			/* Now process the other waiters. */
			for (p = next; p != NULL; p = next) {
				int p_is_writer;
				struct nsync_waiter_s *p_nw = DLL_NSYNC_WAITER (p);
				waiter *p_w = NULL;
				if ((p_nw->flags & NSYNC_WAITER_FLAG_MUCV) != 0) {
					p_w = DLL_WAITER (p);
				}
				next = dll_next (to_wake_list, p);
				p_is_writer = (p_w != NULL &&
					       DLL_WAITER (p)->l_type == nsync_writer_type_);
				/* We transfer this element if any of:
				   - the first waiter can't acquire *pmu, or
				   - the first waiter is a writer, or
				   - this element is a writer. */
				if (p_w == NULL) {
					/* wake non-native waiter */
				} else if (first_cant_acquire || first_is_writer || p_is_writer) {
					dll_remove (&to_wake_list, p);
					dll_make_last (&pmu->waiters, p);
					/* tell nsync_cv_wait_with_deadline()
					   that we moved the waiter to *pmu's
					   queue.  */
					p_w->cv_mu = NULL;
					/* p_nw->waiting is already 1, from
					   being on cv's waiter queue.  */
					transferred_a_writer = transferred_a_writer || p_is_writer;
				} else {
					woke_areader = woke_areader || !p_is_writer;
				}
			}

			/* Claim a waiting writer if we transferred one, except if we woke readers,
			   in which case we want those readers to be able to acquire immediately. */
			if (transferred_a_writer && !woke_areader) {
				set_on_release |= MU_WRITER_WAITING;
			}

			/* release *pmu's spinlock  (MU_WAITING was set by CAS above) */
			old_mu_word = ATM_LOAD (&pmu->word);
			while (!ATM_CAS_REL (&pmu->word, old_mu_word,
					     (old_mu_word|set_on_release) & ~MU_SPINLOCK)) {
				old_mu_word = ATM_LOAD (&pmu->word);
			}
		}
	}

	/* Wake any waiters we didn't manage to enqueue on the mu. */
	for (p = dll_first (to_wake_list); p != NULL; p = next) {
		struct nsync_waiter_s *p_nw = DLL_NSYNC_WAITER (p);
		next = dll_next (to_wake_list, p);
		dll_remove (&to_wake_list, p);
		/* Wake the waiter. */
		ATM_STORE_REL (&p_nw->waiting, 0); /* release store */
		nsync_mu_semaphore_v (p_nw->sem);
	}
}

/* ------------------------------------------ */

/* Versions of nsync_mu_lock() and nsync_mu_unlock() that take "void *"
   arguments, to avoid call through a function pointer of a different type,
   which is undefined.  */
static void void_mu_lock (void *mu) {
	nsync_mu_lock ((nsync_mu *) mu);
}
static void void_mu_unlock (void *mu) {
	nsync_mu_unlock ((nsync_mu *) mu);
}

/* Memory needed for a single nsync_cv_wait_with_deadline_generic() call. */
struct nsync_cv_wait_with_deadline_s {
	nsync_cv *pcv;
	int sem_outcome;
	int is_reader_mu;
	uint32_t old_word;
	uint32_t remove_count;
	void *pmu;
	void (*lock) (void *);
	nsync_mu *cv_mu;
	nsync_time abs_deadline;
	nsync_note cancel_note;
	waiter *w;
};

/* Wait until awoken or timeout, or back out of wait if the thread is being cancelled. */
static int nsync_cv_wait_with_deadline_impl_ (struct nsync_cv_wait_with_deadline_s *c) {
	int outcome = 0;
	int attempts = 0;
	IGNORE_RACES_START ();
	while (ATM_LOAD_ACQ (&c->w->nw.waiting) != 0) { /* acquire load */
		if (c->sem_outcome == 0) {
			c->sem_outcome = nsync_sem_wait_with_cancel_ (c->w, c->abs_deadline, c->cancel_note);
		}
		if (c->sem_outcome != 0 && ATM_LOAD (&c->w->nw.waiting) != 0) {
			/* A timeout or cancellation occurred, and no wakeup.
			   Acquire *pcv's spinlock, and confirm.  */
			c->old_word = nsync_spin_test_and_set_ (&c->pcv->word, CV_SPINLOCK,
								CV_SPINLOCK, 0);
			/* Check that w wasn't removed from the queue after we
			   checked above, but before we acquired the spinlock.
			   The test of remove_count confirms that the waiter *w
			   is still governed by *pcv's spinlock; otherwise, some
			   other thread is about to set w.waiting==0.  */
			if (ATM_LOAD (&c->w->nw.waiting) != 0) {
				if (c->remove_count == ATM_LOAD (&c->w->remove_count)) {
					uint32_t old_value;
					/* still in cv waiter queue */
					/* Not woken, so remove *w from cv
					   queue, and declare a
					   timeout/cancellation.  */
					outcome = c->sem_outcome;
					dll_remove (&c->pcv->waiters, &c->w->nw.q);
					do {
						old_value = ATM_LOAD (&c->w->remove_count);
					} while (!ATM_CAS (&c->w->remove_count, old_value, old_value+1));
					if (dll_is_empty (c->pcv->waiters)) {
						c->old_word &= ~(CV_NON_EMPTY);
					}
					ATM_STORE_REL (&c->w->nw.waiting, 0); /* release store */
				}
			}
			/* Release spinlock. */
			ATM_STORE_REL (&c->pcv->word, c->old_word); /* release store */
		}
		if (ATM_LOAD (&c->w->nw.waiting) != 0) {
                        /* The delay here causes this thread ultimately to
                           yield to another that has dequeued this thread, but
                           has not yet set the waiting field to zero; a
                           cancellation or timeout may prevent this thread
                           from blocking above on the semaphore.  */
			attempts = nsync_spin_delay_ (attempts);
		}
	}
	if (c->cv_mu != NULL && c->w->cv_mu == NULL) { /* waiter was moved to *pmu's queue, and woken. */
		/* Requeue on *pmu using existing waiter struct; current thread
		   is the designated waker.  */
		nsync_mu_lock_slow_ (c->cv_mu, c->w, MU_DESIG_WAKER, c->w->l_type);
	} else {
		/* Traditional case: We've woken from the cv, and need to reacquire *pmu. */
		if (c->is_reader_mu) {
			nsync_mu_rlock (c->cv_mu);
		} else {
			(*c->lock) (c->pmu);
		}
	}
	nsync_waiter_free_ (c->w);
	IGNORE_RACES_END ();
	return (outcome);
}

/* Handle POSIX thread DEFERRED mode cancellation. */
static void nsync_cv_wait_with_deadline_unwind_ (void *arg) {
	struct nsync_cv_wait_with_deadline_s *c;
	c = (struct nsync_cv_wait_with_deadline_s *)arg;
	c->sem_outcome = ECANCELED;
	nsync_cv_wait_with_deadline_impl_ (c);
}

/* Atomically release *pmu (which must be held on entry)
   and block the calling thread on *pcv.  Then wait until awakened by a
   call to nsync_cv_signal() or nsync_cv_broadcast() (or a spurious wakeup), or by the time
   reaching abs_deadline, or by cancel_note being notified.  In all cases,
   reacquire *pmu, and return the reason for the call returned (0, ETIMEDOUT,
   or ECANCELED).  Callers should abs_deadline==nsync_time_no_deadline for no
   deadline, and cancel_note==NULL for no cancellation.  nsync_cv_wait_with_deadline()
   should be used in a loop, as with all Mesa-style condition variables.  See
   examples above.

   There are two reasons for using an absolute deadline, rather than a relative
   timeout---these are why pthread_cond_timedwait() also uses an absolute
   deadline.  First, condition variable waits have to be used in a loop; with
   an absolute times, the deadline does not have to be recomputed on each
   iteration.  Second, in most real programmes, some activity (such as an RPC
   to a server, or when guaranteeing response time in a UI), there is a
   deadline imposed by the specification or the caller/user; relative delays
   can shift arbitrarily with scheduling delays, and so after multiple waits
   might extend beyond the expected deadline.  Relative delays tend to be more
   convenient mostly in tests and trivial examples than they are in real
   programmes. */
int nsync_cv_wait_with_deadline_generic (nsync_cv *pcv, void *pmu,
					 void (*lock) (void *), void (*unlock) (void *),
					 nsync_time abs_deadline,
					 nsync_note cancel_note) {
	int outcome;
	struct nsync_cv_wait_with_deadline_s c;
	IGNORE_RACES_START ();

	c.w = nsync_waiter_new_ ();
	c.abs_deadline = abs_deadline;
	c.cancel_note = cancel_note;
	c.cv_mu = NULL;
	c.lock = lock;
	c.pcv = pcv;
	c.pmu = pmu;

	ATM_STORE (&c.w->nw.waiting, 1);
	c.w->cond.f = NULL; /* Not using a conditional critical section. */
	c.w->cond.v = NULL;
	c.w->cond.eq = NULL;
	if (lock == &void_mu_lock ||
	    lock == (void (*) (void *)) &nsync_mu_lock ||
	    lock == (void (*) (void *)) &nsync_mu_rlock) {
		c.cv_mu = (nsync_mu *) pmu;
	}
	c.w->cv_mu = c.cv_mu;  /* If *pmu is an nsync_mu, record its address, else record NULL. */
	c.is_reader_mu = 0; /* If true, an nsync_mu in reader mode. */
	if (c.cv_mu == NULL) {
		c.w->l_type = NULL;
	} else {
		uint32_t old_mu_word = ATM_LOAD (&c.cv_mu->word);
		int is_writer = (old_mu_word & MU_WHELD_IF_NON_ZERO) != 0;
		int is_reader = (old_mu_word & MU_RHELD_IF_NON_ZERO) != 0;
		if (is_writer) {
			if (is_reader) {
				nsync_panic_ ("mu held in reader and writer mode simultaneously "
				       "on entry to nsync_cv_wait_with_deadline()\n");
			}
			c.w->l_type = nsync_writer_type_;
		} else if (is_reader) {
			c.w->l_type = nsync_reader_type_;
			c.is_reader_mu = 1;
		} else {
			nsync_panic_ ("mu not held on entry to nsync_cv_wait_with_deadline()\n");
		}
	}

	/* acquire spinlock, set non-empty */
	c.old_word = nsync_spin_test_and_set_ (&pcv->word, CV_SPINLOCK, CV_SPINLOCK|CV_NON_EMPTY, 0);
	dll_make_last (&pcv->waiters, &c.w->nw.q);
	c.remove_count = ATM_LOAD (&c.w->remove_count);
	/* Release the spin lock. */
	ATM_STORE_REL (&pcv->word, c.old_word|CV_NON_EMPTY); /* release store */

	/* Release *pmu. */
	if (c.is_reader_mu) {
		nsync_mu_runlock (c.cv_mu);
	} else {
		(*unlock) (pmu);
	}

	/* Wait until awoken or a timeout. */
	c.sem_outcome = 0;
	pthread_cleanup_push (nsync_cv_wait_with_deadline_unwind_, &c);
	outcome = nsync_cv_wait_with_deadline_impl_ (&c);
	pthread_cleanup_pop (0);
	IGNORE_RACES_END ();
	return (outcome);
}

/* Wake at least one thread if any are currently blocked on *pcv.  If
   the chosen thread is a reader on an nsync_mu, wake all readers and, if
   possible, a writer. */
void nsync_cv_signal (nsync_cv *pcv) {
	IGNORE_RACES_START ();
	if ((ATM_LOAD_ACQ (&pcv->word) & CV_NON_EMPTY) != 0) { /* acquire load */
		struct Dll *to_wake_list = NULL; /* waiters that we will wake */
		int all_readers = 0;
		/* acquire spinlock */
		uint32_t old_word = nsync_spin_test_and_set_ (&pcv->word, CV_SPINLOCK,
							      CV_SPINLOCK, 0);
		if (!dll_is_empty (pcv->waiters)) {
			/* Point to first waiter that enqueued itself, and
			   detach it from all others.  */
			struct nsync_waiter_s *first_nw;
			struct Dll *first = dll_first (pcv->waiters);
			dll_remove (&pcv->waiters, first);
			first_nw = DLL_NSYNC_WAITER (first);
			if ((first_nw->flags & NSYNC_WAITER_FLAG_MUCV) != 0) {
				uint32_t old_value;
				do {
					old_value = ATM_LOAD (&DLL_WAITER (first)->remove_count);
				} while (!ATM_CAS (&DLL_WAITER (first)->remove_count,
						   old_value, old_value+1));
			}
			dll_make_last (&to_wake_list, first);
			if ((first_nw->flags & NSYNC_WAITER_FLAG_MUCV) != 0 &&
			    DLL_WAITER (first)->l_type == nsync_reader_type_) {
				int woke_writer;
				/* If the first waiter is a reader, wake all readers, and
				   if it's possible, one writer.  This allows reader-regions
				   to be added to a monitor without invalidating code in which
				   a client has optimized broadcast calls by converting them to
				   signal calls.  In particular, we wake a writer when waking
				   readers because the readers will not invalidate the condition
				   that motivated the client to call nsync_cv_signal().  But we
				   wake at most one writer because the first writer may invalidate
				   the condition; the client is expecting only one writer to be
				   able make use of the wakeup, or he would have called
				   nsync_cv_broadcast().  */
				struct Dll *p = NULL;
				struct Dll *next = NULL;
				all_readers = 1;
				woke_writer = 0;
				for (p = dll_first (pcv->waiters); p != NULL; p = next) {
					struct nsync_waiter_s *p_nw = DLL_NSYNC_WAITER (p);
					int should_wake;
					next = dll_next (pcv->waiters, p);
					should_wake = 0;
					if ((p_nw->flags & NSYNC_WAITER_FLAG_MUCV) != 0 &&
					     DLL_WAITER (p)->l_type == nsync_reader_type_) {
						should_wake = 1;
					} else if (!woke_writer) {
						woke_writer = 1;
						all_readers = 0;
						should_wake = 1;
					}
					if (should_wake) {
						dll_remove (&pcv->waiters, p);
						if ((p_nw->flags & NSYNC_WAITER_FLAG_MUCV) != 0) {
							uint32_t old_value;
							do {
								old_value = ATM_LOAD (
								    &DLL_WAITER (p)->remove_count);
							} while (!ATM_CAS (&DLL_WAITER (p)->remove_count,
									   old_value, old_value+1));
						}
						dll_make_last (&to_wake_list, p);
					}
				}
			}
			if (dll_is_empty (pcv->waiters)) {
				old_word &= ~(CV_NON_EMPTY);
			}
		}
		/* Release spinlock. */
		ATM_STORE_REL (&pcv->word, old_word); /* release store */
		if (!dll_is_empty (to_wake_list)) {
			wake_waiters (to_wake_list, all_readers);
		}
	}
	IGNORE_RACES_END ();
}

/* Wake all threads currently blocked on *pcv. */
void nsync_cv_broadcast (nsync_cv *pcv) {
	IGNORE_RACES_START ();
	if ((ATM_LOAD_ACQ (&pcv->word) & CV_NON_EMPTY) != 0) { /* acquire load */
		struct Dll *p;
		struct Dll *next;
		int all_readers;
		struct Dll *to_wake_list = NULL;   /* waiters that we will wake */
		/* acquire spinlock */
		nsync_spin_test_and_set_ (&pcv->word, CV_SPINLOCK, CV_SPINLOCK, 0);
		p = NULL;
		next = NULL;
		all_readers = 1;
		/* Wake entire waiter list, which we leave empty. */
		for (p = dll_first (pcv->waiters); p != NULL; p = next) {
			struct nsync_waiter_s *p_nw = DLL_NSYNC_WAITER (p);
			next = dll_next (pcv->waiters, p);
			all_readers = all_readers && (p_nw->flags & NSYNC_WAITER_FLAG_MUCV) != 0 &&
				      (DLL_WAITER (p)->l_type == nsync_reader_type_);
			dll_remove (&pcv->waiters, p);
			if ((p_nw->flags & NSYNC_WAITER_FLAG_MUCV) != 0) {
				uint32_t old_value;
				do {
					old_value = ATM_LOAD (&DLL_WAITER (p)->remove_count);
				} while (!ATM_CAS (&DLL_WAITER (p)->remove_count,
						   old_value, old_value+1));
			}
			dll_make_last (&to_wake_list, p);
		}
		/* Release spinlock and mark queue empty. */
		ATM_STORE_REL (&pcv->word, 0); /* release store */
		if (!dll_is_empty (to_wake_list)) {    /* Wake them. */
			wake_waiters (to_wake_list, all_readers);
		}
	}
	IGNORE_RACES_END ();
}

/* Wait with deadline, using an nsync_mu. */
errno_t nsync_cv_wait_with_deadline (nsync_cv *pcv, nsync_mu *pmu,
				     nsync_time abs_deadline,
				     nsync_note cancel_note) {
	return (nsync_cv_wait_with_deadline_generic (pcv, pmu, &void_mu_lock,
						     &void_mu_unlock,
						     abs_deadline, cancel_note));
}

/* Atomically release *pmu and block the caller on *pcv.  Wait
   until awakened by a call to nsync_cv_signal() or nsync_cv_broadcast(), or a spurious
   wakeup.  Then reacquires *pmu, and return.  The call is equivalent to a call
   to nsync_cv_wait_with_deadline() with abs_deadline==nsync_time_no_deadline, and a NULL
   cancel_note.  It should be used in a loop, as with all standard Mesa-style
   condition variables.  See examples above. Returns 0 normally, otherwise
   ECANCELED may be returned if calling POSIX thread is cancelled only when
   the PTHREAD_CANCEL_MASKED mode is in play. */
errno_t nsync_cv_wait (nsync_cv *pcv, nsync_mu *pmu) {
	return nsync_cv_wait_with_deadline (pcv, pmu, nsync_time_no_deadline, NULL);
}

static nsync_time cv_ready_time (void *v, struct nsync_waiter_s *nw) {
	nsync_time r;
	r = (nw == NULL || ATM_LOAD_ACQ (&nw->waiting) != 0? nsync_time_no_deadline : nsync_time_zero);
	return (r);
}

static int cv_enqueue (void *v, struct nsync_waiter_s *nw) {
	nsync_cv *pcv = (nsync_cv *) v;
	/* acquire spinlock */
	uint32_t old_word = nsync_spin_test_and_set_ (&pcv->word, CV_SPINLOCK, CV_SPINLOCK, 0);
	dll_make_last (&pcv->waiters, &nw->q);
	ATM_STORE (&nw->waiting, 1);
	/* Release spinlock. */
	ATM_STORE_REL (&pcv->word, old_word | CV_NON_EMPTY); /* release store */
	return (1);
}

static int cv_dequeue (void *v, struct nsync_waiter_s *nw) {
	nsync_cv *pcv = (nsync_cv *) v;
	int was_queued = 0;
	/* acquire spinlock */
	uint32_t old_word = nsync_spin_test_and_set_ (&pcv->word, CV_SPINLOCK, CV_SPINLOCK, 0);
	if (ATM_LOAD_ACQ (&nw->waiting) != 0) {
		dll_remove (&pcv->waiters, &nw->q);
		ATM_STORE (&nw->waiting, 0);
		was_queued = 1;
	}
	if (dll_is_empty (pcv->waiters)) {
		old_word &= ~(CV_NON_EMPTY);
	}
	/* Release spinlock. */
	ATM_STORE_REL (&pcv->word, old_word); /* release store */
	return (was_queued);
}

const struct nsync_waitable_funcs_s nsync_cv_waitable_funcs = {
	&cv_ready_time,
	&cv_enqueue,
	&cv_dequeue
};
