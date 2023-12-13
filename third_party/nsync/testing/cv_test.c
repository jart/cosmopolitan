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
#include "third_party/nsync/cv.h"
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "third_party/nsync/debug.h"
#include "third_party/nsync/mu.h"
#include "third_party/nsync/mu_wait.h"
#include "third_party/nsync/note.h"
#include "third_party/nsync/testing/closure.h"
#include "third_party/nsync/testing/smprintf.h"
#include "third_party/nsync/testing/testing.h"
#include "third_party/nsync/testing/time_extra.h"
#include "third_party/nsync/time.h"

/* --------------------------- */

/* A cv_queue represents a FIFO queue with up to limit elements.
   The storage for the queue expands as necessary up to limit. */
typedef struct cv_queue_s {
	int limit;          /* max value of count---should not be changed after initialization */
	nsync_cv non_empty; /* signalled when count transitions from zero to non-zero */
	nsync_cv non_full;  /* signalled when count transitions from limit to less than limit */
	nsync_mu mu;        /* protects fields below */
	int pos;            /* index of first in-use element */
	int count;          /* number of elements in use */
	void *data[1];      /* in use elements are data[pos, ..., (pos+count-1)%limit] */
} cv_queue;

/* Return a pointer to new cv_queue. */
static cv_queue *cv_queue_new (int limit) {
	cv_queue *q;
	int size = offsetof (struct cv_queue_s, data) + sizeof (q->data[0]) * limit;
	q = (cv_queue *) malloc (size);
	bzero ((void *) q, size);
	q->limit = limit;
	return (q);
}

/* Add v to the end of the FIFO *q and return non-zero, or if the FIFO already
   has limit elements and continues to do so until abs_deadline, do nothing and
   return 0. */
static int cv_queue_put (cv_queue *q, void *v, nsync_time abs_deadline) {
	int added = 0;
	int wake = 0;
	nsync_mu_lock (&q->mu);
	while (q->count == q->limit &&
	       nsync_cv_wait_with_deadline (&q->non_full, &q->mu, abs_deadline, NULL) == 0) {
	}
	if (q->count != q->limit) {
		int i = q->pos + q->count;
		if (q->limit <= i) {
			i -= q->limit;
		}
		q->data[i] = v;
		if (q->count == 0) {
			wake = 1;
		}
		q->count++;
		added = 1;
	}
	nsync_mu_unlock (&q->mu);
	if (wake) {
		nsync_cv_broadcast (&q->non_empty);
	}
	return (added);
}

/* Remove the first value from the front of the FIFO *q and return it,
   or if the FIFO is empty and continues to be so until abs_deadline,
   do nothing and return NULL. */
static void *cv_queue_get (cv_queue *q, nsync_time abs_deadline) {
	void *v = NULL;
	nsync_mu_lock (&q->mu);
	while (q->count == 0 &&
	       nsync_cv_wait_with_deadline (&q->non_empty, &q->mu, abs_deadline, NULL) == 0) {
	}
	if (q->count != 0) {
		v = q->data[q->pos];
		q->data[q->pos] = NULL;
		if (q->count == q->limit) {
			nsync_cv_broadcast (&q->non_full);
		}
		q->pos++;
		q->count--;
		if (q->pos == q->limit) {
			q->pos = 0;
		}
	}
	nsync_mu_unlock (&q->mu);
	return (v);
}

/* --------------------------- */

static char ptr_to_int_c;
#define INT_TO_PTR(x) ((x) + &ptr_to_int_c)
#define PTR_TO_INT(p) (((char *) (p)) - &ptr_to_int_c)

/* Put count integers on *q, in the sequence start*3, (start+1)*3, (start+2)*3, .... */
static void producer_cv_n (testing t, cv_queue *q, int start, int count) {
	int i;
	for (i = 0; i != count; i++) {
		if (!cv_queue_put (q, INT_TO_PTR ((start+i)*3), nsync_time_no_deadline)) {
			TEST_FATAL (t, ("cv_queue_put() returned 0 with no deadline"));
		}
	}
}
CLOSURE_DECL_BODY4 (producer_cv_n, testing, cv_queue *, int, int)

/* Get count integers from *q, and check that they are in the
   sequence start*3, (start+1)*3, (start+2)*3, .... */
static void consumer_cv_n (testing t, cv_queue *q, int start, int count) {
	int i;
	for (i = 0; i != count; i++) {
		void *v = cv_queue_get (q, nsync_time_no_deadline);
		int x;
		if (v == NULL) {
			TEST_FATAL (t, ("cv_queue_get() returned NULL with no deadline"));
		}
		x = PTR_TO_INT (v);
		if (x != (start+i)*3) {
			TEST_FATAL (t, ("cv_queue_get() returned bad value; want %d, got %d",
				   (start+i)*3, x));
		}
	}
}

/* CV_PRODUCER_CONSUMER_N is the number of elements passed from producer to consumer in the
   test_cv_producer_consumer*() tests below. */
#define CV_PRODUCER_CONSUMER_N 100000

/* Send a stream of integers from a producer thread to
   a consumer thread via a queue with limit 10**0. */
static void test_cv_producer_consumer0 (testing t) {
	cv_queue *q = cv_queue_new (1);
	closure_fork (closure_producer_cv_n (&producer_cv_n, t, q, 0, CV_PRODUCER_CONSUMER_N));
	consumer_cv_n (t, q, 0, CV_PRODUCER_CONSUMER_N);
	free (q);
}

/* Send a stream of integers from a producer thread to
   a consumer thread via a queue with limit 10**1. */
static void test_cv_producer_consumer1 (testing t) {
	cv_queue *q = cv_queue_new (10);
	closure_fork (closure_producer_cv_n (&producer_cv_n, t, q, 0, CV_PRODUCER_CONSUMER_N));
	consumer_cv_n (t, q, 0, CV_PRODUCER_CONSUMER_N);
	free (q);
}

/* Send a stream of integers from a producer thread to
   a consumer thread via a queue with limit 10**2. */
static void test_cv_producer_consumer2 (testing t) {
	cv_queue *q = cv_queue_new (100);
	closure_fork (closure_producer_cv_n (&producer_cv_n, t, q, 0, CV_PRODUCER_CONSUMER_N));
	consumer_cv_n (t, q, 0, CV_PRODUCER_CONSUMER_N);
	free (q);
}

/* Send a stream of integers from a producer thread to
   a consumer thread via a queue with limit 10**3. */
static void test_cv_producer_consumer3 (testing t) {
	cv_queue *q = cv_queue_new (1000);
	closure_fork (closure_producer_cv_n (&producer_cv_n, t, q, 0, CV_PRODUCER_CONSUMER_N));
	consumer_cv_n (t, q, 0, CV_PRODUCER_CONSUMER_N);
	free (q);
}

/* Send a stream of integers from a producer thread to
   a consumer thread via a queue with limit 10**4. */
static void test_cv_producer_consumer4 (testing t) {
	cv_queue *q = cv_queue_new (10 * 1000);
	closure_fork (closure_producer_cv_n (&producer_cv_n, t, q, 0, CV_PRODUCER_CONSUMER_N));
	consumer_cv_n (t, q, 0, CV_PRODUCER_CONSUMER_N);
	free (q);
}

/* Send a stream of integers from a producer thread to
   a consumer thread via a queue with limit 10**5. */
static void test_cv_producer_consumer5 (testing t) {
	cv_queue *q = cv_queue_new (100 * 1000);
	closure_fork (closure_producer_cv_n (&producer_cv_n, t, q, 0, CV_PRODUCER_CONSUMER_N));
	consumer_cv_n (t, q, 0, CV_PRODUCER_CONSUMER_N);
	free (q);
}

/* Send a stream of integers from a producer thread to
   a consumer thread via a queue with limit 10**6. */
static void test_cv_producer_consumer6 (testing t) {
	cv_queue *q = cv_queue_new (1000 * 1000);
	closure_fork (closure_producer_cv_n (&producer_cv_n, t, q, 0, CV_PRODUCER_CONSUMER_N));
	consumer_cv_n (t, q, 0, CV_PRODUCER_CONSUMER_N);
	free (q);
}

/* The following values control how aggressively we police the timeout. */
#define TOO_EARLY_MS 1
#define TOO_LATE_MS 100   /* longer, to accommodate scheduling delays */
#define TOO_LATE_ALLOWED 25         /* number of iterations permitted to violate too_late */

/* Check timeouts on a CV wait_with_deadline(). */
static void test_cv_deadline (testing t) {
	int too_late_violations;
	nsync_mu mu;
	nsync_cv cv;
	int i;
	nsync_time too_early;
	nsync_time too_late;

	nsync_mu_init (&mu);
	nsync_cv_init (&cv);
	too_early = nsync_time_ms (TOO_EARLY_MS);
	too_late = nsync_time_ms (TOO_LATE_MS);
	too_late_violations = 0;
	nsync_mu_lock (&mu);
	for (i = 0; i != 50; i++) {
		nsync_time end_time;
		nsync_time start_time;
		nsync_time expected_end_time;
		start_time = nsync_time_now ();
		expected_end_time = nsync_time_add (start_time, nsync_time_ms (87));
		if (nsync_cv_wait_with_deadline (&cv, &mu, expected_end_time,
						 NULL) != ETIMEDOUT) {
			TEST_FATAL (t, ("nsync_cv_wait() returned non-expired for a timeout"));
		}
		end_time = nsync_time_now ();
		if (nsync_time_cmp (end_time, nsync_time_sub (expected_end_time, too_early)) < 0) {
			char *elapsed_str = nsync_time_str (nsync_time_sub (expected_end_time, end_time), 2);
			TEST_ERROR (t, ("nsync_cv_wait() returned %s too early", elapsed_str));
			free (elapsed_str);
		}
		if (nsync_time_cmp (nsync_time_add (expected_end_time, too_late), end_time) < 0) {
			too_late_violations++;
		}
	}
	nsync_mu_unlock (&mu);
	if (too_late_violations > TOO_LATE_ALLOWED) {
		TEST_ERROR (t, ("nsync_cv_wait() returned too late %d times", too_late_violations));
	}
}

/* Check cancellations with nsync_cv_wait_with_deadline(). */
static void test_cv_cancel (testing t) {
	nsync_time future_time;
	int too_late_violations;
	nsync_mu mu;
	nsync_cv cv;
	int i;
	nsync_time too_early;
	nsync_time too_late;

	nsync_mu_init (&mu);
	nsync_cv_init (&cv);
	too_early = nsync_time_ms (TOO_EARLY_MS);
	too_late = nsync_time_ms (TOO_LATE_MS);

	/* The loops below cancel after 87 milliseconds, like the timeout tests above. */

	future_time = nsync_time_add (nsync_time_now (), nsync_time_ms (3600000)); /* test cancels with timeout */

	too_late_violations = 0;
	nsync_mu_lock (&mu);
	for (i = 0; i != 50; i++) {
		int x;
		nsync_note cancel;
		nsync_time end_time;
		nsync_time start_time;
		nsync_time expected_end_time;
		start_time = nsync_time_now ();
		expected_end_time = nsync_time_add (start_time, nsync_time_ms (87));

		cancel = nsync_note_new (NULL, expected_end_time);

		x = nsync_cv_wait_with_deadline (&cv, &mu, future_time, cancel);
		if (x != ECANCELED) {
			TEST_FATAL (t, ("nsync_cv_wait() returned non-cancelled (%d) for "
				   "a cancellation; expected %d",
				   x, ECANCELED));
		}
		end_time = nsync_time_now ();
		if (nsync_time_cmp (end_time, nsync_time_sub (expected_end_time, too_early)) < 0) {
			char *elapsed_str = nsync_time_str (nsync_time_sub (expected_end_time, end_time), 2);
			TEST_ERROR (t, ("nsync_cv_wait() returned %s too early", elapsed_str));
			free (elapsed_str);
		}
		if (nsync_time_cmp (nsync_time_add (expected_end_time, too_late), end_time) < 0) {
			too_late_violations++;
		}

		/* Check that an already cancelled wait returns immediately. */
		start_time = nsync_time_now ();

		x = nsync_cv_wait_with_deadline (&cv, &mu, nsync_time_no_deadline, cancel);
		if (x != ECANCELED) {
			TEST_FATAL (t, ("nsync_cv_wait() returned non-cancelled (%d) for "
				   "a cancellation; expected %d",
				   x, ECANCELED));
		}
		end_time = nsync_time_now ();
		if (nsync_time_cmp (end_time, start_time) < 0) {
			char *elapsed_str = nsync_time_str (nsync_time_sub (expected_end_time, end_time), 2);
			TEST_ERROR (t, ("nsync_cv_wait() returned %s too early", elapsed_str));
			free (elapsed_str);
		}
		if (nsync_time_cmp (nsync_time_add (start_time, too_late), end_time) < 0) {
			too_late_violations++;
		}
		nsync_note_notify (cancel);

		nsync_note_free (cancel);
	}
	nsync_mu_unlock (&mu);
	if (too_late_violations > TOO_LATE_ALLOWED) {
		TEST_ERROR (t, ("nsync_cv_wait() returned too late %d times", too_late_violations));
	}
}

/* --------------------------- */

/* Names of debug results for test_cv_debug. */
static const char *result_name[] = {
	"init_mu0",
	"init_cv0",
	"init_mu1",
	"init_cv1",
	"init_mu2",
	"init_cv2",
	"held_mu",
	"wait0_mu",
	"wait0_cv",
	"wait1_mu",
	"wait1_cv",
	"wait2_mu",
	"wait2_cv",
	"wait3_mu",
	"wait3_cv",
	"rheld1_mu",
	"rheld2_mu",
	"rheld1again_mu",
	NULL /* sentinel */
};

/* state for test_cv_debug() */
struct debug_state {
	nsync_mu mu;  /* protects flag field */
	nsync_cv cv;  /* signalled when flag becomes zero */
	int flag;     /* 0 => threads proceed; non-zero => threads block */

	/* result[] is an array of nul-terminated string values, accessed via
	   name (in result_name[]) via slot().  Entries accessed from multiple
	   threads are protected by result_mu.  */
	char *result[sizeof (result_name) / sizeof (result_name[0])];
	nsync_mu result_mu;
};

/* Return a pointer to the slot in s->result[] associated with the
   nul-terminated name[] */
static char **slot (struct debug_state *s, const char *name) {
	int i = 0;
	while (result_name[i] != NULL && strcmp (result_name[i], name) != 0) {
		i++;
	}
	if (result_name[i] == NULL) {  /* caller gave non-existent name */
		abort ();
	}
	return (&s->result[i]);
}

/* Check that the strings associated with nul-terminated strings name0[] and
   name1[] have the same values in s->result[].  */
static void check_same (testing t, struct debug_state *s,
			     const char *name0, const char *name1) {
	if (strcmp (*slot (s, name0), *slot (s, name1)) != 0) {
		TEST_ERROR (t, ("nsync_mu_debug_state() %s state != %s state (%s vs. %s)",
				name0, name1, *slot (s, name0), *slot (s, name1)));
	}
}

/* Check that the strings associated with nul-terminated strings name0[] and
   name1[] have different values in s->result[].  */
static void check_different (testing t, struct debug_state *s,
			     const char *name0, const char *name1) {
	if (strcmp (*slot (s, name0), *slot (s, name1)) == 0) {
		TEST_ERROR (t, ("nsync_mu_debug_state() %s state == %s state",
				name0, name1));
	}
}

/* Return whether the integer at address v is zero. */
static int int_is_zero (const void *v) {
	return (*(int *)v == 0);
}

/* Acquire and release s->mu in write mode, waiting for s->flag==0
   using nsync_mu_wait(). */
static void debug_thread_writer (struct debug_state *s) {
	nsync_mu_lock (&s->mu);
	nsync_mu_wait (&s->mu, &int_is_zero, &s->flag, NULL);
	nsync_mu_unlock (&s->mu);
}

/* Acquire and release s->mu in write mode, waiting for s->flag==0
   using nsync_cv_wait(). */
static void debug_thread_writer_cv (struct debug_state *s) {
	nsync_mu_lock (&s->mu);
	while (s->flag != 0) {
		nsync_cv_wait (&s->cv, &s->mu);
	}
	nsync_mu_unlock (&s->mu);
}

/* Acquire and release s->mu in read mode, waiting for s->flag==0
   using nsync_mu_wait().
   If name!=NULL, record state of s->mu while held using name[]. */
static void debug_thread_reader (struct debug_state *s,
				 const char *name) {
	nsync_mu_rlock (&s->mu);
	nsync_mu_wait (&s->mu, &int_is_zero, &s->flag, NULL);
	if (name != NULL) {
		int len = 1024;
		nsync_mu_lock (&s->result_mu);
		*slot (s, name) = nsync_mu_debug_state_and_waiters (
			&s->mu, (char *) malloc (len), len);
		nsync_mu_unlock (&s->result_mu);
	}
	nsync_mu_runlock (&s->mu);
}

/* Acquire and release s->mu in read mode, waiting for s->flag==0
   using nsync_cv_wait().
   If name!=NULL, record state of s->mu while held using name[]. */
static void debug_thread_reader_cv (struct debug_state *s,
				    const char *name) {
	nsync_mu_rlock (&s->mu);
	while (s->flag != 0) {
		nsync_cv_wait (&s->cv, &s->mu);
	}
	if (name != NULL) {
		int len = 1024;
		nsync_mu_lock (&s->result_mu);
		*slot (s, name) = nsync_mu_debug_state_and_waiters (
			&s->mu, (char *) malloc (len), len);
		nsync_mu_unlock (&s->result_mu);
	}
	nsync_mu_runlock (&s->mu);
}

CLOSURE_DECL_BODY1 (debug_thread, struct debug_state *)
CLOSURE_DECL_BODY2 (debug_thread_reader, struct debug_state *, const char *)

/* Check that nsync_mu_debug_state() and nsync_cv_debug_state()
   and their variants yield reasonable results.

   The specification of those routines is intentionally loose,
   so this do not check much, but the various possibilities can be 
   examined using the verbose testing flag (-v). */
static void test_cv_debug (testing t) {
	int i;
	int len = 1024;
	char *tmp;
	char *buf;
	int buflen;
	struct debug_state xs;
	struct debug_state *s = &xs;
	bzero ((void *) s, sizeof (*s));

	/* Use nsync_*_debugger to check that they work. */
	tmp = nsync_mu_debugger (&s->mu);
	buflen = strlen (tmp)+1;
	buf = (char *) malloc (buflen);
	snprintf (buf, buflen, "%s", tmp);
	*slot (s, "init_mu0") = buf;

	tmp = nsync_cv_debugger (&s->cv);
	buflen = strlen (tmp)+1;
	buf = (char *) malloc (buflen);
	snprintf (buf, buflen, "%s", tmp);
	*slot (s, "init_cv0") = buf;

	/* Get the same information via the other routines */
	*slot (s, "init_mu1") = nsync_mu_debug_state (
		&s->mu, (char *) malloc (len), len);
	*slot (s, "init_cv1") = nsync_cv_debug_state (
		&s->cv, (char *) malloc (len), len);
	*slot (s, "init_mu2") = nsync_mu_debug_state_and_waiters (
		&s->mu, (char *) malloc (len), len);
	*slot (s, "init_cv2") = nsync_cv_debug_state_and_waiters (
		&s->cv, (char *) malloc (len), len);

	nsync_mu_lock (&s->mu);
	*slot (s, "held_mu") = nsync_mu_debug_state_and_waiters (
		&s->mu, (char *) malloc (len), len);
	nsync_mu_unlock (&s->mu);

	/* set up several threads waiting on the mutex */
	nsync_mu_lock (&s->mu);
	s->flag = 1;   /* so thread will block on conditions */
	closure_fork (closure_debug_thread (&debug_thread_writer, s));
	closure_fork (closure_debug_thread (&debug_thread_writer, s));
	closure_fork (closure_debug_thread (&debug_thread_writer, s));
	closure_fork (closure_debug_thread_reader (&debug_thread_reader, s, NULL));
	closure_fork (closure_debug_thread (&debug_thread_writer_cv, s));
	closure_fork (closure_debug_thread (&debug_thread_writer_cv, s));
	closure_fork (closure_debug_thread (&debug_thread_writer_cv, s));
	closure_fork (closure_debug_thread_reader (&debug_thread_reader_cv, s, NULL));
	nsync_time_sleep (nsync_time_ms (500));
	*slot (s, "wait0_mu") = nsync_mu_debug_state_and_waiters (
		&s->mu, (char *) malloc (len), len);
	*slot (s, "wait0_cv") = nsync_cv_debug_state_and_waiters (
		&s->cv, (char *) malloc (len), len);

	/* allow the threads to proceed to their conditional waits */
	nsync_mu_unlock (&s->mu);
	nsync_time_sleep (nsync_time_ms (500));
	*slot (s, "wait1_mu") = nsync_mu_debug_state_and_waiters (
		&s->mu, (char *) malloc (len), len);
	*slot (s, "wait1_cv") = nsync_cv_debug_state_and_waiters (
		&s->cv, (char *) malloc (len), len);

	nsync_mu_lock (&s->mu);
	/* move cv waiters to mutex queue */
	nsync_cv_broadcast (&s->cv);
	*slot (s, "wait2_mu") = nsync_mu_debug_state_and_waiters (
		&s->mu, (char *) malloc (len), len);
	*slot (s, "wait2_cv") = nsync_cv_debug_state_and_waiters (
		&s->cv, (char *) malloc (len), len);

	/* allow all threads to proceed and exit */
	s->flag = 0;
	nsync_mu_unlock (&s->mu);
	nsync_time_sleep (nsync_time_ms (500));
	*slot (s, "wait3_mu") = nsync_mu_debug_state_and_waiters (
		&s->mu, (char *) malloc (len), len);
	*slot (s, "wait3_cv") = nsync_cv_debug_state_and_waiters (
		&s->cv, (char *) malloc (len), len);

	/* Test with more than one reader */
	nsync_mu_rlock (&s->mu);
	*slot (s, "rheld1_mu") = nsync_mu_debug_state_and_waiters (
		&s->mu, (char *) malloc (len), len);
	closure_fork (closure_debug_thread_reader (
		&debug_thread_reader, s, "rheld2_mu"));
	nsync_time_sleep (nsync_time_ms (500));
	*slot (s, "rheld1again_mu") = nsync_mu_debug_state_and_waiters (
		&s->mu, (char *) malloc (len), len);
	nsync_mu_runlock (&s->mu);

	check_same (t, s, "init_mu0", "init_mu1");
	check_same (t, s, "init_mu0", "init_mu2");
	check_same (t, s, "init_cv0", "init_cv1");
	check_same (t, s, "init_cv0", "init_cv2");
	check_different (t, s, "init_mu0", "held_mu");
	check_different (t, s, "rheld1_mu", "held_mu");
	/* Must acquire result_mu, because the "rheld2_mu" slot is accessed
	   from the debug_thread_reader() thread created above.  */
	nsync_mu_lock (&s->result_mu);
	check_different (t, s, "rheld1_mu", "rheld2_mu");
	nsync_mu_unlock (&s->result_mu);
	check_different (t, s, "init_mu0", "init_cv0");

	for (i = 0; result_name[i] != NULL; i++) {
		if (testing_verbose (t)) {
			const char *str = *slot (s, result_name[i]);
			TEST_LOG (t, ("%-16s  %s\n", result_name[i], str));
		}
		if (strlen (s->result[i]) == 0) {
			TEST_ERROR (t, ("nsync_mu_debug_state() %s empty",
					result_name[i]));
		}
		free (s->result[i]);
	}
}

/* --------------------------- */

/* Max number of waiter threads used in transfer test.
   The last uses a conditional critical section, and others
   use a condition variable.   */
#define TRANSFER_MAX_WAITERS 8

/* A struct cv_transfer is used to test cv-to-mu thread transfer.
   There are up to TRANSFER_MAX_WAITERS waiter threads, and a wakeup thread.
   Some threads wait using conditional critical sections,
   and others using a condition variable. */
struct cv_transfer {
	nsync_mu mu;

	nsync_cv cv;  /* signalled each time a cond[] element becomes non-zero */
	/* Thread i waits for cond[i] to be non-zero; under mu.  */
        int cond[TRANSFER_MAX_WAITERS];

	nsync_mu control_mu;  /* protects fields below */
	nsync_cv done_cv; /* signalled each time an element of done[] becomes non-zero */
	int ready[TRANSFER_MAX_WAITERS];  /* set by waiters as they wait */
	int done[TRANSFER_MAX_WAITERS];   /* set by completed waiters: to 1 by readers, and to 2 by writers */
};

/* Return whether *(int *)v != 0.  Used as a condition for nsync_mu_wait().  */
static int int_is_non_zero (const void *v) {
	return (0 != *(const int *)v);
}

/* Return when *pi becomes non-zero, where *pi is protected by *mu.
   Acquires and releases *mu. */
static void transfer_await_nonzero (nsync_mu *mu, int *pi) {
	nsync_mu_lock (mu);
	nsync_mu_wait (mu, &int_is_non_zero, pi, NULL);
	nsync_mu_unlock (mu);
}

/* Set *pi to x value, where *pi is protected by *mu.
   Acquires and releases *mu. */
static void transfer_set (nsync_mu *mu, int *pi, int x) {
	nsync_mu_lock (mu);
	*pi = x;
	nsync_mu_unlock (mu);
}

/* Lock and unlock routines for writers (index 0), and readers (index 1).  */
static const struct {
	void (*lock) (nsync_mu *);
	void (*unlock) (nsync_mu *);
} lock_type[2] = {
	{ &nsync_mu_lock, &nsync_mu_unlock },
	{ &nsync_mu_rlock, &nsync_mu_runlock },
};

/* Signal and broadcast routines */
typedef void (*wakeup_func_type) (nsync_cv *);
static wakeup_func_type wakeup_func[2] = { &nsync_cv_broadcast, &nsync_cv_signal };

/* Acquire cvt->mu in write or read mode (depending on "reader"),
   set cvt->ready[i], wait for cvt->cond[i] to become non-zero (using
   a condition variable if use_cv!=0), then release cvt->mu, and
   set cvt->done[i].
   Used as the body of waiter threads created by test_cv_transfer(). */
static void transfer_waiter_thread (struct cv_transfer *cvt, int i, int reader, int use_cv) {
	(*lock_type[reader].lock) (&cvt->mu);
	transfer_set (&cvt->control_mu, &cvt->ready[i], 1);
	if (use_cv) {
		while (!cvt->cond[i]) {
			nsync_cv_wait (&cvt->cv, &cvt->mu);
		}
	} else {
		nsync_mu_wait (&cvt->mu, &int_is_non_zero, &cvt->cond[i], NULL);
	}
	(*lock_type[reader].unlock) (&cvt->mu);

	transfer_set (&cvt->control_mu, &cvt->done[i], reader? 1 : 2);
	nsync_cv_broadcast (&cvt->done_cv);
}

/* Return whether all the elements a[0..n-1] are less than x. */
static int are_all_below (int a[], int n, int x) {
	int i;
	for (i = 0; i != n && a[i] < x; i++) {
	}
	return (i == n);
}

CLOSURE_DECL_BODY4 (transfer_thread, struct cv_transfer *, int, int, int)

/* Test cv-to-mutex queue transfer.  (See the code in cv.c, wake_waiters().)

   The queue transfer needs to work regardless of:
   - whether the mutex is also being used with conditional critical sections,
   - whether reader locks are used,
   - whether the waker signals from within the critical section (as it would in
     a traditional monitor), or after that critical section, and
   - the number of threads that might be awoken.  */
static void test_cv_transfer (testing t) {
	int waiters;	 /* number of waiters (in [2, TRANSFER_MAX_WAITERS]). */
	int cv_writers;  /* number of cv_writers: -1 means all */
	int ccs_reader; /* ccs waiter is a reader */
	int wakeup_type; /* bits: use_signal and after_region */
	enum { use_signal = 0x1 };  /* use signal rather than broadcast */
	enum { after_region = 0x2 };  /* perform wakeup after region, rather than within */
	struct cv_transfer Xcvt;
	struct cv_transfer *cvt = &Xcvt;  /* So all accesses are of form cvt-> */
	int i;

	/* for all settings of all of wakeup_type, ccs_reader, cv_writers,
	   and various different numbers of waiters */
	for (waiters = 2; waiters <= TRANSFER_MAX_WAITERS; waiters <<= 1) {
		for (wakeup_type = 0; wakeup_type != 4; wakeup_type++) {
			for (cv_writers = -1; cv_writers != 3; cv_writers++) {
				for (ccs_reader = 0; ccs_reader != 2; ccs_reader++) {
					if (testing_verbose (t)) {
						TEST_LOG (t, ("transfer waiters %d wakeup_type %d  cv_writers %d  ccs_reader %d\n",
							      waiters, wakeup_type, cv_writers, ccs_reader));
					}
					bzero ((void *) cvt, sizeof (*cvt));

					/* Start the waiter threads that use condition variables. */
					for (i = 0; i < waiters-1; i++) {
						int is_reader = (cv_writers != -1 && i < waiters-1-cv_writers);
						closure_fork (closure_transfer_thread (&transfer_waiter_thread, cvt, i,
										       is_reader, 1/*use_cv*/));
						transfer_await_nonzero (&cvt->control_mu, &cvt->ready[i]);
					}
					/* Start the waiter thread that uses conditional critical sections. */
					closure_fork (closure_transfer_thread (&transfer_waiter_thread, cvt, i,
									       ccs_reader, 0/*use_cv*/));
					/* Wait for all waiters to enter their regions. */
					for (i = 0; i != waiters; i++) {
						transfer_await_nonzero (&cvt->control_mu, &cvt->ready[i]);
					}

					nsync_mu_lock (&cvt->mu);
					/* At this point, all the waiter threads are in waiting: 
					   they have set their ready[] flags, and have released cvt->mu. */

					/* Mark all the condition-variable as runnable,
					   and signal at least one of them.
					   This may wake more than one, depending on
					   the presence of readers, and the use of
					   signal vs broadcast.  */
					for (i = 0; i != waiters-1; i++) {
						cvt->cond[i] = 1;
					}
					if ((wakeup_type & after_region) == 0) {
						(*wakeup_func[wakeup_type & use_signal]) (&cvt->cv);
					}
					nsync_mu_unlock (&cvt->mu);
					if ((wakeup_type & after_region) != 0) {
						for (i = 0; i != waiters-1; i++) {
							(*wakeup_func[wakeup_type & use_signal]) (&cvt->cv);
						}
					}

					/* Wait for at least one woken waiter to proceed,
					   and at least one writer if there is one.  */
					nsync_mu_lock (&cvt->control_mu);
					while (are_all_below (&cvt->done[0], waiters-1, cv_writers!=0? 2 : 1)) {
						nsync_cv_wait (&cvt->done_cv, &cvt->control_mu);
					}
					nsync_mu_unlock (&cvt->control_mu);

					/* Wake all remaining threads. */
					nsync_cv_broadcast (&cvt->cv);
					transfer_set (&cvt->mu, &cvt->cond[waiters-1], 1);

					/* And wait for all to finish. */
					for (i = 0; i != waiters; i++) {
						transfer_await_nonzero (&cvt->control_mu, &cvt->done[i]);
					}

					if (testing_verbose (t)) {
						TEST_LOG (t, ("transfer waiters %d wakeup_type %d  cv_writers %d  ccs_reader %d complete\n",
							      waiters, wakeup_type, cv_writers, ccs_reader));
					}
				}
			}
		}
	}
}


/* --------------------------- */

int main (int argc, char *argv[]) {
	testing_base tb = testing_new (argc, argv, 0);
	TEST_RUN (tb, test_cv_producer_consumer0);
	TEST_RUN (tb, test_cv_producer_consumer1);
	TEST_RUN (tb, test_cv_producer_consumer2);
	TEST_RUN (tb, test_cv_producer_consumer3);
	TEST_RUN (tb, test_cv_producer_consumer4);
	TEST_RUN (tb, test_cv_producer_consumer5);
	TEST_RUN (tb, test_cv_producer_consumer6);
	TEST_RUN (tb, test_cv_deadline);
	TEST_RUN (tb, test_cv_cancel);
	TEST_RUN (tb, test_cv_debug);
	TEST_RUN (tb, test_cv_transfer);
	return (testing_base_exit (tb));
}
