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
#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "third_party/nsync/cv.h"
#include "third_party/nsync/mu.h"
#include "third_party/nsync/mu_wait.h"
#include "third_party/nsync/testing/closure.h"
#include "third_party/nsync/testing/smprintf.h"
#include "third_party/nsync/testing/testing.h"

/* A cv_stress_data represents the data used by the threads of the tests below. */
typedef struct cv_stress_data_s {
	nsync_mu mu; /* protects fields below */
	uintmax_t count;   /* incremented by the various threads */
	uintmax_t timeouts;   /* incremented on each timeout */
	uintmax_t reader_loops;   /* the number of loops executed by the reader threads, if any */

	unsigned refs; /* ref count: one per normal test thread, decremented on its exit */
	unsigned reader_refs; /* ref count: one per reader test thread, decremented on its exit */

	int use_cv;        /* threads are using CVs; under mu */
	nsync_cv count_is_imod4[4]; /* element i signalled when count==i mod 4 if use_cv non-0. */
	nsync_cv refs_is_zero;    /* signalled when refs==0 */
	nsync_cv reader_refs_is_zero;    /* signalled when reader_refs==0 */


	/* iterations per writer thread; under mu--increased until deadline exceeded */
	uintmax_t loop_count;

	/* number of various types of thread to create -- r/o after init */
	uintmax_t cv_threads_per_value;
	uintmax_t cv_reader_threads_per_value;
	uintmax_t mu_threads_per_value;
	uintmax_t mu_reader_threads_per_value;

	/* end times */
	nsync_time deadline;  /* r/o after init */
} cv_stress_data;

/* --------------------------- */

/* The delays in cv_stress_inc_loop(), cv_stress_reader_loop(), mu_stress_inc_loop(),
   and mu_stress_reader_loop() are uniformly distributed from 0 to
   STRESS_MAX_DELAY_MICROS-1 microseconds. */
#define STRESS_MAX_DELAY_MICROS (4000)                                    /* maximum delay */
#define STRESS_MEAN_DELAY_MICROS (STRESS_MAX_DELAY_MICROS / 2)               /* mean delay */
#define STRESS_EXPECT_TIMEOUTS_PER_SEC (1000000 / STRESS_MEAN_DELAY_MICROS) /* expect timeouts/s*/

/* Acquire s.mu, then increment s.count n times, each time
   waiting until condition is true.  Use a random delay between 0us and 999us
   for each wait; if the timeout expires, increment s.timeouts, and
   retry the wait.  Decrement s.refs before the returning. */
static void cv_stress_inc_loop (cv_stress_data *s, uintmax_t count_imod4) {
	uintmax_t i;
	nsync_mu_lock (&s->mu);
	s->use_cv = 1;
	nsync_mu_assert_held (&s->mu);
	for (i = 0; i != s->loop_count; i++) {
		nsync_mu_assert_held (&s->mu);
		while ((s->count & 3) != count_imod4) {
			nsync_time abs_deadline;
			abs_deadline = nsync_time_add (nsync_time_now (),
				nsync_time_us (rand () % STRESS_MAX_DELAY_MICROS));
			while (nsync_cv_wait_with_deadline (
					&s->count_is_imod4[count_imod4],
					&s->mu, abs_deadline, NULL) != 0 &&
			       (s->count&3) != count_imod4) {
				nsync_mu_assert_held (&s->mu);
				s->timeouts++;
				nsync_mu_assert_held (&s->mu);
				abs_deadline = nsync_time_add (nsync_time_now (),
				       nsync_time_us (rand () % STRESS_MAX_DELAY_MICROS));
			}
		}
		nsync_mu_assert_held (&s->mu);
		s->count++;
		nsync_cv_signal (&s->count_is_imod4[s->count&3]);
	}
	s->refs--;
	if (s->refs == 0) {
		if (s->reader_refs != 0) { /* wake any readers so they will exit */
			for (i = 0; i != 4; i++) {
				nsync_cv_broadcast (&s->count_is_imod4[i]);
			}
		}
		nsync_cv_signal (&s->refs_is_zero);
	}
	nsync_mu_assert_held (&s->mu);
	nsync_mu_unlock (&s->mu);
}

CLOSURE_DECL_BODY2 (cv_stress_inc_loop, cv_stress_data *, uintmax_t)

/* Acquires s.u in reader mode, and wait until a
   condition is true or a timeout occurs on a random wait between 0us and 999us, repeatedly.
   Every 16 times, release the reader lock, but immediately reacquire it.
   Once the count of threads running cv_stress_inc_loop() reaches zero (s.refs == 0),
   sum the number of loops complete into s.reader_loops, and
   the number of timeouts experience into s.timeouts.
   Then decrement s.reader_refs before returning. */
static void cv_stress_reader_loop (cv_stress_data *s, uintmax_t count_imod4) {
	uintmax_t loops;
	uintmax_t timeouts = 0;
	nsync_mu_lock (&s->mu);
	s->use_cv = 1;
	nsync_mu_unlock (&s->mu);
	nsync_mu_rlock (&s->mu);
	nsync_mu_rassert_held (&s->mu);
	loops = 0;
	while (s->refs != 0) {
		nsync_mu_rassert_held (&s->mu);
		while ((s->count&3) != count_imod4 && s->refs != 0) {
			nsync_time abs_deadline;
			abs_deadline = nsync_time_add (nsync_time_now (),
				nsync_time_us (rand () % STRESS_MAX_DELAY_MICROS));
			while (nsync_cv_wait_with_deadline (&s->count_is_imod4[count_imod4],
							    &s->mu, abs_deadline, NULL) != 0 &&
			       (s->count&3) != count_imod4 && s->refs != 0) {

				nsync_mu_rassert_held (&s->mu);
				timeouts++;
				abs_deadline = nsync_time_add (nsync_time_now (),
					nsync_time_us (rand () % STRESS_MAX_DELAY_MICROS));
			}
		}
		nsync_mu_rassert_held (&s->mu);
		loops++;
		if ((loops & 0xf) == 0) {
			nsync_mu_runlock (&s->mu);
			if ((loops & 0xfff) == 0) {
				nsync_time_sleep (nsync_time_ms (1));
			}
			nsync_mu_rlock (&s->mu);
		}
	}
	nsync_mu_rassert_held (&s->mu);
	nsync_mu_runlock (&s->mu);

	nsync_mu_lock (&s->mu);
	s->reader_loops += loops;
	s->timeouts += timeouts;
	s->reader_refs--;
	if (s->reader_refs == 0) {
		nsync_cv_signal (&s->reader_refs_is_zero);
	}
	nsync_mu_assert_held (&s->mu);
	nsync_mu_unlock (&s->mu);
}

CLOSURE_DECL_BODY2 (cv_stress_reader_loop, cv_stress_data *, uintmax_t)



/* --------------------------- */

/* These tests use the data structure cv_stress_data defined above.
   One test uses nsync_mu and nsync_cv, one nsync_mu and its conditional critical sections, while
   a third mixes conditional critical sections and condition variables; they
   all the routines above */

/* ---------------------------
   The various conditions that threads wait for on cv_stress_data. */
static int count_is0mod4 (const void *v) {
	const cv_stress_data *s = (const cv_stress_data *) v;
	nsync_mu_rassert_held (&s->mu);
	return ((s->count & 3) == 0);
}
static int count_is1mod4 (const void *v) {
	const cv_stress_data *s = (const cv_stress_data *) v;
	nsync_mu_rassert_held (&s->mu);
	return ((s->count & 3) == 1);
}
static int count_is2mod4 (const void *v) {
	const cv_stress_data *s = (const cv_stress_data *) v;
	nsync_mu_rassert_held (&s->mu);
	return ((s->count & 3) == 2);
}
static int count_is3mod4 (const void *v) {
	const cv_stress_data *s = (const cv_stress_data *) v;
	nsync_mu_rassert_held (&s->mu);
	return ((s->count & 3) == 3);
}
static int count_is0mod4or_refs_is0 (const void *v) {
	const cv_stress_data *s = (const cv_stress_data *) v;
	nsync_mu_rassert_held (&s->mu);
	return ((s->count&3) == 0 || s->refs == 0);
}
static int count_is1mod4or_refs_is0 (const void *v) {
	const cv_stress_data *s = (const cv_stress_data *) v;
	nsync_mu_rassert_held (&s->mu);
	return ((s->count&3) == 1 || s->refs == 0);
}
static int count_is2mod4or_refs_is0 (const void *v) {
	const cv_stress_data *s = (const cv_stress_data *) v;
	nsync_mu_rassert_held (&s->mu);
	return ((s->count&3) == 2 || s->refs == 0);
}
static int count_is3mod4or_refs_is0 (const void *v) {
	const cv_stress_data *s = (const cv_stress_data *) v;
	nsync_mu_rassert_held (&s->mu);
	return ((s->count&3) == 3 || s->refs == 0);
}

/* --------------------------- */

typedef int (*condition_func) (const void *);

/* Acquire s.mu, then increment s.count n times, each time
   waiting until condition is true.  Use a random delay between 0us and 999us
   for each wait; if the timeout expires, increment s.timeouts, and
   the retry the wait.  Decrement s.refs before returning. */
static void mu_stress_inc_loop (cv_stress_data *s, condition_func condition,
				const void *condition_arg) {
	uintmax_t i;
	nsync_mu_lock (&s->mu);
	nsync_mu_assert_held (&s->mu);
	for (i = 0; i != s->loop_count; i++) {
		nsync_time abs_deadline;
		nsync_mu_assert_held (&s->mu);

		abs_deadline = nsync_time_add (nsync_time_now (),
			nsync_time_us (rand () % STRESS_MAX_DELAY_MICROS));
		while (nsync_mu_wait_with_deadline (&s->mu, condition, condition_arg, NULL,
						    abs_deadline, NULL) != 0) {
			nsync_mu_assert_held (&s->mu);
			s->timeouts++;
			nsync_mu_assert_held (&s->mu);
			abs_deadline = nsync_time_add (nsync_time_now (),
				nsync_time_us (rand () % STRESS_MAX_DELAY_MICROS));
		}

		nsync_mu_assert_held (&s->mu);
		s->count++;
		if (s->use_cv) {
			nsync_cv_signal (&s->count_is_imod4[s->count&3]);
		}
	}
	s->refs--;
	if (s->refs == 0) {
		if (s->use_cv && s->reader_refs != 0) { /* wake any readers so they will exit */
			for (i = 0; i != 4; i++) {
				nsync_cv_broadcast (&s->count_is_imod4[i]);
			}
		}
		nsync_cv_signal (&s->refs_is_zero);
	}
	nsync_mu_assert_held (&s->mu);
	nsync_mu_unlock (&s->mu);
}

CLOSURE_DECL_BODY3 (mu_stress_inc_loop, cv_stress_data *, condition_func, const void *)


/* Acquire s.u in reader mode, and wait until a
   condition is true or a timeout occurs on a random wait between 0us and 999us, repeatedly.
   Every 16 times, release the reader lock, but immediately reacquire it.
   Once the count of threads running mu_stress_inc_loop() reaches zero (s.refs == 0),
   sum the number of loops completed into s.reader_loops, and
   the number of timeouts it experienced into s.timeouts.
   Then decrement s.reader_refs before returning. */
static void mu_stress_reader_loop (cv_stress_data *s, condition_func condition,
				   const void *condition_arg) {
	uintmax_t loops;
	uintmax_t timeouts = 0;
	nsync_mu_rlock (&s->mu);
	nsync_mu_rassert_held (&s->mu);
	loops = 0;
	while (s->refs != 0) {
		nsync_time abs_deadline;
		nsync_mu_rassert_held (&s->mu);
		abs_deadline = nsync_time_add (nsync_time_now (),
			nsync_time_us (rand () % STRESS_MAX_DELAY_MICROS));
		while (nsync_mu_wait_with_deadline (&s->mu, condition, condition_arg, NULL,
						    abs_deadline, NULL) != 0) {
			nsync_mu_rassert_held (&s->mu);
			s->timeouts++;
			nsync_mu_rassert_held (&s->mu);
			abs_deadline = nsync_time_add (nsync_time_now (),
				nsync_time_us (rand () % STRESS_MAX_DELAY_MICROS));
		}

		nsync_mu_rassert_held (&s->mu);
		loops++;
		if ((loops & 0xf) == 0) {
			nsync_mu_runlock (&s->mu);
			if ((loops & 0xfff) == 0) {
				nsync_time_sleep (nsync_time_ms (1));
			}
			nsync_mu_rlock (&s->mu);
		}
	}
	nsync_mu_rassert_held (&s->mu);
	nsync_mu_runlock (&s->mu);

	nsync_mu_lock (&s->mu);
	s->reader_loops += loops;
	s->timeouts += timeouts;
	s->reader_refs--;
	if (s->reader_refs == 0) {
		nsync_cv_signal (&s->reader_refs_is_zero);
	}
	nsync_mu_assert_held (&s->mu);
	nsync_mu_unlock (&s->mu);
}

CLOSURE_DECL_BODY3 (mu_stress_reader_loop, cv_stress_data *, condition_func, const void *)

static const condition_func is_n_mod_4[] = {
	&count_is0mod4,
	&count_is1mod4,
	&count_is2mod4,
	&count_is3mod4
};
static const condition_func is_n_mod_4_or_refs0[] = {
	&count_is0mod4or_refs_is0,
	&count_is1mod4or_refs_is0,
	&count_is2mod4or_refs_is0,
	&count_is3mod4or_refs_is0
};

/* Create cv_threads_per_value threads using cv_stress_inc_loop(),
   and mu_threads_per_value threads using mu_stress_inc_loop(), all trying to
   increment s.count from 1 to 2 mod 4, plus the same from 2 to 3 mod 4, and
   again from 3 to 0 mod 4, using random delays in their waits.  Sleep a few seconds,
   ensuring many random timeouts by these threads, because there is no thread
   incrementing s.count from 0 (which is 0 mod 4).  Then create
   cv_threads_per_value threads using cv_stress_inc_loop(), and mu_threads_per_value
   threads using mu_stress_inc_loop(), all trying to increment s.count from 0 to 1
   mod 4.  This allows all the threads to run to completion, since there are
   equal numbers for each condition.

   At the same time, create cv_reader_threads_per_value threads using cv_stress_reader_loop
   and mu_reader_threads_per_value using mu_stress_reader_loop, for each of the transitions
   0 to 1, 1, to 2, 2 to 3, and 3 to 0 mod 4.

   All these loops count how many timeouts they encounter.  The reader loops
   count how many times they manage to run.

   These counts are tested against expected values.

   Finally, it waits for all threads to exit.

   It returns whether the deadlie has expired. */
static int run_stress_test (cv_stress_data *s, testing t,
			    const char *test_name) {
	int done = 0;
	uintmax_t expected_timeouts;
	uintmax_t timeouts_seen;
	uintmax_t expected_count;
	uintmax_t i;
	static const int sleep_seconds = 1;

	nsync_mu_lock (&s->mu);
	nsync_mu_assert_held (&s->mu);
	/* Create threads trying to increment from 1, 2, and 3 mod 4.
	   They will continually hit their timeouts because s.count==0 */
	for (i = 0; i != s->cv_threads_per_value; i++) {
		nsync_mu_assert_held (&s->mu);
		s->refs++;
		closure_fork (closure_cv_stress_inc_loop (&cv_stress_inc_loop, s, 1));
		s->refs++;
		closure_fork (closure_cv_stress_inc_loop (&cv_stress_inc_loop, s, 2));
		s->refs++;
		closure_fork (closure_cv_stress_inc_loop (&cv_stress_inc_loop, s, 3));
	}
	for (i = 0; i != s->cv_reader_threads_per_value; i++) {
		nsync_mu_assert_held (&s->mu);
		s->reader_refs++;
		closure_fork (closure_cv_stress_reader_loop (&cv_stress_reader_loop, s, 1));
		s->reader_refs++;
		closure_fork (closure_cv_stress_reader_loop (&cv_stress_reader_loop, s, 2));
		s->reader_refs++;
		closure_fork (closure_cv_stress_reader_loop (&cv_stress_reader_loop, s, 3));
	}
	for (i = 0; i != s->mu_threads_per_value; i++) {
		nsync_mu_assert_held (&s->mu);
		s->refs++;
		closure_fork (closure_mu_stress_inc_loop (&mu_stress_inc_loop,
							  s, is_n_mod_4[1], s));
		s->refs++;
		closure_fork (closure_mu_stress_inc_loop (&mu_stress_inc_loop,
							  s, is_n_mod_4[2], s));
		s->refs++;
		closure_fork (closure_mu_stress_inc_loop (&mu_stress_inc_loop,
							  s, is_n_mod_4[3], s));
	}
	for (i = 0; i != s->mu_reader_threads_per_value; i++) {
		nsync_mu_assert_held (&s->mu);
		s->reader_refs++;
		closure_fork (closure_mu_stress_reader_loop (&mu_stress_reader_loop,
							     s, is_n_mod_4_or_refs0[1], s));
		s->reader_refs++;
		closure_fork (closure_mu_stress_reader_loop (&mu_stress_reader_loop,
							     s, is_n_mod_4_or_refs0[2], s));
		s->reader_refs++;
		closure_fork (closure_mu_stress_reader_loop (&mu_stress_reader_loop,
							     s, is_n_mod_4_or_refs0[3], s));
	}
	nsync_mu_assert_held (&s->mu);
	nsync_mu_unlock (&s->mu);

	/* Sleep a while to cause many timeouts. */
	nsync_time_sleep (nsync_time_ms (sleep_seconds * 1000));

	nsync_mu_lock (&s->mu);
	nsync_mu_assert_held (&s->mu);

	/* Check that approximately the right number of timeouts have occurred.
	   The 3 below is the three classes of thread produced before the Sleep().
	   The factor of 1/8 is to allow for randomness and slow test machines. */
	expected_timeouts = (s->cv_threads_per_value + s->mu_threads_per_value) * 3 *
			     sleep_seconds * STRESS_EXPECT_TIMEOUTS_PER_SEC / 8;
	timeouts_seen = s->timeouts;
	/* actual check is below. */

	/* Create the threads that increment from 0 mod 4.   s.count will then be incremented. */
	for (i = 0; i != s->cv_threads_per_value; i++) {
		nsync_mu_assert_held (&s->mu);
		s->refs++;
		closure_fork (closure_cv_stress_inc_loop (&cv_stress_inc_loop, s, 0));
	}
	for (i = 0; i != s->cv_reader_threads_per_value; i++) {
		nsync_mu_assert_held (&s->mu);
		s->reader_refs++;
		closure_fork (closure_cv_stress_reader_loop (&cv_stress_reader_loop, s, 0));
	}
	for (i = 0; i != s->mu_threads_per_value; i++) {
		nsync_mu_assert_held (&s->mu);
		s->refs++;
		closure_fork (closure_mu_stress_inc_loop (&mu_stress_inc_loop,
							  s, is_n_mod_4[0], s));
	}
	for (i = 0; i != s->mu_reader_threads_per_value; i++) {
		nsync_mu_assert_held (&s->mu);
		s->reader_refs++;
		closure_fork (closure_mu_stress_reader_loop (&mu_stress_reader_loop,
							     s, is_n_mod_4_or_refs0[0], s));
	}

	/* wait for threads to exit. */
	nsync_mu_assert_held (&s->mu);
	while (s->refs != 0) {
		nsync_cv_wait (&s->refs_is_zero, &s->mu);
	}
	while (s->reader_refs != 0) {
		nsync_cv_wait (&s->reader_refs_is_zero, &s->mu);
	}

	nsync_mu_assert_held (&s->mu);
	nsync_mu_unlock (&s->mu);

	if (nsync_time_cmp (s->deadline, nsync_time_now ()) < 0) {
		if (timeouts_seen < expected_timeouts && !testing_is_uniprocessor (t)) {
			TEST_ERROR (t, ("%s: expected more than %d timeouts, got %d",
				   test_name, expected_timeouts, timeouts_seen));
		}

		/* Check that s.count has the right value. */
		expected_count = s->loop_count * (s->cv_threads_per_value +
						  s->mu_threads_per_value) * 4;
		if (s->count != expected_count) {
			TEST_ERROR (t, ("%s: expected to increment s->count to %d, got %d",
				   test_name, expected_count, s->count));
		}

		/* Some timeouts should have happened while the counts were being incremented. */
		expected_timeouts = timeouts_seen + 1;
		if (s->timeouts < expected_timeouts) {
			TEST_ERROR (t, ("%s: expected more than %d timeouts, got %d",
				   test_name, expected_timeouts, s->timeouts));
		}
		done = 1;
	}
	return (done);
}

/* Test many threads using a single lock, using
   condition variables and timeouts.  See run_stress_test() for details. */
static void test_cv_timeout_stress (testing t) {
	uintmax_t loop_count = 3;
	cv_stress_data s;
	nsync_time deadline;
	deadline = nsync_time_add (nsync_time_now (), nsync_time_ms (5000));
	do {
		bzero ((void *) &s, sizeof (s));
		s.loop_count = loop_count;
		s.cv_threads_per_value = 4;
		s.cv_reader_threads_per_value = 2;
		s.mu_threads_per_value = 0;
		s.mu_reader_threads_per_value = 0;
		s.deadline = deadline;
		loop_count *= 2;
	} while (!run_stress_test (&s, t, "test_cv_timeout_stress"));
}


/* Test many threads using a single lock, using
   conditional critical sections and timeouts. See run_stress_test() for details. */
static void test_mu_timeout_stress (testing t) {
	uintmax_t loop_count = 3;
	cv_stress_data s;
	nsync_time deadline;
	deadline = nsync_time_add (nsync_time_now (), nsync_time_ms (5000));
	do {
		bzero ((void *) &s, sizeof (s));
		s.loop_count = loop_count;
		s.cv_threads_per_value = 0;
		s.cv_reader_threads_per_value = 0;
		s.mu_threads_per_value = 4;
		s.mu_reader_threads_per_value = 2;
		s.deadline = deadline;
		loop_count *= 2;
	} while (!run_stress_test (&s, t, "test_mu_timeout_stress"));
}

/* Like test_mu_timeout_stress() but has both threads that use conditional
   critical sections and threads that use condition variables, to ensure that
   they work together.  */
static void test_mu_cv_timeout_stress (testing t) {
	uintmax_t loop_count = 3;
	cv_stress_data s;
	nsync_time deadline;
	deadline = nsync_time_add (nsync_time_now (), nsync_time_ms (5000));
	do {
		bzero ((void *) &s, sizeof (s));
		s.loop_count = loop_count;
		s.cv_threads_per_value = 4;
		s.cv_reader_threads_per_value = 1;
		s.mu_threads_per_value = 4;
		s.mu_reader_threads_per_value = 1;
		s.deadline = deadline;
		loop_count *= 2;
	} while (!run_stress_test (&s, t, "test_mu_cv_timeout_stress"));
}

int main (int argc, char *argv[]) {
	testing_base tb = testing_new (argc, argv, 0);
	TEST_RUN (tb, test_cv_timeout_stress);
	TEST_RUN (tb, test_mu_timeout_stress);
	TEST_RUN (tb, test_mu_cv_timeout_stress);
	return (testing_base_exit (tb));
}
