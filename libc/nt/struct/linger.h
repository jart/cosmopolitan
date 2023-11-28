#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_LINGER_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_LINGER_H_
COSMOPOLITAN_C_START_

struct linger_nt {
  uint16_t l_onoff;  /* on/off */
  uint16_t l_linger; /* seconds */
};

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_LINGER_H_ */
