#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMESPEC_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMESPEC_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct timespec {
  int64_t tv_sec;
  int64_t tv_nsec; /* nanoseconds */
};

int futex(uint32_t *, int, int, const struct timespec *, uint32_t *);

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMESPEC_H_ */
