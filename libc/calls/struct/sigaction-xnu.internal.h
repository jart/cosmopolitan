#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGACTION_XNU_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGACTION_XNU_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct __darwin_ucontext;
struct __darwin_siginfo;

struct sigset_xnu {
  uint32_t sig[1];
};

struct sigaction_xnu_in {
  intptr_t sa_handler;
  void (*sa_restorer)(void *, int, int, const struct __darwin_siginfo *,
                      const struct __darwin_ucontext *);
  struct sigset_xnu sa_mask;
  int32_t sa_flags;
};

struct sigaction_xnu_out {
  intptr_t sa_handler;
  struct sigset_xnu sa_mask;
  int32_t sa_flags;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGACTION_XNU_H_ */
