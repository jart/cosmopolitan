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
#include "third_party/nsync/mu_wait.h"
#include "libc/errno.h"
#include "libc/str/str.h"
#include "third_party/nsync/mu.h"
#include "third_party/nsync/note.h"
#include "third_party/nsync/testing/closure.h"
#include "third_party/nsync/testing/smprintf.h"
#include "third_party/nsync/testing/testing.h"
#include "third_party/nsync/testing/time_extra.h"
#include "third_party/nsync/time.h"

/* --------------------------- */

/* A FIFO queue with up to limit elements.
   The storage for the queue expands as necessary up to limit. */
typedef struct mu_queue_s {
	int limit;     /* max value of count---should not be changed after initialization */
	nsync_mu mu;   /* protects fields below */
	int pos;       /* index of first in-use element */
	int count;     /* number of elements in use */
	void *data[1]; /* in use elements are data[pos, ..., (pos+count-1)%limit] */
} mu_queue;

/* Return a pointer to new mu_queue. */
static mu_queue *mu_queue_new (int limit) {
	mu_queue *q;
	int size = offsetof (struct mu_queue_s, data) + sizeof (q->data[0]) * limit;
	q = (mu_queue *) malloc (size);
	bzero ((void *) q, size);
	q->limit = limit;
	return (q);
}

static int mu_queue_non_empty (const void *v) {
	const mu_queue *q = (const mu_queue *) v;
	return (q->count != 0);
}
static int mu_queue_non_full (const void *v) {
	const mu_queue *q = (const mu_queue *) v;
	return (q->count != q->limit);
}

/* Add v to the end of the FIFO *q and return non-zero, or if the FIFO already
   has limit elements and continues to do so until abs_deadline, do nothing and
   return 0. */
static int mu_queue_put (mu_queue *q, void *v, nsync_time abs_deadline) {
	int added = 0;
	nsync_mu_lock (&q->mu);
	if (nsync_mu_wait_with_deadline (&q->mu, &mu_queue_non_full,
					 q, NULL, abs_deadline, NULL) == 0) {
		int i = q->pos + q->count;
		if (q->count == q->limit) {
			testing_panic ("q->count == q->limit");
		}
		if (q->limit <= i) {
			i -= q->limit;
		}
		q->data[i] = v;
		q->count++;
		added = 1;
	}
	nsync_mu_unlock (&q->mu);
	return (added);
}

/* Remove the first value from the front of the FIFO *q and return it,
   or if the FIFO is empty and continues to be so until abs_deadline,
   do nothing and return NULL. */
static void *mu_queue_get (mu_queue *q, nsync_time abs_deadline) {
	void *v = NULL;
	nsync_mu_lock (&q->mu);
	if (nsync_mu_wait_with_deadline (&q->mu, &mu_queue_non_empty,
					 q, NULL, abs_deadline, NULL) == 0) {
		if (q->count == 0) {
			testing_panic ("q->count == 0");
		}
		v = q->data[q->pos];
		q->data[q->pos] = NULL;
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
static void producer_mu_n (testing t, mu_queue *q, int start, int count) {
	int i;
	for (i = 0; i != count; i++) {
		if (!mu_queue_put (q, INT_TO_PTR ((start+i)*3), nsync_time_no_deadline)) {
			TEST_FATAL (t, ("mu_queue_put() returned 0 with no deadline"));
		}
	}
}

CLOSURE_DECL_BODY4 (producer_mu_n, testing , mu_queue *, int, int)

/* Get count integers from *q, and check that they are in the
   sequence start*3, (start+1)*3, (start+2)*3, .... */
static void consumer_mu_n (testing t, mu_queue *q, int start, int count) {
	int i;
	for (i = 0; i != count; i++) {
		void *v = mu_queue_get (q, nsync_time_no_deadline);
		int x;
		if (v == NULL) {
			TEST_FATAL (t, ("mu_queue_get() returned 0 with no deadline"));
		}
		x = PTR_TO_INT (v);
		if (x != (start+i)*3) {
			TEST_FATAL (t, ("mu_queue_get() returned bad value; want %d, got %d",
				   (start+i)*3, x));
		}
	}
}

/* The number of elements passed from producer to consumer in the
   test_mu_producer_consumer*() tests below. */
#define MU_PRODUCER_CONSUMER_N (100000)

/* Send a stream of integers from a producer thread to
   a consumer thread via a queue with limit 10**0. */
static void test_mu_producer_consumer0 (testing t) {
	mu_queue *q = mu_queue_new (1);
	closure_fork (closure_producer_mu_n (&producer_mu_n, t, q, 0, MU_PRODUCER_CONSUMER_N));
	consumer_mu_n (t, q, 0, MU_PRODUCER_CONSUMER_N);
	free (q);
}

/* Send a stream of integers from a producer thread to
   a consumer thread via a queue with limit 10**1. */
static void test_mu_producer_consumer1 (testing t) {
	mu_queue *q = mu_queue_new (10);
	closure_fork (closure_producer_mu_n (&producer_mu_n, t, q, 0, MU_PRODUCER_CONSUMER_N));
	consumer_mu_n (t, q, 0, MU_PRODUCER_CONSUMER_N);
	free (q);
}

/* Send a stream of integers from a producer thread to
   a consumer thread via a queue with limit 10**2. */
static void test_mu_producer_consumer2 (testing t) {
	mu_queue *q = mu_queue_new (100);
	closure_fork (closure_producer_mu_n (&producer_mu_n, t, q, 0, MU_PRODUCER_CONSUMER_N));
	consumer_mu_n (t, q, 0, MU_PRODUCER_CONSUMER_N);
	free (q);
}

/* Send a stream of integers from a producer thread to
   a consumer thread via a queue with limit 10**3. */
static void test_mu_producer_consumer3 (testing t) {
	mu_queue *q = mu_queue_new (1000);
	closure_fork (closure_producer_mu_n (&producer_mu_n, t, q, 0, MU_PRODUCER_CONSUMER_N));
	consumer_mu_n (t, q, 0, MU_PRODUCER_CONSUMER_N);
	free (q);
}

/* Send a stream of integers from a producer thread to
   a consumer thread via a queue with limit 10**4. */
static void test_mu_producer_consumer4 (testing t) {
	mu_queue *q = mu_queue_new (10000);
	closure_fork (closure_producer_mu_n (&producer_mu_n, t, q, 0, MU_PRODUCER_CONSUMER_N));
	consumer_mu_n (t, q, 0, MU_PRODUCER_CONSUMER_N);
	free (q);
}

/* Send a stream of integers from a producer thread to
   a consumer thread via a queue with limit 10**5. */
static void test_mu_producer_consumer5 (testing t) {
	mu_queue *q = mu_queue_new (100000);
	closure_fork (closure_producer_mu_n (&producer_mu_n, t, q, 0, MU_PRODUCER_CONSUMER_N));
	consumer_mu_n (t, q, 0, MU_PRODUCER_CONSUMER_N);
	free (q);
}

/* Send a stream of integers from a producer thread to
   a consumer thread via a queue with limit 10**6. */
static void test_mu_producer_consumer6 (testing t) {
	mu_queue *q = mu_queue_new (1000000);
	closure_fork (closure_producer_mu_n (&producer_mu_n, t, q, 0, MU_PRODUCER_CONSUMER_N));
	consumer_mu_n (t, q, 0, MU_PRODUCER_CONSUMER_N);
	free (q);
}

/* A perpetually false wait condition. */
static int false_condition (const void *v) {
	return (0);
}

/* The following values control how aggressively we police the timeout. */
#define TOO_EARLY_MS 1  
#define TOO_LATE_MS 100   /* longer, to accommodate scheduling delays */
#define TOO_LATE_ALLOWED 25       /* number of iterations permitted to violate too_late */

/* Check timeouts on a mu wait_with_deadline(). */
static void test_mu_deadline (testing t) {
	int i;
	int too_late_violations;
	nsync_mu mu;
	nsync_time too_early;
	nsync_time too_late;

	nsync_mu_init (&mu);
	too_early = nsync_time_ms (TOO_EARLY_MS);
	too_late = nsync_time_ms (TOO_LATE_MS);
	too_late_violations = 0;
	nsync_mu_lock (&mu);;
	for (i = 0; i != 50; i++) {
		nsync_time end_time;
		nsync_time start_time;
		nsync_time expected_end_time;
		start_time = nsync_time_now ();
		expected_end_time = nsync_time_add (start_time, nsync_time_ms (87));
		if (nsync_mu_wait_with_deadline (&mu, &false_condition, NULL, NULL,
						 expected_end_time, NULL) != ETIMEDOUT) {
			TEST_FATAL (t, ("nsync_mu_wait() returned non-expired for a timeout"));
		}
		end_time = nsync_time_now ();
		if (nsync_time_cmp (end_time, nsync_time_sub (expected_end_time, too_early)) < 0) {
			char *elapsed_str = nsync_time_str (nsync_time_sub (expected_end_time, end_time), 2);
			TEST_ERROR (t, ("nsync_mu_wait() returned %s too early", elapsed_str));
			free (elapsed_str);
		}
		if (nsync_time_cmp (nsync_time_add (expected_end_time, too_late),  end_time) < 0) {
			too_late_violations++;
		}
	}
	nsync_mu_unlock (&mu);
	if (too_late_violations > TOO_LATE_ALLOWED) {
		TEST_ERROR (t, ("nsync_mu_wait() returned too late %d (> %d) times",
			   too_late_violations, TOO_LATE_ALLOWED));
	}
}

/* Check cancellations on a mu wait_with_deadline(). */
static void test_mu_cancel (testing t) {
	int i;
	nsync_time future_time;
	int too_late_violations;
	nsync_mu mu;
	nsync_time too_early;
	nsync_time too_late;

	nsync_mu_init (&mu);
	too_early = nsync_time_ms (TOO_EARLY_MS);
	too_late = nsync_time_ms (TOO_LATE_MS);

	/* The loops below cancel after 87 milliseconds, like the timeout tests above. */

	future_time = nsync_time_add (nsync_time_now (), nsync_time_ms (3600000)); /* test cancels with timeout */

	too_late_violations = 0;
	nsync_mu_lock (&mu);
	for (i = 0; i != 50; i++) {
		nsync_time end_time;
		nsync_time start_time;
		nsync_time expected_end_time;
		int x;
		nsync_note cancel;

		start_time = nsync_time_now ();
		expected_end_time = nsync_time_add (start_time, nsync_time_ms (87));
		cancel = nsync_note_new (NULL, expected_end_time);

		x = nsync_mu_wait_with_deadline (&mu, &false_condition, NULL, NULL,
						 future_time, cancel);
		if (x != ECANCELED) {
			TEST_FATAL (t, ("nsync_mu_wait() return non-cancelled (%d) for "
				   "a cancellation; expected %d",
				   x, ECANCELED));
		}
		end_time = nsync_time_now ();
		if (nsync_time_cmp (end_time, nsync_time_sub (expected_end_time, too_early)) < 0) {
			char *elapsed_str = nsync_time_str (nsync_time_sub (expected_end_time, end_time), 2);
			TEST_ERROR (t, ("nsync_mu_wait() returned %s too early", elapsed_str));
			free (elapsed_str);
		}
		if (nsync_time_cmp (nsync_time_add (expected_end_time, too_late), end_time) < 0) {
			too_late_violations++;
		}

		/* Check that an already cancelled wait returns immediately. */
		start_time = nsync_time_now ();
		x = nsync_mu_wait_with_deadline (&mu, &false_condition, NULL, NULL,
						 nsync_time_no_deadline, cancel);
		if (x != ECANCELED) {
			TEST_FATAL (t, ("nsync_mu_wait() returned non-cancelled for a "
				   "cancellation; expected %d",
				   x, ECANCELED));
		}
		end_time = nsync_time_now ();
		if (nsync_time_cmp (end_time, start_time) < 0) {
			char *elapsed_str = nsync_time_str (nsync_time_sub (expected_end_time, end_time), 2);
			TEST_ERROR (t, ("nsync_mu_wait() returned %s too early", elapsed_str));
			free (elapsed_str);
		}
		if (nsync_time_cmp (nsync_time_add (start_time, too_late), end_time) < 0) {
			too_late_violations++;
		}
		nsync_note_free (cancel);
	}
	nsync_mu_unlock (&mu);
	if (too_late_violations > TOO_LATE_ALLOWED) {
		TEST_ERROR (t, ("nsync_mu_wait() returned too late %d (> %d) times",
			   too_late_violations, TOO_LATE_ALLOWED));
	}
}

int main (int argc, char *argv[]) {
	testing_base tb = testing_new (argc, argv, 0);
	TEST_RUN (tb, test_mu_producer_consumer0);
	TEST_RUN (tb, test_mu_producer_consumer1);
	TEST_RUN (tb, test_mu_producer_consumer2);
	TEST_RUN (tb, test_mu_producer_consumer3);
	TEST_RUN (tb, test_mu_producer_consumer4);
	TEST_RUN (tb, test_mu_producer_consumer5);
	TEST_RUN (tb, test_mu_producer_consumer6);
	TEST_RUN (tb, test_mu_deadline);
	TEST_RUN (tb, test_mu_cancel);
	return (testing_base_exit (tb));
}
