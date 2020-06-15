#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_FLOCK_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_FLOCK_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct flock {
  short l_type;
  short l_whence;
  int64_t l_start;
  int64_t l_len;
  int l_pid;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_FLOCK_H_ */
