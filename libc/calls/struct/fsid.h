#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_FSID_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_FSID_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

typedef struct fsid_t {
  uint32_t __val[2];
} fsid_t;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_FSID_H_ */
