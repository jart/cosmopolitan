#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_RLIMIT_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_RLIMIT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct rlimit {
  uint64_t rlim_cur; /* current (soft) limit in bytes */
  uint64_t rlim_max; /* maximum limit in bytes */
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_RLIMIT_H_ */
