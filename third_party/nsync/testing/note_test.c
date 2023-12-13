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
#include "third_party/nsync/note.h"
#include "third_party/nsync/testing/closure.h"
#include "third_party/nsync/testing/smprintf.h"
#include "third_party/nsync/testing/testing.h"
#include "third_party/nsync/testing/time_extra.h"
#include "third_party/nsync/time.h"

/* Verify the properties of a prenotified note. */
static void test_note_prenotified (testing t) {
	int i;
	nsync_note n = nsync_note_new (NULL, nsync_time_zero /* prenotified */);
	nsync_time expiry;
	expiry = nsync_note_expiry (n);
	if (nsync_time_cmp (expiry, nsync_time_zero) != 0) {
		TEST_ERROR (t, ("prenotified note time mismatch 0"));
	}
	for (i = 0; i != 2; i++) {
		if (!nsync_note_is_notified (n)) {
			TEST_ERROR (t, ("prenotified note is not notified (test, %d)", i));
		}
		if (!nsync_note_wait (n, nsync_time_zero)) {
			TEST_ERROR (t, ("prenotified note is not notified (poll, %d)", i));
		}
		if (!nsync_note_wait (n, nsync_time_no_deadline)) {
			TEST_ERROR (t, ("prenotified note is not notified (infinite wait, %d)", i));
		}
		nsync_note_notify (n);
	}
	expiry = nsync_note_expiry (n);
	if (nsync_time_cmp (expiry, nsync_time_zero) != 0) {
		TEST_ERROR (t, ("prenotified note time mismatch 1"));
	}
	nsync_note_free (n);
}

/* Verify the properties of a unnotified note. */
static void test_note_unnotified (testing t) {
	nsync_time start;
	nsync_time waited;
	nsync_time deadline;
	nsync_note n = nsync_note_new (NULL, nsync_time_no_deadline);
	nsync_time expiry;
	expiry = nsync_note_expiry (n);
	if (nsync_time_cmp (expiry, nsync_time_no_deadline) != 0) {
		TEST_ERROR (t, ("unnotified note time mismatch 0"));
	}

	if (nsync_note_is_notified (n)) {
		TEST_ERROR (t, ("unnotified note is notified (test)"));
	}
	if (nsync_note_wait (n, nsync_time_zero)) {
		TEST_ERROR (t, ("notified note is notified (poll)"));
	}
	start = nsync_time_now ();
	deadline = nsync_time_add (nsync_time_now (), nsync_time_ms (1000));
	if (nsync_note_wait (n, deadline)) {
		TEST_ERROR (t, ("unnotified note is notified (1s wait)"));
	}
	waited = nsync_time_sub (nsync_time_now (), start);
	if (nsync_time_cmp (waited, nsync_time_ms (900)) < 0) {
		TEST_ERROR (t, ("timed wait on unnotified note returned too quickly (1s wait took %s)",
			   nsync_time_str (waited, 2)));
	}
	if (nsync_time_cmp (waited, nsync_time_ms (2000)) > 0) {
		TEST_ERROR (t, ("timed wait on unnotified note returned too slowly (1s wait took %s)",
			   nsync_time_str (waited, 2)));
	}

	nsync_note_notify (n);

	if (!nsync_note_is_notified (n)) {
		TEST_ERROR (t, ("notified note is not notified (test)"));
	}
	if (!nsync_note_wait (n, nsync_time_zero)) {
		TEST_ERROR (t, ("notified note is not notified (poll)"));
	}
	if (!nsync_note_wait (n, nsync_time_no_deadline)) {
		TEST_ERROR (t, ("notified note is not notified (infinite wait)"));
	}

	expiry = nsync_note_expiry (n);
	if (nsync_time_cmp (expiry, nsync_time_no_deadline) != 0) {
		TEST_ERROR (t, ("unnotified note time mismatch 1"));
	}

	nsync_note_free (n);
}

/* Test expiry on a note. */
static void test_note_expiry (testing t) {
	nsync_time start;
	nsync_time waited;
	nsync_time deadline;
	nsync_note n;

	deadline = nsync_time_add (nsync_time_now (), nsync_time_ms (1000));
	n = nsync_note_new (NULL, deadline);
	start = nsync_time_now ();
	if (!nsync_note_wait (n, nsync_time_no_deadline)) {
		TEST_ERROR (t, ("expired note is not notified"));
	}
	waited = nsync_time_sub (nsync_time_now (), start);
	if (nsync_time_cmp (waited, nsync_time_ms (900)) < 0) {
		TEST_ERROR (t, ("note expired too quickly (1s expiry took %s)",
			   nsync_time_str (waited, 2)));
	}
	if (nsync_time_cmp (waited, nsync_time_ms (2000)) > 0) {
		TEST_ERROR (t, ("timed expired too slowly (1s expiry took %s)",
			   nsync_time_str (waited, 2)));
	}
	if (!nsync_note_is_notified (n)) {
		TEST_ERROR (t, ("expired note note is not notified (test)"));
	}
	if (!nsync_note_wait (n, nsync_time_zero)) {
		TEST_ERROR (t, ("expired note note is not notified (poll)"));
	}
	if (!nsync_note_wait (n, nsync_time_no_deadline)) {
		TEST_ERROR (t, ("expired note note is not notified (infinite wait)"));
	}
	nsync_note_free (n);

	deadline = nsync_time_add (nsync_time_now (), nsync_time_ms (1000));
	n = nsync_note_new (NULL, deadline);
	start = nsync_time_now ();
	while (!nsync_note_is_notified (n)) {
		nsync_time_sleep (nsync_time_ms (10));
	}
	waited = nsync_time_sub (nsync_time_now (), start);
	if (nsync_time_cmp (waited, nsync_time_ms (900)) < 0) {
		TEST_ERROR (t, ("note expired too quickly (1s expiry took %s)",
			   nsync_time_str (waited, 2)));
	}
	if (nsync_time_cmp (waited, nsync_time_ms (2000)) > 0) {
		TEST_ERROR (t, ("timed expired too slowly (1s expiry took %s)",
			   nsync_time_str (waited, 2)));
	}
	if (!nsync_note_is_notified (n)) {
		TEST_ERROR (t, ("expired note note is not notified (test)"));
	}
	if (!nsync_note_wait (n, nsync_time_zero)) {
		TEST_ERROR (t, ("expired note note is not notified (poll)"));
	}
	if (!nsync_note_wait (n, nsync_time_no_deadline)) {
		TEST_ERROR (t, ("expired note note is not notified (infinite wait)"));
	}
	nsync_note_free (n);
}

static void notify_at (nsync_note n, nsync_time abs_deadline) {
	nsync_time_sleep_until (abs_deadline);
	nsync_note_notify (n);
}

CLOSURE_DECL_BODY2 (notify, nsync_note, nsync_time)

/* Test notification of a note. */
static void test_note_notify (testing t) {
	nsync_time start;
	nsync_time waited;
	nsync_time deadline;
	nsync_note n;

	deadline = nsync_time_add (nsync_time_now (), nsync_time_ms (10000));
	n = nsync_note_new (NULL, deadline);
	closure_fork (closure_notify (&notify_at, n, nsync_time_add (nsync_time_now (), nsync_time_ms (1000))));
	start = nsync_time_now ();
	if (!nsync_note_wait (n, nsync_time_no_deadline)) {
		TEST_ERROR (t, ("expired note is not notified"));
	}
	waited = nsync_time_sub (nsync_time_now (), start);
	if (nsync_time_cmp (waited, nsync_time_ms (900)) < 0) {
		TEST_ERROR (t, ("note expired too quickly (1s expiry took %s)",
			   nsync_time_str (waited, 2)));
	}
	if (nsync_time_cmp (waited, nsync_time_ms (2000)) > 0) {
		TEST_ERROR (t, ("timed expired too slowly (1s expiry took %s)",
			   nsync_time_str (waited, 2)));
	}
	if (!nsync_note_is_notified (n)) {
		TEST_ERROR (t, ("expired note note is not notified (test)"));
	}
	if (!nsync_note_wait (n, nsync_time_zero)) {
		TEST_ERROR (t, ("expired note note is not notified (poll)"));
	}
	if (!nsync_note_wait (n, nsync_time_no_deadline)) {
		TEST_ERROR (t, ("expired note note is not notified (infinite wait)"));
	}
	nsync_note_free (n);

	deadline = nsync_time_add (nsync_time_now (), nsync_time_ms (10000));
	n = nsync_note_new (NULL, deadline);
	closure_fork (closure_notify (&notify_at, n, nsync_time_add (nsync_time_now (), nsync_time_ms (1000))));
	start = nsync_time_now ();
	while (!nsync_note_is_notified (n)) {
		nsync_time_sleep (nsync_time_ms (10));
	}
	waited = nsync_time_sub (nsync_time_now (), start);
	if (nsync_time_cmp (waited, nsync_time_ms (900)) < 0) {
		TEST_ERROR (t, ("note expired too quickly (1s expiry took %s)",
			   nsync_time_str (waited, 2)));
	}
	if (nsync_time_cmp (waited, nsync_time_ms (2000)) > 0) {
		TEST_ERROR (t, ("timed expired too slowly (1s expiry took %s)",
			   nsync_time_str (waited, 2)));
	}
	if (!nsync_note_is_notified (n)) {
		TEST_ERROR (t, ("expired note note is not notified (test)"));
	}
	if (!nsync_note_wait (n, nsync_time_zero)) {
		TEST_ERROR (t, ("expired note note is not notified (poll)"));
	}
	if (!nsync_note_wait (n, nsync_time_no_deadline)) {
		TEST_ERROR (t, ("expired note note is not notified (infinite wait)"));
	}
	nsync_note_free (n);
}

/* Test notification of parent/child note. */
static void test_note_in_tree (testing t) {
	int i;
	enum {  /* Indexes of nodes that form a heap in the array node[]. */
		parent_i = 0,
		focus_i = 1,
		sibling_i = 2,
		child0_i = 3,
		child1_i = 4,
		nephew0_i = 5,
		nephew1_i = 6,
		grandchild00 = 7,
		grandchild01 = 8,
		grandchild10 = 9,
		grandchild11 = 10,

		count_i = 11
	};
	nsync_note node[count_i];

	/* Initialize heap structure in the nodes.  No deadlines. */
	node[0] = nsync_note_new (NULL, nsync_time_no_deadline);
	for (i = 1; i != count_i; i++) {
		node[i] = nsync_note_new (node[(i-1)/2], nsync_time_no_deadline);
	}

	/* check that the nodes are not yet notified. */
	for (i = 0; i != count_i; i++) {
		if (nsync_note_is_notified (node[i])) {
			TEST_ERROR (t, ("unnotified note %d is notified", i));
		}
	}

	/* Notify the focus node */
	nsync_note_notify (node[focus_i]);

	/* Check that the right nodes have been notified. */
	for (i = 0; i != count_i; i++) {
		int is_notified = nsync_note_is_notified (node[i]);
		if (i == parent_i || i == sibling_i || i == nephew0_i || i == nephew1_i) {
			/* Parent, sibling, and nephew nodes should not have been notified. */
			if (is_notified) {
				TEST_ERROR (t, ("unnotified note %d is notified", i));
			}
		} else if (!is_notified) { /* But the node and its descendents should be. */
			TEST_ERROR (t, ("notified note %d is not notified", i));
		}
	}
	for (i = 0; i != count_i; i++) {
		nsync_note_free (node[i]);
	}

	/* Initialize heap structure in the nodes.  The focus node has a 1s deadline. */
	node[0] = nsync_note_new (NULL, nsync_time_no_deadline);
	for (i = 1; i != count_i; i++) {
		nsync_time deadline;
		deadline = nsync_time_add (nsync_time_now (), nsync_time_ms (1000));
		if (i != focus_i) {
			deadline = nsync_time_no_deadline;
		}
		node[i] = nsync_note_new (node[(i - 1) / 2], deadline);
	}

	/* check that the nodes are not yet notified. */
	for (i = 0; i != count_i; i++) {
		if (nsync_note_is_notified (node[i])) {
			TEST_ERROR (t, ("unnotified note %d is notified", i));
		}
	}

	/* Wait for timer to go off. */
	nsync_time_sleep (nsync_time_ms (1100));

	/* Check that the right nodes have been notified. */
	for (i = 0; i != count_i; i++) {
		int is_notified = nsync_note_is_notified (node[i]);
		if (i == parent_i || i == sibling_i || i == nephew0_i || i == nephew1_i) {
			/* Parent, sibling, and nephew nodes should not have been notified. */
			if (is_notified) {
				TEST_ERROR (t, ("unnotified note %d is notified", i));
			}
		} else if (!is_notified) { /* But the node and its descendents should be. */
			TEST_ERROR (t, ("notified note %d is not notified", i));
		}
	}
	for (i = 0; i != count_i; i++) {
		nsync_note_free (node[i]);
	}
}

int main (int argc, char *argv[]) {
	testing_base tb = testing_new (argc, argv, 0);
	TEST_RUN (tb, test_note_prenotified);
	TEST_RUN (tb, test_note_unnotified);
	TEST_RUN (tb, test_note_expiry);
	TEST_RUN (tb, test_note_notify);
	TEST_RUN (tb, test_note_in_tree);
	return (testing_base_exit (tb));
}
