#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_LINGER_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_LINGER_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct linger_nt {
  uint16_t l_onoff;  /* on/off */
  uint16_t l_linger; /* seconds */
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_LINGER_H_ */
