#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMEVAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMEVAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct timeval {
  int64_t tv_sec;
  int64_t tv_usec; /* microseconds */
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMEVAL_H_ */
