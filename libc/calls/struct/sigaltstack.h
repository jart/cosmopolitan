#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGALTSTACK_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGALTSTACK_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct sigaltstack {
  void *ss_sp;
  int ss_flags;
  size_t ss_size;
};

typedef struct sigaltstack stack_t;

int sigaltstack(const struct sigaltstack *, struct sigaltstack *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGALTSTACK_H_ */
