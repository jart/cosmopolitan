#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGSET_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGSET_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

typedef struct sigset {
  uint64_t __bits[2];
} sigset_t;

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGSET_H_ */
