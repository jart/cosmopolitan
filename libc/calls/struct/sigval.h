#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGVAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGVAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

union sigval {
  int32_t sival_int;
  void *sival_ptr;
};

int sigqueue(int, int, const union sigval);

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGVAL_H_ */
