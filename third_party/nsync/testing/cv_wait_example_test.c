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
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "third_party/nsync/array.internal.h"
#include "third_party/nsync/cv.h"
#include "third_party/nsync/heap.internal.h"
#include "third_party/nsync/mu.h"
#include "third_party/nsync/testing/closure.h"
#include "third_party/nsync/testing/smprintf.h"
#include "third_party/nsync/testing/testing.h"
#include "third_party/nsync/testing/time_extra.h"

/* Example use of CV.wait():  A priority queue of strings whose
   "remove_with_deadline" operation has a deadline. */

/* --------------------------------------- */

typedef A_TYPE (const char *) a_string; /* An array used as a heap of strings. */

/* heap comparison function */
static int str_lt (const char *e0, const char *e1) {
	return (strcmp (e0, e1) < 0);
}

static void no_set (const char *a, int b) {
}

/* --------------------------------------- */

/* A string_priority_queue_cv is a priority queue of strings, which emits the
   lexicographically least string available. */
typedef struct string_priority_queue_cv_s {
	nsync_cv non_empty; /* signalled when heap becomes non-empty */
	nsync_mu mu; /* protects heap */
	a_string heap;
} string_priority_queue_cv;


/* Add "s" to the queue *q. */
static void string_priority_queue_cv_add (string_priority_queue_cv *q, const char *s) {
	int alen;
	nsync_mu_lock (&q->mu);
	alen = A_LEN (&q->heap);
	if (alen == 0) {
		nsync_cv_broadcast (&q->non_empty);
	}
	A_PUSH (&q->heap) = s;
	heap_add (&A (&q->heap, 0), alen, str_lt, no_set, s);
	nsync_mu_unlock (&q->mu);
}

/* Wait until queue *q is non-empty, then remove a string from its
   beginning, and return it; or if abs_deadline is reached before the
   queue becomes non-empty, return NULL. */
static const char *string_priority_queue_cv_remove_with_deadline (string_priority_queue_cv *q,
							          nsync_time abs_deadline) {
	int alen;
	const char *s = NULL;
	nsync_mu_lock (&q->mu);
	while (A_LEN (&q->heap) == 0 &&
	       nsync_cv_wait_with_deadline (&q->non_empty, &q->mu, abs_deadline, NULL) == 0) {
	}
	alen = A_LEN (&q->heap);
	if (alen != 0) {
		s = A (&q->heap, 0);
		heap_remove (&A (&q->heap, 0), alen, str_lt, no_set, 0);
		A_DISCARD (&q->heap, 1);
	}
	nsync_mu_unlock (&q->mu);
	return (s);
}

/* Free resources associates with *q */
static void string_priority_queue_cv_destroy (string_priority_queue_cv *q) {
	A_FREE (&q->heap);
}

/* --------------------------------------- */

/* Add strings s[0, ..., n-1] to *q, with the specified delay between additions. */
static void add_and_wait_cv (string_priority_queue_cv *q, nsync_time delay,
			     int n, const char *s[]) {
	int i;
	for (i = 0; i != n; i++) {
		string_priority_queue_cv_add (q, s[i]);
		nsync_time_sleep (delay);
	}
}

CLOSURE_DECL_BODY4 (add_and_wait_cv, string_priority_queue_cv *,
		    nsync_time, int, const char **)

typedef A_TYPE (char) a_char; /* an array or characters */

/* Append the nul-terminated string str[] to *a. */
static void a_char_append (a_char *a, const char *str) {
	while (*str != 0) {
		A_PUSH (a) = *str;
		str++;
	}
}

/* Remove the first item from *q and output it on stdout,
   or output "timeout: <delay>" if no value can be found before "delay" elapses. */
static void remove_and_print_cv (string_priority_queue_cv *q, nsync_time delay, a_char *output) {
	const char *s;
	if ((s = string_priority_queue_cv_remove_with_deadline (
			q, nsync_time_add (nsync_time_now(), delay))) != NULL) {
		a_char_append (output, s);
		a_char_append (output, "\n");
	} else {
		char buf[64];
		snprintf (buf, sizeof (buf), "timeout %gs\n",
			  nsync_time_to_dbl (delay));
		a_char_append (output, buf);
	}
}

/* Demonstrates the use of nsync_mu_wait() via a priority queue of strings.
   See the routine string_priority_queue_cv_remove_with_deadline(), above. */
static void example_cv_wait (testing t) {
	static const char *input[] = { "one", "two", "three", "four", "five" };
	string_priority_queue_cv q;
	a_char output;
	static const char *expected =
		"one\n"
		"three\n"
		"two\n"
		"timeout 0.1s\n"
		"four\n"
		"timeout 0.1s\n"
		"five\n"
		"timeout 1s\n";

	bzero ((void *) &q, sizeof (q));
	bzero (&output, sizeof (output));

	closure_fork (closure_add_and_wait_cv (&add_and_wait_cv, &q,
					       nsync_time_ms (500), NELEM (input), input));

	/* delay: "one", "two", "three" are queued; not "four" */
	nsync_time_sleep (nsync_time_ms (1200));

	remove_and_print_cv (&q, nsync_time_ms (1000), &output);    /* "one" */
	remove_and_print_cv (&q, nsync_time_ms (1000), &output);    /* "three" (less than "two") */
	remove_and_print_cv (&q, nsync_time_ms (1000), &output);    /* "two" */
	remove_and_print_cv (&q, nsync_time_ms (100), &output); /* time out because 1.3 < 0.5*3 */
	remove_and_print_cv (&q, nsync_time_ms (1000), &output);    /* "four" */
	remove_and_print_cv (&q, nsync_time_ms (100), &output); /* time out because 0.1 < 0.5 */
	remove_and_print_cv (&q, nsync_time_ms (1000), &output);    /* "five" */
	remove_and_print_cv (&q, nsync_time_ms (1000), &output);    /* time out: no more to fetch */

	A_PUSH (&output) = 0;
	if (strcmp (&A (&output, 0), expected) != 0) {
		TEST_ERROR (t, ("expected = %s\ngot      = %s\n", expected, &A (&output, 0)));
	}
	A_FREE (&output);
	string_priority_queue_cv_destroy (&q);
}

int main (int argc, char *argv[]) {
	testing_base tb = testing_new (argc, argv, 0);
	TEST_RUN (tb, example_cv_wait);
	return (testing_base_exit (tb));
}
