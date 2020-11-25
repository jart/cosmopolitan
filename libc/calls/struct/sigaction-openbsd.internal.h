#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGACTION_OPENBSD_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGACTION_OPENBSD_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct sigaction$openbsd {
  intptr_t sa_handler;
  struct sigset$openbsd {
    uint32_t sig[1];
  } sa_mask;
  int32_t sa_flags;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGACTION_OPENBSD_H_ */
