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
#include "third_party/nsync/once.h"
#include "libc/str/str.h"
#include "libc/thread/thread.h"
#include "third_party/nsync/counter.h"
#include "third_party/nsync/mu.h"
#include "third_party/nsync/testing/closure.h"
#include "third_party/nsync/testing/smprintf.h"
#include "third_party/nsync/testing/testing.h"

/* This tests nsync_once */

/* Data structure for each test of nsync_once */
struct once_test_s {
        nsync_once once;        /* the nsync_once under test */
        int counter;            /* a counter that should be incremented once */
        nsync_counter done;     /* reaches 0 when all threads done */
        testing t;              /* the test handle */
};

/* Per-thread data structure */
struct once_test_thread_s {
        int id;                 /* thread id */
        struct once_test_s *s;  /* the per-test structure */
};

#define N 4  /* number of threads used per test */
static struct once_test_thread_s ott[N];   /* data structure per thread */
static nsync_mu ott_s_mu = NSYNC_MU_INIT;

/* Increment s->counter by a power of two chosen by the thread id.  Called
   via one of the nsync_run_once* calls.  */
static void once_arg_func (void *v) {
        struct once_test_thread_s *lott = (struct once_test_thread_s *) v;
        struct once_test_s *s;
	nsync_mu_lock (&ott_s_mu);
        s = lott->s;
	nsync_mu_unlock (&ott_s_mu);
        if (s->counter != 0) {
                TEST_ERROR (s->t, ("once_arg_func found counter!=0"));
        }
        s->counter += 1 << (2 * lott->id);
}

/* Call once_arg_func() on the first thread structure. */
static void once_func0 (void) {
        once_arg_func (&ott[0]);
}

/* Call once_arg_func() on the second thread structure. */
static void once_func1 (void) {
        once_arg_func (&ott[1]);
}

/* Pause for a short time, then use one of the nsync_run_once* calls on
   ott->s->once, chosen using the thread id.  This is the body of each test
   thread.  */
static void once_thread (struct once_test_thread_s *lott) {
        struct once_test_s *s;
	nsync_mu_lock (&ott_s_mu);
        s = lott->s;
	nsync_mu_unlock (&ott_s_mu);
        nsync_time_sleep (nsync_time_s_ns (0, 1 * 1000 * 1000));
        switch (lott->id & 3) {
        case 0:  nsync_run_once (&s->once, &once_func0); break;
        case 1:  nsync_run_once_spin (&s->once, &once_func1); break;
        case 2:  nsync_run_once_arg (&s->once, &once_arg_func, lott); break;
        case 3:  nsync_run_once_arg_spin (&s->once, &once_arg_func, lott); break;
        }
        nsync_counter_add (s->done, -1);
}

CLOSURE_DECL_BODY1 (once_thread, struct once_test_thread_s *)

/* Test the functionality of nsync_once */
static void test_once_run (testing t) {
        int i;
        int j;
	for (j = 0; j != N; j++) {
		ott[j].id = j;
	}
        for (i = 0; i != 250; i++) {
                struct once_test_s *s =
			(struct once_test_s *) malloc (sizeof (*s));
                bzero ((void *) s, sizeof (*s));
                s->counter = 0;
                s->done = nsync_counter_new (N);
                s->t = t;
                for (j = 0; j != N; j++) {
			nsync_mu_lock (&ott_s_mu);
                        ott[j].s = s;
			nsync_mu_unlock (&ott_s_mu);
		}
                for (j = 0; j != N; j++) {
                        closure_fork (closure_once_thread (&once_thread,
                                                           &ott[j]));
                }
                if (nsync_counter_wait (s->done,
                                        nsync_time_no_deadline) != 0) {
                        TEST_ERROR (t, ("s.done not decremented to 0"));
                }
                if (s->counter == 0) {
                        TEST_ERROR (t, ("s.counter wasn't incremented"));
                }
                /* The counter is expected to be a power of two, because each
                   counter is incremented only via a single nsync_once (so at
                   most one increment should occur) and always by a power of
                   two.  */
                if ((s->counter & (s->counter-1)) != 0) {
                        TEST_ERROR (t, ("s.counter incremented repeatedly: %x",
                                        s->counter));
                }
		nsync_counter_free (s->done);
		free (s);
        }
}

/* Do nothing. */
static void no_op (void) {
}

/* Measure the performance of repeated use of nsync_run_once. */
static void benchmark_nsync_once (testing t) {
	static nsync_once o = NSYNC_ONCE_INIT;
	int n = testing_n (t);
	int i;
	for (i = 0; i != n; i++) {
		nsync_run_once (&o, &no_op);
	}
}

/* Measure the performance of repeated use of pthread_once. */
static void benchmark_native_once (testing t) {
	static pthread_once_t o = PTHREAD_ONCE_INIT;
	int n = testing_n (t);
	int i;
	for (i = 0; i != n; i++) {
		pthread_once (&o, &no_op);
	}
}

int main (int argc, char *argv[]) {
        testing_base tb = testing_new (argc, argv, 0);
        TEST_RUN (tb, test_once_run);
	BENCHMARK_RUN (tb, benchmark_nsync_once);
	BENCHMARK_RUN (tb, benchmark_native_once);
        return (testing_base_exit (tb));
}
