#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGACTION_LINUX_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGACTION_LINUX_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct sigaction$linux {
  intptr_t sa_handler;
  uint64_t sa_flags;
  void (*sa_restorer)(void);
  struct sigset$linux {
    uint32_t sig[2];
  } sa_mask;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGACTION_LINUX_H_ */
