#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGSET_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGSET_H_
COSMOPOLITAN_C_START_

typedef uint64_t sigset_t;

/* clang-format off */
int sigaddset(sigset_t *, int) libcesque paramsnonnull();
int sigdelset(sigset_t *, int) libcesque paramsnonnull();
int sigemptyset(sigset_t *) libcesque paramsnonnull();
int sigfillset(sigset_t *) libcesque paramsnonnull();
int sigandset(sigset_t *, const sigset_t *, const sigset_t *) libcesque paramsnonnull();
int sigorset(sigset_t *, const sigset_t *, const sigset_t *) libcesque paramsnonnull();
int sigisemptyset(const sigset_t *) libcesque paramsnonnull() nosideeffect;
int sigismember(const sigset_t *, int) libcesque paramsnonnull() nosideeffect;
int sigcountset(const sigset_t *) libcesque paramsnonnull() nosideeffect;
int sigprocmask(int, const sigset_t *, sigset_t *) dontthrow;
int sigsuspend(const sigset_t *) dontthrow;
int sigpending(sigset_t *) libcesque;
int pthread_sigmask(int, const sigset_t *, sigset_t *) dontthrow;
/* clang-format on */

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGSET_H_ */
