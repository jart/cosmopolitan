#ifndef COSMOPOLITAN_LIBC_SOCK_STRUCT_ARPHDR_H_
#define COSMOPOLITAN_LIBC_SOCK_STRUCT_ARPHDR_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct arphdr {
  uint16_t ar_hrd;
  uint16_t ar_pro;
  uint8_t ar_hln;
  uint8_t ar_pln;
  uint16_t ar_op;
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SOCK_STRUCT_ARPHDR_H_ */
