#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGSET_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGSET_H_
COSMOPOLITAN_C_START_

typedef uint64_t sigset_t;

int sigaddset(sigset_t *, int) paramsnonnull();
int sigdelset(sigset_t *, int) paramsnonnull();
int sigemptyset(sigset_t *) paramsnonnull();
int sigfillset(sigset_t *) paramsnonnull();
int sigandset(sigset_t *, const sigset_t *, const sigset_t *) paramsnonnull();
int sigorset(sigset_t *, const sigset_t *, const sigset_t *) paramsnonnull();
int sigisemptyset(const sigset_t *) paramsnonnull() nosideeffect;
int sigismember(const sigset_t *, int) paramsnonnull() nosideeffect;
int sigcountset(const sigset_t *) paramsnonnull() nosideeffect;
int sigprocmask(int, const sigset_t *, sigset_t *);
int sigsuspend(const sigset_t *);
int sigpending(sigset_t *);
int pthread_sigmask(int, const sigset_t *, sigset_t *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGSET_H_ */
