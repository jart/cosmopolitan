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
#include "third_party/nsync/testing/testing.h"
#include "libc/calls/calls.h"
#include "libc/fmt/conv.h"
#include "libc/limits.h"
#include "libc/log/log.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/temp.h"
#include "libc/str/str.h"
#include "third_party/nsync/atomic.internal.h"
#include "third_party/nsync/common.internal.h"
#include "third_party/nsync/mu.h"
#include "third_party/nsync/mu_wait.h"
#include "third_party/nsync/races.internal.h"
#include "third_party/nsync/testing/atm_log.h"
#include "third_party/nsync/testing/closure.h"
#include "third_party/nsync/testing/smprintf.h"
#include "third_party/nsync/testing/time_extra.h"

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
	struct Dll *children; /* list of testing_s structs whose base is this testing_base_s */
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
	struct Dll siblings;       /* part of list of siblings */
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
	ShowCrashReports ();
	bzero ((void *) tb, sizeof (*tb));
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
	dll_remove (&tb->children, &t->siblings);
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
		bzero ((void *) t, sizeof (*t));
		dll_init (&t->siblings);
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
			dll_make_last (&tb->children, &t->siblings);
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
			dll_make_last (&tb->children, &t->siblings);
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

/* Abort after printing the nul-terminated string s[]. */
void testing_panic (const char *s) {
	nsync_atm_log_print_ ();
	nsync_panic_ (s);
}
