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
#include "libc/intrin/dll.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "third_party/nsync/atomic.h"
#include "third_party/nsync/common.internal.h"
#include "third_party/nsync/mu_semaphore.h"
#include "third_party/nsync/mu_wait.h"
#include "third_party/nsync/races.internal.h"
#include "third_party/nsync/wait_s.internal.h"
#include "third_party/nsync/waiter.h"

asm(".ident\t\"\\n\\n\
*NSYNC (Apache 2.0)\\n\
Copyright 2016 Google, Inc.\\n\
https://github.com/google/nsync\"");

/* Locking discipline for the nsync_note implementation:

   Each nsync_note has a lock "note_mu" which protects the "parent" pointer,
   "waiters" list, and "disconnecting" count.  It also protects the "children"
   list; thus each node's "parent_child_link", which links together the
   children of a single parent, is protected by the parent's "note_mu".

   To connect a parent to a child, or to disconnect one, the parent's lock must
   be held to manipulate its child list, and the child's lock must be held to
   change the parent pointer, so both must be held simultaneously.
   The locking order is "parent before child".

   Operations like notify and free are given a node pointer n and must
   disconnect *n from its parent n->parent.  The call must hold n->note_mu to
   read n->parent, but need to release n->note_mu to acquire
   n->parent->note_mu.  The parent could be disconnected and freed while
   n->note_mu is not held.  The n->disconnecting count handles this; the
   operation acquires n->note_mu, increments n->disconnecting, and can then
   release n->note_mu, and acquire n->parent->note_mu and n->note_mu is the
   correct order.  n->disconnecting!=0 indicates that a thread is already in
   the processes of disconnecting n from n->parent.  A thread freeing or
   notifying the parent should not perform the disconnection of that child, but
   should instead wait for the "children" list to become empty via
   WAIT_FOR_NO_CHILDREN().  WAKEUP_NO_CHILDREN() should be used whenever this
   condition could become true.  */

/* Set the expiry time in *n to t */
static void set_expiry_time (nsync_note n, nsync_time t) {
	n->expiry_time = t;
	n->expiry_time_valid = 1;
}

/* Return a pointer to the note containing struct Dll *e. */
#define DLL_NOTE(e) DLL_CONTAINER(struct nsync_note_s_, parent_child_link, e)

/* Return whether n->children is empty.  Assumes n->note_mu held. */
static int no_children (const void *v) {
	return (dll_is_empty (((nsync_note)v)->children));
}

#define WAIT_FOR_NO_CHILDREN(pred_, n_) nsync_mu_wait (&(n_)->note_mu, &pred_, (n_), NULL)
#define WAKEUP_NO_CHILDREN(n_) do { } while (0)

/*
// These lines can be used in place of those above if conditional critical
// sections have been removed from the source.
#define WAIT_FOR_NO_CHILDREN(pred_, n_) do { \
		while (!pred_ (n_)) { nsync_cv_wait (&(n_)->no_children_cv, &(n_)->note_mu); } \
	} while (0)
#define WAKEUP_NO_CHILDREN(n_) nsync_cv_broadcast (&(n_)->no_children_cv)
*/

/* Notify *n and all its descendants that are not already disconnnecting.
   n->note_mu is held.  May release and reacquire n->note_mu.
   parent->note_mu is held if parent != NULL. */
static void note_notify_child (nsync_note n, nsync_note parent) {
	nsync_time t;
	t = NOTIFIED_TIME (n);
	if (nsync_time_cmp (t, nsync_time_zero) > 0) {
		struct Dll *p;
		struct Dll *next;
		ATM_STORE_REL (&n->notified, 1);
		while ((p = dll_first (n->waiters)) != NULL) {
			struct nsync_waiter_s *nw = DLL_NSYNC_WAITER (p);
			dll_remove (&n->waiters, p);
			ATM_STORE_REL (&nw->waiting, 0);
			nsync_mu_semaphore_v (nw->sem);
		}
		for (p = dll_first (n->children); p != NULL; p = next) {
			nsync_note child = DLL_NOTE (p);
			next = dll_next (n->children, p);
			nsync_mu_lock (&child->note_mu);
			if (child->disconnecting == 0) {
				note_notify_child (child, n);
			}
			nsync_mu_unlock (&child->note_mu);
		}
		WAIT_FOR_NO_CHILDREN (no_children, n);
		if (parent != NULL) {
			dll_remove (&parent->children, &n->parent_child_link);
			WAKEUP_NO_CHILDREN (parent);
			n->parent = NULL;
		}
	}
}

/* Notify *n and all its descendants that are not already disconnnecting.
   No locks are held. */
static void notify (nsync_note n) {
	nsync_time t;
	nsync_mu_lock (&n->note_mu);
	t = NOTIFIED_TIME (n);
	if (nsync_time_cmp (t, nsync_time_zero) > 0) {
		nsync_note parent;
		n->disconnecting++;
		parent = n->parent;
		if (parent != NULL && !nsync_mu_trylock (&parent->note_mu)) {
			nsync_mu_unlock (&n->note_mu);
			nsync_mu_lock (&parent->note_mu);
			nsync_mu_lock (&n->note_mu);
		}
		note_notify_child (n, parent);
		if (parent != NULL) {
			nsync_mu_unlock (&parent->note_mu);
		}
		n->disconnecting--;
	}
	nsync_mu_unlock (&n->note_mu);
}

/* Return the deadline by which *n is certain to be notified,
   setting it to zero if it already has passed that time.
   Requires n->note_mu not held on entry.

   Not static; used in sem_wait.c */
nsync_time nsync_note_notified_deadline_ (nsync_note n) {
	nsync_time ntime;
	if (ATM_LOAD_ACQ (&n->notified) != 0) {
		ntime = nsync_time_zero;
	} else {
		nsync_mu_lock (&n->note_mu);
		ntime = NOTIFIED_TIME (n);
		nsync_mu_unlock (&n->note_mu);
		if (nsync_time_cmp (ntime, nsync_time_zero) > 0) {
			if (nsync_time_cmp (ntime, nsync_time_now ()) <= 0) {
				notify (n);
				ntime = nsync_time_zero;
			}
		}
	}
	return (ntime);
}

int nsync_note_is_notified (nsync_note n) {
	int result;
	IGNORE_RACES_START ();
	result = (nsync_time_cmp (nsync_note_notified_deadline_ (n), nsync_time_zero) <= 0);
	IGNORE_RACES_END ();
	return (result);
}

nsync_note nsync_note_new (nsync_note parent,
			   nsync_time abs_deadline) {
	nsync_note n = (nsync_note) malloc (sizeof (*n));
	if (n != NULL) {
		bzero (n, sizeof (*n));
		dll_init (&n->parent_child_link);
		set_expiry_time (n, abs_deadline);
		if (!nsync_note_is_notified (n) && parent != NULL) {
			nsync_time parent_time;
			nsync_mu_lock (&parent->note_mu);
			parent_time = NOTIFIED_TIME (parent);
			if (nsync_time_cmp (parent_time, abs_deadline) < 0) {
				set_expiry_time (n, parent_time);
			}
			if (nsync_time_cmp (parent_time, nsync_time_zero) > 0) {
				n->parent = parent;
				dll_make_last (&parent->children,
					       &n->parent_child_link);
			}
			nsync_mu_unlock (&parent->note_mu);
		}
	}
	return (n);
}

void nsync_note_free (nsync_note n) {
	nsync_note parent;
	struct Dll *p;
	struct Dll *next;
	nsync_mu_lock (&n->note_mu);
	n->disconnecting++;
	ASSERT (dll_is_empty (n->waiters));
	parent = n->parent;
	if (parent != NULL && !nsync_mu_trylock (&parent->note_mu)) {
		nsync_mu_unlock (&n->note_mu);
		nsync_mu_lock (&parent->note_mu);
		nsync_mu_lock (&n->note_mu);
	}
	for (p = dll_first (n->children); p != NULL; p = next) {
		nsync_note child = DLL_NOTE (p);
		next = dll_next (n->children, p);
		nsync_mu_lock (&child->note_mu);
		if (child->disconnecting == 0) {
			dll_remove (&n->children, &child->parent_child_link);
			if (parent != NULL) {
				child->parent = parent;
				dll_make_last (&parent->children,
					       &child->parent_child_link);
			} else {
				child->parent = NULL;
			}
		}
		nsync_mu_unlock (&child->note_mu);
	}
	WAIT_FOR_NO_CHILDREN (no_children, n);
	if (parent != NULL) {
		dll_remove (&parent->children, &n->parent_child_link);
		WAKEUP_NO_CHILDREN (parent);
		n->parent = NULL;
		nsync_mu_unlock (&parent->note_mu);
	}
	n->disconnecting--;
	nsync_mu_unlock (&n->note_mu);
	free (n);
}

void nsync_note_notify (nsync_note n) {
	IGNORE_RACES_START ();
	if (nsync_time_cmp (nsync_note_notified_deadline_ (n), nsync_time_zero) > 0) {
		notify (n);
	}
	IGNORE_RACES_END ();
}

int nsync_note_wait (nsync_note n, nsync_time abs_deadline) {
	struct nsync_waitable_s waitable;
	struct nsync_waitable_s *pwaitable = &waitable;
	waitable.v = n;
	waitable.funcs = &nsync_note_waitable_funcs;
	return (nsync_wait_n (NULL, NULL, NULL, abs_deadline, 1, &pwaitable) == 0);
}

nsync_time nsync_note_expiry (nsync_note n) {
	return (n->expiry_time);
}

static nsync_time note_ready_time (void *v, struct nsync_waiter_s *nw) {
	return (nsync_note_notified_deadline_ ((nsync_note)v));
}

static int note_enqueue (void *v, struct nsync_waiter_s *nw) {
	int waiting = 0;
	nsync_note n = (nsync_note) v;
	nsync_time ntime;
	nsync_mu_lock (&n->note_mu);
	ntime = NOTIFIED_TIME (n);
	if (nsync_time_cmp (ntime, nsync_time_zero) > 0) {
		dll_make_last (&n->waiters, &nw->q);
		ATM_STORE (&nw->waiting, 1);
		waiting = 1;
	} else {
		ATM_STORE (&nw->waiting, 0);
		waiting = 0;
	}
	nsync_mu_unlock (&n->note_mu);
	return (waiting);
}

static int note_dequeue (void *v, struct nsync_waiter_s *nw) {
	int was_queued = 0;
	nsync_note n = (nsync_note) v;
	nsync_time ntime;
	nsync_note_notified_deadline_ (n);
	nsync_mu_lock (&n->note_mu);
	ntime = NOTIFIED_TIME (n);
	if (nsync_time_cmp (ntime, nsync_time_zero) > 0) {
		dll_remove (&n->waiters, &nw->q);
		ATM_STORE (&nw->waiting, 0);
		was_queued = 1;
	}
	nsync_mu_unlock (&n->note_mu);
	return (was_queued);
}

const struct nsync_waitable_funcs_s nsync_note_waitable_funcs = {
	&note_ready_time,
	&note_enqueue,
	&note_dequeue
};
