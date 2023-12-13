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
#include "libc/calls/struct/timespec.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/clock.h"
#include "libc/thread/thread.h"
#include "libc/thread/thread2.h"
#include "third_party/nsync/cv.h"
#include "third_party/nsync/mu.h"
#include "third_party/nsync/mu_wait.h"
#include "third_party/nsync/testing/closure.h"
#include "third_party/nsync/testing/smprintf.h"
#include "third_party/nsync/testing/testing.h"
#include "third_party/nsync/waiter.h"

/* The benchmarks in this file use various mechanisms to
   ping-pong back and forth between two threads as they count i from
   0 to limit.
   The data structure contains multiple synchronization primitives,
   but each benchmark uses only those it needs.

   The setting of GOMAXPROCS, and the exact choices of the thread scheduler can
   have great effect on the timings. */
typedef struct ping_pong_s {
	nsync_mu mu;
	nsync_cv cv[2];
	
	pthread_mutex_t mutex;
	pthread_rwlock_t rwmutex;
	pthread_cond_t cond[2];

	pthread_cond_t done_cond;
	int outstanding;

	int i;
	int limit;
} ping_pong;

static void ping_pong_init (ping_pong *pp, int limit) {
	bzero ((void *) pp, sizeof (*pp));
	pthread_mutex_init (&pp->mutex, NULL);
	pthread_rwlock_init (&pp->rwmutex, NULL);
	pthread_cond_init (&pp->cond[0], NULL);
	pthread_cond_init (&pp->cond[1], NULL);
	pthread_cond_init (&pp->done_cond, NULL);
	pp->outstanding = 2;
	pp->limit = limit;
}

static void ping_pong_done (ping_pong *pp) {
	pthread_mutex_lock (&pp->mutex);
	pp->outstanding--;
	if (pp->outstanding == 0) {
		pthread_cond_broadcast (&pp->done_cond);
	}
	pthread_mutex_unlock (&pp->mutex);
}

static void ping_pong_destroy (ping_pong *pp) {
	pthread_mutex_lock (&pp->mutex);
	while (pp->outstanding != 0) {
		pthread_cond_wait (&pp->done_cond, &pp->mutex);
	}
	pthread_mutex_unlock (&pp->mutex);

	pthread_mutex_destroy (&pp->mutex);
	pthread_rwlock_destroy (&pp->rwmutex);
	pthread_cond_destroy (&pp->cond[0]);
	pthread_cond_destroy (&pp->cond[1]);
	pthread_cond_destroy (&pp->done_cond);
}

/* --------------------------------------- */

CLOSURE_DECL_BODY2 (ping_pong, ping_pong *, int)

/* void pthread_mutex_lock */
static void void_pthread_mutex_lock (void *mu) {
	pthread_mutex_lock ((pthread_mutex_t *) mu);
}

/* void pthread_mutex_unlock */
static void void_pthread_mutex_unlock (void *mu) {
	pthread_mutex_unlock ((pthread_mutex_t *) mu);
}

/* Run by each thread in benchmark_ping_pong_mutex_cv(). */
static void mutex_cv_ping_pong (ping_pong *pp, int parity) {
	pthread_mutex_lock (&pp->mutex);
	while (pp->i < pp->limit) {
		while ((pp->i & 1) == parity) {
			nsync_cv_wait_with_deadline_generic (&pp->cv[parity], &pp->mutex,
						             &void_pthread_mutex_lock,
						             &void_pthread_mutex_unlock,
						             nsync_time_no_deadline, NULL);
		}
		pp->i++;
		nsync_cv_signal (&pp->cv[1-parity]);
	}

	pthread_mutex_unlock (&pp->mutex);
	ping_pong_done (pp);
}

/* Measure the wakeup speed of pthread_mutex_t/nsync_cv used to ping-pong back and
   forth between two threads.  */
static void benchmark_ping_pong_mutex_cv (testing t) {
	ping_pong pp;
	ping_pong_init (&pp, testing_n (t));
	closure_fork (closure_ping_pong (&mutex_cv_ping_pong, &pp, 0));
	mutex_cv_ping_pong (&pp, 1);
	ping_pong_destroy (&pp);
}

/* --------------------------------------- */

/* Run by each thread in benchmark_ping_pong_mu_cv(). */
static void mu_cv_ping_pong (ping_pong *pp, int parity) {
	nsync_mu_lock (&pp->mu);
	while (pp->i < pp->limit) {
		while ((pp->i & 1) == parity) {
			nsync_cv_wait (&pp->cv[parity], &pp->mu);
		}
		pp->i++;
		nsync_cv_signal (&pp->cv[1-parity]);
	}
	nsync_mu_unlock (&pp->mu);
	ping_pong_done (pp);
}

/* Measure the wakeup speed of nsync_mu/nsync_cv used to
   ping-pong back and forth between two threads. */
static void benchmark_ping_pong_mu_cv (testing t) {
	ping_pong pp;
	ping_pong_init (&pp, testing_n (t));
	closure_fork (closure_ping_pong (&mu_cv_ping_pong, &pp, 0));
	mu_cv_ping_pong (&pp, 1);
	ping_pong_destroy (&pp);
}

/* --------------------------------------- */

/* Run by each thread in benchmark_ping_pong_mu_cv_unexpired_deadline(). */
static void mu_cv_unexpired_deadline_ping_pong (ping_pong *pp, int parity) {
	nsync_time deadline_in1hour;
	deadline_in1hour = nsync_time_add (nsync_time_now (), nsync_time_ms (3600000));
	nsync_mu_lock (&pp->mu);
	while (pp->i < pp->limit) {
		while ((pp->i & 1) == parity) {
			nsync_cv_wait_with_deadline (&pp->cv[parity], &pp->mu,
						     deadline_in1hour, NULL);
		}
		pp->i++;
		nsync_cv_signal (&pp->cv[1 - parity]);
	}
	nsync_mu_unlock (&pp->mu);
	ping_pong_done (pp);
}

/* Measure the wakeup speed of nsync_mu/nsync_cv used to ping-pong back and forth
   between two threads, with an unexpired deadline pending.  */
static void benchmark_ping_pong_mu_cv_unexpired_deadline (testing t) {
	ping_pong pp;
	ping_pong_init (&pp, testing_n (t));
	closure_fork (closure_ping_pong (&mu_cv_unexpired_deadline_ping_pong, &pp, 0));
	mu_cv_unexpired_deadline_ping_pong (&pp, 1);
	ping_pong_destroy (&pp);
}

/* --------------------------------------- */
/* even_ping_pong and odd_ping_pong are wait conditions used by mu_ping_pong. */
static int even_ping_pong (const void *v) {
	return ((((const ping_pong *) v)->i & 1) == 0);
}

static int odd_ping_pong (const void *v) {
	return ((((const ping_pong *) v)->i & 1) == 1);
}

typedef int (*condition_func) (const void *v);
static const condition_func condition[] = { &even_ping_pong, &odd_ping_pong };

/* Run by each thread in benchmark_ping_pong_mu_unexpired_deadline(). */
static void mu_unexpired_deadline_ping_pong (ping_pong *pp, int parity) {
	nsync_time deadline_in1hour;
	deadline_in1hour = nsync_time_add (nsync_time_now (), nsync_time_ms (3600000));
	nsync_mu_lock (&pp->mu);
	while (pp->i < pp->limit) {
		nsync_mu_wait_with_deadline (&pp->mu, condition[parity], pp, NULL,
					     deadline_in1hour, NULL);
		pp->i++;
	}
	nsync_mu_unlock (&pp->mu);
	ping_pong_done (pp);
}

/* Measure the wakeup speed of nsync_mu's wait_with_deadline() primitive used to
   ping-pong back and forth between two threads with an unexpired deadline
   pending.  */
static void benchmark_ping_pong_mu_unexpired_deadline (testing t) {
	ping_pong pp;
	ping_pong_init (&pp, testing_n (t));
	closure_fork (closure_ping_pong (&mu_unexpired_deadline_ping_pong, &pp, 0));
	mu_unexpired_deadline_ping_pong (&pp, 1);
	ping_pong_destroy (&pp);
}

/* --------------------------------------- */

/* Run by each thread in benchmark_ping_pong_mutex_cond_unexpired_deadline(). */
static void mutex_cond_unexpired_deadline_ping_pong (ping_pong *pp, int parity) {
	struct timespec ts;
	clock_gettime (CLOCK_REALTIME, &ts);
	ts.tv_sec += 3600;
	pthread_mutex_lock (&pp->mutex);
	while (pp->i < pp->limit) {
		while ((pp->i & 1) == parity) {
			pthread_cond_timedwait (&pp->cond[parity], &pp->mutex, &ts);
		}
		pp->i++;
		pthread_cond_signal (&pp->cond[1-parity]);
	}
	pthread_mutex_unlock (&pp->mutex);
	ping_pong_done (pp);
}

/* Measure the wakeup speed of pthread_mutex_t/pthread_cond_t used to ping-pong
   back and forth between two threads.  */
static void benchmark_ping_pong_mutex_cond_unexpired_deadline (testing t) {
	ping_pong pp;
	ping_pong_init (&pp, testing_n (t));
	closure_fork (closure_ping_pong (&mutex_cond_unexpired_deadline_ping_pong, &pp, 0));
	mutex_cond_unexpired_deadline_ping_pong (&pp, 1);
	ping_pong_destroy (&pp);
}

/* --------------------------------------- */

/* Run by each thread in benchmark_ping_pong_mutex_cond(). */
static void mutex_cond_ping_pong (ping_pong *pp, int parity) {
	pthread_mutex_lock (&pp->mutex);
	while (pp->i < pp->limit) {
		while ((pp->i & 1) == parity) {
			pthread_cond_wait (&pp->cond[parity], &pp->mutex);
		}
		pp->i++;
		pthread_cond_signal (&pp->cond[1-parity]);
	}
	pthread_mutex_unlock (&pp->mutex);
	ping_pong_done (pp);
}

/* Measure the wakeup speed of pthread_mutex_t/pthread_cond_t used to ping-pong
   back and forth between two threads.  */
static void benchmark_ping_pong_mutex_cond (testing t) {
	ping_pong pp;
	ping_pong_init (&pp, testing_n (t));
	closure_fork (closure_ping_pong (&mutex_cond_ping_pong, &pp, 0));
	mutex_cond_ping_pong (&pp, 1);
	ping_pong_destroy (&pp);
}

/* --------------------------------------- */

/* Run by each thread in benchmark_ping_pong_mu(). */
static void mu_ping_pong (ping_pong *pp, int parity) {
	nsync_mu_lock (&pp->mu);
	while (pp->i < pp->limit) {
		nsync_mu_wait (&pp->mu, condition[parity], pp, NULL);
		pp->i++;
	}
	nsync_mu_unlock (&pp->mu);
	ping_pong_done (pp);
}

/* Measure the wakeup speed of nsync_mu's conditional
   critical sections, used to ping-pong back and forth between two threads. */
static void benchmark_ping_pong_mu (testing t) {
	ping_pong pp;
	ping_pong_init (&pp, testing_n (t));
	closure_fork (closure_ping_pong (&mu_ping_pong, &pp, 0));
	mu_ping_pong (&pp, 1);
	ping_pong_destroy (&pp);
}

/* --------------------------------------- */

/* void pthread_rwlock_wrlock */
static void void_pthread_rwlock_wrlock (void *mu) {
	pthread_rwlock_wrlock ((pthread_rwlock_t *) mu);
}

/* void pthread_rwlock_unlock */
static void void_pthread_rwlock_unlock (void *mu) {
	pthread_rwlock_unlock ((pthread_rwlock_t *) mu);
}

/* Run by each thread in benchmark_ping_pong_rwmutex_cv(). */
static void rw_mutex_cv_ping_pong (ping_pong *pp, int parity) {
	pthread_rwlock_wrlock (&pp->rwmutex);
	while (pp->i < pp->limit) {
		while ((pp->i & 1) == parity) {
			nsync_cv_wait_with_deadline_generic (&pp->cv[parity], &pp->rwmutex,
						             &void_pthread_rwlock_wrlock,
						             &void_pthread_rwlock_unlock,
						             nsync_time_no_deadline, NULL);
		}
		pp->i++;
		nsync_cv_signal (&pp->cv[1-parity]);
	}
	pthread_rwlock_unlock (&pp->rwmutex);
	ping_pong_done (pp);
}

/* Measure the wakeup speed of pthread_rwlock_t/nsync_cv used to ping-pong back and
   forth between two threads.  */
static void benchmark_ping_pong_rwmutex_cv (testing t) {
	ping_pong pp;
	ping_pong_init (&pp, testing_n (t));
	closure_fork (closure_ping_pong (&rw_mutex_cv_ping_pong, &pp, 0));
	rw_mutex_cv_ping_pong (&pp, 1);
	ping_pong_destroy (&pp);
}

/* --------------------------------------- */

/* Run by each thread in benchmark_ping_pong_wait_n_cv(). */
static void wait_n_cv_ping_pong (ping_pong *pp, int parity) {
	struct nsync_waitable_s waitable;
	struct nsync_waitable_s *pwaitable = &waitable;
	waitable.v = &pp->cv[parity];
	waitable.funcs = &nsync_cv_waitable_funcs;
	nsync_mu_lock (&pp->mu);
	while (pp->i < pp->limit) {
		while ((pp->i & 1) == parity) {
			nsync_wait_n (&pp->mu, (void (*) (void *)) &nsync_mu_lock,
				      (void (*) (void *)) &nsync_mu_unlock,
				      nsync_time_no_deadline, 1, &pwaitable);
		}
		pp->i++;
		nsync_cv_signal (&pp->cv[1 - parity]);
	}
	nsync_mu_unlock (&pp->mu);
	ping_pong_done (pp);
}

/* Measure the wakeup speed of nsync_mu/nsync_cv using nsync_wait_n, used to
   ping-pong back and forth between two threads.  */
static void benchmark_ping_pong_wait_n_cv (testing t) {
	ping_pong pp;
	ping_pong_init (&pp, testing_n (t));
	closure_fork (closure_ping_pong (&wait_n_cv_ping_pong, &pp, 0));
	wait_n_cv_ping_pong (&pp, 1);
	ping_pong_destroy (&pp);
}

/* --------------------------------------- */

int main (int argc, char *argv[]) {
	testing_base tb = testing_new (argc, argv, 0);

	BENCHMARK_RUN (tb, benchmark_ping_pong_mu);
	BENCHMARK_RUN (tb, benchmark_ping_pong_mu_unexpired_deadline);
	BENCHMARK_RUN (tb, benchmark_ping_pong_mu_cv);
	BENCHMARK_RUN (tb, benchmark_ping_pong_mu_cv_unexpired_deadline);
	BENCHMARK_RUN (tb, benchmark_ping_pong_mutex_cond);
	BENCHMARK_RUN (tb, benchmark_ping_pong_mutex_cond_unexpired_deadline);
	BENCHMARK_RUN (tb, benchmark_ping_pong_mutex_cv);
	BENCHMARK_RUN (tb, benchmark_ping_pong_rwmutex_cv);
	BENCHMARK_RUN (tb, benchmark_ping_pong_wait_n_cv);

	return (testing_base_exit (tb));
}
