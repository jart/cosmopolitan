#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGACTION_FREEBSD_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGACTION_FREEBSD_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct sigaction$freebsd {
  intptr_t sa_handler;
  uint32_t sa_flags;
  struct sigset$freebsd {
    uint32_t sig[4];
  } sa_mask;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGACTION_FREEBSD_H_ */
