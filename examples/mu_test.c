/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│vi: set et ft=c ts=8 tw=8 fenc=utf-8                                       :vi│
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
#include "libc/calls/weirdtypes.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/fmt.h"
#include "libc/limits.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "libc/str/str.h"
#include "libc/thread/thread.h"
#include "third_party/nsync/atomic.internal.h"
#include "third_party/nsync/cv.h"
#include "third_party/nsync/dll.h"
#include "third_party/nsync/mu.h"
#include "third_party/nsync/mu_wait.h"
#include "third_party/nsync/races.internal.h"
#include "third_party/nsync/time.h"
// clang-format off

/**
 * @fileoverview *NSYNC Mutex Tests / Benchmarks.
 *
 *     make -j8 o//examples/mu_test.com
 *     o//examples/mu_test.com -b
 *
 */

typedef struct testing_base_s *testing_base;
typedef const struct testing_base_s *const_testing_base;
typedef struct testing_s *testing;

/* Return a newly initialized testing_base. */
testing_base testing_new (int argc, char *argv[], int flags);

/* Return the index of the first argument in argv[] not processed by testing_new() */
int testing_base_argn (testing_base tb);

/* exit() with the test's exit status */
int testing_base_exit (testing_base tb);

/* Stop and start the benchmark timer. */
void testing_stop_timer (testing t);
void testing_start_timer (testing t);

/* Return whether the machine appears to be a uniprocessor.
   Some tests get different results on uniprocessors, because
   the probability of certain interleavings of thread actions is
   greatly reduced. */
int testing_is_uniprocessor (testing t);

/* Given a testing_base, run f (t), where t has type testing.
   Output will be for a test. */
#define TEST_RUN(tb, f) testing_run_ ((tb), &f, #f, 0)

/* Given a testing_base, run f (t), where t has type testing.
   Output will be for a benchmark, which should iterate testing_n (t) times. */
#define BENCHMARK_RUN(tb, f) testing_run_ ((tb), &f, #f, 1)

/* Return the iteration count for a benchmark. */
int testing_n (testing t);

/* Output nul-terminated string msg[] to stderr, then abort(). */
void testing_panic (const char *msg);

/* Return a value below 0 if tests should run short, 0 if normal, and a value exceeding
   0  if tests should run long. */
int testing_longshort (testing t);

/* Return non-zero if the user requested verbose output. */
int testing_verbose (testing t);

/* Output a printf-formated log message associated with *t.
   Example:    TEST_LOG (t, ("wombat %d", some_int));
   The TEST_ERROR() and TEST_FATAL() forms of the call makr the test as failing.
   The TEST_FATAL() form causes other subtests not to run.  */
#define TEST_LOG(t, args) testing_error_ ((t), 0, __FILE__, __LINE__, smprintf args);
#define TEST_ERROR(t, args) testing_error_ ((t), 1, __FILE__, __LINE__, smprintf args);
#define TEST_FATAL(t, args) testing_error_ ((t), 2, __FILE__, __LINE__, smprintf args);

/* ---------------------------------------- */

/* internal details follow */

/* An internal routine used by TEST_RUN() and BENCHMARK_RUN(). */
void testing_run_ (testing_base tb, void (*f) (testing t), const char *name, int is_benchmark);

/* Output an error message msg, and record status. */
void testing_error_ (testing t, int status, const char *file, int line, char *msg);

////////////////////////////////////////////////////////////////////////////////
// time_extra.h

/* Return a malloced nul-terminated string representing time t, using
   "decimals" decimal places.  */
char *nsync_time_str (nsync_time t, int decimals);

/* Sleep until the specified time.  Returns 0 on success, and EINTR
   if the call was interrupted. */
int nsync_time_sleep_until (nsync_time abs_deadline);

/* Return t as a double. */
double nsync_time_to_dbl (nsync_time t);

/* Return a time corresponding to double d. */
nsync_time nsync_time_from_dbl (double d);

////////////////////////////////////////////////////////////////////////////////
// smprintf.h

char *smprintf (const char *fmt, ...);

////////////////////////////////////////////////////////////////////////////////
// smprintf.c

char *smprintf (const char *fmt, ...) {
	int m = strlen (fmt) * 2 + 1;
	char *buf = (char *) malloc (m);
	int didnt_fit;
	do {
		va_list ap;
		int x;
		va_start (ap, fmt);
		x = vsnprintf (buf, m, fmt, ap);
		va_end (ap);
		if (x >= m) {
			buf = (char *) realloc (buf, m = x+1);
			didnt_fit = 1;
		} else if (x < 0 || x == m-1) {
			buf = (char *) realloc (buf, m *= 2);
			didnt_fit = 1;
		} else {
			didnt_fit = 0;
		}
	} while (didnt_fit);
	return (buf);
}

////////////////////////////////////////////////////////////////////////////////
// time_extra.c

char *nsync_time_str (nsync_time t, int decimals) {
	static const struct {
		const char *suffix;
		double multiplier;
	} scale[] = {
		{ "ns", 1.0e-9, },
		{ "us", 1e-6, },
		{ "ms", 1e-3, },
		{ "s", 1.0, },
		{ "hr", 3600.0, },
	};
        double s = nsync_time_to_dbl (t);
	int i = 0;
	while (i + 1 != sizeof (scale) / sizeof (scale[0]) && scale[i + 1].multiplier <= s) {
		i++;
	}
	return (smprintf ("%.*f%s", decimals, s/scale[i].multiplier, scale[i].suffix));
}

int nsync_time_sleep_until (nsync_time abs_deadline) {
	int result = 0;
	nsync_time now;
	now = nsync_time_now ();
	if (nsync_time_cmp (abs_deadline, now) > 0) {
		nsync_time remaining;
		remaining = nsync_time_sleep (nsync_time_sub (abs_deadline, now));
		if (nsync_time_cmp (remaining, nsync_time_zero) > 0) {
			result = EINTR;
		}
	}
	return (result);
}

double nsync_time_to_dbl (nsync_time t) {
	return (((double) NSYNC_TIME_SEC (t)) + ((double) NSYNC_TIME_NSEC (t) * 1e-9));
}

nsync_time nsync_time_from_dbl (double d) {
	time_t s = (time_t) d;
	if (d < s) {
		s--;
	}
	return (nsync_time_s_ns (s, (unsigned) ((d - (double) s) * 1e9)));
}

////////////////////////////////////////////////////////////////////////////////
// start_thread.c

struct thd_args {
	void (*f) (void *);
	void *arg;
};

static void *body (void *v) {
	struct thd_args *args = (struct thd_args *) v;
	(*args->f) (args->arg);
	free (args);
	return (NULL);
}

void nsync_start_thread_ (void (*f) (void *), void *arg) {
	struct thd_args *args = (struct thd_args *) malloc (sizeof (*args));
	pthread_t t;
	args->f = f;
	args->arg = arg;
	pthread_create (&t, NULL, body, args);
	pthread_detach (t);
}

////////////////////////////////////////////////////////////////////////////////
// closure.h

/* A run-once, self-freeing closure. */
typedef struct closure_s {
	void (*f0) (void *);
} closure;

/* Run the closure *cl, and free it. */
void closure_run (closure *cl);

/* Fork a new thread running the closure *cl, which will be freed when the
   thread exits.  */
void closure_fork (closure *cl);

/* To create a closure, declare a closure constructor with the right function arguments.

   For functions taking no arguments, use
	CLOSURE_DECL_BODY0 (foo)
   to generate the static routine:
	static closure *closure_foo (void (*f) (void));
   that will return a closure for any function *f that takes no argument.

   For an 1-argument function, use
	CLOSURE_DECL_BODY1 (foo, type)
   to generate the static routine:
	static closure *closure_foo (void (*f) (type), type x);
   that will return a closure for any function taking a single argument of the
   specified type.

   For an 2-argument function, use
	CLOSURE_DECL_BODY2 (foo, type0, type1)
   to generate the static routine:
	static closure *closure_foo (void (*f) (type0, type1), type0 x0, type1 x1);
   that will return a closure for any function taking a "type0" argument, and
   a "type1" argument.

   And so on, up to 9 arguments.

   For example, to make closures out of qsort():

	// First, just once (per module) define:
	//      static closure *closure_qsort_args (
	//              void (*f) (void *, size_t, size_t, int (*)(const void *, const void *))
	//              void *x0, size_t x1, size_t x2, int (*x3)(const void *, const void *));
	// by writing:
	CLOSURE_DECL_BODY4 (qsort_args, void *, size_t, size_t, int (*)(const void *, const void *))

	// Second, for each closure to be created, write something like this:
	closure *cl = closure_qsort_args (array, n_elements, sizeof (array[0]), &elem_cmp);

	// Then to run (and free) each closure:
	closure_run (cl);
	// This is like calling
	//      qsort (array, n_elements, sizeof (array[0]), &elem_cmp);
	//      free (cl);
 */




/* ------------------------------------------------------------------ */
/* Internal macro details follow. */
#define CLOSURE_S0(x,e) e
#define CLOSURE_S1(x,e) x##0
#define CLOSURE_S2(x,e) x##0, x##1
#define CLOSURE_S3(x,e) x##0, x##1, x##2
#define CLOSURE_S4(x,e) x##0, x##1, x##2, x##3
#define CLOSURE_S5(x,e) x##0, x##1, x##2, x##3, x##4
#define CLOSURE_S6(x,e) x##0, x##1, x##2, x##3, x##4, x##5
#define CLOSURE_S7(x,e) x##0, x##1, x##2, x##3, x##4, x##5, x##6
#define CLOSURE_S8(x,e) x##0, x##1, x##2, x##3, x##4, x##5, x##6, x##7
#define CLOSURE_S9(x,e) x##0, x##1, x##2, x##3, x##4, x##5, x##6, x##7, x##8

#define CLOSURE_P0(x,y,p,s,t)
#define CLOSURE_P1(x,y,p,s,t) p x##0 y##0 t
#define CLOSURE_P2(x,y,p,s,t) p x##0 y##0 s x##1 y##1 t
#define CLOSURE_P3(x,y,p,s,t) p x##0 y##0 s x##1 y##1 s x##2 y##2 t
#define CLOSURE_P4(x,y,p,s,t) p x##0 y##0 s x##1 y##1 s x##2 y##2 s x##3 y##3 t
#define CLOSURE_P5(x,y,p,s,t) p x##0 y##0 s x##1 y##1 s x##2 y##2 s x##3 y##3 s x##4 y##4 t
#define CLOSURE_P6(x,y,p,s,t) p x##0 y##0 s x##1 y##1 s x##2 y##2 s x##3 y##3 s x##4 y##4 s \
				x##5 y##5 t
#define CLOSURE_P7(x,y,p,s,t) p x##0 y##0 s x##1 y##1 s x##2 y##2 s x##3 y##3 s x##4 y##4 s \
				x##5 y##5 s x##6 y##6 t
#define CLOSURE_P8(x,y,p,s,t) p x##0 y##0 s x##1 y##1 s x##2 y##2 s x##3 y##3 s x##4 y##4 s \
				x##5 y##5 s x##6 y##6 s x##7 y##7 t
#define CLOSURE_P9(x,y,p,s,t) p x##0 y##0 s x##1 y##1 s x##2 y##2 s x##3 y##3 s x##4 y##4 s \
				x##5 y##5 s x##6 y##6 s x##7 y##7 s x##8 y##8 t

#define CLOSURE_COMMA_ ,
#define CLOSURE_SEMI_ ;
#define CLOSURE_BLANK_

#define CLOSURE_DECL_BODY_N_(name, n) \
	struct closure_s_##name { \
		void (*f0) (void *); /* must be first; matches closure. */ \
		void (*f) (CLOSURE_S##n (closure_t_##name##_,void)); \
		CLOSURE_P##n (closure_t_##name##_, a, CLOSURE_BLANK_, \
			      CLOSURE_SEMI_, CLOSURE_SEMI_) \
	}; \
	static void closure_f0_##name (void *v) { \
		struct closure_s_##name *a = (struct closure_s_##name *) v; \
		(*a->f) (CLOSURE_S##n (a->a,CLOSURE_BLANK_)); \
		free (a); \
	} \
	static closure *closure_##name (void (*f) (CLOSURE_S##n (closure_t_##name##_,void)) \
					CLOSURE_P##n (closure_t_##name##_, a, CLOSURE_COMMA_, \
					CLOSURE_COMMA_, CLOSURE_BLANK_)) { \
		struct closure_s_##name *cl = (struct closure_s_##name *) malloc (sizeof (*cl)); \
		cl->f0 = &closure_f0_##name; \
		cl->f = f; \
		CLOSURE_P##n (cl->a, = a, CLOSURE_BLANK_, CLOSURE_SEMI_, CLOSURE_SEMI_) \
		return ((closure *) cl); \
	}


#define CLOSURE_DECL_BODY0(name) \
	CLOSURE_DECL_BODY_N_ (name, 0)
#define CLOSURE_DECL_BODY1(name, t0) \
	typedef t0 closure_t_##name##_0; \
	CLOSURE_DECL_BODY_N_ (name, 1)
#define CLOSURE_DECL_BODY2(name, t0, t1) \
	typedef t0 closure_t_##name##_0; \
	typedef t1 closure_t_##name##_1; \
	CLOSURE_DECL_BODY_N_ (name, 2)
#define CLOSURE_DECL_BODY3(name, t0, t1, t2) \
	typedef t0 closure_t_##name##_0; \
	typedef t1 closure_t_##name##_1; \
	typedef t2 closure_t_##name##_2; \
	CLOSURE_DECL_BODY_N_ (name, 3)
#define CLOSURE_DECL_BODY4(name, t0, t1, t2, t3) \
	typedef t0 closure_t_##name##_0; \
	typedef t1 closure_t_##name##_1; \
	typedef t2 closure_t_##name##_2; \
	typedef t3 closure_t_##name##_3; \
	CLOSURE_DECL_BODY_N_ (name, 4)
#define CLOSURE_DECL_BODY5(name, t0, t1, t2, t3, t4) \
	typedef t0 closure_t_##name##_0; \
	typedef t1 closure_t_##name##_1; \
	typedef t2 closure_t_##name##_2; \
	typedef t3 closure_t_##name##_3; \
	typedef t4 closure_t_##name##_4; \
	CLOSURE_DECL_BODY_N_ (name, 5)
#define CLOSURE_DECL_BODY6(name, t0, t1, t2, t3, t4, t5) \
	typedef t0 closure_t_##name##_0; \
	typedef t1 closure_t_##name##_1; \
	typedef t2 closure_t_##name##_2; \
	typedef t3 closure_t_##name##_3; \
	typedef t4 closure_t_##name##_4; \
	typedef t5 closure_t_##name##_5; \
	CLOSURE_DECL_BODY_N_ (name, 6)
#define CLOSURE_DECL_BODY7(name, t0, t1, t2, t3, t4, t5, t6) \
	typedef t0 closure_t_##name##_0; \
	typedef t1 closure_t_##name##_1; \
	typedef t2 closure_t_##name##_2; \
	typedef t3 closure_t_##name##_3; \
	typedef t4 closure_t_##name##_4; \
	typedef t5 closure_t_##name##_5; \
	typedef t6 closure_t_##name##_6; \
	CLOSURE_DECL_BODY_N_ (name, 7)
#define CLOSURE_DECL_BODY8(name, t0, t1, t2, t3, t4, t5, t6, t7) \
	typedef t0 closure_t_##name##_0; \
	typedef t1 closure_t_##name##_1; \
	typedef t2 closure_t_##name##_2; \
	typedef t3 closure_t_##name##_3; \
	typedef t4 closure_t_##name##_4; \
	typedef t5 closure_t_##name##_5; \
	typedef t6 closure_t_##name##_6; \
	typedef t7 closure_t_##name##_7; \
	CLOSURE_DECL_BODY_N_ (name, 8)
#define CLOSURE_DECL_BODY9(name, t0, t1, t2, t3, t4, t5, t6, t7, t8) \
	typedef t0 closure_t_##name##_0; \
	typedef t1 closure_t_##name##_1; \
	typedef t2 closure_t_##name##_2; \
	typedef t3 closure_t_##name##_3; \
	typedef t4 closure_t_##name##_4; \
	typedef t5 closure_t_##name##_5; \
	typedef t6 closure_t_##name##_6; \
	typedef t7 closure_t_##name##_7; \
	typedef t8 closure_t_##name##_8; \
	CLOSURE_DECL_BODY_N_ (name, 9)

////////////////////////////////////////////////////////////////////////////////
// closure.c

void nsync_start_thread_ (void (*f) (void *), void *arg);

/* Run the closure *cl. */
void closure_run (closure *cl) {
	(*cl->f0) (cl);
}

/* Run the closure (closure *), but wrapped to fix the type. */
static void closure_run_body (void *v) {
	closure_run ((closure *)v);
}

void closure_fork (closure *cl) {
	nsync_start_thread_ (&closure_run_body, cl);
}

////////////////////////////////////////////////////////////////////////////////
// atm_log.h

void nsync_atm_log_ (int c, void *p, uint32_t o, uint32_t n, const char *file, int line);
void nsync_atm_log_print_ (void);

////////////////////////////////////////////////////////////////////////////////
// atm_log.c

#ifndef NSYNC_ATM_LOG
#define NSYNC_ATM_LOG 0
#endif

struct atm_log {
	uintptr_t i;
	uintptr_t thd_id;
	uintptr_t c;
	void *p;
	uintptr_t o;
	uintptr_t n;
	const char *file;
	uintptr_t line;
};

#define LOG_N 14

static struct atm_log log_entries[1 << LOG_N];
static uint32_t log_i;

static pthread_mutex_t log_mu;

static pthread_key_t key;
static pthread_once_t once = PTHREAD_ONCE_INIT;
static void do_once (void) {
	pthread_mutex_init (&log_mu, NULL);
	pthread_key_create (&key, NULL);
}
static int thread_id;

void nsync_atm_log_ (int c, void *p, uint32_t o, uint32_t n, const char *file, int line) {
	if (NSYNC_ATM_LOG) {
		struct atm_log *e;
		uint32_t i;
		int *pthd_id;
		int thd_id;

		pthread_once (&once, &do_once);
		pthd_id = (int *) pthread_getspecific (key);

		pthread_mutex_lock (&log_mu);
		i = log_i++;
		if (pthd_id == NULL) {
			thd_id = thread_id++;
			pthd_id = (int *) malloc (sizeof (*pthd_id));
			pthread_setspecific (key, pthd_id);
			*pthd_id = thd_id;
		} else {
			thd_id = *pthd_id;
		}
		pthread_mutex_unlock (&log_mu);

		e = &log_entries[i & ((1 << LOG_N) - 1)];
		e->i = i;
		e->thd_id = thd_id;
		e->c = c;
		e->p = p;
		e->o = o;
		e->n = n;
		e->file = file;
		e->line = line;
	}
}

void nsync_atm_log_print_ (void) {
	if (NSYNC_ATM_LOG) {
		uint32_t i;
		pthread_once (&once, &do_once);
		pthread_mutex_lock (&log_mu);
		for (i = 0; i != (1 << LOG_N); i++) {
			struct atm_log *e = &log_entries[i];
			if (e->file != 0) {
				fprintf (stderr, "%6lx %3d %c  p %16p  o %8x  n %8x  %10s:%d\n",
					(unsigned long) e->i,
					(int) e->thd_id,
					e->c <= ' '? '?' : (char)e->c,
					e->p,
					(uint32_t) e->o,
					(uint32_t) e->n,
					e->file,
					(int) e->line);
			}
		}
		pthread_mutex_unlock (&log_mu);
	}
}

////////////////////////////////////////////////////////////////////////////////
// testing.c

struct testing_base_s {
	int flags;		/* flags from testing_new(); r/o after init */
	int parallelism;	/* max parallelism to use; r/o after init */
	FILE *fp;		/* where to send output; pointer is r/o after init */
	int argn;		/* first arg not processed by testing_new(); r/o after init */
	int argc;		/* argc passed to testing_new(); r/o after init */
	char **argv;		/* argv passed to testing_new(); r/o after init */
	char *prog;		/* name of programme, from argv[0] */
	int suppress_header;    /* supress hreader on benchmarks */
	int run_tests;		/* whether to run tests */
	int run_benchmarks;     /* whether to run benchmarks */
	int benchmarks_running; /* and benchmarks are now running */
	char *include_pat;      /* ,- or |-separated substrings of tests to include */
	char *exclude_pat;      /* ,- or |-separated substrings of tests to exclude */
	int longshort;		/* 0 normal, -1 short, 1 long */
	int verbose;		/* 0 normal; 1 verbose output */

	nsync_mu testing_mu;      /* protects fields below */
	int is_uniprocessor;      /* whether the system is a uniprocessor */
	nsync_dll_list_ children; /* list of testing_s structs whose base is this testing_base_s */
	int child_count;	  /* count of testing_s structs whose base is this testing_base_s */
	int exit_status;	  /* final exit status */
};

struct testing_s {
	struct testing_base_s *base;       /* r/o after init */
	int test_status;		   /* status; merged into common->exit_status */
	int n;				   /* benchmark iteration count */
	nsync_atomic_uint32_ partial_line; /* whether partial test banner emitted last*/
	FILE *fp;			   /* where to output; merged into common->fp if != to it */
	nsync_time start_time;	/* timer start time; for benchmarks */
	nsync_time stop_time;	 /* when the timer was stopped; for benchmarks */
	void (*f) (testing);		   /* test function to run */
	const char *name;		   /* name of test */
	nsync_dll_element_ siblings;       /* part of list of siblings */
};

/* Output the header for benchmarks. */
static void output_header (FILE *fp, const char *prog) {
	int i;
	int hdrlen = fprintf (fp, "%-10s%-40s %9s %8s  %8s %8s\n", "Benchmark", prog, "ops", "time",
			      "ops/sec", "time/op");
	for (i = 1; i < hdrlen; i++) {
		putc ('-', fp);
	}
	putc ('\n', fp);
	fflush (fp);
}

/* Process a single flag.  *pargn is main's argn */
static void process_flag (testing_base tb, int *pargn, int argc, char *argv[], int flag,
			  const char *arg) {
	switch (flag) {
	case 'b':
		tb->run_benchmarks = 1;
		break;
	case 'B':
		tb->run_benchmarks = 1;
		tb->run_tests = 0;
		break;
	case 'H':
		output_header (stdout, "");
		exit (0);
	case 'h':
		tb->suppress_header = 1;
		break;
	case 'l':
		tb->longshort++;
		break;
	case 'm':
		if (*pargn + 1 == argc) {
			fprintf (stderr, "%s: -m flag expects ,- or |-separated strings\n",
				 argv[0]);
			exit (2);
		}
		tb->include_pat = argv[++*pargn];
		break;
	case 'n':
		if (*pargn + 1 == argc || atoi (argv[1 + *pargn]) < 1) {
			fprintf (stderr, "%s: -n flag expects parallelism value >= 1\n", argv[0]);
			exit (2);
		}
		tb->parallelism = atoi (argv[++*pargn]);
		break;
	case 's':
		tb->longshort--;
		break;
	case 'v':
		tb->verbose = 1;
		break;
	case 'x':
		if (*pargn + 1 == argc) {
			fprintf (stderr, "%s: -x flag expects ,- or |-separated strings\n",
				 argv[0]);
			exit (2);
		}
		tb->exclude_pat = argv[++*pargn];
		break;
	default:
		fprintf (stderr, "%s: unrecognized flag '%c' in arg %d: \"%s\"\n", argv[0], flag,
			 *pargn, arg);
		exit (2);
	}
}

testing_base testing_new (int argc, char *argv[], int flags) {
	static const char sep[] = { '/', '\\' };
	int i;
	int argn;
	testing_base tb = (testing_base)malloc (sizeof (*tb));
	memset ((void *) tb, 0, sizeof (*tb));
	tb->flags = flags;
	tb->fp = stderr;
	tb->argc = argc;
	tb->argv = argv;
	tb->parallelism = 1;
	tb->run_tests = 1;
	tb->is_uniprocessor = -1;
	tb->prog = tb->argv[0];
	for (i = 0; i != sizeof (sep) / sizeof (sep[0]); i++) {
		char *last = strrchr (tb->prog, sep[i]);
		if (last != NULL) {
			tb->prog = last + 1;
		}
	}
	for (argn = 1; argn != argc && argv[argn][0] == '-' &&
		       strcmp (argv[argn], "--") != 0; argn++) {
		const char *arg = argv[argn];
		const char *f;
		for (f = &arg[1]; *f != 0; f++) {
			process_flag (tb, &argn, argc, argv, *f, arg);
		}
	}
	tb->argn = argn + (argn != argc && strcmp (argv[argn], "--") == 0);
	return (tb);
}

int testing_verbose (testing t) {
	return (t->base->verbose);
}

int testing_longshort (testing t) {
	return (t->base->longshort);
}

int testing_n (testing t) {
	return (t->n);
}

int testing_base_argn (testing_base tb) {
	return (tb->argn);
}

/* Return whether *(int *)v is zero.  Used with nsync_mu_wait().  */
static int int_is_zero (const void *v) {
	return (*(const int *)v == 0);
}

int testing_base_exit (testing_base tb) {
	int exit_status;
	nsync_mu_lock (&tb->testing_mu);
	nsync_mu_wait (&tb->testing_mu, &int_is_zero, &tb->child_count, NULL);
	exit_status = tb->exit_status;
	nsync_mu_unlock (&tb->testing_mu);
	free (tb);
	exit (exit_status);
	return (exit_status);
}

/* Cleanup code used after running either a test or a benchmark,
   called at the end of run_test() and run_benchmark(). */
static void finish_run (testing t) {
	testing_base tb = t->base;
	fflush (t->fp);
	nsync_mu_lock (&tb->testing_mu);
	if (t->fp != tb->fp) {
		int c;
		rewind (t->fp);
		while ((c = getc (t->fp)) != EOF) {
			putc (c, tb->fp);
		}
		fclose (t->fp);
		fflush (tb->fp);
	}
	if (tb->exit_status < t->test_status) {
		tb->exit_status = t->test_status;
	}
	tb->children = nsync_dll_remove_ (tb->children, &t->siblings);
	tb->child_count--;
	nsync_mu_unlock (&tb->testing_mu);
	free (t);
}

/* Run the test (*t->f)(t), and report on t->fp how long it took and its final
   status, which is set to non-zero if the test reported errors.  */
static void run_test (testing t) {
	testing_base tb = t->base;
	char *elapsed_str = 0;
	fprintf (t->fp, "%-25s %-45s  ", tb->prog, t->name);
	fflush (t->fp);
	ATM_STORE (&t->partial_line, 1);
	t->test_status = 0;
	t->n = 0;
	t->stop_time = nsync_time_zero;
	t->start_time = nsync_time_now ();
	(*t->f) (t);
	elapsed_str = nsync_time_str (nsync_time_sub (nsync_time_now (), t->start_time), 2);
	if (!ATM_LOAD (&t->partial_line)) {
		fprintf (t->fp, "%-25s %-45s  %s %8s\n", tb->prog, t->name,
		         t->test_status != 0? "failed": "passed", elapsed_str);
	} else {
		fprintf (t->fp, "%s %8s\n", t->test_status != 0? "failed": "passed", elapsed_str);
	}
	ATM_STORE (&t->partial_line, 0);
	fflush (t->fp);
	free (elapsed_str);
	finish_run (t);
}

/* Run the benchmark (*t->f)(t) repeatedly, specifying successively greater
   numbers of iterations, measuring how long it takes each time.  Eventually,
   it takes long enough to get a reasonable estimate of how long each iteration
   takes, which is reported on t->fp.  */
static void run_benchmark (testing t) {
	char *elapsed_str = 0;
	char *time_per_op_str = 0;
	double elapsed;
	int n = 1;
	double target = 2.0;
	int longshort = testing_longshort (t);
	if (longshort < 0) {
		target = 1e-3 * (2000 >> -longshort);
	} else if (longshort > 0) {
		target = 1e-3 * (2000 << longshort);
	}
	do {
		int32_t mul;
		t->test_status = 0;
		t->n = n;
		t->stop_time = nsync_time_zero;
		t->start_time = nsync_time_now ();
		(*t->f) (t);
		elapsed = nsync_time_to_dbl (nsync_time_sub (nsync_time_now (), t->start_time));
		if (elapsed < 1e-1) {
			elapsed = 1e-1;
		}
		mul = (int32_t) (target / elapsed);
		while (elapsed * mul * 4 < target * 5) {
			mul++;
		}
		if (mul > 1 && elapsed * mul * 2 < target * 3 && n < INT_MAX / mul) {
			n *= mul;
		} else if (n < INT_MAX / 2) {
			n *= 2;
		}
	} while (t->test_status == 0 && elapsed < target && n != t->n);
	elapsed_str = nsync_time_str (nsync_time_from_dbl (elapsed), 2);
	time_per_op_str = nsync_time_str (nsync_time_from_dbl (elapsed / t->n), 2);
	fprintf (t->fp, "%-50s %9d %8s  %8.2g %8s%s\n", t->name, t->n, elapsed_str,
		 ((double)t->n) / elapsed, time_per_op_str,
		 t->test_status != 0 ? "  *** failed ***" : "");
	free (elapsed_str);
	free (time_per_op_str);
	finish_run (t);
}

CLOSURE_DECL_BODY1 (testing, testing)

/* Return whether there's a "spare thread"; that is, whether the current count
   of child threads is less than the allowed parallelism.  */
static int spare_thread (const void *v) {
	const_testing_base tb = (const_testing_base) v;
	return (tb->child_count < tb->parallelism);
}

/* Return whether nul-terminated string str[] contains a string listed in
   comma-separated (or vertical bar-separted) strings in nul-terminated string
   pat[].  A dollar at the end of a string in pat[] matches the end of
   string in str[]. */
static int match (const char *pat, const char *str) {
	static const char seps[] = ",|";
	int found = 0;
	char Xbuf[128];
	int m = sizeof (Xbuf) - 1;
	char *mbuf = NULL;
	char *buf = Xbuf;
	int i = 0;
	while (!found && pat[i] != '\0') {
		int blen = strcspn (&pat[i], seps);
		int e = i + blen;
		if (blen > m) {
			m = blen + 128;
			buf = mbuf = (char *) realloc (mbuf, m + 1);
		}
		memcpy (buf, &pat[i], blen);
		buf[blen] = '\0';
		if (blen > 0 && buf[blen - 1] == '$') {
			int slen = strlen (str);
			buf[--blen] = 0;
			found = (slen >= blen &&
				 strcmp (&str[slen-blen], buf) == 0);
		} else {
			found = (strstr (str, buf) != NULL);
		}
		i = e + strspn (&pat[e], seps);
	}
	free (mbuf);
	return (found);
}

void testing_run_ (testing_base tb, void (*f) (testing t), const char *name, int is_benchmark) {
	int exit_status;
	nsync_mu_lock (&tb->testing_mu);
	exit_status = tb->exit_status;
	nsync_mu_unlock (&tb->testing_mu);
	if (exit_status < 2 &&
	    (!is_benchmark || tb->run_benchmarks) &&
	    (is_benchmark || tb->run_tests) &&
	    (tb->include_pat == NULL || match (tb->include_pat, name)) &&
	    (tb->exclude_pat == NULL || !match (tb->exclude_pat, name))) {
		testing t = (testing) malloc (sizeof (*t));
		memset ((void *) t, 0, sizeof (*t));
		nsync_dll_init_ (&t->siblings, t);
		t->base = tb;
		t->f = f;
		t->name = name;
		if (tb->parallelism == 1) {
			t->fp = tb->fp;
		} else {
			t->fp = tmpfile ();
		}
		if (!is_benchmark) {
			if (tb->benchmarks_running) {
				nsync_mu_lock (&tb->testing_mu);
				nsync_mu_wait (&tb->testing_mu, &int_is_zero, &tb->child_count, NULL);
				nsync_mu_unlock (&tb->testing_mu);
				tb->benchmarks_running = 0;
			}
			nsync_mu_lock (&tb->testing_mu);
			nsync_mu_wait (&tb->testing_mu, &spare_thread, tb, NULL);
			tb->child_count++;
			tb->children = nsync_dll_make_last_in_list_ (tb->children, &t->siblings);
			nsync_mu_unlock (&tb->testing_mu);
			closure_fork (closure_testing (&run_test, t));
		} else {
			if (!tb->benchmarks_running) {
				nsync_mu_lock (&tb->testing_mu);
				nsync_mu_wait (&tb->testing_mu, &int_is_zero, &tb->child_count, NULL);
				nsync_mu_unlock (&tb->testing_mu);
				if (!tb->suppress_header) {
					output_header (tb->fp, tb->prog);
				}
				tb->benchmarks_running = 1;
			}
			nsync_mu_lock (&tb->testing_mu);
			nsync_mu_wait (&tb->testing_mu, &spare_thread, tb, NULL);
			tb->child_count++;
			tb->children = nsync_dll_make_last_in_list_ (tb->children, &t->siblings);
			nsync_mu_unlock (&tb->testing_mu);
			closure_fork (closure_testing (&run_benchmark, t));
		}
	}
}

/* Used to decide whether the test is running on a uniprocessor. */
struct is_uniprocessor_s {
	double count;		   /* count of iterations while *state==1 */
	nsync_atomic_uint32_ done; /* set to 1 when thread finishes */
	char dummy[256];	   /* so structs don't share cache line */
};

/* An anciliary thread that waits until *state is 1, then increments s->count
   while *state stays 1, and then sets s->done to 1 before exiting.  */
static void uniprocessor_check (nsync_atomic_uint32_ *state, struct is_uniprocessor_s *s) {
	IGNORE_RACES_START ();
	while (ATM_LOAD_ACQ (state) != 1) {
	}
	while (ATM_LOAD_ACQ (state) == 1) {
		s->count++;
	}
	ATM_STORE_REL (&s->done, 1);
	IGNORE_RACES_END ();
}

CLOSURE_DECL_BODY2 (uniprocessor_check, nsync_atomic_uint32_ *, struct is_uniprocessor_s *)

/* Return whether the test is running on a uniprocessor.

   Some of the tests rely on interleaving of actions between threads.
   Particular interleavings are much less likely on uniprocessors, so the tests
   do not run, or do not declare an error if the system is a uniprocessor.

   Operating systems vary significantly in how one may ask how many procerssors
   are present, so we use a heuristic based on comparing the timing of a single
   thread, and two concurrent threads.  */
int testing_is_uniprocessor (testing t) {
	int is_uniprocessor;
	nsync_mu_lock (&t->base->testing_mu);
	is_uniprocessor = t->base->is_uniprocessor;
	if (is_uniprocessor == -1) {
		int i;
		struct is_uniprocessor_s s[3];
		nsync_atomic_uint32_ state;
		for (i = 0; i != 3; i++) {
			s[i].count = 0.0;
			s[i].done = 0;
		}

		ATM_STORE_REL (&state, 0);
		closure_fork (closure_uniprocessor_check (&uniprocessor_check, &state, &s[0]));
		nsync_time_sleep (nsync_time_ms (100));
		ATM_STORE_REL (&state, 1);
		nsync_time_sleep (nsync_time_ms (400));
		ATM_STORE_REL (&state, 2);
		while (!ATM_LOAD_ACQ (&s[0].done)) {
		}

		ATM_STORE_REL (&state, 0);
		closure_fork (closure_uniprocessor_check (&uniprocessor_check, &state, &s[1]));
		closure_fork (closure_uniprocessor_check (&uniprocessor_check, &state, &s[2]));
		nsync_time_sleep (nsync_time_ms (100));
		ATM_STORE_REL (&state, 1);
		nsync_time_sleep (nsync_time_ms (400));
		ATM_STORE_REL (&state, 2);
		while (!ATM_LOAD_ACQ (&s[1].done) || !ATM_LOAD_ACQ (&s[2].done)) {
		}
                t->base->is_uniprocessor = ((s[1].count + s[2].count) / s[0].count) < 1.7;
		is_uniprocessor = t->base->is_uniprocessor;
	}
	nsync_mu_unlock (&t->base->testing_mu);
	return (is_uniprocessor);
}

void testing_stop_timer (testing t) {
	if (nsync_time_cmp (t->stop_time, nsync_time_zero) != 0) {
		abort ();
	}
	t->stop_time = nsync_time_now ();
}

void testing_start_timer (testing t) {
	if (nsync_time_cmp (t->stop_time, nsync_time_zero) == 0) {
		abort ();
	}
	t->start_time = nsync_time_add (t->start_time,
		nsync_time_sub (nsync_time_now (), t->stop_time));
	t->stop_time = nsync_time_zero;
}

void testing_error_ (testing t, int test_status, const char *file, int line, char *msg) {
	int len = strlen (msg);
	int addnl = (len == 0 || msg[len - 1] != '\n');
	if (t->test_status < test_status) {
		t->test_status = test_status;
	}
	if (ATM_LOAD (&t->partial_line)) {
		ATM_STORE (&t->partial_line, 0);
		fprintf (t->fp, "\n%s: %s:%d: %s%s",
		         test_status == 2? "fatal": test_status == 1? "error": "info", file, line, msg,
		         addnl? "\n": "");
	} else {
		fprintf (t->fp, "%s: %s:%d: %s%s",
		         test_status == 2? "fatal": test_status == 1? "error": "info", file, line, msg,
		         addnl? "\n": "");
	}
	free (msg);
}

void nsync_panic_(const char *);

/* Abort after printing the nul-terminated string s[]. */
void testing_panic (const char *s) {
	nsync_atm_log_print_ ();
	nsync_panic_ (s);
}

////////////////////////////////////////////////////////////////////////////////
// mu_test.c

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
		memset ((void *) &td, 0, sizeof (td));
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
		memset ((void *) &td, 0, sizeof (td));
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
		memset ((void *) &td, 0, sizeof (td));
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
			sched_yield ();
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
		memset ((void *) &td, 0, sizeof (td));
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
	memset ((void *) c, 0, sizeof (*c));
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
	memset ((void *) &cs, 0, sizeof (cs));
	contended_state_run_test (&cs, t, &cs.mu, (void (*) (void*))&nsync_mu_lock,
				  (void (*) (void*))&nsync_mu_unlock);
}

/* Measure the performance of highly contended
   pthread_mutex_t locks, with small critical sections.  */
static void benchmark_mutex_contended (testing t) {
	contended_state cs;
	memset ((void *) &cs, 0, sizeof (cs));
	pthread_mutex_init (&cs.mutex, NULL);
	contended_state_run_test (&cs, t, &cs.mutex, &void_pthread_mutex_lock,
				  &void_pthread_mutex_unlock);
	pthread_mutex_destroy (&cs.mutex);
}

/* Measure the performance of highly contended
   pthread_rwlock_t locks, with small critical sections.  */
static void benchmark_wmutex_contended (testing t) {
	contended_state cs;
	memset ((void *) &cs, 0, sizeof (cs));
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
