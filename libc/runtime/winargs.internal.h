#ifndef COSMOPOLITAN_LIBC_RUNTIME_WINARGS_INTERNAL_H_
#define COSMOPOLITAN_LIBC_RUNTIME_WINARGS_INTERNAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct WinArgs {
  char *argv[4096];
  char *envp[4092];
  intptr_t auxv[2][2];
  char argblock[ARG_MAX / 2];
  char envblock[ARG_MAX / 2];
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_WINARGS_INTERNAL_H_ */
