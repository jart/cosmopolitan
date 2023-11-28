#ifndef COSMOPOLITAN_LIBC_SOCK_STRUCT_LINGER_H_
#define COSMOPOLITAN_LIBC_SOCK_STRUCT_LINGER_H_
COSMOPOLITAN_C_START_

struct linger {     /* Linux+XNU+BSD ABI */
  int32_t l_onoff;  /* on/off */
  int32_t l_linger; /* seconds */
};

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SOCK_STRUCT_LINGER_H_ */
