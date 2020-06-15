#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_RLIMIT_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_RLIMIT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct rlimit {
  int64_t rlim_cur;
  int64_t rlim_max;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_RLIMIT_H_ */
