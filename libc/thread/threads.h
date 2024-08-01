#ifndef COSMOPOLITAN_LIBC_THREAD_THREADS_H_
#define COSMOPOLITAN_LIBC_THREAD_THREADS_H_
COSMOPOLITAN_C_START_

#if !defined(__cplusplus) &&                   \
    (!(defined(__GNUC__) && __GNUC__ >= 13) || \
     !(defined(__STDC_VERSION__) && __STDC_VERSION__ > 201710L))
#define thread_local _Thread_local
#endif

#define TSS_DTOR_ITERATIONS 4

enum {
  thrd_success = 0,
  thrd_busy = 1,
  thrd_error = 2,
  thrd_nomem = 3,
  thrd_timedout = 4,
};

enum {
  mtx_plain = 0,
  mtx_recursive = 1,
  mtx_timed = 2,
};

typedef uintptr_t thrd_t;
typedef void (*tss_dtor_t)(void *);
typedef int (*thrd_start_t)(void *);
typedef uint32_t once_flag;

void call_once(once_flag *, void (*)(void));
int thrd_create(thrd_t *, thrd_start_t, void *);
void thrd_exit(int) wontreturn;
int thrd_join(thrd_t, int *);
int thrd_detach(thrd_t);
int thrd_equal(thrd_t, thrd_t);
thrd_t thrd_current(void);
void thrd_yield(void);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_THREAD_THREADS_H_ */
