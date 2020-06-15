#ifndef COSMOPOLITAN_LIBC_CALLS_TYPEDEF_SIGACTION_F_H_
#define COSMOPOLITAN_LIBC_CALLS_TYPEDEF_SIGACTION_F_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct siginfo;
struct ucontext;

typedef void (*sigaction_f)(int, struct siginfo *, struct ucontext *);

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_TYPEDEF_SIGACTION_F_H_ */
