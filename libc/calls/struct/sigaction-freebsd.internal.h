#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGACTION_FREEBSD_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGACTION_FREEBSD_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct sigset_freebsd {
  uint32_t sig[4];
};

struct sigaction_freebsd {
  intptr_t sa_handler;
  uint32_t sa_flags;
  struct sigset_freebsd sa_mask;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGACTION_FREEBSD_H_ */
