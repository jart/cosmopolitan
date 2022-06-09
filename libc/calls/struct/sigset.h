#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGSET_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGSET_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

typedef struct sigset {
  uint64_t __bits[2];
} sigset_t;

int sigprocmask(int, const sigset_t *, sigset_t *);
int sigsuspend(const sigset_t *);

int sigaddset(sigset_t *, int) paramsnonnull();
int sigdelset(sigset_t *, int) paramsnonnull();
int sigemptyset(sigset_t *) paramsnonnull();
int sigfillset(sigset_t *) paramsnonnull();
int sigismember(const sigset_t *, int) paramsnonnull() nosideeffect;

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGSET_H_ */
