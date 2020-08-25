#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGALTSTACK_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGALTSTACK_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct sigaltstack {
  void *ss_sp;
  int ss_flags;
  size_t ss_size;
};

typedef struct sigaltstack stack_t;

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGALTSTACK_H_ */
