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
#include "third_party/nsync/mu.h"
#include "libc/calls/calls.h"
#include "libc/str/str.h"
#include "libc/thread/thread.h"
#include "third_party/nsync/cv.h"
#include "third_party/nsync/mu_wait.h"
#include "third_party/nsync/testing/closure.h"
#include "third_party/nsync/testing/smprintf.h"
#include "third_party/nsync/testing/testing.h"
#include "third_party/nsync/testing/time_extra.h"

/* The state shared between the threads in each of the tests below. */
typedef struct test_data_s {
	testing t;
	int n_threads;  /* Number of test threads; constant after init. */
	int loop_count; /* Iteration count for each test thread; constant after init */
	
	/* mu_in_use protects i, id, loop_count, and finished_threads. */
	void *mu_in_use; /* points at mu, mutex, or rwmutex depending on which is in use. */
	void (*lock) (void *);  /* operations on mu_in_use */
	void (*unlock) (void *);
	
	nsync_mu mu;
	pthread_mutex_t mutex;
	pthread_rwlock_t rwmutex;
	
	int i; /* counter incremented by test loops. */
	volatile int id; /* id of current lock-holding thread in some tests. */
	
	nsync_cv done; /* Signalled when finished_threads==n_threads. */
	int finished_threads;      /* Count of threads that have finished. */
} test_data;

/* Indicate that a thread has finished its operations on test_data
   by incrementing td.finished_threads, and signal td.done when it reaches td.n_threads.
   See test_data_wait_for_all_threads(). */
static void test_data_thread_finished (test_data *td) {
	(*td->lock) (td->mu_in_use);
	td->finished_threads++;
	if (td->finished_threads == td->n_threads) {
		nsync_cv_broadcast (&td->done);
	}
	(*td->unlock) (td->mu_in_use);
}

/* Wait until all td.n_threads have called test_data_thread_finished(),
   and then return. */
static void test_data_wait_for_all_threads (test_data *td) {
	(*td->lock) (td->mu_in_use);
	while (td->finished_threads != td->n_threads) {
		nsync_cv_wait_with_deadline_generic (&td->done, td->mu_in_use,
						     td->lock, td->unlock,
						     nsync_time_no_deadline, NULL);
	}
	(*td->unlock) (td->mu_in_use);
}

/* --------------------------------------- */

/* The body of each thread executed by test_mu_nthread()
   and test_mutex_nthread.
   *td represents the test data that the threads share, and id is an integer
   unique to each test thread. */
static void counting_loop (test_data *td, int id) {
	int n = td->loop_count;
	int i = 0;
	for (i = 0; i != n; i++) {
		(*td->lock) (td->mu_in_use);
		td->id = id;
		td->i++;
		if (td->id != id) {
			testing_panic ("td->id != id");
		}
		(*td->unlock) (td->mu_in_use);
	}
	test_data_thread_finished (td);
}

CLOSURE_DECL_BODY2 (counting, test_data *, int)

/* Versions of nsync_mu_lock() and nsync_mu_unlock() that take "void *"
   arguments, to avoid call through a function pointer of a different type,
   which is undefined.  */
static void void_mu_lock (void *mu) {
	nsync_mu_lock ((nsync_mu *) mu);
}
static void void_mu_unlock (void *mu) {
	nsync_mu_unlock((nsync_mu *) mu);
}

/* Create a few threads, each of which increments an
   integer a fixed number of times, using an nsync_mu for mutual exclusion.
   It checks that the integer is incremented the correct number of times. */
static void test_mu_nthread (testing t) {
	int loop_count = 100000;
	nsync_time deadline;
	deadline = nsync_time_add (nsync_time_now (), nsync_time_ms (1500));
	do {
		int i;
		test_data td;
		bzero ((void *) &td, sizeof (td));
		td.t = t;
		td.n_threads = 5;
		td.loop_count = loop_count;
		td.mu_in_use = &td.mu;
		td.lock = &void_mu_lock;
		td.unlock = &void_mu_unlock;
		for (i = 0; i != td.n_threads; i++) {
			closure_fork (closure_counting (&counting_loop, &td, i));
		}
		test_data_wait_for_all_threads (&td);
		if (td.i != td.n_threads*td.loop_count) {
			TEST_FATAL (t, ("test_mu_nthread final count inconsistent: want %d, got %d",
				   td.n_threads*td.loop_count, td.i));
		}
		loop_count *= 2;
	} while (nsync_time_cmp (nsync_time_now (), deadline) < 0);
}

/* void pthread_mutex_lock */
static void void_pthread_mutex_lock (void *mu) {
        pthread_mutex_lock ((pthread_mutex_t *) mu);
}

/* void pthread_mutex_unlock */
static void void_pthread_mutex_unlock (void *mu) {
        pthread_mutex_unlock ((pthread_mutex_t *) mu);
}

/* Create a few threads, each of which increments an
   integer a fixed number of times, using a pthread_mutex_t for mutual exclusion.
   It checks that the integer is incremented the correct number of times. */
static void test_mutex_nthread (testing t) {
	int loop_count = 100000;
	nsync_time deadline;
	deadline = nsync_time_add (nsync_time_now (), nsync_time_ms (1500));
	do {
		int i;
		test_data td;
		bzero ((void *) &td, sizeof (td));
		td.t = t;
		td.n_threads = 5;
		td.loop_count = loop_count;
		td.mu_in_use = &td.mutex;
		td.lock = &void_pthread_mutex_lock;
		td.unlock = &void_pthread_mutex_unlock;
		pthread_mutex_init (&td.mutex, NULL);
		for (i = 0; i != td.n_threads; i++) {
			closure_fork (closure_counting (&counting_loop, &td, i));
		}
		test_data_wait_for_all_threads (&td);
		if (td.i != td.n_threads*td.loop_count) {
			TEST_FATAL (t, ("test_mutex_nthread final count inconsistent: want %d, got %d",
				   td.n_threads*td.loop_count, td.i));
		}
		pthread_mutex_destroy (&td.mutex);
		loop_count *= 2;
	} while (nsync_time_cmp (nsync_time_now (), deadline) < 0);
}

/* void pthread_rwlock_wrlock */
static void void_pthread_rwlock_wrlock (void *mu) {
        pthread_rwlock_wrlock ((pthread_rwlock_t *) mu);
}

/* void pthread_rwlock_unlock */
static void void_pthread_rwlock_unlock (void *mu) {
        pthread_rwlock_unlock ((pthread_rwlock_t *) mu);
}

/* Create a few threads, each of which increments an
   integer a fixed number of times, using a pthread_rwlock_t for mutual exclusion.
   It checks that the integer is incremented the correct number of times. */
static void test_rwmutex_nthread (testing t) {
	int loop_count = 100000;
	nsync_time deadline;
	deadline = nsync_time_add (nsync_time_now (), nsync_time_ms (1500));
	do {
		int i;
		test_data td;
		bzero ((void *) &td, sizeof (td));
		td.t = t;
		td.n_threads = 5;
		td.loop_count = loop_count;
		td.mu_in_use = &td.rwmutex;
		td.lock = &void_pthread_rwlock_wrlock;
		td.unlock = &void_pthread_rwlock_unlock;
		pthread_rwlock_init (&td.rwmutex, NULL);
		for (i = 0; i != td.n_threads; i++) {
			closure_fork (closure_counting (&counting_loop, &td, i));
		}
		test_data_wait_for_all_threads (&td);
		if (td.i != td.n_threads*td.loop_count) {
			TEST_FATAL (t, ("test_mutex_nthread final count inconsistent: want %d, got %d",
				   td.n_threads*td.loop_count, td.i));
		}
		pthread_rwlock_destroy (&td.rwmutex);
		loop_count *= 2;
	} while (nsync_time_cmp (nsync_time_now (), deadline) < 0);
}

/* --------------------------------------- */

/* The body of each thread executed by test_try_mu_nthread().
   *td represents the test data that the threads share, and id is an integer
   unique to each test thread. */
static void counting_loop_try_mu (test_data *td, int id) {
	int i;
	int n = td->loop_count;
	for (i = 0; i != n; i++) {
		while (!nsync_mu_trylock (&td->mu)) {
			pthread_yield ();
		}
		td->id = id;
		td->i++;
		if (td->id != id) {
			testing_panic ("td->id != id");
		}
		n = td->loop_count;
		nsync_mu_unlock (&td->mu);
	}
	test_data_thread_finished (td);
}

/* Test that acquiring an nsync_mu with nsync_mu_trylock()
   using several threads provides mutual exclusion. */
static void test_try_mu_nthread (testing t) {
	int loop_count = 100000;
	nsync_time deadline;
	deadline = nsync_time_add (nsync_time_now (), nsync_time_ms (1500));
	do {
		int i;
		test_data td;
		bzero ((void *) &td, sizeof (td));
		td.t = t;
		td.n_threads = 5;
		td.loop_count = loop_count;
		td.mu_in_use = &td.mu;
		td.lock = &void_mu_lock;
		td.unlock = &void_mu_unlock;
		for (i = 0; i != td.n_threads; i++) {
			closure_fork (closure_counting (&counting_loop_try_mu, &td, i));
		}
		test_data_wait_for_all_threads (&td);
		if (td.i != td.n_threads*td.loop_count) {
			TEST_FATAL (t, ("test_try_mu_nthread final count inconsistent: want %d, got %d",
				   td.n_threads*td.loop_count, td.i));
		}
		loop_count *= 2;
	} while (nsync_time_cmp (nsync_time_now (), deadline) < 0);
}

/* --------------------------------------- */

/* An integer protected by a mutex, and with an associated
   condition variable that is signalled when the counter reaches 0. */
typedef struct counter_s {
	nsync_mu mu; /* protects value */
	int value;
	nsync_cv cv; /* signalled when value becomes 0 */
} counter;

/* Return a counter with initial value "initial". */
static counter *counter_new (int initial) {
	counter *c = (counter *) malloc (sizeof (*c));
	bzero ((void *) c, sizeof (*c));
	c->value = initial;
	return (c);
}

/* Increment *c by "increment". */
static void counter_inc (counter *c, int increment) {
	if (increment != 0) {
		nsync_mu_lock (&c->mu);
		c->value += increment;
		if (c->value == 0) {
			nsync_cv_broadcast (&c->cv);
		}
		nsync_mu_unlock (&c->mu);
	}
}

/* Wait on *c's condition variable until the counter
   becomes 0, or abs_deadline is reached. */
static int counter_wait_for_zero_with_deadline (counter *c, nsync_time abs_deadline) {
	int value;
	nsync_mu_rlock (&c->mu);
	while (c->value != 0 &&
	       nsync_cv_wait_with_deadline (&c->cv, &c->mu, abs_deadline, NULL) == 0) {
	}
	value = c->value;
	nsync_mu_runlock (&c->mu);
	return (value);
}

/* Wait on *c's condition variable until the counter becomes 0. */
static void counter_wait_for_zero (counter *c) {
	int value = counter_wait_for_zero_with_deadline (c, nsync_time_no_deadline);
	if (value != 0) {
		testing_panic (smprintf ("wait_for_zero() about to return with "
					 "non-zero value %d", value));
	}
}

/* Return the current value of *c. */
static int counter_value (counter *c) {
	int value;
	nsync_mu_rlock (&c->mu);
	value = c->value;
	nsync_mu_runlock (&c->mu);
	return (value);
}

/* --------------------------------------- */

CLOSURE_DECL_BODY9 (attempt_trylock, testing , const char *, int, nsync_mu *,
		    int, int, int *, int, counter *)

/* Call nsync_mu_trylock(), and compares the result to expected_acquire.
   If the lock was acquired, then:
   - if expected_value != -1, compare *value against expected_value.
   - increment *value.
   - if release is non-zero, release the lock before returning.
   In any case, the counter *done is decremented. */
static void attempt_trylock (testing t, const char *id, int verbose,
			     nsync_mu *mu, int expected_acquire, int release,
			     int *value, int expected_value, counter *done) {
	int acquired = nsync_mu_trylock (mu);
	if (acquired != expected_acquire) {
		testing_panic (smprintf ("attempt_trylock %s:  expected "
					 "nsync_mu_trylock() to return %d but got %d",
					 id, expected_acquire, acquired));
	}
	if (verbose) {
		TEST_LOG (t, ("attempt_trylock %s %d\n", id, acquired));
	}
	if (acquired) {
		nsync_mu_assert_held (mu);
		if (expected_value != -1 && *value != expected_value) {
			testing_panic (smprintf ("attempt_trylock %s expected "
						 "value %d, *value=%d",
						 id, expected_value, *value));
		}
		(*value)++;
		if (verbose) {
			TEST_LOG (t, ("attempt_trylock %s incremented value to %d\n", id, *value));
		}
		if (release) {
			nsync_mu_unlock (mu);
		}
	}
	counter_inc (done, -1);
}

/* Call nsync_mu_rtrylock(), and compare the result to expected_acquire.
   If the lock was acquired, then:
   - if expected_value != -1, compare *value against expected_value.
   - if release is non-zero, release the lock before returning.
   In any case, decrement *done. */
static void attempt_rtrylock (testing t, const char *id, int verbose,
			      nsync_mu *mu, int expected_acquire, int release,
			      int *value, int expected_value, counter *done) {
	int acquired = nsync_mu_rtrylock (mu);
	if (acquired != expected_acquire) {
		testing_panic (smprintf ("attempt_rtrylock %s: expected "
					 "nsync_mu_rtrylock() to return %d but got %d",
					 id, expected_acquire, acquired));
	}
	if (verbose) {
		TEST_LOG (t, ("attempt_rtrylock %s %d\n", id, acquired));
	}
	if (acquired) {
		nsync_mu_rassert_held (mu);
		if (expected_value != -1 && *value != expected_value) {
			testing_panic (smprintf ("attempt_rtrylock %s expected "
						 "value %d, *value=%d",
						 id, expected_value, *value));
		}
		if (release) {
			nsync_mu_runlock (mu);
		}
	}
	counter_inc (done, -1);
}

CLOSURE_DECL_BODY9 (lock_unlock, testing, const char *, int, nsync_mu *,
		    int *, int, nsync_time, counter *, counter *)

/* First acquire *mu, then:
   - if expected_value != -1, compare *value against expected_value.
   - increment *value.
   - sleep for "sleep".
   Then release *mu and decrement *done. */
static void lock_unlock (testing t, const char *id, int verbose, nsync_mu *mu, int *value,
		  int expected_value, nsync_time sleep, counter *sleeping, counter *done) {
	if (verbose) {
		TEST_LOG (t, ("lock_unlock %s\n", id));
	}
	if (sleeping != NULL) {
		counter_inc (sleeping, -1);
	}
	nsync_mu_lock (mu);
	nsync_mu_assert_held (mu);
	if (expected_value != -1 && *value != expected_value) {
		testing_panic (smprintf ("lock_unlock %s expected "
					 "value %d, *value=%d",
					 id, expected_value, *value));
	}
	(*value)++;
	if (verbose) {
		TEST_LOG (t, ("lock_unlock %s incremented value to %d\n", id, *value));
	}
	nsync_time_sleep (sleep);
	nsync_mu_unlock (mu);
	counter_inc (done, -1);
}

/* First acquire *mu in read mode, then:
   - if expected_value != -1, compare *value against expected_value.
   - sleep for "sleep".
   Then release *mu and decrement *done. */
static void rlock_runlock (testing t, const char *id, int verbose, nsync_mu *mu,
			   int *value, int expected_value, nsync_time sleep,
			   counter *sleeping, counter *done) {
	if (verbose) {
		TEST_LOG (t, ("rlock_runlock %s\n", id));
	}
	if (sleeping != NULL) {
		counter_inc (sleeping, -1);
	}
	nsync_mu_rlock (mu);
	nsync_mu_rassert_held (mu);
	if (expected_value != -1 && *value != expected_value) {
		testing_panic (smprintf ("rlock_runlock %s expected "
					 "value %d, *value=%d", id, expected_value, *value));
	}
	nsync_time_sleep (sleep);
	nsync_mu_runlock (mu);
	counter_inc (done, -1);
}

/* Check that the time since start_time is between expected_duration-1ms.
   If the time exceeds expected_duration+slop_duration, return 1, else 0. */
static int check_times (testing t, const char *id, nsync_time start_time,
			nsync_time expected_duration, nsync_time slop_duration) {
	int exceeds_count = 0;
	nsync_time now;
	nsync_time measured_duration;
	now = nsync_time_now ();
	measured_duration = nsync_time_sub (now, start_time);
	if (nsync_time_cmp (measured_duration,
			    nsync_time_sub (expected_duration, nsync_time_ms (5))) < 0) {
		char *m_str = nsync_time_str (measured_duration, 2);
		char *e_str = nsync_time_str (expected_duration, 2);
		TEST_ERROR (t, ("check_times %s too short a delay: %s instead of %s",
			   id, m_str, e_str));
		free (m_str);
		free (e_str);
	}
	if (nsync_time_cmp (nsync_time_add (expected_duration, slop_duration), measured_duration) < 0) {
		exceeds_count++;
	}
	return (exceeds_count);
}

/* Check the operation of nsync_mu as a reader/writer lock. */
static void test_rlock (testing t) {
	int loop;
	int i;
	int max_write_wait_exceeded;
	int max_read_wait_exceeded;
	nsync_time time_unit;
	nsync_time slop_duration;
	nsync_time delay_duration;
	nsync_time writer_duration;
	nsync_time reader_duration;
	static const int loop_count = 5;
	static const int read_threads = 3;
	static const int limit = 3;
	static const int verbose = 0;
	max_write_wait_exceeded = 0;
	max_read_wait_exceeded = 0;

	time_unit = nsync_time_ms (100);
	slop_duration = nsync_time_add (nsync_time_add (time_unit, time_unit), time_unit);
	delay_duration = time_unit;
	writer_duration = time_unit;
	reader_duration = nsync_time_add (time_unit, time_unit);

	max_write_wait_exceeded = 0;
	max_read_wait_exceeded = 0;
	for (loop = 0; loop != loop_count; loop++) {
		counter *lock_unlock_sleeping;
		counter *rlock_runlock_sleeping;
		counter *lock_unlock_done;
		counter *rlock_runlock_done;
		nsync_time read_start_time;
		nsync_mu mu;
		int value = 0;
		counter *thread_done;

		nsync_time start_time;
		nsync_mu_init (&mu);
		start_time = nsync_time_now ();

		/* ------------------------------------ */
		/* Acquire lock with nsync_mu_rtrylock().  This thread will
		   hold a read lock until the next line with =====.  */
		thread_done = counter_new (1);
		attempt_rtrylock (t, "a", verbose, &mu, 1, 0, &value, 0, thread_done);
		counter_wait_for_zero (thread_done);

		nsync_mu_rassert_held (&mu);

		counter_inc (thread_done, 1);
		/* Can get read lock holding read lock. */
		closure_fork (closure_attempt_trylock (&attempt_rtrylock,
			t, "b", verbose, &mu, 1, 1, &value, 0, thread_done));
		counter_wait_for_zero (thread_done);

		nsync_mu_rassert_held (&mu);

		counter_inc (thread_done, 1);
		/* Can't get write lock holding read lock. */
		closure_fork (closure_attempt_trylock (&attempt_trylock, t, "c", verbose,
						       &mu, 0, 1, &value, -1, thread_done));
		counter_wait_for_zero (thread_done);

		if (!nsync_mu_is_reader (&mu)) {
			TEST_FATAL(t, ("expected mu held in reader mode"));
		}

		counter_inc (thread_done, 1);
		closure_fork (closure_lock_unlock (&rlock_runlock, t, "d", verbose,
						   &mu, &value, 0, nsync_time_zero /*no delay*/,
						   NULL, thread_done));
		counter_wait_for_zero (thread_done);

		nsync_mu_rassert_held (&mu);

		lock_unlock_done = counter_new (1);
		lock_unlock_sleeping = counter_new (1);
		closure_fork (closure_lock_unlock (&lock_unlock, t, "e", verbose,
						   &mu, &value, 0, writer_duration,
						   lock_unlock_sleeping, lock_unlock_done));

		counter_wait_for_zero (lock_unlock_sleeping);
		nsync_time_sleep (delay_duration); /* give time for lock_unlock() thread to wait. */

		nsync_mu_rassert_held (&mu);

		rlock_runlock_done = counter_new (read_threads);
		rlock_runlock_sleeping = counter_new (read_threads);
		for (i = 0; i != read_threads; i++) {
			/* read lock will be acquired after lock_unlock() completes */
			closure_fork (closure_lock_unlock (&rlock_runlock, t, "f", verbose,
							   &mu, &value, 1, reader_duration,
							   rlock_runlock_sleeping,
							   rlock_runlock_done));
		}

		nsync_mu_rassert_held (&mu);

		counter_wait_for_zero (rlock_runlock_sleeping);
		nsync_time_sleep (delay_duration); /* time for rlock_runlock() threads to wait. */

		nsync_mu_rassert_held (&mu);

		if (counter_value (lock_unlock_done) == 0) {
			TEST_FATAL (t, ("thread was able to acquire write lock while read lock held"));
		}
		if (counter_value (rlock_runlock_done) == 0) {
			TEST_FATAL (t, ("thread was able to acquire read lock with "
				   "other reader and waiting writer"));
		}

		nsync_mu_rassert_held (&mu);

		counter_inc (thread_done, 1);
	       /* Still can't get write lock. */
		closure_fork (closure_attempt_trylock (&attempt_trylock, t, "g", verbose,
						       &mu, 0, 1, &value, -1, thread_done));
		counter_wait_for_zero (thread_done);

		counter_inc (thread_done, 1);
		/* Now can't get read lock because a writer is waiting. */
		closure_fork (closure_attempt_trylock (&attempt_rtrylock, t, "h", verbose,
						       &mu, 0, 1, &value, -1, thread_done));
		counter_wait_for_zero (thread_done);

		nsync_mu_runlock (&mu);
		/* ==================================== */

		read_start_time = nsync_time_now ();
		counter_wait_for_zero (lock_unlock_done); /* Now can get write lock. */
		max_write_wait_exceeded += check_times (t, "i", start_time,
			nsync_time_add (nsync_time_add (delay_duration, delay_duration), writer_duration),
			slop_duration);

		counter_wait_for_zero (rlock_runlock_done); /* And now an get read lock again. */
		max_read_wait_exceeded += check_times (t, "j", read_start_time,
						       reader_duration, slop_duration);

		free (thread_done);
		free (lock_unlock_done);
		free (rlock_runlock_done);
		free (lock_unlock_sleeping);
		free (rlock_runlock_sleeping);
	}
	if (verbose) {
		TEST_LOG (t, ("read lock max_write_wait_exceeded %d max_read_wait_exceeded %d\n",
			 max_write_wait_exceeded, max_read_wait_exceeded));
	}
	if (max_write_wait_exceeded > limit) {
		TEST_ERROR (t, ("lock_unlock() took too long %d "
			   "(more than %d) times out of %d",
			   max_write_wait_exceeded, limit, loop_count));
	}
	if (max_read_wait_exceeded > limit) {
		TEST_ERROR (t, ("rlock_runlock() took too long %d "
			   "(more than %d) times out of %d",
			   max_read_wait_exceeded, limit, loop_count));
	}

	max_write_wait_exceeded = 0;
	max_read_wait_exceeded = 0;
	for (loop = 0; loop != loop_count; loop++) {
		counter *lock_unlock_sleeping;
		counter *rlock_runlock_sleeping;
		counter *lock_unlock_done;
		counter *rlock_runlock_done;
		nsync_time read_start_time;
		nsync_mu mu;
		int value = 0;
		counter *thread_done;

		nsync_time start_time;

		nsync_mu_init (&mu);
		start_time = nsync_time_now ();

		/* ------------------------------------ */
		/* Acquire lock with nsync_mu_trylock().  This thread will hold
		   a write lock until the next line with =====.  */
		thread_done = counter_new (1);
		attempt_trylock (t, "A", verbose, &mu, 1, 0, &value, 0, thread_done);
		counter_wait_for_zero (thread_done);

		nsync_mu_assert_held (&mu);
		nsync_mu_rassert_held (&mu);

		counter_inc (thread_done, 1);
		/* Can't get read lock while holding write lock. */
		closure_fork (closure_attempt_trylock (&attempt_rtrylock, t, "B", verbose,
						       &mu, 0, 1, &value, -1, thread_done));
		counter_wait_for_zero (thread_done);

		if (nsync_mu_is_reader (&mu)) {
			TEST_FATAL (t, ("expected mu held in write mode"));
		}
		nsync_mu_assert_held (&mu);
		nsync_mu_rassert_held (&mu);

		counter_inc (thread_done, 1);
		/* Can't get write lock while holding write lock. */
		closure_fork (closure_attempt_trylock (&attempt_trylock, t, "C", verbose,
						       &mu, 0, 1, &value, -1, thread_done));
		counter_wait_for_zero (thread_done);

		nsync_mu_assert_held (&mu);
		nsync_mu_rassert_held (&mu);

		lock_unlock_done = counter_new (1);
		lock_unlock_sleeping = counter_new (1);
		closure_fork (closure_lock_unlock (&lock_unlock, t, "D", verbose,
						   &mu, &value, 1, writer_duration,
						   lock_unlock_sleeping, lock_unlock_done));

		counter_wait_for_zero (lock_unlock_sleeping);
		nsync_time_sleep (delay_duration); /* give time for lock_unlock() thread to wait. */

		nsync_mu_assert_held (&mu);
		nsync_mu_rassert_held (&mu);

		rlock_runlock_done = counter_new (read_threads);
		rlock_runlock_sleeping = counter_new (read_threads);
		for (i = 0; i != read_threads; i++) {
			/* not guaranteed will complete after lock_unlock() above */
			closure_fork (closure_lock_unlock (&rlock_runlock, t, "E", verbose,
							   &mu, &value, -1, reader_duration,
							   rlock_runlock_sleeping,
							   rlock_runlock_done));
		}

		nsync_mu_assert_held (&mu);
		nsync_mu_rassert_held (&mu);

		counter_wait_for_zero (rlock_runlock_sleeping);
		nsync_time_sleep (delay_duration); /* time for rlock_runlock() threads to wait. */

		nsync_mu_assert_held (&mu);
		nsync_mu_rassert_held (&mu);

		if (counter_value (lock_unlock_done) == 0) {
			TEST_FATAL (t, ("thread was able to acquire write lock "
				   "while other write lock held"));
		}
		if (counter_value (rlock_runlock_done) == 0) {
			TEST_FATAL (t, ("thread was able to acquire read lock "
				   "while  write lock held"));
		}

		nsync_mu_assert_held (&mu);
		nsync_mu_rassert_held (&mu);

		counter_inc (thread_done, 1);
		/* Still can't get read lock while holding write lock. */
		closure_fork (closure_attempt_trylock (&attempt_rtrylock, t, "F", verbose,
						       &mu, 0, 1, &value, -1, thread_done));
		counter_wait_for_zero (thread_done);

		nsync_mu_assert_held (&mu);
		nsync_mu_rassert_held (&mu);

		counter_inc (thread_done, 1);
		/* Still can't get write lock while holding write lock. */
		closure_fork (closure_attempt_trylock (&attempt_trylock, t, "G", verbose,
						       &mu, 0, 1, &value, -1, thread_done));
		counter_wait_for_zero (thread_done);

		nsync_mu_assert_held (&mu);
		nsync_mu_rassert_held (&mu);

		nsync_mu_unlock (&mu);
		/* ==================================== */

		read_start_time = nsync_time_now ();
		counter_wait_for_zero (lock_unlock_done); /* Now can get write lock. */
		max_write_wait_exceeded += check_times (t, "H", start_time,
			nsync_time_add (nsync_time_add (delay_duration, delay_duration), writer_duration),
			slop_duration);

		counter_wait_for_zero (rlock_runlock_done); /* And now can get read lock again. */
		max_read_wait_exceeded += check_times (t, "I", read_start_time,
						       reader_duration, slop_duration);

		free (thread_done);
		free (lock_unlock_done);
		free (rlock_runlock_done);
		free (lock_unlock_sleeping);
		free (rlock_runlock_sleeping);
	}
	if (verbose) {
		TEST_LOG (t, ("write lock max_write_wait_exceeded %d "
			 "max_read_wait_exceeded %d\n",
			 max_write_wait_exceeded, max_read_wait_exceeded));
	}
	if (max_write_wait_exceeded > limit) {
		TEST_ERROR (t, ("lock_unlock() took too long %d (more than %d) "
			   "times out of %d",
			   max_write_wait_exceeded, limit, loop_count));
	}
	if (max_read_wait_exceeded > limit) {
		TEST_ERROR (t, ("rlock_runlock() took too long %d (more than %d) "
			   "times out of %d",
			   max_read_wait_exceeded, limit, loop_count));
	}
}

/* --------------------------------------- */

/* Measure the performance of an uncontended nsync_mu. */
static void benchmark_mu_uncontended (testing t) {
	int i;
	int n = testing_n (t);
	nsync_mu mu;
	nsync_mu_init (&mu);
	for (i = 0; i != n; i++) {
		nsync_mu_lock (&mu);
		nsync_mu_unlock (&mu);
	}
}

/* Return whether int *value is one. */
static int int_is_1 (const void *value) { return (*(const int *)value == 1); }

/* Return whether int *value is two. */
static int int_is_2 (const void *value) { return (*(const int *)value == 2); }

/* Return whether int *value is three. */
static int int_is_3 (const void *value) { return (*(const int *)value == 3); }

/* Set *value to 1, wait for it to become 2, then set it to 3.  *value is under
   *mu */
static void waiter (nsync_mu *mu, int *value) {
	nsync_mu_lock (mu);
	*value = 1;
	nsync_mu_wait (mu, &int_is_2, value, NULL);
	*value = 3;
	nsync_mu_unlock (mu);
}

CLOSURE_DECL_BODY2 (waiter, nsync_mu *, int *)

/* Measure the performance of an uncontended nsync_mu
   with a blocked waiter. */
static void benchmark_mu_uncontended_waiter (testing t) {
	int i;
	int n = testing_n (t);
	nsync_mu mu;
	int value = 0;
	nsync_mu_init (&mu);
	closure_fork (closure_waiter (&waiter, &mu, &value));
	nsync_mu_lock (&mu);
	nsync_mu_wait (&mu, &int_is_1, &value, NULL);
	nsync_mu_unlock (&mu);
	for (i = 0; i != n; i++) {
		nsync_mu_lock (&mu);
		nsync_mu_unlock (&mu);
	}
	nsync_mu_lock (&mu);
	value = 2;
	nsync_mu_wait (&mu, &int_is_3, &value, NULL);
	nsync_mu_unlock (&mu);
}

/* Measure the performance of an uncontended nsync_mu
   with a blocked waiter using nsync_mu_unlock_without_wakeup. */
static void benchmark_mu_uncontended_no_wakeup (testing t) {
	int i;
	int n = testing_n (t);
	nsync_mu mu;
	int value = 0;
	nsync_mu_init (&mu);
	closure_fork (closure_waiter (&waiter, &mu, &value));
	nsync_mu_lock (&mu);
	nsync_mu_wait (&mu, &int_is_1, &value, NULL);
	nsync_mu_unlock (&mu);
	for (i = 0; i != n; i++) {
		nsync_mu_lock (&mu);
		nsync_mu_unlock_without_wakeup (&mu);
	}
	nsync_mu_lock (&mu);
	value = 2;
	nsync_mu_wait (&mu, &int_is_3, &value, NULL);
	nsync_mu_unlock (&mu);
}

/* Measure the performance of an uncontended
   nsync_mu in read mode. */
static void benchmark_rmu_uncontended (testing t) {
	int i;
	int n = testing_n (t);
	nsync_mu mu;
	nsync_mu_init (&mu);
	for (i = 0; i != n; i++) {
		nsync_mu_rlock (&mu);
		nsync_mu_runlock (&mu);
	}
}

/* Measure the performance of an uncontended nsync_mu
   in read mode with a blocked waiter. */
static void benchmark_rmu_uncontended_waiter (testing t) {
	int i;
	int n = testing_n (t);
	nsync_mu mu;
	int value = 0;
	nsync_mu_init (&mu);
	closure_fork (closure_waiter (&waiter, &mu, &value));
	nsync_mu_lock (&mu);
	nsync_mu_wait (&mu, &int_is_1, &value, NULL);
	nsync_mu_unlock (&mu);
	for (i = 0; i != n; i++) {
		nsync_mu_rlock (&mu);
		nsync_mu_runlock (&mu);
	}
	nsync_mu_lock (&mu);
	value = 2;
	nsync_mu_wait (&mu, &int_is_3, &value, NULL);
	nsync_mu_unlock (&mu);
}

/* Measure the performance of an uncontended pthread_mutex_t. */
static void benchmark_mutex_uncontended (testing t) {
	int i;
	int n = testing_n (t);
	pthread_mutex_t mu;
	pthread_mutex_init (&mu, NULL);
	for (i = 0; i != n; i++) {
		pthread_mutex_lock (&mu);
		pthread_mutex_unlock (&mu);
	}
	pthread_mutex_destroy (&mu);
}

/* Measure the performance of an uncontended pthread_rwlock_t. */
static void benchmark_wmutex_uncontended (testing t) {
	int i;
	int n = testing_n (t);
	pthread_rwlock_t mu;
	pthread_rwlock_init (&mu, NULL);
	for (i = 0; i != n; i++) {
		pthread_rwlock_wrlock (&mu);
		pthread_rwlock_unlock (&mu);
	}
	pthread_rwlock_destroy (&mu);
}

/* Measure the performance of an uncontended
   pthread_rwlock_t in read mode. */
static void benchmark_rmutex_uncontended (testing t) {
	int i;
	int n = testing_n (t);
	pthread_rwlock_t mu;
	pthread_rwlock_init (&mu, NULL);
	for (i = 0; i != n; i++) {
		pthread_rwlock_rdlock (&mu);
		pthread_rwlock_unlock (&mu);
	}
	pthread_rwlock_destroy (&mu);
}

/* ---------------------------------------
   Benchmarks for contended locks. */

/* It's hard to write these as benchmark functions, since we wish to measure
   throughput over an extended period (a second or two), rather than get the
   latency of a few iterations. */

/* A contended_state represents state shared between threads
   in the contended benchmarks. */
typedef struct contended_state_s {
	testing t;

	/* locks to test */
	nsync_mu mu;
	pthread_mutex_t mutex;
	pthread_rwlock_t rwmutex;
	int count; /* counter protected by a lock above */
	
	nsync_mu start_done_mu;
	int start; /* whether threads should start, under start_done_mu */
	int not_yet_done;  /* threads not yet complete, under start_done_mu */
} contended_state;

static int contended_state_may_start (const void *v) {
	return (((const contended_state *)v)->start);
}

static int contended_state_all_done (const void *v) {
	return (((const contended_state *)v)->not_yet_done == 0);
}

/* Wait for cs.start to become non-zero, then loop, acquiring and
   releasing mu on each iteration until cs.deadline is reached, then decrement
   cs.not_yet_done. */
static void contended_state_contend_loop (contended_state *cs,
					  void *mu, void (*lock) (void *),
					  void (*unlock) (void *)) {
	int n = testing_n (cs->t);
	int j;
	int i;
	nsync_mu_rlock (&cs->start_done_mu);
	nsync_mu_wait (&cs->start_done_mu, &contended_state_may_start, cs, NULL);
	nsync_mu_runlock (&cs->start_done_mu);

	for (j = 0; j < n; j += 10000) {
		for (i = 0; i != 10000; i++) {
			(*lock) (mu);
			cs->count++;
			(*unlock) (mu);
		}
	}

	nsync_mu_lock (&cs->start_done_mu);
	cs->not_yet_done--;
	nsync_mu_unlock (&cs->start_done_mu);
}

typedef void (*func_any) (void *);
CLOSURE_DECL_BODY4 (contended_state_contend_loop, contended_state *, void *, func_any, func_any)

/* Start the threads in a contended test, wait for them to finish,
   and print the number of iterations achieved. */
static void contended_state_run_test (contended_state *cs, testing t,
				      void *mu, void (*lock) (void *),
				      void (*unlock) (void *)) {
	int i;
	cs->t = t;
	cs->not_yet_done = 4; /* number of threads */
	cs->start = 0;
	cs->count = 0;
	for (i = 0; i != cs->not_yet_done; i++) {
		closure_fork (closure_contended_state_contend_loop (
			&contended_state_contend_loop, cs, mu, lock, unlock));
	}
	nsync_mu_lock (&cs->start_done_mu);
	cs->start = 1;
	nsync_mu_wait (&cs->start_done_mu, &contended_state_all_done, cs, NULL);
	nsync_mu_unlock (&cs->start_done_mu);
}

/* Measure the performance of highly contended
   nsync_mu locks, with small critical sections.  */
static void benchmark_mu_contended (testing t) {
	contended_state cs;
	bzero ((void *) &cs, sizeof (cs));
	contended_state_run_test (&cs, t, &cs.mu, (void (*) (void*))&nsync_mu_lock,
				  (void (*) (void*))&nsync_mu_unlock);
}

/* Measure the performance of highly contended
   pthread_mutex_t locks, with small critical sections.  */
static void benchmark_mutex_contended (testing t) {
	contended_state cs;
	bzero ((void *) &cs, sizeof (cs));
	pthread_mutex_init (&cs.mutex, NULL);
	contended_state_run_test (&cs, t, &cs.mutex, &void_pthread_mutex_lock,
				  &void_pthread_mutex_unlock);
	pthread_mutex_destroy (&cs.mutex);
}

/* Measure the performance of highly contended
   pthread_rwlock_t locks, with small critical sections.  */
static void benchmark_wmutex_contended (testing t) {
	contended_state cs;
	bzero ((void *) &cs, sizeof (cs));
	pthread_rwlock_init (&cs.rwmutex, NULL);
	contended_state_run_test (&cs, t, &cs.rwmutex, &void_pthread_rwlock_wrlock,
				  &void_pthread_rwlock_unlock);
	pthread_rwlock_destroy (&cs.rwmutex);
}

int main (int argc, char *argv[]) {
	testing_base tb = testing_new (argc, argv, 0);

	TEST_RUN (tb, test_rlock);
	TEST_RUN (tb, test_mu_nthread);
	TEST_RUN (tb, test_mutex_nthread);
	TEST_RUN (tb, test_rwmutex_nthread);
	TEST_RUN (tb, test_try_mu_nthread);

	BENCHMARK_RUN (tb, benchmark_mu_contended);
	BENCHMARK_RUN (tb, benchmark_mutex_contended);
	BENCHMARK_RUN (tb, benchmark_wmutex_contended);

	BENCHMARK_RUN (tb, benchmark_mu_uncontended);
	BENCHMARK_RUN (tb, benchmark_rmu_uncontended);
	BENCHMARK_RUN (tb, benchmark_mutex_uncontended);
	BENCHMARK_RUN (tb, benchmark_wmutex_uncontended);
	BENCHMARK_RUN (tb, benchmark_rmutex_uncontended);
	BENCHMARK_RUN (tb, benchmark_mu_uncontended_waiter);
	BENCHMARK_RUN (tb, benchmark_mu_uncontended_no_wakeup);
	BENCHMARK_RUN (tb, benchmark_rmu_uncontended_waiter);

	return (testing_base_exit (tb));
}
