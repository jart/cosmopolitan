#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGACTION_NETBSD_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGACTION_NETBSD_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct sigset_netbsd {
  uint32_t sig[4];
};

struct sigaction_netbsd {
  intptr_t sa_handler;
  struct sigset_netbsd sa_mask;
  uint32_t sa_flags;
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGACTION_NETBSD_H_ */
