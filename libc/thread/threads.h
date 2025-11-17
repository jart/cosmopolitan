#ifndef COSMOPOLITAN_LIBC_THREAD_THREADS_H_
#define COSMOPOLITAN_LIBC_THREAD_THREADS_H_
COSMOPOLITAN_C_START_

#if !defined(__cplusplus) && defined(__STDC_VERSION__) && \
    __STDC_VERSION__ >= 201112L && __STDC_VERSION__ <= 201710L
#define thread_local _Thread_local
#endif

#define ONCE_FLAG_INIT 0

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
typedef unsigned once_flag;
typedef unsigned tss_t;

int thrd_create(thrd_t *, thrd_start_t, void *);
int thrd_detach(thrd_t);
int thrd_equal(thrd_t, thrd_t);
int thrd_join(thrd_t, int *);
int tss_create(tss_t *, tss_dtor_t);
int tss_set(tss_t, void *);
thrd_t thrd_current(void);
void *tss_get(tss_t);
void call_once(once_flag *, void (*)(void));
void thrd_exit(int) wontreturn;
void thrd_yield(void);
void tss_delete(tss_t);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_THREAD_THREADS_H_ */
