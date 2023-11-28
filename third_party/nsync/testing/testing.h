#ifndef NSYNC_TESTING_TESTING_H_
#define NSYNC_TESTING_TESTING_H_

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

#endif /*NSYNC_TESTING_TESTING_H_*/
