#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMESPEC_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMESPEC_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct timespec {
  int64_t tv_sec;
  int64_t tv_nsec; /* nanoseconds */
};

int sys_futex(int *, int, int, const struct timespec *, int *);
bool _timespec_gt(struct timespec, struct timespec);
struct timespec _timespec_add(struct timespec, struct timespec);
struct timespec _timespec_sub(struct timespec, struct timespec);

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMESPEC_H_ */
