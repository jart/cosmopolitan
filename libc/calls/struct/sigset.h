#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGSET_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGSET_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct sigset {    /* cosmo abi (linux is uint64_t) */
  uint32_t sig[4]; /* ignore sig[2] and sig[3] (for freebsd) */
} aligned(8);

typedef struct sigset sigset_t;

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGSET_H_ */
