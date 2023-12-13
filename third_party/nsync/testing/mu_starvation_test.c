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
#include "libc/calls/calls.h"
#include "libc/str/str.h"
#include "libc/thread/thread.h"
#include "third_party/nsync/mu.h"
#include "third_party/nsync/mu_wait.h"
#include "third_party/nsync/testing/closure.h"
#include "third_party/nsync/testing/smprintf.h"
#include "third_party/nsync/testing/testing.h"
#include "third_party/nsync/testing/time_extra.h"
#include "third_party/nsync/time.h"

/* Test the behaviour of mu in situations where starvation might be expected. */

/* starve_data is the data used by the starvation tests */
typedef struct starve_data_s {
	nsync_mu mu; /* precedes control_mu in locking order */
	int cancel;     /* whether threads should shutdown; under mu */
	nsync_time start;  /* when test started */

	nsync_mu control_mu;
	int not_yet_started; /* threads not yet started; under control_mu */
	int not_yet_done; /* threads not yet done; under control_mu */
} starve_data;

/* initialize *sd */
static void starve_data_init (starve_data *sd, int threads) {
	bzero ((void *) sd, sizeof (*sd));
	sd->not_yet_started = threads;
	sd->not_yet_done = threads;
	sd->start = nsync_time_now ();
}

/* Loop until *cancel or deadline, and on each iteration
   acquire *mu in reader mode, and hold it until the next odd or even
   multiple of period, according to parity.  Just before return, decrement *done
   under *mu.  Two threads using these calls are used to hold the
   mutex continually, in the absence of other activity. */
static void starve_with_readers (starve_data *sd, nsync_time period,
				 uint32_t parity, nsync_time deadline) {
	nsync_time now;
	uint32_t period_us = (uint32_t) (nsync_time_to_dbl (period) * 1e6);
	nsync_mu_rlock (&sd->mu);

	nsync_mu_lock (&sd->control_mu);
	sd->not_yet_started--;
	nsync_mu_unlock (&sd->control_mu);

	for (now = nsync_time_now ();
	     !sd->cancel && nsync_time_cmp (now, deadline) < 0;
	     now = nsync_time_now ()) {
		uint32_t new_us;
		uint32_t now_us = (uint32_t) (nsync_time_to_dbl (nsync_time_sub (now, sd->start)) * 1e6);
		uint32_t index = (now_us + period_us - 1) / period_us;
		if ((index & 1) != parity) {
			index++;
		}
		new_us = index * period_us;
		nsync_time_sleep (nsync_time_from_dbl (1e-6 * (double) (new_us-now_us)));
		nsync_mu_runlock (&sd->mu);
		nsync_mu_rlock (&sd->mu);
	}
	nsync_mu_runlock (&sd->mu);

	nsync_mu_lock (&sd->control_mu);
	sd->not_yet_done--;
	nsync_mu_unlock (&sd->control_mu);
}

CLOSURE_DECL_BODY4 (starve_with_readers, starve_data *, nsync_time, uint32_t, nsync_time)

static int started (const void *v) {
	return (((const starve_data *) v)->not_yet_started == 0);
}

static int done (const void *v) {
	return (((const starve_data *) v)->not_yet_done == 0);
}

/* Verify the behaviour of nsync_mu in the face of reader threads that conspire
   keep the lock held continuously in reader mode, even though each of the
   threads releases and reacquires periodically (while another thread holds the
   lock).  The routine starve_with_readers() is used to achieve this effect.

   We expect that nsync_mu_trylock() will not be able to acquire while this is
   happening, but that nsync_mu_lock() will be able to acquire, due to the action of the
   mu's mu_writer_waiting bit. */
static void test_starve_with_readers (testing t) {
	nsync_time finish;
	int trylock_acquires;
	int expected_lo;
	int lock_acquires;

	nsync_time deadline;

	starve_data sd;
	starve_data_init (&sd, 2); /* two threads, started below */

	/* Threads run for at most 10s. */
	deadline = nsync_time_add (nsync_time_now (), nsync_time_ms (10000));

	/* These two threads will try to hold a reader lock
	   continuously until cancel is set or deadline is reached,
	   even though each will release the lock every 20ms. */
	closure_fork (closure_starve_with_readers (
		&starve_with_readers, &sd, nsync_time_ms (10), 0, deadline));
	closure_fork (closure_starve_with_readers (
		&starve_with_readers, &sd, nsync_time_ms (10), 1, deadline));

	/* wait for the threads to acquire their first lock. */
	nsync_mu_lock (&sd.control_mu);
	nsync_mu_wait (&sd.control_mu, &started, &sd, NULL);
	nsync_mu_unlock (&sd.control_mu);

	/* If using an nsync_mu, use nsync_mu_trylock() to attempt to acquire while the
	   readers are hogging the lock.  We expect no acquisitions to succeed. */
	finish = nsync_time_add (nsync_time_now (), nsync_time_ms (500));
	trylock_acquires = 0; /* number of acquires */
	while (nsync_time_cmp (nsync_time_now (), finish) < 0) {
		if (nsync_mu_trylock (&sd.mu)) {
			trylock_acquires++;
			nsync_mu_unlock (&sd.mu);
		}
		pthread_yield ();
	}
	if (trylock_acquires != 0) {
		TEST_ERROR (t, ("expected no acquisitions via nsync_mu_trylock(), got %d\n",
			   trylock_acquires));
	}

	/* Use nsync_mu_lock() to attempt to acquire while the readers are hogging
	   the lock.  We expect several acquisitions to succeed. */
	expected_lo = 2;
	finish = nsync_time_add (nsync_time_now (), nsync_time_ms (5000));
	lock_acquires = 0; /* number of acquires */
	while (nsync_time_cmp (nsync_time_now (), finish) < 0 && lock_acquires < expected_lo) {
		nsync_mu_lock (&sd.mu);
		lock_acquires++;
		nsync_mu_unlock (&sd.mu);
		nsync_time_sleep (nsync_time_ms (1));
	}
	if (nsync_time_cmp (nsync_time_now (), deadline) > 0 && lock_acquires == 1) {
		lock_acquires = 0; /* hog threads timed out */
	}
	if (lock_acquires < expected_lo) {
		TEST_ERROR (t, ("expected at least %d acquisitions via nsync_mu_lock(), got %d\n",
			   expected_lo, lock_acquires));
	}

	nsync_mu_lock (&sd.mu);
	sd.cancel = 1; /* Tell threads to exit. */
	nsync_mu_unlock (&sd.mu);

	nsync_mu_lock (&sd.control_mu);
	nsync_mu_wait (&sd.control_mu, &done, &sd, NULL); /* wait for exit. */
	nsync_mu_unlock (&sd.control_mu);
}

/* Loop until sd.cancel or deadline.  On each iteration<
   acquire sd.mu in writer mode, sleep for hold_time, and release sd.mu.
   Just before return, decrement sd.not_yet_done under sd.control_mu. */
static void starve_with_writer (starve_data *sd, nsync_time hold_time,
			        nsync_time deadline) {
	nsync_time now;

	nsync_mu_lock (&sd->mu);

	nsync_mu_lock (&sd->control_mu);
	sd->not_yet_started--;
	nsync_mu_unlock (&sd->control_mu);

	for (now = nsync_time_now ();
	     !sd->cancel && nsync_time_cmp (now, deadline) < 0;
	     now = nsync_time_now ()) {
		nsync_time_sleep (hold_time);
		nsync_mu_unlock (&sd->mu);
		nsync_mu_lock (&sd->mu);
	}
	nsync_mu_unlock (&sd->mu);

	nsync_mu_lock (&sd->control_mu);
	sd->not_yet_done--;
	nsync_mu_unlock (&sd->control_mu);
}

CLOSURE_DECL_BODY3 (starve_with_writer, starve_data *, nsync_time, nsync_time)

/* Verify the behaviour of nsync_mu in the face of a
   single writer thread that repeatedly hogs the lock by acquiring it and
   holding it for longer than the runtime's wakeup time, then releasing.  The
   next iteration reacquires the lock moments later, a time much shorter than
   the runtime's wakeup time.  The routine starve_with_writer() is used to
   achieve this effect.

   These circumstances can make it hard for another thread T to acquire.  T
   will first wait on the mutex's queue.  Eventually, it will be woken by the
   hog thread, but under normal circumstances T will take so long to run that
   the hog will have reacquired the mutex.  Because the hog keeps the lock for
   longer than the runtime's wakeup time, T will go back to sleep again, and
   the process repeats indefinitely.

   We expect that incessant attempts via nsync_mu_trylock() and nsync_mu_rtrylock() will
   occasionally manage to hit the moments when the lock is not held.  nsync_mu_lock()
   and nsync_mu_rlock() will succeed only because of the action of mu's mu_long_wait bit,
   which will eventually force the hog to wait itself, and allow a waiter
   to acquire.  We expect few acquires because mu_long_wait kicks in only
   when things look dire. */
static void test_starve_with_writer (testing t) {
	int expected_lo;
	nsync_time finish;
	int lock_acquires;
	int rlock_acquires;
	int trylock_acquires;
	int rtrylock_acquires;
	nsync_time deadline;
	starve_data sd;
	starve_data_init (&sd, 1); /* one thread, started below */
	deadline = nsync_time_add (nsync_time_now (), nsync_time_ms (25000)); /* runs for at most 25s. */

	/* This thread will try to hold a writer lock almost
	   continuously, releasing momentarily every 10ms. */
	closure_fork (closure_starve_with_writer (&starve_with_writer, &sd,
						  nsync_time_ms (10), deadline));

	nsync_mu_lock (&sd.control_mu);
	nsync_mu_wait (&sd.control_mu, &started, &sd, NULL);
	nsync_mu_unlock (&sd.control_mu);

	expected_lo = 0;   /* minimum expected operations at each test */
	finish = nsync_time_zero; /* finish time for each test */

	if (!testing_is_uniprocessor (t)) {  /* this test won't work on a uniprocessor */
		/* Use nsync_mu_trylock() to attempt to acquire while the writer is hogging the
		   lock.  We expect some acquisitions to succeed. */
		expected_lo = 1;
		finish = nsync_time_add (nsync_time_now (), nsync_time_ms (30000));
		trylock_acquires = 0; /* number of acquires */
		while (nsync_time_cmp (nsync_time_now (), finish) < 0 && trylock_acquires < expected_lo) {
			if (nsync_mu_trylock (&sd.mu)) {
				trylock_acquires++;
				nsync_mu_unlock (&sd.mu);
			}
			pthread_yield ();
		}
		if (trylock_acquires < expected_lo) {
			TEST_ERROR (t, ("expected at least %d acquisitions via "
				   "nsync_mu_trylock(), got %d\n",
				   expected_lo, trylock_acquires));
		}
	}

	if (!testing_is_uniprocessor (t)) {  /* this test won't work on a uniprocessor */
		/* Use nsync_mu_rtrylock() to attempt to read-acquire while the writer is
		   hogging the lock.  We expect some acquisitions to succeed. */
		expected_lo = 1;
		finish = nsync_time_add (nsync_time_now (), nsync_time_ms (30000));
		rtrylock_acquires = 0; /* number of acquires */
		while (nsync_time_cmp (nsync_time_now (), finish) < 0 && rtrylock_acquires < expected_lo) {
			if (nsync_mu_rtrylock (&sd.mu)) {
				rtrylock_acquires++;
				nsync_mu_runlock (&sd.mu);
			}
			pthread_yield ();
		}
		if (rtrylock_acquires < expected_lo) {
			TEST_ERROR (t, ("expected at least %d acquisitions via "
				   "nsync_mu_rtrylock(), got %d\n",
				   expected_lo, rtrylock_acquires));
		}
	}

	/* Use nsync_mu_lock() to attempt to acquire while the writer is hogging
	   the lock.  We expect several acquisitions to succeed. */
	expected_lo = 2;
	finish = nsync_time_add (nsync_time_now (), nsync_time_ms (5000));
	lock_acquires = 0; /* number of acquires */
	while (nsync_time_cmp (nsync_time_now (), finish) < 0 && lock_acquires < expected_lo) {
		nsync_mu_lock (&sd.mu);
		lock_acquires++;
		nsync_mu_unlock (&sd.mu);
		nsync_time_sleep (nsync_time_ms (2));
	}
	if (lock_acquires == 1 && nsync_time_cmp (nsync_time_now (), deadline) > 0) {
		lock_acquires = 0; /* hog thread timed out */
	}
	if (lock_acquires < expected_lo) {
		TEST_ERROR (t, ("expected at least %d acquisitions via nsync_mu_lock(), got %d\n",
			   expected_lo, lock_acquires));
	}

	/* If enough time remains to run the test, use nsync_mu_rlock() to attempt to
	   acquire while the writer is hogging the lock.  We expect several
	   acquisitions to succeed.  It's ok not to run the test if we ran out
	   time----it means that a writer couldn't break in (the test case
	   above failed), so a reader is unlikely to manage it either. */
	expected_lo = 2;
	finish = nsync_time_add (nsync_time_now (), nsync_time_ms (5000));
	rlock_acquires = 0; /* number of acquires */
	if (nsync_time_cmp (finish, deadline) < 0) {
		while (nsync_time_cmp (nsync_time_now (), finish) < 0 && rlock_acquires < expected_lo) {
			nsync_mu_rlock (&sd.mu);
			rlock_acquires++;
			nsync_mu_runlock (&sd.mu);
			nsync_time_sleep (nsync_time_ms (2));
		}
		if (rlock_acquires == 1 && nsync_time_cmp (nsync_time_now (), deadline) > 0) {
			rlock_acquires = 0; /* hog thread timed out */
		}
		if (rlock_acquires < expected_lo) {
			TEST_ERROR (t, ("expected at least %d acquisitions via "
				   "nsync_mu_rlock(), got %d\n",
				   expected_lo, rlock_acquires));
		}
	}

	nsync_mu_lock (&sd.mu);
	sd.cancel = 1; /* Tell threads to exit. */
	nsync_mu_unlock (&sd.mu);

	nsync_mu_lock (&sd.control_mu);
	nsync_mu_wait (&sd.control_mu, &done, &sd, NULL); /* wait for exit. */
	nsync_mu_unlock (&sd.control_mu);
}

int main (int argc, char *argv[]) {
	testing_base tb = testing_new (argc, argv, 0);
	TEST_RUN (tb, test_starve_with_readers);
	TEST_RUN (tb, test_starve_with_writer);
	return (testing_base_exit (tb));
}
