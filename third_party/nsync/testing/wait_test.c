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
#include "libc/str/str.h"
#include "third_party/nsync/array.internal.h"
#include "third_party/nsync/counter.h"
#include "third_party/nsync/note.h"
#include "third_party/nsync/testing/closure.h"
#include "third_party/nsync/testing/smprintf.h"
#include "third_party/nsync/testing/testing.h"
#include "third_party/nsync/testing/time_extra.h"
#include "third_party/nsync/time.h"
#include "third_party/nsync/waiter.h"

static void decrement_at (nsync_counter c, nsync_time abs_deadline, nsync_counter done) {
	nsync_time_sleep_until (abs_deadline);
	nsync_counter_add (c, -1);
	nsync_counter_add (done, -1);
}

CLOSURE_DECL_BODY3 (decrement, nsync_counter, nsync_time, nsync_counter)

static void notify_at (nsync_note n, nsync_time abs_deadline, nsync_counter done) {
	nsync_time_sleep_until (abs_deadline);
	nsync_note_notify (n);
	nsync_counter_add (done, -1);
}

CLOSURE_DECL_BODY3 (notify, nsync_note, nsync_time, nsync_counter)

typedef A_TYPE (struct nsync_waitable_s) a_waitable;
typedef A_TYPE (struct nsync_waitable_s *) a_pwaitable;

/* Test nsync_wait_n(). */
static void test_wait_n (testing t) {
	int i;
	int j;
	int k;
	int ncounter = 10;
	int nnote = 10;
	int nnote_expire = 10;
	for (i = 0; i != 30; i++) {
		nsync_counter done = nsync_counter_new (0);
		nsync_time now;
		nsync_time deadline;
		a_waitable aw;
		a_pwaitable apw;
		bzero (&aw, sizeof (aw));
		bzero (&apw, sizeof (apw));
		now = nsync_time_now ();
		deadline = nsync_time_add (now, nsync_time_ms (100));
		for (j = A_LEN (&aw); A_LEN (&aw) < j+ncounter;) {
			nsync_counter c = nsync_counter_new (0);
			struct nsync_waitable_s *w = &A_PUSH (&aw);
			w->v = c;
			w->funcs = &nsync_counter_waitable_funcs;
			for (k = 0; k != 4 && A_LEN (&aw) < j+ncounter; k++) {
				nsync_counter_add (c, 1);
				nsync_counter_add (done, 1);
				closure_fork (closure_decrement (&decrement_at, c, deadline, done));
			}
		}
		for (j = A_LEN (&aw); A_LEN (&aw) < j+nnote;) {
			nsync_note n = nsync_note_new (NULL, nsync_time_no_deadline);
			struct nsync_waitable_s *w = &A_PUSH (&aw);
			w->v = n;
			w->funcs = &nsync_note_waitable_funcs;
			nsync_counter_add (done, 1);
			closure_fork (closure_notify (&notify_at, n, deadline, done));
			for (k = 0; k != 4 && A_LEN (&aw) < j+nnote; k++) {
				nsync_note cn = nsync_note_new (n, nsync_time_no_deadline);
				struct nsync_waitable_s *lw = &A_PUSH (&aw);
				lw->v = cn;
				lw->funcs = &nsync_note_waitable_funcs;
			}
		}
		for (j = A_LEN (&aw); A_LEN (&aw) < j+nnote_expire;) {
			nsync_note n = nsync_note_new (NULL, deadline);
			struct nsync_waitable_s *w = &A_PUSH (&aw);
			w->v = n;
			w->funcs = &nsync_note_waitable_funcs;
			nsync_counter_add (done, 1);
			closure_fork (closure_notify (&notify_at, n, deadline, done));
			for (k = 0; k != 4 && A_LEN (&aw) < j+nnote; k++) {
				nsync_note cn = nsync_note_new (n, nsync_time_no_deadline);
				struct nsync_waitable_s *lw = &A_PUSH (&aw);
				lw->v = cn;
				lw->funcs = &nsync_note_waitable_funcs;
			}
		}
		if (ncounter + nnote + nnote_expire != A_LEN (&aw)) {
			TEST_ERROR (t, ("array length not equal to number of counters"));
		}
		for (j = 0; j != A_LEN (&aw); j++) {
			A_PUSH (&apw) = &A (&aw, j);
		}
		while (A_LEN (&apw) != 0) {
			k = nsync_wait_n (NULL, NULL, NULL, nsync_time_no_deadline,
					  A_LEN (&apw), &A (&apw, 0));
			if (k == A_LEN (&apw)) {
				TEST_ERROR (t, ("nsync_wait_n returned with no waiter ready"));
			}
			A (&apw, k) = A (&apw, A_LEN (&apw) - 1);
			A_DISCARD (&apw, 1);
		}
		nsync_counter_wait (done, nsync_time_no_deadline);
		for (k = 0; k != ncounter; k++) {
			nsync_counter_free ((nsync_counter) A (&aw, k).v);
		}
		for (; k < A_LEN (&aw); k++) {
			nsync_note_free ((nsync_note) A (&aw, k).v);
		}
		A_FREE (&apw);
		A_FREE (&aw);
		nsync_counter_free (done);
	}
}

/* Call *nsync_note_waitable_funcs.ready_time, and return its result, but
   before returning, notify the nsync_note.  This is used by
   test_wait_n_ready_while_queuing() to wrap nsync_note's normal *ready_time
   function, so that the behaviour of nsync_wait_n() can be checked when a
   notification happens while the enqueueing process. */
static nsync_time note_ready_time_wrapper (void *v, struct nsync_waiter_s *nw) {
	nsync_note n = (nsync_note) v;
	nsync_time result;
	result = (*nsync_note_waitable_funcs.ready_time) (v, nw);
	nsync_note_notify (n);
	return (result);
}

/* The following test checks that nsync_wait_n() behaves correctly if
   some object becomes ready during the enqueuing process. */
static void test_wait_n_ready_while_queuing (testing t) {
	struct nsync_waitable_s w[2];
	struct nsync_waitable_s *pw[2];
	int count;
	int woken;

        /* This test works by wrapping nsync_note's *ready_time function so
           that the note is notified just after nsync_wait_n() checks that it
           if not notified on entry.  */
	struct nsync_waitable_funcs_s wrapped_note_waitable_funcs;
	wrapped_note_waitable_funcs = nsync_note_waitable_funcs;
	wrapped_note_waitable_funcs.ready_time = &note_ready_time_wrapper;

	for (count = 0; count != sizeof (w) / sizeof (w[0]); count++) {
		nsync_note n = nsync_note_new (NULL, nsync_time_no_deadline);
		if (nsync_note_is_notified (n)) {
			TEST_ERROR (t, ("nsync_note is unexpectedly notified"));
		}
		w[count].v = n;
		w[count].funcs = &wrapped_note_waitable_funcs;
		pw[count] = &w[count];
	}
	woken = nsync_wait_n (NULL, NULL, NULL, nsync_time_no_deadline,
			      count, pw);
	if (woken != 0) {
		TEST_ERROR (t, ("nsync_wait_n unexpectedly failed to find pw[0] notified"));
	}
	for (count = 0; count != sizeof (w) / sizeof (w[0]); count++) {
		nsync_note n = (nsync_note) w[count].v;
		if (!nsync_note_is_notified (n)) {
			TEST_ERROR (t, ("nsync_note is unexpectedly not notified"));
		}
		nsync_note_free (n);
	}
}

int main (int argc, char *argv[]) {
	testing_base tb = testing_new (argc, argv, 0);
	TEST_RUN (tb, test_wait_n);
	TEST_RUN (tb, test_wait_n_ready_while_queuing);
	return (testing_base_exit (tb));
}
